// editor.cpp

// includes

// engine
#include "engine/base.h"
#include "engine/os.h"
#include "engine/gfx.h"

#include "engine/base.cpp"
#include "engine/os.cpp"
#include "engine/gfx.cpp"

// globals
global os_window_t* window;
global gfx_renderer_t* renderer;

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


	
	gfx_renderer_submit(renderer);
}

// entry point

function i32
app_entry_point(i32 argc, char** argv) {

	// init layers
	os_init();
	gfx_init();
	
	// init
	app_init();

	// main loop
	b8 quit = false;
	while (!quit) {

		// update layers
		os_update();
		gfx_update();

		// update app
		app_update();
		
		// get close events
		for (os_event_t* e = os_state.event_list.first, *next = nullptr; e != 0; e = next) {
			next = e->next;
			if (e->type == os_event_type_window_close) {
				quit = true;
			}
		}
	}

	// release
	app_release();

	// release layers
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