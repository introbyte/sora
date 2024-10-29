// ui.cpp

#ifndef UI_CPP
#define UI_CPP

// defines

#define ui_default_init(name, value) \
ui_state.name##_default_node.v = value; \

#define ui_stack_reset(name) \
ui_state.name##_stack.top = &ui_state.name##_default_node; ui_state.name##_stack.free = 0; ui_state.name##_stack.auto_pop = 0; \

// implementation


// state functions

function void
ui_init() {

	// create arenas
	ui_state.frame_arena = arena_create(gigabytes(1));
	ui_state.per_frame_arena = arena_create(gigabytes(1));
	ui_state.event_arena = arena_create(megabytes(2));
	ui_state.drag_state_arena = arena_create(megabytes(64));
	ui_state.scratch_arena = arena_create(gigabytes(1));

	// set context to nullptr
	ui_state.window = nullptr;
	ui_state.renderer = nullptr;

	// set event bindings
	// no clue if this is a decent way of doing this, but don't care because it works.
	ui_state.event_bindings[0] =  { os_key_left,      0, ui_event_type_navigate, 0, ui_event_delta_unit_char, {-1,  0 } };
	ui_state.event_bindings[1] =  { os_key_right,     0, ui_event_type_navigate, 0, ui_event_delta_unit_char, {+1,  0 } };
	ui_state.event_bindings[2] =  { os_key_up,        0, ui_event_type_navigate, 0, ui_event_delta_unit_char, { 0, -1 } };
	ui_state.event_bindings[3] =  { os_key_down,      0, ui_event_type_navigate, 0, ui_event_delta_unit_char, { 0, +1 } };
												      
	ui_state.event_bindings[4] =  { os_key_left,      os_modifier_shift, ui_event_type_navigate, ui_event_flag_keep_mark, ui_event_delta_unit_char, {-1,  0 } };
	ui_state.event_bindings[5] =  { os_key_right,     os_modifier_shift, ui_event_type_navigate, ui_event_flag_keep_mark, ui_event_delta_unit_char, {+1,  0 } };
	ui_state.event_bindings[6] =  { os_key_up,        os_modifier_shift, ui_event_type_navigate, ui_event_flag_keep_mark, ui_event_delta_unit_char, { 0, -1 } };
	ui_state.event_bindings[7] =  { os_key_down,      os_modifier_shift, ui_event_type_navigate, ui_event_flag_keep_mark, ui_event_delta_unit_char, { 0, +1 } };
												      
	ui_state.event_bindings[8] =  { os_key_left,      os_modifier_ctrl, ui_event_type_navigate, 0, ui_event_delta_unit_word, {-1,  0 } };
	ui_state.event_bindings[9] =  { os_key_right,     os_modifier_ctrl, ui_event_type_navigate, 0, ui_event_delta_unit_word, {+1,  0 } };
	ui_state.event_bindings[10] = { os_key_up,        os_modifier_ctrl, ui_event_type_navigate, 0, ui_event_delta_unit_word, { 0, -1 } };
	ui_state.event_bindings[11] = { os_key_down,      os_modifier_ctrl, ui_event_type_navigate, 0, ui_event_delta_unit_word, { 0, +1 } };
												      
	ui_state.event_bindings[12] = { os_key_left,      os_modifier_shift | os_modifier_ctrl, ui_event_type_navigate, ui_event_flag_keep_mark, ui_event_delta_unit_word, {-1,  0 } };
	ui_state.event_bindings[13] = { os_key_right,     os_modifier_shift | os_modifier_ctrl, ui_event_type_navigate, ui_event_flag_keep_mark, ui_event_delta_unit_word, {+1,  0 } };
	ui_state.event_bindings[14] = { os_key_up,        os_modifier_shift | os_modifier_ctrl, ui_event_type_navigate, ui_event_flag_keep_mark, ui_event_delta_unit_word, { 0, -1 } };
	ui_state.event_bindings[15] = { os_key_down,      os_modifier_shift | os_modifier_ctrl, ui_event_type_navigate, ui_event_flag_keep_mark, ui_event_delta_unit_word, { 0, +1 } };
												      
	ui_state.event_bindings[16] = { os_key_home,      0, ui_event_type_navigate, 0, ui_event_delta_unit_line, { -1, 0 } };
	ui_state.event_bindings[17] = { os_key_end,       0, ui_event_type_navigate, 0, ui_event_delta_unit_line, { +1, 0 } };
	ui_state.event_bindings[18] = { os_key_home,      os_modifier_shift, ui_event_type_navigate, ui_event_flag_keep_mark, ui_event_delta_unit_line, { -1, 0 } };
	ui_state.event_bindings[19] = { os_key_end,       os_modifier_shift, ui_event_type_navigate, ui_event_flag_keep_mark, ui_event_delta_unit_line, { +1, 0 } };
												      
	ui_state.event_bindings[20] = { os_key_home,      os_modifier_ctrl, ui_event_type_navigate, 0, ui_event_delta_unit_whole, { -1, 0 } };
	ui_state.event_bindings[21] = { os_key_end,       os_modifier_ctrl, ui_event_type_navigate, 0, ui_event_delta_unit_whole, { +1, 0 } };
	ui_state.event_bindings[22] = { os_key_home,      os_modifier_shift | os_modifier_ctrl, ui_event_type_navigate, ui_event_flag_keep_mark, ui_event_delta_unit_whole, { -1, 0 } };
	ui_state.event_bindings[23] = { os_key_end,       os_modifier_shift | os_modifier_ctrl, ui_event_type_navigate, ui_event_flag_keep_mark, ui_event_delta_unit_whole, { +1, 0 } };

	ui_state.event_bindings[24] = { os_key_delete,    0, ui_event_type_edit, ui_event_flag_delete | ui_event_flag_zero_delta, ui_event_delta_unit_char, { +1, 0 } };
	ui_state.event_bindings[25] = { os_key_backspace, 0, ui_event_type_edit, ui_event_flag_delete | ui_event_flag_zero_delta, ui_event_delta_unit_char, { -1, 0 } };
	ui_state.event_bindings[26] = { os_key_delete,    os_modifier_ctrl, ui_event_type_edit, ui_event_flag_delete | ui_event_flag_zero_delta, ui_event_delta_unit_word, { +1, 0 } };
	ui_state.event_bindings[27] = { os_key_backspace, os_modifier_ctrl, ui_event_type_edit, ui_event_flag_delete | ui_event_flag_zero_delta, ui_event_delta_unit_word, { -1, 0 } };

	// last click time
	ui_state.last_click_time[0] = 0;
	ui_state.last_click_time[1] = 0;
	ui_state.last_click_time[2] = 0;

	// text point
	ui_state.cursor = { 1, 1 };
	ui_state.mark = { 1, 1 };

	// default resources
	ui_state.default_palette.dark_background = color(0x282828ff);
	ui_state.default_palette.dark_border = color(0x3d3d3dff);
	ui_state.default_palette.light_background = color(0x3d3d3dff);
	ui_state.default_palette.light_border = color(0x525252ff);
	ui_state.default_palette.shadow = color(0x15151580);
	ui_state.default_palette.hover = color(0x151515ff);
	ui_state.default_palette.active = color(0x151515ff);
	ui_state.default_palette.text = color(0xe2e2e2ff);
	ui_state.default_palette.accent = color(0x5bd9ffff); // blue

	ui_state.default_texture = draw_state.default_texture;
	ui_state.default_font = font_open(str("res/fonts/segoe_ui.ttf"));
	ui_state.default_icon_font = font_open(str("res/fonts/icons.ttf"));
	
	// default stack
	ui_default_init(parent, nullptr);
	ui_default_init(flags, 0);
	ui_default_init(fixed_x, 0.0f);
	ui_default_init(fixed_y, 0.0f);
	ui_default_init(fixed_width, 0.0f);
	ui_default_init(fixed_height, 0.0f);
	ui_default_init(pref_width, ui_size(ui_size_type_null, 0.0f, 0.0f));
	ui_default_init(pref_height, ui_size(ui_size_type_null, 0.0f, 0.0f));
	ui_default_init(text_alignment, ui_text_alignment_left);
	ui_default_init(text_padding, 4.0f);
	ui_default_init(hover_cursor, os_cursor_pointer);
	ui_default_init(layout_axis, ui_layout_axis_y);
	ui_default_init(rounding_00, 2.0f);
	ui_default_init(rounding_01, 2.0f);
	ui_default_init(rounding_10, 2.0f);
	ui_default_init(rounding_11, 2.0f);
	ui_default_init(palette, &ui_state.default_palette);
	ui_default_init(texture, ui_state.default_texture);
	ui_default_init(font, ui_state.default_font);
	ui_default_init(font_size, 9.0f);
	ui_default_init(focus_hot, ui_focus_type_null);
	ui_default_init(focus_active, ui_focus_type_null);

	// set keys to zero
	ui_state.hovered_frame_key = { 0 };
	ui_state.active_frame_key[0] = {0};
	ui_state.active_frame_key[1] = {0};
	ui_state.active_frame_key[2] = {0};
	ui_state.focused_frame_key = { 0 };

	// build state
	ui_state.build_index = 0;

	// assets
	global gfx_shader_attribute_t ui_shader_attributes[] = {
		{"BBOX", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance},
		{"UV", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance},
		{"TYPE", 0, gfx_vertex_format_int, gfx_vertex_class_per_instance},
		{"POS", 0,  gfx_vertex_format_float2, gfx_vertex_class_per_instance},
		{"POS", 1,  gfx_vertex_format_float2, gfx_vertex_class_per_instance},
		{"POS", 2,  gfx_vertex_format_float2, gfx_vertex_class_per_instance},
		{"POS", 3,  gfx_vertex_format_float2, gfx_vertex_class_per_instance},
		{"COL", 0,  gfx_vertex_format_float4, gfx_vertex_class_per_instance},
		{"COL", 1,  gfx_vertex_format_float4, gfx_vertex_class_per_instance},
		{"COL", 2,  gfx_vertex_format_float4, gfx_vertex_class_per_instance},
		{"COL", 3,  gfx_vertex_format_float4, gfx_vertex_class_per_instance},
		{"RAD", 0,  gfx_vertex_format_float4, gfx_vertex_class_per_instance},
		{"STY", 0,  gfx_vertex_format_float3, gfx_vertex_class_per_instance},
	};
	ui_state.ui_shader = gfx_shader_load(str("res/shaders/shader_2d.hlsl"), ui_shader_attributes, 13);

}

function void
ui_release() {

	ui_state.window = nullptr;
	ui_state.renderer = nullptr;

	font_close(ui_state.default_font);
	font_close(ui_state.default_icon_font);

	// release arenas
	arena_release(ui_state.frame_arena);
	arena_release(ui_state.per_frame_arena);
	arena_release(ui_state.event_arena);
	arena_release(ui_state.drag_state_arena);
	arena_release(ui_state.scratch_arena);

}

