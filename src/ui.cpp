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
	ui_state.drag_state_arena = arena_create(megabytes(64));
	ui_state.scratch_arena = arena_create(megabytes(64));

	ui_state.window = nullptr;
	ui_state.renderer = nullptr;

	// default resources
	ui_state.default_palette.dark_background = color(0x282828ff);
	ui_state.default_palette.dark_border = color(0x3d3d3dff);
	ui_state.default_palette.light_background = color(0x3d3d3dff);
	ui_state.default_palette.light_border = color(0x525252ff);
	ui_state.default_palette.shadow = color(0x15151580);
	ui_state.default_palette.hover = color(0x151515ff);
	ui_state.default_palette.active = color(0x151515ff);
	ui_state.default_palette.text = color(0xe2e2e2ff);
	ui_state.default_palette.accent = color(0x38BAD780); // blue

	ui_state.default_texture = gfx_state.default_texture;
	ui_state.default_font = gfx_font_load(str("res/fonts/segoe_ui.ttf"));
	ui_state.default_icon_font = gfx_font_load(str("res/fonts/icons.ttf"));
	
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
	ui_default_init(layout_axis, ui_layout_axis_y);
	ui_default_init(rounding_00, 2.0f);
	ui_default_init(rounding_01, 2.0f);
	ui_default_init(rounding_10, 2.0f);
	ui_default_init(rounding_11, 2.0f);
	ui_default_init(palette, &ui_state.default_palette);
	ui_default_init(texture, ui_state.default_texture);
	ui_default_init(font, ui_state.default_font);
	ui_default_init(font_size, 9.0f);

	// build state
	ui_state.build_index = 0;

}

function void
ui_release() {

	ui_state.window = nullptr;
	ui_state.renderer = nullptr;

	gfx_font_release(ui_state.default_font);

	// release arenas
	arena_release(ui_state.frame_arena);
	arena_release(ui_state.per_frame_arena);
	arena_release(ui_state.drag_state_arena);
	arena_release(ui_state.scratch_arena);

}

