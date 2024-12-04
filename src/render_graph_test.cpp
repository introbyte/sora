// render_graph_test.cpp

// includes

// engine
#include "engine/base.h"
#include "engine/log.h"
#include "engine/os.h"
#include "engine/gfx.h"

#include "engine/base.cpp"
#include "engine/log.cpp"
#include "engine/os.cpp"
#include "engine/gfx.cpp"

// utils
#include "utils/render_graph.h"

#include "utils/render_graph.cpp"


// globals
global os_window_t* window;
global gfx_renderer_t* renderer;
global render_graph_t* graph;
global b8 quit = false;

// functions

function void app_init();
function void app_release();
function void app_frame();
function void app_main_pass(render_pass_data_t* in, render_pass_data_t* out);

// implementation

function void
app_init() {

	// open window and create renderer
	window = os_window_open(str("render graph"), 1280, 960);
	renderer = gfx_renderer_create(window, color(0x000000ff));

	// set frame function
	os_window_set_frame_function(window, app_frame);

	// create render graph
	graph = render_graph_create(renderer);
	render_pass_desc_t desc = { 0 };
	desc.label = str("main");
	desc.size = renderer->resolution;
	desc.execute_func = app_main_pass;
	desc.format = gfx_texture_format_rgba8;
	//desc.flags = gfx_render_target_flag_depth;
	render_pass_t* main_pass = render_graph_add_pass(graph, desc);
	render_graph_pass_connect(main_pass, graph->output_pass);

	render_graph_build(graph);

}

function void
app_release() {
	
	// release renderer and window
	gfx_renderer_release(renderer);
	os_window_close(window);

}

function void
app_main_pass(render_pass_data_t* in, render_pass_data_t* out) {
	if (out->render_target != nullptr) {
		
		// resize if needed
		if (!uvec2_equals(out->render_target->size, renderer->resolution)) {
			gfx_render_target_resize(out->render_target, renderer->resolution);
		}

		gfx_set_render_target(out->render_target);
		gfx_render_target_clear(out->render_target, color(0xffffffff));



		


	}

}

function void
app_frame() {

	// update layers
	os_update();
	gfx_update();
	
	// hotkeys
	if (os_key_press(window, os_key_F11)) {
		os_window_fullscreen(window);
	}

	if (os_key_press(window, os_key_esc)) {
		quit = true;
	}

	// render
	if (renderer != nullptr) {
		gfx_renderer_begin(renderer);

		render_graph_execute(graph);

		gfx_renderer_end(renderer);
	}

	// get close events
	if (os_event_get(os_event_type_window_close) != 0) {
		quit = true;
	}
}

// entry point

function i32
app_entry_point(i32 argc, char** argv) {

	// init layers
	log_init();
	os_init();
	gfx_init();
	render_init();

	// init
	app_init();

	// main loop
	while (!quit) {
		app_frame();
	}

	// release
	app_release();

	// release layers
	render_release();
	gfx_release();
	os_release();
	log_release();

	printf("[info] successfully closed.\n");

	return 0;
}