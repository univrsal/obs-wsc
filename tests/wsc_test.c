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
#include <stdio.h>

int main()
{
    bool test_recording = false;
    bool test_profiles = false;
    bool test_replay = false;
    bool test_outputs = false;

    wsc_init();
    wsc_connection_t *c = wsc_connect(NULL);
    wsc_init_struct(wsc_auth_data_t, auth);
    wsc_init_struct(wsc_video_info_t, vinf);
    wsc_init_struct(wsc_version_info_t, ver);
    wsc_init_struct(wsc_stats_t, stats);
    wsc_init_struct(wsc_ouputs_t, outputs);
    wsc_init_struct(wsc_output_t, output);
    wsc_init_struct(wsc_profiles_t, profiles);

    char *format = NULL, *profile = NULL, *video_folder = NULL;

    assert(c);
    assert(wsc_auth_required(c, &auth));

    if (auth.required) {
        assert(auth.salt);
        assert(auth.challenge);
        assert(wsc_prepare_auth(&auth, "1234qwer"));
        assert(wsc_authenticate(c, &auth));
    }

    /* Test requests */

    /* General */
    assert(wsc_get_video_info(c, &vinf));
    assert(wsc_get_version_info(c, &ver));
    assert(wsc_get_stats(c, &stats));
    assert(wsc_set_filename_format(c, "%CCYY.%MM.%DD %hh.%mm.%ss"));
    assert(wsc_get_filename_format(c, &format));
    assert(strcmp(format, "%CCYY.%MM.%DD %hh.%mm.%ss") == 0);
//    assert(wsc_set_heartbeat(c, true)); // TODO: Heartbeat event

    /* Profiles */
    if (test_profiles) {
        assert(wsc_set_current_profile(c, "debug"));
        assert(wsc_get_current_profile(c, &profile));
        assert(strcmp(profile, "debug") == 0);
        assert(wsc_list_profiles(c, &profiles));
    }

    if (test_recording) {
        wsc_wait_ms(1000);
        assert(wsc_start_recording(c));
        wsc_wait_ms(1000);
        assert(wsc_pause_recording(c));
        wsc_wait_ms(1000);
        assert(wsc_resume_recording(c));
        wsc_wait_ms(1000);
        assert(wsc_toggle_recording(c));
        wsc_wait_ms(1000);
        assert(!wsc_stop_recording(c));
    }

    assert(wsc_set_recording_folder(c, "/home/usr/videos"));
    assert(wsc_get_recording_folder(c, &video_folder));
    assert(strcmp(video_folder, "/home/usr/videos"));

    if (test_outputs) {
        wsc_wait_ms(1000);
        assert(wsc_list_outputs(c, &outputs));
        assert(outputs.arr);
        assert(outputs.count);
        assert(wsc_get_output_info(c, "adv_file_output", &output));

        /* These only work if the recording has been started once before */
        assert(wsc_start_output(c, "adv_file_output"));
        wsc_wait_ms(1000);
        assert(wsc_stop_output(c, "adv_file_output", false));
    }

    if (test_replay) {
        assert(wsc_start_replay_buffer(c));
        wsc_wait_ms(1000);
        assert(wsc_save_replay_buffer(c));
        assert(wsc_toggle_replay_buffer(c));
        wsc_wait_ms(1000);
        assert(!wsc_stop_replay_buffer(c));
    }

    /* Clean up */
    wsc_free(format);
    wsc_free(video_folder);
    wsc_free(profile);
    wsc_free_profiles(&profiles);
    wsc_free_output(&output);
    wsc_free_outputs(&outputs);
    wsc_disconnect(c);
    assert(wsc_shutdown() == 0);
    return 0;
}
