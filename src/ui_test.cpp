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

// projects

#include "projects/graphs/force_graph.h"
#include "projects/graphs/node_graph.h"

#include "projects/graphs/force_graph.cpp"
#include "projects/graphs/node_graph.cpp"

// globals

// contexts
global os_handle_t window;
global gfx_handle_t renderer;
global ui_context_t* ui;
global b8 quit = false;

// graphs
global ng_state_t* node_state;
global fg_state_t* fg_state;

// functions

function void app_init();
function void app_release();
function void app_frame();

// views
function void widget_view(ui_view_t* view);
function void debug_view(ui_view_t* view);
function void node_graph_view(ui_view_t* view);
function void properties_view(ui_view_t* view);
function void console_view(ui_view_t* view);

// implementation

// ui

function void
widget_view(ui_view_t* view) {

	ui_padding_begin(ui_size_pixel(8.0f, 1.0f));

	ui_push_pref_width(ui_size_percent(1.0f));
	ui_push_pref_height(ui_size_pixel(20.0f, 1.0f));

	// labels
	persist b8 labels_expanded = false;
	ui_expander_begin(str("Labels"), &labels_expanded);
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
	ui_expander_end();
	ui_spacer();

	// buttons
	persist b8 buttons_expanded = false;
	ui_expander_begin(str("Buttons"), &buttons_expanded);
	if (buttons_expanded) {
		ui_padding_begin(ui_size_pixel(4.0f, 1.0f));
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
		ui_padding_end(ui_size_pixel(4.0f, 1.0f));
	}
	ui_expander_end();
	ui_spacer();

	// checkboxes
	persist b8 checkboxes_expanded = false;
	ui_expander_begin(str("Checkboxes"), &checkboxes_expanded);
	if (checkboxes_expanded) {
		ui_padding_begin(ui_size_pixel(4.0f, 1.0f));
		persist b8 checkbox_value = false;
		ui_checkbox(str("Checkbox"), &checkbox_value);
		ui_padding_end(ui_size_pixel(4.0f, 1.0f));
	}
	ui_expander_end();
	ui_spacer();

	// sliders
	persist b8 sliders_expanded = false;
	ui_expander_begin(str("Sliders"), &sliders_expanded);
	if (sliders_expanded) {
		ui_padding_begin(ui_size_pixel(4.0f, 1.0f));
		persist f32 slider_f32_value = 0.35f;
		ui_slider(str("Float Slider: "), &slider_f32_value, 0.0f, 8.0f);

		ui_spacer();
		persist i32 slider_i32_value = 10;
		ui_slider(str("Int Slider: "), &slider_i32_value, 5, 15);

		ui_padding_end(ui_size_pixel(4.0f, 1.0f));
	}
	ui_expander_end();
	ui_spacer();

	// float edits
	persist b8 float_edits_expanded = false;
	ui_expander_begin(str("Float Edits"), &float_edits_expanded);
	if (float_edits_expanded) {
		ui_padding_begin(ui_size_pixel(4.0f, 1.0f));

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

		ui_padding_end(ui_size_pixel(4.0f, 1.0f));
	}
	ui_expander_end();
	ui_spacer();

	// expander
	persist b8 color_pickers_expanded = false;
	ui_expander_begin(str("Color Pickers"), &color_pickers_expanded);
	if (color_pickers_expanded) {
		ui_padding_begin(ui_size_pixel(4.0f, 1.0f));
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
		ui_padding_end(ui_size_pixel(4.0f, 1.0f));
	}
	ui_expander_end();
	ui_spacer();

	ui_pop_pref_width();
	ui_pop_pref_height();

	ui_padding_end(ui_size_pixel(8.0f, 1.0f));
}