function void
ui_begin_frame(gfx_renderer_t* renderer) {

	// set context
	ui_state.window = renderer->window;
	ui_state.renderer = renderer;

	// set input
	ui_state.mouse_pos = ui_state.window->mouse_pos;
	ui_state.mouse_delta = ui_state.window->mouse_delta;

	// clear arenas
	arena_clear(ui_state.per_frame_arena);
	arena_clear(ui_state.scratch_arena);

	// reset event list
	ui_state.event_list = { 0 };

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
	ui_stack_reset(layout_axis);
	ui_stack_reset(rounding_00);
	ui_stack_reset(rounding_01);
	ui_stack_reset(rounding_10);
	ui_stack_reset(rounding_11);
	ui_stack_reset(palette);
	ui_stack_reset(texture);
	ui_stack_reset(font);
	ui_stack_reset(font_size);

	// add root frame
	str_t root_string = str_format(ui_state.scratch_arena, "%.*s_root_frame", ui_state.window->title.size, ui_state.window->title.data);
	ui_set_next_fixed_width((f32)ui_state.window->width);
	ui_set_next_fixed_height((f32)ui_state.window->height);
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

	// gather events
	for (os_event_t* os_event = os_state.event_list.first; os_event != 0; os_event = os_event->next) {
		ui_event_t ui_event = { 0 };
		ui_event_type type = ui_event_type_null;

		if (os_event->type != 0 && os_event->window == ui_state.window) {

			switch (os_event->type) {
				case os_event_type_key_press: { type = ui_event_type_key_press; break; }
				case os_event_type_key_release: { type = ui_event_type_key_release; break; }
				case os_event_type_mouse_press: { type = ui_event_type_mouse_press; break; }
				case os_event_type_mouse_release: { type = ui_event_type_mouse_release; break; }
				case os_event_type_mouse_move: { type = ui_event_type_mouse_move; break; }
				case os_event_type_text: { type = ui_event_type_text; break; }
				case os_event_type_mouse_scroll: { type = ui_event_type_mouse_scroll; break; }
			}

			ui_event.type = type;
			ui_event.key = os_event->key;
			ui_event.mouse = os_event->mouse;
			ui_event.modifiers = os_event->modifiers;
			ui_event.character = os_event->character;
			ui_event.position = os_event->position;
			ui_event.scroll = os_event->scroll;

			ui_event_push(&ui_event);
		}
		
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

	// layout pass
	ui_layout_solve_independent(ui_state.root);
	ui_layout_solve_upward_dependent(ui_state.root);
	ui_layout_solve_downward_dependent(ui_state.root);
	ui_layout_solve_violations(ui_state.root);
	ui_layout_solve_set_positions(ui_state.root);

	// animate
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

		frame->view_offset_target = vec2_add(frame->view_offset_target, vec2_mul(vec2_sub(frame->view_offset_target, frame->view_offset), fast_rate));
	}
	
	// draw
	gfx_renderer_t* renderer = ui_state.renderer;
	u32 depth = 0;
	for (ui_frame_t* frame = ui_state.root; frame != nullptr;) {

		// do recursive depth first search
		u32 sibling_member_offset = (u32)(&(((ui_frame_t*)0)->tree_prev));
		u32 child_member_offset = (u32)(&(((ui_frame_t*)0)->tree_child_last));
		ui_frame_rec_t recursion = ui_frame_rec_depth_first(frame, ui_state.root, sibling_member_offset, child_member_offset);

		// set depth
		frame->depth = depth;
		gfx_push_depth(depth);

		// grab frame info
		ui_palette_t* palette = frame->palette;

		// frame shadow
		if (frame->flags & ui_frame_flag_draw_shadow) {
			gfx_quad_params_t shadow_params = gfx_quad_params(palette->shadow, 0.0f, 0.0f, 0.33f);
			shadow_params.radii = frame->rounding;
			gfx_draw_quad(rect_translate(frame->rect, 1.0f), shadow_params);
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
			gfx_quad_params_t background_params = gfx_quad_params(background_color);
			background_params.radii = frame->rounding;
			gfx_draw_quad(frame->rect, background_params);

		}

		// border
		if (frame->flags & (ui_frame_flag_draw_border_light | ui_frame_flag_draw_border_dark)) {
			gfx_push_depth(depth + 1);
			gfx_quad_params_t border_params = gfx_quad_params(border_color, 0.0f, 1.0f);
			border_params.radii = frame->rounding;
			gfx_draw_quad(frame->rect, border_params);
			gfx_pop_depth();
		}
		
		// clip
		if (frame->flags & ui_frame_flag_clip) {
			rect_t top_clip = gfx_top_clip();
			rect_t new_clip = rect_shrink(frame->rect, 1.0f);
			if (top_clip.x1 != 0.0f || top_clip.y1 != 0.0f) {
				new_clip = rect_intersection(new_clip, top_clip);
			}
			gfx_push_clip(new_clip);
		}

		// text
		if (frame->flags & ui_frame_flag_draw_text) {

			// calculate text pos
			vec2_t text_pos;

			gfx_font_metrics_t font_metrics = gfx_font_get_metrics(frame->font, frame->font_size);
			text_pos.y = roundf((frame->rect.y0 + frame->rect.y1 + font_metrics.capital_height) * 0.5f - font_metrics.ascent) + 2.0f;
			switch (frame->text_alignment) {
				default: 
				case ui_text_alignment_left:{
					text_pos.x = frame->rect.x0 + frame->text_padding;
					break;
				}

				case ui_text_alignment_center: {
					f32 text_width = gfx_font_text_width(frame->font, frame->font_size, frame->string); // TODO: cache string size.
					text_pos.x = roundf((frame->rect.x0 + frame->rect.x1 - text_width) * 0.5f);
					text_pos.x = max(text_pos.x, frame->rect.x0 + frame->text_padding);
					break;
				}

				case ui_text_alignment_right: {
					f32 text_width = gfx_font_text_width(frame->font, frame->font_size, frame->string);
					text_pos.x = roundf(frame->rect.x1 - text_width - frame->text_padding);
					text_pos.x = max(text_pos.x, frame->rect.x0 + frame->text_padding);
					break;
				}
			}
			text_pos.x = floorf(text_pos.x);

			// TODO: truncate text if too long.
			// make text appear in front
			gfx_push_depth(depth + 1);
			{
				// text shadow
				gfx_text_params_t shadow_params = gfx_text_params(palette->shadow, frame->font, frame->font_size);
				gfx_draw_text(frame->string, vec2_add(text_pos, 1.0f), shadow_params);

				// text
				gfx_text_params_t text_params = gfx_text_params(palette->text, frame->font, frame->font_size);
				gfx_draw_text(frame->string, text_pos, text_params);
			}
			gfx_pop_depth();
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
				gfx_pop_clip();
			}
		}

		// pop depth
		gfx_pop_depth();

		if (!frame->is_transient) {
			depth += recursion.push_count * 5; // each 'layer' of nodes gets 5 layers.
			depth -= recursion.pop_count * 5;
		}

		frame = recursion.next;
	}


}

