/*************************************************************************
 * This file is part of obs-wsc
 * github.con/univrsal/obs-wsc
 * Copyright 2020 univrsal <universailp@web.de>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#include "obs_wsc.h"
#include "opaque.h"
#include "send.h"
#include "util.h"
#include "external/bmem.h"
#include "external/platform.h"
#include "external/threading.h"
#include "external/dstr.h"
#include "external/sha256.h"
#include "external/base64.h"
#include <stdlib.h>
#include <time.h>

typedef struct obs_wsc_connection_s obs_wsc_connection_t;

void obs_wsc_init()
{
    /* rand() is only used for generating message ids */
    time_t t;
    srand((unsigned)time(&t));
}

long obs_wsc_shutdown()
{
    if (bnum_allocs() > 0)
        bwarn("Number of memory leaks: %li", bnum_allocs());
    return bnum_allocs();
}

void obs_wsc_set_allocator(struct base_allocator *defs)
{
    base_set_allocator(defs);
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
    obs_wsc_connection_t *conn = nc->user_data;
    int status;
    uint64_t time;
    struct http_message *msg;
    struct websocket_message *wm;

    pthread_mutex_lock(&conn->poll_mutex);

    switch (ev) {
    case MG_EV_CONNECT:
        status = *((int *)ev_data);
        if (status != 0) {
            berr("Connection for %s failed with code %i", conn->domain, status);
        }
        break;
    case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
        msg = (struct http_message *)ev_data;
        if (msg->resp_code == 101) {
            binfo("Connected to %s", conn->domain);
            conn->connected = true;
        }
        break;
    case MG_EV_CLOSE:
        if (conn->connected)
            binfo("Disconnected from %s", conn->domain);

        conn->connected = false;
        conn->thread_flag = false;
        break;
    case MG_EV_WEBSOCKET_FRAME:
        wm = ev_data;
        time = os_get_time_ns();
        binfo("Received message at %lu: %.*s", time, (int)wm->size, wm->data);

        json_decref(conn->last_message.data);
        conn->last_message.data = recv_json(wm->data, wm->size);
        conn->last_message.time = time;
        break;
    }

    pthread_mutex_unlock(&conn->poll_mutex);
}

static void *wsc_thread(void *arg)
{
    obs_wsc_connection_t *conn = arg;

    while (conn->thread_flag) {
        mg_mgr_poll(&conn->manager, conn->timeout - 20);
        os_sleep_ms(20);
    }

    return NULL;
}

obs_wsc_connection_t *obs_wsc_connect(const char *addr)
{
    static const char *local_host = "ws://127.0.0.1:4444";

    if (!addr)
        addr = local_host;

    obs_wsc_connection_t *n = bzalloc(sizeof(obs_wsc_connection_t));
    n->timeout = 100;

    mg_mgr_init(&n->manager, n);
    n->connection = mg_connect_ws(&n->manager, ev_handler, addr, "websocket", NULL);

    if (!n->connection) {
        berr("mg_connect_ws faild for %s", addr);
        goto fail;
    }

    pthread_mutex_init_value(&n->poll_mutex);
    if (pthread_mutex_init(&n->poll_mutex, NULL)) {
        berr("Couldn't initialize poll mutex");
        goto fail;
    }

    n->thread_flag = true;
    n->connection->user_data = n;
    n->domain = bstrdup(addr);

    if (pthread_create(&n->poll_thread, NULL, wsc_thread, n)) {
        berr("Couldn't initialize poll thread");
        goto fail;
    }

    /* Wait for connection */
    uint32_t wait = 0;
    while (!n->connected && wait < 1000) {
        wait += 10;
        os_sleep_ms(10);
    }

    if (wait >= 1000) {
        berr("Timed out while waiting for handshake response");
        goto fail;
    }

    return n;

fail:
    obs_wsc_disconnect(n);
    return NULL;
}

void obs_wcs_set_timeout(obs_wsc_connection_t *conn, int32_t ms)
{
    if (conn) {
        pthread_mutex_lock(&conn->poll_mutex);
        conn->timeout = ms;
        pthread_mutex_unlock(&conn->poll_mutex);
    }
}

void obs_wsc_disconnect(obs_wsc_connection_t *conn)
{
    if (!conn)
        return;

    for (size_t i = 0; i < conn->message_ids_len; i++)
        bfree(conn->message_ids[i]);
    bfree(conn->message_ids);

    conn->thread_flag = false;

    if (pthread_join(conn->poll_thread, NULL))
        berr("Failed to join poll thread for %s", conn->domain);

    mg_mgr_free(&conn->manager);
    pthread_mutex_destroy(&conn->poll_mutex);
    if (conn->domain)
        bfree(conn->domain);
    json_decref(conn->last_message.data);
    bfree(conn);
}

