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

#include "util.h"
#include "external/bmem.h"
#include <stdlib.h>
#include <string.h>

char *util_random_id(const struct darray *ids)
{
    static char id[MSG_ID_LENGTH];
    int idx = 0;
    bool unique = false;

    while (!unique) {
        while (idx < MSG_ID_LENGTH) {
            int rnd = rand() % (26 + 26 + 10);
            if (rnd < 26)
                id[idx] = (char)('a' + rnd);
            else if (rnd < 26 + 26)
                id[idx] = (char)('A' + rnd - 26);
            else
                id[idx] = (char)('0' + rnd - 26 - 26);
            idx++;
        }

        id[MSG_ID_LENGTH - 1] = '\0';

        unique = true;
        char **arr = ids->array;
        for (size_t i = 0; i < ids->num; i++) {
            if (strcmp(arr[i], id) == 0) {
                unique = false;
                break;
            }
        }
    }

    return id;
}
