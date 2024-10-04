// node.cpp

#ifndef NODE_CPP
#define NODE_CPP
	
function void 
node_state_init() {
	node_state.node_arena = arena_create(megabytes(64));
	node_state.scratch_arena = arena_create(megabytes(64));
}

function void
node_state_release() {
	arena_release(node_state.node_arena);
	arena_release(node_state.scratch_arena);
}

function void
node_state_update() {
	
	arena_clear(node_state.scratch_arena);

	for (node_t* node = node_state.first; node != nullptr; node = node->next) {

		ui_frame_flags flags =
			ui_frame_flag_draw_background_dark |
			ui_frame_flag_draw_shadow |
			ui_frame_flag_draw_border_dark | 
			ui_frame_flag_floating |
			ui_frame_flag_clickable;

		ui_set_next_fixed_x(node->pos.x);
		ui_set_next_fixed_y(node->pos.y);
		ui_set_next_pref_width(ui_size_pixel(150.0f, 1.0f));
		ui_set_next_pref_height(ui_size_pixel(100.0f, 1.0f));
		ui_frame_t* frame = ui_frame_from_string(node->label, flags);

		ui_push_parent(frame);

		// label
		ui_set_next_pref_width(ui_size_percent(1.0f));
		ui_set_next_pref_height(ui_size_pixel(20.0f, 1.0f));
		ui_label(node->label);

		// layout
		ui_set_next_pref_width(ui_size_percent(1.0f));
		ui_push_pref_height(ui_size_percent(1.0f));
		ui_row_begin();

			// input ports
			ui_set_next_pref_width(ui_size_pixel(15.0f, 1.0f));
			ui_column_begin();

			ui_push_pref_width(ui_size_percent(1.0f));
			ui_push_pref_height(ui_size_pixel(15.0f, 1.0f));
			// input port
			for (i32 i = 0; i < node->input_count; i++) {
				node_port(node->inputs[i].label, node->inputs[i].connected_port);
			}
			ui_pop_pref_height();
			ui_pop_pref_width();

			ui_row_end();

			ui_set_next_pref_width(ui_size_percent(1.0f));
			ui_column_begin();
			// blank
			ui_column_end();

			// output ports
			ui_set_next_pref_width(ui_size_pixel(15.0f, 1.0f));
			ui_column_begin();

			ui_push_pref_width(ui_size_percent(1.0f));
			ui_push_pref_height(ui_size_pixel(15.0f, 1.0f));
			// output port
			for (i32 i = 0; i < node->output_count; i++) {
				node_port(node->outputs[i].label, node->outputs[i].connected_port);
			}
			ui_pop_pref_height();
			ui_pop_pref_width();

			ui_column_end();


		ui_row_end();
		ui_pop_pref_height();
		ui_pop_parent();

		ui_interaction interaction = ui_frame_interaction(frame);
		if (interaction & ui_interaction_left_dragging) {
			node->pos = vec2_add(node->pos, ui_state.mouse_delta);
		}
	}

}


function node_t* 
node_create(str_t label, vec2_t pos) {

	node_t* node = (node_t*)arena_alloc(node_state.node_arena, sizeof(node_t));
	node->label = label;
	node->pos = pos;
	node->input_count = 0;
	node->output_count = 0;

	dll_push_back(node_state.first, node_state.last, node);

	node_state.count++;

	return node;
}

function void 
node_release(node_t* node) {
	dll_remove(node_state.first, node_state.last, node);
}

function void 
node_add_input(node_t* node) {
	str_t label = str_format(node_state.scratch_arena, "%.*s_input_port_%u", node->label.size, node->label.data, node->input_count);
	node->inputs[node->input_count].label = label;
	node->inputs[node->input_count].key = ui_key_from_string({0},label);
	node->input_count++;
}

function void 
node_add_output(node_t* node) {
	str_t label = str_format(node_state.scratch_arena, "%.*s_output_port_%u", node->label.size, node->label.data, node->output_count);
	node->outputs[node->input_count].label = label;
	node->outputs[node->input_count].key = ui_key_from_string({ 0 },label);
	node->output_count++;
}



function ui_interaction
node_port(str_t label, node_port_t* connected) {
	ui_frame_flags flags = ui_frame_flag_clickable;
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);
	node_port_draw_data_t* data = (node_port_draw_data_t*)arena_alloc(ui_state.per_frame_arena, sizeof(node_port_draw_data_t));
	ui_frame_set_custom_draw(frame, node_port_draw_function, data);
	data->connected_port = connected;
	data->interaction = interaction;

	return interaction;
}

function void 
node_port_draw_function(ui_frame_t* frame) {

	// get data
	node_port_draw_data_t* data = (node_port_draw_data_t*)frame->custom_draw_data;

	// draw port
	vec2_t frame_center = rect_center(frame->rect);
	gfx_radial_params_t params = gfx_radial_params(frame->palette->accent, 3.0f);
	gfx_draw_radial(frame_center, 6.0f, 0.0f, 360.0f, params);

	// draw connected
	if (data->connected_port != nullptr) {

		// find pos
		ui_frame_t* other_frame = ui_frame_find(data->connected_port->key);
		if (other_frame != nullptr) {
			vec2_t other_pos = rect_center(other_frame->rect);

			gfx_line_params_t line_params = gfx_line_params(frame->palette->accent, 2.5f, 0.5f);
			gfx_draw_bezier(frame_center, other_pos, vec2_add(frame_center, vec2(50.0f, 0.0f)), vec2_add(other_pos, vec2(-50.0f, 0.0f)), line_params);
			
		}

	}

	if (data->interaction & ui_interaction_left_dragging) {
		gfx_line_params_t line_params = gfx_line_params(frame->palette->accent, 2.0f, 0.5f);
		gfx_draw_bezier(frame_center, ui_state.mouse_pos, vec2_add(frame_center, vec2(50.0f, 0.0f)), vec2_add(ui_state.mouse_pos, vec2(-50.0f, 0.0f)), line_params);
	}
	

}



#endif // NODE_CPP