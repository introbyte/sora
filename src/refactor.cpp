// refactor.cpp

// includes

// engine
#include "engine/base.h"
#include "engine/os.h"
#include "engine/gfx.h"
#include "engine/font.h"
#include "engine/draw.h"
#include "engine/ui.h"

#include "engine/base.cpp"
#include "engine/os.cpp"
#include "engine/gfx.cpp"
#include "engine/font.cpp"
#include "engine/draw.cpp"
#include "engine/ui.cpp"

// globals
global os_window_t* window;
global gfx_renderer_t* renderer;
global arena_t* scratch;
global b8 quit = false;

// functions

function void
app_init() {

	// open window and create renderer
	window = os_window_open(str("app"), 1280, 960);
	renderer = gfx_renderer_create(window, color(0x000000ff));

	scratch = arena_create(megabytes(8));
}

function void
app_release() {
	
	// release renderer and window
	gfx_renderer_release(renderer);
	os_window_close(window);

}

function void
app_update() {

	// hotkeys
	if (os_key_press(window, os_key_F11)) {
		os_window_fullscreen(window);
	}

	if (os_key_press(window, os_key_esc)) {
		quit = true;
	}

	// render
	gfx_renderer_begin(renderer);
	draw_begin(renderer);
	ui_begin_frame(renderer);

	ui_push_pref_width(ui_size_pixel(150.0f, 1.0f));
	ui_push_pref_height(ui_size_pixel(20.0f, 1.0f));

	ui_button(str("button"));
	persist f32 value = 0.5f;
	ui_slider(str("slider"), &value, 0.0f, 1.0f);

	persist color_t col = color(0x84322ff);
	ui_set_next_pref_height(ui_size_pixel(150.0f, 1.0f));
	ui_color_hue_sat_circle(str("color_circle"), &col.h, &col.s, col.v);

	ui_color_hue_bar(str("color_hue"), &col.h, col.s, col.v);
	ui_color_sat_bar(str("color_sat"), col.h, &col.s, col.v);
	ui_color_val_bar(str("color_val"), col.h, col.s, &col.v);

	ui_end_frame();
	draw_end(renderer);
	gfx_renderer_end(renderer);

	arena_clear(scratch);
}

// entry point

function i32
app_entry_point(i32 argc, char** argv) {

	// init layers
	os_init();
	gfx_init();
	font_init();
	draw_init();
	ui_init();

	// init
	app_init();

	// main loop
	while (!quit) {

		// update layers
		os_update();
		gfx_update();

		// update app
		app_update();
		
		// get close events
		if (os_event_get(os_event_type_window_close) != 0) {
			quit = true;
		}
	}

	// release
	app_release();

	// release layers
	ui_release();
	draw_release();
	font_release();
	gfx_release();
	os_release();

	printf("[info] successfully closed.\n");

	return 0;
}

// per build entry point

#if defined(BUILD_DEBUG)
int main(int argc, char** argv) {
	return app_entry_point(argc, argv);
}
#elif defined(BUILD_RELEASE)
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	return app_entry_point(__argc, __argv);
}
#endif 