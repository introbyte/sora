// ui.cpp

#ifndef UI_CPP
#define UI_CPP

// defines

#define ui_default_init(name, value) \
ui_state.ui_active->name##_default_node.v = value; \

#define ui_stack_reset(name) \
ui_state.ui_active->name##_stack.top = &ui_state.ui_active->name##_default_node; ui_state.ui_active->name##_stack.free = 0; ui_state.ui_active->name##_stack.auto_pop = 0; \

// implementation

// state 

function void
ui_init() {

	// create arenas
	ui_state.arena = arena_create(gigabytes(1));
	ui_state.event_arena = arena_create(megabytes(2));

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
	ui_state.event_bindings[26] = { os_key_delete,    os_modifier_shift, ui_event_type_edit, ui_event_flag_delete | ui_event_flag_zero_delta, ui_event_delta_unit_char, { +1, 0 } };
	ui_state.event_bindings[27] = { os_key_backspace, os_modifier_shift, ui_event_type_edit, ui_event_flag_delete | ui_event_flag_zero_delta, ui_event_delta_unit_char, { -1, 0 } };

	ui_state.event_bindings[28] = { os_key_delete,    os_modifier_ctrl, ui_event_type_edit, ui_event_flag_delete | ui_event_flag_zero_delta, ui_event_delta_unit_word, { +1, 0 } };
	ui_state.event_bindings[29] = { os_key_backspace, os_modifier_ctrl, ui_event_type_edit, ui_event_flag_delete | ui_event_flag_zero_delta, ui_event_delta_unit_word, { -1, 0 } };

	// last click time
	ui_state.last_click_time[0] = 0;
	ui_state.last_click_time[1] = 0;
	ui_state.last_click_time[2] = 0;

	ui_state.default_theme.borders = true;
	ui_state.default_theme.shadows = true;
	ui_state.default_theme.rounding = 2.0f;
	ui_state.default_theme.padding = 2.0f;

	// default
	ui_color_group_t* default = &ui_state.default_theme.default;
	default->background = color(0x3d3d3dff);
	default->border = color(0x303030ff);
	default->hover = color(0x151515ff);
	default->active = color(0x151515ff);
	default->text = color(0xe2e2e2ff);
	default->shadow = color(0xe00000015);
	default->accent = color(0x5bd9ffff);

	// label
	ui_color_group_t* label = &ui_state.default_theme.label;
	*label = *default;
	
	// button
	ui_color_group_t* button = &ui_state.default_theme.button;
	*button = *default;

	// slider
	ui_color_group_t* slider = &ui_state.default_theme.slider;
	*slider = *default;
	slider->accent.a = 0.3f;

	// checkbox
	ui_color_group_t* checkbox = &ui_state.default_theme.checkbox;
	*checkbox = *default;
	checkbox->accent.a = 0.3f;

	// expander
	ui_color_group_t* expander = &ui_state.default_theme.expander;
	*expander = *default;
	expander->background = color(0x3d3d3dff);

	// number edit
	ui_color_group_t* number_edit = &ui_state.default_theme.number_edit;
	*number_edit = *default;

	// color picker
	ui_color_group_t* color_picker = &ui_state.default_theme.color_picker;
	*color_picker = *default;

	// text edit
	ui_color_group_t* text_edit = &ui_state.default_theme.text_edit;
	*text_edit = *default;

	// combo box
	ui_color_group_t* combo_box = &ui_state.default_theme.combo_box;
	*combo_box = *default;

	// panel
	ui_color_group_t* panel = &ui_state.default_theme.panel;
	*panel = *default;
	panel->background = color(0x303030ff);

	ui_color_group_t* view_tab = &ui_state.default_theme.view_tab;
	*view_tab = *default;
	view_tab->background = color(0x303030ff);

	// tooltip
	ui_color_group_t* tooltip = &ui_state.default_theme.tooltip;
	*tooltip = *default;
	tooltip->background = color(0x1d1d1dff);
	tooltip->border = color(0x242424ff);

	// context
	ui_color_group_t* popup = &ui_state.default_theme.popup;
	*popup = *default;
	popup->background = color(0x1d1d1dff);
	popup->border = color(0x242424ff);

	//// default resources
	//ui_state.default_palette.background = color(0x282828ff);
	//ui_state.default_palette.border = color(0x3d3d3dff);
	//ui_state.default_palette.hover = color(0x151515ff);
	//ui_state.default_palette.active = color(0x151515ff);
	//ui_state.default_palette.text = color(0xe2e2e2ff);
	//ui_state.default_palette.accent = color(0x5bd9ffff); // blue
	//
	//ui_state.default_font = draw_state.font;
	//ui_state.default_icon_font = font_open(str("res/fonts/icons.ttf"));

}

function void
ui_release() {

	// release arenas
	arena_release(ui_state.arena);
	arena_release(ui_state.event_arena);

}

function void
ui_update() {

	// reset event list
	arena_clear(ui_state.event_arena);
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
		ui_event.os_event = os_event;
		ui_event.window = os_event->window;
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

		if (os_event->type != 0) {

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
				case os_event_type_text: {
					ui_event.type = ui_event_type_text;
					if (os_event->character == '\n' || os_event->character == '\t') {
						ui_event.type = ui_event_type_null;
					}
					break; 
				}
				case os_event_type_mouse_scroll: { ui_event.type = ui_event_type_mouse_scroll; break; }
			}
			ui_event_push(&ui_event);
		}
	}

}

// context

function ui_context_t*
ui_context_create(gfx_renderer_t* renderer) {

	ui_context_t* context = ui_state.ui_free;
	if (context != nullptr) {
		stack_pop(ui_state.ui_free);
	} else {
		context = (ui_context_t*)arena_alloc(ui_state.arena, sizeof(ui_context_t));
	}
	memset(context, 0, sizeof(ui_context_t));

	ui_context_t* prev_context = ui_state.ui_active;
	ui_state.ui_active = context;

	// create arenas
	context->arena = arena_create(gigabytes(1));
	context->per_frame_arena = arena_create(gigabytes(1));
	context->drag_state_arena = arena_create(megabytes(8));
	context->scratch_arena = arena_create(megabytes(64));

	// set context to nullptr
	context->window = renderer->window;
	context->renderer = renderer;

	// text point
	context->cursor = { 1, 1 };
	context->mark = { 1, 1 };

	// panel
	context->panel_root = ui_panel_create(context);
	context->panel_root->percent_of_parent = 1.0f;
	context->panel_root->split_axis = ui_axis_x;

	// default font
	context->font = draw_state.font;
	context->icon_font = font_open(str("res/fonts/icons.ttf"));

	// default theme
	context->theme = ui_state.default_theme;

	// default stack
	ui_default_init(parent, nullptr);
	ui_default_init(flags, 0);
	ui_default_init(seed_key, { 0 });
	ui_default_init(fixed_x, 0.0f);
	ui_default_init(fixed_y, 0.0f);
	ui_default_init(fixed_width, 0.0f);
	ui_default_init(fixed_height, 0.0f);
	ui_default_init(pref_width, ui_size(ui_size_type_null, 0.0f, 0.0f));
	ui_default_init(pref_height, ui_size(ui_size_type_null, 0.0f, 0.0f));
	ui_default_init(text_alignment, ui_text_alignment_left);
	ui_default_init(text_padding, 4.0f);
	ui_default_init(hover_cursor, os_cursor_pointer);
	ui_default_init(layout_axis, ui_axis_y);
	ui_default_init(rounding_00, 2.0f);
	ui_default_init(rounding_01, 2.0f);
	ui_default_init(rounding_10, 2.0f);
	ui_default_init(rounding_11, 2.0f);
	ui_default_init(color_group, context->theme.default);
	ui_default_init(font, context->font);
	ui_default_init(font_size, 9.0f);
	ui_default_init(focus_hot, ui_focus_type_null);
	ui_default_init(focus_active, ui_focus_type_null);
	ui_default_init(texture, nullptr);

	// set keys to zero
	context->hovered_frame_key = { 0 };
	context->active_frame_key[0] = { 0 };
	context->active_frame_key[1] = { 0 };
	context->active_frame_key[2] = { 0 };
	context->focused_frame_key = { 0 };

	// build state
	context->build_index = 0;

	// add to list
	dll_push_back(ui_state.ui_first, ui_state.ui_last, context);

	ui_state.ui_active = prev_context;

	return context;

}

function void 
ui_context_release(ui_context_t* context) {
	dll_remove(ui_state.ui_first, ui_state.ui_last, context);
	stack_push(ui_state.ui_free, context);

	// release assets
	font_close(context->icon_font);

	// release arenas
	arena_release(context->arena);
	arena_release(context->per_frame_arena);
	arena_release(context->drag_state_arena);
	arena_release(context->scratch_arena);
}

