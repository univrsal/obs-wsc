#include <cheat.h>
#include <obs_wsc.h>

static obs_wsc_connection_t *conn = NULL;
static obs_wsc_auth_data_t auth = {};

CHEAT_TEST(connect,
           cheat_assert((conn = obs_wsc_connect(NULL, 4444)) != NULL);
)

CHEAT_TEST(auth_required,
           cheat_assert(obs_wsc_auth_required(conn, &auth));
)
