/* 
 * Neuros LINK launcher
 * Copyright (C) 2010  Anthony DeRobertis
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 * USA
*/

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Edje.h>
#include <Evas.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

void fatal(const char *err) {
    fprintf(stderr, "%s\n", err);
    abort();
}

void init_efl() {
    if (! ecore_init())
        fatal("Could not init ecore");
    if (! ecore_x_init(NULL))
        fatal("Could not init ecore_x");
    if (! ecore_evas_init())
        fatal("Could not init ecore_evas");
    if (! edje_init())
    	fatal("Could not init edje");
}

void shutdown_efl() {
	edje_shutdown();
	ecore_evas_shutdown();
	ecore_x_shutdown();
	ecore_shutdown();
}

Ecore_X_Screen_Size get_screen_size(Ecore_Evas *ee) {
	Ecore_X_Window child = ecore_evas_software_x11_window_get(ee);
	Ecore_X_Window root = ecore_x_window_root_get(child);

	/*
	 * Supposedly, you have to call these. But ecore_x doesn't export
	 * them! And it works without, so, whatever.
	*/
	//ecore_x_randr_get_screen_info_prefetch(root);
	//ecore_x_randr_get_screen_info_fetch();
	Ecore_X_Screen_Size size = ecore_x_randr_current_screen_size_get(root);

	double aspect = (double)size.width / size.height;
	if (fabs(aspect - 8.0/3) < 0.1) {
		// kluge: must be two 4:3 monitors, i.e., dero's setup. really
		// need to get size of single destkop...
		size.width /= 2;
	}

	return size;
}

char *find_theme() {
	/* returns a string, which you must free. */

	const char *paths[] = {
		NULL, // will fill with ~/.nl-launcher.edj
		"./bin/nl-launcher.edj",
		"nl-launcher.edj",
		"/usr/share/nl-launcher/nl-launcher.edj"
	};

	const char *home = getenv("HOME");
	struct passwd *passwd = getpwuid(getuid());
	if (!home && passwd)
		home = passwd->pw_dir;

	char buf[256];
	if (home) {
		buf[0] = 0;
		strncat(buf, home, sizeof(buf)-1);
		strncat(buf, "/.nl-launcher.edj", sizeof(buf)-1-strlen(buf));
		paths[0] = buf;
	}

	// finally, we can search!
	for (size_t a = 0; a < sizeof(paths)/sizeof(char*); ++a) {
		struct stat sbuf;
		if (0 == stat(paths[a], &sbuf))
			return strdup(paths[a]);
	}

	return NULL;
}

int main(int argc, char **argv) {

    init_efl();

    char *theme = find_theme();
    if (!theme)
		fatal("Could not find theme");

    Ecore_Evas *ee = ecore_evas_software_x11_new(NULL, 0,  0, 0, 400, 400);
    if (!ee) {
		fprintf(stderr, "FATAL: Could not get an ee...\n");
		exit(1);
	}
    Ecore_X_Screen_Size sz = get_screen_size(ee);
    ecore_evas_resize(ee, sz.width, sz.height);
    ecore_evas_title_set(ee, "Launcher");
    ecore_evas_borderless_set(ee, 1);
    ecore_evas_fullscreen_set(ee, 1);

    Evas *evas = ecore_evas_get(ee);
    Evas_Object *edje = edje_object_add(evas);
    edje_object_file_set(edje, theme, "main");
    evas_object_move(edje, 0, 0);
    evas_object_resize(edje, sz.width, sz.height);
    evas_object_show(edje);

    ecore_evas_show(ee);
    ecore_main_loop_begin();

	shutdown_efl();

    return 0;

}