function void
ui_begin_frame(ui_context_t* context) {

	// set context
	ui_state.ui_active = context;

	// clear arenas
	arena_clear(context->per_frame_arena);
	arena_clear(context->scratch_arena);
	
	// get mouse input
	for (ui_event_t* ui_event = ui_state.event_list.first, *next = 0; ui_event != 0; ui_event = next) {
		next = ui_event->next;
		if (ui_event->window == context->window) {
			if (ui_event->type == ui_event_type_mouse_move) {
				context->mouse_pos = ui_event->position;
			}
		}
	}

	// set defaults
	ui_default_init(rounding_00, context->theme.rounding);
	ui_default_init(rounding_01, context->theme.rounding);
	ui_default_init(rounding_10, context->theme.rounding);
	ui_default_init(rounding_11, context->theme.rounding);

	// reset stacks
	ui_stack_reset(parent);
	ui_stack_reset(flags);
	ui_stack_reset(seed_key);
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
	ui_stack_reset(color_group);
	ui_stack_reset(font);
	ui_stack_reset(font_size);
	ui_stack_reset(focus_hot);
	ui_stack_reset(focus_active);
	ui_stack_reset(texture);
	
	
	// TODO: do navigation

	// set to next nav keys
	for (ui_frame_t* frame = context->frame_first; frame != nullptr; frame = frame->hash_next) {
		frame->nav_focus_hot_key = frame->nav_focus_next_hot_key;
		frame->nav_focus_active_key = frame->nav_focus_next_active_key;
	}

	// add root frame
	str_t root_string = str_format(context->scratch_arena, "###%p_root_frame", context->window);
	ui_set_next_fixed_width((f32)context->window->resolution.x);
	ui_set_next_fixed_height((f32)context->window->resolution.y);
	ui_set_next_layout_axis(ui_axis_y);
	context->root = ui_frame_from_string(root_string, 0);
	ui_push_parent(context->root);

	// add root tooltip frame
	str_t tooltip_root_string = str_format(context->scratch_arena, "###%p_tooltip_frame", context->window);
	ui_set_next_fixed_x(context->mouse_pos.x + 15.0f);
	ui_set_next_fixed_y(context->mouse_pos.y + 15.0f);
	ui_set_next_pref_width(ui_size_by_child(1.0f));
	ui_set_next_pref_height(ui_size_by_child(1.0f));
	context->tooltip_root = ui_frame_from_string(tooltip_root_string, ui_frame_flag_floating);

	// reset active keys if removed
	for (i32 i = 0; i < os_mouse_button_count; i++) {
		ui_frame_t* frame = ui_frame_find(context->active_frame_key[i]);

		if (frame == nullptr) {
			context->active_frame_key[i] = { 0 };
		}

	}

	// reset hovered key
	b8 has_active = false;
	for (i32 i = 0; i < os_mouse_button_count; i++) {
		if (!ui_key_equals(context->active_frame_key[i], { 0 })) {
			has_active = true;
			break;
		}
	}
	if (!has_active) {
		context->hovered_frame_key = { 0 };
	}

	// update build index
	context->build_index++;

	// add panels
	rect_t client_rect = rect(0.0f, 0.0f, (f32)context->window->resolution.x, (f32)context->window->resolution.y);

	// build non leaf panel ui
	for (ui_panel_t* panel = context->panel_root; panel != 0; panel = ui_panel_rec_depth_first(panel).next) {
		
		rect_t panel_rect = ui_rect_from_panel(context->scratch_arena, panel, client_rect);
		ui_axis split_axis = panel->split_axis;

		// build drag boundaries
		for (ui_panel_t* child = panel->child_first; child != nullptr && child->next != nullptr; child = child->next) {	

			ui_panel_t* min_child = child;
			ui_panel_t* max_child = child->next;

			rect_t min_child_rect = ui_rect_from_panel_child(child, panel_rect);
			rect_t max_child_rect = ui_rect_from_panel_child(max_child, panel_rect);

			// calculate boundary rect
			rect_t boundary_rect = { 0 };
			boundary_rect.v0[split_axis]  = min_child_rect.v1[split_axis] - 4.0f;
			boundary_rect.v1[split_axis]  = max_child_rect.v0[split_axis] + 4.0f;
			boundary_rect.v0[!split_axis] = panel_rect.v0[!split_axis];
			boundary_rect.v1[!split_axis] = panel_rect.v1[!split_axis];	

			// build frame
			ui_set_next_rect(boundary_rect);
			ui_set_next_hover_cursor(split_axis == ui_axis_x ? os_cursor_resize_EW : os_cursor_resize_NS);
			ui_frame_flags flags = ui_frame_flag_clickable |ui_frame_flag_floating;
			ui_frame_t* frame = ui_frame_from_string(str_format(context->scratch_arena, "###panel_boundary_%p", panel), flags);
			ui_interaction interaction = ui_frame_interaction(frame);

			if (interaction & ui_interaction_left_dragging) {

				if (interaction & ui_interaction_left_pressed) {
					vec2_t drag_data = vec2(min_child->percent_of_parent, max_child->percent_of_parent);
					ui_store_drag_data(&drag_data, sizeof(vec2_t));
				}

				vec2_t drag_data = *(vec2_t*)ui_get_drag_data();
				vec2_t mouse_delta = ui_get_drag_delta();

				f32 total_size = rect_size(panel_rect)[split_axis];

				// min child
				f32 min_pct_before = drag_data[0];
				f32 min_px_before = min_pct_before * total_size;
				f32 min_px_after = max(min_px_before + mouse_delta[split_axis], 50.0f);
				f32 min_pct_after = min_px_after / total_size;
				f32 pct_delta = min_pct_after - min_pct_before;

				// max child
				f32 max_pct_before = drag_data[1];
				f32 max_pct_after = max_pct_before - pct_delta;
				f32 max_px_after = max_pct_after * total_size;
				if (max_px_after < 50.0f) {
					max_px_after = 50.0f;
					max_pct_after = max_px_after / total_size;
					pct_delta = -(max_pct_after - max_pct_before);
					min_pct_after = min_pct_before + pct_delta;
				}

				min_child->percent_of_parent = min_pct_after;
				max_child->percent_of_parent = max_pct_after;

			}

		}
		
	}

	// build leaf panel ui
	for (ui_panel_t* panel = context->panel_root; panel != 0; panel = ui_panel_rec_depth_first(panel).next) {
		rect_t panel_rect = ui_rect_from_panel(context->scratch_arena, panel, client_rect);
		
		if (panel->child_first == nullptr && panel != context->panel_root) {

			// build panel frame
			ui_frame_flags flags = ui_frame_flag_draw_background | ui_frame_flag_clip;
			
			rect_t adjusted_rect = panel_rect;
			adjusted_rect.y0 += 20.0f;

			ui_set_next_color_group(ui_state.ui_active->theme.panel);
			ui_set_next_rounding_11(0.0f);
			ui_set_next_rect(rect_shrink(adjusted_rect, 2.0f));
			str_t panel_string = str_format(context->scratch_arena, "##panel_frame_%p", panel);
			ui_frame_t* panel_frame = ui_frame_from_string(panel_string, flags);

			panel->frame = panel_frame;


		}
		
	}

}

