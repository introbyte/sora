// editor.cpp

// includes

// engine
#include "engine/base.h"
#include "engine/os.h"
#include "engine/gfx.h"
#include "engine/font.h"
#include "engine/draw.h"

#include "engine/base.cpp"
#include "engine/os.cpp"
#include "engine/gfx.cpp"
#include "engine/font.cpp"
#include "engine/draw.cpp"

// globals
global os_window_t* window;
global gfx_renderer_t* renderer;
global b8 quit = false;

// functions

function void
app_init() {

	// open window and create renderer
	window = os_window_open(str("app template"), 1280, 960);
	renderer = gfx_renderer_create(window, color(0x000000ff));

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


	gfx_renderer_end(renderer);
}

// entry point

function i32
app_entry_point(i32 argc, char** argv) {

	// init layers
	os_init();
	gfx_init();
	font_init();

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