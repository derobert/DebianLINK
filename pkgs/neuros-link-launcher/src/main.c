#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Evas.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

typedef struct {
	const char *icon;
	const char *cmd;
} LauncherButton;

static const LauncherButton g_launcher_buttons[] = {
	{ "obj/tv.png", "iceweasel" },
	{ "obj/movie.png", "/opt/xbmc/bin/xbmc" },
	{ "obj/chicken.png", "e16" },
};

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
}

void shutdown_efl() {
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

char *find_wallpaper() {
	/* returns a string, which you must free. */

	const char *exts[] = {".png", ".jpg"};
	const char *paths[] = {
		NULL, // will fill with ~/.wallpaper
		"../neuros-link-art/usr/share/images/link/desktop",
		"/usr/share/images/link/desktop"
	};

	const char *home = getenv("HOME");
	struct passwd *passwd = getpwuid(getuid());
	if (!home && passwd)
		home = passwd->pw_dir;

	char buf[256];
	if (home) {
		buf[0] = 0;
		strncat(buf, home, sizeof(buf)-1);
		strncat(buf, "/.wallpaper", sizeof(buf)-1-strlen(buf));
		paths[0] = buf;
	}

	// finally, we can search!
	for (size_t a = 0; a < sizeof(paths)/sizeof(char*); ++a) {
		for (size_t b = 0; b< sizeof(exts)/sizeof(char*); ++b) {
			char filename[256];
			filename[0] = 0;
			strncat(filename, paths[a], sizeof(filename)-1);
			strncat(filename, exts[b], sizeof(filename)-1-strlen(filename));

			struct stat sbuf;
			if (0 == stat(filename, &sbuf))
				return strdup(filename);
		}
	}

	return NULL;
}

void make_buttons(Evas *evas, int screen_w, int screen_h) {
	int button_count = sizeof(g_launcher_buttons) / sizeof(LauncherButton);

	int usable_w = screen_w * 3/5;
	int usable_h = screen_h * 3/5;

	int button_size, left_usable, top_usable;
	if (usable_w / button_count > usable_h) {
		// we are height constrained, so set size to usable height and
		// center horizontally
		button_size = usable_h;
		left_usable = (usable_w - (button_size * button_count)) / 2;
		top_usable = 0;
	} else {
		// we are width constrained (normal case), so set size to usable
		// width divided by button count and center vertically.
		button_size = usable_w / button_count;
		left_usable = 0;
		top_usable = (usable_h - button_size) / 2;
	}

	int left_absolute = left_usable + (screen_w * 1/5);
	int top_absolute = top_usable + (screen_h * 1/5);

	for (int i = 0; i < button_count; ++i) {
		Evas_Object *btn = evas_object_rectangle_add(evas);
		evas_object_resize(btn, button_size, button_size);
		evas_object_move(btn, left_absolute, top_absolute);
		evas_object_color_set(btn, 20, 20, 30, 150);
		evas_object_show(btn);

		Evas_Object *btni = evas_object_image_filled_add(evas);
		evas_object_image_file_set(btni, g_launcher_buttons[i].icon, "");
		printf("btni->load_error: %i\n", evas_object_image_load_error_get(btni));
		evas_object_image_smooth_scale_set(btni, 1);
		evas_object_resize(btni, button_size, button_size);
		evas_object_move(btni, left_absolute, top_absolute);
		evas_object_show(btni);


		left_absolute += button_size;
	}
}

int main(int argc, char **argv) {
    Ecore_Evas *ee;
    Evas *evas;

    init_efl();

    ee = ecore_evas_software_x11_new(NULL, 0,  0, 0, 400, 400);
    if (!ee) {
		fprintf(stderr, "FATAL: Could not get an ee...\n");
		exit(1);
	}
    Ecore_X_Screen_Size sz = get_screen_size(ee);
    ecore_evas_resize(ee, sz.width, sz.height);
    ecore_evas_title_set(ee, "Launcher");
    ecore_evas_borderless_set(ee, 1);
    ecore_evas_fullscreen_set(ee, 1);


    evas = ecore_evas_get(ee);

	char *wallpaper;
	if ((wallpaper = find_wallpaper())) {
		Evas_Object *bg = evas_object_image_filled_add(evas);
		evas_object_image_file_set(bg, wallpaper, "");
		evas_object_image_smooth_scale_set(bg, 1);
		evas_object_move(bg, 0, 0);
		evas_object_resize(bg, sz.width, sz.height);
		evas_object_show(bg);
		free(wallpaper);
	}

	make_buttons(evas, sz.width, sz.height);

    ecore_evas_show(ee);
    ecore_main_loop_begin();

	shutdown_efl();

    return 0;

}

