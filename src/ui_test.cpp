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

struct connection_t {
	connection_t* next;
	connection_t* prev;

	vec2_t from_pos;
	vec2_t to_pos;

	node_t* from;
	node_t* to;

	ui_frame_t* frame;
};

struct node_state_t {
	
	arena_t* arena;

	node_t* node_first;
	node_t* node_last;
	node_t* node_free;
	u32 node_count;

	connection_t* connection_first;
	connection_t* connection_last;
	connection_t* connection_free;
	u32 connection_count;

};

// globals
global os_handle_t window;
global gfx_handle_t renderer;
global ui_context_t* ui;
global b8 quit = false;

global ui_panel_t* widgets_panel;
global ui_panel_t* node_graph_panel;
global ui_panel_t* console_panel;
global ui_panel_t* properties_panel;


global ui_view_t* view;

global node_state_t node_state;

// functions

// node state
function void node_state_init();
function void node_state_release();

// node
function node_t* node_create(str_t label, vec2_t pos);
function void node_release(node_t* node);
function void node_bring_to_front(node_t* node);

// connection
function connection_t* connection_create(node_t* from, node_t* to);
function void connection_release(connection_t* connection);

// ui custom draw
function void node_background_draw_function(ui_frame_t* frame);
function void connection_draw_function(ui_frame_t* frame);
function void port_draw_function(ui_frame_t* frame);

// app
function void app_init();
function void app_release();
function void app_frame();

// implementation

// node state

function void
node_state_init() {

	node_state.arena = arena_create(megabytes(64));

	node_state.node_first = nullptr;
	node_state.node_last = nullptr;
	node_state.node_free = nullptr;
	node_state.node_count = 0;

	node_state.connection_first = nullptr;
	node_state.connection_last = nullptr;
	node_state.connection_free = nullptr;
	node_state.connection_count = 0;

}

function void
node_state_release() {
	arena_release(node_state.arena);
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
	node_state.node_count++;


	node->label = label;
	node->pos = pos;
	node->value = random_f32_range(0.0f, 10.0f);
	
	return node;
}

function void
node_release(node_t* node) {
	dll_remove(node_state.node_first, node_state.node_last, node);
	stack_push(node_state.node_free, node);
	node_state.node_count--;
}

function void 
node_bring_to_front(node_t* node) {

	dll_remove(node_state.node_first, node_state.node_last, node);
	dll_push_front(node_state.node_first, node_state.node_last, node);

}

// connection

function connection_t* 
connection_create(node_t* from, node_t* to) {

	// grab from free list of allocate one.
	connection_t* connection = node_state.connection_free;
	if (connection != nullptr) {
		stack_pop(node_state.connection_free);
	} else {
		connection = (connection_t*)arena_alloc(node_state.arena, sizeof(connection_t));
	}
	memset(connection, 0, sizeof(connection_t));

	// fill
	connection->from = from;
	connection->to = to;

	// add to list
	dll_push_back(node_state.connection_first, node_state.connection_last, connection);
	node_state.connection_count++;

	return connection;
}

function void 
connection_release(connection_t* connection) {
	dll_remove(node_state.connection_first, node_state.connection_last, connection);
	stack_push(node_state.connection_free, connection);
	node_state.connection_count--;
}

// ui

