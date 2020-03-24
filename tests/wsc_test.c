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
#include <string.h>

int main()
{
    wsc_init();
    wsc_connection_t *c = wsc_connect(NULL);
    wsc_init_struct(wsc_auth_data_t, auth);
    wsc_init_struct(wsc_video_info_t, vinf);
    wsc_init_struct(wsc_version_info_t, ver);
    wsc_init_struct(wsc_stats_t, stats);
    wsc_init_struct(wsc_ouputs_t, outputs);

    char *format = NULL;

    assert(c);
    assert(wsc_auth_required(c, &auth));

    if (auth.required) {
        assert(auth.salt);
        assert(auth.challenge);
        assert(wsc_prepare_auth(&auth, "1234qwer"));
        assert(wsc_authenticate(c, &auth));
    }

    /* Test messages */

    /* General */
    assert(wsc_get_video_info(c, &vinf));
    assert(wsc_get_version_info(c, &ver));
    assert(wsc_get_stats(c, &stats));
    assert(wsc_set_filename_format(c, "%CCYY.%MM.%DD %hh.%mm.%ss"));
    assert(wsc_get_filename_format(c, &format));
    assert(strcmp(format, "%CCYY.%MM.%DD %hh.%mm.%ss") == 0);
//    assert(wsc_set_heartbeat(c, true)); // TODO: Heartbeat event

    /* Outputs */
    assert(wsc_list_outputs(c, &outputs));
    assert(outputs.arr);
    assert(outputs.count);

    /* Clean up */
    wsc_free(format);
    wsc_free_outputs(&outputs);
    wsc_disconnect(c);
    assert(wsc_shutdown() == 0);
    return 0;
}
