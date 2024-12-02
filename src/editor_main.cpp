// editor_main.cpp

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

#include "projects/editor/editor.h"

#include "projects/editor/editor.cpp"

// app structs

struct app_window_t {
	app_window_t* next;
	app_window_t* prev;

	str_t title;

	arena_t* scratch_arena;

	os_window_t* os_window;
	gfx_renderer_t* gfx_renderer;

	space_t* space;
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

	window->scratch_arena = arena_create(megabytes(64));

	// open os window
	window->os_window = os_window_open(title, width, height);

	// create gfx renderer
	window->gfx_renderer = gfx_renderer_create(window->os_window, color(0x131313ff));

	// create space
	window->space = space_create();
	node_t* node0 = node_create(window->space, str("int"), node_type_keyword, node_flag_draw_text);
	node_t* node1 = node_create(window->space, str("main"), node_type_function_declaration, node_flag_draw_text);
	node_t* node2 = node_create(window->space, str("int"), node_type_keyword, node_flag_draw_text);
	node_t* node3 = node_create(window->space, str("argc"), node_type_identifier, node_flag_draw_text);
	node_t* node4 = node_create(window->space, str("char**"), node_type_keyword, node_flag_draw_text);
	node_t* node5 = node_create(window->space, str("argv"), node_type_identifier, node_flag_draw_text);

	node_insert(window->space->node_root, node0);
	node_insert(window->space->node_root, node1);
	node_insert(window->space->node_root, node2);
	node_insert(window->space->node_root, node3);
	node_insert(window->space->node_root, node4);
	node_insert(window->space->node_root, node5);

	return window;
}

function void
app_window_close(app_window_t* window) {

	// remove from app state window list
	dll_remove(app_state.window_first, app_state.window_last, window);

	// add to app state window free stack
	stack_push(app_state.window_free, window);

	arena_release(window->scratch_arena);

	space_release(window->space);

	// release os window and gfx renderer
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
	
	// open window
	app_window_open(str("editor"), 640, 480);

}

function void
app_release() {

	// release memory arena
	arena_release(app_state.arena);
}

function void 
app_frame() {

	// update layers
	os_update();
	gfx_update();
	editor_update();

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

		if (os_key_press(window->os_window, os_key_N, os_modifier_ctrl)) {
			app_window_open(str("editor"), 640, 480);
		}

		// render
		if (window->gfx_renderer != nullptr) {

			gfx_renderer_begin(window->gfx_renderer);
			draw_begin(window->gfx_renderer);

			//space_update(window->space);

			space_render(window->space);

			draw_end(window->gfx_renderer);
			gfx_renderer_end(window->gfx_renderer);
		}
		// clear scratch arena
		arena_clear(window->scratch_arena);
	}

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

// entry point

function i32
app_entry_point(i32 argc, char** argv) {

	// init layers
	os_init();
	gfx_init();
	font_init();
	draw_init();
	editor_init();

	// init
	app_init();

	// main loop
	while (!app_no_active_windows()) {
		app_frame();
	}

	// release
	app_release();

	// release layers
	editor_release();
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