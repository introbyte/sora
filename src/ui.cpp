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
	ui_state.scratch_arena = arena_create(megabytes(64));

	ui_state.window = nullptr;
	ui_state.renderer = nullptr;

	// default resources
	ui_state.default_palette.background = color(0x282828ff);
	ui_state.default_palette.border = color(0x3d3d3dff);
	ui_state.default_palette.shadow = color(0x15151580);
	ui_state.default_palette.hover = color(0x151515ff);
	ui_state.default_palette.active = color(0x151515ff);
	ui_state.default_palette.text = color(0xe2e2e2ff);

	ui_state.default_texture = gfx_state.default_texture;
	ui_state.default_font = gfx_font_load(str("res/fonts/segoe_ui.ttf"));
	
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
	arena_release(ui_state.scratch_arena);

}

function void
ui_begin_frame(gfx_renderer_t* renderer) {

	// set context
	ui_state.window = renderer->window;
	ui_state.renderer = renderer;

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
	for (ui_frame_t* frame = ui_state.frame_first; frame != 0; frame = frame->hash_next) {
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
	for (ui_frame_t* frame = ui_state.root; frame != nullptr;) {

		// do recursive depth first search
		u32 sibling_member_offset = (u32)(&(((ui_frame_t*)0)->tree_prev));
		u32 child_member_offset = (u32)(&(((ui_frame_t*)0)->tree_child_last));
		ui_frame_rec_t recursion = ui_frame_rec_depth_first(frame, ui_state.root, sibling_member_offset, child_member_offset);

		ui_palette_t* palette = frame->palette;
		
		// background
		if (frame->flags & ui_frame_flag_draw_background) {

			// determine color
			color_t background_color = palette->background;
			color_t border_color = palette->border;

			if (frame->flags & ui_frame_flag_draw_hover_effects) {
				background_color = color_lerp(background_color, color_add(background_color, palette->hover), frame->hover_t);
				border_color = color_lerp(border_color, color_add(border_color, palette->hover), frame->hover_t);
			}
			
			if (frame->flags & ui_frame_flag_draw_active_effects) {
				background_color = color_lerp(background_color, color_add(background_color, palette->active), frame->active_t);
				border_color = color_lerp(border_color, color_add(border_color, palette->active), frame->hover_t);
			}

			// frame shadow
			gfx_quad_params_t shadow_params = gfx_quad_params(palette->shadow, 0.0f, 0.0f, 0.33f);
			shadow_params.radii = frame->rounding;
			gfx_push_quad(renderer, rect_translate(frame->rect, 1.0f), shadow_params);

			// background
			gfx_quad_params_t background_params = gfx_quad_params(background_color);
			background_params.radii = frame->rounding;
			gfx_push_quad(renderer, frame->rect, background_params);

			// border
			gfx_quad_params_t border_params = gfx_quad_params(border_color, 0.0f, 1.0f);
			border_params.radii = frame->rounding;
			gfx_push_quad(renderer, frame->rect, border_params);

		}

		// clip
		if (frame->flags & ui_frame_flag_clip) {
			rect_t top_clip = gfx_top_clip(renderer);
			rect_t new_clip = rect_shrink(frame->rect, 1.0f);
			if (top_clip.x1 != 0.0f || top_clip.y1 != 0.0f) {
				new_clip = rect_intersection(new_clip, top_clip);
			}
			gfx_push_clip(renderer, new_clip);
		}


		// text
		if (frame->flags & ui_frame_flag_draw_text) {

			// calculate text pos
			vec2_t text_pos;

			gfx_font_metrics_t font_metrics = gfx_font_get_metrics(frame->font, frame->font_size);
			text_pos.y = roundf((frame->rect.y0 + frame->rect.y1 + font_metrics.capital_height) * 0.5f - font_metrics.ascent);
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

			// text shadow
			gfx_text_params_t shadow_params = gfx_text_params(palette->shadow, frame->font, frame->font_size);
			gfx_push_text(renderer, frame->string, vec2_add(text_pos, 1.0f), shadow_params);

			// text
			gfx_text_params_t text_params = gfx_text_params(palette->text, frame->font, frame->font_size);
			gfx_push_text(renderer, frame->string, text_pos, text_params);

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
				gfx_pop_clip(renderer);
			}
		}

		frame = recursion.next;
	}



}

// widgets