function void
widget_view(ui_view_t* view) {

	ui_padding_begin(8.0f);

	ui_push_pref_width(ui_size_percent(1.0f));
	ui_push_pref_height(ui_size_pixel(20.0f, 1.0f));

	// labels
	persist b8 labels_expanded = false;
	ui_expander(str("Labels"), &labels_expanded);
	if (labels_expanded) {
		ui_spacer();
		ui_labelf("This is a label.");

		ui_set_next_color_var(ui_color_text, color(0xef8080ff));
		ui_labelf("This is a colored label.");

		ui_set_next_text_alignment(ui_text_alignment_center);
		ui_labelf("center aligned");

		ui_set_next_text_alignment(ui_text_alignment_right);
		ui_labelf("right aligned");

	}
	ui_spacer();

	// buttons
	persist b8 buttons_expanded = false;
	ui_expander(str("Buttons"), &buttons_expanded);
	if (buttons_expanded) {
		ui_spacer();
		ui_set_next_text_alignment(ui_text_alignment_left);
		ui_buttonf("Left Button");
		ui_spacer();
		ui_set_next_text_alignment(ui_text_alignment_center);
		ui_buttonf("Center Button");
		ui_spacer();
		ui_set_next_text_alignment(ui_text_alignment_right);
		ui_buttonf("Right Button");
		ui_spacer();

		ui_interaction interaction = ui_buttonf("Button with tooltip");
		ui_spacer();
		if (interaction & ui_interaction_hovered) {
			ui_tooltip_begin();
			ui_set_next_pref_width(ui_size_text(2.0f));
			ui_labelf("This is a tooltip.");
			ui_tooltip_end();
		}

		ui_set_next_color_var(ui_color_background, color(0x203556ff));
		ui_buttonf("Colored Button");
		ui_spacer();

		ui_set_next_pref_height(ui_size_pixel(100.0f, 1.0f));
		ui_set_next_text_alignment(ui_text_alignment_center);
		ui_set_next_color_var(ui_color_background, color(0x563520ff));
		ui_buttonf("Large Button!");
	}
	ui_spacer();

	// checkboxes
	persist b8 checkboxes_expanded = false;
	ui_expander(str("Checkboxes"), &checkboxes_expanded);
	if (checkboxes_expanded) {
		ui_spacer();
		persist b8 checkbox_value = false;
		ui_checkbox(str("Checkbox"), &checkbox_value);
	}
	ui_spacer();

	// sliders
	persist b8 sliders_expanded = false;
	ui_expander(str("Sliders"), &sliders_expanded);
	if (sliders_expanded) {
		ui_spacer();
		persist f32 slider_f32_value = 0.35f;
		ui_slider(str("Float Slider: "), &slider_f32_value, 0.0f, 8.0f);

		ui_spacer();
		persist i32 slider_i32_value = 10;
		ui_slider(str("Int Slider: "), &slider_i32_value, 5, 15);

	}
	ui_spacer();

	// float edits
	persist b8 float_edits_expanded = false;
	ui_expander(str("Float Edits"), &float_edits_expanded);
	if (float_edits_expanded) {
		ui_spacer();
		persist f32 float_edit_value = 10.75f;
		ui_float_edit(str("float_edit"), &float_edit_value, 0.01f);
		ui_spacer();

		persist vec2_t vec2_edit_value = vec2(1.0f, 2.0f);
		ui_vec2_edit(str("vec2_edit"), &vec2_edit_value);
		ui_spacer();

		persist vec3_t vec3_edit_value = vec3(1.0f, 2.0f, 3.0f);
		ui_vec3_edit(str("vec3_edit"), &vec3_edit_value);
		ui_spacer();

		persist vec4_t vec4_edit_value = vec4(1.0f, 2.0f, 3.0f, 4.0f);
		ui_vec4_edit(str("vec4_edit"), &vec4_edit_value);
		ui_spacer();

		// custom vec3 widget
		ui_labelf("Custom Vec3 Widget");
		ui_spacer();

		{
			ui_row_begin();
			ui_push_pref_width(ui_size_percent(1.0f));


			ui_push_color_var(ui_color_border, color(0xa01f1cff));
			ui_float_edit(str("custom_float_edit_x"), &vec3_edit_value.x, 0.01f);
			ui_pop_color_var(ui_color_border);

			ui_spacer();

			ui_push_color_var(ui_color_border, color(0x357f2fff));
			ui_float_edit(str("custom_float_edit_y"), &vec3_edit_value.y, 0.01f);
			ui_pop_color_var(ui_color_border);

			ui_spacer();

			ui_push_color_var(ui_color_border, color(0x263593ff));
			ui_float_edit(str("custom_float_edit_z"), &vec3_edit_value.z, 0.01f);
			ui_pop_color_var(ui_color_border);

			ui_pop_pref_width();

			ui_row_end();
		}


	}
	ui_spacer();

	// expander
	persist b8 color_pickers_expanded = false;
	ui_expander(str("Color Pickers"), &color_pickers_expanded);
	if (color_pickers_expanded) {
		ui_spacer();
		persist color_t hsv_color = color(1.0f, 0.5f, 0.7f);

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
	ui_spacer();

	ui_pop_pref_width();
	ui_pop_pref_height();

	ui_padding_end();
}

function void
debug_view(ui_view_t* view) {

	ui_padding_begin(8.0f);

	ui_push_pref_size(ui_size_percent(1.0f), ui_size_pixel(20.0f, 1.0f));

	persist b8 debug_option_expanded = false;
	ui_expander(str("UI Frame Debug Options"), &debug_option_expanded);
	if (debug_option_expanded) {
		ui_spacer();
		ui_checkbox(str("Frame Outline"), &ui_debug_frame);
		ui_spacer();
		ui_checkbox(str("Text Outline"), &ui_debug_text);
		ui_spacer();
		ui_checkbox(str("Show Hovered"), &ui_show_hovered);
		ui_spacer();
		ui_checkbox(str("Show Active"), &ui_show_active);
		ui_spacer();
		ui_checkbox(str("Show Focused"), &ui_show_focused);
	}

	ui_spacer();
	persist b8 frame_styling_expanded = false;
	ui_expander(str("UI Frame Styling Options"), &frame_styling_expanded);
	if (frame_styling_expanded) {
		ui_spacer();
		ui_checkbox(str("Frame Borders"), &ui_state.ui_active->theme.frame_borders);
		ui_spacer();
		ui_checkbox(str("Frame Shadows"), &ui_state.ui_active->theme.frame_shadows);
		ui_spacer();
		ui_checkbox(str("Text Shadows"), &ui_state.ui_active->theme.text_shadows);
		ui_spacer();
		ui_slider(str("Frame Rounding"), &ui_state.ui_active->theme.rounding, 0.0f, 10.0f);

		color_t hsv_color = color_rgb_to_hsv(ui_active()->theme.pallete.accent);
		ui_set_next_pref_height(ui_size_pixel(150.0f, 1.0f));
		ui_color_wheel(str("Accent color_wheel"), &hsv_color.h, &hsv_color.s, hsv_color.v);
		ui_color_val_bar(str("Accent val_bar"), hsv_color.h, hsv_color.s, &hsv_color.v);
		ui_active()->theme.pallete.accent = color_hsv_to_rgb(hsv_color);

		ui_labelf("Accent Color: %x", color_to_hex(ui_active()->theme.pallete.accent));
	}

	ui_pop_pref_size();

	ui_padding_end();
}

function void
node_graph_view(ui_view_t* view) {

	ui_set_next_pref_size(ui_size_percent(1.0f), ui_size_percent(1.0f));
	ui_frame_t* background_frame = ui_frame_from_string(str("node_background_frame"), ui_frame_flag_clickable | ui_frame_flag_draw_custom | ui_frame_flag_clip);
	ui_frame_set_custom_draw(background_frame, node_background_draw_function, nullptr);

	ui_push_parent(background_frame);

	ui_push_pref_size(ui_size_pixel(150.0f, 1.0f), ui_size_pixel(20.0f, 1.0f));
	ui_push_flags(ui_frame_flag_ignore_view_scroll);
	ui_labelf("node count: %u", node_state.node_count);
	ui_labelf("connection count: %u", node_state.connection_count);
	ui_pop_pref_size();
	ui_pop_flags();


	// get parent panel pos
	f32 panel_x = node_graph_panel->frame->rect.x0;
	f32 panel_y = node_graph_panel->frame->rect.y0;

	persist node_t* begin_node = nullptr;
	persist node_t* end_node = nullptr;

	// render nodes
	for (node_t* node = node_state.node_first; node != nullptr; node = node->next) {

		ui_set_next_fixed_x(panel_x + node->pos.x);
		ui_set_next_fixed_y(panel_y + node->pos.y);
		ui_set_next_fixed_width(150.0f);
		ui_set_next_pref_height(ui_size_by_child(1.0f));
		ui_set_next_rounding(8.0f);

		ui_set_next_shadow_size(5.0f);
		ui_key_t node_key = ui_key_from_stringf({ 0 }, "%s_%p", node->label.data, node);
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
		persist b8 dragging = false;

		ui_row_begin();
		ui_push_pref_height(ui_size_percent(1.0f));
		ui_push_text_alignment(ui_text_alignment_center);
		{

			ui_interaction button_interaction = 0;

			// custom port
			ui_set_next_pref_width(ui_size_pixel(20.0f, 1.0f));
			ui_key_t port_1_key = ui_key_from_stringf({ 0 }, "###%p_port_1", node);
			ui_frame_t* port_1 = ui_frame_from_key(port_1_key, ui_frame_flag_clickable | ui_frame_flag_draw_custom);
			ui_frame_set_custom_draw(port_1, port_draw_function, nullptr);
			button_interaction |= ui_frame_interaction(port_1);

			ui_set_next_pref_width(ui_size_percent(1.0f));
			ui_labelf("Label");

			// custom port
			ui_set_next_pref_width(ui_size_pixel(20.0f, 1.0f));
			ui_key_t port_2_key = ui_key_from_stringf({ 0 }, "###%p_port_2", node);
			ui_frame_t* port_2 = ui_frame_from_key(port_2_key, ui_frame_flag_clickable | ui_frame_flag_draw_custom);
			ui_frame_set_custom_draw(port_2, port_draw_function, nullptr);
			button_interaction |= ui_frame_interaction(port_2);

			// button interaction
			if (button_interaction & ui_interaction_left_pressed) {
				//ui_kill_action();
				//dragging = true;
				//begin_node = node;
			}

			if (button_interaction & ui_interaction_left_released) {
				//if (dragging) {
				//	end_node = node;
				//	dragging = false;

				//	if (begin_node != nullptr && end_node != nullptr && begin_node != end_node) {
				//		connection_create(begin_node, end_node);
				//	}

				//	begin_node = nullptr;
				//	end_node = nullptr;
				//}
			}
		}
		ui_pop_text_alignment();
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
			vec2_t mouse_delta = os_window_get_mouse_delta(window);
			node->pos = vec2_add(node->pos, mouse_delta);
		}

	}

	// render connection
	for (connection_t* connection = node_state.connection_first; connection != nullptr; connection = connection->next) {

		// update connection pos
		connection->from_pos = connection->from->pos;
		connection->to_pos = connection->to->pos;

		ui_key_t frame_key = ui_key_from_stringf({ 0 }, "%p_connection", connection);

		vec2_t min_pos = vec2_min(connection->from_pos, connection->to_pos);
		vec2_t max_pos = vec2_max(connection->from_pos, connection->to_pos);

		vec2_t size = vec2(max_pos.x - min_pos.x, max_pos.y - min_pos.y);

		ui_set_next_fixed_x(panel_x + min_pos.x);
		ui_set_next_fixed_y(panel_y + min_pos.y);
		ui_set_next_fixed_width(size.x);
		ui_set_next_fixed_height(size.y);
		ui_frame_t* frame = ui_frame_from_key(frame_key, ui_frame_flag_clickable | ui_frame_flag_draw_custom | ui_frame_flag_floating);
		ui_frame_set_custom_draw(frame, connection_draw_function, nullptr);

	}

	ui_pop_parent();

	ui_interaction panel_interaction = ui_frame_interaction(background_frame);

	if (panel_interaction & ui_interaction_middle_dragging) {
		vec2_t mouse_delta = os_window_get_mouse_delta(window);
		background_frame->view_offset_target.x -= mouse_delta.x;
		background_frame->view_offset_target.y -= mouse_delta.y;
		os_set_cursor(os_cursor_resize_all);
	}

}

