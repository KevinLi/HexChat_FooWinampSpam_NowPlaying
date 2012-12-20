/*
HexChat_FooWinampSpam_NowPlaying - A "What the hell am I listening to now?" plugin for HexChat

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wformat"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
*/

#include <stdio.h>
#include <windows.h>

#include "hexchat-plugin.h"
#include "winamp.h"

static hexchat_plugin *ph;
static hexchat_hook *timer_hook;
static int enable = 1;

HWND hwndWinamp = NULL;
char old_title[1024];
char new_title[1024];
unsigned int pos;

int np_timer(void *userdata)
{
    hwndWinamp = FindWindow("Winamp v1.x", NULL);

    strcpy(old_title, new_title);

    SendMessage(hwndWinamp, WM_USER, (WPARAM) 0, (LPARAM) IPC_GETLISTPOS);
    GetWindowText(hwndWinamp, new_title, 1024);

    // foo_winamp_spam and foo_vis_shpeck give almost identical outputs.
    // foo_vis_shpeck probably shouldn't use this block of code here.
    // But this plugin's for foo_winamp_spam.
    // Find the first period, remove that and whatever's in front of it.
    for(pos = 0; pos < strlen(new_title); pos++)
	{
        if(new_title[pos] == '.')
		{
            break;
        }
    }
    strncpy(new_title, new_title + pos + 2, 1024);

    // Truncate "- Winamp" from end
    new_title[strlen(new_title) - 9] = '\0';

    if(strcmp(new_title, old_title) != 0 && strcmp(new_title, "") != 0)
    {
        hexchat_printf(ph, "Now playing: %s", new_title);
    }

    return 1;
}

int hexchat_plugin_init(hexchat_plugin *plugin_handle, char **plugin_name,
	char **plugin_desc, char **plugin_version, char *arg)
{
    ph = plugin_handle;

    *plugin_name = "HexChat_FooWinampSpam_NowPlaying";
    *plugin_desc = "Prints the track title when the track changes";
    *plugin_version = "1.0.2";

    timer_hook = hexchat_hook_timer(ph, 1000, np_timer, NULL);

    hexchat_printf(ph, "%s %s plugin loaded successfully!\n", *plugin_name, *plugin_version);

    return 1;
}