// widgets

function ui_interaction 
ui_button(str_t label) {

	ui_frame_flags flags =
		ui_frame_flag_clickable |
		ui_frame_flag_draw;

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

	arena_clear(ui_state.scratch_arena);
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
		*value = clamp_01(*value);
	}

	f32 percent = remap(*value, min, max, 0.0f, 1.0f);
	data->value = percent;

	return interaction;
}

function ui_interaction
ui_checkbox(str_t label, b8* value) {

	// build parent frame
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
	ui_frame_flags flags = ui_frame_flag_clickable;
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);
	ui_color_data_t* data = (ui_color_data_t*)arena_alloc(ui_state.per_frame_arena, sizeof(ui_color_data_t));
	ui_frame_set_custom_draw(frame, ui_color_wheel_draw_function, data);
	
	// calculate frame and mouse info
	vec2_t frame_center = rect_center(frame->rect);
	f32 frame_width = rect_width(frame->rect);
	f32 frame_height = rect_height(frame->rect);
	f32 outer_wheel_radius = min(frame_width, frame_height) * 0.5f;
	f32 inner_wheel_radius = outer_wheel_radius - (outer_wheel_radius * 0.15f);
	vec2_t mouse_pos = ui_state.mouse_pos;
	vec2_t dir = vec2_sub(frame_center, mouse_pos);
	f32 dist = vec2_length(dir);
	f32 tri_dist = inner_wheel_radius * 0.9f;
	vec2_t tri_p0 = vec2_add(frame_center, vec2_from_angle(*hue * (2.0f * f32_pi), tri_dist));
	vec2_t tri_p1 = vec2_add(frame_center, vec2_from_angle((*hue - 0.3333f) * (2.0f * f32_pi), tri_dist));
	vec2_t tri_p2 = vec2_add(frame_center, vec2_from_angle((*hue + 0.3333f) * (2.0f * f32_pi), tri_dist));

	// interactions
	
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
	ui_frame_flags flags = ui_frame_flag_clickable;
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

// widget draw functions