function void
ui_begin_frame(gfx_renderer_t* renderer) {

	// set context
	ui_state.window = renderer->window;
	ui_state.renderer = renderer;

	// clear arenas
	arena_clear(ui_state.per_frame_arena);
	arena_clear(ui_state.event_arena);
	arena_clear(ui_state.scratch_arena);


	// reset event list
	ui_state.event_list = { 0 };
	
	// reset mouse clicks
	u64 time_now = (os_time_microseconds() / 1000);
	for (i32 i = 0; i < 3; i++) {
		u32 time_since = time_now - ui_state.last_click_time[i];
		if (time_since > os_state.double_click_time) {
			ui_state.click_counter[i] = 0;
		}
	}

	// gather events
	for (os_event_t* os_event = os_state.event_list.first; os_event != 0; os_event = os_event->next) {
		ui_event_t ui_event = { 0 };

		// start with default
		ui_event.type = ui_event_type_null;
		ui_event.flags = 0;
		ui_event.delta_unit = ui_event_delta_unit_null;
		ui_event.key = os_event->key;
		ui_event.mouse = os_event->mouse;
		ui_event.modifiers = os_event->modifiers;
		ui_event.character = os_event->character;
		ui_event.position = os_event->position;
		ui_event.scroll = os_event->scroll;
		ui_event.delta = ivec2(0);

		if (os_event->type != 0 && os_event->window == ui_state.window) {

			switch (os_event->type) {

				// key pressed
				case os_event_type_key_press: {
					ui_event.type = ui_event_type_key_press;

					// check for bindings
					ui_event_binding_t* binding = ui_event_get_binding(os_event->key, os_event->modifiers);

					if (binding != nullptr) {
						ui_event.type = binding->result_type;
						ui_event.flags = binding->result_flags;
						ui_event.delta_unit = binding->result_delta_unit;
						ui_event.delta = binding->result_delta;
					}

					break;
				}

				case os_event_type_key_release: { ui_event.type = ui_event_type_key_release; break; }
				case os_event_type_mouse_press: { 
					
					ui_event.type = ui_event_type_mouse_press; 
					ui_state.click_counter[os_event->mouse]++;
					ui_state.last_click_time[os_event->mouse] = (os_time_microseconds() / 1000);
					break;
				}
				case os_event_type_mouse_release: { ui_event.type = ui_event_type_mouse_release; break; }
				case os_event_type_mouse_move: { ui_event.type = ui_event_type_mouse_move; break; }
				case os_event_type_text: { ui_event.type = ui_event_type_text; break; }
				case os_event_type_mouse_scroll: { ui_event.type = ui_event_type_mouse_scroll; break; }
			}
			ui_event_push(&ui_event);
		}
	}

	// set input
	ui_state.mouse_pos = ui_state.window->mouse_pos;
	ui_state.mouse_delta = ui_state.window->mouse_delta;

	// reset stacks
	ui_stack_reset(parent);
	ui_stack_reset(flags);
	ui_stack_reset(fixed_x);
	ui_stack_reset(fixed_y);
	ui_stack_reset(fixed_width);
	ui_stack_reset(fixed_height);
	ui_stack_reset(pref_width);
	ui_stack_reset(pref_height);
	ui_stack_reset(text_alignment);
	ui_stack_reset(text_padding);
	ui_stack_reset(hover_cursor);
	ui_stack_reset(layout_axis);
	ui_stack_reset(rounding_00);
	ui_stack_reset(rounding_01);
	ui_stack_reset(rounding_10);
	ui_stack_reset(rounding_11);
	ui_stack_reset(palette);
	ui_stack_reset(texture);
	ui_stack_reset(font);
	ui_stack_reset(font_size);
	ui_stack_reset(focus_hot);
	ui_stack_reset(focus_active);

	// do navigation


	// set to next nav keys
	for (ui_frame_t* frame = ui_state.frame_first; frame != nullptr; frame = frame->hash_next) {
		frame->nav_focus_hot_key = frame->nav_focus_next_hot_key;
		frame->nav_focus_active_key = frame->nav_focus_next_active_key;
	}

	// add root frame
	str_t root_string = str_format(ui_state.scratch_arena, "%.*s_root_frame", ui_state.window->title.size, ui_state.window->title.data);
	ui_set_next_fixed_width((f32)ui_state.window->resolution.x);
	ui_set_next_fixed_height((f32)ui_state.window->resolution.y);
	ui_set_next_layout_axis(ui_layout_axis_y);
	ui_frame_t* frame = ui_frame_from_string(root_string, 0);
	ui_state.root = frame;
	ui_push_parent(frame);

	// reset hovered key
	b8 has_active = false;
	for (i32 i = 0; i < os_mouse_button_count; i++) {
		if (!ui_key_equals(ui_state.active_frame_key[i], { 0 })) {
			has_active = true;
		}
	}
	if (!has_active) {
		ui_state.hovered_frame_key = { 0 };
	}

	ui_state.build_index++;
}

function void
ui_end_frame() {

	// remove untouched frames
	ui_frame_t* next = nullptr;
	for (ui_frame_t* frame = ui_state.frame_first; frame != 0; frame = next) {
		next = frame->hash_next;
		if (frame->last_build_index != ui_state.build_index || ui_key_equals(frame->key, {0})) {
			dll_remove_np(ui_state.frame_first, ui_state.frame_last, frame, hash_next, hash_prev);
			stack_push_n(ui_state.frame_free, frame, hash_next);
		}
	}

	// remove focus
	for (ui_event_t* event = ui_state.event_list.first; event != nullptr; event = event->next) {
		if (event->type == ui_event_type_mouse_release) {
			ui_state.focused_frame_key = { 0 };
		}
	}

	// layout pass
	ui_layout_solve_independent(ui_state.root);
	ui_layout_solve_upward_dependent(ui_state.root);
	ui_layout_solve_downward_dependent(ui_state.root);
	ui_layout_solve_violations(ui_state.root);
	ui_layout_solve_set_positions(ui_state.root);

	// animate
	{
		// animate frames
		f32 fast_rate = 1.0f - powf(2.0f, -50.0f * ui_state.window->delta_time);
		f32 slow_rate = 1.0f - powf(2.0f, -30.0f * ui_state.window->delta_time);
		for (ui_frame_t* frame = ui_state.frame_first; frame != 0; frame = frame->hash_next) {
			b8 is_hovered = ui_key_equals(frame->key, ui_state.hovered_frame_key);
			b8 is_active = ui_key_equals(frame->key, ui_state.active_frame_key[os_mouse_button_left]);

			frame->hover_t += fast_rate * ((f32)is_hovered - frame->hover_t);
			frame->active_t += fast_rate * ((f32)is_active - frame->active_t);

			if (frame->flags & ui_frame_flag_view_clamp) {
				vec2_t max_view_offset_target = vec2(
					max(0.0f, frame->view_bounds.x - frame->fixed_size.x),
					max(0.0f, frame->view_bounds.y - frame->fixed_size.y)
				);
				if (frame->flags & ui_frame_flag_view_clamp_x) { frame->view_offset_target.x = clamp(frame->view_offset_target.x, 0.0f, max_view_offset_target.x); }
				if (frame->flags & ui_frame_flag_view_clamp_y) { frame->view_offset_target.y = clamp(frame->view_offset_target.y, 0.0f, max_view_offset_target.y); }
			}
			
			frame->view_offset = vec2_add(frame->view_offset, vec2_mul(vec2_sub(frame->view_offset_target, frame->view_offset), fast_rate));
		}

		// animate cursor
		ui_state.cursor_pos.x += (ui_state.cursor_target_pos.x - ui_state.cursor_pos.x) * fast_rate;
		ui_state.mark_pos.x += (ui_state.mark_target_pos.x - ui_state.mark_pos.x) * fast_rate;
	}

	// hover cursor
	{

		ui_frame_t* hovered_frame = ui_frame_find(ui_state.hovered_frame_key);
		ui_frame_t* active_frame = ui_frame_find(ui_state.active_frame_key[os_mouse_button_left]);

		if (hovered_frame != nullptr && !(hovered_frame->flags & ui_frame_flag_custom_hover_cursor)) {
			os_cursor cursor = hovered_frame->hover_cursor;
			os_set_cursor(cursor);
		}

	}
	
	// set draw pipeline
	ui_state.constants.window_size = vec2((f32)ui_state.renderer->resolution.x, (f32)ui_state.renderer->resolution.y);
	gfx_pipeline_t pipeline = gfx_pipeline_create();
	pipeline.topology = gfx_topology_tri_strip;
	pipeline.depth_mode = gfx_depth_none;
	draw_push_pipeline(pipeline);
	draw_push_shader(ui_state.ui_shader);
	draw_push_constants(&ui_state.constants, sizeof(ui_constants_t), 0);
	draw_push_texture(font_state.atlas_texture);

	// draw
	gfx_renderer_t* renderer = ui_state.renderer;
	u32 depth = 0;
	for (ui_frame_t* frame = ui_state.root; frame != nullptr;) {

		// do recursive depth first search
		u32 sibling_member_offset = (u32)(&(((ui_frame_t*)0)->tree_prev));
		u32 child_member_offset = (u32)(&(((ui_frame_t*)0)->tree_child_last));
		ui_frame_rec_t recursion = ui_frame_rec_depth_first(frame, ui_state.root, sibling_member_offset, child_member_offset);

		// grab frame info
		ui_palette_t* palette = frame->palette;

		// frame shadow
		if (frame->flags & ui_frame_flag_draw_shadow) {
			//draw_set_next_color(palette->shadow);
			//draw_set_next_radius(frame->rounding);
			//draw_push_rect(frame->rect);
		}

		// determine color
		color_t background_color = palette->dark_background;
		color_t border_color = palette->dark_border;

		if (frame->flags & ui_frame_flag_draw_background_light) {
			background_color = palette->light_background;
		}

		if (frame->flags & ui_frame_flag_draw_border_light) {
			border_color = palette->light_border;
		}

		// background
		if (frame->flags & (ui_frame_flag_draw_background_light | ui_frame_flag_draw_background_dark)) {

			// hover effects
			if (frame->flags & ui_frame_flag_draw_hover_effects) {
				background_color = color_lerp(background_color, color_add(background_color, palette->hover), frame->hover_t);
				border_color = color_lerp(border_color, color_add(border_color, palette->hover), frame->hover_t);
			}
			
			// active effects
			if (frame->flags & (ui_frame_flag_draw_active_effects)) {
				background_color = color_lerp(background_color, color_add(background_color, palette->active), frame->active_t);
				border_color = color_lerp(border_color, color_add(border_color, palette->active), frame->hover_t);
			}

			// background
			draw_set_next_color(background_color);
			draw_set_next_radius(frame->rounding);
			draw_push_rect(frame->rect);
		}

		// border
		if (frame->flags & (ui_frame_flag_draw_border_light | ui_frame_flag_draw_border_dark)) {
			draw_set_next_color(border_color);
			draw_set_next_radius(frame->rounding);
			draw_set_next_thickness(1.0f);
			draw_push_rect(frame->rect);
		}
	
		// clip
		if (frame->flags & ui_frame_flag_clip) {
			//rect_t top_clip = gfx_top_clip();
			//rect_t new_clip = rect_shrink(frame->rect, 1.0f);
			//if (top_clip.x1 != 0.0f || top_clip.y1 != 0.0f) {
			//	new_clip = rect_intersection(new_clip, top_clip);
			//}
			//gfx_push_clip(new_clip);
		}

		// text
		if (frame->flags & ui_frame_flag_draw_text) {

			// calculate text pos
			vec2_t text_pos = ui_text_align(frame->font, frame->font_size, frame->string, frame->rect, frame->text_alignment);
			
			draw_push_font(frame->font);
			draw_push_font_size(frame->font_size);

			{
				// text shadow
				draw_set_next_color(palette->shadow);
				draw_push_text(frame->string, vec2_add(text_pos, 1.0f));

				// text
				draw_set_next_color(palette->text);
				draw_push_text(frame->string, text_pos);
			}

			draw_pop_font();
			draw_pop_font_size();
		}
		
		// custom draw
		if (frame->flags & ui_frame_flag_draw_custom) {
			if (frame->custom_draw_func != 0) {
				frame->custom_draw_func(frame);
			}
		}
		
		// pop
		i32 pop_index = 0;
		for (ui_frame_t* f = frame; f != nullptr && pop_index <= recursion.pop_count; f = f->tree_parent) {
			pop_index += 1;
			if (f == frame && recursion.push_count != 0) {
				continue;
			}
			
			// pop clips
			if (f->flags & ui_frame_flag_clip) {
				//gfx_pop_clip();
			}
		}

		frame = recursion.next;
	}

	// submit
	draw_submit();
}

// widgets

function ui_interaction 
ui_button(str_t label) {

	ui_frame_flags flags =
		ui_frame_flag_clickable |
		ui_frame_flag_draw;

	ui_set_next_hover_cursor(os_cursor_hand_point);
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);

	return interaction;
}

function ui_interaction 
ui_buttonf(char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	str_t display_string = str_formatv(ui_state.scratch_arena, fmt, args);
	va_end(args);

	ui_interaction interaction = ui_button(display_string);
	
	return interaction;
}

function ui_interaction 
ui_label(str_t label) {

	ui_frame_flags flags =
		ui_frame_flag_draw_text;

	ui_frame_t* frame = ui_frame_from_string(str(""), flags);
	frame->string = ui_string_display_format(label);
	ui_interaction interaction = ui_frame_interaction(frame);

	return interaction;
}

function ui_interaction 
ui_labelf(char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	str_t display_string = str_formatv(ui_state.scratch_arena, fmt, args);
	va_end(args);

	ui_interaction interaction = ui_label(display_string);
	
	return interaction;
}

