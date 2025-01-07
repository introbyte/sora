// render_graph_test.cpp

// the api i want:
// 
// render_graph_t* graph = render_graph_create();
//  
//

// includes

// engine
#include "engine/base.h"
#include "engine/log.h"
#include "engine/os.h"
#include "engine/gfx.h"
#include "engine/font.h"
#include "engine/draw.h"
#include "engine/ui.h"

#include "engine/base.cpp"
#include "engine/log.cpp"
#include "engine/os.cpp"
#include "engine/gfx.cpp"
#include "engine/font.cpp"
#include "engine/draw.cpp"
#include "engine/ui.cpp"

// utils
#include "utils/render_graph.h"
#include "utils/render_graph.cpp"

// structs

struct main_render_pass_data_t {
	gfx_render_target_t* render_target;
};

// globals
global os_window_t* window;
global gfx_renderer_t* renderer;
global ui_context_t* ui_context;
global render_graph_t* graph;
global b8 quit = false;

global gfx_texture_3d_t* cloud_texture;

global gfx_compute_shader_t* compute_shader;

// functions

// main pass
function void app_main_pass_setup(render_pass_t* pass);
function void app_main_pass_execute(void* in, void* out);
function void app_main_pass_release(render_pass_t* pass);

// ui pass
function void app_ui_pass_setup(render_pass_t* pass);
function void app_ui_pass_execute(void* in, void* out);
function void app_ui_pass_release(render_pass_t* pass);

// app
function void app_init();
function void app_release();
function void app_frame();
function i32 app_entry_point(i32 argc, char** argv);

// implementation

// main pass

function void 
app_main_pass_setup(render_pass_t* pass) {

	main_render_pass_data_t* data = (main_render_pass_data_t*)pass->data;

	// allocate resources for main pass
	data->render_target = gfx_render_target_create(gfx_texture_format_rgba8, renderer->resolution, 0);
	
}

function void
app_main_pass_execute(void* in, void* out) {	
	
	main_render_pass_data_t* data = (main_render_pass_data_t*)out;

	// handle resize 
	if (!uvec2_equals(data->render_target->size, renderer->resolution)) {
		gfx_render_target_resize(data->render_target, renderer->resolution);
	}


	gfx_set_render_target(data->render_target);
	gfx_render_target_clear(data->render_target, color(0x000000ff));

	gfx_set_texture_3d(cloud_texture, 0);



}

function void 
app_main_pass_release(render_pass_t* pass) {
	// release resources
	main_render_pass_data_t* data = (main_render_pass_data_t*)pass->data;
	gfx_render_target_release(data->render_target);
}

// ui pass

function void
app_ui_pass_setup(render_pass_t* pass) {

	output_render_pass_data_t* data = (output_render_pass_data_t*)pass->data;

	// allocate resources for ui pass
	data->renderer = renderer;
	data->render_target = gfx_render_target_create(gfx_texture_format_rgba8, renderer->resolution, 0);
}

function void
app_ui_pass_execute(void* in, void* out) {

	main_render_pass_data_t* in_data = (main_render_pass_data_t*)in;
	output_render_pass_data_t* out_data = (output_render_pass_data_t*)out;

	// handle resize 
	if (!uvec2_equals(out_data->render_target->size, renderer->resolution)) {
		gfx_render_target_resize(out_data->render_target, renderer->resolution);
	}

	gfx_texture_blit(out_data->render_target->color_texture, in_data->render_target->color_texture); // copy from in data
	gfx_set_render_target(out_data->render_target);

	draw_begin(renderer);
	ui_begin_frame(ui_context);

	ui_push_pref_width(ui_size_pixel(200.0f, 1.0f));
	ui_push_pref_height(ui_size_pixel(20.0f, 1.0f));

	u32 index = 1;
	for (render_pass_node_t* node = graph->execute_list.first; node != 0; node = node->next) {
		ui_labelf("%u : %s", index, node->pass->label.data);
		index++;
	}


	ui_end_frame(ui_context);
	draw_end(renderer);


}

function void
app_ui_pass_release(render_pass_t* pass) {
	// release resources
	output_render_pass_data_t* data = (output_render_pass_data_t*)pass->data;
	gfx_render_target_release(data->render_target);
}

// entry point

function void
app_init() {

	// open window and create renderer
	window = os_window_open(str("render graph"), 960, 720);
	renderer = gfx_renderer_create(window, color(0x000000ff));
	ui_context = ui_context_create(renderer);

	// set frame function
	os_window_set_frame_function(window, app_frame);

	// create render graph
	graph = render_graph_create(renderer);

	// main pass
	render_pass_desc_t desc = { 0 };
	desc.label = str("main");
	desc.data_size = sizeof(main_render_pass_data_t);
	desc.setup_func = app_main_pass_setup;
	desc.execute_func = app_main_pass_execute;
	desc.release_func = app_main_pass_release;
	render_pass_t* main_pass = render_graph_add_pass(graph, desc);

	// ui pass
	desc.label = str("ui");
	desc.data_size = sizeof(output_render_pass_data_t);
	desc.setup_func = app_ui_pass_setup;
	desc.execute_func = app_ui_pass_execute;
	desc.release_func = app_ui_pass_release;
	render_pass_t* ui_pass = render_graph_add_pass(graph, desc);

	// build render graph
	render_graph_pass_connect(main_pass, ui_pass);
	render_graph_pass_connect(ui_pass, graph->output_pass);
	render_graph_build(graph);

	// load compute shader
	compute_shader = gfx_compute_shader_load(str("res/shaders/compute/gen_clouds.hlsl"));

	// create 3d cloud texture
	cloud_texture = gfx_texture_3d_create(str("clouds"), uvec3(64, 64, 64), gfx_texture_format_rgba8);
	
	// run compute shader
	gfx_set_compute_shader(compute_shader);
	gfx_set_texture_3d(cloud_texture, 0, gfx_texture_usage_cs);
	gfx_dispatch(8, 8, 8);
	gfx_set_compute_shader();
	gfx_set_texture_3d(nullptr, 0);

}

function void
app_release() {
	
	// release renderer and window
	gfx_renderer_release(renderer);
	os_window_close(window);

}

function void
app_frame() {

	// update layers
	os_update();
	gfx_update();
	ui_update();

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

function i32
app_entry_point(i32 argc, char** argv) {

	// init layers
	log_init();
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
		app_frame();
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
	log_release();

	printf("[info] successfully closed.\n");

	return 0;
}