bool obs_wsc_auth_required(obs_wsc_connection_t *conn, obs_wsc_auth_data_t *auth)
{
    if (!conn || !conn->connection || !auth)
        return false;

    bool result = false;
    if (send_request(conn, "GetAuthRequired", NULL)) {
        pthread_mutex_lock(&conn->poll_mutex);
        json_t *response = json_copy(conn->last_message.data);
        char *last_msg_id = bstrdup(conn->last_message.message_id);
        pthread_mutex_unlock(&conn->poll_mutex);

        json_error_t err;
        char *challenge = NULL, *salt = NULL;

        if (parse_basic_json(response, last_msg_id) &&
            json_unpack_ex(response, &err, 0, "{sb}", "authRequired", &auth->required) == 0) {

            if (auth->required) {
                if (json_unpack_ex(response, &err, 0, "{ss,ss}", "challenge", &challenge, "salt", &salt) == 0) {
                    auth->challenge = bstrdup(challenge);
                    auth->salt = bstrdup(salt);
                    result = true;
                } else {
                    berr("Couldn't unpack auth json: %s at line %i", err.text, err.line);
                }
            } else {
                result = true;
            }
        } else {
            berr("Error while unpacking json: %s at line %i", err.text, err.line);
        }

        json_decref(response);
        bfree(last_msg_id);
    }
    return result;
}

void obs_wsc_free_auth_data(obs_wsc_auth_data_t *data)
{
    if (data) {
        bfree(data->salt);
        bfree(data->challenge);
    }
}

EXPORT bool obs_wsc_prepare_auth(obs_wsc_auth_data_t *auth, const char *password)
{
    struct dstr pw_salt, pw_salt_hash, pw_salt_hash_b64, final;
    dstr_init(&pw_salt);
    dstr_init(&pw_salt_hash);
    dstr_init(&pw_salt_hash_b64);
    dstr_init(&final);
    BYTE sha256[SHA256_BLOCK_SIZE];

    /* Step 1: Concatenate password with salt */
    dstr_copy(&pw_salt, password);
    dstr_cat(&pw_salt, auth->salt);

    /* Step 2: Hash it */
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (BYTE *)pw_salt.array, pw_salt.len);
    sha256_final(&ctx, sha256);
    dstr_from_sha256(&pw_salt_hash, sha256);

    /* Step 3: base64 encode it */
    size_t len = base64_encode((BYTE *)pw_salt_hash.array, NULL, pw_salt_hash.len, 1);
    dstr_resize(&pw_salt_hash_b64, len);
    base64_encode((BYTE *)pw_salt_hash.array, (BYTE *)pw_salt_hash_b64.array, pw_salt_hash.len, 1);

    /* Step 4: Concatenate it with the challenge */
    dstr_cat(&pw_salt_hash_b64, auth->challenge);

    /* Step 5: Hash it again */
    sha256_init(&ctx);
    sha256_update(&ctx, (BYTE *)pw_salt_hash_b64.array, pw_salt_hash_b64.len);
    sha256_final(&ctx, sha256);
    dstr_free(&pw_salt_hash);
    dstr_from_sha256(&pw_salt_hash, sha256);

    /* Step 6: base64 encode it again */
    len = base64_encode((BYTE *)pw_salt_hash.array, NULL, pw_salt_hash.len, 0);
    dstr_reserve(&final, len);
    base64_encode((BYTE *)pw_salt_hash.array, (BYTE *)final.array, pw_salt_hash.len, 1);

    auth->auth_response = bstrdup(final.array);

    /* Clean up */
    dstr_free(&pw_salt);
    dstr_free(&pw_salt_hash);
    dstr_free(&pw_salt_hash_b64);
    dstr_free(&final);
    bool result = strlen(auth->auth_response);
    if (result) {
        bfree(auth->salt);
        bfree(auth->challenge);
        auth->salt = NULL;
        auth->challenge = NULL;
    }
    return result;
}

bool obs_wsc_authenticate(obs_wsc_connection_t *conn, obs_wsc_auth_data_t *auth)
{
    if (!conn || !conn->connection || !auth || !auth->auth_response)
        return false;
    bool result = false;
    json_error_t err;
    json_t *auth_response = json_pack_ex(&err, 0, "{ss}", "auth", auth->auth_response);

    if (!auth_response) {
        berr("Error packing auth response json: %s at line %i", err.text, err.line);
    } else if (send_request(conn, "Authenticate", auth_response)) {
        pthread_mutex_lock(&conn->poll_mutex);
        json_t *response = json_copy(conn->last_message.data);
        char *last_msg_id = bstrdup(conn->last_message.message_id);
        pthread_mutex_unlock(&conn->poll_mutex);

        result = parse_basic_json(response, last_msg_id);
        json_decref(response);
        if (result) {
            bfree(auth->auth_response);
            auth->auth_response = NULL;
        }
    }
    return result;
}