function ui_interaction 
ui_button(str_t label) {

	ui_frame_flags flags =
		ui_frame_flag_clickable |
		ui_frame_flag_draw |
		ui_frame_flag_clip;

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
		ui_frame_flag_draw_text |
		ui_frame_flag_clip;

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
ui_color_picker(str_t label, color_t* color) {

	ui_frame_flags flags =
		ui_frame_flag_clickable |
		ui_frame_flag_draw;
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);

	return interaction;

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

// alignment functions

function vec2_t
ui_text_align(gfx_font_t* font, f32 size, str_t text, rect_t rect, ui_text_alignment alignment) {

	vec2_t result = { 0 };

	gfx_font_metrics_t font_metrics = gfx_font_get_metrics(font, size);
	result.y = roundf((rect.y0 + rect.y1 + font_metrics.capital_height) * 0.5f - font_metrics.ascent);

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
	ui_event_t* new_event = (ui_event_t*)arena_malloc(ui_state.per_frame_arena, sizeof(ui_event_t));
	memcpy(new_event, event, sizeof(ui_event_t));
	dll_push_back(ui_state.event_list.first, ui_state.event_list.last, new_event);
	ui_state.event_list.count++;
}

function void 
ui_event_pop(ui_event_t* event) {
	dll_remove(ui_state.event_list.first, ui_state.event_list.last, event);
	ui_state.event_list.count--;
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
			f32* child_fixups = (f32*)arena_malloc(ui_state.scratch_arena, sizeof(f32) * root->child_count);
			
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
			f32* child_fixups = (f32*)arena_malloc(ui_state.scratch_arena, sizeof(f32) * root->child_count);

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
	root->view_bounds.y = bounds;

	// recurse through children
	for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
		ui_layout_solve_set_positions(child);
	}
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
	b8 frame_is_new = frame == nullptr;

	// duplicate
	if (!frame_is_new && frame->last_build_index == ui_state.build_index) {
		frame = nullptr;
		key = { 0 };
		frame_is_new = true;
	}

	// box doesn't need persistant data
	b8 frame_is_transient = ui_key_equals(key, { 0 });

	// if it didn't, grab from free list, or create one
	if (frame_is_new) {
		frame = !frame_is_transient ? ui_state.frame_free : 0;

		if (frame != nullptr) {
			stack_pop_n(ui_state.frame_free, hash_next);
		} else {
			frame = (ui_frame_t*)arena_malloc(ui_state.frame_arena, sizeof(ui_frame_t));
		}
		memset(frame, 0, sizeof(ui_frame_t));
		
	}

	if (frame_is_new && !frame_is_transient) {

		// add to list
		dll_push_back_np(ui_state.frame_first, ui_state.frame_last, frame, hash_next, hash_prev);

		// set first build index
		frame->first_build_index = ui_state.build_index;

	}

	// add to tree and set parent
	ui_frame_t* parent = ui_top_parent();
	if (parent != nullptr) {
		parent->child_count++;
		frame->tree_parent = parent;
		
		dll_push_back_np(parent->tree_child_first, parent->tree_child_last, frame, tree_next, tree_prev);
	}

	// fill struct
	frame->key = key;
	frame->flags = flags | ui_top_flags();
	frame->last_build_index = ui_state.build_index;
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

	if (frame->flags & ui_frame_flag_draw_text) {
		frame->string = ui_string_display_format(string);
	}

	return frame;

}

function ui_frame_rec_t 
ui_frame_rec_depth_first(ui_frame_t* frame, ui_frame_t* root, u32 sibling_member_offset, u32 child_member_offset) {
	ui_frame_rec_t result = { 0 };

	result.next = nullptr;
	
	ui_frame_t** child_offset = (ui_frame_t**)((u8*)frame + child_member_offset);
	if (*child_offset != nullptr) {
		result.next = *child_offset;
		result.push_count++;
	} else for (ui_frame_t* f = frame; f != 0 && f != root; f = f->tree_parent) {

		ui_frame_t** sibling_offset = (ui_frame_t**)((u8*)f + sibling_member_offset);
		if (*sibling_offset != nullptr) {
			result.next = *sibling_offset;
			break;
		}

		result.pop_count++;
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
			if (mouse_in_bounds && event->type == ui_event_type_mouse_release) {
				ui_state.active_frame_key[event->mouse] = { 0 };
				result |= ui_interaction_left_released << event->mouse;
				result |= ui_interaction_left_clicked << event->mouse;
				taken = true;
			}

			// we mouse release off the frame
			if (!mouse_in_bounds && event->type == ui_event_type_mouse_release) {
				ui_state.active_frame_key[event->mouse] = {0};
				result |= ui_interaction_left_released << event->mouse;
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
	node = (ui_##name##_node_t*)arena_malloc(ui_state.per_frame_arena, sizeof(ui_##name##_node_t)); \
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
	node = (ui_##name##_node_t*)arena_malloc(ui_state.per_frame_arena, sizeof(ui_##name##_node_t)); \
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