function void
ui_end_frame(ui_context_t* context) {

	// remove untouched frames
	ui_frame_t* next = nullptr;
	for (ui_frame_t* frame = context->frame_first; frame != 0; frame = next) {
		next = frame->hash_next;
		if (frame->last_build_index != context->build_index || ui_key_equals(frame->key, {0})) {
			dll_remove_np(context->frame_first, context->frame_last, frame, hash_next, hash_prev);
			stack_push_n(context->frame_free, frame, hash_next);
		}
	}

	// remove focus
	for (ui_event_t* event = ui_state.event_list.first; event != nullptr; event = event->next) {
		if (event->type == ui_event_type_mouse_release) {
			context->focused_frame_key = { 0 };
		}
	}

	// layout pass
	for (u32 axis = ui_axis_x; axis < ui_axis_count; axis++) {
		ui_layout_solve_independent(context->root, axis);
		ui_layout_solve_upward_dependent(context->root, axis);
		ui_layout_solve_downward_dependent(context->root, axis);
		ui_layout_solve_violations(context->root, axis);
		ui_layout_solve_set_positions(context->root, axis);
	}

	// TODO: bound tooltip within screen



	// animate
	{
		// animate frames
		f32 fast_rate = 1.0f - powf(2.0f, -50.0f * context->window->delta_time);
		f32 slow_rate = 1.0f - powf(2.0f, -30.0f * context->window->delta_time);
		for (ui_frame_t* frame = context->frame_first; frame != 0; frame = frame->hash_next) {
			b8 is_hovered = ui_key_equals(frame->key, context->hovered_frame_key);
			b8 is_active = ui_key_equals(frame->key, context->active_frame_key[os_mouse_button_left]);

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
		context->cursor_pos.x += (context->cursor_target_pos.x - context->cursor_pos.x) * fast_rate;
		context->mark_pos.x += (context->mark_target_pos.x - context->mark_pos.x) * fast_rate;
	}

	// hover cursor
	{

		ui_frame_t* hovered_frame = ui_frame_find(context->hovered_frame_key);
		ui_frame_t* active_frame = ui_frame_find(context->active_frame_key[os_mouse_button_left]);

		if (hovered_frame != nullptr && !(hovered_frame->flags & ui_frame_flag_custom_hover_cursor)) {
			os_cursor cursor = hovered_frame->hover_cursor;
			os_set_cursor(cursor);
		}

	}
	
	// draw
	gfx_renderer_t* renderer = context->renderer;
	u32 depth = 0;
	for (ui_frame_t* frame = context->root; frame != nullptr;) {

		// do recursive depth first search
		u32 sibling_member_offset = (u32)(&(((ui_frame_t*)0)->tree_prev));
		u32 child_member_offset = (u32)(&(((ui_frame_t*)0)->tree_child_last));
		ui_frame_rec_t recursion = ui_frame_rec_depth_first(frame);

		// grab frame info
		ui_color_group_t* color_group = &frame->color_group;

		// frame shadow
		if (frame->flags & ui_frame_flag_draw_shadow) {
			draw_set_next_color(color_group->shadow);
			draw_set_next_radii(frame->rounding);
			draw_rect(rect_translate(frame->rect, 1.0f));
		}

		// determine color
		color_t border_color = color_group->border;
		color_t background_color = color_group->background;

		// background
		if (frame->flags & ui_frame_flag_draw_background) {

			// hover effects
			if (frame->flags & ui_frame_flag_draw_hover_effects) {
				background_color = color_lerp(background_color, color_add(background_color, color_group->hover), frame->hover_t);
				border_color = color_lerp(border_color, color_add(border_color, color_group->hover), frame->hover_t);
			}
			
			// active effects
			if (frame->flags & (ui_frame_flag_draw_active_effects)) {
				background_color = color_lerp(background_color, color_add(background_color, color_group->active), frame->active_t);
				border_color = color_lerp(border_color, color_add(border_color, color_group->active), frame->hover_t);
			}

			// background
			if (frame->texture != nullptr) {
				draw_set_next_texture(frame->texture);
				draw_image(frame->rect);
			} else {
				draw_set_next_color(background_color);
				draw_set_next_radii(frame->rounding);
				draw_rect(frame->rect);
			}
		}

		// border
		if (frame->flags & ui_frame_flag_draw_border) {
			draw_set_next_color(border_color);
			draw_set_next_radii(frame->rounding);
			draw_set_next_thickness(1.0f);
			draw_rect(frame->rect);
		}
	
		// clip
		if (frame->flags & ui_frame_flag_clip) {
			rect_t top_clip = draw_top_clip_mask();
			rect_t new_clip = rect_shrink(frame->rect, 1.0f);
			if (top_clip.x1 != 0.0f || top_clip.y1 != 0.0f) {
				new_clip = rect_intersection(new_clip, top_clip);
			}
			rect_validate(new_clip);
			draw_push_clip_mask(new_clip);
		}

#if 0 // debug frame position
		draw_set_next_color(color(0xffff00ff));
		draw_set_next_thickness(1.0f);
		draw_rect(frame->rect);
#endif 

		// text
		if (frame->flags & ui_frame_flag_draw_text) {

			// truncate text if needed
			f32 frame_width = rect_width(frame->rect);
			//str_t text = font_text_truncate(context->scratch_arena, frame->font, frame->font_size, frame->string, frame_width, str("..."));

			// calculate text pos
			vec2_t text_pos = ui_text_align(frame->font, frame->font_size, frame->string, frame->rect, frame->text_alignment);
			
#if 0 // debug text position
			draw_set_next_color(color(0xff0000ff));
			draw_set_next_thickness(1.0f);
			f32 font_width = font_text_get_width(frame->font, frame->font_size, frame->string);
			f32 font_height = font_text_get_height(frame->font, frame->font_size, frame->string);
			draw_rect(rect(text_pos.x, text_pos.y, text_pos.x + font_width, text_pos.y + font_height));
#endif

			draw_push_font(frame->font);
			draw_push_font_size(frame->font_size);

			{
				// text shadow
				draw_set_next_color(color_group->shadow);
				draw_text(frame->string, vec2_add(text_pos, 1.0f));

				// text
				draw_set_next_color(color_group->text);
				draw_text(frame->string, text_pos);
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
				draw_pop_clip_mask();
			}
		}

		frame = recursion.next;
	}
	
	ui_state.ui_active = nullptr;
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
	str_t string = str_formatv(ui_state.ui_active->scratch_arena, fmt, args);
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

function ui_size_t
ui_size_text(f32 padding) {
	return { ui_size_type_text, padding, 0.0f };
}


// alignment functions

function vec2_t
ui_text_align(font_t* font, f32 size, str_t text, rect_t rect, ui_text_alignment alignment) {

	vec2_t result = { 0 };

	font_metrics_t font_metrics = font_get_metrics(font, size);
	f32 text_height = font_text_get_height(font, size, text);
	result.y = roundf(rect.y0 + (rect.y1 - rect.y0 - (text_height)) / 2.0f);

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

function vec2_t
ui_text_size(font_t* font, f32 font_size, str_t text) {
	return vec2(font_text_get_width(font, font_size, text), font_text_get_height(font, font_size, text));
}

function f32
ui_text_offset_from_index(font_t* font, f32 font_size, str_t string, u32 index) {
	f32 width = 0.0f;
	for (u32 offset = 0; offset < index; offset++) {
		char c = *(string.data + offset);
		font_glyph_t* glyph = font_get_glyph(font, font_size, (u8)c);
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
		font_glyph_t* glyph = font_get_glyph(font, font_size, (u8)c);
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
	if (event->os_event != nullptr) {
		os_event_pop(event->os_event); // take os event if ui event was taken.
	}
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
	arena_clear(ui_state.ui_active->drag_state_arena);
	ui_state.ui_active->drag_state_data = arena_alloc(ui_state.ui_active->drag_state_arena, size);
	ui_state.ui_active->drag_state_size = size;
	memcpy(ui_state.ui_active->drag_state_data, data, size);
}

function void* 
ui_get_drag_data() {
	return ui_state.ui_active->drag_state_data;
}

function vec2_t 
ui_get_drag_delta() {
	return vec2_sub(ui_state.ui_active->mouse_pos, ui_state.ui_active->drag_start_pos);
}

function void 
ui_clear_drag_data() {
	arena_clear(ui_state.ui_active->drag_state_arena);
	ui_state.ui_active->drag_state_size = 0;
}

//  layout 

function void 
ui_layout_solve_independent(ui_frame_t* root, ui_axis axis) {

	switch (root->pref_size[axis].type) {
		case ui_size_type_pixel: {
			root->fixed_size[axis] = root->pref_size[axis].value;
			break;
		}
		case ui_size_type_text: {
			f32 padding = root->pref_size[axis].value;
			vec2_t text_size = ui_text_size(root->font, root->font_size, root->string);
			root->fixed_size[axis] = padding + text_size[axis] + 8.0f;
			break;
		}
	}

	// recurse through children
	for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
		ui_layout_solve_independent(child, axis);
	}

}

function void 
ui_layout_solve_upward_dependent(ui_frame_t* root, ui_axis axis) {

	// x axis
	switch (root->pref_size[axis].type) {
		case ui_size_type_percent: {
			// find parent that has a fixed size	
			ui_frame_t* fixed_parent = nullptr;
			for (ui_frame_t* p = root->tree_parent; p != 0; p = p->tree_parent) {
				if (p->flags & (ui_frame_flag_fixed_width << axis) ||
					p->pref_size[axis].type == ui_size_type_pixel ||
					p->pref_size[axis].type == ui_size_type_percent) {
					fixed_parent = p;
					break;
				}
			}
			
			// calculate percent size of fixed parent.
			f32 size = fixed_parent->fixed_size[axis] * root->pref_size[axis].value;
			root->fixed_size[axis] = size;

			break;
		}
	}

	// recurse through children
	for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
		ui_layout_solve_upward_dependent(child, axis);
	}

}

function void
ui_layout_solve_downward_dependent(ui_frame_t* root, ui_axis axis) {

	// recurse through children
	for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
		ui_layout_solve_downward_dependent(child, axis);
	}

	// x axis
	switch (root->pref_size[axis].type) {
		case ui_size_type_by_children: {
			
			// find width of children
			f32 sum = 0.0f;
			for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
				if (!(child->flags & (ui_frame_flag_floating_x << axis))) {
					if (root->layout_axis == axis) {
						sum += child->fixed_size[axis];
					} else {
						sum = max(sum, child->fixed_size[axis]);
					}
				}
			}
			root->fixed_size[axis] = sum;
			break;
		}
	}
	
}

function void 
ui_layout_solve_violations(ui_frame_t* root, ui_axis axis) {
	
	// x axis

	// fix children sizes in non layout axis
	if (root->layout_axis != axis && !(root->flags & (ui_frame_flag_overflow_x << axis))) {
		f32 allowed_size = root->fixed_size[axis];
		for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
			if (!(child->flags & (ui_frame_flag_floating_x << axis))) {
				f32 child_size = child->fixed_size[axis];
				f32 violation = child_size - allowed_size;
				f32 max_fixup = child_size;
				f32 fixup = clamp(violation, 0.0f, max_fixup);
				if (fixup > 0.0f) {
					child->fixed_size[axis] -= fixup;
				}
			}
		}
	}

	// fix children sizes in layout axis
	if (root->layout_axis == axis && !(root->flags & ui_frame_flag_overflow_x << axis)) {

		// figure out total size
		f32 total_allowed_size = root->fixed_size[axis];
		f32 total_size = 0.0f;
		f32 total_weighted_size = 0.0f;
		
		for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
			if (!(child->flags & (ui_frame_flag_floating_x << axis))) {
				total_size += child->fixed_size[axis];
				total_weighted_size += child->fixed_size[axis] * (1.0f - child->pref_size[axis].strictness);
			}
		}

		f32 violation = total_size - total_allowed_size;
		if (violation > 0.0f) {
			
			// find child fixup size
			f32 child_fixup_sum = 0.0f;
			f32* child_fixups = (f32*)arena_alloc(ui_state.ui_active->scratch_arena, sizeof(f32) * root->child_count);
			
			u32 child_index = 0;
			for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next, child_index++) {
				if (!(child->flags & (ui_frame_flag_floating_x << axis))) {
					f32 fixup_size_this_child = child->fixed_size[axis] * (1.0f - child->pref_size[axis].strictness);
					fixup_size_this_child = max(0.0f, fixup_size_this_child);
					child_fixups[child_index] = fixup_size_this_child;
					child_fixup_sum += fixup_size_this_child;
				}
			}

			// fixup child size
			child_index = 0;
			for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next, child_index++) {
				if (!(child->flags & (ui_frame_flag_floating_x << axis))) {
					f32 fixup_percent = violation / total_weighted_size;
					fixup_percent = clamp_01(fixup_percent);
					child->fixed_size[axis] -= child_fixups[child_index] * fixup_percent;
				}
			}
		}
	}

	// fix child percent sizes if we allow overflow
	if (root->flags & (ui_frame_flag_overflow_x << axis)) {
		for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
			if (child->pref_size[axis].type == ui_size_type_percent) {
				child->fixed_size[axis] = root->fixed_size[axis] * child->pref_size[axis].value;
			}
		}
	}
	
	// recurse through children
	for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
		ui_layout_solve_violations(child, axis);
	}
}

function void 
ui_layout_solve_set_positions(ui_frame_t* root, ui_axis axis) {

	// x axis

	f32 layout_position = 0.0f;
	f32 bounds = 0.0f;

	for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {

		f32 original_pos = min(child->rect.v0[axis], child->rect.v1[axis]);
		f32 root_pos = root->rect.v0[axis];

		// calculate fixed position and size
		if (!(child->flags & (ui_frame_flag_floating_x << axis))) {
			child->fixed_position[axis] = layout_position;

			if (root->layout_axis == axis) {
				layout_position += child->fixed_size[axis];
				bounds += child->fixed_size[axis];
			} else {
				bounds = max(bounds, child->fixed_size[axis]);
			}
		} else {
			root_pos = 0.0f;
		}

		// determine final rect for child
		child->rect.v0[axis] = root_pos + child->fixed_position[axis] - floorf(root->view_offset[axis]);
		child->rect.v1[axis] = child->rect.v0[axis] + child->fixed_size[axis];

	}

	// store view bounds
	root->view_bounds[axis] = bounds + 1.0f;

	// recurse through children
	for (ui_frame_t* child = root->tree_child_first; child != 0; child = child->tree_next) {
		ui_layout_solve_set_positions(child, axis);
	}
}