function void
properties_view(ui_view_t* view) {

	ui_padding_begin(8.0f);

	

	ui_padding_end();
}

// ui custom draw

function void 
node_background_draw_function(ui_frame_t* frame) {
	
	// draw background
	draw_set_next_color(color(0x232323ff));
	draw_rect(frame->rect);

	f32 frame_width = rect_width(frame->rect);

	// draw grid
	/*f32 offset_x = fmodf(frame->view_offset_last.x, 50.0f);
	for (f32 x = frame->rect.x0 - offset_x; x < frame->rect.x1 - offset_x + 50.0f; x += 50.0f) {
		f32 round_x = roundf(x);
		rect_t line = rect(round_x, frame->rect.y0, round_x + 1.0f, frame->rect.y1);
		draw_set_next_color(color(0x303030ff));
		draw_rect(line);
	}

	f32 offset_y = fmodf(frame->view_offset_last.y, 50.0f);
	for (f32 y = frame->rect.y0 - offset_y; y < frame->rect.y1 - offset_y + 50.0f; y += 50.0f) {
		f32 round_y = roundf(y);
		rect_t line = rect(frame->rect.x0, round_y, frame->rect.x1, round_y + 1.0f);
		draw_set_next_color(color(0x303030ff));
		draw_rect(line);
	}*/

	f32 offset_x = fmodf(frame->view_offset_last.x, 25.0f);
	f32 offset_y = fmodf(frame->view_offset_last.y, 25.0f);
	for (f32 x = frame->rect.x0 - offset_x; x < frame->rect.x1 - offset_x + 25.0f; x += 25.0f) {
		for (f32 y = frame->rect.y0 - offset_y; y < frame->rect.y1 - offset_y + 25.0f; y += 25.0f) {
			f32 radius = 1.25f;
			draw_set_next_color(color(0x303030ff));
			draw_circle(vec2(x, y), radius, 0.0f, 360.0f);
		}
	}


}

