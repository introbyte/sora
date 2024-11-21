// template_multi_window.cpp

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


// app structs

struct app_window_t {
	app_window_t* next;
	app_window_t* prev;

	str_t title;

	os_window_t* os_window;
	gfx_renderer_t* gfx_renderer;
	ui_context_t* ui_context;
};

struct app_state_t {

	arena_t* arena;
	arena_t* scratch;

	app_window_t* window_first;
	app_window_t* window_last;
	app_window_t* window_free;


};

// globals
global app_state_t app_state;

// functions

function app_window_t* 
app_window_open(str_t title, u32 width, u32 height) {

	// try to get unused window from app state
	app_window_t* window = app_state.window_free;
	if (window != nullptr) {
		stack_pop(app_state.window_free);
	} else {
		// else allocate a new window
		window = (app_window_t*)arena_alloc(app_state.arena, sizeof(app_window_t));
	}
	memset(window, 0, sizeof(app_window_t));

	// add to app state window list
	dll_push_back(app_state.window_first, app_state.window_last, window);

	window->title = title;

	// open os window
	window->os_window = os_window_open(title, width, height);

	// create gfx renderer
	window->gfx_renderer = gfx_renderer_create(window->os_window, color(0x131313ff));
	
	// create ui
	window->ui_context = ui_context_create(window->gfx_renderer);

	return window;
}

function void
app_window_close(app_window_t* window) {

	// remove from app state window list
	dll_remove(app_state.window_first, app_state.window_last, window);

	// add to app state window free stack
	stack_push(app_state.window_free, window);

	// release os window and gfx renderer
	ui_context_release(window->ui_context);
	gfx_renderer_release(window->gfx_renderer);
	os_window_close(window->os_window);

}

function app_window_t*
app_window_from_os_window(os_window_t* window) {
	app_window_t* result = nullptr;
	for (app_window_t* app_window = app_state.window_first; app_window != 0; app_window = app_window->next) {
		if (app_window->os_window == window) {
			result = app_window;
			break;
		}
	}
	return result;
}

function b8
app_no_active_windows() {
	return (app_state.window_first == 0);
}

function void
app_init() {

	// allocate memory arena
	app_state.arena = arena_create(gigabytes(2));

	// window list
	app_state.window_first = nullptr;
	app_state.window_last = nullptr;
	app_state.window_free = nullptr;
	
	// open windows
	app_window_open(str("multi-window 1"), 640, 480);
	app_window_open(str("multi-window 2"), 640, 480);


}

function void
app_release() {

	// release memory arena
	arena_release(app_state.arena);
}

function void
app_update() {

	// update every window
	for (app_window_t* window = app_state.window_first, *next = 0; window != 0; window = next) {
		next = window->next;

		// hotkeys
		if (os_key_press(window->os_window, os_key_F11)) {
			os_window_fullscreen(window->os_window);
		}

		if (os_key_press(window->os_window, os_key_esc)) {
			app_window_close(window);
			continue;
		}

		// render
		gfx_renderer_begin(window->gfx_renderer);
		draw_begin(window->gfx_renderer);
		ui_begin_frame(window->ui_context);

		ui_push_pref_width(ui_size_percent(1.0f));
		ui_push_pref_height(ui_size_pixel(20.0f, 1.0f));

		ui_labelf("hello, world!");
		if (ui_buttonf("button") & ui_interaction_left_clicked) {
			app_window_open(str("another window"), 640, 480);
		}

		ui_end_frame(window->ui_context);
		draw_end(window->gfx_renderer);
		gfx_renderer_end(window->gfx_renderer);
	}

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
	while (!app_no_active_windows()) {

		// update layers
		os_update();
		gfx_update();
		ui_update();

		// update app
		app_update();
		
		// get close events
		os_event_t* close_event = os_event_get(os_event_type_window_close);
		if (close_event != nullptr) {
			app_window_t* window = app_window_from_os_window(close_event->window);
			if (window != nullptr) {
				app_window_close(window);
			}
			os_event_pop(close_event);
		}
	}

	// release
	app_release();

	// release layers
	ui_release();
	draw_init();
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