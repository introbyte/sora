// ui_test.cpp

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

global ui_panel_t* left;
global ui_panel_t* right;
global ui_panel_t* top;
global ui_panel_t* bottom;

// functions

function void app_init();
function void app_release();
function void app_frame();

// implementation

function void
app_init() {

	// open window and create renderer
	window = os_window_open(str("ui_test"), 1280, 960);
	renderer = gfx_renderer_create(window, color(0x000000ff));
	ui = ui_context_create(renderer);

	// set frame function
	os_window_set_frame_function(window, app_frame);

	// create panels
	left = ui_panel_create(ui, 0.3f);
	right = ui_panel_create(ui, 0.7f, ui_axis_y);
	top = ui_panel_create(ui, 0.6f);
	bottom = ui_panel_create(ui, 0.4f);

	ui_panel_insert(right, bottom);
	ui_panel_insert(right, top);
	ui_panel_insert(ui->panel_root, right);
	ui_panel_insert(ui->panel_root, left);

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

		ui_panel_begin(left);
		{

			ui_push_pref_width(ui_size_percent(1.0f));
			ui_push_pref_height(ui_size_pixel(20.0f, 1.0f));

			// labels
			ui_labelf("This is a label.");
			ui_spacer();

			// buttons
			ui_interaction interaction = ui_buttonf("Button");
			if (interaction & ui_interaction_hovered) {
				ui_tooltip_begin();
				ui_set_next_pref_width(ui_size_text(2.0f));
				ui_labelf("This is a tooltip.");
				ui_tooltip_end();
			}

			ui_spacer();

			// checkbox
			persist b8 checkbox_value;
			ui_checkbox(str("Checkbox"), &checkbox_value);
			ui_spacer();

			// slider
			persist f32 slider_value = 0.35f;
			ui_slider(str("slider"), &ui->theme.rounding, 0.0f, 8.0f);
			ui_spacer();

			persist f32 float_edit_value = 10.75f;
			ui_float_edit(str("float_edit"), &float_edit_value);
			ui_spacer();

			// expander
			persist b8 expander_value = false;
			ui_expander(str("Color Pickers"), &expander_value);
			ui_spacer();
			persist color_t hsv_color = color(1.0f, 0.5f, 0.7f);
			if (expander_value) {

				ui_labelf("Color Wheel");
				ui_spacer();
				ui_set_next_pref_height(ui_size_pixel(150.0f, 1.0f));
				ui_color_wheel(str("color_wheel"), &hsv_color.h, &hsv_color.s, hsv_color.v);
				ui_spacer();
				ui_color_val_bar(str("color_val"), hsv_color.h, hsv_color.s, &hsv_color.v);

				ui_spacer();
				ui_labelf("Color Ring");
				ui_spacer();

				ui_set_next_pref_height(ui_size_pixel(150.0f, 1.0f));
				ui_color_ring(str("color_ring"), &hsv_color.h, &hsv_color.s, &hsv_color.v);


				ui_spacer();
				ui_labelf("Color Quad");
				ui_spacer();

				ui_set_next_pref_height(ui_size_pixel(150.0f, 1.0f));
				ui_color_quad(str("color_quad"), hsv_color.h, &hsv_color.s, &hsv_color.v);
				ui_color_hue_bar(str("hue_bar"), &hsv_color.h, hsv_color.s, hsv_color.v);

			}

			ui_pop_pref_width();
			ui_pop_pref_height();

		}
		ui_panel_end();


		ui_panel_begin(bottom);
		{
			ui_push_pref_width(ui_size_percent(1.0f));
			ui_push_pref_height(ui_size_pixel(20.0f, 1.0f));

			ui_set_next_text_alignment(ui_text_alignment_left);
			ui_buttonf("Left Button");
			ui_spacer();
			ui_set_next_text_alignment(ui_text_alignment_center);
			ui_buttonf("Center Button");
			ui_spacer();
			ui_set_next_text_alignment(ui_text_alignment_right);
			ui_buttonf("Right Button");
			ui_spacer();
			persist b8 value_0 = false;
			persist b8 value_1 = true;
			persist b8 value_2 = false;
			ui_checkbox(str("Some Option 1"), &value_0);
			ui_spacer();
			ui_checkbox(str("Some Option 2"), &value_1);
			ui_spacer();
			ui_checkbox(str("Some Option 3"), &value_2);


			persist i32 current_selection = 2;
			char* items[] = {
				"Item 1", "Item 2", "Item 3",
			};
			ui_combo(str("Combo Box"), &current_selection, items, 3);

			ui_pop_pref_width();
			ui_pop_pref_height();
		}
		ui_panel_end();

		ui_panel_begin(top);
		{
			
		}
		ui_panel_end();

		ui_end_frame(ui);
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