function void
connection_draw_function(ui_frame_t* frame) {
	
	vec2_t p0 = frame->rect.v0;
	vec2_t p1 = frame->rect.v1;
	vec2_t c0 = vec2_add(p0, vec2(50.0f, 0.0f));
	vec2_t c1 = vec2_add(p1, vec2(-50.0f, 0.0f));

	draw_push_color(frame->palette.accent);
	draw_push_thickness(2.5f);
	draw_push_softness(0.5f);
	draw_bezier(p0, p1, c0, c1);
	draw_pop_softness();
	draw_pop_thickness();
	draw_pop_color();

}

function void 
port_draw_function(ui_frame_t* frame) {

	vec2_t center = rect_center(frame->rect);
	
	// animate radius
	f32 radius = roundf((min(rect_width(frame->rect), rect_height(frame->rect)) * 0.5f) - 2.0f);
	radius = lerp(radius, radius + 1.0f, frame->hover_t);
	f32 inner_radius = lerp(0.0f, radius - 5.0f, frame->active_t);

	// animate color
	color_t port_color = frame->palette.accent;
	port_color = color_lerp(port_color, color_blend(port_color, frame->palette.hover), frame->hover_t);
	port_color = color_lerp(port_color, color_blend(port_color, frame->palette.active), frame->active_t);

	color_t inner_port_color = color_lerp(color(port_color.r, port_color.g, port_color.b, 0.0f), port_color, frame->active_t);

	// draw
	draw_set_next_color(port_color);
	draw_set_next_thickness(3.0f);
	draw_set_next_softness(0.75f);
	draw_circle(center, radius, 0.0f, 360.0f);

	// inner
	draw_set_next_color(inner_port_color);
	draw_set_next_softness(0.5f);
	draw_circle(center, inner_radius, 0.0f, 360.0f);

}

