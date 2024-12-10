// render_graph_test.cpp

// includes

// engine
#include "engine/base.h"
#include "engine/log.h"
#include "engine/os.h"
#include "engine/gfx.h"
#include "engine/font.h"
#include "engine/draw.h"
#include "engine/ui.h"
#include "engine/draw_3d.h"

#include "engine/base.cpp"
#include "engine/log.cpp"
#include "engine/os.cpp"
#include "engine/gfx.cpp"
#include "engine/font.cpp"
#include "engine/draw.cpp"
#include "engine/ui.cpp"
#include "engine/draw_3d.cpp"


// utils
#include "utils/render_graph.h"

#include "utils/render_graph.cpp"


// globals
global os_window_t* window;
global gfx_renderer_t* renderer;
global ui_context_t* ui_context;
global render_graph_t* graph;
global b8 quit = false;

// functions

function void app_init();
function void app_release();
function void app_frame();
function void app_main_pass(render_pass_data_t* in, render_pass_data_t* out);
function void app_ui_pass(render_pass_data_t* in, render_pass_data_t* out);

// implementation

function void
app_init() {

	// open window and create renderer
	window = os_window_open(str("render graph"), 1280, 960);
	renderer = gfx_renderer_create(window, color(0x000000ff));
	ui_context = ui_context_create(renderer);

	// set frame function
	os_window_set_frame_function(window, app_frame);

	// create render graph
	graph = render_graph_create(renderer);
	render_pass_desc_t desc = { 0 };
	desc.label = str("main");
	desc.size = renderer->resolution;
	desc.execute_func = app_main_pass;
	desc.format = gfx_texture_format_rgba8;
	render_pass_t* main_pass = render_graph_add_pass(graph, desc);

	desc.label = str("ui");
	desc.size = renderer->resolution;
	desc.execute_func = app_ui_pass;
	desc.format = gfx_texture_format_rgba8;
	render_pass_t* ui_pass = render_graph_add_pass(graph, desc);



	render_graph_pass_connect(main_pass, ui_pass);
	render_graph_pass_connect(ui_pass, graph->output_pass);

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
		gfx_render_target_clear(out->render_target, color(0x181820ff));

		// render scene

		


	}

}

function void
app_ui_pass(render_pass_data_t* in, render_pass_data_t* out) {
	if (out->render_target != nullptr) {

		// resize if needed
		if (!uvec2_equals(out->render_target->size, renderer->resolution)) {
			gfx_render_target_resize(out->render_target, renderer->resolution);
		}

		gfx_set_render_target(out->render_target);

		// blit previous
		if (in->render_target != nullptr) {
			gfx_texture_blit(out->render_target->color_texture, in->render_target->color_texture);
		}

		// draw ui
		draw_begin(renderer);
		ui_begin_frame(ui_context);
		ui_push_pref_width(ui_size_pixel(150.0f, 1.0f));
		ui_push_pref_height(ui_size_pixel(20.0f, 1.0f));


		// labels
		f32 font_size = roundf(5.0f * (sinf(window->elasped_time) + 2.0f));
		ui_labelf("This is a label");

		// buttons
		ui_buttonf("Button");
		ui_spacer();

		// checkbox
		persist b8 checkbox_value;
		ui_checkbox(str("Checkbox"), &checkbox_value);
		ui_spacer();

		// slider
		persist f32 slider_value = 0.35f;
		ui_slider(str("Slider"), &slider_value, 0.0f, 1.0f);
		ui_spacer();

		// expander
		persist b8 expander_value = false;
		ui_expander(str("Color Pickers"), &expander_value);
		if (expander_value) {	

			persist color_t hsv_color = color(1.0f, 0.5f, 0.7f, color_format_hsv);
			ui_set_next_pref_height(ui_size_pixel(150.0f, 1.0f));
			ui_color_wheel(str("Color_Wheel"), &hsv_color.h, &hsv_color.s, &hsv_color.v);


		}


		ui_end_frame(ui_context);
		draw_end(renderer);

	}

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

// entry point

function i32
app_entry_point(i32 argc, char** argv) {

	// init layers
	log_init();
	os_init();
	gfx_init();
	font_init();
	draw_init();
	draw_3d_init();
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
	draw_3d_release();
	draw_release();
	font_release();
	gfx_release();
	os_release();
	log_release();

	printf("[info] successfully closed.\n");

	return 0;
}