function ui_interaction
ui_slider(str_t label, f32* value, f32 min, f32 max) {
	
	ui_frame_flags flags =
		ui_frame_flag_clickable |
		ui_frame_flag_draw;

	ui_set_next_hover_cursor(os_cursor_hand_point);
	ui_set_next_text_alignment(ui_text_alignment_center);
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	str_t text = str_format(ui_state.per_frame_arena, "%.2f", *value);
	ui_frame_set_display_text(frame, text);
	ui_interaction interaction = ui_frame_interaction(frame);
	ui_slider_data_t* data = (ui_slider_data_t*)arena_alloc(ui_state.per_frame_arena, sizeof(ui_slider_data_t));
	ui_frame_set_custom_draw(frame, ui_slider_draw_function, data);

	// interaction
	if (interaction & ui_interaction_left_dragging) {
		vec2_t mouse_pos = ui_state.mouse_pos;
		*value = remap(mouse_pos.x, frame->rect.x0, frame->rect.x1, min, max);
		*value = clamp(*value, min, max);
	}

	f32 percent = remap(*value, min, max, 0.0f, 1.0f);
	data->value = percent;

	return interaction;
}

function ui_interaction
ui_checkbox(str_t label, b8* value) {

	// build parent frame
	ui_set_next_hover_cursor(os_cursor_hand_point);
	ui_set_next_layout_axis(ui_layout_axis_x);

	u32 flags = ui_frame_flag_clickable;
	ui_frame_t* parent_frame = ui_frame_from_string(label, flags);

	ui_push_parent(parent_frame);
	{
		ui_set_next_pref_width(ui_size_pixel(rect_height(parent_frame->rect), 1.0f));
		ui_set_next_font(ui_state.default_icon_font);

		u32 flag_frame_flags = 
			ui_frame_flag_draw_text | ui_frame_flag_draw_background_dark |
			ui_frame_flag_draw_hover_effects | ui_frame_flag_draw_active_effects |
			ui_frame_flag_draw_shadow | ui_frame_flag_draw_border_dark;

		ui_frame_t* icon_frame = ui_frame_from_string(str(""), flag_frame_flags);
		icon_frame->active_t = parent_frame->active_t;
		icon_frame->hover_t = parent_frame->hover_t;
		if (*value) {
			ui_frame_set_display_text(icon_frame, str("X"));
		}

		ui_set_next_pref_width(ui_size_percent(1.0f));
		ui_set_next_pref_height(ui_size_percent(1.0f));
		ui_label(label);
	}
	ui_pop_parent();

	ui_interaction interaction = ui_frame_interaction(parent_frame);

	if (interaction & ui_interaction_left_released) {
		*value = !*value;
	}

	return interaction;


}

function ui_interaction
ui_expander(str_t label, b8* is_expanded) {

	// build parent frame
	ui_set_next_hover_cursor(os_cursor_hand_point);
	ui_set_next_layout_axis(ui_layout_axis_x);

	u32 flags = 
		ui_frame_flag_clickable | ui_frame_flag_draw_background_light | 
		ui_frame_flag_draw_hover_effects | ui_frame_flag_draw_active_effects |
		ui_frame_flag_draw_shadow | ui_frame_flag_draw_border_light;
	ui_frame_t* parent_frame = ui_frame_from_string(label, flags);

	ui_push_parent(parent_frame);
	{
		ui_set_next_pref_width(ui_size_pixel(rect_height(parent_frame->rect), 1.0f));
		ui_set_next_font(ui_state.default_icon_font);
		ui_frame_t* icon_frame = ui_frame_from_string(str(""), ui_frame_flag_draw_text);
		if (*is_expanded) {
			ui_frame_set_display_text(icon_frame, str(">"));
		} else {
			ui_frame_set_display_text(icon_frame, str("v"));
		}

		ui_set_next_pref_width(ui_size_percent(1.0f));
		ui_set_next_pref_height(ui_size_percent(1.0f));
		ui_label(label);
	}
	ui_pop_parent();

	ui_interaction interaction = ui_frame_interaction(parent_frame);

	if (interaction & ui_interaction_left_released) {
		*is_expanded = !*is_expanded;
	}

	return interaction;
}

function ui_interaction
ui_color_sat_val_quad(str_t label, f32 hue, f32* sat, f32* val) {

	// build frame and set draw data
	ui_set_next_hover_cursor(os_cursor_hand_point);
	ui_frame_flags flags = ui_frame_flag_clickable | ui_frame_flag_draw_shadow | ui_frame_flag_draw_border_dark;
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);
	ui_color_data_t* data = (ui_color_data_t*)arena_alloc(ui_state.per_frame_arena, sizeof(ui_color_data_t));
	ui_frame_set_custom_draw(frame, ui_color_sat_val_quad_draw_function, data);

	// interaction
	if (interaction & ui_interaction_left_dragging) {
		vec2_t mouse_pos = ui_state.mouse_pos;
		f32 frame_width = rect_width(frame->rect);
		f32 frame_height = rect_height(frame->rect);

		*sat = (mouse_pos.x - frame->rect.x0) / frame_width;
		*val = 1.0f - (mouse_pos.y - frame->rect.y0) / frame_height;
		*sat = clamp_01(*sat);
		*val = clamp_01(*val);
	}

	// set data
	data->hue = hue;
	data->sat = *sat;
	data->val = *val;

	return interaction;

}

function ui_interaction 
ui_color_hue_bar(str_t label, f32* hue, f32 sat, f32 val) {

	// build frame and set draw data
	ui_set_next_hover_cursor(os_cursor_hand_point);
	ui_frame_flags flags = ui_frame_flag_clickable | ui_frame_flag_draw_shadow | ui_frame_flag_draw_border_dark;
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);
	ui_color_data_t* data = (ui_color_data_t*)arena_alloc(ui_state.per_frame_arena, sizeof(ui_color_data_t));
	ui_frame_set_custom_draw(frame, ui_color_hue_bar, data);

	// interaction
	if (interaction & ui_interaction_left_dragging) {
		vec2_t mouse_pos = ui_state.mouse_pos;
		*hue = remap(mouse_pos.x, frame->rect.x0, frame->rect.x1, 0.0f, 1.0f);
		*hue = clamp_01(*hue);
	}

	data->hue = *hue;
	data->sat = sat;
	data->val = val;

	return interaction;
}

function ui_interaction
ui_color_wheel(str_t label, f32* hue, f32* sat, f32* val) {

	enum {
		area_clicked_none,
		area_clicked_hue_wheel, 
		area_clicked_sat_val_tri,
	};

	// build frame and set custom draw data
	ui_frame_flags flags = ui_frame_flag_clickable | ui_frame_flag_custom_hover_cursor;
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);
	ui_color_data_t* data = (ui_color_data_t*)arena_alloc(ui_state.per_frame_arena, sizeof(ui_color_data_t));
	ui_frame_set_custom_draw(frame, ui_color_wheel_draw_function, data);
	
	// calculate frame and mouse info
	vec2_t frame_center = rect_center(frame->rect);
	f32 frame_width = rect_width(frame->rect);
	f32 frame_height = rect_height(frame->rect);
	f32 outer_wheel_radius = min(frame_width, frame_height) * 0.5f;
	f32 inner_wheel_radius = outer_wheel_radius - (20.0f);
	vec2_t mouse_pos = ui_state.mouse_pos;
	vec2_t dir = vec2_sub(frame_center, mouse_pos);
	f32 dist = vec2_length(dir);
	f32 tri_dist = inner_wheel_radius * 0.9f;
	vec2_t tri_p0 = vec2_add(frame_center, vec2_from_angle(*hue * (2.0f * f32_pi), tri_dist));
	vec2_t tri_p1 = vec2_add(frame_center, vec2_from_angle((*hue - 0.3333f) * (2.0f * f32_pi), tri_dist));
	vec2_t tri_p2 = vec2_add(frame_center, vec2_from_angle((*hue + 0.3333f) * (2.0f * f32_pi), tri_dist));

	// interactions

	// custom hover cursor 
	if (interaction & ui_interaction_hovered) {
		// if within ring or tri
		if (dist > inner_wheel_radius && dist < outer_wheel_radius || tri_contains(tri_p0, tri_p1, tri_p2, mouse_pos)) {
			os_set_cursor(os_cursor_hand_point);
		}
	}

	// when intially pressed figure out where we were clicked
	if (interaction& ui_interaction_left_pressed) {
		u32 area_clicked = area_clicked_none;

		// if within ring
		if (dist > inner_wheel_radius && dist < outer_wheel_radius) {
			area_clicked = area_clicked_hue_wheel;
		}

		// if within circle
		if (tri_contains(tri_p0, tri_p1, tri_p2, mouse_pos)) {
			area_clicked = area_clicked_sat_val_tri;
		}

		// store data
		ui_store_drag_data(&area_clicked, sizeof(area_clicked));
	}

	if (interaction & ui_interaction_left_dragging) {

		// get drag data
		u32* area_clicked = (u32*)ui_get_drag_data();

		if (area_clicked != nullptr) {

			// edit via hue wheel
			if (*area_clicked == area_clicked_hue_wheel) {

				// convert direction to hue
				f32 angle = (vec2_to_angle(dir) + f32_pi) / (2.0f * f32_pi);
				*hue = angle;
			}

			// edit via sat val tri
			if (*area_clicked == area_clicked_sat_val_tri) {
				
				// convert barycentric coord to sat and val
				vec3_t bary = barycentric(mouse_pos, tri_p0, tri_p2, tri_p1);
				*val = clamp_01(1.0f - bary.y);
				*sat = clamp_01(bary.x / *val);

			}
		}
	}

	// clear drag data on release
	if (interaction & ui_interaction_left_released) {
		ui_clear_drag_data();
	}

	data->hue = *hue;
	data->sat = *sat;
	data->val = *val;

	return interaction;
}

function ui_interaction 
ui_color_hue_sat_circle(str_t label, f32* hue, f32* sat, f32 val) {

	enum {
		area_clicked_none,
		area_clicked_hue_sat_circle,
	};

	// build frame and set custom draw data
	ui_frame_flags flags = ui_frame_flag_clickable | ui_frame_flag_custom_hover_cursor;
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);
	ui_color_data_t* data = (ui_color_data_t*)arena_alloc(ui_state.per_frame_arena, sizeof(ui_color_data_t));
	ui_frame_set_custom_draw(frame, ui_color_hue_sat_circle_draw_function, data);

	// calculate frame and mouse info
	vec2_t frame_center = rect_center(frame->rect);
	f32 frame_width = rect_width(frame->rect);
	f32 frame_height = rect_height(frame->rect);
	f32 outer_wheel_radius = min(frame_width, frame_height) * 0.5f;
	vec2_t mouse_pos = ui_state.mouse_pos;
	vec2_t dir = vec2_sub(frame_center, mouse_pos);
	f32 dist = vec2_length(dir);

	// custom hover cursor 
	if (interaction & ui_interaction_hovered) {
		// if within ring or tri
		if (dist < outer_wheel_radius) {
			os_set_cursor(os_cursor_hand_point);
		}
	}

	if (interaction & ui_interaction_left_pressed) {
		u32 area_clicked = area_clicked_none;

		// if within ring
		if (dist < outer_wheel_radius) {
			area_clicked = area_clicked_hue_sat_circle;
		}

		// store data
		ui_store_drag_data(&area_clicked, sizeof(area_clicked));
	}

	if (interaction & ui_interaction_left_dragging) {

		// get drag data
		u32* area_clicked = (u32*)ui_get_drag_data();

		// edit via hue wheel
		if (*area_clicked == area_clicked_hue_sat_circle) {
			
			// convert direction to hue
			f32 angle = (vec2_to_angle(dir) + f32_pi) / (2.0f * f32_pi);
			*hue = angle;

			// convert distance to sat
			*sat = clamp_01(dist / outer_wheel_radius);
		}
	}

	// clear drag data on release
	if (interaction & ui_interaction_left_released) {
		ui_clear_drag_data();
	}

	data->hue = *hue;
	data->sat = *sat;
	data->val = val;

	return interaction;

}

