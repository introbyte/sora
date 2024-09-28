// ui.cpp

#ifndef UI_CPP
#define UI_CPP

// defines

#define ui_default_init(name, value) \
ui_state.name##_default_node.v = value; \

#define ui_stack_reset(name) \
ui_state.name##_stack.top = &ui_state.name##_default_node; ui_state.name##_stack.free = 0; ui_state.name##_stack.auto_pop = 0; \

// implementation

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
	ui_state.default_palette.hover = color(0x151515ff);
	ui_state.default_palette.active = color(0x151515ff);
	ui_state.default_palette.text = color(0xe2e2e2ff);

	ui_state.default_texture = gfx_state.default_texture;
	ui_state.default_font = gfx_font_load(str("res/fonts/segoe_ui.ttf"));

	// default stack
	ui_default_init(parent, nullptr);
	ui_default_init(flags, 0);
	ui_default_init(fixed_x, 0);
	ui_default_init(fixed_y, 0);
	ui_default_init(fixed_width, 0);
	ui_default_init(fixed_height, 0);
	ui_default_init(pref_width, ui_size(ui_size_type_null, 0.0f, 0.0f));
	ui_default_init(pref_height, ui_size(ui_size_type_null, 0.0f, 0.0f));
	ui_default_init(text_alignment, ui_text_alignment_left);
	ui_default_init(layout_axis, ui_layout_axis_y);
	ui_default_init(rounding, 0);
	ui_default_init(palette, &ui_state.default_palette);
	ui_default_init(texture, ui_state.default_texture);
	ui_default_init(font, ui_state.default_font);
	ui_default_init(font_size, 8.0f);

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
	ui_stack_reset(text_alignment);
	ui_stack_reset(layout_axis);
	ui_stack_reset(rounding);
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
	for (ui_frame_t* frame = ui_state.frame_first; frame != 0; frame = frame->hash_next) {
		frame->rect = rect(
			frame->fixed_position.x, frame->fixed_position.y, 
			frame->fixed_position.x + frame->fixed_size.x, frame->fixed_position.y + frame->fixed_size.y
		);
	}

	// animate
	f32 fast_rate = 1.0f - powf(2.0f, -144.0f * ui_state.window->delta_time);
	f32 slow_rate = 1.0f - powf(2.0f, -30.0f * ui_state.window->delta_time);
	for (ui_frame_t* frame = ui_state.frame_first; frame != 0; frame = frame->hash_next) {
		b8 is_hovered = ui_key_equals(frame->key, ui_state.hovered_frame_key);
		b8 is_active = ui_key_equals(frame->key, ui_state.active_frame_key[os_mouse_button_left]);

		frame->hover_t += fast_rate * ((f32)is_hovered - frame->hover_t);
		frame->active_t += fast_rate * ((f32)is_active - frame->active_t);
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

			gfx_quad_params_t shadow_params = gfx_quad_params(color(0x15151580), frame->rounding, 0.0f, 0.5f);
			gfx_push_quad(renderer, rect_translate(frame->rect, 1.0f), shadow_params);

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

			gfx_quad_params_t background_params = gfx_quad_params(background_color, frame->rounding);
			gfx_push_quad(renderer, frame->rect, background_params);

			gfx_quad_params_t border_params = gfx_quad_params(border_color, frame->rounding, 1.0f);
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
			vec2_t text_pos = ui_text_align(frame->font, frame->font_size, frame->string, frame->rect, frame->text_alignment);

			// TODO: truncate text if too long.

			gfx_text_params_t shadow_params = gfx_text_params(color(0x15151580), frame->font, frame->font_size);
			gfx_push_text(renderer, frame->string, vec2_add(text_pos, 1.0f), shadow_params);

			gfx_text_params_t text_params = gfx_text_params(palette->text, frame->font, frame->font_size);
			gfx_push_text(renderer, frame->string, text_pos, text_params);

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


// key functions

function ui_key_t
ui_key_from_string(ui_key_t seed, str_t string) {
	ui_key_t key = { 0 };
	if (string.size != 0) {
		memcpy(&key, &seed, sizeof(ui_key_t));
		for (u64 i = 0; i < string.size; i++) {
			key.data[0] = ((key.data[0] << 5) + key.data[0]) + string.data[i];
		}
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

// string functions

function str_t
ui_display_string(str_t string) {
	u32 pos = str_find_substr(string, str("##"));
	if (pos < string.size) {
		string.size = pos;
	}
	return string;
}

function str_t
ui_hash_string(str_t string) {
	u32 pos = str_find_substr(string, str("###"));
	if (pos < string.size) {
		string = str_skip(string, pos);
	}
	return string;
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

	// if it didn't, grab from free list, or create one
	if (frame == nullptr) {
		frame = ui_state.frame_free;

		if (frame != nullptr) {
			stack_pop_n(ui_state.frame_free, hash_next);
		} else {
			frame = (ui_frame_t*)arena_malloc(ui_state.frame_arena, sizeof(ui_frame_t));
		}
		memset(frame, 0, sizeof(ui_frame_t));
		
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
	frame->pref_size[0] = ui_top_pref_width();
	frame->pref_size[1] = ui_top_pref_height();
	frame->text_alignment = ui_top_text_alignment();
	frame->layout_axis = ui_top_layout_axis();
	frame->rounding = ui_top_rounding();
	frame->palette = ui_top_palette();
	frame->texture = ui_top_texture();
	frame->font = ui_top_font();
	frame->font_size = ui_top_font_size();
	
	ui_auto_pop_stacks();

	return frame;
}

function ui_frame_t* 
ui_frame_from_string(str_t string, ui_frame_flags flags) {
	
	str_t hash_string = ui_hash_string(string);
	ui_key_t key = ui_key_from_string({ 0 }, hash_string);

	ui_frame_t* frame = ui_frame_from_key(key, flags);
	frame->string = string;

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

	for (ui_event_t* event = ui_state.event_list.first; event != 0; event = event->next) {
		b8 taken = false;
		vec2_t mouse_pos = event->position;
		b8 mouse_in_bounds = rect_contains(rect, mouse_pos);

		if (frame->flags & ui_frame_flag_interactable) {

			// mouse hovers
			if (mouse_in_bounds) {
				ui_state.hovered_frame_key = frame->key;
			} else {
				ui_state.hovered_frame_key = { 0 };
			}

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

		if (taken) {
			ui_event_pop(event);
		}

	}

	// mouse dragging
	if (frame->flags & ui_frame_flag_interactable) {
		for (i32 mouse_button = 0; mouse_button < os_mouse_button_count; mouse_button++) {
			if (ui_key_equals(ui_state.active_frame_key[mouse_button], frame->key) || (result & ui_interaction_left_pressed << mouse_button)) {
				result |= ui_interaction_left_dragging << mouse_button;
			}
		}
	}

	return result;
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
	ui_stack_auto_pop_impl(layout_axis);
	ui_stack_auto_pop_impl(rounding);
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
ui_stack_impl(layout_axis, ui_layout_axis)
ui_stack_impl(rounding, f32)
ui_stack_impl(palette, ui_palette_t*)
ui_stack_impl(texture, gfx_texture_t*)
ui_stack_impl(font, gfx_font_t*)
ui_stack_impl(font_size, f32)



#endif // UI_CPP