function void
debug_view(ui_view_t* view) {

	ui_padding_begin(ui_size_pixel(8.0f, 1.0f));

	ui_push_pref_size(ui_size_percent(1.0f), ui_size_pixel(20.0f, 1.0f));

	persist b8 debug_option_expanded = false;
	ui_expander_begin(str("UI Frame Debug Options"), &debug_option_expanded);
	if (debug_option_expanded) {
		ui_padding_begin(ui_size_pixel(4.0f, 1.0f));
		ui_checkbox(str("Frame Outline"), &ui_debug_frame);
		ui_spacer();
		ui_checkbox(str("Text Outline"), &ui_debug_text);
		ui_spacer();
		ui_checkbox(str("Show Hovered"), &ui_show_hovered);
		ui_spacer();
		ui_checkbox(str("Show Active"), &ui_show_active);
		ui_spacer();
		ui_checkbox(str("Show Focused"), &ui_show_focused);
		ui_padding_end(ui_size_pixel(4.0f, 1.0f));
	}
	ui_expander_end();
	ui_spacer();

	persist b8 frame_styling_expanded = false;
	ui_expander_begin(str("UI Frame Styling Options"), &frame_styling_expanded);
	if (frame_styling_expanded) {
		ui_padding_begin(ui_size_pixel(4.0f, 1.0f));
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
		ui_padding_end(ui_size_pixel(4.0f, 1.0f));
	}
	ui_expander_end();

	ui_pop_pref_size();

	ui_padding_end(ui_size_pixel(8.0f, 1.0f));
}

function void
node_graph_view(ui_view_t* view) {

	ui_frame_flags background_flags = 
		ui_frame_flag_clickable | 
		ui_frame_flag_draw_background |
		ui_frame_flag_draw_custom | 
		ui_frame_flag_clip;

	ui_set_next_pref_size(ui_size_percent(1.0f), ui_size_percent(1.0f));
	ui_set_next_color_background(color(0x131313ff));
	ui_frame_t* background_frame = ui_frame_from_string(str("node_background_frame"), background_flags);
	ui_frame_set_custom_draw(background_frame, ng_node_background_draw_function, nullptr);

	ui_push_parent(background_frame);

	// labels
	{
		/*ui_push_flags(ui_frame_flag_ignore_view_scroll);
		ui_push_pref_size(ui_size_pixel(150.0f, 1.0f), ui_size_pixel(20.0f, 1.0f));
		ui_labelf("node count: %u", node_state->node_count);
		ui_labelf("connection count: %u", node_state->connection_count);
		ui_pop_pref_size();
		ui_pop_flags();*/
	}

	// get parent panel pos

	persist ng_node_t* begin_node = nullptr;
	persist ng_node_t* end_node = nullptr;

	// render nodes
	for (ng_node_t* node = node_state->node_first; node != nullptr; node = node->next) {

		ui_set_next_fixed_x(node->pos.x);
		ui_set_next_fixed_y(node->pos.y);
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
			ui_frame_set_custom_draw(port_1, ng_port_draw_function, nullptr);
			button_interaction |= ui_frame_interaction(port_1);

			ui_set_next_pref_width(ui_size_percent(1.0f));
			ui_labelf("Label");

			// custom port
			ui_set_next_pref_width(ui_size_pixel(20.0f, 1.0f));
			ui_key_t port_2_key = ui_key_from_stringf({ 0 }, "###%p_port_2", node);
			ui_frame_t* port_2 = ui_frame_from_key(port_2_key, ui_frame_flag_clickable | ui_frame_flag_draw_custom);
			ui_frame_set_custom_draw(port_2, ng_port_draw_function, nullptr);
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
			ng_node_bring_to_front(node_state, node);
		}

		if (interaction & ui_interaction_left_dragging) {
			vec2_t mouse_delta = os_window_get_mouse_delta(window);
			node->pos = vec2_add(node->pos, mouse_delta);
		}

	}

	// render connection
	for (ng_connection_t* connection = node_state->connection_first; connection != nullptr; connection = connection->next) {

		// update connection pos
		connection->from_pos = connection->from->pos;
		connection->to_pos = connection->to->pos;

		ui_key_t frame_key = ui_key_from_stringf({ 0 }, "%p_connection", connection);

		vec2_t min_pos = vec2_min(connection->from_pos, connection->to_pos);
		vec2_t max_pos = vec2_max(connection->from_pos, connection->to_pos);

		vec2_t size = vec2(max_pos.x - min_pos.x, max_pos.y - min_pos.y);

		ui_set_next_fixed_x(min_pos.x);
		ui_set_next_fixed_y(min_pos.y);
		ui_set_next_fixed_width(size.x);
		ui_set_next_fixed_height(size.y);
		ui_frame_t* frame = ui_frame_from_key(frame_key, ui_frame_flag_clickable | ui_frame_flag_draw_custom | ui_frame_flag_floating);
		ui_frame_set_custom_draw(frame, ng_connection_draw_function, nullptr);

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

	ui_padding_begin(ui_size_pixel(8.0f, 1.0f));
	ui_push_pref_size(ui_size_percent(1.0f), ui_size_pixel(20.0f, 1.0f));


	persist b8 force_graph_params_expanded = false;
	ui_expander_begin(str("Force Graph Params"), &force_graph_params_expanded);
	if (force_graph_params_expanded) {
		ui_padding_begin(ui_size_pixel(4.0f, 1.0f));


		ui_float_edit(str("link_size"), &fg_state->link_size, 0.1f, 1.0f, 200.0f);
		ui_spacer();
		ui_float_edit(str("link_strength"), &fg_state->link_strength, 0.1f, 1.0f, 200.0f);
		ui_spacer();
		ui_float_edit(str("repulsive_strength"), &fg_state->repulsive_strength, 0.1f, 1.0f, 5000.0f);
		ui_spacer();
		ui_float_edit(str("damping"), &fg_state->damping, 0.01f, 0.01f, 1.0f);

		ui_padding_end(ui_size_pixel(4.0f, 1.0f));
	}
	ui_expander_end();

	ui_pop_pref_size();
	ui_padding_end(ui_size_pixel(8.0f, 1.0f));
}