function void 
ui_slider_draw_function(ui_frame_t* frame) {
	
	// get data
	ui_slider_data_t* data = (ui_slider_data_t*)frame->custom_draw_data;

	rect_t bar_rect = frame->rect;
	bar_rect.x1 = lerp(bar_rect.x0, bar_rect.x1, data->value);

	gfx_quad_params_t params = gfx_quad_params(frame->palette->accent);
	params.radii = frame->rounding;
	gfx_draw_quad(bar_rect, params);
	
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
			gfx_quad_params_t quad_params;
			quad_params.col0 = segments[i + 0];
			quad_params.col1 = segments[i + 0];
			quad_params.col2 = segments[i + 1];
			quad_params.col3 = segments[i + 1];
			quad_params.radii = { 0.0f, 0.0f, 0.0f, 0.0f };
			quad_params.thickness = 0.0f;
			quad_params.softness = 0.33f;

			// TODO: this is a hack and looks ugly. find a better way to fix this.
			f32 fix = 0.0f;

			// frame rounding
			if (i == 0) {
				quad_params.radii = { 0.0f, 0.0f, frame->rounding.z, frame->rounding.w };
			} else if (i == 5) {
				quad_params.radii = { frame->rounding.x, frame->rounding.y, 0.0f, 0.0f };
			} else {
				fix = 1.0f;
			}

			f32 x0 = roundf(frame->rect.x0 + (step *( i + 0)) * frame_width) - fix;
			f32 x1 = roundf(frame->rect.x0 + (step * (i + 1)) * frame_width) + fix;
			rect_t segment = rect(x0, frame->rect.y0, x1, frame->rect.y1);
			gfx_draw_quad(segment, quad_params);
		}

	}

	// draw hue indicator
	gfx_push_depth(frame->depth + 2);
	{
		vec2_t indicator_pos = vec2(frame->rect.x0 + (data->hue * frame_width), (frame->rect.y0 + frame->rect.y1) * 0.5f);

		// border
		gfx_disk_params_t outer_border_params = gfx_disk_params(color(0x151515ff), 0.0f, 0.45f);
		gfx_draw_disk(indicator_pos, 8.0f, 0.0f, 360.0f, outer_border_params);

		gfx_disk_params_t inner_border_params = gfx_disk_params(color(0xe2e2e2ff), 0.0f, 0.45f);
		gfx_draw_disk(indicator_pos, 7.0f, 0.0f, 360.0f, inner_border_params);

		// color
		gfx_disk_params_t disk_params = gfx_disk_params(hue_col, 0.0f, 0.45f);
		gfx_draw_disk(indicator_pos, 6.0f, 0.0f, 360.0f, disk_params);
	}
	gfx_pop_depth();
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
	gfx_quad_params_t params;
	params.col0 = color(0xffffffff);
	params.col1 = color(0x000000ff);
	params.col2 = hue_col;
	params.col3 = color(0x000000ff);
	params.radii = frame->rounding;
	params.thickness = 0.0f;
	params.softness = 0.33f;
	gfx_draw_quad(frame->rect, params);

	// draw indicator
	vec2_t indicator_pos = vec2(
		frame->rect.x0 + (data->sat * frame_width), 
		frame->rect.y0 + ((1.0f - data->val) * frame_height)
	);
	gfx_push_depth(frame->depth + 2);
	{
		// border
		gfx_disk_params_t outer_border_params = gfx_disk_params(color(0x151515ff), 0.0f, 0.45f);
		gfx_draw_disk(indicator_pos, 8.0f, 0.0f, 360.0f, outer_border_params);

		gfx_disk_params_t inner_border_params = gfx_disk_params(color(0xe2e2e2ff), 0.0f, 0.45f);
		gfx_draw_disk(indicator_pos, 7.0f, 0.0f, 360.0f, inner_border_params);

		// color
		gfx_disk_params_t disk_params = gfx_disk_params(rgb_col, 0.0f, 0.45f);
		gfx_draw_disk(indicator_pos, 6.0f, 0.0f, 360.0f, disk_params);
	}
	gfx_pop_depth();


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
	f32 wheel_thickness = wheel_radius * 0.15f;

	// calculate tri points
	f32 tri_dist = (wheel_radius - wheel_thickness) * 0.9f;
	vec2_t tri_p0 = vec2_add(frame_center, vec2_from_angle(data->hue * (2.0f * f32_pi), tri_dist));
	vec2_t tri_p1 = vec2_add(frame_center, vec2_from_angle((data->hue - 0.3333f) * (2.0f * f32_pi), tri_dist));
	vec2_t tri_p2 = vec2_add(frame_center, vec2_from_angle((data->hue + 0.3333f) * (2.0f * f32_pi), tri_dist));

	// draw hue wheel
	{
		// draw shadow
		gfx_disk_params_t shadow_params = gfx_disk_params(frame->palette->shadow, wheel_radius * 0.15f);
		gfx_draw_disk(vec2_add(frame_center, 1.0f), wheel_radius, 0.0f, 360.0f, shadow_params);

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
			gfx_disk_params_t disk_params;
			disk_params.col0 = segments[i + 0];
			disk_params.col1 = segments[i + 1];
			disk_params.col2 = segments[i + 0];
			disk_params.col3 = segments[i + 1];
			disk_params.thickness = wheel_radius * 0.15f;
			disk_params.softness = 0.33f;

			f32 start_angle = ((i + 0) * step) * 360.0f;
			f32 end_angle = ((i + 1) * step) * 360.0f;
			gfx_draw_disk(frame_center, wheel_radius, start_angle, end_angle, disk_params);
		}
	}
	
	// draw sat val triangle
	{

		// draw shadow
		// TODO: there is some weird rendering artifact when I draw this shadow.
		//gfx_tri_params_t shadow_params = gfx_tri_params(frame->palette->shadow, 0.0f, 0.33f);
		//gfx_draw_tri(vec2_add(tri_p0, 1.0f), vec2_add(tri_p1, 1.0f), vec2_add(tri_p2, 1.0f), shadow_params);
			
		// draw sat val tri
		gfx_tri_params_t tri_params;
		tri_params.col0 = hue_col;
		tri_params.col1 = color(0xffffffff);
		tri_params.col2 = color(0x000000ff);
		tri_params.thickness = 0.0f;
		tri_params.softness = 0.33f;
		gfx_draw_tri(tri_p0, tri_p1, tri_p2, tri_params);

	}
	
	// draw hue indicator
	gfx_push_depth(frame->depth + 2);
	{
		vec2_t indicator_pos = vec2_add(frame_center, vec2_from_angle(data->hue * 2.0f * f32_pi, wheel_radius - (wheel_thickness * 0.5f)));

		// border
		gfx_disk_params_t outer_border_params = gfx_disk_params(color(0x151515ff), 0.0f, 0.45f);
		gfx_draw_disk(indicator_pos, 8.0f, 0.0f, 360.0f, outer_border_params);

		gfx_disk_params_t inner_border_params = gfx_disk_params(color(0xe2e2e2ff), 0.0f, 0.45f);
		gfx_draw_disk(indicator_pos, 7.0f, 0.0f, 360.0f, inner_border_params);

		// color
		gfx_disk_params_t disk_params = gfx_disk_params(hue_col, 0.0f, 0.45f);
		gfx_draw_disk(indicator_pos, 6.0f, 0.0f, 360.0f, disk_params);
	}
	gfx_pop_depth();

	// draw sat val indicator
	gfx_push_depth(frame->depth + 2);
	{
		vec2_t indicator_pos = vec2_lerp(vec2_lerp(tri_p1, tri_p0, clamp_01(data->sat)), tri_p2, clamp_01(1.0f - data->val));

		// border
		gfx_disk_params_t outer_border_params = gfx_disk_params(color(0x151515ff), 0.0f, 0.45f);
		gfx_draw_disk(indicator_pos, 8.0f, 0.0f, 360.0f, outer_border_params);

		gfx_disk_params_t inner_border_params = gfx_disk_params(color(0xe2e2e2ff), 0.0f, 0.45f);
		gfx_draw_disk(indicator_pos, 7.0f, 0.0f, 360.0f, inner_border_params);

		// color
		gfx_disk_params_t disk_params = gfx_disk_params(rgb_col, 0.0f, 0.45f);
		gfx_draw_disk(indicator_pos, 6.0f, 0.0f, 360.0f, disk_params);
	}
	gfx_pop_depth();

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
		gfx_disk_params_t shadow_params = gfx_disk_params(frame->palette->shadow, 0.0f);
		gfx_draw_disk(vec2_add(frame_center, 1.0f), wheel_radius, 0.0f, 360.0f, shadow_params);

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
			gfx_disk_params_t disk_params;
			disk_params.col0 = color(0xffffffff);
			disk_params.col1 = color(0xffffffff);
			disk_params.col2 = segments[i + 0];
			disk_params.col3 = segments[i + 1];
			disk_params.thickness = 0.0f;
			disk_params.softness = 0.33f;

			f32 start_angle = ((i + 0) * step) * 360.0f;
			f32 end_angle = ((i + 1) * step) * 360.0f;
			gfx_draw_disk(frame_center, wheel_radius, start_angle, end_angle, disk_params);
		}
	}

	// draw hue sat indicator
	gfx_push_depth(frame->depth + 2);
	{
		vec2_t indicator_pos = vec2_add(frame_center, vec2_from_angle(data->hue * 2.0f * f32_pi, wheel_radius * data->sat));

		// border
		gfx_disk_params_t outer_border_params = gfx_disk_params(color(0x151515ff), 0.0f, 0.45f);
		gfx_draw_disk(indicator_pos, 8.0f, 0.0f, 360.0f, outer_border_params);

		gfx_disk_params_t inner_border_params = gfx_disk_params(color(0xe2e2e2ff), 0.0f, 0.45f);
		gfx_draw_disk(indicator_pos, 7.0f, 0.0f, 360.0f, inner_border_params);

		// color
		gfx_disk_params_t disk_params = gfx_disk_params(hue_sat_col, 0.0f, 0.45f);
		gfx_draw_disk(indicator_pos, 6.0f, 0.0f, 360.0f, disk_params);
	}
	gfx_pop_depth();

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
		gfx_quad_params_t quad_params;
		quad_params.col0 = color(0x000000ff);
		quad_params.col1 = color(0x000000ff);
		quad_params.col2 = hue_sat_col;
		quad_params.col3 = hue_sat_col;
		quad_params.radii = frame->rounding;
		quad_params.thickness = 0.0f;
		quad_params.softness = 0.33f;

		gfx_draw_quad(frame->rect, quad_params);
	}

	// draw hue indicator
	gfx_push_depth(frame->depth + 2);
	{
		vec2_t indicator_pos = vec2(frame->rect.x0 + (data->val * frame_width), (frame->rect.y0 + frame->rect.y1) * 0.5f);

		// border
		gfx_disk_params_t outer_border_params = gfx_disk_params(color(0x151515ff), 0.0f, 0.45f);
		gfx_draw_disk(indicator_pos, 8.0f, 0.0f, 360.0f, outer_border_params);

		gfx_disk_params_t inner_border_params = gfx_disk_params(color(0xe2e2e2ff), 0.0f, 0.45f);
		gfx_draw_disk(indicator_pos, 7.0f, 0.0f, 360.0f, inner_border_params);

		// color
		gfx_disk_params_t disk_params = gfx_disk_params(rgb_col, 0.0f, 0.45f);
		gfx_draw_disk(indicator_pos, 6.0f, 0.0f, 360.0f, disk_params);
	}
	gfx_pop_depth();

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
ui_text_align(gfx_font_t* font, f32 size, str_t text, rect_t rect, ui_text_alignment alignment) {

	vec2_t result = { 0 };

	gfx_font_metrics_t font_metrics = gfx_font_get_metrics(font, size);
	result.y = roundf((rect.y0 + rect.y1 + font_metrics.capital_height) * 0.5f - font_metrics.ascent) + 2.0f; // temp fix

	switch (alignment) {
		default:
		case ui_text_alignment_left: {
			result.x = rect.x0 + 4.0f; // TODO: make this a text padding param for frames.
			break;
		}

		case ui_text_alignment_center: {
			f32 text_width = gfx_font_text_width(font, size, text); // TODO: cache string size.
			result.x = roundf((rect.x0 + rect.x1 - text_width) * 0.5f);
			result.x = max(result.x, rect.x0 + 4.0f);
			break;
		}

		case ui_text_alignment_right: {
			f32 text_width = gfx_font_text_width(font, size, text); // TODO: cache string size.
			result.x = roundf(rect.x1 - text_width - 4.0f); // TODO: also this one.
			result.x = max(result.x, rect.x0 + 4.0f);
			break;
		}

	}
	result.x = floorf(result.x);
	return result;

}

