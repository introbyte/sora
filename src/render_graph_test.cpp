// test.cpp

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

#include "utils/camera.h"
#include "utils/render_graph.h"

#include "utils/camera.cpp"
#include "utils/render_graph.cpp"

// globals
global os_window_t* window;
global gfx_renderer_t* renderer;
global ui_context_t* ui;
global arena_t* scratch;
global b8 quit = false;
global render_graph_t* render_graph;

// functions

function void 
ui_pass(render_pass_data_t* in, render_pass_data_t* out) {

	gfx_set_render_target(out->target_first->gfx_render_target);
	draw_begin(renderer);
	ui_begin_frame(ui);

	ui_push_pref_width(ui_size_pixel(300.0f, 1.0f));
	ui_push_pref_height(ui_size_pixel(20.0f, 1.0f));
	ui_labelf("Hello, World!");

	ui_end_frame(ui);
	draw_end(renderer);
	gfx_set_render_target();

}

function void
app_init() {

	// open window and create renderer
	window = os_window_open(str("app"), 640, 480);
	renderer = gfx_renderer_create(window, color(0x000000ff));
	ui = ui_context_create(renderer);
	scratch = arena_create(megabytes(8));

	// build render graph
	render_graph = render_graph_create(renderer);
	render_pass_t* pass = render_graph_add_pass(render_graph, str("ui_pass"), ui_pass);
	render_pass_add_target(pass, gfx_texture_format_rgba8);
	render_graph_pass_connect(pass, render_graph->output_pass);
	render_graph_build(render_graph);
}

function void
app_release() {
	
	// release renderer and window
	render_graph_release(render_graph);
	ui_context_release(ui);
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

	render_graph_execute(render_graph);

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
	render_init();

	// init
	app_init();

	// main loop
	while (!quit) {

		// update layers
		os_update();
		gfx_update();
		ui_update();

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
	render_release();
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