function ui_interaction 
ui_color_val_bar(str_t label, f32 hue, f32 sat, f32* val) {

	// build frame and set draw data
	ui_set_next_hover_cursor(os_cursor_hand_point);
	ui_frame_flags flags = ui_frame_flag_clickable | ui_frame_flag_draw_shadow | ui_frame_flag_draw_border_dark;
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);
	ui_color_data_t* data = (ui_color_data_t*)arena_alloc(ui_state.per_frame_arena, sizeof(ui_color_data_t));
	ui_frame_set_custom_draw(frame, ui_color_val_bar_draw_function, data);

	// interaction
	if (interaction & ui_interaction_left_dragging) {
		vec2_t mouse_pos = ui_state.mouse_pos;
		*val = remap(mouse_pos.x, frame->rect.x0, frame->rect.x1, 0.0f, 1.0f);
		*val = clamp_01(*val);
	}

	data->hue = hue;
	data->sat = sat;
	data->val = *val;

	return interaction;
}

function ui_interaction
ui_text_edit(str_t label, char* buffer, u32 buffer_size, u32* out_size) {

	ui_key_t key = ui_key_from_string({0}, label);
	ui_frame_flags flags =
		ui_frame_flag_draw_text | 
		ui_frame_flag_draw_shadow | 
		ui_frame_flag_draw_background_dark |
		ui_frame_flag_draw_border_dark |
		ui_frame_flag_clickable;

	// if focused already, don't do hover effects
	b8 frame_focused = ui_key_equals(ui_state.focused_frame_key, key);
	if (!frame_focused) {
		flags |= ui_frame_flag_draw_hover_effects;
	}

	ui_set_next_hover_cursor(os_cursor_I_beam);
	ui_frame_t* frame = ui_frame_from_key(key, flags);
	ui_frame_set_custom_draw(frame, ui_text_edit_draw_function, nullptr);

	if (frame_focused) {

		// loop through events
		ui_event_t* next = nullptr;
		for (ui_event_t* event = ui_state.event_list.first; event != nullptr; event = next) {
			str_t edit_string = str(buffer, *out_size);
			next = event->next;

			// skip if not text input events
			if (event->type != ui_event_type_edit && event->type != ui_event_type_navigate && event->type != ui_event_type_text) {
				continue;
			}

			// get text op
			ui_text_op_t text_op = ui_event_to_text_op(ui_state.scratch_arena, event, edit_string, ui_state.cursor, ui_state.mark);

			// skip if invalid
			if (text_op.flags & ui_text_op_flag_invalid) {
				continue;
			}

			// replace range
			if (!ui_text_point_equals(text_op.range.min, text_op.range.max) || text_op.replace.size != 0) {
				str_t new_string = ui_string_replace_range(ui_state.scratch_arena, edit_string, text_op.range, text_op.replace);
				new_string.size = min(buffer_size, new_string.size);
				memcpy(buffer, new_string.data, new_string.size);
				*out_size = new_string.size;
			}

			// update cursor
			ui_state.cursor = text_op.cursor;
			ui_state.mark = text_op.mark;

			// pop event
			ui_event_pop(event);
		}
	}

	frame->string = str(buffer, *out_size);

	// mouse interaction
	ui_interaction interaction = ui_frame_interaction(frame);
	u32 delta_unit = 0;
	vec2_t text_align_pos = ui_text_align(frame->font, frame->font_size, frame->string, frame->rect, frame->text_alignment);
	vec2_t mouse_pos = ui_state.mouse_pos;
	vec2_t rel_pos = vec2_sub(mouse_pos, text_align_pos);
	u32 index = ui_text_index_from_offset(frame->font, frame->font_size, frame->string, rel_pos.x);

	if (interaction & ui_interaction_left_pressed) {
		ui_state.mark.column = index;
		ui_state.last_click_index[0] = index;
	}
	
	if (interaction & ui_interaction_left_dragging) {
		ui_state.cursor.column = index;
	}

	return interaction;
}

function ui_interaction
ui_combo(str_t label, i32* current, const char** items, u32 item_count) {

	// build parent frame
	ui_set_next_hover_cursor(os_cursor_hand_point);
	ui_set_next_layout_axis(ui_layout_axis_x);

	u32 flags =
		ui_frame_flag_clickable | ui_frame_flag_draw_background_light |
		ui_frame_flag_draw_hover_effects | ui_frame_flag_draw_active_effects |
		ui_frame_flag_draw_shadow | ui_frame_flag_draw_border_light;
	ui_key_t key = ui_key_from_string({ 0 }, label);
	ui_frame_t* parent_frame = ui_frame_from_key(key, flags);
	
	// set combo text

	ui_push_parent(parent_frame);
	{
		str_t combo_label = label;
		if (*current >= 0) {
			combo_label = str(items[*current]);
		}

		ui_set_next_pref_width(ui_size_percent(1.0f));
		ui_set_next_pref_height(ui_size_percent(1.0f));
		ui_label(combo_label);

		ui_set_next_pref_width(ui_size_pixel(rect_height(parent_frame->rect), 1.0f));
		ui_set_next_font(ui_state.default_icon_font);
		ui_frame_t* icon_frame = ui_frame_from_string(str(""), ui_frame_flag_draw_text);
		ui_frame_set_display_text(icon_frame, str("v"));

	}
	ui_pop_parent();

	b8 frame_focused = ui_key_equals(ui_state.focused_frame_key, key);

	// popup
	if (frame_focused) {
		ui_set_next_fixed_x(parent_frame->rect.x0);
		ui_set_next_fixed_y(parent_frame->rect.y1);
		ui_set_next_pref_width(ui_top_pref_width());
		ui_set_next_pref_width(ui_size_by_child(1.0f));
		ui_frame_t* popup_frame = ui_frame_from_string(str(""), ui_frame_flag_draw_background_dark | ui_frame_flag_floating | ui_frame_flag_fixed_size);
	
		ui_push_parent(popup_frame);
		
		for (i32 i = 0; i < item_count; i++) {
			
			if (*current == i) {
				//ui_palette_t palette = ui_state.default_palette;
				//palette.dark_background = palette.accent;
				//ui_set_next_palette(&palette);
			}

			ui_set_next_text_alignment(ui_text_alignment_left);
			ui_interaction item_interaction = ui_buttonf("%s##%s", items[i], label.data);
			if (item_interaction & ui_interaction_left_pressed) {
				*current = i;
			}
		}

		ui_pop_parent();

	}

	ui_interaction interaction = ui_frame_interaction(parent_frame);

	return interaction;
}

// widget draw functions

function void 
ui_slider_draw_function(ui_frame_t* frame) {
	
	// get data
	ui_slider_data_t* data = (ui_slider_data_t*)frame->custom_draw_data;

	rect_t bar_rect = frame->rect;
	bar_rect.x1 = lerp(bar_rect.x0, bar_rect.x1, data->value);

	color_t color = frame->palette->accent;
	color.a = 0.3f;
	draw_set_next_color(color);
	draw_set_next_radius(frame->rounding);
	draw_push_rect(bar_rect);
}

