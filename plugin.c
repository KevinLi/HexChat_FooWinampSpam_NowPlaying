/*
WinampNowPlaying - A "What the hell am I listening to now?" plugin for HexChat

circularstrstr() and GetCurrentSongsName()
Copyright (C) Yann HAMON & contributors

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

int circularstrstr(char* string, char* search)
{
	int equal = 1;
	int length;
	int i;
	// The compiler complains about comparing signed/unsigned integers
	unsigned int j;

	length = strlen(string);

	for (i = 0; i < length; ++i)
	{
		for (j = 0; j < strlen(search); j++)
		{
			if (string[(i+j) %length] != search[j])
				equal = 0;
			else
				equal = 1;
		}
		if (equal == 1)
		{
			return i;
		}
	}
	return -1;
}

void GetCurrentSongsName (HWND hwndWinamp, char* title, int titlesize)
{
	int length;
	char *title2;
	int i, j = 0;
	int pos = -1;
	char *p;

	GetWindowText (hwndWinamp, title, titlesize);
	length = strlen(title);

	if ((pos = circularstrstr (title, "- Winamp ***")) != -1)
	{
		// The option "scroll song title in taskbar" is on
		title2 = (char*) malloc (titlesize * sizeof (char));

		for (i = (pos + 12) % length; i != pos; i = (i + 1) % length)
			title2[j++] = title[i];

		title2[j] = '\0';

		p = title2;
		while (p < title2 + titlesize && *p != '.')
			p++;
		p += 2; // Deletes the . and the following white space

		strcpy(title, p);
		free(title2);
	}
	else
	{
		p = title;
		while (p < title + titlesize && *p != '.')
			p++;
		p += 2; // Deletes the . and the following white space
		if (p < title + titlesize)
			strncpy (title, p, titlesize - (p - title));

		// Deletes the trailing "- winamp"
		p = title + titlesize - 1;
		while (p > title && *p != '-')
			p--;
		*p = '\0';
	}
}

int np_timer(void *userdata)
{
	hwndWinamp = FindWindow ("Winamp v1.x", NULL);

	strcpy(old_title, new_title);

	SendMessage (hwndWinamp, WM_USER, (WPARAM) 0, (LPARAM) IPC_GETLISTPOS);
	GetCurrentSongsName (hwndWinamp, new_title, 1024);

	if(strcmp(new_title, old_title) != 0 && strcmp(new_title,"") != 0)
	{
		hexchat_printf(ph, "Now playing: %s", new_title);
	}

	return 1;
}

int hexchat_plugin_init (hexchat_plugin *plugin_handle,
						 char **plugin_name,
						 char **plugin_desc,
						 char **plugin_version,
						 char *arg)
{
	ph = plugin_handle;

	*plugin_name = "WinampNowPlaying";
	*plugin_desc = "Prints the track title when the track changes";
	*plugin_version = "1.0.1";

	timer_hook = hexchat_hook_timer(ph, 1000, np_timer, NULL);

	hexchat_printf (ph, "%s %s plugin loaded successfully!\n", *plugin_name, *plugin_version);

	return 1;
}