// app

function void
app_init() {

	// open window and create renderer
	window = os_window_open(str("ui_test"), 1440, 960);
	renderer = gfx_renderer_create(window, color(0x262626ff));
	ui = ui_context_create(window, renderer);

	// set frame function
	os_window_set_frame_function(window, app_frame);

	// create panels
	widgets_panel = ui_panel_create(ui, 0.2f);
	ui_panel_t* center = ui_panel_create(ui, 0.6f, ui_axis_y);
	properties_panel = ui_panel_create(ui, 0.2f);
	node_graph_panel = ui_panel_create(ui, 0.7f);
	console_panel = ui_panel_create(ui, 0.3f);

	// insert into tree
	ui_panel_insert(center, console_panel);
	ui_panel_insert(center, node_graph_panel);
	ui_panel_insert(ui->panel_root, properties_panel);
	ui_panel_insert(ui->panel_root, center);
	ui_panel_insert(ui->panel_root, widgets_panel);

	// create views
	ui_view_t* view1 = ui_view_create(ui, str("Properties"), properties_view);
	ui_view_t* view2 = ui_view_create(ui, str("Widgets"), widget_view);
	ui_view_t* view3 = ui_view_create(ui, str("Debug"), debug_view);
	ui_view_t* view4 = ui_view_create(ui, str("Node Graph"), node_graph_view);

	// insert views into panels
	ui_panel_insert_view(properties_panel, view1);
	ui_panel_insert_view(widgets_panel, view2);
	ui_panel_insert_view(console_panel, view3);
	ui_panel_insert_view(node_graph_panel, view4);

	// create nodes
	node_create(str("Node 1"), vec2(220.0f, 200.0f));
	node_create(str("Node 2"), vec2(450.0f, 280.0f));

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
	if (!gfx_handle_equals(renderer, { 0 })) {
		temp_t scratch = scratch_begin();

		gfx_renderer_begin(renderer);
		draw_begin(renderer);
		ui_begin_frame(ui);


		ui_end_frame(ui);
		draw_end(renderer);
		gfx_renderer_end(renderer);

		scratch_end(scratch);
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