function ui_frame_t*
ui_row_begin() {
	ui_set_next_layout_axis(ui_axis_x);
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
	ui_set_next_layout_axis(ui_axis_y);
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
		for (ui_frame_t* frame = ui_state.ui_active->frame_first; frame != 0; frame = frame->hash_next) {
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
	ui_context_t* context = ui_state.ui_active;

	// duplicate
	if (!frame_is_new && (frame->last_build_index == context->build_index)) {
		frame = nullptr;
		key = { 0 };
		frame_is_new = true;
	}

	// box doesn't need persistant data
	b8 frame_is_transient = ui_key_equals(key, { 0 });

	// if it didn't, grab from free list, or create one
	if (frame_is_new) {
		frame = !frame_is_transient ? context->frame_free : nullptr;

		if (frame != nullptr) {
			stack_pop_n(context->frame_free, hash_next);
		} else {
			frame = (ui_frame_t*)arena_alloc(!frame_is_transient ? context->arena : context->per_frame_arena , sizeof(ui_frame_t));
		}
		memset(frame, 0, sizeof(ui_frame_t));
		
	}

	frame->tree_child_first = frame->tree_child_last = frame->tree_next = frame->tree_prev = frame->tree_parent = nullptr;
	frame->child_count = 0;
	frame->flags = 0;

	// add to ui_state list
	if (frame_is_new && !frame_is_transient) {
		dll_push_back_np(context->frame_first, context->frame_last, frame, hash_next, hash_prev);
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
		frame->first_build_index = context->build_index;
	}
	frame->last_build_index = context->build_index;

	// fill struct
	frame->key = key;
	frame->flags = flags | ui_top_flags();
	frame->is_transient = frame_is_transient;
	frame->fixed_position.x = ui_top_fixed_x();
	frame->fixed_position.y = ui_top_fixed_y();
	frame->fixed_size.x = ui_top_fixed_width();
	frame->fixed_size.y = ui_top_fixed_height();
	frame->pref_size[ui_axis_x] = ui_top_pref_width();
	frame->pref_size[ui_axis_y] = ui_top_pref_height();
	frame->text_alignment = ui_top_text_alignment();
	frame->text_padding = ui_top_text_padding();
	frame->hover_cursor = ui_top_hover_cursor();
	frame->layout_axis = ui_top_layout_axis();
	frame->rounding.x = ui_top_rounding_00();
	frame->rounding.y = ui_top_rounding_01();
	frame->rounding.z = ui_top_rounding_10();
	frame->rounding.w = ui_top_rounding_11();
	frame->color_group = ui_top_color_group();
	frame->texture = ui_top_texture();
	frame->font = ui_top_font();
	frame->font_size = ui_top_font_size();
	
	// set fixed flags
	if (frame->fixed_size.x != 0.0f) {
		frame->flags |= ui_frame_flag_floating_x;
	}
	if (frame->fixed_size.y != 0.0f) {
		frame->flags |= ui_frame_flag_floating_y;
	}
	if (frame->fixed_size.x != 0.0f) {
		frame->flags |= ui_frame_flag_fixed_width;
	}
	if (frame->fixed_size.y != 0.0f) {
		frame->flags |= ui_frame_flag_fixed_height;
	}

	ui_auto_pop_stacks();

	return frame;
}

function ui_frame_t* 
ui_frame_from_string(str_t string, ui_frame_flags flags) {
	
	ui_key_t seed_key = ui_top_seed_key();
	ui_key_t key = ui_key_from_string(seed_key, string);
	ui_frame_t* frame = ui_frame_from_key(key, flags);
	frame->string = ui_string_display_format(string);

	return frame;

}

function ui_frame_rec_t 
ui_frame_rec_depth_first(ui_frame_t* frame) {

	ui_frame_rec_t rec = { 0 };
	
	if (frame->tree_child_last != nullptr) {
		rec.next = frame->tree_child_last;
		rec.push_count = 1;
	} else for (ui_frame_t* f = frame; f != nullptr; f = f->tree_parent) {
		if (f->tree_prev != nullptr) {
			rec.next = f->tree_prev;
			break;
		}
		rec.pop_count++;
	}

	return rec;
}

function ui_interaction 
ui_frame_interaction(ui_frame_t* frame) {

	ui_interaction result = ui_interaction_none;

	ui_context_t* context = ui_state.ui_active;

	// calculate interaction rect
	rect_t rect = frame->rect;
	for (ui_frame_t* f = frame->tree_parent; f != 0; f = f->tree_parent) {
		if (f->flags & ui_frame_flag_clip) {
			rect = rect_intersection(rect, f->rect);
		}
	}

	b8 view_scrolled = false;

	for (ui_event_t* event = ui_state.event_list.first; event != 0; event = event->next) {
		if (event->window == context->window) {
			b8 taken = false;
			vec2_t mouse_pos = event->position;
			b8 mouse_in_bounds = rect_contains(rect, mouse_pos);

			// clicking
			if (frame->flags & ui_frame_flag_clickable) {

				// we mouse press on the frame
				if (mouse_in_bounds && event->type == ui_event_type_mouse_press) {
					context->active_frame_key[event->mouse] = frame->key;
					context->focused_frame_key = frame->key;
					result |= ui_interaction_left_pressed << event->mouse;

					context->drag_start_pos = event->position;

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
				if (event->type == ui_event_type_mouse_release && ui_key_equals(context->active_frame_key[event->mouse], frame->key)) {

					if (mouse_in_bounds) {
						result |= ui_interaction_left_clicked << event->mouse;
					}
					context->active_frame_key[event->mouse] = { 0 };
					result |= ui_interaction_left_released << event->mouse;

					context->active_frame_key[event->mouse] = { 0 };
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
			if (ui_key_equals(context->active_frame_key[mouse_button], frame->key) || (result & ui_interaction_left_pressed << mouse_button)) {
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
	vec2_t mouse_pos = context->mouse_pos;
	if ((frame->flags & ui_frame_flag_clickable) && 
		(rect_contains(rect, mouse_pos)) &&
		(ui_key_equals(context->hovered_frame_key, {0})|| ui_key_equals(context->hovered_frame_key, frame->key)) &&
		(ui_key_equals(context->active_frame_key[os_mouse_button_left], {0}) || ui_key_equals(context->active_frame_key[os_mouse_button_left], frame->key)) &&
		(ui_key_equals(context->active_frame_key[os_mouse_button_middle], {0}) || ui_key_equals(context->active_frame_key[os_mouse_button_middle], frame->key)) &&
		(ui_key_equals(context->active_frame_key[os_mouse_button_right], {0}) || ui_key_equals(context->active_frame_key[os_mouse_button_right], frame->key))) {
		context->hovered_frame_key = frame->key;
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


// panels

function ui_panel_t* 
ui_panel_create(ui_context_t* context, f32 percent, ui_axis split_axis) {

	ui_panel_t* panel = context->panel_free;
	if (panel != nullptr) {
		stack_pop(context->panel_free);
	} else {
		panel = (ui_panel_t*)arena_alloc(context->arena, sizeof(ui_panel_t));
	}
	memset(panel, 0, sizeof(ui_panel_t));

	panel->percent_of_parent = percent;
	panel->split_axis = split_axis;

	return panel;
}

function void 
ui_panel_release(ui_context_t* context, ui_panel_t* panel) {
	stack_push(context->panel_free, panel);
}

function ui_frame_t* 
ui_panel_begin(ui_panel_t* panel) {

	ui_set_next_parent(panel->frame);
	ui_set_next_rect(rect_shrink(panel->frame->rect, 8.0f));
	ui_frame_t* inner_frame = ui_frame_from_string(str(""), ui_frame_flag_floating);

	ui_push_parent(inner_frame);

	return panel->frame;
}

function void 
ui_panel_end() {
	ui_pop_parent();
}

function void 
ui_panel_insert(ui_panel_t * parent, ui_panel_t * panel, ui_panel_t * prev = nullptr) {
	dll_insert(parent->child_first, parent->child_last, prev, panel);
	parent->child_count++;
	panel->parent = parent;
}

function void 
ui_panel_remove(ui_panel_t* parent, ui_panel_t* panel) {
	dll_remove(parent->child_first, parent->child_last, panel);
	panel->next = nullptr;
	panel->prev = nullptr;
	panel->parent = nullptr;
	parent->child_count--;
}

function ui_panel_rec_t
ui_panel_rec_depth_first(ui_panel_t* panel) {

	ui_panel_rec_t rec = { 0 };

	if (panel->child_first != nullptr) {
		rec.next = panel->child_first;
		rec.push_count = 1;
	} else for (ui_panel_t* p = panel; p != 0; p = p->parent) {
		if (p->next != nullptr) {
			rec.next = p->next;
			break;
		}
		rec.pop_count++;
	}

	return rec;
}

function rect_t
ui_rect_from_panel_child(ui_panel_t* panel, rect_t parent_rect) {
	
	rect_t result = parent_rect;
	ui_panel_t* parent = panel->parent;

	if (parent != nullptr) {
		
		vec2_t parent_rect_size = rect_size(parent_rect);
		ui_axis axis = parent->split_axis;

		result.v1[axis] = result.v0[axis];

		for (ui_panel_t* p = parent->child_first; p != 0; p = p->next) {
			result.v1[axis] += parent_rect_size[axis] * p->percent_of_parent;
			if (p == panel) {
				break;
			}
			result.v0[axis] = result.v1[axis];
		}

		result = rect_round(result);
	}

	return result;
}

function rect_t
ui_rect_from_panel(arena_t* scratch, ui_panel_t* panel, rect_t root_rect) {

	ui_panel_t* root = panel;

	// count ancestors
	i32 ancestor_count = 0;
	for (ui_panel_t* p = panel->parent; p != nullptr; p = p->parent) {
		ancestor_count++;
	}

	// gather ancestors
	ui_panel_t** ancestors = (ui_panel_t**)arena_alloc(scratch, sizeof(ui_panel_t*) * ancestor_count);
	i32 ancestor_index = 0;
	for (ui_panel_t* p = panel->parent; p != nullptr; p = p->parent) {
		ancestors[ancestor_index] = p;
		ancestor_index++;
	}

	// go from highest ancestor and calculate rect
	rect_t parent_rect = root_rect;
	for (i32 ancestor_index = ancestor_count - 1; ancestor_index >= 0 && ancestor_index < ancestor_count; ancestor_index--) {

		ui_panel_t* ancestor = ancestors[ancestor_index];
		
		if (ancestor->parent != nullptr) {
			parent_rect = ui_rect_from_panel_child(ancestor, parent_rect);
		}

	}

	// calculate final rect
	rect_t result = ui_rect_from_panel_child(panel, parent_rect);
	
	return result;

}


// view

function ui_view_t*
ui_view_create(str_t label, view_ui_function* ui_function) {

}

function void
ui_view_release(ui_view_t* view) {

}



// tooltip

function void
ui_tooltip_begin() {

	ui_state.ui_active->tooltip_open = 1;


	ui_set_next_parent(ui_state.ui_active->tooltip_root);
	ui_set_next_pref_width(ui_size_by_child(1.0f));
	ui_set_next_pref_height(ui_size_by_child(1.0f));
	ui_set_next_color_group(ui_state.ui_active->theme.tooltip);
	ui_frame_flags flags =
		ui_frame_flag_draw_background |
		ui_frame_flag_draw_border |
		ui_frame_flag_draw_shadow;
	ui_frame_t* frame = ui_frame_from_string(str(""), flags);

	ui_push_parent(frame);

}

function void 
ui_tooltip_end() {

	ui_pop_parent();

}


// widgets

function void
ui_spacer(ui_size_t size = ui_size_pixel(2.0f, 1.0f)) {
	ui_frame_t* parent = ui_top_parent();
	if (parent->layout_axis == ui_axis_x) {
		ui_set_next_pref_width(size);
	}
	else {
		ui_set_next_pref_height(size);
	}
	ui_set_next_layout_axis(parent->layout_axis);
	ui_frame_t* frame = ui_frame_from_key({ 0 }, 0);
}

function ui_interaction
ui_label(str_t label) {

	ui_frame_flags flags =
		ui_frame_flag_draw_text;

	ui_set_next_color_group(ui_state.ui_active->theme.label);
	ui_frame_t* frame = ui_frame_from_string(str(""), flags);
	frame->string = ui_string_display_format(label);
	ui_interaction interaction = ui_frame_interaction(frame);

	return interaction;
}

function ui_interaction
ui_labelf(char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	str_t display_string = str_formatv(ui_state.ui_active->scratch_arena, fmt, args);
	va_end(args);

	ui_interaction interaction = ui_label(display_string);

	return interaction;
}

function ui_interaction
ui_button(str_t label) {

	ui_frame_flags flags =
		ui_frame_flag_clickable |
		ui_frame_flag_draw;

	ui_set_next_hover_cursor(os_cursor_hand_point);
	ui_set_next_color_group(ui_state.ui_active->theme.button);
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);

	return interaction;
}

function ui_interaction
ui_buttonf(char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	str_t display_string = str_formatv(ui_state.ui_active->scratch_arena, fmt, args);
	va_end(args);

	ui_interaction interaction = ui_button(display_string);

	return interaction;
}

function ui_interaction 
ui_image(str_t label, gfx_texture_t* texture) {

	ui_frame_flags flags = ui_frame_flag_draw_background;
	ui_set_next_texture(texture);
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);

	return interaction;
}

function ui_interaction
ui_slider(str_t label, i32* value, i32 min, i32 max) {
	ui_frame_flags flags =
		ui_frame_flag_clickable |
		ui_frame_flag_draw;

	ui_set_next_hover_cursor(os_cursor_resize_EW);
	ui_set_next_text_alignment(ui_text_alignment_center);
	ui_set_next_color_group(ui_state.ui_active->theme.slider);
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	str_t text = str_format(ui_state.ui_active->per_frame_arena, "%i", *value);
	ui_frame_set_display_text(frame, text);
	ui_interaction interaction = ui_frame_interaction(frame);
	ui_slider_data_t* data = (ui_slider_data_t*)arena_alloc(ui_state.ui_active->per_frame_arena, sizeof(ui_slider_data_t));
	ui_frame_set_custom_draw(frame, ui_slider_draw_function, data);

	// interaction
	if (interaction & ui_interaction_left_dragging) {
		vec2_t mouse_pos = ui_state.ui_active->mouse_pos;
		*value = remap(mouse_pos.x, frame->rect.x0, frame->rect.x1, min, max);
		*value = clamp(*value, min, max);
	}

	f32 percent = remap(*value, min, max, 0.0f, 1.0f);
	data->value = percent;

	return interaction;
}

function ui_interaction
ui_slider(str_t label, f32* value, f32 min, f32 max) {

	ui_frame_flags flags =
		ui_frame_flag_clickable |
		ui_frame_flag_draw;

	ui_set_next_hover_cursor(os_cursor_resize_EW);
	ui_set_next_text_alignment(ui_text_alignment_center);
	ui_set_next_color_group(ui_state.ui_active->theme.slider);
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	str_t text = str_format(ui_state.ui_active->per_frame_arena, "%.2f", *value);
	ui_frame_set_display_text(frame, text);
	ui_interaction interaction = ui_frame_interaction(frame);
	ui_slider_data_t* data = (ui_slider_data_t*)arena_alloc(ui_state.ui_active->per_frame_arena, sizeof(ui_slider_data_t));
	ui_frame_set_custom_draw(frame, ui_slider_draw_function, data);

	// interaction
	if (interaction & ui_interaction_left_dragging) {
		vec2_t mouse_pos = ui_state.ui_active->mouse_pos;
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
	ui_set_next_layout_axis(ui_axis_x);
	ui_push_color_group(ui_state.ui_active->theme.checkbox);

	u32 flags = ui_frame_flag_clickable;
	ui_frame_t* parent_frame = ui_frame_from_string(label, flags);

	ui_push_parent(parent_frame);
	{
		ui_set_next_pref_width(ui_size_pixel(rect_height(parent_frame->rect), 1.0f));
		ui_set_next_font(ui_state.ui_active->icon_font);

		u32 flag_frame_flags =
			ui_frame_flag_draw_text | ui_frame_flag_draw_background |
			ui_frame_flag_draw_hover_effects | ui_frame_flag_draw_active_effects |
			ui_frame_flag_draw_shadow | ui_frame_flag_draw_border;
		
		if (*value) {
			
		}

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
	ui_pop_color_group();

	ui_interaction interaction = ui_frame_interaction(parent_frame);

	if (interaction & ui_interaction_left_clicked) {
		*value = !*value;
	}

	return interaction;


}

function ui_interaction
ui_expander(str_t label, b8* is_expanded) {

	// build parent frame
	ui_set_next_hover_cursor(os_cursor_hand_point);
	ui_set_next_layout_axis(ui_axis_x);
	ui_push_color_group(ui_state.ui_active->theme.expander);

	u32 flags =
		ui_frame_flag_clickable | ui_frame_flag_draw_background |
		ui_frame_flag_draw_hover_effects | ui_frame_flag_draw_active_effects |
		ui_frame_flag_draw_shadow | ui_frame_flag_draw_border;
	ui_frame_t* parent_frame = ui_frame_from_string(label, flags);

	ui_push_parent(parent_frame);
	{
		ui_set_next_pref_width(ui_size_pixel(rect_height(parent_frame->rect), 1.0f));
		ui_set_next_font(ui_state.ui_active->icon_font);
		ui_frame_t* icon_frame = ui_frame_from_string(str(""), ui_frame_flag_draw_text);
		if (*is_expanded) {
			ui_frame_set_display_text(icon_frame, str("v"));
		}
		else {
			ui_frame_set_display_text(icon_frame, str(">"));
		}

		ui_set_next_pref_width(ui_size_percent(1.0f));
		ui_set_next_pref_height(ui_size_percent(1.0f));
		ui_label(label);
	}
	ui_pop_parent();
	ui_pop_color_group();

	ui_interaction interaction = ui_frame_interaction(parent_frame);

	if (interaction & ui_interaction_left_clicked) {
		*is_expanded = !*is_expanded;
	}

	return interaction;
}

function ui_interaction 
ui_float_edit(str_t label, f32* value) {

	// build parent frame
	ui_set_next_layout_axis(ui_axis_x);
	ui_push_color_group(ui_state.ui_active->theme.number_edit);

	ui_frame_t* parent_frame = ui_frame_from_string(label, 0);
	ui_interaction interaction = ui_frame_interaction(parent_frame);

	ui_push_parent(parent_frame);
	{

		u32 flag_frame_flags = ui_frame_flag_draw | ui_frame_flag_clickable;
		ui_set_next_text_alignment(ui_text_alignment_center);
		ui_set_next_font(ui_state.ui_active->icon_font);
		ui_set_next_pref_width(ui_size_pixel(rect_height(parent_frame->rect), 1.0f));
		ui_frame_t* decrement_frame = ui_frame_from_string(str("<"), flag_frame_flags);
		ui_interaction decrement_interaction = ui_frame_interaction(decrement_frame);

		ui_set_next_pref_width(ui_size_percent(1.0f));
		ui_set_next_pref_height(ui_size_percent(1.0f));
		ui_label(label);


		ui_set_next_text_alignment(ui_text_alignment_center);
		ui_set_next_font(ui_state.ui_active->icon_font);
		ui_set_next_pref_width(ui_size_pixel(rect_height(parent_frame->rect), 1.0f));
		ui_frame_t* increment_frame = ui_frame_from_string(str(">"), flag_frame_flags);
		ui_interaction increment_interaction = ui_frame_interaction(increment_frame);

	}
	ui_pop_parent();
	ui_pop_color_group();

	//if (interaction & ui_interaction_left_released) {
	//	*value = !*value;
	//}

	return interaction;

}

function ui_interaction
ui_color_quad(str_t label, f32 hue, f32* sat, f32* val) {

	// build frame and set draw data
	ui_set_next_hover_cursor(os_cursor_hand_point);
	ui_frame_flags flags = ui_frame_flag_clickable | ui_frame_flag_draw_shadow | ui_frame_flag_draw_border;
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);
	ui_color_data_t* data = (ui_color_data_t*)arena_alloc(ui_state.ui_active->per_frame_arena, sizeof(ui_color_data_t));
	ui_frame_set_custom_draw(frame, ui_color_quad_draw_function, data);

	// interaction
	if (interaction & ui_interaction_left_dragging) {
		vec2_t mouse_pos = ui_state.ui_active->mouse_pos;
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
	ui_frame_flags flags = ui_frame_flag_clickable | ui_frame_flag_draw_shadow | ui_frame_flag_draw_border;
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);
	ui_color_data_t* data = (ui_color_data_t*)arena_alloc(ui_state.ui_active->per_frame_arena, sizeof(ui_color_data_t));
	ui_frame_set_custom_draw(frame, ui_color_hue_bar_draw_function, data);

	// interaction
	if (interaction & ui_interaction_left_dragging) {
		vec2_t mouse_pos = ui_state.ui_active->mouse_pos;
		*hue = remap(mouse_pos.x, frame->rect.x0, frame->rect.x1, 0.0f, 1.0f);
		*hue = clamp_01(*hue);
	}

	data->hue = *hue;
	data->sat = sat;
	data->val = val;

	return interaction;
}

function ui_interaction
ui_color_sat_bar(str_t label, f32 hue, f32* sat, f32 val) {

	// build frame and set draw data
	ui_set_next_hover_cursor(os_cursor_hand_point);
	ui_frame_flags flags = ui_frame_flag_clickable | ui_frame_flag_draw_shadow | ui_frame_flag_draw_border;
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);
	ui_color_data_t* data = (ui_color_data_t*)arena_alloc(ui_state.ui_active->per_frame_arena, sizeof(ui_color_data_t));
	ui_frame_set_custom_draw(frame, ui_color_sat_bar_draw_function, data);

	// interaction
	if (interaction & ui_interaction_left_dragging) {
		vec2_t mouse_pos = ui_state.ui_active->mouse_pos;
		*sat = remap(mouse_pos.x, frame->rect.x0, frame->rect.x1, 0.0f, 1.0f);
		*sat = clamp_01(*sat);
	}

	data->hue = hue;
	data->sat = *sat;
	data->val = val;

	return interaction;
}

function ui_interaction
ui_color_val_bar(str_t label, f32 hue, f32 sat, f32* val) {

	// build frame and set draw data
	ui_set_next_hover_cursor(os_cursor_hand_point);
	ui_frame_flags flags = ui_frame_flag_clickable | ui_frame_flag_draw_shadow | ui_frame_flag_draw_border;
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);
	ui_color_data_t* data = (ui_color_data_t*)arena_alloc(ui_state.ui_active->per_frame_arena, sizeof(ui_color_data_t));
	ui_frame_set_custom_draw(frame, ui_color_val_bar_draw_function, data);

	// interaction
	if (interaction & ui_interaction_left_dragging) {
		vec2_t mouse_pos = ui_state.ui_active->mouse_pos;
		*val = remap(mouse_pos.x, frame->rect.x0, frame->rect.x1, 0.0f, 1.0f);
		*val = clamp_01(*val);
	}

	data->hue = hue;
	data->sat = sat;
	data->val = *val;

	return interaction;
}

function ui_interaction
ui_color_ring(str_t label, f32* hue, f32* sat, f32* val) {

	enum {
		area_clicked_none,
		area_clicked_hue_wheel,
		area_clicked_sat_val_tri,
	};

	// build frame and set custom draw data
	ui_frame_flags flags = ui_frame_flag_clickable | ui_frame_flag_custom_hover_cursor;
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);
	ui_color_data_t* data = (ui_color_data_t*)arena_alloc(ui_state.ui_active->per_frame_arena, sizeof(ui_color_data_t));
	ui_frame_set_custom_draw(frame, ui_color_ring_draw_function, data);

	// calculate frame and mouse info
	vec2_t frame_center = rect_center(frame->rect);
	f32 frame_width = rect_width(frame->rect);
	f32 frame_height = rect_height(frame->rect);
	f32 outer_wheel_radius = min(frame_width, frame_height) * 0.5f;
	f32 inner_wheel_radius = outer_wheel_radius - (20.0f);
	vec2_t mouse_pos = ui_state.ui_active->mouse_pos;
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
	if (interaction & ui_interaction_left_pressed) {
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
ui_color_wheel(str_t label, f32* hue, f32* sat, f32 val) {

	enum {
		area_clicked_none,
		area_clicked_hue_sat_circle,
	};

	// build frame and set custom draw data
	ui_frame_flags flags = ui_frame_flag_clickable | ui_frame_flag_custom_hover_cursor;
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);
	ui_color_data_t* data = (ui_color_data_t*)arena_alloc(ui_state.ui_active->per_frame_arena, sizeof(ui_color_data_t));
	ui_frame_set_custom_draw(frame, ui_color_wheel_draw_function, data);

	// calculate frame and mouse info
	vec2_t frame_center = rect_center(frame->rect);
	f32 frame_width = rect_width(frame->rect);
	f32 frame_height = rect_height(frame->rect);
	f32 outer_wheel_radius = min(frame_width, frame_height) * 0.5f;
	vec2_t mouse_pos = ui_state.ui_active->mouse_pos;
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
ui_color_hex(str_t label, f32* hue, f32* sat, f32 val) {

	enum {
		area_clicked_none,
		area_clicked_hue_sat_hex,
	};

	// build frame and set custom draw data
	ui_frame_flags flags = ui_frame_flag_clickable | ui_frame_flag_custom_hover_cursor;
	ui_frame_t* frame = ui_frame_from_string(label, flags);
	ui_interaction interaction = ui_frame_interaction(frame);
	ui_color_data_t* data = (ui_color_data_t*)arena_alloc(ui_state.ui_active->per_frame_arena, sizeof(ui_color_data_t));
	ui_frame_set_custom_draw(frame, ui_color_hex_draw_function, data);

	// calculate frame and mouse info
	vec2_t frame_center = rect_center(frame->rect);
	f32 frame_width = rect_width(frame->rect);
	f32 frame_height = rect_height(frame->rect);
	f32 outer_wheel_radius = min(frame_width, frame_height) * 0.5f;
	vec2_t mouse_pos = ui_state.ui_active->mouse_pos;
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
			area_clicked = area_clicked_hue_sat_hex;
		}

		// store data
		ui_store_drag_data(&area_clicked, sizeof(area_clicked));
	}

	if (interaction & ui_interaction_left_dragging) {

		// get drag data
		u32* area_clicked = (u32*)ui_get_drag_data();

		// edit via hue wheel
		if (*area_clicked == area_clicked_hue_sat_hex) {

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
ui_text_edit(str_t label, char* buffer, u32 buffer_size, u32* out_size) {

	ui_key_t key = ui_key_from_string({ 0 }, label);
	ui_frame_flags flags =
		ui_frame_flag_draw_text |
		ui_frame_flag_draw_shadow |
		ui_frame_flag_draw_background |
		ui_frame_flag_draw_border |
		ui_frame_flag_clickable;

	// if focused already, don't do hover effects
	b8 frame_focused = ui_key_equals(ui_state.ui_active->focused_frame_key, key);
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
			ui_text_op_t text_op = ui_event_to_text_op(ui_state.ui_active->scratch_arena, event, edit_string, ui_state.ui_active->cursor, ui_state.ui_active->mark);

			// skip if invalid
			if (text_op.flags & ui_text_op_flag_invalid) {
				continue;
			}

			// replace range
			if (!ui_text_point_equals(text_op.range.min, text_op.range.max) || text_op.replace.size != 0) {
				str_t new_string = ui_string_replace_range(ui_state.ui_active->scratch_arena, edit_string, text_op.range, text_op.replace);
				new_string.size = min(buffer_size, new_string.size);
				memcpy(buffer, new_string.data, new_string.size);
				*out_size = new_string.size;
			}

			// update cursor
			ui_state.ui_active->cursor = text_op.cursor;
			ui_state.ui_active->mark = text_op.mark;

			// pop event
			ui_event_pop(event);
		}
	}

	frame->string = str(buffer, *out_size);

	// mouse interaction
	ui_interaction interaction = ui_frame_interaction(frame);
	u32 delta_unit = 0;
	vec2_t text_align_pos = ui_text_align(frame->font, frame->font_size, frame->string, frame->rect, frame->text_alignment);
	vec2_t mouse_pos = ui_state.ui_active->mouse_pos;
	vec2_t rel_pos = vec2_sub(mouse_pos, text_align_pos);
	u32 index = ui_text_index_from_offset(frame->font, frame->font_size, frame->string, rel_pos.x);

	if (interaction & ui_interaction_left_pressed) {
		ui_state.ui_active->mark.column = index;
		ui_state.last_click_index[0] = index;
	}

	if (interaction & ui_interaction_left_dragging) {
		ui_state.ui_active->cursor.column = index;
	}

	return interaction;
}

function ui_interaction
ui_combo(str_t label, i32* current, char** items, u32 item_count) {

	// build parent frame
	ui_set_next_hover_cursor(os_cursor_hand_point);
	ui_set_next_layout_axis(ui_axis_x);

	u32 flags =
		ui_frame_flag_clickable | ui_frame_flag_draw_background |
		ui_frame_flag_draw_hover_effects | ui_frame_flag_draw_active_effects |
		ui_frame_flag_draw_shadow | ui_frame_flag_draw_border;
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
		ui_set_next_font(ui_state.ui_active->icon_font);
		ui_frame_t* icon_frame = ui_frame_from_string(str(""), ui_frame_flag_draw_text);
		ui_frame_set_display_text(icon_frame, str("v"));

	}
	ui_pop_parent();

	b8 frame_focused = ui_key_equals(ui_state.ui_active->focused_frame_key, key);

	// popup
	if (frame_focused) {
		ui_set_next_fixed_x(parent_frame->rect.x0);
		ui_set_next_fixed_y(parent_frame->rect.y1);
		ui_set_next_pref_width(ui_top_pref_width());
		ui_set_next_pref_width(ui_size_by_child(1.0f));
		ui_frame_t* popup_frame = ui_frame_from_string(str(""), ui_frame_flag_draw_background | ui_frame_flag_floating | ui_frame_flag_fixed_size);

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

	color_t color = frame->color_group.accent;
	draw_set_next_color(color);
	draw_set_next_radii(frame->rounding);
	draw_rect(bar_rect);
}

function void
ui_color_hue_bar_draw_function(ui_frame_t* frame) {

	// get data
	ui_color_data_t* data = (ui_color_data_t*)frame->custom_draw_data;

	// frame info
	f32 frame_width = rect_width(frame->rect);
	f32 frame_height = rect_height(frame->rect);

	// unpack color
	color_t hue_col = color_hsv_to_rgb(color(data->hue, 1.0f, 1.0f, 1.0f));

	// draw hue bars
	{
		f32 step = 1.0f / 6.0f;
		color_t segments[] = {
			color_hsv_to_rgb({0 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({1 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({2 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({3 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({4 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({5 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({6 * step, 1.0f, 1.0f, 1.0f}),
		};

		for (i32 i = 0; i < 6; i++) {
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
			}
			else if (i == 5) {
				draw_set_next_radius0(frame->rounding.x);
				draw_set_next_radius1(frame->rounding.y);
			}
			else {
				fix = 1.0f;
			}

			f32 x0 = roundf(frame->rect.x0 + (step * (i + 0)) * frame_width) - fix;
			f32 x1 = roundf(frame->rect.x0 + (step * (i + 1)) * frame_width) + fix;
			rect_t segment = rect(x0, frame->rect.y0, x1, frame->rect.y1);
			draw_rect(segment);
		}

	}

	// draw hue indicator
	{
		vec2_t indicator_pos = vec2(frame->rect.x0 + (data->hue * frame_width), (frame->rect.y0 + frame->rect.y1) * 0.5f);

		f32 indicator_size = 6.0f;
		indicator_size = lerp(indicator_size, indicator_size + 1.0f, frame->hover_t);
		indicator_size = lerp(indicator_size, indicator_size + 1.0f, frame->active_t);
		
		// borders
		draw_set_next_color(color(0x151515ff));
		draw_circle(indicator_pos, indicator_size + 2.0f, 0.0f, 360.0f);

		draw_set_next_color(color(0xe2e2e2ff));
		draw_circle(indicator_pos, indicator_size + 1.0f, 0.0f, 360.0f);

		// color
		draw_set_next_color(hue_col);
		draw_circle(indicator_pos, indicator_size, 0.0f, 360.0f);
	}
}

function void
ui_color_sat_bar_draw_function(ui_frame_t* frame) {

	// get data
	ui_color_data_t* data = (ui_color_data_t*)frame->custom_draw_data;

	// frame info
	f32 frame_width = rect_width(frame->rect);
	f32 frame_height = rect_height(frame->rect);

	// unpack color
	color_t hue_val_col = color_hsv_to_rgb(color(data->hue, 1.0f, data->val, 1.0f));
	color_t rgb_col = color_hsv_to_rgb(color(data->hue, data->sat, data->val, 1.0f));
	color_t val_col = color_hsv_to_rgb(color(0.0f, 0.0f, data->val, 1.0f));

	// draw val bar
	{
		draw_set_next_color0(val_col);
		draw_set_next_color1(val_col);
		draw_set_next_color2(hue_val_col);
		draw_set_next_color3(hue_val_col);
		draw_set_next_radii(frame->rounding);
		draw_rect(frame->rect);
	}

	// draw hue indicator
	{
		vec2_t indicator_pos = vec2(frame->rect.x0 + (data->sat * frame_width), (frame->rect.y0 + frame->rect.y1) * 0.5f);

		f32 indicator_size = 6.0f;
		indicator_size = lerp(indicator_size, indicator_size + 1.0f, frame->hover_t);
		indicator_size = lerp(indicator_size, indicator_size + 1.0f, frame->active_t);

		// borders
		draw_set_next_color(color(0x151515ff));
		draw_circle(indicator_pos, indicator_size + 2.0f, 0.0f, 360.0f);

		draw_set_next_color(color(0xe2e2e2ff));
		draw_circle(indicator_pos, indicator_size + 1.0f, 0.0f, 360.0f);

		// color
		draw_set_next_color(rgb_col);
		draw_circle(indicator_pos, indicator_size, 0.0f, 360.0f);
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
	color_t hue_sat_col = color_hsv_to_rgb(color(data->hue, data->sat, 1.0f, 1.0f));
	color_t rgb_col = color_hsv_to_rgb(color(data->hue, data->sat, data->val, 1.0f));

	// draw val bar
	{
		draw_set_next_color0(color(0x000000ff));
		draw_set_next_color1(color(0x000000ff));
		draw_set_next_color2(hue_sat_col);
		draw_set_next_color3(hue_sat_col);
		draw_set_next_radii(frame->rounding);
		draw_rect(frame->rect);
	}

	// draw hue indicator
	{
		vec2_t indicator_pos = vec2(frame->rect.x0 + (data->val * frame_width), (frame->rect.y0 + frame->rect.y1) * 0.5f);

		f32 indicator_size = 6.0f;
		indicator_size = lerp(indicator_size, indicator_size + 1.0f, frame->hover_t);
		indicator_size = lerp(indicator_size, indicator_size + 1.0f, frame->active_t);

		// borders
		draw_set_next_color(color(0x151515ff));
		draw_circle(indicator_pos, indicator_size + 2.0f, 0.0f, 360.0f);

		draw_set_next_color(color(0xe2e2e2ff));
		draw_circle(indicator_pos, indicator_size + 1.0f, 0.0f, 360.0f);

		// color
		draw_set_next_color(rgb_col);
		draw_circle(indicator_pos, indicator_size, 0.0f, 360.0f);
	}

}

function void
ui_color_quad_draw_function(ui_frame_t* frame) {

	// get data
	ui_color_data_t* data = (ui_color_data_t*)frame->custom_draw_data;

	// unpack color
	color_t hue_col = color_hsv_to_rgb(color(data->hue, 1.0f, 1.0f, 1.0f));
	color_t rgb_col = color_hsv_to_rgb(color(data->hue, data->sat, data->val, 1.0f));

	// frame info
	f32 frame_width = rect_width(frame->rect);
	f32 frame_height = rect_height(frame->rect);

	// draw hue quad
	draw_set_next_color0(color(0xffffffff));
	draw_set_next_color1(color(0x000000ff));
	draw_set_next_color2(hue_col);
	draw_set_next_color3(color(0x000000ff));
	draw_set_next_radii(frame->rounding);
	draw_rect(frame->rect);


	{
		// draw indicator
		vec2_t indicator_pos = vec2(
			frame->rect.x0 + (data->sat * frame_width),
			frame->rect.y0 + ((1.0f - data->val) * frame_height)
		);

		f32 indicator_size = 6.0f;
		indicator_size = lerp(indicator_size, indicator_size + 1.0f, frame->hover_t);
		indicator_size = lerp(indicator_size, indicator_size + 1.0f, frame->active_t);

		// borders
		draw_set_next_color(color(0x151515ff));
		draw_circle(indicator_pos, indicator_size + 2.0f, 0.0f, 360.0f);

		draw_set_next_color(color(0xe2e2e2ff));
		draw_circle(indicator_pos, indicator_size + 1.0f, 0.0f, 360.0f);

		// color
		draw_set_next_color(rgb_col);
		draw_circle(indicator_pos, indicator_size, 0.0f, 360.0f);
	}


}

function void
ui_color_ring_draw_function(ui_frame_t* frame) {

	// get data
	ui_color_data_t* data = (ui_color_data_t*)frame->custom_draw_data;

	// unpack color
	color_t hue_col = color_hsv_to_rgb(color(data->hue, 1.0f, 1.0f, 1.0f));
	color_t rgb_col = color_hsv_to_rgb(color(data->hue, data->sat, data->val, 1.0f));

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
		draw_set_next_color(color(0x15151580));
		draw_set_next_thickness(wheel_thickness);
		draw_circle(vec2_add(frame_center, 1.0f), wheel_radius, 0.0f, 360.0f);

		// draw hue arcs
		f32 step = 1.0f / 6.0f;
		color_t segments[] = {
			color_hsv_to_rgb({0 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({1 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({2 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({3 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({4 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({5 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({6 * step, 1.0f, 1.0f, 1.0f}),
		};

		for (i32 i = 0; i < 6; i++) {
			draw_set_next_color0(segments[i + 0]);
			draw_set_next_color1(segments[i + 1]);
			draw_set_next_color2(segments[i + 0]);
			draw_set_next_color3(segments[i + 1]);
			draw_set_next_thickness(wheel_thickness);

			f32 start_angle = ((i + 0) * step) * 360.0f;
			f32 end_angle = ((i + 1) * step) * 360.0f;
			draw_circle(frame_center, wheel_radius, start_angle, end_angle);
		}
	}

	// draw sat val triangle
	{

		// border
		draw_set_next_color(color(0x15151580));
		draw_tri(vec2_add(tri_p0, 1.0f), vec2_add(tri_p1, 1.0f), vec2_add(tri_p2, 1.0f));

		draw_set_next_color0(hue_col);
		draw_set_next_color1(color(0xffffffff));
		draw_set_next_color2(color(0x000000ff));
		draw_tri(tri_p0, tri_p1, tri_p2);
	}

	// draw hue indicator
	{
		vec2_t indicator_pos = vec2_add(frame_center, vec2_from_angle(data->hue * 2.0f * f32_pi, wheel_radius - (wheel_thickness * 0.5f)));

		// borders
		draw_set_next_color(color(0x151515ff));
		draw_circle(indicator_pos, 8.0f, 0.0f, 360.0f);

		draw_set_next_color(color(0xe2e2e2ff));
		draw_circle(indicator_pos, 7.0f, 0.0f, 360.0f);

		//color
		draw_set_next_color(hue_col);
		draw_circle(indicator_pos, 6.0f, 0.0f, 360.0f);
	}

	// draw sat val indicator
	{
		vec2_t indicator_pos = vec2_lerp(vec2_lerp(tri_p1, tri_p0, clamp_01(data->sat)), tri_p2, clamp_01(1.0f - data->val));

		// borders
		draw_set_next_color(color(0x151515ff));
		draw_circle(indicator_pos, 8.0f, 0.0f, 360.0f);

		draw_set_next_color(color(0xe2e2e2ff));
		draw_circle(indicator_pos, 7.0f, 0.0f, 360.0f);

		// color
		draw_set_next_color(rgb_col);
		draw_circle(indicator_pos, 6.0f, 0.0f, 360.0f);
	}

}

function void
ui_color_wheel_draw_function(ui_frame_t* frame) {

	// get data
	ui_color_data_t* data = (ui_color_data_t*)frame->custom_draw_data;

	// unpack color
	color_t hue_col = color_hsv_to_rgb(color(data->hue, 1.0f, 1.0f, 1.0f));
	color_t rgb_col = color_hsv_to_rgb(color(data->hue, data->sat, data->val, 1.0f));
	color_t hue_sat_col = color_hsv_to_rgb(color(data->hue, data->sat, 1.0f, 1.0f));

	// frame info
	f32 frame_width = rect_width(frame->rect);
	f32 frame_height = rect_height(frame->rect);
	vec2_t frame_center = rect_center(frame->rect);
	f32 wheel_radius = min(frame_width, frame_height) * 0.5f;
	f32 wheel_thickness = wheel_radius * 0.15f;

	// draw hue sat circle
	{
		// draw shadow
		draw_set_next_color(color(0x15151580));
		draw_circle(vec2_add(frame_center, 1.0f), wheel_radius + 1.0f, 0.0f, 360.0f);

		// draw hue bars
		f32 step = 1.0f / 6.0f;
		color_t segments[] = {
			color_hsv_to_rgb({0 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({1 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({2 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({3 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({4 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({5 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({6 * step, 1.0f, 1.0f, 1.0f}),
		};

		for (i32 i = 0; i < 6; i++) {
			draw_set_next_color0(color(0xffffffff));
			draw_set_next_color1(color(0xffffffff));
			draw_set_next_color2(segments[i + 0]);
			draw_set_next_color3(segments[i + 1]);

			f32 start_angle = ((i + 0) * step) * 360.0f;
			f32 end_angle = ((i + 1) * step) * 360.0f;
			draw_circle(frame_center, wheel_radius, start_angle, end_angle);
		}

		// draw circle border
		draw_set_next_color(frame->color_group.border);
		draw_set_next_thickness(1.75f);
		draw_circle(frame_center, wheel_radius + 0.25f, 0.0f, 360.0f);
	}

	// draw hue sat indicator
	{
		vec2_t indicator_pos = vec2_add(frame_center, vec2_from_angle(data->hue * 2.0f * f32_pi, wheel_radius * data->sat));

		f32 indicator_size = 6.0f;
		indicator_size = lerp(indicator_size, indicator_size + 1.0f, frame->hover_t);
		indicator_size = lerp(indicator_size, indicator_size + 1.0f, frame->active_t);

		// borders
		draw_set_next_color(color(0x151515ff));
		draw_circle(indicator_pos, indicator_size + 2.0f, 0.0f, 360.0f);

		draw_set_next_color(color(0xe2e2e2ff));
		draw_circle(indicator_pos, indicator_size + 1.0f, 0.0f, 360.0f);

		// color
		draw_set_next_color(hue_sat_col);
		draw_circle(indicator_pos, indicator_size, 0.0f, 360.0f);
	}

}

function void 
ui_color_hex_draw_function(ui_frame_t* frame) {

	// get data
	ui_color_data_t* data = (ui_color_data_t*)frame->custom_draw_data;

	// unpack color
	color_t hue_col = color_hsv_to_rgb(color(data->hue, 1.0f, 1.0f, 1.0f));
	color_t rgb_col = color_hsv_to_rgb(color(data->hue, data->sat, data->val, 1.0f));
	color_t hue_sat_col = color_hsv_to_rgb(color(data->hue, data->sat, 1.0f, 1.0f));

	// frame info
	f32 frame_width = rect_width(frame->rect);
	f32 frame_height = rect_height(frame->rect);
	vec2_t frame_center = rect_center(frame->rect);
	f32 wheel_radius = min(frame_width, frame_height) * 0.5f;
	f32 wheel_thickness = wheel_radius * 0.15f;

	// draw hue sat circle
	{
		// draw shadow
		draw_set_next_color(color(0x15151580));
		draw_circle(vec2_add(frame_center, 1.0f), wheel_radius, 0.0f, 360.0f);

		// draw hue bars
		f32 step = 1.0f / 6.0f;
		color_t segments[] = {
			color_hsv_to_rgb({0 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({1 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({2 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({3 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({4 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({5 * step, 1.0f, 1.0f, 1.0f}),
			color_hsv_to_rgb({6 * step, 1.0f, 1.0f, 1.0f}),
		};

		for (i32 i = 0; i < 6; i++) {
			draw_set_next_color0(color(0xffffffff));
			//draw_set_next_color1(color(0xffffffff));
			draw_set_next_color1(segments[i + 0]);
			draw_set_next_color2(segments[i + 1]);

			f32 start_angle = ((i + 0) * step) * 360.0f;
			f32 end_angle = ((i + 1) * step) * 360.0f;
			vec2_t start_pos = vec2_add(frame_center, vec2_rotate(vec2(wheel_radius, 0.0f), radians(start_angle)));
			vec2_t end_pos = vec2_add(frame_center, vec2_rotate(vec2(wheel_radius, 0.0f), radians(end_angle)));
			//draw_circle(frame_center, wheel_radius, start_angle, end_angle);
			draw_tri(frame_center, start_pos, end_pos);
		}
	}

	// draw hue sat indicator
	{
		vec2_t indicator_pos = vec2_add(frame_center, vec2_from_angle(data->hue * 2.0f * f32_pi, wheel_radius * data->sat));

		// borders
		draw_set_next_color(color(0x151515ff));
		draw_circle(indicator_pos, 8.0f, 0.0f, 360.0f);

		draw_set_next_color(color(0xe2e2e2ff));
		draw_circle(indicator_pos, 7.0f, 0.0f, 360.0f);

		// color
		draw_set_next_color(hue_sat_col);
		draw_circle(indicator_pos, 6.0f, 0.0f, 360.0f);
	}

}

function void
ui_text_edit_draw_function(ui_frame_t* frame) {

	if (ui_key_equals(ui_state.ui_active->focused_frame_key, frame->key)) {

		// get offsets
		f32 cursor_offset = ui_text_offset_from_index(frame->font, frame->font_size, frame->string, ui_state.ui_active->cursor.column);
		f32 mark_offset = ui_text_offset_from_index(frame->font, frame->font_size, frame->string, ui_state.ui_active->mark.column);
		vec2_t text_start = ui_text_align(frame->font, frame->font_size, frame->string, frame->rect, frame->text_alignment);
		ui_state.ui_active->cursor_target_pos.x = cursor_offset;
		ui_state.ui_active->mark_target_pos.x = mark_offset;

		// draw cursor
		draw_set_next_color(frame->color_group.accent);

		f32 left = ui_state.ui_active->cursor_pos.x;
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
		draw_rect(cursor_rect);

		// draw mark
		color_t mark_color = frame->color_group.accent;
		mark_color.a = 0.3f;
		draw_set_next_color(mark_color);
		rect_t mark_rect = rect(
			frame->rect.x0 + text_start.x + cursor_offset,
			frame->rect.y0 + 2.0f,
			frame->rect.x0 + text_start.x + mark_offset + 2.0f,
			frame->rect.y1 - 2.0f
		);
		draw_rect(mark_rect);

	}

}


// stack functions

// macro hell :(

#define ui_stack_top_impl(name, type) \
function type \
ui_top_##name() { \
	return ui_state.ui_active->name##_stack.top->v; \
} \

#define ui_stack_push_impl(name, type) \
function type \
ui_push_##name(type v) { \
ui_##name##_node_t* node = ui_state.ui_active->name##_stack.free; \
if (node != 0) { \
	stack_pop(ui_state.ui_active->name##_stack.free); \
} else { \
	node = (ui_##name##_node_t*)arena_alloc(ui_state.ui_active->per_frame_arena, sizeof(ui_##name##_node_t)); \
} \
type old_value = ui_state.ui_active->name##_stack.top->v; \
node->v = v; \
stack_push(ui_state.ui_active->name##_stack.top, node); \
ui_state.ui_active->name##_stack.auto_pop = 0; \
return old_value; \
} \

#define ui_stack_pop_impl(name, type) \
function type \
ui_pop_##name() { \
ui_##name##_node_t* popped = ui_state.ui_active->name##_stack.top; \
if (popped != 0) { \
	stack_pop(ui_state.ui_active->name##_stack.top); \
	stack_push(ui_state.ui_active->name##_stack.free, popped); \
	ui_state.ui_active->name##_stack.auto_pop = 0; \
} \
return popped->v; \
} \

#define ui_stack_set_next_impl(name, type) \
function type \
ui_set_next_##name(type v) { \
ui_##name##_node_t* node = ui_state.ui_active->name##_stack.free; \
if (node != 0) { \
	stack_pop(ui_state.ui_active->name##_stack.free); \
} else { \
	node = (ui_##name##_node_t*)arena_alloc(ui_state.ui_active->per_frame_arena, sizeof(ui_##name##_node_t)); \
} \
type old_value = ui_state.ui_active->name##_stack.top->v; \
node->v = v; \
stack_push(ui_state.ui_active->name##_stack.top, node); \
ui_state.ui_active->name##_stack.auto_pop = 1; \
return old_value; \
} \

#define ui_stack_auto_pop_impl(name) \
if (ui_state.ui_active->name##_stack.auto_pop) { ui_pop_##name(); ui_state.ui_active->name##_stack.auto_pop = 0; }

#define ui_stack_impl(name, type)\
ui_stack_top_impl(name, type)\
ui_stack_push_impl(name, type)\
ui_stack_pop_impl(name, type)\
ui_stack_set_next_impl(name, type)\

function void
ui_auto_pop_stacks() {
	
	ui_stack_auto_pop_impl(parent);
	ui_stack_auto_pop_impl(flags);
	ui_stack_auto_pop_impl(seed_key);
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
	ui_stack_auto_pop_impl(color_group);
	ui_stack_auto_pop_impl(font);
	ui_stack_auto_pop_impl(font_size);
	ui_stack_auto_pop_impl(focus_hot);
	ui_stack_auto_pop_impl(focus_active);

}

ui_stack_impl(parent, ui_frame_t*)
ui_stack_impl(flags, ui_frame_flags)
ui_stack_impl(seed_key, ui_key_t)
ui_stack_impl(fixed_x, f32)
ui_stack_impl(fixed_y, f32)
ui_stack_impl(fixed_width, f32)
ui_stack_impl(fixed_height, f32)
ui_stack_impl(pref_width, ui_size_t)
ui_stack_impl(pref_height, ui_size_t)
ui_stack_impl(text_alignment, ui_text_alignment)
ui_stack_impl(text_padding, f32)
ui_stack_impl(hover_cursor, os_cursor)
ui_stack_impl(layout_axis, ui_axis)
ui_stack_impl(rounding_00, f32)
ui_stack_impl(rounding_01, f32)
ui_stack_impl(rounding_10, f32)
ui_stack_impl(rounding_11, f32)
ui_stack_impl(color_group, ui_color_group_t)
ui_stack_impl(font, font_t*)
ui_stack_impl(font_size, f32)
ui_stack_impl(focus_hot, ui_focus_type)
ui_stack_impl(focus_active, ui_focus_type)
ui_stack_impl(texture, gfx_texture_t*)

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


function void 
ui_push_rect(rect_t rect) {
	ui_push_fixed_x(rect.x0);
	ui_push_fixed_y(rect.y0);
	ui_push_fixed_width(rect.x1 - rect.x0);
	ui_push_fixed_height(rect.y1 - rect.y0);
}

function void 
ui_pop_rect() {
	ui_pop_fixed_x();
	ui_pop_fixed_y();
	ui_pop_fixed_width();
	ui_pop_fixed_height();
}

function void 
ui_set_next_rect(rect_t rect) {
	ui_set_next_fixed_x(rect.x0);
	ui_set_next_fixed_y(rect.y0);
	ui_set_next_fixed_width(rect.x1 - rect.x0);
	ui_set_next_fixed_height(rect.y1 - rect.y0);
}




#endif // UI_CPP