#include <stdlib.h>
#include <stdio.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>

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

	return size;
}

int main(int argc, char **argv) {
    Ecore_Evas *ee;
    Evas *evas;

    init_efl();

    ee = ecore_evas_software_x11_new(NULL, 0,  0, 0, 400, 400);
    Ecore_X_Screen_Size sz = get_screen_size(ee);
    ecore_evas_resize(ee, sz.width, sz.height);
    ecore_evas_title_set(ee, "Launcher");
    ecore_evas_borderless_set(ee, 1);
    ecore_evas_fullscreen_set(ee, 1);


    evas = ecore_evas_get(ee);

    // Evas_Object *bg;
    // bg = evas_object_rectangle_add(evas);
    // evas_object_resize(bg, (double)WIDTH, (double)HEIGHT);
    // evas_object_color_set(bg, 255, 255, 0, 255);
    // evas_object_show(bg);

    /* Insert Object Here */
    Evas_Object *img = evas_object_image_add(evas);
    evas_object_image_file_set(img, "test.png", "");
    evas_object_move(img, 0, 0);
    evas_object_resize(img, 768, 768);
    evas_object_image_fill_set(img, 0, 0, 768, 768);
    evas_object_show(img);


    ecore_evas_show(ee);
    ecore_main_loop_begin();

	shutdown_efl();

    return 0;

}