// event functions

function void 
ui_event_push(ui_event_t* event) {
	ui_event_t* new_event = (ui_event_t*)arena_alloc(ui_state.per_frame_arena, sizeof(ui_event_t));
	memcpy(new_event, event, sizeof(ui_event_t));
	dll_push_back(ui_state.event_list.first, ui_state.event_list.last, new_event);
	ui_state.event_list.count++;
}

function void 
ui_event_pop(ui_event_t* event) {
	dll_remove(ui_state.event_list.first, ui_state.event_list.last, event);
	ui_state.event_list.count--;
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

	arena_clear(ui_state.scratch_arena);

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
				layout_position += child->fixed_size.x + 1.0f;
				bounds += child->fixed_size.x + 1.0f;
			} else {
				bounds = max(bounds, child->fixed_size.x);
			}
		}

		// determine final rect for child
		child->rect.x0 = root->rect.x0 + child->fixed_position.x - floorf(root->view_offset.x);
		child->rect.x1 = child->rect.x0 + child->fixed_size.x;

	}

	// store view bounds
	root->view_bounds.x = bounds;

	// y axis
	layout_position = 0.0f;
	bounds = 0.0f;

	for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {

		f32 original_pos = min(child->rect.y0, child->rect.y1);

		// calculate fixed position and size
		if (!(child->flags & ui_frame_flag_floating_y)) {
			child->fixed_position.y = layout_position;

			if (root->layout_axis == ui_layout_axis_y) {
				layout_position += child->fixed_size.y + 1.0f;
				bounds += child->fixed_size.y + 1.0f;
			} else {
				bounds = max(bounds, child->fixed_size.y);
			}
		}

		// determine final rect for child
		child->rect.y0 = root->rect.y0 + child->fixed_position.y - floorf(root->view_offset.y);
		child->rect.y1 = child->rect.y0 + child->fixed_size.y;

	}

	// store view bounds
	root->view_bounds.y = bounds;

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
				result |= ui_interaction_left_pressed << event->mouse;
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

			vec2_t scroll = event->scroll;
			
			// swap scrolling on shift
			if (event->modifiers & os_modifier_shift) {
				scroll.x = event->scroll.y;
				scroll.y = event->scroll.x;
			}

			if (!(frame->flags & ui_frame_flag_view_clamp_x)) {
				scroll.x = 0.0f;
			}

			if (!(frame->flags & ui_frame_flag_view_clamp_y)) {
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
	ui_stack_auto_pop_impl(layout_axis);
	ui_stack_auto_pop_impl(rounding_00);
	ui_stack_auto_pop_impl(rounding_01);
	ui_stack_auto_pop_impl(rounding_10);
	ui_stack_auto_pop_impl(rounding_11);
	ui_stack_auto_pop_impl(palette);
	ui_stack_auto_pop_impl(texture);
	ui_stack_auto_pop_impl(font);
	ui_stack_auto_pop_impl(font_size);

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
ui_stack_impl(layout_axis, ui_layout_axis)
ui_stack_impl(rounding_00, f32)
ui_stack_impl(rounding_01, f32)
ui_stack_impl(rounding_10, f32)
ui_stack_impl(rounding_11, f32)
ui_stack_impl(palette, ui_palette_t*)
ui_stack_impl(texture, gfx_texture_t*)
ui_stack_impl(font, gfx_font_t*)
ui_stack_impl(font_size, f32)

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