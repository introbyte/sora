// code_editor.cpp

// - includes

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

#include "projects/editor/format.h"
#include "projects/editor/format.cpp"

// - globals
global os_window_t* window;
global gfx_renderer_t* renderer;
global ui_context_t* ui;
global b8 quit = false;

// - functions

// title bar
function ui_interaction title_bar_button(str_t label);

// app
function void app_init();
function void app_release();
function void app_frame();

// entry point
function i32 app_entry_point(i32 argc, char** argv);

// - implementation

// title bar

function ui_interaction 
title_bar_button(str_t label) {

	ui_frame_flags flags =
		ui_frame_flag_clickable |
		ui_frame_flag_draw_background |
		ui_frame_flag_draw_hover_effects |
		ui_frame_flag_draw_active_effects |
		ui_frame_flag_draw_text;
	
	ui_set_next_text_alignment(ui_text_alignment_center);
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);

	return interaction;
}

function void
title_bar_ui() {

	// parent frame
	ui_set_next_pref_width(ui_size_percent(1.0f));
	ui_set_next_pref_height(ui_size_pixel(30.0f, 1.0f));
	ui_push_rounding(0.0f);
	ui_set_next_layout_axis(ui_axis_x);
	ui_frame_t* title_bar_frame = ui_frame_from_string({0}, ui_frame_flag_draw_background);

	ui_push_parent(title_bar_frame);
		
		// set buttons sizes and y pos
		ui_push_fixed_width(40.0f);
		ui_push_fixed_height(30.0f);
		ui_push_fixed_y(0.0f);
		ui_push_flags(ui_frame_flag_floating);

		// close button
		ui_set_next_fixed_x(window->resolution.x - 40.0f);
		if (title_bar_button(str("###close button")) & ui_interaction_left_clicked) {
			quit = true;
		}

		// maximize button
		ui_set_next_fixed_x(window->resolution.x - 80.0f);
		if (title_bar_button(str("###maximize button")) & ui_interaction_left_clicked) {
			if (window->maximized) {
				os_window_restore(window);
			} else {
				os_window_maximize(window);
			}
		}

		// minimize button
		ui_set_next_fixed_x(window->resolution.x - 120.0f);
		if (title_bar_button(str("###minimize button")) & ui_interaction_left_clicked) {
			os_window_minimize(window);
		}

		ui_pop_fixed_height();
		ui_pop_fixed_width();
		ui_pop_fixed_y();
		ui_pop_flags();


		ui_push_pref_width(ui_size_pixel(50.0f, 1.0f));
		ui_push_pref_height(ui_size_percent(1.0f));

		// icon

		title_bar_button(str("File"));
		title_bar_button(str("Edit"));
		title_bar_button(str("View"));
		title_bar_button(str("Help"));
		
		ui_pop_pref_width();
		ui_pop_pref_height();


	ui_pop_parent();
	ui_pop_rounding();
}

function void
title_bar_draw() {

	// draw top border
	draw_push_softness(0.0f);
	
	if (!os_window_is_maximized(window)) {

		color_t border_color;
		if (os_window_is_active(window)) {
			border_color = color(0x5C5A58ff);
		} else {
			border_color = color(0x393939ff);
		}
		draw_set_next_color(border_color);
		draw_rect(rect(0.0f, 0.0f, window->resolution.x, 1.0f));
	}

	// title bar shadow
	draw_set_next_color0(color(0x00000018));
	draw_set_next_color1(color(0x00000000));
	draw_set_next_color2(color(0x00000018));
	draw_set_next_color3(color(0x00000000));
	draw_rect(rect(0.0f, 30.0f, window->resolution.x, 40.0f));
	
	// button icons
	vec2_t close_pos = vec2(window->resolution.x - 20.0f, 15.0f);
	draw_set_next_thickness(1.0f);
	draw_line(vec2_sub(close_pos, vec2(5, 5)), vec2_add(close_pos, vec2(5, 5)));
	draw_set_next_thickness(1.0f);
	draw_line(vec2_sub(close_pos, vec2(5, -5)), vec2_add(close_pos, vec2(5, -5)));

	vec2_t maximize_pos = vec2(window->resolution.x - 60.0f, 15.0f);
	draw_set_next_thickness(1.0f);
	draw_rect(rect(maximize_pos.x - 6.0f, maximize_pos.y - 5.0f, maximize_pos.x + 6.0f, maximize_pos.y + 5.0f));

	vec2_t minimize_pos = vec2(window->resolution.x - 100.0f, 15.0f);
	draw_rect(rect(minimize_pos.x - 6.0f, minimize_pos.y, minimize_pos.x + 6.0f, minimize_pos.y + 1.0f));

	draw_pop_softness();
}


// app

function void
app_init() {

	// open window and create renderer
	window = os_window_open(str("code editor"), 1280, 960, os_window_flag_borderless);
	renderer = gfx_renderer_create(window, color(0x181818ff));
	ui = ui_context_create(renderer);

	os_window_set_frame_function(window, app_frame);

}

function void
app_release() {
	
	// release renderer and window
	ui_context_release(ui);
	gfx_renderer_release(renderer);
	os_window_close(window);

}

function void
app_frame() {
	
	// update layers
	os_update();
	gfx_update();
	ui_update();
	space_update();

	// add custom title bar areas
	os_window_clear_title_bar_client_area(window);
	os_window_add_title_bar_client_area(window, rect(window->resolution.x - 120.0f, 0.0f, window->resolution.x, 30.0f));
	os_window_add_title_bar_client_area(window, rect(0.0f, 0.0f, 200.0f, 30.0f));

	// hotkeys
	if (os_key_press(window, os_key_F11)) {
		if (window->maximized) {
			os_window_restore(window);
		} else {
			os_window_maximize(window);
		}
	}

	if (os_key_press(window, os_key_esc)) {
		quit = true;
	}

	// render
	if (renderer != nullptr) {
		gfx_renderer_begin(renderer);
		draw_begin(renderer);
		
		ui_begin_frame(ui);
		title_bar_ui();
		ui_end_frame(ui);



		space_render(window->delta_time);
		
		title_bar_draw();
		draw_end(renderer);
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
	os_init();
	gfx_init();
	font_init();
	draw_init();
	ui_init();
	space_init();

	// init
	app_init();

	// main loop
	while (!quit) {
		app_frame();
	}

	// release
	app_release();

	// release layers
	space_release();
	ui_release();
	draw_release();
	font_release();
	gfx_release();
	os_release();

	printf("[info] successfully closed.\n");

	return 0;
}
