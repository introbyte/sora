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


// structs

struct node_t {
	node_t* next;
	node_t* prev;

	str_t label;
	vec2_t pos;
	ui_frame_t* frame;
	f32 value;
};

struct node_state_t {
	
	arena_t* arena;

	node_t* node_first;
	node_t* node_last;
	node_t* node_free;

};


// globals
global os_window_t* window;
global gfx_renderer_t* renderer;
global ui_context_t* ui;
global b8 quit = false;

global ui_panel_t* left;
global ui_panel_t* right;
global ui_panel_t* top;
global ui_panel_t* bottom;

global node_state_t node_state;

// functions

function void app_init();
function void app_release();
function void app_frame();

// node

// node state
function void node_state_init();
function void node_state_release();
function void node_state_render();

// node
function node_t* node_create(str_t label, vec2_t pos);
function void node_release(node_t* node);
function void node_bring_to_front(node_t* node);

// implementation

// node state

function void
node_state_init() {

	node_state.arena = arena_create(megabytes(64));

	node_state.node_first = nullptr;
	node_state.node_last = nullptr;
	node_state.node_free = nullptr;

}

function void
node_state_release() {
	arena_release(node_state.arena);
}

function void
node_state_render() {

	// get parent panel pos

	f32 panel_x = top->frame->rect.x0;
	f32 panel_y = top->frame->rect.y0;

	for (node_t* node = node_state.node_first; node != nullptr; node = node->next) {

		ui_set_next_fixed_x(panel_x + node->pos.x);
		ui_set_next_fixed_y(panel_y + node->pos.y);
		ui_set_next_fixed_width(150.0f);
		ui_set_next_pref_height(ui_size_by_child(1.0f));
		ui_set_next_rounding(8.0f);
		ui_set_next_color_group(ui_state.default_theme.popup);

		ui_key_t node_key = ui_key_from_stringf({0}, "%s_%p", node->label.data, node);
		ui_frame_flags flags =
			ui_frame_flag_draw_background |
			ui_frame_flag_draw_border |
			ui_frame_flag_draw_shadow |
			ui_frame_flag_floating |
			ui_frame_flag_clickable;
		ui_frame_t* frame = ui_frame_from_key(node_key, flags);

		ui_push_seed_key(node_key);
		ui_push_parent(frame);
		ui_push_pref_width(ui_size_percent(1.0f));
		ui_push_pref_height(ui_size_pixel(20.0f, 1.0f));

		ui_spacer();
		ui_label(node->label);
		ui_spacer();
		//ui_slider(str("slider"), &node->value, 0.0f, 10.0f);
		
		ui_row_begin();
		ui_push_pref_height(ui_size_percent(1.0f));

		ui_set_next_pref_width(ui_size_pixel(20.0f, 1.0f));
		ui_interaction button_1_interaction = ui_buttonf("*###button_1");
		if (button_1_interaction & ui_interaction_left_released) {
			printf("here!\n");
		}


		ui_set_next_pref_width(ui_size_percent(1.0f));
		ui_labelf("Label");

		ui_set_next_pref_width(ui_size_pixel(20.0f, 1.0f));
		ui_buttonf("*###button_2");

		ui_pop_pref_height();
		ui_row_end();
		
		
		
		ui_spacer();
	

		ui_pop_seed_key();
		ui_pop_parent();
		ui_pop_pref_width();
		ui_pop_pref_height();

		ui_interaction interaction = ui_frame_interaction(frame);
		
		if (interaction & ui_interaction_left_pressed) {
			node_bring_to_front(node);
		}

		if (interaction & ui_interaction_left_dragging) {

			node->pos.x += window->mouse_delta.x;
			node->pos.y += window->mouse_delta.y;

		}


	}


}

// node

function node_t* 
node_create(str_t label, vec2_t pos) {

	node_t* node = node_state.node_free;
	if (node != nullptr) {
		stack_pop(node_state.node_free);
	} else {
		node = (node_t*)arena_alloc(node_state.arena, sizeof(node_t));
	}
	memset(node, 0, sizeof(node_t));
	dll_push_back(node_state.node_first, node_state.node_last, node);

	node->label = label;
	node->pos = pos;
	node->value = random_f32_range(0.0f, 10.0f);
	
	return node;
}

function void
node_release(node_t* node) {
	dll_remove(node_state.node_first, node_state.node_last, node);
	stack_push(node_state.node_free, node);
}

function void 
node_bring_to_front(node_t* node) {

	dll_remove(node_state.node_first, node_state.node_last, node);
	dll_push_front(node_state.node_first, node_state.node_last, node);

}


function void
app_init() {

	// open window and create renderer
	window = os_window_open(str("ui_test"), 1280, 960);
	renderer = gfx_renderer_create(window, color(0x262626ff));
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


	// create nodes
	node_create(str("Node 1"), vec2(50.0f, 50.0f));
	node_create(str("Node 2"), vec2(250.0f, 80.0f));

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

		ui_frame_t* panel_frame = ui_panel_begin(top);
		{

			node_state_render();


			ui_interaction panel_interaction = ui_frame_interaction(panel_frame);

			if (panel_interaction & ui_interaction_middle_dragging) {
				panel_frame->view_offset_target.x -= window->mouse_delta.x;
				panel_frame->view_offset_target.y -= window->mouse_delta.y;
				os_set_cursor(os_cursor_resize_NS);
			}

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
	node_state_init();

	// init
	app_init();

	// main loop
	while (!quit) {
		app_frame();
	}

	// release
	app_release();

	// release layers
	node_state_release();
	ui_release();
	draw_release();
	font_release();
	gfx_release();
	os_release();

	printf("[info] successfully closed.\n");

	return 0;
}