function void
console_view(ui_view_t* view) {

	ui_padding_begin(ui_size_pixel(8.0f, 1.0f));
	ui_set_next_pref_size(ui_size_percent(1.0f), ui_size_percent(1.0f));

	ui_pop_pref_size();
	ui_padding_end(ui_size_pixel(8.0f, 1.0f));
}

function void
force_graph_view(ui_view_t* view) {
	
	ui_frame_flags background_flags =
		ui_frame_flag_clickable |
		ui_frame_flag_draw_background |
		ui_frame_flag_draw_custom |
		ui_frame_flag_clip;

	ui_set_next_pref_size(ui_size_percent(1.0f), ui_size_percent(1.0f));
	ui_set_next_color_background(color(0x131313ff));
	ui_key_t background_key = ui_key_from_stringf({ 0 }, "%p_force_graph_background", view);
	ui_frame_t* background_frame = ui_frame_from_key(background_key, background_flags);

	ui_push_parent(background_frame);

	// draw nodes
	for (fg_node_t* node = fg_state->node_first; node != nullptr; node = node->next) {

		ui_frame_flags node_flags = 
			ui_frame_flag_clickable | 
			ui_frame_flag_draw_custom | 
			ui_frame_flag_floating;
		
		f32 node_size = node->size;
		ui_set_next_rect(rect(node->pos.x - node_size, node->pos.y - node_size, node->pos.x + node_size, node->pos.y + node_size));
		ui_key_t node_key = ui_key_from_stringf(background_key, "%p_frame", node);
		ui_frame_t* node_frame = ui_frame_from_key(node_key, node_flags);
		node->frame = node_frame;
		ui_frame_set_custom_draw(node_frame, fg_node_custom_draw, nullptr);

		ui_interaction node_interaction = ui_frame_interaction(node_frame);

		if (node_interaction & ui_interaction_left_dragging) {
			fg_state->node_active = node;
			vec2_t mouse_pos = os_window_get_cursor_pos(window);
			node->pos.x = mouse_pos.x - background_frame->rect.x0 + background_frame->view_offset.x;
			node->pos.y = mouse_pos.y - background_frame->rect.y0 + background_frame->view_offset.y;
		}

	}

	// draw connections
	for (fg_link_t* link = fg_state->link_first; link != nullptr; link = link->next) {

		// calculate points
		vec2_t from = vec2(0.0f);
		vec2_t to = vec2(0.0f);
		if (link->from->frame != nullptr && link->to->frame != nullptr) {
			from = rect_center(link->from->frame->rect);
			to = rect_center(link->to->frame->rect);
		}

		// calculate bounding rect
		vec2_t points[2] = { from, to };
		rect_t link_rect = rect_grow(rect_bbox(points, 2), 2.0f);

		ui_set_next_rect(link_rect);
		ui_key_t node_key = ui_key_from_stringf(background_key, "%p_frame", link);
		ui_frame_t* link_frame = ui_frame_from_key(node_key, ui_frame_flag_draw_custom | ui_frame_flag_floating);
		fg_link_draw_data_t* data = (fg_link_draw_data_t*)arena_alloc(ui_active()->per_frame_arena, sizeof(fg_link_draw_data_t));
		ui_frame_set_custom_draw(link_frame, fg_link_custom_draw, data);

		data->from = from;
		data->to = to;
	}
	
	ui_pop_parent();

	ui_interaction background_interaction = ui_frame_interaction(background_frame);
	if (background_interaction & ui_interaction_middle_dragging) {
		vec2_t mouse_delta = os_window_get_mouse_delta(window);
		background_frame->view_offset_target.x -= mouse_delta.x;
		background_frame->view_offset_target.y -= mouse_delta.y;
		os_set_cursor(os_cursor_resize_all);
	}

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

	// create panels and view
	{
		// create panels
		ui_panel_t* widgets_panel = ui_panel_create(ui, 0.2f);
		ui_panel_t* center = ui_panel_create(ui, 0.6f, ui_axis_y);
		ui_panel_t* properties_panel = ui_panel_create(ui, 0.2f);
		ui_panel_t* node_graph_panel = ui_panel_create(ui, 0.7f);
		ui_panel_t* console_panel = ui_panel_create(ui, 0.3f);

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
		ui_view_t* view5 = ui_view_create(ui, str("Console"), console_view);
		ui_view_t* view6 = ui_view_create(ui, str("Force Graph"), force_graph_view);

		// insert views into panels
		//ui_panel_insert_view(properties_panel, view1);
		ui_panel_insert_view(widgets_panel, view2);
		ui_panel_insert_view(console_panel, view3);
		ui_panel_insert_view(console_panel, view5);
		ui_panel_insert_view(node_graph_panel, view4);
		ui_panel_insert_view(node_graph_panel, view6);
		ui_panel_insert_view(properties_panel , view1);


	}
	
	// create node graph
	{
		node_state = ng_init();

		// create nodes
		ng_node_create(node_state, str("Node 1"), vec2(220.0f, 200.0f));
		ng_node_create(node_state, str("Node 2"), vec2(450.0f, 280.0f));


	}

	// create force graph
	{
		fg_state = fg_create();
		
		vec2_t pos = vec2(random_f32_range(50.0f, 500.0f), random_f32_range(50.0f, 500.0f));
		fg_node_t* root = fg_node_create(fg_state, pos, 15.0f);

		u32 random_value_i = random_u32_range(5, 15);
		for (u32 i = 0; i < random_value_i; i++) {

			// create node
			vec2_t pos_i = vec2(random_f32_range(50.0f, 500.0f), random_f32_range(50.0f, 500.0f));
			fg_node_t* node_i = fg_node_create(fg_state, pos_i);
			fg_link_create(fg_state, root, node_i, 100.0f);

			// create random children
			u32 random_value_j = random_u32_range(0, 4);
			for (u32 j = 0; j < random_value_j; j++) {
				vec2_t pos_j = vec2(random_f32_range(50.0f, 500.0f), random_f32_range(50.0f, 500.0f));
				fg_node_t* node_j = fg_node_create(fg_state, pos_j);
				fg_link_create(fg_state, node_i, node_j);

				// create random children
				u32 random_value_k = random_u32_range(0, 3);
				for (u32 k = 0; k < random_value_k; k++) {
					vec2_t pos_k = vec2(random_f32_range(50.0f, 500.0f), random_f32_range(50.0f, 500.0f));
					fg_node_t* node_k = fg_node_create(fg_state, pos_k);
					fg_link_create(fg_state, node_j, node_k, 50.0f);
				}

			}
			
		}


	}
}

function void
app_release() {

	// release node graph
	ng_release(node_state);
	fg_release(fg_state);

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

	// update force graph sim
	f32 delta_time = os_window_get_delta_time(window);
	fg_update(fg_state, delta_time);
	
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