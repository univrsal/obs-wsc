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

void obs_wsc_init()
{
    /* rand() is only used for generating message ids */
    time_t t;
    srand((unsigned)time(&t));
    json_set_alloc_funcs(bzalloc, bfree);
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

void obs_wsc_get_logger(log_handler_t *handler, void **param)
{
    base_get_log_handler(handler, param);
}

void obs_wsc_set_logger(log_handler_t handler, void *param)
{
    base_set_log_handler(handler, param);
}

void obs_wsc_set_crash_handler(void (*handler)(const char *, va_list, void *), void *param)
{
    base_set_crash_handler(handler, param);
}

void notify_request(obs_wsc_connection_t *con, json_t *j)
{
    request_t *f = con->first_active_request;
    char *msg_id = NULL;

    if (json_unpack(j, "{ss}", "message-id", &msg_id)) {
        berr("Couldn't get message id form response json");
        return;
    }

    while (f) {
        if (strcmp(f->id, msg_id) == 0) {
            f->status = f->cb(j, f->cb_data);
            break;
        }
        f = f->next;
    }
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
    static json_t *data = NULL;
    obs_wsc_connection_t *conn = nc->user_data;
    int status;
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
        bdebug("Received message: %.*s", (int)wm->size, wm->data);
        data = recv_json(wm->data, wm->size);
        notify_request(conn, data);
        json_decref(data);
        break;
    }

    pthread_mutex_unlock(&conn->poll_mutex);
}

static void *wsc_thread(void *arg)
{
    obs_wsc_connection_t *conn = arg;

    while (conn->thread_flag) {
        mg_mgr_poll(&conn->manager, conn->poll_time);
        os_sleep_ms(10);
    }

    return NULL;
}

obs_wsc_connection_t *obs_wsc_connect(const char *addr)
{
    static const char *local_host = "ws://127.0.0.1:4444";

    if (!addr)
        addr = local_host;

    obs_wsc_connection_t *n = bzalloc(sizeof(obs_wsc_connection_t));
    n->timeout = 2000;
    n->poll_time = 100;

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
        if (!n->thread_flag) {
            berr("Thread exited before handshake completed");
            break;
        }
        wait += 10;
        os_sleep_ms(10);
    }

    if (wait >= 1000 || !n->thread_flag) {
        berr("Timed out while waiting for handshake response");
        goto fail;
    }

    darray_init(&n->ids.da);

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

void obs_wsc_set_poll_time(obs_wsc_connection_t *conn, int32_t ms)
{
    if (conn) {
        pthread_mutex_lock(&conn->poll_mutex);
        conn->poll_time = max(ms, 10);
        pthread_mutex_unlock(&conn->poll_mutex);
    }
}

void obs_wsc_disconnect(obs_wsc_connection_t *conn)
{
    if (!conn)
        return;

    conn->thread_flag = false;

    if (pthread_join(conn->poll_thread, NULL))
        berr("Failed to join poll thread for %s", conn->domain);

    mg_mgr_free(&conn->manager);
    pthread_mutex_destroy(&conn->poll_mutex);

    if (conn->domain)
        bfree(conn->domain);

    for (size_t i = 0; i < conn->ids.num; i++)
        bfree(conn->ids.array[i]);
    darray_free(&conn->ids.da);

    request_t *f = conn->first_active_request;
    while (f) {
        request_t *n = f->next;
        bfree(f);
        f = n;
    }

    bfree(conn);
}

void obs_wsc_free_auth_data(obs_wsc_auth_data_t *data)
{
    if (data) {
        bfree(data->salt);
        bfree(data->challenge);
        bfree(data->auth_response);
        data->salt = NULL;
        data->challenge = NULL;
        data->auth_response = NULL;
    }
}

bool obs_wsc_prepare_auth(obs_wsc_auth_data_t *auth, const char *password)
{
    if (!auth || !auth->required || !password || !auth->salt || !auth->challenge)
        return false;

    struct dstr pw_salt, pw_salt_hash_b64, final;
    dstr_init(&pw_salt);
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

    /* Step 3: base64 encode it */
    size_t len = base64_encode(sha256, NULL, SHA256_BLOCK_SIZE, 0);
    dstr_resize(&pw_salt_hash_b64, len);
    base64_encode(sha256, (BYTE *)pw_salt_hash_b64.array, SHA256_BLOCK_SIZE, 0);

    /* Step 4: Concatenate it with the challenge */
    dstr_cat(&pw_salt_hash_b64, auth->challenge);

    /* Step 5: Hash it again */
    sha256_init(&ctx);
    sha256_update(&ctx, (BYTE *)pw_salt_hash_b64.array, pw_salt_hash_b64.len);
    sha256_final(&ctx, sha256);

    /* Step 6: base64 encode it again */
    len = base64_encode(sha256, NULL, SHA256_BLOCK_SIZE, 0);
    dstr_resize(&final, len);
    base64_encode(sha256, (BYTE *)final.array, SHA256_BLOCK_SIZE, 0);

    auth->auth_response = bstrdup(final.array);

    /* Clean up */
    dstr_free(&pw_salt);
    dstr_free(&pw_salt_hash_b64);
    dstr_free(&final);
    bool result = auth->auth_response && strlen(auth->auth_response);

    if (result) {
        bfree(auth->salt);
        bfree(auth->challenge);
        auth->salt = NULL;
        auth->challenge = NULL;
    }
    return result;
}
