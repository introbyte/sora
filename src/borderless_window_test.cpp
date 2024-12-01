// template_borderless_window.cpp

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
global ui_context_t* ui;
global b8 quit = false;

// functions

function void
app_init() {

	// open window and create renderer
	window = os_window_open(str("borderless"), 640, 480, os_window_flag_borderless);
	renderer = gfx_renderer_create(window, color(0x121212ff));
	ui = ui_context_create(renderer);

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

	if (window == nullptr) {
		return;
	}

	// update layers
	os_update();
	gfx_update();
	ui_update();

	// add custom title bar areas
	os_window_clear_title_bar_client_area(window);
	os_window_add_title_bar_client_area(window, rect(window->resolution.x - 120.0f, 0.0f, window->resolution.x, 30.0f));

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
		draw_begin(renderer);


		ui_begin_frame(ui);
			ui_push_flags(ui_frame_flag_floating);
			ui_push_fixed_width(40.0f);
			ui_push_fixed_height(30.0f);
			ui_push_rounding(0.0f);

			ui_set_next_fixed_x(window->resolution.x - 40.0f);
			ui_set_next_fixed_y(0.0f);
			if (ui_button(str("###close button")) & ui_interaction_left_clicked) {
				quit = true;
			}

			ui_set_next_fixed_x(window->resolution.x - 80.0f);
			ui_set_next_fixed_y(0.0f);
			if (ui_button(str("###maximize button")) & ui_interaction_left_clicked) {
				if (window->maximized) {
					os_window_restore(window);
				} else {
					os_window_maximize(window);
				}
			}

			ui_set_next_fixed_x(window->resolution.x - 120.0f);
			ui_set_next_fixed_y(0.0f);
			if (ui_button(str("###minimize button")) & ui_interaction_left_clicked) {
				os_window_minimize(window);
			}

		ui_end_frame(ui);
		
		
		{ // draw title bar
			
			// buttons
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

			// title
			draw_text(str("borderless"), vec2(7.0f, 7.0f));
		}



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

	// init
	app_init();

	// main loop
	while (!quit) {
		app_frame();
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