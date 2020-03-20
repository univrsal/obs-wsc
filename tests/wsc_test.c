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

#include <obs_wsc.h>
#include <assert.h>

int main()
{
    obs_wsc_init();
    obs_wsc_connection_t *c = obs_wsc_connect(NULL);
    obs_wsc_auth_data_t auth = obs_wsc_auth_init();
    obs_wsc_video_info_t vinf = obs_wsc_video_info_init();
    obs_wsc_version_info_t ver = obs_wsc_version_info_init();

    assert(c);
    assert(obs_wsc_auth_required(c, &auth));

    if (auth.required) {
        assert(auth.salt);
        assert(auth.challenge);
        assert(obs_wsc_prepare_auth(&auth, "1234qwer"));
        assert(obs_wsc_authenticate(c, &auth));
    }

    /* Test messages */
    assert(obs_wsc_get_video_info(c, &vinf));
    assert(obs_wsc_get_version_info(c, &ver));

    /* Clean up */
    obs_wsc_disconnect(c);
    assert(obs_wsc_shutdown() == 0);
    return 0;
}