function void 
ui_color_hue_bar(ui_frame_t* frame) {

	// get data
	ui_color_data_t* data = (ui_color_data_t*)frame->custom_draw_data;

	// frame info
	f32 frame_width = rect_width(frame->rect);
	f32 frame_height = rect_height(frame->rect);

	// unpack color
	color_t hue_col = color_hsv_to_rgb(color(data->hue, 1.0f, 1.0f, 1.0f, color_format_hsv));
	
	// draw hue bars
	{
		f32 step = 1.0f / 6.0f;
		color_t segments[] = {
			color_hsv_to_rgb({0 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
			color_hsv_to_rgb({1 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
			color_hsv_to_rgb({2 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
			color_hsv_to_rgb({3 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
			color_hsv_to_rgb({4 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
			color_hsv_to_rgb({5 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
			color_hsv_to_rgb({6 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
		};

		for (i32 i = 0; i < 6; i++)  {
			draw_set_next_color0(segments[i + 0]);
			draw_set_next_color1(segments[i + 0]);
			draw_set_next_color2(segments[i + 1]);
			draw_set_next_color3(segments[i + 1]);

			// TODO: this is a hack and looks ugly. find a better way to fix this.
			f32 fix = 0.0f;

			// frame rounding
			if (i == 0) {
				draw_set_next_radius2(frame->rounding.z);
				draw_set_next_radius3(frame->rounding.w);
			} else if (i == 5) {
				draw_set_next_radius0(frame->rounding.x);
				draw_set_next_radius1(frame->rounding.y);
			} else {
				fix = 1.0f;
			}

			f32 x0 = roundf(frame->rect.x0 + (step *( i + 0)) * frame_width) - fix;
			f32 x1 = roundf(frame->rect.x0 + (step * (i + 1)) * frame_width) + fix;
			rect_t segment = rect(x0, frame->rect.y0, x1, frame->rect.y1);
			draw_push_rect(segment);
		}

	}

	// draw hue indicator
	{
		vec2_t indicator_pos = vec2(frame->rect.x0 + (data->hue * frame_width), (frame->rect.y0 + frame->rect.y1) * 0.5f);

		// borders
		draw_set_next_color(color(0x151515ff));
		draw_push_circle(indicator_pos, 8.0f, 0.0f, 360.0f);

		draw_set_next_color(color(0xe2e2e2ff));
		draw_push_circle(indicator_pos, 7.0f, 0.0f, 360.0f);

		// color
		draw_set_next_color(hue_col);
		draw_push_circle(indicator_pos, 6.0f, 0.0f, 360.0f);
	}
}

function void 
ui_color_sat_val_quad_draw_function(ui_frame_t* frame) {

	// get data
	ui_color_data_t* data = (ui_color_data_t*)frame->custom_draw_data;
	
	// unpack color
	color_t hue_col = color_hsv_to_rgb(color(data->hue, 1.0f, 1.0f, 1.0f, color_format_hsv));
	color_t rgb_col = color_hsv_to_rgb(color(data->hue, data->sat, data->val, 1.0f, color_format_hsv));

	// frame info
	f32 frame_width = rect_width(frame->rect);
	f32 frame_height = rect_height(frame->rect);

	// draw hue quad
	draw_set_next_color0(color(0xffffffff));
	draw_set_next_color1(color(0x000000ff));
	draw_set_next_color2(hue_col);
	draw_set_next_color3(color(0x000000ff));
	draw_set_next_radius(frame->rounding);
	draw_push_rect(frame->rect);

	// draw indicator
	vec2_t indicator_pos = vec2(
		frame->rect.x0 + (data->sat * frame_width), 
		frame->rect.y0 + ((1.0f - data->val) * frame_height)
	);

	{
		// borders
		draw_set_next_color(color(0x151515ff));
		draw_push_circle(indicator_pos, 8.0f, 0.0f, 360.0f);

		draw_set_next_color(color(0xe2e2e2ff));
		draw_push_circle(indicator_pos, 7.0f, 0.0f, 360.0f);

		// color
		draw_set_next_color(rgb_col);
		draw_push_circle(indicator_pos, 6.0f, 0.0f, 360.0f);
	}


}

function void
ui_color_wheel_draw_function(ui_frame_t* frame) {

	// get data
	ui_color_data_t* data = (ui_color_data_t*)frame->custom_draw_data;

	// unpack color
	color_t hue_col = color_hsv_to_rgb(color(data->hue, 1.0f, 1.0f, 1.0f, color_format_hsv));
	color_t rgb_col = color_hsv_to_rgb(color(data->hue, data->sat, data->val, 1.0f, color_format_hsv));

	// frame info
	f32 frame_width = rect_width(frame->rect);
	f32 frame_height = rect_height(frame->rect);
	vec2_t frame_center = rect_center(frame->rect);
	f32 wheel_radius = min(frame_width, frame_height) * 0.5f;
	f32 wheel_thickness = 20.0f;

	// calculate tri points
	f32 tri_dist = (wheel_radius - wheel_thickness) * 0.9f;
	vec2_t tri_p0 = vec2_add(frame_center, vec2_from_angle(data->hue * (2.0f * f32_pi), tri_dist));
	vec2_t tri_p1 = vec2_add(frame_center, vec2_from_angle((data->hue - 0.3333f) * (2.0f * f32_pi), tri_dist));
	vec2_t tri_p2 = vec2_add(frame_center, vec2_from_angle((data->hue + 0.3333f) * (2.0f * f32_pi), tri_dist));

	// draw hue wheel
	{
		// draw shadow
		draw_set_next_color(frame->palette->shadow);
		draw_set_next_thickness(wheel_thickness);
		draw_push_circle(vec2_add(frame_center, 1.0f), wheel_radius, 0.0f, 360.0f);

		// draw hue arcs
		f32 step = 1.0f / 6.0f;
		color_t segments[] = {
			color_hsv_to_rgb({0 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
			color_hsv_to_rgb({1 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
			color_hsv_to_rgb({2 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
			color_hsv_to_rgb({3 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
			color_hsv_to_rgb({4 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
			color_hsv_to_rgb({5 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
			color_hsv_to_rgb({6 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
		};

		for (i32 i = 0; i < 6; i++) {
			draw_set_next_color0(segments[i + 0]);
			draw_set_next_color1(segments[i + 1]);
			draw_set_next_color2(segments[i + 0]);
			draw_set_next_color3(segments[i + 1]);
			draw_set_next_thickness(wheel_thickness);

			f32 start_angle = ((i + 0) * step) * 360.0f;
			f32 end_angle = ((i + 1) * step) * 360.0f;
			draw_push_circle(frame_center, wheel_radius, start_angle, end_angle);
		}
	}
	
	// draw sat val triangle
	{	

		// border
		draw_set_next_color(frame->palette->shadow);
		draw_push_tri(vec2_add(tri_p0, 1.0f), vec2_add(tri_p1, 1.0f), vec2_add(tri_p2, 1.0f));

		draw_set_next_color0(hue_col);
		draw_set_next_color1(color(0xffffffff));
		draw_set_next_color2(color(0x000000ff));
		draw_push_tri(tri_p0, tri_p1, tri_p2);
	}
	
	// draw hue indicator
	{
		vec2_t indicator_pos = vec2_add(frame_center, vec2_from_angle(data->hue * 2.0f * f32_pi, wheel_radius - (wheel_thickness * 0.5f)));

		// borders
		draw_set_next_color(color(0x151515ff));
		draw_push_circle(indicator_pos, 8.0f, 0.0f, 360.0f);

		draw_set_next_color(color(0xe2e2e2ff));
		draw_push_circle(indicator_pos, 7.0f, 0.0f, 360.0f);

		//color
		draw_set_next_color(hue_col);
		draw_push_circle(indicator_pos, 6.0f, 0.0f, 360.0f);
	}

	// draw sat val indicator
	{
		vec2_t indicator_pos = vec2_lerp(vec2_lerp(tri_p1, tri_p0, clamp_01(data->sat)), tri_p2, clamp_01(1.0f - data->val));

		// borders
		draw_set_next_color(color(0x151515ff));
		draw_push_circle(indicator_pos, 8.0f, 0.0f, 360.0f);

		draw_set_next_color(color(0xe2e2e2ff));
		draw_push_circle(indicator_pos, 7.0f, 0.0f, 360.0f);

		// color
		draw_set_next_color(rgb_col);
		draw_push_circle(indicator_pos, 6.0f, 0.0f, 360.0f);
	}

}

function void 
ui_color_hue_sat_circle_draw_function(ui_frame_t* frame) {

	// get data
	ui_color_data_t* data = (ui_color_data_t*)frame->custom_draw_data;

	// unpack color
	color_t hue_col = color_hsv_to_rgb(color(data->hue, 1.0f, 1.0f, 1.0f, color_format_hsv));
	color_t rgb_col = color_hsv_to_rgb(color(data->hue, data->sat, data->val, 1.0f, color_format_hsv));
	color_t hue_sat_col = color_hsv_to_rgb(color(data->hue, data->sat, 1.0f, 1.0f, color_format_hsv));

	// frame info
	f32 frame_width = rect_width(frame->rect);
	f32 frame_height = rect_height(frame->rect);
	vec2_t frame_center = rect_center(frame->rect);
	f32 wheel_radius = min(frame_width, frame_height) * 0.5f;
	f32 wheel_thickness = wheel_radius * 0.15f;

	// draw hue sat circle
	{
		// draw shadow
		draw_set_next_color(frame->palette->shadow);
		draw_push_circle(vec2_add(frame_center, 1.0f), wheel_radius, 0.0f, 360.0f);

		// draw hue bars
		f32 step = 1.0f / 6.0f;
		color_t segments[] = {
			color_hsv_to_rgb({0 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
			color_hsv_to_rgb({1 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
			color_hsv_to_rgb({2 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
			color_hsv_to_rgb({3 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
			color_hsv_to_rgb({4 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
			color_hsv_to_rgb({5 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
			color_hsv_to_rgb({6 * step, 1.0f, 1.0f, 1.0f, color_format_hsv}),
		};

		for (i32 i = 0; i < 6; i++) {
			//gfx_set_next_color0(color(0xffffffff));
			//gfx_set_next_color1(color(0xffffffff));
			//gfx_set_next_color2(segments[i + 0]);
			//gfx_set_next_color3(segments[i + 1]);

			f32 start_angle = ((i + 0) * step) * 360.0f;
			f32 end_angle = ((i + 1) * step) * 360.0f;
			draw_push_circle(frame_center, wheel_radius, start_angle, end_angle);
		}
	}

	// draw hue sat indicator
	{
		vec2_t indicator_pos = vec2_add(frame_center, vec2_from_angle(data->hue * 2.0f * f32_pi, wheel_radius * data->sat));

		// borders
		draw_set_next_color(color(0x151515ff));
		draw_push_circle(indicator_pos, 8.0f, 0.0f, 360.0f);

		draw_set_next_color(color(0xe2e2e2ff));
		draw_push_circle(indicator_pos, 7.0f, 0.0f, 360.0f);

		// color
		draw_set_next_color(hue_sat_col);
		draw_push_circle(indicator_pos, 6.0f, 0.0f, 360.0f);
	}

}

function void
ui_color_val_bar_draw_function(ui_frame_t* frame) {

	// get data
	ui_color_data_t* data = (ui_color_data_t*)frame->custom_draw_data;

	// frame info
	f32 frame_width = rect_width(frame->rect);
	f32 frame_height = rect_height(frame->rect);

	// unpack color
	color_t hue_sat_col = color_hsv_to_rgb(color(data->hue, data->sat, 1.0f, 1.0f, color_format_hsv));
	color_t rgb_col = color_hsv_to_rgb(color(data->hue, data->sat, data->val, 1.0f, color_format_hsv));

	// draw val bar
	{
		draw_set_next_color0(color(0x000000ff));
		draw_set_next_color1(color(0x000000ff));
		draw_set_next_color2(hue_sat_col);
		draw_set_next_color3(hue_sat_col);
		draw_set_next_radius(frame->rounding);
		draw_push_rect(frame->rect);
	}

	// draw hue indicator
	{
		vec2_t indicator_pos = vec2(frame->rect.x0 + (data->val * frame_width), (frame->rect.y0 + frame->rect.y1) * 0.5f);

		// borders
		draw_set_next_color(color(0x151515ff));
		draw_push_circle(indicator_pos, 8.0f, 0.0f, 360.0f);

		draw_set_next_color(color(0xe2e2e2ff));
		draw_push_circle(indicator_pos, 7.0f, 0.0f, 360.0f);

		// color
		draw_set_next_color(rgb_col);
		draw_push_circle(indicator_pos, 6.0f, 0.0f, 360.0f);
	}

}

function void
ui_text_edit_draw_function(ui_frame_t* frame) {

	if (ui_key_equals(ui_state.focused_frame_key, frame->key)) {

		// get offsets
		f32 cursor_offset = ui_text_offset_from_index(frame->font, frame->font_size, frame->string, ui_state.cursor.column);
		f32 mark_offset = ui_text_offset_from_index(frame->font, frame->font_size, frame->string, ui_state.mark.column);
		vec2_t text_start = ui_text_align(frame->font, frame->font_size, frame->string, frame->rect, frame->text_alignment);
		ui_state.cursor_target_pos.x = cursor_offset;
		ui_state.mark_target_pos.x = mark_offset;

		// draw cursor
		draw_set_next_color(frame->palette->accent);

		f32 left = ui_state.cursor_pos.x;
		f32 right = cursor_offset;
		// swap
		if (left > right) {
			f32 temp = left;
			left = right;
			right = temp;
		}

		rect_t cursor_rect = rect(
			frame->rect.x0 + text_start.x + left - 1.0f,
			frame->rect.y0 + 2.0f,
			frame->rect.x0 + text_start.x + right + 1.0f,
			frame->rect.y1 - 2.0f
		);
		draw_push_rect(cursor_rect);

		// draw mark
		color_t mark_color = frame->palette->accent;
		mark_color.a = 0.3f;
		draw_set_next_color(mark_color);
		rect_t mark_rect = rect(
			frame->rect.x0 + text_start.x + cursor_offset, 
			frame->rect.y0 + 2.0f,
			frame->rect.x0 + text_start.x + mark_offset + 2.0f,
			frame->rect.y1 - 2.0f
		);
		draw_push_rect(mark_rect);

	}

}


// string functions

function str_t
ui_string_display_format(str_t string) {
	u32 pos = str_find_substr(string, str("##"));
	if (pos < string.size) {
		string.size = pos;
	}
	return string;
}

function str_t
ui_string_hash_format(str_t string) {
	u32 pos = str_find_substr(string, str("###"));
	if (pos < string.size) {
		string = str_skip(string, pos);
	}
	return string;
}

function str_t 
ui_string_replace_range(arena_t* arena, str_t string, ui_text_range_t range, str_t replace) {

	// get range
	i32 min = range.min.column;
	i32 max = range.max.column;

	// clamp range
	if (min > string.size) {
		min = 0;
	}
	if (max > string.size) {
		max = string.size;
	}

	// calculate new size
	u32 old_size = string.size;
	u32 new_size = old_size - (max - min) + replace.size;

	u8* new_string_data = (u8*)arena_alloc(arena, sizeof(u8) * new_size);
	memcpy(new_string_data, string.data, min);
	memcpy(new_string_data + min + replace.size, string.data + max, string.size - max);
	if (replace.data != 0) {
		memcpy(new_string_data + min, replace.data, replace.size);
	}

	str_t result = str((char*)new_string_data, new_size);
	return result;
}

function i32
ui_string_find_word_index(str_t string, i32 start_index, i32 delta) {

	if (delta == 1) {
		while (start_index < string.size && !char_is_whitespace(string.data[start_index])) { start_index++; }
		while (start_index < string.size && char_is_whitespace(string.data[start_index])) { start_index++; }
	} else if (delta == -1) {
		start_index--;
		while (start_index > 0 && char_is_whitespace(string.data[start_index])) { start_index--; }
		while (start_index > 0 && !char_is_whitespace(string.data[start_index - 1])) { start_index--; }
	}

	return start_index;
}

function void 
ui_string_find_word_boundaries(str_t string, i32 index, i32* word_start, i32* word_end) {

	// find start
	i32 start = index;
	while (start > 0 && !char_is_whitespace(string.data[start])) {
		start--;
	}

	// find end
	i32 end = index;
	while (end != string.size && !char_is_whitespace(string.data[end])) {
		end++;
	}

	*word_start = start;
	*word_end = end - 1;
}

// key functions

function u64
ui_hash_string(u64 seed, str_t string) {
	u64 result = seed;
	for (u64 i = 0; i < string.size; i += 1) {
		result = ((result << 5) + result) + string.data[i];
	}
	return result;
}

function ui_key_t
ui_key_from_string(ui_key_t seed, str_t string) {
	ui_key_t key = { 0 };
	if (string.size != 0) {
		str_t hash_string = ui_string_hash_format(string);
		key.data[0] = ui_hash_string(seed.data[0], hash_string);
	}
	return key;
}

function ui_key_t
ui_key_from_stringf(ui_key_t seed, char* fmt, ...) {

	va_list args;
	va_start(args, fmt);
	str_t string = str_formatv(ui_state.scratch_arena, fmt, args);
	va_end(args);

	ui_key_t result = ui_key_from_string(seed, string);

	return result;
}

function b8
ui_key_equals(ui_key_t a, ui_key_t b) {
	return (a.data[0] == b.data[0]);
}

// size functions

function ui_size_t 
ui_size(ui_size_type type, f32 value, f32 strictness) {
	return { type, value, strictness };
}

function ui_size_t
ui_size_pixel(f32 value, f32 strictness) {
	return { ui_size_type_pixel,  value, strictness };
}

function ui_size_t
ui_size_percent(f32 value) {
	return { ui_size_type_percent, value, 0.0f };
}

function ui_size_t
ui_size_by_child(f32 strictness) {
	return { ui_size_type_by_children, 0.0f, strictness };
}

function ui_size_t 
ui_size_em(f32 value, f32 strictness) {
	return { ui_size_type_pixel, ui_top_font_size() * value, strictness };
}

// alignment functions

function vec2_t
ui_text_align(font_t* font, f32 size, str_t text, rect_t rect, ui_text_alignment alignment) {

	vec2_t result = { 0 };

	font_metrics_t font_metrics = font_get_metrics(font, size);
	result.y = roundf((rect.y0 + rect.y1 + font_metrics.capital_height) * 0.5f - font_metrics.ascent); // temp fix

	switch (alignment) {
		default:
		case ui_text_alignment_left: {
			result.x = rect.x0 + 4.0f; // TODO: make this a text padding param for frames.
			break;
		}

		case ui_text_alignment_center: {
			f32 text_width = font_text_get_width(font, size, text); // TODO: cache string size.
			result.x = roundf((rect.x0 + rect.x1 - text_width) * 0.5f);
			result.x = max(result.x, rect.x0 + 4.0f);
			break;
		}

		case ui_text_alignment_right: {
			f32 text_width = font_text_get_width(font, size, text); // TODO: cache string size.
			result.x = roundf(rect.x1 - text_width - 4.0f); // TODO: also this one.
			result.x = max(result.x, rect.x0 + 4.0f);
			break;
		}

	}
	result.x = floorf(result.x);
	return result;

}

function f32
ui_text_offset_from_index(font_t* font, f32 font_size, str_t string, u32 index) {
	f32 width = 0.0f;
	for (u32 offset = 0; offset < index; offset++) {
		char c = *(string.data + offset);
		font_glyph_t* glyph = font_get_glyph(font, (u8)c, font_size);
		width += glyph->advance;
	}
	return width;
}

function u32 
ui_text_index_from_offset(font_t* font, f32 font_size, str_t string, f32 offset) {
	u32 result = 0;
	f32 advance = 0.0f;

	for (u32 index = 0; index < string.size; index++) {
		char c = *(string.data + index);
		font_glyph_t* glyph = font_get_glyph(font, (u8)c, font_size);
		if (offset < advance + (glyph->advance * 0.5f)) {
			result = index;
			break;
		}
		if (offset >= advance + (glyph->advance * 0.5f)) {
			result = index + 1;
		}
		advance += glyph->advance;
	}

	return result;
}

// text point

function ui_text_point_t
ui_text_point(i32 line, i32 column) {
	return { line, column };
}

function b8
ui_text_point_equals(ui_text_point_t a, ui_text_point_t b) {
	return a.line == b.line && a.column == b.column;
}

function b8
ui_text_point_less_than(ui_text_point_t a, ui_text_point_t b) {
	b8 result = 0;
	if (a.line < b.line) {
		result = 1;
	} else if (a.line == b.line) {
		result = a.column < b.column;
	}
	return result;
}

function ui_text_point_t
ui_text_point_min(ui_text_point_t a, ui_text_point_t b) {
	ui_text_point_t result = b;
	if (ui_text_point_less_than(a, b)) {
		result = a;
	}
	return result;
}

function ui_text_point_t
ui_text_point_max(ui_text_point_t a, ui_text_point_t b) {
	ui_text_point_t result = a;
	if (ui_text_point_less_than(a, b)) {
		result = b;
	}
	return result;
}

// text range

function ui_text_range_t
ui_text_range(ui_text_point_t min, ui_text_point_t max) {
	ui_text_range_t range = { 0 };
	if (ui_text_point_less_than(min, max)) {
		range.min = min;
		range.max = max;
	} else {
		range.min = max;
		range.max = min;
	}
	return range;
}

function ui_text_range_t
ui_text_range_intersects(ui_text_range_t a, ui_text_range_t b) {
	ui_text_range_t result = { 0 };
	result.min = ui_text_point_max(a.min, b.min);
	result.max = ui_text_point_min(a.max, b.max);
	if (ui_text_point_less_than(result.max, result.min)) {
		memset(&result, 0, sizeof(ui_text_range_t));
	}
	return result;
}

function ui_text_range_t
ui_text_range_union(ui_text_range_t a, ui_text_range_t b) {
	ui_text_range_t result = { 0 };
	result.min = ui_text_point_min(a.min, b.min);
	result.max = ui_text_point_max(a.max, b.max);
	return result;
}

function b8
ui_text_range_contains(ui_text_range_t r, ui_text_point_t pt) {
	b8 result = ((ui_text_point_less_than(r.min, pt) || ui_text_point_equals(r.min, pt)) && ui_text_point_less_than(pt, r.max));
	return result;
}

// event functions

function void 
ui_event_push(ui_event_t* event) {
	ui_event_t* new_event = (ui_event_t*)arena_calloc(ui_state.event_arena, sizeof(ui_event_t));
	memcpy(new_event, event, sizeof(ui_event_t));
	dll_push_back(ui_state.event_list.first, ui_state.event_list.last, new_event);
	ui_state.event_list.count++;
}

function void 
ui_event_pop(ui_event_t* event) {
	dll_remove(ui_state.event_list.first, ui_state.event_list.last, event);
	ui_state.event_list.count--;
}

function b8 
ui_key_pressed(os_key key, os_modifiers modifiers) {
	b8 result = false;
	for (ui_event_t* event = ui_state.event_list.first; event != nullptr; event = event->next) {
		if (event->type == ui_event_type_key_press && event->key == key && event->modifiers == modifiers) {
			result = true;
			ui_event_pop(event);
			break;
		}
	}
	return result;
}

function b8 
ui_key_released(os_key key, os_modifiers modifiers) {
	b8 result = false;
	for (ui_event_t* event = ui_state.event_list.first; event != nullptr; event = event->next) {
		if (event->type == ui_event_type_key_release && event->key == key && event->modifiers == modifiers) {
			result = true;
			ui_event_pop(event);
			break;
		}
	}
	return result;
}

function b8 
ui_text(u32 codepoint) {
	b8 result = false;
	for (ui_event_t* event = ui_state.event_list.first; event != nullptr; event = event->next) {
		if (event->type == ui_event_type_text && event->character == codepoint) {
			result = true;
			ui_event_pop(event);
			break;
		}
	}
	return result;
}

function ui_event_binding_t* 
ui_event_get_binding(os_key key, os_modifiers modifiers) {

	for (i32 i = 0; i < 64; i++) {
		ui_event_binding_t* binding = &ui_state.event_bindings[i];

		if (key == binding->key && modifiers == binding->modifiers) {
			return binding;
		}
	}

	return nullptr;
}

function ui_text_op_t
ui_event_to_text_op(arena_t* arena, ui_event_t* event, str_t string, ui_text_point_t cursor, ui_text_point_t mark) {

	ui_text_point_t next_cursor = cursor;
	ui_text_point_t next_mark = mark;
	ui_text_range_t range = { 0 };
	str_t replace = { 0 };
	str_t copy = { 0 };
	ivec2_t delta = event->delta;
	ivec2_t original_delta = delta;
	ui_text_op_flags flags = 0;

	switch (event->delta_unit) {

		case ui_event_delta_unit_char: {
			break;
		}

		case ui_event_delta_unit_word: {
			i32 dst_index = ui_string_find_word_index(string, cursor.column, (delta.x > 0) ? 1 : -1);
			delta.x = dst_index - cursor.column;
			break;
		}

		case ui_event_delta_unit_line:
		case ui_event_delta_unit_page:
		case ui_event_delta_unit_whole: {
			i32 dst_column = (delta.x > 0) ? (i32)string.size : 0;
			delta.x = dst_column - cursor.column;
			break;
		}
	}
	
	// zero delta
	if (!ui_text_point_equals(cursor, mark) && (event->flags & ui_event_flag_zero_delta)) {
		delta = ivec2(0);
	}

	// push next cursor
	if (ui_text_point_equals(cursor, mark) || !(event->flags & ui_event_flag_zero_delta)) {
		next_cursor.column += delta.x;
	}

	// deletion
	if (event->flags & ui_event_flag_delete) {
		ui_text_point_t new_pos = ui_text_point_min(next_cursor, next_mark);
		range = ui_text_range(next_cursor, next_mark);
		replace = str("");
		next_cursor = next_mark = new_pos;
	}
	
	// update mark
	if (!(event->flags & ui_event_flag_keep_mark)) {
		next_mark = next_cursor;
	}

	// insert
	if (event->character != 0) {
		range = ui_text_range(cursor, mark);
		replace = str_copy(arena, str((char*)(&event->character), 1));
		next_cursor = next_mark = ui_text_point(range.min.line, range.min.column + 1);
	}
	
	if (next_cursor.column > string.size + 1 || 0 > next_cursor.column || event->delta.y != 0) {
		flags |= ui_text_op_flag_invalid;
	}
	next_cursor.column = clamp(next_cursor.column, 0, string.size + replace.size);
	next_mark.column = clamp(next_mark.column, 0, string.size + replace.size);

	ui_text_op_t text_op = { 0 };
	text_op.flags = flags;
	text_op.replace = replace;
	text_op.copy = copy;
	text_op.range = range;
	text_op.cursor = next_cursor;
	text_op.mark = next_mark;
	return text_op;
}

// drag state
function void 
ui_store_drag_data(void* data, u32 size) {
	arena_clear(ui_state.drag_state_arena);
	ui_state.drag_state_data = arena_alloc(ui_state.drag_state_arena, size);
	ui_state.drag_state_size = size;
	memcpy(ui_state.drag_state_data, data, size);
}

function void* 
ui_get_drag_data() {
	return ui_state.drag_state_data;
}

function void 
ui_clear_drag_data() {
	arena_clear(ui_state.drag_state_arena);
	ui_state.drag_state_size = 0;
}

//  layout functions

function void 
ui_layout_solve_independent(ui_frame_t* root) {

	// x axis
	switch (root->pref_width.type) {
		case ui_size_type_pixel: {
			root->fixed_size.x = root->pref_width.value;
			break;
		}
	}

	// y axis
	switch (root->pref_height.type) {
		case ui_size_type_pixel: {
			root->fixed_size.y = root->pref_height.value;
			break;
		}
	}

	// recurse through children
	for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
		ui_layout_solve_independent(child);
	}

}

function void 
ui_layout_solve_upward_dependent(ui_frame_t* root) {

	// x axis
	switch (root->pref_width.type) {
		case ui_size_type_percent: {
			// find parent that has a fixed size	
			ui_frame_t* fixed_parent = nullptr;
			for (ui_frame_t* p = root->tree_parent; p != 0; p = p->tree_parent) {
				if (p->flags & ui_frame_flag_fixed_width ||
					p->pref_width.type == ui_size_type_pixel ||
					p->pref_width.type == ui_size_type_percent) {
					fixed_parent = p;
				}
			}
			
			// calculate percent size of fixed parent.
			f32 size = fixed_parent->fixed_size.x * root->pref_width.value;
			root->fixed_size.x = size;

			break;
		}
	}

	// y axis
	switch (root->pref_height.type) {
		case ui_size_type_percent: {
			// find parent that has a fixed size	
			ui_frame_t* fixed_parent = nullptr;
			for (ui_frame_t* p = root->tree_parent; p != 0; p = p->tree_parent) {
				if (p->flags & ui_frame_flag_fixed_width ||
					p->pref_height.type == ui_size_type_pixel ||
					p->pref_height.type == ui_size_type_percent) {
					fixed_parent = p;
				}
			}

			// calculate percent size of fixed parent.
			f32 size = fixed_parent->fixed_size.y * root->pref_height.value;
			root->fixed_size.y = size;

			break;
		}
	}

	// recurse through children
	for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
		ui_layout_solve_upward_dependent(child);
	}

}

function void
ui_layout_solve_downward_dependent(ui_frame_t* root) {

	// recurse through children
	for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
		ui_layout_solve_downward_dependent(child);
	}

	// x axis
	switch (root->pref_width.type) {
		case ui_size_type_by_children: {
			
			// find width of children
			f32 sum = 0.0f;
			for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
				if (!(child->flags & ui_frame_flag_floating_x)) {
					if (root->layout_axis == ui_layout_axis_x) {
						sum += child->fixed_size.x;
					} else {
						sum = max(sum, child->fixed_size.x);
					}
				}
			}
			root->fixed_size.x = sum;
			break;
		}
	}

	// y axis
	switch (root->pref_height.type) {
		case ui_size_type_by_children: {

			// find height of children
			f32 sum = 0.0f;
			for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
				if (!(child->flags & ui_frame_flag_floating_y)) {
					if (root->layout_axis == ui_layout_axis_y) {
						sum += child->fixed_size.y;
					} else {
						sum = max(sum, child->fixed_size.y);
					}
				}
			}
			root->fixed_size.y = sum;
			break;
		}
	}
	
}

function void 
ui_layout_solve_violations(ui_frame_t* root) {
	
	// x axis

	// fix children sizes in non layout axis
	if (root->layout_axis != ui_layout_axis_x && !(root->flags & ui_frame_flag_overflow_x)) {
		f32 allowed_size = root->fixed_size.x;
		for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
			if (!(child->flags & ui_frame_flag_floating_x)) {
				f32 child_size = child->fixed_size.x;
				f32 violation = child_size - allowed_size;
				f32 max_fixup = child_size;
				f32 fixup = clamp(violation, 0.0f, max_fixup);
				if (fixup > 0.0f) {
					child->fixed_size.x -= fixup;
				}
			}
		}
	}

	// fix children sizes in layout axis
	if (root->layout_axis == ui_layout_axis_x && !(root->flags & ui_frame_flag_overflow_x)) {

		// figure out total size
		f32 total_allowed_size = root->fixed_size.x;
		f32 total_size = 0.0f;
		f32 total_weighted_size = 0.0f;
		
		for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
			if (!(child->flags & ui_frame_flag_floating_x)) {
				total_size += child->fixed_size.x;
				total_weighted_size += child->fixed_size.x * (1.0f - child->pref_width.strictness);
			}
		}

		f32 violation = total_size - total_allowed_size;
		if (violation > 0.0f) {
			
			// find child fixup size
			f32 child_fixup_sum = 0.0f;
			f32* child_fixups = (f32*)arena_alloc(ui_state.scratch_arena, sizeof(f32) * root->child_count);
			
			u32 child_index = 0;
			for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
				if (!(child->flags & ui_frame_flag_floating_x)) {
					f32 fixup_size_this_child = child->fixed_size.x * (1.0f - child->pref_width.strictness);
					fixup_size_this_child = max(0.0f, fixup_size_this_child);
					child_fixups[child_index] = fixup_size_this_child;
					child_fixup_sum += fixup_size_this_child;
					child_index++;
				}
			}

			// fixup child size
			child_index = 0;
			for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
				if (!(child->flags & ui_frame_flag_floating_x)) {
					f32 fixup_percent = violation / total_weighted_size;
					fixup_percent = clamp_01(fixup_percent);
					child->fixed_size.x -= child_fixups[child_index] * fixup_percent;
					child_index++;
				}
			}
		}
	}

	// fix child percent sizes if we allow overflow
	if (root->flags & ui_frame_flag_overflow_x) {
		for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
			if (child->pref_width.type == ui_size_type_percent) {
				child->fixed_size.x = root->fixed_size.x * child->pref_width.value;
			}
		}
	}


	// y axis

	// fix children sizes in non layout axis
	if (root->layout_axis != ui_layout_axis_y && !(root->flags & ui_frame_flag_overflow_y)) {
		f32 allowed_size = root->fixed_size.y;
		for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
			if (!(child->flags & ui_frame_flag_floating_y)) {
				f32 child_size = child->fixed_size.y;
				f32 violation = child_size - allowed_size;
				f32 max_fixup = child_size;
				f32 fixup = clamp(violation, 0.0f, max_fixup);
				if (fixup > 0.0f) {
					child->fixed_size.y -= fixup;
				}
			}
		}
	}

	// fix children sizes in layout axis
	if (root->layout_axis == ui_layout_axis_y && !(root->flags & ui_frame_flag_overflow_y)) {

		// figure out total size
		f32 total_allowed_size = root->fixed_size.y;
		f32 total_size = 0.0f;
		f32 total_weighted_size = 0.0f;

		for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
			if (!(child->flags & ui_frame_flag_floating_y)) {
				total_size += child->fixed_size.y;
				total_weighted_size += child->fixed_size.y * (1.0f - child->pref_height.strictness);
			}
		}

		f32 violation = total_size - total_allowed_size;
		if (violation > 0.0f) {

			// find child fixup size
			f32 child_fixup_sum = 0.0f;
			f32* child_fixups = (f32*)arena_alloc(ui_state.scratch_arena, sizeof(f32) * root->child_count);

			u32 child_index = 0;
			for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
				if (!(child->flags & ui_frame_flag_floating_y)) {
					f32 fixup_size_this_child = child->fixed_size.y * (1.0f - child->pref_height.strictness);
					fixup_size_this_child = max(0.0f, fixup_size_this_child);
					child_fixups[child_index] = fixup_size_this_child;
					child_fixup_sum += fixup_size_this_child;
					child_index++;
				}
			}

			// fixup child size
			child_index = 0;
			for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
				if (!(child->flags & ui_frame_flag_floating_y)) {
					f32 fixup_percent = (violation / total_weighted_size);
					fixup_percent = clamp_01(fixup_percent);
					child->fixed_size.y -= child_fixups[child_index] * fixup_percent;
					child_index++;
				}
			}
		}
	}

	// fix child percent sizes if we allow overflow
	if (root->flags & (ui_frame_flag_overflow_y)) {
		for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
			if (child->pref_height.type == ui_size_type_percent) {
				child->fixed_size.y = root->fixed_size.y * child->pref_height.value;
			}
		}
	}

	// recurse through children
	for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
		ui_layout_solve_violations(child);
	}
}

function void 
ui_layout_solve_set_positions(ui_frame_t* root) {

	// x axis

	f32 layout_position = 0.0f;
	f32 bounds = 0.0f;

	for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {

		f32 original_pos = min(child->rect.x0, child->rect.x1);

		// calculate fixed position and size
		if (!(child->flags & ui_frame_flag_floating_x)) {
			child->fixed_position.x = layout_position;

			if (root->layout_axis == ui_layout_axis_x) {
				layout_position += child->fixed_size.x;
				bounds += child->fixed_size.x;
			} else {
				bounds = max(bounds, child->fixed_size.x);
			}
		}

		// determine final rect for child
		child->rect.x0 = root->rect.x0 + child->fixed_position.x - floorf(root->view_offset.x);
		child->rect.x1 = child->rect.x0 + child->fixed_size.x;

	}

	// store view bounds
	root->view_bounds.x = bounds + 1.0f;

	// y axis
	layout_position = 0.0f;
	bounds = 0.0f;

	for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {

		f32 original_pos = min(child->rect.y0, child->rect.y1);

		// calculate fixed position and size
		if (!(child->flags & ui_frame_flag_floating_y)) {
			child->fixed_position.y = layout_position;

			if (root->layout_axis == ui_layout_axis_y) {
				layout_position += child->fixed_size.y;
				bounds += child->fixed_size.y;
			} else {
				bounds = max(bounds, child->fixed_size.y);
			}
		}

		// determine final rect for child
		child->rect.y0 = root->rect.y0 + child->fixed_position.y - floorf(root->view_offset.y);
		child->rect.y1 = child->rect.y0 + child->fixed_size.y;

	}

	// store view bounds
	root->view_bounds.y = bounds + 2.0f;

	// recurse through children
	for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
		ui_layout_solve_set_positions(child);
	}
}


function ui_frame_t*
ui_row_begin() {
	ui_set_next_layout_axis(ui_layout_axis_x);
	ui_frame_t* frame = ui_frame_from_string(str(""), 0);
	ui_push_parent(frame);
	return frame;
}

function ui_interaction
ui_row_end() {
	ui_frame_t* frame = ui_pop_parent();
	ui_interaction interaction = ui_frame_interaction(frame);
	return interaction;
}

function ui_frame_t*
ui_column_begin() {
	ui_set_next_layout_axis(ui_layout_axis_y);
	ui_frame_t* frame = ui_frame_from_string(str(""), 0);
	ui_push_parent(frame);
	return frame;
}

function ui_interaction
ui_column_end() {
	ui_frame_t* frame = ui_pop_parent();
	ui_interaction interaction = ui_frame_interaction(frame);
	return interaction;
}


// frame functions

function ui_frame_t*
ui_frame_find(ui_key_t key) {

	ui_frame_t* result = nullptr;

	if (!ui_key_equals(key, { 0 })) {

		// search frame list
		for (ui_frame_t* frame = ui_state.frame_first; frame != 0; frame = frame->hash_next) {
			if (ui_key_equals(key, frame->key)) {
				result = frame;
				break;
			}
		}
	}

	return result;
}

function ui_frame_t* 
ui_frame_from_key(ui_key_t key, ui_frame_flags flags) {

	// check if frame existed last frame
	ui_frame_t* frame = ui_frame_find(key);
	b8 frame_is_new = (frame == nullptr);

	// duplicate
	if (!frame_is_new && (frame->last_build_index == ui_state.build_index)) {
		frame = nullptr;
		key = { 0 };
		frame_is_new = true;
	}

	// box doesn't need persistant data
	b8 frame_is_transient = ui_key_equals(key, { 0 });

	// if it didn't, grab from free list, or create one
	if (frame_is_new) {
		frame = !frame_is_transient ? ui_state.frame_free : nullptr;

		if (frame != nullptr) {
			stack_pop_n(ui_state.frame_free, hash_next);
		} else {
			frame = (ui_frame_t*)arena_alloc(ui_state.frame_arena, sizeof(ui_frame_t));
		}
		memset(frame, 0, sizeof(ui_frame_t));
		
	}

	frame->tree_child_first = frame->tree_child_last = frame->tree_next = frame->tree_prev = frame->tree_parent = nullptr;
	frame->child_count = 0;
	frame->flags = 0;

	// add to ui_state list
	if (frame_is_new && !frame_is_transient) {
		dll_push_back_np(ui_state.frame_first, ui_state.frame_last, frame, hash_next, hash_prev);
	}


	// add to tree and set parent
	ui_frame_t* parent = ui_top_parent();
	if (parent != nullptr) {
		dll_push_back_np(parent->tree_child_first, parent->tree_child_last, frame, tree_next, tree_prev);
		frame->tree_parent = parent;
		parent->child_count++;
	}

	// set build index
	if (frame_is_new) {
		frame->first_build_index = ui_state.build_index;
	}
	frame->last_build_index = ui_state.build_index;

	// fill struct
	frame->key = key;
	frame->is_transient = frame_is_transient;
	frame->flags = flags | ui_top_flags();
	frame->fixed_position.x = ui_top_fixed_x();
	frame->fixed_position.y = ui_top_fixed_y();
	frame->fixed_size.x = ui_top_fixed_width();
	frame->fixed_size.y = ui_top_fixed_height();
	frame->pref_width = ui_top_pref_width();
	frame->pref_height = ui_top_pref_height();
	frame->text_alignment = ui_top_text_alignment();
	frame->text_padding = ui_top_text_padding();
	frame->hover_cursor = ui_top_hover_cursor();
	frame->layout_axis = ui_top_layout_axis();
	frame->rounding.x = ui_top_rounding_00();
	frame->rounding.y = ui_top_rounding_01();
	frame->rounding.z = ui_top_rounding_10();
	frame->rounding.w = ui_top_rounding_11();
	frame->palette = ui_top_palette();
	frame->texture = ui_top_texture();
	frame->font = ui_top_font();
	frame->font_size = ui_top_font_size();
	
	ui_auto_pop_stacks();

	return frame;
}

function ui_frame_t* 
ui_frame_from_string(str_t string, ui_frame_flags flags) {
	
	ui_key_t key = ui_key_from_string({ 0 }, string);
	ui_frame_t* frame = ui_frame_from_key(key, flags);
	frame->string = ui_string_display_format(string);

	return frame;

}

function ui_frame_rec_t 
ui_frame_rec_depth_first(ui_frame_t* frame, ui_frame_t* root, u32 sibling_member_offset, u32 child_member_offset) {
	ui_frame_rec_t result = { 0 };

	result.next = nullptr;
	
	ui_frame_t** child_offset = (ui_frame_t**)((u8*)frame + child_member_offset);
	if (*child_offset != nullptr) {
		result.next = *child_offset;
		result.push_count++; // decend into children
	} else for (ui_frame_t* f = frame; f != 0 && f != root; f = f->tree_parent) {

		ui_frame_t** sibling_offset = (ui_frame_t**)((u8*)f + sibling_member_offset);
		if (*sibling_offset != nullptr) {
			result.next = *sibling_offset;
			break;
		}

		result.pop_count++; // ascend from children
	}

	return result;
}

function ui_interaction 
ui_frame_interaction(ui_frame_t* frame) {

	ui_interaction result = ui_interaction_none;

	// calculate interaction rect
	rect_t rect = frame->rect;
	for (ui_frame_t* f = frame->tree_parent; f != 0; f = f->tree_parent) {
		if (f->flags & ui_frame_flag_clip) {
			rect = rect_intersection(rect, f->rect);
		}
	}

	b8 view_scrolled = false;

	for (ui_event_t* event = ui_state.event_list.first; event != 0; event = event->next) {
		b8 taken = false;
		vec2_t mouse_pos = event->position;
		b8 mouse_in_bounds = rect_contains(rect, mouse_pos);

		// clicking
		if (frame->flags & ui_frame_flag_clickable) {
			
			// we mouse press on the frame
			if (mouse_in_bounds && event->type == ui_event_type_mouse_press) {
				ui_state.active_frame_key[event->mouse] = frame->key;
				ui_state.focused_frame_key = frame->key;
				result |= ui_interaction_left_pressed << event->mouse;

				// double and triple clicks
				if (ui_state.click_counter[event->mouse] >= 2) {
					if (ui_state.click_counter[event->mouse] % 2 == 0) {
						result |= ui_interaction_left_double_clicked << event->mouse;
					} else {
						result |= ui_interaction_left_triple_clicked << event->mouse;
					}
				}

				taken = true;
			}

			// we mouse release on the frame
			if (event->type == ui_event_type_mouse_release && ui_key_equals(ui_state.active_frame_key[event->mouse], frame->key)) {

				if (mouse_in_bounds) {
					result |= ui_interaction_left_clicked << event->mouse;
				}
				ui_state.active_frame_key[event->mouse] = { 0 };
				result |= ui_interaction_left_released << event->mouse;

				ui_state.active_frame_key[event->mouse] = { 0 };
				result |= ui_interaction_left_released << event->mouse;
				result |= ui_interaction_left_clicked << event->mouse;
				taken = true;
			}

		}
		
		// scrolling
		if (frame->flags & ui_frame_flag_scroll &&
			event->type == ui_event_type_mouse_scroll &&
			event->modifiers != os_modifier_ctrl &&
			mouse_in_bounds) {

			vec2_t scroll = event->scroll;

			// swap scrolling on shift
			if (event->modifiers & os_modifier_shift) {
				scroll.x = event->scroll.y;
				scroll.y = event->scroll.x;
			}

			// TODO: send scroll delta to ui_interation.

			taken = true;

		}

		// view scrolling
		if (frame->flags & ui_frame_flag_view_scroll &&
			event->type == ui_event_type_mouse_scroll &&
			event->modifiers != os_modifier_ctrl &&
			mouse_in_bounds) {
			
			vec2_t scroll = vec2_mul(event->scroll, -17.0f);
			
			// swap scrolling on shift
			if (event->modifiers & os_modifier_shift) {
				scroll.x = event->scroll.y;
				scroll.y = event->scroll.x;
			}

			if (!(frame->flags & ui_frame_flag_view_scroll_x)) {
				scroll.x = 0.0f;
			}

			if (!(frame->flags & ui_frame_flag_view_scroll_y)) {
				scroll.y = 0.0f;
			}

			// add to view target
			frame->view_offset_target = vec2_add(frame->view_offset_target, scroll);

			view_scrolled = true;
			taken = true;
		}

		if (taken) {
			ui_event_pop(event);
		}

	}

	// clamp view scrolling
	if (view_scrolled && frame->flags & ui_frame_flag_view_clamp) {
		vec2_t max_view_offset_target = vec2(
			max(0.0f, frame->view_bounds.x - frame->fixed_size.x), 
			max(0.0f, frame->view_bounds.y - frame->fixed_size.y)
		);
		if (frame->flags & ui_frame_flag_view_clamp_x) { frame->view_offset_target.x = clamp(frame->view_offset_target.x, 0.0f, max_view_offset_target.x); }
		if (frame->flags & ui_frame_flag_view_clamp_y) { frame->view_offset_target.y = clamp(frame->view_offset_target.y, 0.0f, max_view_offset_target.y); }

	}

	// mouse dragging
	if (frame->flags & ui_frame_flag_clickable) {
		for (i32 mouse_button = 0; mouse_button < os_mouse_button_count; mouse_button++) {
			if (ui_key_equals(ui_state.active_frame_key[mouse_button], frame->key) || (result & ui_interaction_left_pressed << mouse_button)) {
				result |= ui_interaction_left_dragging << mouse_button;
			}
			if ((result & ui_interaction_left_dragging << mouse_button) &&
				ui_state.click_counter[mouse_button] >= 2) {
				if (ui_state.click_counter[mouse_button] % 2 == 0) {
					result |= ui_interaction_left_double_dragging << mouse_button;
				} else {
					result |= ui_interaction_left_triple_dragging << mouse_button;
				}
			}
		}
	}

	// hovering
	vec2_t mouse_pos = ui_state.window->mouse_pos;
	if ((frame->flags & ui_frame_flag_clickable) && 
		(rect_contains(rect, mouse_pos)) &&
		(ui_key_equals(ui_state.hovered_frame_key, {0})|| ui_key_equals(ui_state.hovered_frame_key, frame->key)) && 
		(ui_key_equals(ui_state.active_frame_key[os_mouse_button_left], {0}) || ui_key_equals(ui_state.active_frame_key[os_mouse_button_left], frame->key)) &&
		(ui_key_equals(ui_state.active_frame_key[os_mouse_button_middle], {0}) || ui_key_equals(ui_state.active_frame_key[os_mouse_button_middle], frame->key)) &&
		(ui_key_equals(ui_state.active_frame_key[os_mouse_button_right], {0}) || ui_key_equals(ui_state.active_frame_key[os_mouse_button_right], frame->key))) {
		ui_state.hovered_frame_key = frame->key;
		result |= ui_interaction_hovered;
	}
	
	return result;
}

function void
ui_frame_set_display_text(ui_frame_t* frame, str_t text) {
	frame->string = text;
}

function void
ui_frame_set_custom_draw(ui_frame_t* frame, ui_frame_custom_draw_func* func, void* data) {
	frame->custom_draw_func = func;
	frame->custom_draw_data = data;
	frame->flags |= ui_frame_flag_draw_custom;
}

// frame list

function void 
ui_frame_list_push(arena_t* arena, ui_frame_list_t* frame_list, ui_frame_t* frame) {
	ui_frame_node_t* frame_node = (ui_frame_node_t*)arena_alloc(arena, sizeof(ui_frame_node_t));
	frame_node->frame = frame;
	dll_push_back(frame_list->first, frame_list->last, frame_node);
	frame_list->count++;
}


// stack functions

// macro magic >:)

#define ui_stack_top_impl(name, type) \
function type \
ui_top_##name() { \
	return ui_state.name##_stack.top->v; \
} \

#define ui_stack_push_impl(name, type) \
function type \
ui_push_##name(type v) { \
ui_##name##_node_t* node = ui_state.name##_stack.free; \
if (node != 0) { \
	stack_pop(ui_state.name##_stack.free); \
} else { \
	node = (ui_##name##_node_t*)arena_alloc(ui_state.per_frame_arena, sizeof(ui_##name##_node_t)); \
} \
type old_value = ui_state.name##_stack.top->v; \
node->v = v; \
stack_push(ui_state.name##_stack.top, node); \
ui_state.name##_stack.auto_pop = 0; \
return old_value; \
} \

#define ui_stack_pop_impl(name, type) \
function type \
ui_pop_##name() { \
ui_##name##_node_t* popped = ui_state.name##_stack.top; \
if (popped != 0) { \
	stack_pop(ui_state.name##_stack.top); \
	stack_push(ui_state.name##_stack.free, popped); \
	ui_state.name##_stack.auto_pop = 0; \
} \
return popped->v; \
} \

#define ui_stack_set_next_impl(name, type) \
function type \
ui_set_next_##name(type v) { \
ui_##name##_node_t* node = ui_state.name##_stack.free; \
if (node != 0) { \
	stack_pop(ui_state.name##_stack.free); \
} else { \
	node = (ui_##name##_node_t*)arena_alloc(ui_state.per_frame_arena, sizeof(ui_##name##_node_t)); \
} \
type old_value = ui_state.name##_stack.top->v; \
node->v = v; \
stack_push(ui_state.name##_stack.top, node); \
ui_state.name##_stack.auto_pop = 1; \
return old_value; \
} \

#define ui_stack_auto_pop_impl(name) \
if (ui_state.name##_stack.auto_pop) { ui_pop_##name(); ui_state.name##_stack.auto_pop = 0; }

#define ui_stack_impl(name, type)\
ui_stack_top_impl(name, type)\
ui_stack_push_impl(name, type)\
ui_stack_pop_impl(name, type)\
ui_stack_set_next_impl(name, type)\

function void
ui_auto_pop_stacks() {
	
	ui_stack_auto_pop_impl(parent);
	ui_stack_auto_pop_impl(flags);
	ui_stack_auto_pop_impl(fixed_x);
	ui_stack_auto_pop_impl(fixed_y);
	ui_stack_auto_pop_impl(fixed_width);
	ui_stack_auto_pop_impl(fixed_height);
	ui_stack_auto_pop_impl(pref_width);
	ui_stack_auto_pop_impl(pref_height);
	ui_stack_auto_pop_impl(text_alignment);
	ui_stack_auto_pop_impl(text_padding);
	ui_stack_auto_pop_impl(hover_cursor);
	ui_stack_auto_pop_impl(layout_axis);
	ui_stack_auto_pop_impl(rounding_00);
	ui_stack_auto_pop_impl(rounding_01);
	ui_stack_auto_pop_impl(rounding_10);
	ui_stack_auto_pop_impl(rounding_11);
	ui_stack_auto_pop_impl(palette);
	ui_stack_auto_pop_impl(texture);
	ui_stack_auto_pop_impl(font);
	ui_stack_auto_pop_impl(font_size);
	ui_stack_auto_pop_impl(focus_hot);
	ui_stack_auto_pop_impl(focus_active);

}

ui_stack_impl(parent, ui_frame_t*)
ui_stack_impl(flags, ui_frame_flags)
ui_stack_impl(fixed_x, f32)
ui_stack_impl(fixed_y, f32)
ui_stack_impl(fixed_width, f32)
ui_stack_impl(fixed_height, f32)
ui_stack_impl(pref_width, ui_size_t)
ui_stack_impl(pref_height, ui_size_t)
ui_stack_impl(text_alignment, ui_text_alignment)
ui_stack_impl(text_padding, f32)
ui_stack_impl(hover_cursor, os_cursor)
ui_stack_impl(layout_axis, ui_layout_axis)
ui_stack_impl(rounding_00, f32)
ui_stack_impl(rounding_01, f32)
ui_stack_impl(rounding_10, f32)
ui_stack_impl(rounding_11, f32)
ui_stack_impl(palette, ui_palette_t*)
ui_stack_impl(texture, gfx_texture_t*)
ui_stack_impl(font, font_t*)
ui_stack_impl(font_size, f32)
ui_stack_impl(focus_hot, ui_focus_type)
ui_stack_impl(focus_active, ui_focus_type)

// groups
function void
ui_push_rounding(f32 rounding) {
	ui_push_rounding_00(rounding);
	ui_push_rounding_01(rounding);
	ui_push_rounding_10(rounding);
	ui_push_rounding_11(rounding);
}

function void
ui_pop_rounding() {
	ui_pop_rounding_00();
	ui_pop_rounding_01();
	ui_pop_rounding_10();
	ui_pop_rounding_11();
}

function void 
ui_set_next_rounding(f32 rounding) {
	ui_set_next_rounding_00(rounding);
	ui_set_next_rounding_01(rounding);
	ui_set_next_rounding_10(rounding);
	ui_set_next_rounding_11(rounding);
}

#endif // UI_CPP