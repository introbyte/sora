// ui.cpp

#ifndef UI_CPP
#define UI_CPP

// implementation

//- state

function void
ui_init() {
    
	// allocate memory
	ui_state.context_arena = arena_create(megabytes(64));
	ui_state.event_arena = arena_create(kilobytes(4));
	ui_state.command_arena = arena_create(kilobytes(4));
    
	// clear context list
	ui_state.context_first = nullptr;
	ui_state.context_last = nullptr;
	ui_state.context_free = nullptr;
	ui_state.context_active = nullptr;
    
    // clear command list
    ui_state.command_first = nullptr;
    ui_state.command_last = nullptr;
    ui_state.command_free = nullptr;
    
    // clear event list
    ui_state.event_first = nullptr;
    ui_state.event_last = nullptr;
    
    // load icon font
    ui_state.icon_font = font_open(str("res/fonts/icons.ttf"));
    
    // event bindings
    {
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
        
    }
    
    
    
}

function void
ui_release() {
	
    // close icon font
    font_close(ui_state.icon_font);
    
    // release arenas
    arena_clear(ui_state.context_arena);
	arena_clear(ui_state.event_arena);
	arena_clear(ui_state.command_arena);
    
}

function void
ui_update() {
    
    // clear event arena
    arena_clear(ui_state.event_arena);
    ui_state.event_first = nullptr;
    ui_state.event_last = nullptr;
    
    // reset mouse clicks
    u64 time_now = (os_time_microseconds() / 1000);
    for (i32 i = 0; i < 3; i++) {
        u32 time_since = time_now - ui_state.last_click_time[i];
        if (time_since > os_state.double_click_time) {
            ui_state.click_counter[i] = 0;
        }
    }
    
    // gather events
    for (os_event_t* os_event = os_state.event_list.first; os_event != nullptr; os_event = os_event->next) {
        ui_event_t ui_event = { 0 };
        
        // start with default
        ui_event.os_event = os_event;
        ui_event.window = os_event->window;
        ui_event.type = ui_event_type_none;
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
                        ui_event.type = ui_event_type_none;
                    }
                    break; 
                }
                case os_event_type_mouse_scroll: { ui_event.type = ui_event_type_mouse_scroll; break; }
            }
            ui_event_push(&ui_event);
        }
        
    }
    
}

function void
ui_begin(ui_context_t* context) {
    
	// set active context
	ui_state.context_active = context;
    context->frame_mrc = nullptr; // most recently created
    
    // handle inputs
    for (ui_event_t* event = ui_state.event_first; event != nullptr; event = event->next) {
        if (!os_handle_equals(event->window, context->window)) { continue; }
        
        // drop drag state
        if (event->type == ui_event_type_mouse_release && ui_drag_is_active()) {
            context->drag_state = ui_drag_state_dropping;
        }
        
        if (event->type == ui_event_type_mouse_press) {
            
            // find which panel we clicked on 
            for (ui_panel_t* panel = context->panel_root; panel != nullptr; panel = ui_panel_rec_depth_first(panel).next) {
                
                // skip if not leaf panel
                if (panel->tree_first != nullptr) { continue; }
                
                if (rect_contains(panel->frame->rect, event->position)) {
                    ui_cmd_t* cmd = ui_cmd_push(context, ui_cmd_type_focus_panel);
                    cmd->src_panel = panel;
                    cmd->view = panel->view_focus;
                    
                }
                
                
                
            }
            
        }
        
    }
    
    // process commands
    for (ui_cmd_t* command = ui_state.command_first, *next = nullptr; command != nullptr; command = next) {
        next = command->next;
        if (command->context != context) { continue; }
        
        switch (command->type) {
            
            case ui_cmd_type_focus_panel: {
                
                if (command->src_panel != nullptr) {
                    context->panel_focused = command->src_panel;
                    
                    if (command->view != nullptr) {
                        context->view_focus = command->view;
                        command->src_panel->view_focus = command->view;
                    }
                }
                break;
            }
            
            case ui_cmd_type_close_panel: {
                
                // get panels
                ui_panel_t* panel = command->src_panel;
                ui_panel_t* parent = panel->tree_parent;
                
                // skip if last panel so we can't remove all panels
                if (panel == context->panel_root) { break; }
                
                // if no sibling panels
                if (parent->child_count == 2) {
                    
                    // get panels
                    ui_panel_t* discard_panel = panel;
                    ui_panel_t* keep_panel = panel == parent->tree_first ? parent->tree_last : parent->tree_first;
                    ui_panel_t* grandparent = parent->tree_parent;
                    ui_panel_t* parent_prev = parent->tree_prev;
                    f32 percent_of_parent = parent->percent_of_parent;
                    
                    ui_panel_remove(keep_panel);
                    
                    if (grandparent != nullptr) {
                        ui_panel_remove(parent);
                    }
                    
                    ui_panel_release(context, parent);
                    ui_panel_release(context, discard_panel);
                    
                    if (grandparent == nullptr) {
                        context->panel_root = keep_panel;
                    } else {
                        ui_panel_insert(grandparent, keep_panel, parent_prev);
                    }
                    keep_panel->percent_of_parent = percent_of_parent;
                    
                    // focus panel
                    if (context->panel_focused == discard_panel) {
                        context->panel_focused = keep_panel;
                        for (ui_panel_t* grandchild = context->panel_focused; grandchild != nullptr; grandchild = grandchild->tree_first) {
                            context->panel_focused = grandchild;
                        }
                    }
                    
                    if (grandparent != nullptr && grandparent->split_axis == keep_panel->split_axis && keep_panel->tree_first != nullptr) {
                        ui_panel_remove(keep_panel);
                        ui_panel_t* prev = parent_prev;
                        for (ui_panel_t* child = keep_panel->tree_first, *next = 0; child != nullptr; child = next) {
                            next = child->tree_next;
                            ui_panel_remove(child);
                            ui_panel_insert(grandparent, child, prev);
                            prev = child;
                            child->percent_of_parent *= keep_panel->percent_of_parent;
                        }
                        ui_panel_release(context, keep_panel);
                    }
                } else {
                    
                    ui_panel_t* next = nullptr;
                    f32 removed_size_percent = panel->percent_of_parent;
                    if (next == nullptr) { next = panel->tree_prev; }
                    if (next == nullptr) { next = panel->tree_next; }
                    ui_panel_remove(panel);
                    ui_panel_release(context, panel);
                    
                    // focus panel
                    //if (context->panel_focused == discard_panel) {
                    //context->panel_focused = keep_panel;
                    //for (ui_panel_t* grandchild = context->panel_focused; grandchild != nullptr; grandchild = grandchild->tree_first) {
                    //context->panel_focused = grandchild;
                    //}
                    //}
                    
                    // set sizes
                    for (ui_panel_t* child = parent->tree_first; child != nullptr; child = child->tree_next) {
                        child->percent_of_parent /= 1.0f - removed_size_percent;
                    }
                }
                
                break;
            }
            
            case ui_cmd_type_split_panel: {
                
                ui_panel_t* split_panel = command->dst_panel;
                ui_axis split_axis = ui_axis_from_dir(command->dir);
                ui_side split_side = ui_side_from_dir(command->dir);
                
                ui_panel_t* new_panel = nullptr;
                ui_panel_t* split_parent = split_panel->tree_parent;
                
                // if the parents split axis is the same
                if (split_parent != nullptr && split_parent->split_axis == split_axis) {
                    
                    // create and insert panel
                    ui_panel_t* inserted_panel = ui_panel_create(context);
                    ui_panel_insert(split_parent, inserted_panel, split_side == ui_side_max ? split_panel : split_panel->tree_prev);
                    inserted_panel->percent_of_parent = 1.0f / (f32)split_parent->child_count;
                    
                    // update sizes
                    for (ui_panel_t* child = split_parent->tree_first; child != nullptr; child = child->tree_next) {
                        if (child != inserted_panel) {
                            child->percent_of_parent *= (f32)(split_parent->child_count - 1) / (split_parent->child_count);
                        }
                    }
                    
                    context->panel_focused = inserted_panel;
                    new_panel = inserted_panel;
                } else {
                    // parents split axis is not the same
                    
                    // create new parent with correct split axis
                    ui_panel_t* pre_prev = split_panel->tree_prev;
                    ui_panel_t* pre_parent = split_parent;
                    ui_panel_t* new_parent = ui_panel_create(context, split_panel->percent_of_parent, split_axis);
                    
                    // reorder panels
                    if (pre_parent != nullptr) {
                        ui_panel_remove(split_panel);
                        ui_panel_insert(pre_parent, new_parent, pre_prev);
                    } else {
                        context->panel_root = new_parent;
                    }
                    
                    // create new panel
                    ui_panel_t* left = split_panel;
                    ui_panel_t* right = ui_panel_create(context);
                    new_panel = right;
                    
                    if (split_side == ui_side_min) {
                        ui_panel_t* t = left;
                        left = right;
                        right = t;
                    }
                    
                    left->percent_of_parent = 0.5f;
                    right->percent_of_parent = 0.5f;
                    
                    context->panel_focused = new_panel;
                    
                    // insert both panels
                    ui_panel_insert(new_parent, left);
                    ui_panel_insert(new_parent, right, left);
                    
                }
                
                
                if (command->view != nullptr) {
                    
                    ui_panel_t* from_panel =command->src_panel;
                    
                    ui_view_remove(from_panel, command->view);
                    ui_view_insert(new_panel, command->view, new_panel->view_last);
                    
                    if (from_panel->view_first == nullptr && from_panel != context->panel_root &&
                        from_panel != new_panel->tree_prev && from_panel != new_panel->tree_next) {
                        ui_cmd_t* cmd = ui_cmd_push(context, ui_cmd_type_close_panel);
                        cmd->src_panel = from_panel; 
                    }
                    
                    context->view_focus = new_panel->view_first;
                }
                
                break;
            }
            
            case ui_cmd_type_move_view: {
                
                ui_panel_t* from_panel = command->src_panel;
                ui_panel_t* to_panel = command->dst_panel;
                ui_view_t* view = command->view;
                
                ui_view_remove(from_panel, view);
                ui_view_insert(to_panel, view, to_panel->view_last);
                
                if (from_panel->view_first == nullptr) {
                    ui_cmd_t* cmd = ui_cmd_push(context, ui_cmd_type_close_panel);
                    cmd->src_panel = from_panel; 
                }
                
                context->panel_focused = to_panel;
                context->view_focus = view;
                
                break;
            }
            
        }
        
        // pop command
        ui_cmd_pop(command);
        
    }
    
    // animation rates
    f32 dt = os_window_get_delta_time(context->window);
    context->anim_fast_rate = 1.0f - powf(2.0f, -50.0f * dt);
    context->anim_slow_rate = 1.0f - powf(2.0f, -25.0f * dt);
    
    // remove unused animation nodes
    {
        for (ui_anim_node_t* n = context->anim_node_lru, *next = nullptr; n != nullptr; n = next) {
            next = n->lru_next;
            if (n->last_build_index + 1 < context->build_index) {
                dll_remove_np(context->anim_node_first, context->anim_node_last, n, list_next, list_prev);
                dll_remove_np(context->anim_node_lru, context->anim_node_mru, n, lru_next, lru_prev);
                stack_push_n(context->anim_node_free, n, list_next);
            } else {
                break;
            }
        }
    }
    
    // remove unused data node
    {
        for (ui_data_node_t* n = context->data_node_lru, *next = nullptr; n != nullptr; n = next) {
            next = n->lru_next;
            if (n->last_build_index + 1 < context->build_index) {
                dll_remove_np(context->data_node_first, context->data_node_last, n, list_next, list_prev);
                dll_remove_np(context->data_node_lru, context->data_node_mru, n, lru_next, lru_prev);
                stack_push_n(context->data_node_free, n, list_next);
            } else {
                break;
            }
        }
    }
    
    
    // reset stacks
    ui_context_reset_stacks(context);
    
    // reset popup
    context->popup_updated_this_frame = false;
    
    // reset keys
    {
        
        // reset active keys
        for (i32 i = 0 ; i < 3; i++) {
            ui_frame_t* frame = ui_frame_find(context->key_active[i]);
            
            if (frame == nullptr) {
                context->key_active[i] = { 0 };
            }
        }
        
        // reset hover key
        context->key_hovered_prev = context->key_hovered;
        context->key_hovered = { 0 };
        
    }
    
    // get input
    context->mouse_pos = os_window_get_cursor_pos(context->window);
    context->mouse_delta = os_window_get_mouse_delta(context->window);
    uvec2_t content_size = gfx_renderer_get_size(context->renderer);
    rect_t content_rect = rect(0.0f, 0.0f, content_size.x, content_size.y);
    
    // create root frame
    {
        ui_set_next_rect(content_rect);
        ui_key_t root_key = ui_key_from_stringf({ 0 }, "%p_window_root_frame", context);
        context->frame_root = ui_frame_from_key(0, root_key);
        ui_frame_set_display_string(context->frame_root, str("window_root_frame")); // for debug
        ui_push_parent(context->frame_root);
    }
    
    // create tooltip frame
    {
        ui_frame_flags tooltip_flags = ui_frame_flag_floating;
        ui_set_next_fixed_x(context->mouse_pos.x + 15.0f);
        ui_set_next_fixed_y(context->mouse_pos.y + 15.0f);
        ui_set_next_size(ui_size_by_children(1.0f), ui_size_by_children(1.0f));
        ui_set_next_padding(2.0f);
        ui_set_next_color_background(color(0x1d1d1dff));
        ui_key_t tooltip_key = ui_key_from_stringf({ 0 }, "%p_tooltip_root_frame", context);
        context->frame_tooltip = ui_frame_from_key(tooltip_flags, tooltip_key);
        ui_frame_set_display_string(context->frame_tooltip, str("tooltip_root_frame")); // for debug
    }
    // create popup frame
    {
        ui_frame_flags popup_flags = 
            ui_frame_flag_interactable |
            ui_frame_flag_floating;
        
        ui_key_t popup_key = ui_key_from_stringf({ 0 }, "%p_popup_root_frame", context);
        ui_set_next_fixed_x(context->popup_pos.x);
        ui_set_next_fixed_y(context->popup_pos.y);
        ui_set_next_size(ui_size_by_children(1.0f), ui_size_by_children(1.0f));
        ui_set_next_padding(2.0f);
        ui_set_next_color_background(color(0x1d1d1dff));
        context->frame_popup = ui_frame_from_key(popup_flags, popup_key);
        ui_frame_set_display_string(context->frame_popup, str("popup_root_frame")); // for debug
    }
    
    // drop site and visualization params
    
    ui_frame_flags drop_site_flags = 
        ui_frame_flag_interactable |
        ui_frame_flag_draw_background |
        ui_frame_flag_draw_border |
        ui_frame_flag_draw_custom;
    
    ui_frame_flags vis_panel_flags =
        ui_frame_flag_draw_background |
        ui_frame_flag_draw_border;
    
    color_t drop_background_color = color(0x151515ff);
    color_t drop_border_color = color(0x353535ff);
    
    color_t vis_background_color = color(0x50505080);
    color_t vis_border_color = color(0x80808080);
    
    f32 drop_site_major_size = 40.0f;
    f32 drop_site_minor_size = 30.0f;
    
    f32 vis_major_size = 50.0f;
    
    // create non leaf panel ui
    {
        
        for (ui_panel_t* panel = context->panel_root; panel != nullptr; panel = ui_panel_rec_depth_first(panel).next) {
            
            // skip if leaf panel
            if (panel->tree_first == nullptr) { continue; }
            
            // calculate rect
            rect_t panel_rect = ui_rect_from_panel(panel, content_rect);
            ui_axis split_axis = panel->split_axis;
            
            // boundary drop sites
            if (ui_drag_is_active() && context->view_drag != nullptr) {
                
                // build root drop sites
                if (panel == context->panel_root) {
                    
                    for (ui_side side = ui_side_min; side < ui_side_count; side++) {
                        
                        // calculate rect
                        vec2_t panel_rect_center = rect_center(panel_rect);
                        rect_t drop_rect = rect(panel_rect_center, panel_rect_center);
                        
                        drop_rect.v0[split_axis] = panel_rect_center[split_axis] - drop_site_major_size;
                        drop_rect.v1[split_axis] = panel_rect_center[split_axis] + drop_site_major_size;
                        
                        if (side == ui_side_min) {
                            drop_rect.v0[!split_axis] = panel_rect.v[side][!split_axis] + 10.0f;
                            drop_rect.v1[!split_axis] = panel_rect.v[side][!split_axis] + 10.0f + (drop_site_minor_size * 2.0f);
                        } else {
                            drop_rect.v0[!split_axis] = panel_rect.v[side][!split_axis] - 10.0f - (drop_site_minor_size * 2.0f);
                            drop_rect.v1[!split_axis] = panel_rect.v[side][!split_axis] - 10.0f;
                        }
                        
                        // build frame
                        ui_set_next_rect(drop_rect);
                        ui_set_next_color_background(drop_background_color);
                        ui_set_next_color_border(drop_border_color);
                        ui_set_next_rounding(vec4(5.0f));
                        ui_key_t drop_key = ui_key_from_stringf({ 0 }, "root_drop_site_%i", side);
                        ui_frame_t* drop_frame = ui_frame_from_key(drop_site_flags, drop_key);
                        ui_drop_site_draw_data_t* data = (ui_drop_site_draw_data_t*)arena_alloc(ui_build_arena(), sizeof(ui_drop_site_draw_data_t));
                        data->type = ui_drop_site_type_edge;
                        data->axis = !split_axis;
                        data->side = side;
                        ui_frame_set_custom_draw(drop_frame, ui_drop_site_draw_function, data);
                        ui_frame_interaction(drop_frame);
                        
                        // visualize new panel
                        if (ui_key_equals(drop_key, context->key_hovered)) {
                            
                            // calculate rect
                            rect_t new_panel_rect = drop_rect;
                            new_panel_rect.v0[split_axis] = panel_rect.v0[split_axis] + 5.0f;
                            new_panel_rect.v1[split_axis] = panel_rect.v1[split_axis] - 5.0f;
                            
                            if (side == ui_side_min) {
                                new_panel_rect.v0[!split_axis] = panel_rect.v0[!split_axis] + 5.0f;
                                new_panel_rect.v1[!split_axis] += vis_major_size * drop_frame->hover_t;
                            } else {
                                new_panel_rect.v0[!split_axis] -= vis_major_size * drop_frame->hover_t;
                                new_panel_rect.v1[!split_axis] = panel_rect.v1[!split_axis] - 5.0f;
                            }
                            
                            //build visualization frame
                            ui_set_next_rect(new_panel_rect);
                            ui_set_next_color_background(vis_background_color);
                            ui_set_next_color_border(vis_border_color);
                            ui_frame_from_key(vis_panel_flags, { 0 });
                            
                        }
                        
                        // perform drop
                        if (ui_key_equals(drop_key, context->key_hovered) && ui_drag_drop()) {
                            
                            // detemine split panel and directiob
                            ui_dir dir = (split_axis == ui_axis_x ? (side == ui_side_min ? ui_dir_up : ui_dir_down) :
                                          split_axis == ui_axis_y ? (side == ui_side_min ? ui_dir_left: ui_dir_right) :
                                          ui_dir_none);
                            
                            if (dir != ui_dir_none) {
                                
                                // push command
                                ui_cmd_t* cmd = ui_cmd_push(context, ui_cmd_type_split_panel);
                                cmd->dst_panel = panel;
                                cmd->src_panel = context->view_drag->parent_panel;
                                cmd->dir = dir;
                                cmd->view = context->view_drag;
                                
                            }
                        }
                        
                    }
                    
                }
                
                // interate through children building drop sites
                for (ui_panel_t* child = panel->tree_first;  ; child = child->tree_next) {
                    
                    // calculate drop rect
                    rect_t child_rect = ui_rect_from_panel_child(panel, child, panel_rect);
                    vec2_t child_rect_center = rect_center(child_rect);
                    
                    // determine if edge or split
                    rect_t drop_rect;
                    ui_drop_site_type drop_type;
                    ui_side drop_side = 0;
                    if (child != panel->tree_first && child != nullptr) {
                        drop_type = ui_drop_site_type_split;
                        drop_rect = rect(child_rect_center, child_rect_center);
                        drop_rect.v0[split_axis] = child_rect.v0[split_axis] - drop_site_minor_size * 1.5f;
                        drop_rect.v1[split_axis] = child_rect.v0[split_axis] + drop_site_minor_size * 1.5f;
                        drop_rect.v0[!split_axis] -= drop_site_major_size;
                        drop_rect.v1[!split_axis] += drop_site_major_size;
                    } else {
                        drop_type = ui_drop_site_type_edge;
                        drop_rect = rect(child_rect_center, child_rect_center);
                        if (child == panel->tree_first) {
                            drop_rect.v0[split_axis] = child_rect.v0[split_axis] + 10.0f;
                            drop_rect.v1[split_axis] = child_rect.v0[split_axis] + 10.0f + drop_site_minor_size * 2.0f;
                            drop_side = ui_side_min;
                        } else {
                            drop_rect.v0[split_axis] = child_rect.v0[split_axis] - 10.0f - drop_site_minor_size * 2.0f;
                            drop_rect.v1[split_axis] = child_rect.v0[split_axis] - 10.0f;
                            drop_side = ui_side_max;
                        }
                        drop_rect.v0[!split_axis] -= drop_site_major_size;
                        drop_rect.v1[!split_axis] += drop_site_major_size;
                    }
                    
                    // build drop frame
                    ui_set_next_rect(drop_rect);
                    ui_set_next_rounding(vec4(5.0f));
                    ui_set_next_color_background( drop_background_color);
                    ui_set_next_color_border(drop_border_color);
                    ui_key_t drop_key = ui_key_from_stringf({0}, "drop_boundary_%p_%p", panel, child);
                    ui_frame_t* drop_frame = ui_frame_from_key(drop_site_flags, drop_key);
                    ui_frame_interaction(drop_frame);
                    
                    // set draw data
                    ui_drop_site_draw_data_t* data = (ui_drop_site_draw_data_t*)arena_alloc(ui_build_arena(), sizeof(ui_drop_site_draw_data_t));
                    data->type = drop_type;
                    data->axis = split_axis;
                    data->side = drop_side;
                    ui_frame_set_custom_draw(drop_frame, ui_drop_site_draw_function, data);
                    
                    // visualize new panel
                    if (ui_key_equals(drop_key, context->key_hovered)) {
                        
                        // calculate rect
                        rect_t new_panel_rect = drop_rect;
                        
                        if (drop_type == ui_drop_site_type_split) {
                            new_panel_rect.v0[split_axis] -= vis_major_size * drop_frame->hover_t;
                            new_panel_rect.v1[split_axis] += vis_major_size * drop_frame->hover_t;
                            new_panel_rect.v0[!split_axis] = child_rect.v0[!split_axis] + 5.0f;
                            new_panel_rect.v1[!split_axis] = child_rect.v1[!split_axis] - 5.0f;
                        } else {
                            
                            if (child == panel->tree_first) {
                                new_panel_rect.v0[split_axis] -= 5.0f;
                                new_panel_rect.v1[split_axis] += vis_major_size * drop_frame->hover_t;
                            } else {
                                new_panel_rect.v0[split_axis] -= vis_major_size * drop_frame->hover_t;
                                new_panel_rect.v1[split_axis] += 5.0f;
                            }
                            
                            new_panel_rect.v0[!split_axis] = child_rect.v0[!split_axis] + 5.0f;
                            new_panel_rect.v1[!split_axis] = child_rect.v1[!split_axis] - 5.0f;
                        }
                        
                        
                        //build visualization frame
                        ui_set_next_rect(new_panel_rect);
                        ui_set_next_color_background(vis_background_color);
                        ui_set_next_color_border(vis_border_color);
                        ui_frame_from_key(vis_panel_flags, { 0 });
                    }
                    
                    // perform drop
                    if (ui_key_equals(drop_key, context->key_hovered) && ui_drag_drop()) {
                        
                        // detemine split panel and direction
                        ui_panel_t* split_panel = child;
                        ui_dir split_dir = (split_axis == ui_axis_x) ? ui_dir_left : ui_dir_up;
                        if (split_panel == nullptr) {
                            split_panel = panel->tree_last;
                            split_dir = (split_axis == ui_axis_x) ? ui_dir_right : ui_dir_down;
                        }
                        
                        // push command
                        ui_cmd_t* cmd = ui_cmd_push(context, ui_cmd_type_split_panel);
                        cmd->src_panel = context->view_drag->parent_panel;
                        cmd->dst_panel = split_panel;
                        cmd->dir = split_dir;
                        cmd->view = context->view_drag;
                    }
                    
                    // end on opl
                    if (child == nullptr) {
                        break;
                    }
                    
                }
                
            }
            
            
            // build drag boundaries
            for (ui_panel_t* child = panel->tree_first; child != nullptr && child->tree_next != nullptr; child = child->tree_next) {
                
                ui_panel_t* min_child = child;
                ui_panel_t* max_child = child->tree_next;
                
                rect_t min_child_rect = ui_rect_from_panel_child(min_child->tree_parent, min_child, panel_rect);
                rect_t max_child_rect = ui_rect_from_panel_child(max_child->tree_parent, max_child, panel_rect);
                
                // calculate boundary rect
                rect_t boundary_rect = { 0 };
                boundary_rect.v0[split_axis]  = min_child_rect.v1[split_axis] - 8.0f;
                boundary_rect.v1[split_axis]  = max_child_rect.v0[split_axis] + 8.0f;
                boundary_rect.v0[!split_axis] = panel_rect.v0[!split_axis];
                boundary_rect.v1[!split_axis] = panel_rect.v1[!split_axis];	
                
                // build frame
                ui_frame_flags boundary_flags = 
                    ui_frame_flag_interactable |
                    ui_frame_flag_floating |
                    ui_frame_flag_hover_cursor;
                
                ui_set_next_rect(boundary_rect);
                ui_set_next_hover_cursor(split_axis == ui_axis_x ? os_cursor_resize_EW : os_cursor_resize_NS);
                ui_key_t boundary_key = ui_key_from_stringf({ 0 }, "%p_panel_boundary", child);
                ui_frame_t* boundary_frame = ui_frame_from_key(boundary_flags, boundary_key);
                ui_frame_set_display_string(boundary_frame, str("panel_boundary")); // for debug
                ui_interaction boundary_interaction = ui_frame_interaction(boundary_frame);
                
                /*if (boundary_interaction & ui_interaction_hovered) {
                    rect_t vis_rect = boundary_rect;
                    vis_rect.v0[split_axis] += 7.0f;
                    vis_rect.v1[split_axis] -= 8.0f;
                    
                    if (boundary_interaction & ui_interaction_left_dragging) {
                        vis_rect.v0[split_axis] += context->mouse_delta[split_axis];
                        vis_rect.v1[split_axis] += context->mouse_delta[split_axis];
                    }
                    
                    ui_set_next_rect(vis_rect);
                    ui_frame_t* vis_frame = ui_frame_from_key(ui_frame_flag_draw_background, { 0 });
                }*/
                
                if (boundary_interaction & ui_interaction_left_double_clicked) {
                    ui_kill_action();
                    f32 sum_percent = min_child->percent_of_parent + max_child->percent_of_parent;
                    min_child->percent_of_parent = 0.5f * sum_percent ;
                    max_child->percent_of_parent = 0.5f * sum_percent ;
                    
                } else if (boundary_interaction & ui_interaction_left_pressed) {
                    vec2_t drag_data = vec2(min_child->percent_of_parent, max_child->percent_of_parent);
                    ui_drag_store_data(&drag_data, sizeof(vec2_t));
                } else if (boundary_interaction & ui_interaction_left_dragging) {
                    
                    // get drag data
                    vec2_t* drag_data = (vec2_t*)ui_drag_get_data();
                    vec2_t mouse_delta = ui_drag_delta();
                    
                    vec2_t panel_size = rect_size(panel_rect);
                    f32 total_size = panel_size[split_axis];
                    
                    // min child
                    f32 min_pct_before = drag_data->x;
                    f32 min_px_before = min_pct_before * total_size;
                    f32 min_px_after = max(min_px_before + mouse_delta[split_axis], 50.0f);
                    f32 min_pct_after = min_px_after / total_size;
                    f32 pct_delta = min_pct_after - min_pct_before;
                    
                    // max child
                    f32 max_pct_before = drag_data->y;
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
        
    }
    
    // create leaf panel ui
    {
        
        for (ui_panel_t* panel = context->panel_root; panel != nullptr; panel = ui_panel_rec_depth_first(panel).next) {
            
            // skip if not leaf panel
            if (panel->tree_first != nullptr) { continue; }
            
            b8 panel_is_focused = (context->panel_focused == panel);
            
            // calculate rect
            rect_t panel_rect = rect_shrink(ui_rect_from_panel(panel, content_rect), 2.0f);
            vec2_t panel_center = rect_center(panel_rect);
            vec2_t panel_size = rect_size(panel_rect);
            
            rect_t container_rect = panel_rect; container_rect.y0 += 23.0f;
            rect_t tab_bar_rect = panel_rect; tab_bar_rect.y1 = tab_bar_rect.y0 + 25.0f;
            
            // build drop sites
            if (ui_drag_is_active() && context->view_drag != nullptr && rect_contains(panel_rect, context->mouse_pos)) {
                
                // calculate drop rects
                rect_t drop_rect_center = rect(vec2_sub(panel_center, drop_site_major_size), vec2_add(panel_center, drop_site_major_size));
                rect_t drop_rect_up = rect_translate(drop_rect_center, vec2(0.0f, -(drop_site_major_size * 2.25f)));
                rect_t drop_rect_down = rect_translate(drop_rect_center, vec2(0.0f, +(drop_site_major_size * 2.25f)));
                rect_t drop_rect_left =  rect_translate(drop_rect_center, vec2(-(drop_site_major_size * 2.25f), 0.0f));
                rect_t drop_rect_right = rect_translate(drop_rect_center, vec2(+(drop_site_major_size * 2.25f), 0.0f));
                
                struct ui_drop_site_t {
                    ui_key_t key;
                    ui_dir split_dir;
                    rect_t rect;
                    ui_frame_t* frame;
                };
                
                ui_drop_site_t drop_sites[] = {
                    { ui_key_from_stringf( { 0 }, "drop_site_center_%p", panel), ui_dir_none, drop_rect_center },
                    { ui_key_from_stringf( { 0 }, "drop_site_up_%p", panel), ui_dir_up, drop_rect_up},
                    { ui_key_from_stringf( { 0 }, "drop_site_down_%p", panel), ui_dir_down, drop_rect_down},
                    { ui_key_from_stringf( { 0 }, "drop_site_left_%p", panel), ui_dir_left, drop_rect_left},
                    { ui_key_from_stringf( { 0 }, "drop_site_right_%p", panel), ui_dir_right, drop_rect_right},
                };
                
                for (u32 i = 0; i < array_count(drop_sites); i++) {
                    
                    // get info
                    ui_key_t drop_key = drop_sites[i].key;
                    ui_dir drop_dir = drop_sites[i].split_dir;
                    rect_t drop_rect = drop_sites[i].rect;
                    ui_axis split_axis = ui_axis_from_dir(drop_dir);
                    ui_side split_side = ui_side_from_dir(drop_dir);
                    
                    // skip if not in same axis as split axis
                    if (drop_dir != ui_dir_none && split_axis == panel->tree_parent->split_axis) {
                        continue;
                    }
                    
                    // build drop site frame
                    ui_set_next_rect(drop_rect);
                    ui_set_next_color_background(drop_background_color);
                    ui_set_next_color_border(drop_border_color);
                    ui_set_next_rounding(vec4(5.0f));
                    drop_sites[i].frame = ui_frame_from_key(drop_site_flags, drop_key);
                    ui_frame_interaction(drop_sites[i].frame);
                    
                    // set draw data
                    ui_drop_site_draw_data_t* data = (ui_drop_site_draw_data_t*)arena_alloc(ui_build_arena(), sizeof(ui_drop_site_draw_data_t));
                    ui_frame_set_custom_draw(drop_sites[i].frame, ui_drop_site_draw_function, data);
                    
                    if (drop_dir == ui_dir_none) {
                        data->type = ui_drop_site_type_center;
                    } else {
                        data->type = ui_drop_site_type_edge;
                        data->axis = split_axis;
                        data->side = split_side;
                    }
                    
                    
                }
                
                for (i32 i = 0; i < array_count(drop_sites); i++) {
                    
                    // get drop site info
                    ui_key_t drop_key = drop_sites[i].key;
                    rect_t drop_rect = drop_sites[i].rect;
                    ui_frame_t* drop_frame = drop_sites[i].frame;
                    ui_dir drop_dir = drop_sites[i].split_dir;
                    ui_axis split_axis = ui_axis_from_dir(drop_dir);
                    ui_side split_side = ui_side_from_dir(drop_dir);
                    
                    // visualize new panel
                    if (ui_key_equals(drop_key, context->key_hovered)) {
                        
                        // calculate rect based on which drop site
                        rect_t new_panel_rect = drop_rect;
                        rect_t padded_panel_rect = rect_shrink(panel_rect, 5.0f);
                        vec2_t padded_panel_size = rect_size(padded_panel_rect);
                        
                        if (drop_dir == ui_dir_none) {
                            new_panel_rect = rect_lerp(rect_shrink(padded_panel_rect, vec2_mul(padded_panel_size, 0.1f)), padded_panel_rect, drop_frame->hover_t);
                        } else if (split_side == ui_side_min) {
                            f32 padding = (padded_panel_size[split_axis] * 0.1f);
                            new_panel_rect = padded_panel_rect;
                            new_panel_rect.v0[split_axis] = lerp(new_panel_rect.v0[split_axis] + padding, new_panel_rect.v0[split_axis], drop_frame->hover_t);
                            new_panel_rect.v1[split_axis] = padded_panel_rect.v0[split_axis] + (padded_panel_size[split_axis] * 0.5f);
                        } else {
                            f32 padding = (padded_panel_size[split_axis] * 0.1f);
                            new_panel_rect = padded_panel_rect;
                            new_panel_rect.v0[split_axis] = padded_panel_rect.v0[split_axis] + (padded_panel_size[split_axis] * 0.5f);
                            new_panel_rect.v1[split_axis] = lerp(new_panel_rect.v1[split_axis] - padding, new_panel_rect.v1[split_axis], drop_frame->hover_t);
                        }
                        
                        // build visualization frame
                        color_t vis_color = color_lerp(color(0x00000000), vis_background_color, drop_frame->hover_t);
                        
                        ui_set_next_rect(new_panel_rect);
                        ui_set_next_color_background(vis_color);
                        ui_set_next_color_border(vis_border_color);
                        ui_set_next_rounding(vec4(5.0f));
                        ui_frame_from_key(vis_panel_flags, { 0 });
                        
                        // perform drop
                        if (ui_key_equals(drop_key, context->key_hovered) && ui_drag_drop()) {
                            
                            if (drop_dir != ui_dir_none) {
                                // push command
                                ui_cmd_t* cmd = ui_cmd_push(context, ui_cmd_type_split_panel);
                                cmd->src_panel = context->view_drag->parent_panel;
                                cmd->dst_panel = panel;
                                cmd->dir = drop_dir;
                                cmd->view = context->view_drag;
                            } else {
                                
                                ui_cmd_t* cmd = ui_cmd_push(context, ui_cmd_type_move_view);
                                cmd->src_panel = context->view_drag->parent_panel;
                                cmd->dst_panel = panel;
                                cmd->view = context->view_drag;
                                
                            }
                            
                        }
                        
                    }
                    
                }
                
            }
            
            
            // build container frame
            {
                
                ui_frame_flags panel_flags = 
                    ui_frame_flag_floating |
                    ui_frame_flag_draw_background |
                    ui_frame_flag_draw_border |
                    ui_frame_flag_draw_clip;
                
                // focused border
                color_t border_color = color(0x323232ff);
                if (panel_is_focused) {
                    border_color = context->color_accent_default_node.v;
                    border_color.a = 0.25f;
                }
                
                ui_set_next_rect(container_rect);
                ui_set_next_color_border(border_color);
                ui_set_next_color_background(color(0x242424ff));
                
                ui_key_t panel_key = ui_key_from_stringf({ 0 }, "%p_panel_frame", panel);
                panel->frame = ui_frame_from_key(panel_flags, panel_key);
                ui_frame_set_display_string(panel->frame, str("panel_frame")); // debug
                
                // build panel contents
                ui_push_parent(panel->frame);
                
                // call view function
                if (panel->view_focus != nullptr) {
                    panel->view_focus->view_func();
                }
                
                // empty view panel
                if (panel->view_first == nullptr) {
                    
                    //ui_cmd_t* cmd = ui_cmd_push(context, ui_cmd_type_close_panel);
                    //cmd->panel = panel;
                    
                    ui_spacer(ui_size_percent(1.0f));
                    ui_set_next_size(ui_size_by_children(1.0f), ui_size_by_children(1.0f));
                    ui_row_begin();
                    ui_spacer(ui_size_percent(1.0f));
                    
                    ui_set_next_text_alignment(ui_text_alignment_center);
                    ui_set_next_size(ui_size_pixels(120.0f, 1.0f), ui_size_pixels(20.0f, 1.0f));
                    ui_set_next_color_background(color(0x502018ff));
                    ui_interaction close_button_interaction = ui_buttonf("Close Panel###%p_close_button", panel);
                    
                    ui_spacer(ui_size_percent(1.0f));
                    ui_row_end();
                    ui_spacer(ui_size_percent(1.0f));
                    
                    if (close_button_interaction & ui_interaction_left_clicked) {
                        ui_cmd_t* cmd = ui_cmd_push(context, ui_cmd_type_close_panel);
                        cmd->src_panel = panel;
                    }
                    
                }
                ui_pop_parent();
                
            }
            
            // build tab bar
            {
                ui_frame_flags tab_bar_flags =
                    ui_frame_flag_floating |
                    //ui_frame_flag_draw_background |
                    ui_frame_flag_draw_clip;
                
                ui_set_next_rect(tab_bar_rect);
                ui_set_next_layout_dir(ui_dir_right);
                
                ui_key_t tab_bar_key = ui_key_from_stringf({ 0 }, "%p_tab_bar_frame", panel);
                ui_frame_t* tab_bar_frame = ui_frame_from_key(tab_bar_flags, tab_bar_key);
                ui_frame_set_display_string(tab_bar_frame, str("tab_bar_frame")); // debug
                
                // build tab bar contents
                ui_push_parent(tab_bar_frame);
                
                ui_push_size(ui_size_pixels(120.0f, 0.5f), ui_size_percent(1.0f));
                ui_push_rounding(vec4(0.0f, 4.0f, 0.0f, 4.0f));
                
                for (ui_view_t* view = panel->view_first; view != nullptr; view = view->next) {
                    
                    color_t view_background_color = color(0x121212ff);
                    color_t view_border_color = color(0x242424ff);
                    
                    if (view == context->view_focus) {
                        view_background_color = context->color_accent_default_node.v;
                        view_background_color.a = 0.25f;
                        view_border_color = context->color_accent_default_node.v;
                        view_border_color.a = 0.25f;
                    } else if (view == panel->view_focus) {
                        view_background_color = color(0x242424ff);
                        view_border_color = color(0x323232ff);
                    }
                    
                    
                    ui_spacer(ui_size_pixels(4.0f));
                    
                    ui_set_next_color_background(view_background_color);
                    ui_set_next_color_border(view_border_color);
                    ui_set_next_flags(ui_frame_flag_draggable | ui_frame_flag_draw_border);
                    ui_interaction tab_interaction = ui_buttonf("%.*s###%p_tab", view->label.size, view->label.data, view);
                    
                    if (tab_interaction & ui_interaction_left_clicked) {
                        ui_cmd_t* cmd = ui_cmd_push(context, ui_cmd_type_focus_panel);
                        cmd->src_panel = panel;
                        cmd->view = view;
                    }
                    
                    vec2_t mouse_pos = context->mouse_pos;
                    b8 drag_check = !rect_contains(tab_bar_rect, mouse_pos);
                    
                    if ((tab_interaction & ui_interaction_left_dragging) && !ui_drag_is_active() && drag_check) {
                        ui_drag_begin();
                        context->view_drag = view;
                    }
                    
                }
                
                
                ui_pop_rounding();
                ui_pop_size();
                
                ui_pop_parent();
            }
            
        }
        
        
    }
    
    
    
    
}

function void
ui_end(ui_context_t* context) {
    
    ui_pop_parent();
    
    // remove inactive frames
    for (ui_frame_t* frame = context->frame_first, *next = nullptr; frame != nullptr; frame = next) {
        next = frame->list_next;
        if (frame->last_build_index < context->build_index || ui_key_equals(frame->key, { 0 })) {
            dll_remove_np(context->frame_first, context->frame_last, frame, list_next, list_prev);
            stack_push_n(context->frame_free, frame, list_next);
        }
    }
    
    // close context
    if (!context->popup_updated_this_frame) {
        ui_popup_close();
    }
    
    // popup interaction
    if(context->popup_is_open) {
        ui_frame_interaction(context->frame_popup);
    }
    
    // end drag dropping if neccessary
    if (context->drag_state == ui_drag_state_dropping) {
        context->drag_state = ui_drag_state_none;
        context->view_drag = nullptr;
    }
    
    // unused events
    for (ui_event_t* event = ui_state.event_first; event != nullptr; event = event->next) {
        if (event->type == ui_event_type_mouse_release) {
            context->key_focused = { 0 };
        }
        
        if (event->type == ui_event_type_mouse_press) {
            ui_popup_close();
        }
    }
    
    // layout pass
    {
        for (ui_axis axis = ui_axis_x; axis < ui_axis_count; axis++) {
            ui_layout_solve_independent(context->frame_root, axis);
            ui_layout_solve_upward_dependent(context->frame_root, axis);
            ui_layout_solve_downward_dependent(context->frame_root, axis);
            ui_layout_solve_violations(context->frame_root, axis);
            ui_layout_solve_set_positions(context->frame_root, axis);
        }
    }
    
    // animate
    {
        
        // animate cache
        for (ui_anim_node_t* n = context->anim_node_first; n != nullptr; n = n->list_next) {
            f32 delta = n->params.target - n->current;
            n->current += delta * n->params.rate;
        }
        
        // animate frames
        for (ui_frame_t* frame = context->frame_first; frame != nullptr; frame = frame->list_next) {
            
            b8 is_hovered = ui_key_is_hovered(frame->key);
            b8 is_active = ui_key_equals(context->key_active[os_mouse_button_left], frame->key);
            frame->hover_t += context->anim_fast_rate * ((f32)is_hovered - frame->hover_t);
            frame->active_t += context->anim_fast_rate * ((f32)is_active - frame->active_t);
            
        }
        
    }
    
    // hover cursor
    { 
        
        ui_frame_t* hovered_frame = ui_frame_find(context->key_hovered);
        ui_frame_t* active_frame = ui_frame_find(context->key_active[os_mouse_button_left]);
        ui_frame_t* frame = active_frame == nullptr ? hovered_frame : active_frame; 
        
        if (frame != nullptr) {
            os_cursor cursor = frame->hover_cursor;
            if (frame->flags & ui_frame_flag_hover_cursor) {
                os_set_cursor(cursor);
            }
            
        }
        
    }
    
    
    // draw
    {
        for (ui_frame_t* frame = context->frame_root; frame != nullptr;) {
            ui_frame_rec_t rec = ui_frame_rec_depth_first(frame);
            
            ui_palette_t* palette = &frame->palette;
            
            // draw shadow
            if (frame->flags & ui_frame_flag_draw_shadow) {
                draw_set_next_color(palette->shadow);
                draw_set_next_rounding(frame->rounding);
                draw_set_next_softness(frame->shadow_size);
                draw_rect(rect_translate(rect_grow(frame->rect, frame->shadow_size), 1.0f));
            }
            
            
            // draw background
            if (frame->flags & ui_frame_flag_draw_background) {
                
                color_t background_color = palette->background;
                
                // draw hover effects
                if (frame->flags & ui_frame_flag_draw_hover_effects) {
                    background_color = color_lerp(background_color, color_blend(background_color, palette->hover), frame->hover_t);
                }
                
                // draw active effects
                if (frame->flags & ui_frame_flag_draw_active_effects) {
                    background_color = color_lerp(background_color, color_blend(background_color, palette->active), frame->active_t);
                }
                
                draw_set_next_color(background_color);
                draw_set_next_rounding(frame->rounding);
                draw_rect(frame->rect);
            }
            
            
            // draw border
            if (frame->flags & ui_frame_flag_draw_border) {
                draw_set_next_color(palette->border);
                draw_set_next_rounding(frame->rounding);
                draw_set_next_thickness(frame->border_size);
                draw_rect(frame->rect);
            }
            
            // clipping
            if (frame->flags & ui_frame_flag_draw_clip) {
                rect_t top_clip = draw_top_clip_mask();
                rect_t new_clip = frame->rect;
                if (top_clip.x1 != 0.0f || top_clip.y1 != 0.0f) {
                    new_clip = rect_intersection(new_clip, top_clip);
                }
                rect_validate(new_clip);
                draw_push_clip_mask(new_clip);
            }
            
            // draw text
            if (frame->flags & ui_frame_flag_draw_text) {
                
                vec2_t text_pos = ui_text_align(frame->font, frame->font_size, frame->label, frame->rect, frame->text_alignment);
                
                draw_push_font(frame->font);
                draw_push_font_size(frame->font_size);
                
                // text shadow
                color_t text_shadow = palette->shadow; text_shadow.a = 0.8f;
                draw_set_next_color(text_shadow);
                draw_text(frame->label, vec2_add(text_pos, 1.0f));
                
                // text
                draw_set_next_color(palette->text);
                draw_text(frame->label, text_pos);
                
                draw_pop_font_size();
                draw_pop_font();
                
            }
            
            
            // custom draw
            if (frame->flags & ui_frame_flag_draw_custom) {
                if (frame->custom_draw_func != nullptr) {
                    frame->custom_draw_func(frame);
                }
            }
            
            // debug drawing
            {
                //draw_set_next_thickness(1.0f);
                //draw_set_next_color(color(0xffff0015));
                //draw_rect(frame->rect);
            }
            
            
            // pop clipping
            
            i32 pop_index = 0;
            for (ui_frame_t* f = frame; f != nullptr && pop_index <= rec.pop_count; f = f->tree_parent) {
                pop_index++;
                
                if (f == frame && rec.push_count != 0) {
                    continue;
                }
                
                if (f->flags & ui_frame_flag_draw_clip) {
                    draw_pop_clip_mask();
                }
                
            }
            
            frame = rec.next;
        }
        
        
    }
    
    // reset context
    context->build_index++;
    arena_clear(ui_build_arena());
    
    // reset active context
    ui_state.context_active = nullptr;
}


function ui_context_t*
ui_active() {
    
	if (ui_state.context_active == nullptr) {
		printf("[error] [ui] no active context is set!\n");
	}
    
	return ui_state.context_active;
}

function arena_t*
ui_build_arena() {
    ui_context_t* context = ui_active();
    return context->build_arena[context->build_index % 2];
}

function ui_frame_t*
ui_last_frame() {
    ui_context_t* context = ui_active();
    return context->frame_mrc;
}

//- context

function ui_context_t*
ui_context_create(os_handle_t window, gfx_handle_t renderer) {
    
	// grab from free list or allocate one
	ui_context_t* context = ui_state.context_free;
	if (context != nullptr) {
		stack_pop(ui_state.context_free);
	} else {
		context = (ui_context_t*)arena_alloc(ui_state.context_arena, sizeof(ui_context_t));
	}
	memset(context, 0, sizeof(ui_context_t));
	dll_push_back(ui_state.context_first, ui_state.context_last, context);
    
	// allocate arenas
	context->arena = arena_create(megabytes(256));
	context->build_arena[0] = arena_create(megabytes(256));
	context->build_arena[1] = arena_create(megabytes(256));
	context->drag_state_arena = arena_create(kilobytes(4));
	context->window = window;
	context->renderer = renderer;
    context->build_index = 0;
    context->drag_state = ui_drag_state_none;
    
	// initialize stack
	context->parent_default_node.v = nullptr;
	context->flags_default_node.v = 0;
	context->seed_key_default_node.v = { 0 };
	context->fixed_x_default_node.v = 0.0f;
	context->fixed_y_default_node.v = 0.0f;
	context->fixed_width_default_node.v = 0.0f;
	context->fixed_height_default_node.v = 0.0f;
	context->width_default_node.v = {ui_size_type_none, 0.0f, 0.0f};
	context->height_default_node.v =  {ui_size_type_none, 0.0f, 0.0f};
	context->padding_default_node.v = 0.0f;
    context->layout_dir_default_node.v = ui_dir_down;
    context->text_alignment_default_node.v = ui_text_alignment_left;
	context->rounding_00_default_node.v = 2.0f;
	context->rounding_01_default_node.v = 2.0f;
	context->rounding_10_default_node.v = 2.0f;
	context->rounding_11_default_node.v = 2.0f;
	context->border_size_default_node.v = 1.0f;
	context->shadow_size_default_node.v = 1.0f;
	context->texture_default_node.v = { 0 };
	context->font_default_node.v = draw_state.font;
	context->font_size_default_node.v = 9.0f;
    context->color_background_default_node.v = color(0x323232ff);
    context->color_text_default_node.v = color(0xe2e2e2ff);
    context->color_border_default_node.v = color(0x00000000);
    context->color_shadow_default_node.v = color(0x00000035);
    context->color_hover_default_node.v = color(0xffffff18);
    context->color_active_default_node.v = color(0xffffff18);
    context->color_accent_default_node.v = color(0xf5b500ff);
    context->hover_cursor_default_node.v = os_cursor_pointer;
    
    // create root panel
    context->panel_root = ui_panel_create(context, 1.0f, ui_axis_x);
    context->panel_focused = context->panel_root;
    
	return context;
}

function void
ui_context_release(ui_context_t* context) {
    
	// release memory
	arena_release(context->arena);
	arena_release(context->build_arena[0]);
	arena_release(context->build_arena[1]);
	arena_release(context->drag_state_arena);
    
	// remove from list and push to free list
	dll_remove(ui_state.context_first, ui_state.context_last, context);
	stack_push(ui_state.context_free, context);
}

function void
ui_context_reset_stacks(ui_context_t* context) {
    
    context->parent_stack.top = &context->parent_default_node;
    context->parent_stack.free = nullptr;
    context->parent_stack.auto_pop = false;
    
    context->flags_stack.top = &context->flags_default_node;
    context->flags_stack.free = nullptr;
    context->flags_stack.auto_pop = false;
    
    context->seed_key_stack.top = &context->seed_key_default_node;
    context->seed_key_stack.free = nullptr;
    context->seed_key_stack.auto_pop = false;
    
    context->fixed_x_stack.top = &context->fixed_x_default_node;
    context->fixed_x_stack.free = nullptr;
    context->fixed_x_stack.auto_pop = false;
    
    context->fixed_y_stack.top = &context->fixed_y_default_node;
    context->fixed_y_stack.free = nullptr;
    context->fixed_y_stack.auto_pop = false;
    
    context->fixed_width_stack.top = &context->fixed_width_default_node;
    context->fixed_width_stack.free = nullptr;
    context->fixed_width_stack.auto_pop = false;
    
    context->fixed_height_stack.top = &context->fixed_height_default_node;
    context->fixed_height_stack.free = nullptr;
    context->fixed_height_stack.auto_pop = false;
    
    context->width_stack.top = &context->width_default_node;
    context->width_stack.free = nullptr;
    context->width_stack.auto_pop = false;
    
    context->height_stack.top = &context->height_default_node;
    context->height_stack.free = nullptr;
    context->height_stack.auto_pop = false;
    
    context->padding_stack.top = &context->padding_default_node;
    context->padding_stack.free = nullptr;
    context->padding_stack.auto_pop = false;
    
    context->layout_dir_stack.top = &context->layout_dir_default_node;
    context->layout_dir_stack.free = nullptr;
    context->layout_dir_stack.auto_pop = false;
    
    context->text_alignment_stack.top = &context->text_alignment_default_node;
    context->text_alignment_stack.free = nullptr;
    context->text_alignment_stack.auto_pop = false;
    
    context->rounding_00_stack.top = &context->rounding_00_default_node;
    context->rounding_00_stack.free = nullptr;
    context->rounding_00_stack.auto_pop = false;
    
    context->rounding_01_stack.top = &context->rounding_01_default_node;
    context->rounding_01_stack.free = nullptr;
    context->rounding_01_stack.auto_pop = false;
    
    context->rounding_10_stack.top = &context->rounding_10_default_node;
    context->rounding_10_stack.free = nullptr;
    context->rounding_10_stack.auto_pop = false;
    
    context->rounding_11_stack.top = &context->rounding_11_default_node;
    context->rounding_11_stack.free = nullptr;
    context->rounding_11_stack.auto_pop = false;
    
    context->border_size_stack.top = &context->border_size_default_node;
    context->border_size_stack.free = nullptr;
    context->border_size_stack.auto_pop = false;
    
    context->shadow_size_stack.top = &context->shadow_size_default_node;
    context->shadow_size_stack.free = nullptr;
    context->shadow_size_stack.auto_pop = false;
    
    context->texture_stack.top = &context->texture_default_node;
    context->texture_stack.free = nullptr;
    context->texture_stack.auto_pop = false;
    
    context->font_stack.top = &context->font_default_node;
    context->font_stack.free = nullptr;
    context->font_stack.auto_pop = false;
    
    context->font_size_stack.top = &context->font_size_default_node;
    context->font_size_stack.free = nullptr;
    context->font_size_stack.auto_pop = false;
    
    context->color_background_stack.top = &context->color_background_default_node;
    context->color_background_stack.free = nullptr;
    context->color_background_stack.auto_pop = false;
    
    context->color_text_stack.top = &context->color_text_default_node;
    context->color_text_stack.free = nullptr;
    context->color_text_stack.auto_pop = false;
    
    context->color_border_stack.top = &context->color_border_default_node;
    context->color_border_stack.free = nullptr;
    context->color_border_stack.auto_pop = false;
    
    context->color_shadow_stack.top = &context->color_shadow_default_node;
    context->color_shadow_stack.free = nullptr;
    context->color_shadow_stack.auto_pop = false;
    
    context->color_hover_stack.top = &context->color_hover_default_node;
    context->color_hover_stack.free = nullptr;
    context->color_hover_stack.auto_pop = false;
    
    context->color_active_stack.top = &context->color_active_default_node;
    context->color_active_stack.free = nullptr;
    context->color_active_stack.auto_pop = false;
    
    context->color_accent_stack.top = &context->color_accent_default_node;
    context->color_accent_stack.free = nullptr;
    context->color_accent_stack.auto_pop = false;
    
    context->hover_cursor_stack.top = &context->hover_cursor_default_node;
    context->hover_cursor_stack.free = nullptr;
    context->hover_cursor_stack.auto_pop = false;
    
}

//- string

function u64
ui_string_hash(u64 seed, str_t string) {
    u64 result = seed;
    for (u64 i = 0; i < string.size; i += 1) {
        result = ((result << 5) + result) + string.data[i];
    }
    return result;
}

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

//- key

function ui_key_t
ui_key_from_string(ui_key_t seed, str_t string) {
    ui_key_t key = { 0 };
    if (string.size != 0) {
        str_t hash_string = ui_string_hash_format(string);
        key.data[0] = ui_string_hash(seed.data[0], hash_string);
    }
    return key;
}

function ui_key_t
ui_key_from_stringf(ui_key_t seed, char* fmt, ...) {
    temp_t scratch = scratch_begin();
    
    va_list args;
    va_start(args, fmt);
    str_t string = str_formatv(scratch.arena, fmt, args);
    va_end(args);
    
    ui_key_t result = ui_key_from_string(seed, string);
    
    scratch_end(scratch);
    return result;
}

function b8
ui_key_equals(ui_key_t a, ui_key_t b) {
    return (a.data[0] == b.data[0]);
}

function b8
ui_key_is_hovered(ui_key_t key) {
    ui_key_t hovered_key = ui_active()->key_hovered;
    return ((!ui_key_equals(hovered_key, { 0 })) && (ui_key_equals(key, hovered_key)));
}

function b8
ui_key_is_active(ui_key_t key) {
    b8 result = false;
    for (i32 i = 0; i < os_mouse_button_count; i++) {
        
        ui_key_t active_key = ui_active()->key_active[i];
        
        if ((!ui_key_equals(active_key, { 0 })) && (ui_key_equals(key, active_key))) {
            result = true;
            break;
        }
    }
    return result;
}

function b8
ui_key_is_focused(ui_key_t key) {
    ui_key_t focused_key = ui_active()->key_focused;
    return ((!ui_key_equals(focused_key, { 0 })) && (ui_key_equals(key, focused_key)));
}

//- size

function ui_size_t
ui_size(ui_size_type type, f32 value, f32 strictness) {
    return { type, value, strictness };
}

function ui_size_t
ui_size_pixels(f32 pixels, f32 strictness) {
    return { ui_size_type_pixel, pixels, strictness };
}

function ui_size_t
ui_size_percent(f32 percent) {
    return { ui_size_type_percent, percent, 0.0f };
}

function ui_size_t
ui_size_by_children(f32 strictness) {
    return { ui_size_type_by_children, 0.0f, strictness };
}

function ui_size_t
ui_size_text(f32 padding) {
    return { ui_size_type_text, padding, 0.0f };
}

//- axis

function ui_axis 
ui_axis_from_dir(ui_dir dir) {
    // ui_dir_left -> ui_axis_x;
    // ui_dir_up -> ui_axis_y;
    // ui_dir_right -> ui_axis_x;
    // ui_dir_down -> ui_axis_y;
    return ((dir & 1) ? ui_axis_y : ui_axis_x);
}

//- side
function ui_side
ui_side_from_dir(ui_dir dir) {
    // ui_dir_left -> ui_side_min;
    // ui_dir_up -> ui_side_min;
    // ui_dir_right -> ui_side_max;
    // ui_dir_down -> ui_side_max;
    return ((dir < 2) ? ui_side_min : ui_side_max);
}

//- dir 

function ui_dir
ui_dir_from_axis_side(ui_axis axis, ui_side side) {
    ui_dir result = (axis == ui_axis_x) ? 
    (side == ui_side_min) ? ui_dir_left : ui_dir_right :
    (side == ui_side_min) ? ui_dir_up : ui_dir_down;
    return result;
}


//- text point

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

//- text range

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


//- text alignment

function vec2_t
ui_text_align(font_handle_t font, f32 size, str_t text, rect_t rect, ui_text_alignment alignment) {
    
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
            f32 text_width = font_text_get_width(font, size, text);
            result.x = roundf((rect.x0 + rect.x1 - text_width) * 0.5f);
            result.x = max(result.x, rect.x0 + 4.0f);
            break;
        }
        
        case ui_text_alignment_right: {
            f32 text_width = font_text_get_width(font, size, text);
            result.x = roundf(rect.x1 - text_width - 4.0f);
            result.x = max(result.x, rect.x0 + 4.0f);
            break;
        }
        
    }
    result.x = floorf(result.x);
    return result;
    
}

function vec2_t
ui_text_size(font_handle_t font, f32 font_size, str_t text) {
    return vec2(font_text_get_width(font, font_size, text), font_text_get_height(font, font_size, text));
}

function f32
ui_text_offset_from_index(font_handle_t font, f32 font_size, str_t string, u32 index) {
    f32 width = 0.0f;
    for (u32 offset = 0; offset < index; offset++) {
        char c = *(string.data + offset);
        font_glyph_t* glyph = font_get_glyph(font, font_size, (u8)c);
        width += glyph->advance;
    }
    return width;
}

function u32 
ui_text_index_from_offset(font_handle_t font, f32 font_size, str_t string, f32 offset) {
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

//- commands 

function ui_cmd_t* 
ui_cmd_push(ui_context_t* context, ui_cmd_type type) {
    
    // grab from free list or allocate one
    ui_cmd_t* cmd = ui_state.command_free;
    if (cmd != nullptr) {
        stack_pop(ui_state.command_free);
    } else {
        cmd = (ui_cmd_t*)arena_alloc(ui_state.command_arena, sizeof(ui_cmd_t));
    }
    memset(cmd, 0, sizeof(ui_cmd_t));
    dll_push_back(ui_state.command_first, ui_state.command_last, cmd);
    
    // fill struct
    cmd->type = type;
    cmd->context = context;
    
    return cmd;
}

function void 
ui_cmd_pop(ui_cmd_t* command) {
    dll_remove(ui_state.command_first, ui_state.command_last, command);
	stack_push(ui_state.command_free, command);
}

//- events

function void 
ui_event_push(ui_event_t* event) {
    ui_event_t* new_event = (ui_event_t*)arena_calloc(ui_state.event_arena, sizeof(ui_event_t));
    memcpy(new_event, event, sizeof(ui_event_t));
    dll_push_back(ui_state.event_first, ui_state.event_last, new_event);
}

function void 
ui_event_pop(ui_event_t* event) {
    if (event->os_event != nullptr) {
        os_event_pop(event->os_event); // take os event if ui event was taken.
    }
    dll_remove(ui_state.event_first, ui_state.event_last, event);
}

function void
ui_kill_action() {
    ui_context_t* context = ui_active();
    for (i32 i = 0; i < 3; i++) {
        context->key_active[i] = { 0 };
    }
}

function ui_event_binding_t*
ui_event_get_binding(os_key key, os_modifiers modifiers) {
    
    ui_event_binding_t* binding = nullptr;
    
    for (i32 i = 0; i < 64; i++) {
        ui_event_binding_t* b = &ui_state.event_bindings[i];
        
        if (key == b->key && modifiers == b->modifiers) {
            binding = b;
        }
    }
    
    return binding;
}

function ui_text_op_t 
ui_event_to_text_op(ui_event_t* event, str_t string, ui_text_point_t cursor, ui_text_point_t mark) {
    
    ui_text_op_t text_op = { 0 };
    text_op.cursor = cursor;
    text_op.mark = mark;
    
    ivec2_t delta = event->delta;
    switch (event->delta_unit) {
        
        case ui_event_delta_unit_char: {
            break;
        }
        
        case ui_event_delta_unit_word: {
            i32 dst_index = str_find_word_index(string, cursor.column, delta.x);
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
        text_op.cursor.column += delta.x;
    }
    
    // deletion
    if (event->flags & ui_event_flag_delete) {
        ui_text_point_t new_pos = ui_text_point_min(text_op.cursor, text_op.mark);
        text_op.range = ui_text_range(text_op.cursor, text_op.mark);
        text_op.replace = str("");
        text_op.cursor = text_op.mark = new_pos;
    }
    
    // update mark
    if (!(event->flags & ui_event_flag_keep_mark)) {
        text_op.mark = text_op.cursor;
    }
    
    // insert
    if (event->character != 0) {
        text_op.range = ui_text_range(cursor, mark);
        text_op.replace = str_copy(ui_build_arena(), str((char*)(&event->character), 1));
        text_op.cursor = text_op.mark = ui_text_point(text_op.range.min.line, text_op.range.min.column + 1);
    }
    
    if (text_op.cursor.column > string.size + 1 || 0 > text_op.cursor.column || event->delta.y != 0) {
        text_op.flags |= ui_text_op_flag_invalid;
    }
    text_op.cursor.column = clamp(text_op.cursor.column, 0, string.size + text_op.replace.size);
    text_op.mark.column = clamp(text_op.mark.column, 0, string.size + text_op.replace.size);
    
    return text_op;
}


//- drag state 

function void 
ui_drag_store_data(void* data, u32 size) {
    ui_context_t* context = ui_active();
    arena_clear(context->drag_state_arena);
    context->drag_state_data = arena_alloc(context->drag_state_arena, size);
    context->drag_state_size = size;
    context->drag_start_pos = context->mouse_pos;
    memcpy(context->drag_state_data, data, size);
}

function void* 
ui_drag_get_data() {
    return  ui_active()->drag_state_data;
}

function void 
ui_drag_clear_data() {
    ui_context_t* context = ui_active();
    arena_clear(context->drag_state_arena);
    context->drag_state_size = 0;
}

function vec2_t 
ui_drag_delta() {
    ui_context_t* context =  ui_active();
    return vec2_sub(context->mouse_pos, context->drag_start_pos);
}

function b8 
ui_drag_is_active() {
    ui_context_t* context = ui_active();
    return (context->drag_state == ui_drag_state_dragging || context->drag_state == ui_drag_state_dropping);
}

function void
ui_drag_begin() {
    ui_context_t* context = ui_active();
    if (!ui_drag_is_active()) {
        context->drag_state = ui_drag_state_dragging;
    }
}

function b8
ui_drag_drop() {
    ui_context_t* context = ui_active();
    b8 result = false;
    if (context->drag_state == ui_drag_state_dropping) {
        result = true;
        context->drag_state = ui_drag_state_none;
    }
    return result;
}

function void
ui_drag_kill() {
    ui_context_t* context = ui_active();
    context->drag_state = ui_drag_state_none;
}

//- animation 

function ui_anim_params_t 
ui_anim_params_create(f32 initial, f32 target, f32 rate) {
    return {initial, target, rate};
}

function f32 
ui_anim_ex(ui_key_t key, ui_anim_params_t params) {
    
    // get context
    ui_context_t* context = ui_active();
    
    // search for in list
    ui_anim_node_t* node = nullptr;
    if (!ui_key_equals(key, { 0 })) {
        for (ui_anim_node_t* n = context->anim_node_first; n != nullptr; n = n->list_next) {
            if (ui_key_equals(key, n->key)) {
                node = n;
                break;
            }
        }
    }
    
    // if we didn't find one, allocate it
    if (node == nullptr) {
        node = context->anim_node_free;
        if (node != nullptr) {
            stack_pop_n(context->anim_node_free, list_next);
        } else {
            node = (ui_anim_node_t*)arena_alloc(context->arena, sizeof(ui_anim_node_t));
        }
        memset(node, 0, sizeof(ui_anim_node_t));
        
        // fill struct
        node->first_build_index = context->build_index;
        node->key = key;
        node->params = params;
        node->current = params.initial;
        
        // add to list
        dll_push_back_np(context->anim_node_first, context->anim_node_last, node, list_next, list_prev);
        
    } else {
        // remove from lru list
        dll_remove_np(context->anim_node_lru, context->anim_node_mru, node, lru_next, lru_prev);
    }
    
    // update node
    node->last_build_index = context->build_index;
    dll_push_back_np(context->anim_node_lru, context->anim_node_mru, node, lru_next, lru_prev);
    node->params = params;
    
    if (node->params.rate == 1.0f) {
        node->current = node->params.target;
    }
    
    return node->current;
}

function f32 
ui_anim(ui_key_t key, f32 initial, f32 target) {
    ui_anim_params_t params = { 0 };
	params.initial = initial;
	params.target = target;
	params.rate = ui_active()->anim_fast_rate;
	return ui_anim_ex(key, params);
}

//- data 

function void*
ui_data_ex(ui_key_t key, ui_data_type type, void* initial) {
    
    // get context
    ui_context_t* context = ui_active();
    
    // search for in list
    ui_data_node_t* node = nullptr;
    if (!ui_key_equals(key, { 0 })) {
        for (ui_data_node_t* n = context->data_node_first; n != nullptr; n = n->list_next) {
            if (ui_key_equals(key, n->key)) {
                node = n;
                break;
            }
        }
    }
    
    // if we didn't find one, allocate it
    if (node == nullptr) {
        node = context->data_node_free;
        if (node != nullptr) {
            stack_pop_n(context->data_node_free, list_next);
        } else {
            node = (ui_data_node_t*)arena_alloc(context->arena, sizeof(ui_data_node_t));
        }
        memset(node, 0, sizeof(ui_data_node_t));
        
        // fill struct
        node->first_build_index = context->build_index;
        node->key = key;
        
        // copy initial data
        switch (type) {
            case ui_data_type_b8: { node->b8_value = *(b8*)initial; break; }
            case ui_data_type_u8: { node->u8_value = *(u8*)initial; break;}
            case ui_data_type_u16: { node->u16_value = *(u16*)initial; break;}
            case ui_data_type_u32: { node->u32_value = *(u32*)initial; break;}
            case ui_data_type_u64: { node->u64_value = *(u64*)initial; break;}
            case ui_data_type_i8: { node->i8_value = *(i8*)initial; break;}
            case ui_data_type_i16: { node->i16_value = *(i16*)initial; break; }
            case ui_data_type_i32: { node->i32_value = *(i32*)initial; break;}
            case ui_data_type_i64: { node->i64_value = *(i64*)initial; break; }
            case ui_data_type_f32: { node->f32_value = *(f32*)initial; break; }
            case ui_data_type_f64: { node->f64_value = *(f64*)initial; break;}
        }
        
        // add to list
        dll_push_back_np(context->data_node_first, context->data_node_last, node, list_next, list_prev);
        
    } else {
        // remove from lru list
        dll_remove_np(context->data_node_lru, context->data_node_mru, node, lru_next, lru_prev);
    }
    
    // update node
    node->last_build_index = context->build_index;
    dll_push_back_np(context->data_node_lru, context->data_node_mru, node, lru_next, lru_prev);
    
    return &(node->b8_value);
}

function b8* 
ui_data(ui_key_t key, b8 initial) {
    void* data = ui_data_ex(key, ui_data_type_b8, &initial);
    return (b8*)data;
}

function i32*
ui_data(ui_key_t key, i32 initial) {
    void* data = ui_data_ex(key, ui_data_type_i32, &initial);
    return (i32*)data;
}

function f32*
ui_data(ui_key_t key, f32 initial) {
    void* data = ui_data_ex(key, ui_data_type_f32, &initial);
    return (f32*)data;
}


//- frame

function ui_frame_t* 
ui_frame_find(ui_key_t key) {
    ui_context_t* context = ui_active();
    ui_frame_t* result = nullptr;
    for (ui_frame_t* frame = context->frame_first; frame != nullptr; frame = frame->list_next) {
        if (ui_key_equals(frame->key, key)) {
            result = frame;;
            break;
        }
    }
    return result;
}

function ui_frame_t*
ui_frame_from_key(ui_frame_flags flags, ui_key_t key) {
    
    ui_context_t* context = ui_active();
    ui_frame_t* frame = nullptr;
    
    // try to find existing frame
    b8 frame_is_transient = ui_key_equals(key, { 0 });
    if (!frame_is_transient) {
        frame = ui_frame_find(key);
    }
    
    b8 frame_is_new = (frame == nullptr);
    
    // filter out duplicates
    if (!frame_is_new && (frame->last_build_index == context->build_index)) {
        frame = nullptr;
        key = { 0 };
        frame_is_new = true;
    }
    
    // try to grab from pool or allocate one
    if (frame_is_new) {
        frame = !frame_is_transient ? context->frame_free : nullptr;
        if (frame != nullptr) {
            stack_pop_n(context->frame_free, list_next);
        } else {
            frame = (ui_frame_t*)arena_alloc(!frame_is_transient ? context->arena : ui_build_arena(), sizeof(ui_frame_t));
        }
        memset(frame, 0, sizeof(ui_frame_t));
        frame->first_build_index = context->build_index;
    }
    
    // clear tree
    frame->tree_next = nullptr;
    frame->tree_prev = nullptr;
    frame->tree_first = nullptr;
    frame->tree_last = nullptr;
    frame->tree_parent = nullptr;
    
    // add to frame list if needed
    if (frame_is_new && !frame_is_transient) {
        dll_push_back_np(context->frame_first, context->frame_last, frame, list_next, list_prev);
    }
    
    // add to tree
    ui_frame_t* parent = ui_top_parent();
    if (parent != nullptr) {
        dll_push_back_np(parent->tree_first, parent->tree_last, frame, tree_next, tree_prev);
        frame->tree_parent = parent;
    }
    
    // fill frame members
    frame->key = key;
    frame->flags = flags | ui_top_flags();
    frame->pos_target.x = ui_top_fixed_x();
    frame->pos_target.y = ui_top_fixed_y();
    frame->size_target.x = ui_top_fixed_width();
    frame->size_target.y = ui_top_fixed_height();
    frame->size_wanted[0] = ui_top_width();
    frame->size_wanted[1] = ui_top_height();
    frame->padding = ui_top_padding();
    frame->layout_dir = ui_top_layout_dir();
    frame->text_alignment = ui_top_text_alignment();
    frame->rounding = ui_top_rounding();
    frame->border_size = ui_top_border_size();
    frame->shadow_size = ui_top_shadow_size();
    frame->texture = ui_top_texture();
    frame->font = ui_top_font();
    frame->font_size = ui_top_font_size();
    frame->palette.background = ui_top_color_background();
    frame->palette.text = ui_top_color_text();
    frame->palette.border = ui_top_color_border();
    frame->palette.shadow = ui_top_color_shadow();
    frame->palette.hover = ui_top_color_hover();
    frame->palette.active = ui_top_color_active();
    frame->palette.accent = ui_top_color_accent();
    frame->hover_cursor = ui_top_hover_cursor();
    
    // correct frame flags
    if (frame_is_transient) {
        frame->flags |= ui_frame_flag_is_transient;
    }
    
    if (frame->pos_target.x != 0.0f) { 
        frame->flags |= ui_frame_flag_floating_x; 
    }
    
    if (frame->pos_target.y != 0.0f) {
        frame->flags |= ui_frame_flag_floating_y;
    }
    
    if (frame->size_target.x != 0.0f) {
        frame->flags |= ui_frame_flag_fixed_width;
    }
    
    if (frame->size_target.y != 0.0f) { 
        frame->flags |= ui_frame_flag_fixed_height;
    }
    
    frame->last_build_index = context->build_index;
    
    ui_auto_pop_stacks();
    
    // set most recently created frame
    context->frame_mrc = frame;
    
    return frame;
}

function ui_frame_t*
ui_frame_from_string(ui_frame_flags flags, str_t string) {
    ui_key_t seed_key = ui_top_seed_key();
    ui_key_t key = ui_key_from_string(seed_key, string);
    ui_frame_t* frame = ui_frame_from_key(flags, key);
    frame->label = ui_string_display_format(string);
    return frame;
}

function ui_frame_t*
ui_frame_from_stringf(ui_frame_flags flags, char* fmt, ...) {
    
    va_list args;
    va_start(args, fmt);
    str_t string = str_formatv(ui_build_arena(), fmt, args);
    va_end(args);
    
    ui_frame_t* frame = ui_frame_from_string(flags, string);
    
    return frame;
}

function void
ui_frame_set_display_string(ui_frame_t* frame, str_t string) {
    frame->label = string;
}

function void
ui_frame_set_custom_draw(ui_frame_t* frame,  ui_frame_custom_draw_func* func, void* data) {
    frame->flags |= ui_frame_flag_draw_custom;
    frame->custom_draw_func = func;
    frame->custom_draw_data = data;
}

function ui_frame_rec_t
ui_frame_rec_depth_first(ui_frame_t* frame) {
    ui_frame_rec_t rec = { 0 };
    
    if (frame->tree_last != nullptr) {
        rec.next = frame->tree_last;
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
    
    ui_context_t* context = ui_active();
    ui_interaction result = ui_interaction_none;
    
    // calculate interaction shape
    // TODO: maybe add more shapes?
    rect_t rect = frame->rect;
    for (ui_frame_t* parent = frame->tree_parent; parent != nullptr; parent = parent->tree_parent) {
        if (parent->flags & ui_frame_flag_draw_clip) {
            rect = rect_intersection(rect, parent->rect);
        }
    }
    
    // block out context
    b8 popup_is_ancestor = false;
    for (ui_frame_t* parent = frame; parent != nullptr; parent = parent->tree_parent) {
        if (parent == context->frame_popup) {
            popup_is_ancestor = true;
            break;
        }
    }
    
    rect_t popup_rect = { 0 };
    if (!popup_is_ancestor && context->popup_is_open) {
        popup_rect = context->frame_popup->rect;
    }
    
    b8 mouse_in_bounds = rect_contains(rect, context->mouse_pos) && !rect_contains(popup_rect, context->mouse_pos);
    
    // go through ui events
    for (ui_event_t* event = ui_state.event_first, *next = nullptr; event != nullptr; event = next) {
        next = event->next;
        
        // skip if not same window
        if (!os_handle_equals(context->window, event->window)) { continue; }
        b8 taken = false;
        
        // interactable
        if (frame->flags & ui_frame_flag_interactable) {
            
            // if mouse press event
            if (event->type == ui_event_type_mouse_press && mouse_in_bounds) {
                
                // single click
                context->key_active[event->mouse] = frame->key;
                context->key_focused = frame->key;
                result |= (ui_interaction_left_pressed << event->mouse);
                
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
            
            // if mouse release event
            if (event->type == ui_event_type_mouse_release) {
                
                // if we release on the frame but frame was not active
                if (mouse_in_bounds) {
                    result |= (ui_interaction_left_released << event->mouse);
                }
                
                // if frame was active
                if (ui_key_equals(context->key_active[event->mouse], frame->key)) {
                    
                    // if we released on the frame and frame is active
                    if (mouse_in_bounds) {
                        result |= (ui_interaction_left_clicked << event->mouse);
                        taken = true;
                    }
                    
                    // if was active, reset active key
                    context->key_active[event->mouse] = { 0 };
                }
                
            }
            
            // if mouse scroll event
            if (event->type == ui_event_type_mouse_scroll && mouse_in_bounds) {
                
                // scrollable
                if (frame->flags & ui_frame_flag_scrollable) {
                    vec2_t scroll = event->scroll;
                    
                    // swap scrolling on shift
                    if (event->modifiers & os_modifier_shift) {
                        scroll.x = event->scroll.y;
                        scroll.y = event->scroll.x;
                    }
                    
                    taken = true;
                }
                
                // view scrolling
                if (frame->flags & ui_frame_flag_view_scroll && event->modifiers != os_modifier_ctrl) {
                    
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
                    
                    // clamp view scroll
                    vec2_t max_view_offset_target = vec2(max(0.0f, frame->view_bounds.x - frame->size.x), max(0.0f, frame->view_bounds.y - frame->size.y));
                    if (frame->flags & ui_frame_flag_view_clamp_x) { 
                        frame->view_offset_target.x = clamp(frame->view_offset_target.x, 0.0f, max_view_offset_target.x); 
                    }
                    
                    if (frame->flags & ui_frame_flag_view_clamp_y) { 
                        frame->view_offset_target.y = clamp(frame->view_offset_target.y, 0.0f, max_view_offset_target.y);
                    }
                    
                    taken = true;
                }
                
            }
            
        }
        
        if (taken) {
            ui_event_pop(event);
        }
        
    }
    
    // mouse dragging
    if (frame->flags & ui_frame_flag_interactable) {
        for (i32 mouse_button = 0; mouse_button < os_mouse_button_count; mouse_button++) {
            
            // single dragging
            if (ui_key_equals(context->key_active[mouse_button], frame->key) || (result & ui_interaction_left_pressed << mouse_button)) {
                result |= ui_interaction_left_dragging << mouse_button;
            }
            
            // double and triple dragging
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
    if ((frame->flags & ui_frame_flag_interactable) && mouse_in_bounds) {
        
        // if we are dragging
        if (ui_drag_is_active() &&  (ui_key_equals(context->key_hovered, { 0 }))) { 
            result |= ui_interaction_hovered;
            context->key_hovered = frame->key;
        } else if ((ui_key_equals(context->key_hovered, {0}) || ui_key_equals(context->key_hovered, frame->key)) &&
                   (ui_key_equals(context->key_active[os_mouse_button_left], {0}) || ui_key_equals(context->key_active[os_mouse_button_left], frame->key)) &&
                   (ui_key_equals(context->key_active[os_mouse_button_middle], {0}) || ui_key_equals(context->key_active[os_mouse_button_middle], frame->key)) &&
                   (ui_key_equals(context->key_active[os_mouse_button_right], {0}) || ui_key_equals(context->key_active[os_mouse_button_right], frame->key))) {
            context->key_hovered = frame->key;
            result |= ui_interaction_hovered;
            
        }
        
        // mouse over even if not active
        result |= ui_interaction_mouse_over;
    }
    
    // close popup
    if(context->popup_is_open && !popup_is_ancestor && (result & (ui_interaction_left_pressed | ui_interaction_right_pressed | ui_interaction_middle_pressed))) {
        ui_popup_close();
    }
    
    return result;
}

//- views

function ui_view_t* 
ui_view_create(ui_context_t* context, str_t label, ui_view_function* view_func) {
    
    // grab from free list or allocate one.
    ui_view_t* view = context->view_free;
    if (view != nullptr) {
        stack_pop_n(context->view_free, global_next);
    } else {
        view = (ui_view_t*)arena_alloc(context->arena, sizeof(ui_view_t));
    }
    memset(view, 0, sizeof(ui_view_t));
    dll_push_back_np(context->view_first, context->view_last, view, global_next, global_prev);
    
    // fill struct
    view->label = label;
    view->view_func = view_func;
    
    return view;
}

function void 
ui_view_release(ui_context_t* context, ui_view_t* view) {
    dll_remove_np(context->view_first, context->view_last, view, global_next, global_prev);
    stack_push_n(context->view_free, view, global_next);
}

function void 
ui_view_insert(ui_panel_t* panel, ui_view_t* view, ui_view_t* prev) {
    dll_insert(panel->view_first, panel->view_last, prev, view);
    panel->view_focus = view;
    view->parent_panel = panel;
}

function void 
ui_view_remove(ui_panel_t* panel, ui_view_t* view) {
    
    // if we are focused on the current view
    if (panel->view_focus == view) {
        panel->view_focus = nullptr;
        
        // try to get the next view
        for (ui_view_t* v = view->next; v != nullptr; v = v->next) {
            panel->view_focus = v;
        }
        
        // or try to get the prev view
        for (ui_view_t* v = view->prev; v != nullptr; v = v->prev) {
            panel->view_focus = v;
        }
    }
    
    // remove from list
    dll_remove(panel->view_first, panel->view_last, view);
    view->parent_panel == nullptr;
    
}

//- panels

function ui_panel_t*
ui_panel_create(ui_context_t* context, f32 percent, ui_axis split_axis) {
    
    // grab from free list or allocate one
    ui_panel_t* panel = context->panel_free;
    if (panel != nullptr) {
        stack_pop_n(context->panel_free, list_next);
    } else {
        panel = (ui_panel_t*)arena_alloc(context->arena, sizeof(ui_panel_t));
    }
    memset(panel, 0, sizeof(ui_panel_t));
    dll_push_back_np(context->panel_first, context->panel_last, panel, list_next, list_prev);
    
    // fill struct
    panel->percent_of_parent = percent;
    panel->split_axis = split_axis;
    
    context->panel_count++;
    
    return panel;
    
}

function void 
ui_panel_release(ui_context_t* context, ui_panel_t* panel) {
    dll_remove_np(context->panel_first, context->panel_last, panel, list_next, list_prev);
    stack_push_n(context->panel_free, panel, list_next);
    context->panel_count--;
}

function void 
ui_panel_insert(ui_panel_t* parent, ui_panel_t* panel, ui_panel_t* prev) {
    dll_insert_np(parent->tree_first, parent->tree_last, prev, panel, tree_next, tree_prev);
    panel->tree_parent = parent;
    parent->child_count++;
}

function void 
ui_panel_remove(ui_panel_t* panel) {
    ui_panel_t* parent = panel->tree_parent;
    dll_remove_np(parent->tree_first, parent->tree_last, panel, tree_next, tree_prev);
    panel->tree_next = nullptr;
    panel->tree_prev = nullptr;
    panel->tree_parent = nullptr;
    parent->child_count--;
}

function ui_panel_rec_t 
ui_panel_rec_depth_first(ui_panel_t* panel) {
    ui_panel_rec_t rec = { 0 };
    
    if (panel->tree_first != nullptr) {
        rec.next = panel->tree_first;
        rec.push_count = 1;
    } else for (ui_panel_t* p = panel; p != 0; p = p->tree_parent) {
        if (p->tree_next != nullptr) {
            rec.next = p->tree_next;
            break;
        }
        rec.pop_count++;
    }
    
    return rec;
}

function rect_t 
ui_rect_from_panel_child(ui_panel_t* parent, ui_panel_t* panel, rect_t parent_rect) {
    
    rect_t result = parent_rect;
    
    if (parent != nullptr) {
        
        vec2_t parent_rect_size = rect_size(parent_rect);
        ui_axis axis = parent->split_axis;
        
        result.v1[axis] = result.v0[axis];
        
        for (ui_panel_t* child = parent->tree_first; child != nullptr; child = child->tree_next) {
            result.v1[axis] += parent_rect_size[axis] * child->percent_of_parent;
            if (child == panel) {
                break;
            }
            result.v0[axis] = result.v1[axis];
        }
        
        result = rect_round(result);
    }
    
    return result;
}

function rect_t 
ui_rect_from_panel(ui_panel_t* panel, rect_t root_rect) {
    
    temp_t scratch = scratch_begin();
    ui_panel_t* root = panel;
    
    // count ancestors
    i32 ancestor_count = 0;
    for (ui_panel_t* p = panel->tree_parent; p != nullptr; p = p->tree_parent) {
        ancestor_count++;
    }
    
    // gather ancestors
    ui_panel_t** ancestors = (ui_panel_t**)arena_alloc(scratch.arena, sizeof(ui_panel_t*) * ancestor_count);
    i32 ancestor_index = 0;
    for (ui_panel_t* p = panel->tree_parent; p != nullptr; p = p->tree_parent) {
        ancestors[ancestor_index] = p;
        ancestor_index++;
    }
    
    // go from highest ancestor and calculate rect
    rect_t parent_rect = root_rect;
    for (i32 ancestor_index = ancestor_count - 1; ancestor_index >= 0 && ancestor_index < ancestor_count; ancestor_index--) {
        
        ui_panel_t* ancestor = ancestors[ancestor_index];
        
        if (ancestor->tree_parent != nullptr) {
            parent_rect = ui_rect_from_panel_child(ancestor->tree_parent, ancestor, parent_rect);
        }
        
    }
    
    // calculate final rect
    rect_t result = ui_rect_from_panel_child(panel->tree_parent, panel, parent_rect);
    scratch_end(scratch);
    
    return result;
}


//- layout

function void
ui_layout_solve_independent(ui_frame_t* frame, ui_axis axis) {
    
    // this sets the sizes of the frames which don't depend
    // on any other frames ie: sizes such as pixel or text.
    // if the frame has the any fixed size flag, then the 
    // size has already been set and we don't need to set
    // the size here.
    
    // set size based on size wanted type
    switch (frame->size_wanted[axis].type) {
        
        // by pixels
        case ui_size_type_pixel: {
            frame->size_target[axis] = frame->size_wanted[axis].value;
            break;
        }
        
        // by text
        case ui_size_type_text: {
            f32 padding = frame->size_wanted[axis].value;
            vec2_t text_size = ui_text_size(frame->font, frame->font_size, frame->label);
            frame->size_target[axis] = padding + text_size[axis] + 8.0f;
            break;
        }
        
    }
    
    // fix if transient frames
    if (frame->flags & ui_frame_flag_is_transient) {
        frame->size[axis] = frame->size_target[axis];
    }
    
    // recurse through children
    for (ui_frame_t* child = frame->tree_first; child != nullptr; child = child->tree_next) {
        ui_layout_solve_independent(child, axis);
    }
    
}

function void
ui_layout_solve_upward_dependent(ui_frame_t* frame, ui_axis axis) {
    
    // this sets the sizes of the frames that depend on
    // the frames above it in the tree ie: sizes by
    // percent of parent. if frame has any fixed size flag, 
    // then the size has already been set and we don't need
    // to set the size here.
    
    // fix if transient frames
    if (frame->flags & ui_frame_flag_is_transient) {
        frame->size[axis] = frame->size_target[axis];
    }
    
    // if wanted size is percent of parent
    if (frame->size_wanted[axis].type == ui_size_type_percent) {
        
        // find a parent that has a strict size
        ui_frame_t* parent = nullptr;
        for (ui_frame_t* p = frame->tree_parent; p != nullptr; p = p->tree_parent) {
            if (p->flags & (ui_frame_flag_fixed_width << axis) ||
                p->size_wanted[axis].type == ui_size_type_pixel ||
                p->size_wanted[axis].type == ui_size_type_percent) {
                parent = p;
                break;
            }
        }
        
        // calculate percent size of strict parent
        if (parent != nullptr) {
            // get parent size
            frame->size_target[axis] = (parent->size_target[axis] - parent->padding) * frame->size_wanted[axis].value;
        }
        
    }
    
    // recurse children
    for (ui_frame_t* child = frame->tree_first; child != nullptr; child = child->tree_next) {
        ui_layout_solve_upward_dependent(child, axis);
    }
    
}

function void
ui_layout_solve_downward_dependent(ui_frame_t* frame, ui_axis axis) {
    
    // this sets the sizes of the frames that depend on
    // the frames below it in the tree ie: sizes by
    // children. if frame has any fixed size flag, then 
    // the size has already been set and we don't need
    // to set the size here.
    
    // recurse children
    for (ui_frame_t* child = frame->tree_first; child != nullptr; child = child->tree_next) {
        ui_layout_solve_downward_dependent(child, axis);
    }
    
    // fix if transient frames
    if (frame->flags & ui_frame_flag_is_transient) {
        frame->size[axis] = frame->size_target[axis];
    }
    
    // if wanted size is percent of parent
    if (frame->size_wanted[axis].type == ui_size_type_by_children) {
        
        // find width of children
        f32 sum = 0.0f;
        
        for (ui_frame_t* child = frame->tree_first; child != nullptr; child = child->tree_next) {
            if (!(child->flags & (ui_frame_flag_floating_x << axis))) {
                f32 child_size =  child->size_target[axis];
                ui_axis layout_axis = ui_axis_from_dir(frame->layout_dir);
                if (layout_axis == axis) {
                    sum += child_size;
                } else {
                    sum = max(sum, child_size);
                }
            }
        }
        
        frame->size_target[axis] = sum + (frame->padding * 2.0f);
        
    }
    
    
}

function void
ui_layout_solve_violations(ui_frame_t* frame, ui_axis axis) {
    
    // this solves any size violations between the frames. 
    ui_context_t* context = ui_active();
    
    
    f32 frame_size = frame->size_target[axis] - (frame->padding * 2.0f);
    ui_axis layout_axis = ui_axis_from_dir(frame->layout_dir);
    
    // fix children if in non layout axis
    if (layout_axis != axis && !(frame->flags & (ui_frame_flag_overflow_x << axis))) {
        for (ui_frame_t* child = frame->tree_first; child != nullptr; child = child->tree_next) {
            if (!(child->flags & (ui_frame_flag_floating_x << axis))) {
                f32 child_size = child->size_target[axis];
                f32 violation = child_size - frame_size;
                f32 fixup = clamp(violation, 0.0f, child_size);
                if (fixup > 0.0f) {
                    child->size_target[axis] -= fixup;
                }
            }
        }
    }
    
    // fix children in layout axis
    if (layout_axis == axis && !(frame->flags & (ui_frame_flag_overflow_x << axis))) {
        
        f32 total_allowed_size = frame_size;
        f32 total_size = 0.0f;
        f32 total_weighted_size = 0.0f;
        u32 child_count = 0;
        
        // calculate total size and weighted size
        for (ui_frame_t* child = frame->tree_first; child != nullptr; child = child->tree_next) {
            if (!(child->flags & (ui_frame_flag_floating_x << axis))) {
                f32 child_size = child->size_target[axis];
                total_size += child_size;
                total_weighted_size += child_size * (1.0f - child->size_wanted[axis].strictness);
                child_count++;
            }
        }
        
        f32 violation = total_size - total_allowed_size;
        if (violation > 0.0f) {
            
            // find fixup size
            f32 child_fixup_sum = 0.0f;
            f32* child_fixups = (f32*)arena_alloc(ui_build_arena(), sizeof(f32) * child_count);
            u32 child_index = 0;
            
            for (ui_frame_t* child = frame->tree_first; child != nullptr; child = child->tree_next) {
                if (!(child->flags & (ui_frame_flag_floating_x << axis))) {
                    f32 child_fixup = child->size_target[axis] * (1.0f - child->size_wanted[axis].strictness);
                    child_fixup = max(0.0f, child_fixup);
                    child_fixups[child_index] = child_fixup;
                    child_fixup_sum += child_fixup;
                    child_index++;
                }
            }
            
            // fixup children
            child_index = 0;
            for (ui_frame_t* child = frame->tree_first; child != nullptr; child = child->tree_next) {
                if (!(child->flags & (ui_frame_flag_floating_x << axis))) {
                    f32 fixup_percent = violation / total_weighted_size;
                    fixup_percent = clamp_01(fixup_percent);
                    child->size_target[axis] -= child_fixups[child_index] * fixup_percent;
                    child_index++;
                }
            }
        }
    }
    
    // fix child sizes if we allow overflow
    if (frame->flags & (ui_frame_flag_overflow_x << axis)) {
        for (ui_frame_t* child = frame->tree_first; child != nullptr; child = child->tree_next) {
            if (child->size_wanted[axis].type == ui_size_type_percent) {
                child->size_target[axis] = frame_size * child->size_wanted[axis].value;
            }
        }
    }
    
    // recurse children
    for (ui_frame_t* child = frame->tree_first; child != nullptr; child = child->tree_next) {
        ui_layout_solve_violations(child, axis);
    }
    
}

function void
ui_layout_solve_set_positions(ui_frame_t* frame, ui_axis axis) {
    
    ui_context_t* context = ui_active();
    
    ui_dir layout_dir = frame->layout_dir;
    ui_axis layout_axis = ui_axis_from_dir(layout_dir);
    ui_side layout_side = ui_side_from_dir(layout_dir);
    f32 layout_pos = frame->padding;
    f32 bounds = 0.0f;
    
    if (layout_side == ui_side_min && layout_axis == axis) {
        layout_pos = frame->size[axis] - frame->padding;
    }
    
    for (ui_frame_t* child = frame->tree_first; child != nullptr; child = child->tree_next) {
        
        f32 original_pos = min(child->rect.v0[axis], child->rect.v1[axis]);
        f32 parent_pos = frame->rect.v0[axis];
        f32 child_size = child->size_target[axis];
        
        // calculate position
        // TODO: there will be a problem here about the view bounds, so probably 
        // should seperate it out.
        if (!(child->flags & (ui_frame_flag_floating_x << axis))) {
            if (layout_side == ui_side_min && layout_axis == axis) {
                child->pos_target[axis] = layout_pos - child_size;
                layout_pos -= child_size;
                bounds += child_size;
            } else {
                child->pos_target[axis] = layout_pos;
                if (layout_axis == axis) {
                    layout_pos += child_size; 
                    bounds += child_size;
                }
            }
        }
        
        // calculate pos and size for child
        if (child->flags & (ui_frame_flag_anim_pos_x << axis)) {
            child->pos[axis] += (child->pos_target[axis] - child->pos[axis]) * context->anim_slow_rate;
            if (fabsf(child->pos_target[axis] - child->pos[axis]) < 1.0f) { child->pos[axis] = child->pos_target[axis]; }
        } else {
            child->pos[axis] = (child->pos_target[axis]);
        }
        
        if (child->flags & (ui_frame_flag_anim_width << axis)) {
            child->size[axis] += (child->size_target[axis] - child->size[axis]) * context->anim_slow_rate;
            if (fabsf(child->size_target[axis] - child->size[axis]) < 1.0f) { child->size[axis] = child->size_target[axis]; }
        } else {
            child->size[axis] = child->size_target[axis];
        }
        
        // view offset
        f32 view_offset = floorf(frame->view_offset[axis] * !(child->flags & (ui_frame_flag_ignore_view_scroll_x << axis)));
        
        // apply to rect
        child->rect.v0[axis] = parent_pos + child->pos[axis] - view_offset;
        child->rect.v1[axis] = child->rect.v0[axis] + child->size[axis];
        
    }
    
    // store view bounds
    frame->view_bounds[axis] = bounds;
    
    // recurse children
    for (ui_frame_t* child = frame->tree_first; child != nullptr; child = child->tree_next) {
        ui_layout_solve_set_positions(child, axis);
    }
    
}

//- layout widgets 

function void
ui_row_begin() {
    ui_set_next_layout_dir(ui_dir_right);
    ui_frame_t* frame = ui_frame_from_key(0, { 0 });
    ui_frame_set_display_string(frame, str("row_frame"));
    ui_push_parent(frame);
}

function void
ui_row_end() {
    ui_pop_parent();
}

function void
ui_column_begin() {
    ui_set_next_layout_dir(ui_dir_down);
    ui_frame_t* frame = ui_frame_from_key(0, { 0 });
    ui_frame_set_display_string(frame, str("column_frame"));
    ui_push_parent(frame);
}

function void 
ui_column_end() {
    ui_pop_parent();
}

function void 
ui_padding_begin(f32 size) {
    ui_set_next_padding(size);
    ui_frame_t* frame = ui_frame_from_key(0, { 0 });
    ui_frame_set_display_string(frame, str("padding_frame"));
    ui_push_parent(frame);
}

function void
ui_padding_end() {
    ui_pop_parent();
}

//- tooltip

function void 
ui_tooltip_begin() {
    ui_context_t* context = ui_active();
    context->frame_tooltip->flags |= (ui_frame_flag_draw_background | ui_frame_flag_draw_shadow);
    ui_push_parent(context->frame_tooltip);
}

function void
ui_tooltip_end() {
    ui_pop_parent();
}

//- popups

function b8
ui_popup_begin(ui_key_t key) {
    ui_context_t* context = ui_active();
    
    b8 is_open = ui_key_equals(context->key_popup, key);
    
    if (is_open) {
        context->frame_popup->flags |= (ui_frame_flag_draw_background | ui_frame_flag_draw_shadow);
        context->popup_updated_this_frame = true;
        ui_push_parent(context->frame_popup);
    }
    
    return is_open;
}

function void
ui_popup_end() {
    //ui_context_t* context = ui_active();
    //ui_frame_interaction(context->frame_popup);
    ui_pop_parent();
}

function void
ui_popup_open(ui_key_t key, vec2_t pos) {
    ui_context_t* context = ui_active();
    context->key_popup = key;
    context->popup_pos = pos;
    context->popup_is_open = true;
    context->popup_updated_this_frame = true;
}

function void
ui_popup_close() {
    ui_context_t* context = ui_active();
    context->key_popup = { 0 };
    context->popup_pos = vec2(0.0f, 0.0f);
    context->popup_is_open = false;
}

function b8
ui_popup_is_open(ui_key_t key) {
    return ui_key_equals(ui_active()->key_popup, key);
}

//- widgets

function void 
ui_spacer(ui_size_t size) {
    ui_frame_t* parent = ui_top_parent();
    ui_axis axis = ui_axis_from_dir(parent->layout_dir);
    (axis == ui_axis_x) ? ui_set_next_width(size) : ui_set_next_height(size);
    ui_frame_t* frame = ui_frame_from_key(0, { 0 });
}

function ui_interaction
ui_label(str_t label) {
    ui_frame_flags flags = 
        ui_frame_flag_draw_text;
    
    ui_frame_t* frame = ui_frame_from_string(flags, label);
    ui_interaction interaction = ui_frame_interaction(frame);
    
    return interaction;
}

function ui_interaction
ui_labelf(char* fmt, ...) {
    
    va_list args;
    va_start(args, fmt);
    str_t display_string = str_formatv(ui_build_arena(), fmt, args);
    va_end(args);
    
    ui_interaction interaction = ui_label(display_string);
    
    return interaction;
}

function ui_interaction
ui_button(str_t label) {
    
    ui_frame_flags flags =
        ui_frame_flag_interactable |
        ui_frame_flag_hover_cursor | 
        ui_frame_flag_draw_background |
        ui_frame_flag_draw_hover_effects |
        ui_frame_flag_draw_active_effects |
        ui_frame_flag_draw_text |
        ui_frame_flag_draw_shadow;
    
    ui_set_next_hover_cursor(os_cursor_hand_point);
    ui_frame_t* frame = ui_frame_from_string(flags, label);
    ui_interaction interaction = ui_frame_interaction(frame);
    
    return interaction;
    
}

function ui_interaction
ui_buttonf(char* fmt, ...) {
    
    va_list args;
    va_start(args, fmt);
    str_t display_string = str_formatv(ui_build_arena(), fmt, args);
    va_end(args);
    
    ui_interaction interaction = ui_button(display_string);
    
    return interaction;
    
}

function ui_interaction 
ui_slider(f32* value, f32 min, f32 max, str_t label) {
    
    ui_context_t* context = ui_active();
    
    // build frame
    ui_frame_flags flags = 
        ui_frame_flag_interactable |
        ui_frame_flag_hover_cursor |
        ui_frame_flag_draw_background |
        ui_frame_flag_draw_hover_effects |
        ui_frame_flag_draw_active_effects |
        ui_frame_flag_draw_shadow |
        ui_frame_flag_draw_custom;
    
    ui_set_next_hover_cursor(os_cursor_resize_EW);
    ui_set_next_text_alignment(ui_text_alignment_center);
    ui_key_t slider_key = ui_key_from_string(ui_top_seed_key(), label);
    ui_frame_t* slider_frame = ui_frame_from_key(flags, slider_key);
    
    // set display text
    str_t text = str_format(ui_build_arena(), "%.2f", *value);
    ui_frame_set_display_string(slider_frame, text);
    
    // set custom draw function and data
    f32* percent = (f32*)arena_alloc(ui_build_arena(), sizeof(f32));
    ui_frame_set_custom_draw(slider_frame, ui_slider_draw_function, percent);
    
    // do interaction
    ui_interaction interaction = ui_frame_interaction(slider_frame);
    if (interaction & ui_interaction_left_dragging) {
        vec2_t mouse_pos = context->mouse_pos;
        *value = remap(mouse_pos.x, slider_frame->rect.x0, slider_frame->rect.x1, min, max);
        *value = clamp(*value, min, max);
    }
    
    f32 percent_target = remap(*value, min, max, 0.0f, 1.0f);
    ui_key_t anim_key = ui_key_from_stringf(slider_key, "anim_percent");
    f32 anim_percent = ui_anim_ex(anim_key, ui_anim_params_create(percent_target, percent_target));
    
    *percent = anim_percent;
    
    return interaction;
}

function ui_interaction 
ui_sliderf(f32* value, f32 min, f32 max, char* fmt, ...) {
    
    va_list args;
    va_start(args, fmt);
    str_t string = str_formatv(ui_build_arena(), fmt, args);
    va_end(args);
    
    ui_interaction interaction = ui_slider(value, min, max, string);
    
    return interaction;
}

function ui_interaction 
ui_checkbox(b8* value, str_t label) {
    
    ui_context_t* context = ui_active();
    
    // build button
    ui_frame_flags button_flags =
        ui_frame_flag_interactable |
        ui_frame_flag_hover_cursor; 
    
    ui_set_next_hover_cursor(os_cursor_hand_point);
    ui_key_t button_key = ui_key_from_string(ui_top_seed_key(), label);
    ui_frame_t* button_frame = ui_frame_from_key(button_flags, button_key);
    ui_interaction button_interaction = ui_frame_interaction(button_frame);
    
    // build icon
    ui_frame_flags icon_flags =
        ui_frame_flag_draw_background |
        ui_frame_flag_draw_hover_effects |
        ui_frame_flag_draw_active_effects |
        ui_frame_flag_draw_text |
        ui_frame_flag_draw_shadow;
    
    ui_set_next_parent(button_frame);
    ui_set_next_width(ui_top_height());
    ui_set_next_text_alignment(ui_text_alignment_center);
    ui_set_next_font(ui_state.icon_font);
    ui_frame_t* icon_frame = ui_frame_from_key(icon_flags, { 0 });
    if (*value) {
        ui_frame_set_display_string(icon_frame, str("!"));
    }
    icon_frame->hover_t = button_frame->hover_t;
    icon_frame->active_t = button_frame->active_t;
    
    // do interaction
    if (button_interaction & ui_interaction_left_clicked) {
        *value = !*value;
    }
    
    return button_interaction;
}

function ui_interaction 
ui_checkboxf(b8* value, char* fmt, ...) {
    
    va_list args;
    va_start(args, fmt);
    str_t string = str_formatv(ui_build_arena(), fmt, args);
    va_end(args);
    
    ui_interaction interaction = ui_checkbox(value, string);
    
    return interaction;
}

function ui_interaction
ui_text_edit(str_t label, char* buffer, u32* size, u32 max_size) {
    
    ui_context_t* context = ui_active();
    
    // create keys
    ui_key_t container_key = ui_key_from_stringf(ui_top_seed_key(), "%.*s_container", label.size, label.data);
    ui_key_t cursor_key = ui_key_from_stringf(ui_top_seed_key(), "%.*s_cursor", label.size, label.data);
    ui_key_t mark_key = ui_key_from_stringf(ui_top_seed_key(), "%.*s_mark", label.size, label.data);
    
    b8 frame_focused = ui_key_is_focused(container_key);
    str_t edit_string = str(buffer, *size);
    
    // get cursor and mark
    i32* cursor_value = ui_data(cursor_key, (i32)0);
    i32* mark_value = ui_data(cursor_key, (i32)0);
    
    // build container frame
    ui_frame_flags flags =
        ui_frame_flag_interactable |
        ui_frame_flag_hover_cursor | 
        ui_frame_flag_draw_background |
        ui_frame_flag_draw_hover_effects |
        ui_frame_flag_draw_text |
        ui_frame_flag_draw_shadow;
    
    // if focused don't do effects
    if (frame_focused) {
        flags &= ~ui_frame_flag_draw_hover_effects;
    }
    
    ui_set_next_hover_cursor(os_cursor_I_beam);
    ui_frame_t* frame = ui_frame_from_key(flags, container_key);
    
    ivec2_t* draw_data = (ivec2_t*)arena_alloc(ui_build_arena(), sizeof(ivec2_t));
    draw_data->x = *cursor_value;
    draw_data->y = *mark_value;
    ui_frame_set_display_string(frame, edit_string);
    ui_frame_set_custom_draw(frame, ui_text_edit_draw_function, draw_data);
    
    // keyboard interaction
    if (frame_focused) {
        
        for (ui_event_t* event = ui_state.event_first, *next = nullptr; event != nullptr; event = next) {
            next = event->next;
            
            // skip if not text input events
            if (event->type != ui_event_type_edit && event->type != ui_event_type_navigate && event->type != ui_event_type_text) {
                continue;
            }
            
            // get text op
            ui_text_point_t cursor = ui_text_point(0, *cursor_value);
            ui_text_point_t mark = ui_text_point(0, *mark_value);
            ui_text_op_t text_op = ui_event_to_text_op(event, edit_string, cursor, mark);
            
            // skip if invalid
            if (text_op.flags & ui_text_op_flag_invalid) {
                continue;
            }
            
            // replace range
            if (!ui_text_point_equals(text_op.range.min, text_op.range.max) || text_op.replace.size != 0) {
                str_t new_string = ui_string_replace_range(ui_build_arena(), edit_string, text_op.range, text_op.replace);
                new_string.size = min(max_size, new_string.size);
                memcpy(buffer, new_string.data, new_string.size);
                *size = new_string.size;
            }
            
            // update cursor
            *cursor_value = text_op.cursor.column;
            *mark_value = text_op.mark.column;
            
            // pop event
            ui_event_pop(event);
            
        }
        
        
        
        
    }
    
    // mouse interaction
    ui_interaction interaction = ui_frame_interaction(frame);
    
    
    
    return 0;
}


function ui_interaction
ui_float_edit(str_t label, f32* value, f32 delta, f32 min, f32 max) {
    
    ui_context_t* context = ui_active();
    
    // create keys
    ui_key_t top_key = ui_top_seed_key();
    ui_key_t decrement_key = ui_key_from_stringf(top_key, "%.*s_decrement", label.size, label.data);
    ui_key_t increment_key = ui_key_from_stringf(top_key, "%.*s_increment", label.size, label.data);
    ui_key_t slider_key = ui_key_from_stringf(top_key, "%.*s_slider", label.size, label.data);
    
    // create parent
    ui_frame_flags ui_parent_flags = 
        ui_frame_flag_draw_background |
        ui_frame_flag_draw_border |
        ui_frame_flag_draw_shadow;
    
    ui_set_next_layout_dir(ui_dir_right);
    ui_frame_t* parent_frame = ui_frame_from_key(ui_parent_flags, { 0 });
    
    // determine if we should show arrows
    b8 show_arrows = false;
    ui_key_t last_hovered_key = context->key_hovered_prev;
    if (ui_key_equals(last_hovered_key, decrement_key) || ui_key_is_active(decrement_key) ||
        ui_key_equals(last_hovered_key, increment_key) || ui_key_is_active(increment_key) ||
        ui_key_equals(last_hovered_key, slider_key) || ui_key_is_active(slider_key)) {
        show_arrows = true;
    }
    
    ui_push_parent(parent_frame);
    
    b8 modifier_shift = os_get_modifiers() & os_modifier_shift;
    f32 adjusted_delta = delta * (modifier_shift ? 10.0f : 1.0f);
    
    // decrement and increment flags
    u32 frame_flags =
        ui_frame_flag_interactable |
        ui_frame_flag_draw_background |
        ui_frame_flag_draw_hover_effects |
        ui_frame_flag_draw_active_effects;
    
    // hide if not hovered
    if (show_arrows) {
        flag_set(frame_flags, ui_frame_flag_draw_text);
    }
    
    // decrement
    ui_set_next_font(ui_state.icon_font);
    ui_set_next_text_alignment(ui_text_alignment_center);
    ui_set_next_width(ui_size_pixels(15.0f, 1.0f));
    ui_set_next_rounding_00(0.0f);
    ui_set_next_rounding_01(0.0f);
    ui_set_next_color_background(color(0x00000000));
    ui_frame_t* decrement_frame = ui_frame_from_key(frame_flags, decrement_key);
    ui_frame_set_display_string(decrement_frame, str("N"));
    ui_interaction decrement_interaction = ui_frame_interaction(decrement_frame);
    
    if (decrement_interaction & ui_interaction_left_clicked) {
        *value -= adjusted_delta;
    }
    
    // slider/text_edit
    ui_frame_flags slider_flags =
        ui_frame_flag_interactable |
        ui_frame_flag_draw_background | 
        ui_frame_flag_draw_hover_effects |
        ui_frame_flag_draw_active_effects |
        ui_frame_flag_draw_text |
        ui_frame_flag_hover_cursor;
    
    str_t number_text = str_format(ui_build_arena(), "%.2f", *value);
    ui_set_next_width(ui_size_percent(1.0f));
    ui_set_next_height(ui_size_percent(1.0f));
    ui_set_next_text_alignment(ui_text_alignment_center);
    ui_set_next_hover_cursor(os_cursor_resize_EW);
    ui_set_next_rounding(vec4(0.0f));
    ui_set_next_color_background(color(0x00000000));
    ui_frame_t* slider_frame = ui_frame_from_key(slider_flags, slider_key);
    ui_frame_set_display_string(slider_frame, number_text);
    ui_interaction slider_interaction = ui_frame_interaction(slider_frame);
    
    // slider interatcion
    if (slider_interaction & ui_interaction_left_dragging) {
        
        *value = *value + (adjusted_delta * ui_active()->mouse_delta.x);
        
        // don't clamp if everything equals 0.0f
        if (min != max != 0.0f) {
            *value = clamp(*value, min, max);
        }
        
    }
    
    // increment
    ui_set_next_text_alignment(ui_text_alignment_center);
    ui_set_next_font(ui_state.icon_font);
    ui_set_next_width(ui_size_pixels(15.0f, 1.0f));
    ui_set_next_height(ui_size_percent(1.0f));
    ui_set_next_rounding_10(0.0f);
    ui_set_next_rounding_11(0.0f);
    ui_set_next_color_background(color(0x00000000));
    ui_frame_t* increment_frame = ui_frame_from_key(frame_flags, increment_key);
    ui_frame_set_display_string(increment_frame, str("O"));
    ui_interaction increment_interaction = ui_frame_interaction(increment_frame);
    
    if (increment_interaction & ui_interaction_left_clicked) {
        *value += adjusted_delta;
    }
    
    ui_pop_parent();
    
    
    ui_interaction interaction = 0;
    return interaction;
}




function b8
ui_expander_begin(str_t label) {
    
    // build container
    ui_frame_flags container_flags =
        ui_frame_flag_draw_clip |
        ui_frame_flag_draw_background |
        ui_frame_flag_draw_shadow |
        ui_frame_flag_anim_height;
    
    ui_set_next_size(ui_size_by_children(1.0f), ui_size_by_children(1.0f));
    ui_set_next_color_background(color(0x2d2d2dff));
    ui_key_t container_key = ui_key_from_stringf(ui_top_seed_key(), "%.*s_container", label.size, label.data);
    ui_frame_t* container_frame = ui_frame_from_key(container_flags, container_key);
    
    // get persistent expanded value
    b8* expanded_value = ui_data(container_key, false);
    
    // build button
    ui_frame_flags button_flags = 
        ui_frame_flag_interactable |
        ui_frame_flag_draw_background |
        ui_frame_flag_draw_hover_effects |
        ui_frame_flag_draw_active_effects;
    
    ui_set_next_parent(container_frame);
    ui_set_next_color_background(color(0x2d2d2dff));
    ui_set_next_layout_dir(ui_dir_right);
    ui_key_t button_key = ui_key_from_string(container_key, str("button"));
    ui_frame_t* button_frame = ui_frame_from_key(button_flags, button_key);
    ui_interaction button_interaction = ui_frame_interaction(button_frame);
    
    // build icon
    ui_set_next_parent(button_frame);
    ui_set_next_width(ui_size_pixels(15.0f));
    ui_set_next_height(ui_size_percent(1.0f));
    ui_set_next_font(ui_state.icon_font);
    ui_labelf("%c", (*expanded_value) ? icon_down : icon_right);
    
    // build label
    ui_set_next_parent(button_frame);
    ui_set_next_width(ui_size_percent(1.0f));
    ui_set_next_height(ui_size_percent(1.0f));
    ui_label(label);
    
    
    // do interaction
    if (button_interaction & ui_interaction_left_clicked) {
        *expanded_value = !*expanded_value;
    }
    
    if (*expanded_value) {
        ui_push_parent(container_frame);
    }
    
    return *expanded_value;
}

function b8
ui_expanderf_begin(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    str_t string = str_formatv(ui_build_arena(), fmt, args);
    va_end(args);
    
    b8 expanded = ui_expander_begin(string);
    
    return expanded;
}

function void
ui_expander_end() {
    ui_pop_parent();
}


//- custom draw functions

function void 
ui_drop_site_draw_function(ui_frame_t* frame) {
    
    // get data
    ui_drop_site_draw_data_t* data = (ui_drop_site_draw_data_t*)frame->custom_draw_data;
    ui_drop_site_type type = data->type;
    ui_axis axis = data->axis;
    ui_side side = data->side;
    
    f32 frame_width = rect_width(frame->rect);
    f32 frame_height = rect_height(frame->rect);
    
    // get colors
    ui_palette_t* palette = &frame->palette;
    color_t accent_color = palette->accent;
    accent_color.a = lerp(0.1f, 0.3f, frame->hover_t);
    
    // draw
    switch (type) {
        
        case ui_drop_site_type_edge: {
            
            rect_t min_rect;
            rect_t max_rect;
            
            if (axis == ui_axis_x) {
                f32 half_width = roundf(frame_width * 0.5f);
                min_rect = rect_cut_left(frame->rect, half_width + 2.5f);
                max_rect = rect_cut_right(frame->rect, half_width + 2.5f);
            } else {
                f32 half_height = roundf(frame_height * 0.5f);
                min_rect = rect_cut_top(frame->rect, half_height + 2.5f);
                max_rect = rect_cut_bottom(frame->rect, half_height + 2.5f);
            }
            
            // pad rects
            min_rect = rect_shrink(min_rect, 5.0f);
            max_rect = rect_shrink(max_rect, 5.0f);
            
            // side
            draw_set_next_color(accent_color);
            draw_set_next_rounding(frame->rounding);
            draw_rect(side == ui_side_min ? min_rect : max_rect);
            
            // borders
            draw_push_color(palette->border);
            draw_push_thickness(1.0f);
            draw_push_rounding(frame->rounding);
            
            draw_rect(min_rect);
            draw_rect(max_rect);
            
            draw_pop_rounding();
            draw_pop_thickness();
            draw_pop_color();
            
            break;
        }
        
        case ui_drop_site_type_split: {
            
            // calculate rects
            rect_t min_rect;
            rect_t mid_rect;
            rect_t max_rect;
            
            if (axis == ui_axis_x) {
                f32 quarter_width = roundf(frame_width * 0.25f);
                min_rect = rect_cut_left(frame->rect, quarter_width + 5.0f);
                max_rect = rect_cut_right(frame->rect, quarter_width + 5.0f);
                mid_rect = rect(min_rect.x1 - 5.0f, min_rect.y0, max_rect.x0 + 5.0f, max_rect.y1);
            } else {
                f32 quarter_height = roundf(frame_height * 0.25f);
                min_rect = rect_cut_top(frame->rect, quarter_height + 5.0f);
                max_rect = rect_cut_bottom(frame->rect, quarter_height + 5.0f);
                mid_rect = rect(min_rect.x0, min_rect.y1 - 5.0f, max_rect.x1, max_rect.y0 + 5.0f);
            }
            
            // pad rects
            min_rect = rect_shrink(min_rect, 5.0f);
            mid_rect = rect_shrink(mid_rect, 5.0f);
            max_rect = rect_shrink(max_rect, 5.0f);
            
            // middle
            draw_set_next_color(accent_color);
            draw_set_next_rounding(frame->rounding);
            draw_rect(mid_rect);
            
            // borders
            draw_push_color(palette->border);
            draw_push_thickness(1.0f);
            draw_push_rounding(frame->rounding);
            
            draw_rect(min_rect);
            draw_rect(mid_rect);
            draw_rect(max_rect);
            
            draw_pop_rounding();
            draw_pop_thickness();
            draw_pop_color();
            
            break;
        }
        
        case ui_drop_site_type_center: {
            
            rect_t inner_rect = rect_shrink(frame->rect, 5.0f);
            
            // inner
            draw_set_next_color(accent_color);
            draw_set_next_rounding(frame->rounding);
            draw_rect(inner_rect);
            
            // border
            draw_set_next_color(palette->border);
            draw_set_next_thickness(1.0f);
            draw_set_next_rounding(frame->rounding);
            draw_rect(inner_rect);
            
            break;
        }
        
        
        
        /*
        case ui_dir_none: {
            rect_t inner_rect = rect_shrink(frame->rect, 5.0f);
            
            // inner
            draw_set_next_color(accent_color);
            draw_set_next_rounding(frame->rounding);
            draw_rect(inner_rect);
            
            // border
            draw_set_next_color(palette->border);
            draw_set_next_thickness(1.0f);
            draw_set_next_rounding(frame->rounding);
            draw_rect(inner_rect);
            
            break;
        }
        
        case ui_dir_left:
        case ui_dir_right:
        case ui_dir_down:
        case ui_dir_up: {
            
            rect_t min_rect;
            rect_t max_rect;
            
            if (axis == ui_axis_x) {
                min_rect = rect_shrink(rect_cut_left(frame->rect, roundf(frame_width * 0.5f)), 5.0f);
                max_rect = rect_shrink(rect_cut_right(frame->rect, roundf(frame_width * 0.5f)), 5.0f);
                min_rect.x1 += 2.5f;
                max_rect.x0 -= 2.5f;
            } else {
                min_rect = rect_shrink(rect_cut_top(frame->rect, roundf(frame_height * 0.5f)), 5.0f);
                max_rect = rect_shrink(rect_cut_bottom(frame->rect, roundf(frame_height * 0.5f)), 5.0f);
                min_rect.y1 += 2.5f;
                max_rect.y0 -= 2.5f;
            }
            
            // inner
            draw_set_next_color(accent_color);
            draw_set_next_rounding(frame->rounding);
            draw_rect(side == ui_side_min ? min_rect: max_rect);
            
            // borders
            draw_set_next_color(palette->border);
            draw_set_next_thickness(1.0f);
            draw_set_next_rounding(frame->rounding);
            draw_rect(min_rect);
            
            draw_set_next_color(palette->border);
            draw_set_next_thickness(1.0f);
            draw_set_next_rounding(frame->rounding);
            draw_rect(max_rect);
            
            break;
        }*/
        
    }
    
    
}


function void 
ui_slider_draw_function(ui_frame_t* frame) {
    
    ui_context_t* context = ui_active();
    
    // get data
    f32* data = (f32*)frame->custom_draw_data;
    
    // calculate percent bar
    rect_t bar_rect = frame->rect;
    bar_rect.x1 = lerp(bar_rect.x0, bar_rect.x1, *data);
    
    // draw bar
    color_t bar_color = frame->palette.accent;
    bar_color.a = 0.3f;
    draw_set_next_color(bar_color);
    draw_set_next_rounding(frame->rounding);
    draw_rect(bar_rect);
    
    // draw text
    vec2_t text_pos = ui_text_align(frame->font, frame->font_size, frame->label, frame->rect, frame->text_alignment);
    draw_push_font(frame->font);
    draw_push_font_size(frame->font_size);
    
    // text shadow
    color_t text_shadow = frame->palette.shadow; text_shadow.a = 0.8f;
    draw_set_next_color(text_shadow);
    draw_text(frame->label, vec2_add(text_pos, 1.0f));
    
    // text
    draw_set_next_color(frame->palette.text);
    draw_text(frame->label, text_pos);
    
    draw_pop_font_size();
    draw_pop_font();
    
}


function void 
ui_text_edit_draw_function(ui_frame_t* frame) {
    
    ui_context_t* context = ui_active();
    
    // get data
    ivec2_t* data = (ivec2_t*)frame->custom_draw_data;
    i32 cursor = (*data).x;
    i32 mark = (*data).y;
    
    if (ui_key_is_focused(frame->key)) {
        
        // get offsets
        f32 cursor_offset = ui_text_offset_from_index(frame->font, frame->font_size, frame->label, cursor);
        f32 mark_offset = ui_text_offset_from_index(frame->font, frame->font_size, frame->label, mark);
        vec2_t text_start = ui_text_align(frame->font, frame->font_size, frame->label, frame->rect, frame->text_alignment);
        
        // draw cursor
        draw_set_next_color(frame->palette.accent);
        
        rect_t cursor_rect = rect(
                                  frame->rect.x0 + text_start.x + cursor_offset - 1.0f,
                                  frame->rect.y0 + 2.0f,
                                  frame->rect.x0 + text_start.x + cursor_offset + 1.0f,
                                  frame->rect.y1 - 2.0f
                                  );
        draw_rect(cursor_rect);
        
        // draw mark
        color_t mark_color = frame->palette.accent;
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






//- stacks

function void
ui_auto_pop_stacks() {
    
    ui_context_t* context = ui_active();
    
    if (context->parent_stack.auto_pop) { ui_pop_parent(); context->parent_stack.auto_pop = false; };
    if (context->flags_stack.auto_pop) { ui_pop_flags(); context->flags_stack.auto_pop = false; };
    if (context->seed_key_stack.auto_pop) { ui_pop_seed_key(); context->seed_key_stack.auto_pop = false; };
    if (context->fixed_x_stack.auto_pop) { ui_pop_fixed_x(); context->fixed_x_stack.auto_pop = false; };
    if (context->fixed_y_stack.auto_pop) { ui_pop_fixed_y(); context->fixed_y_stack.auto_pop = false; };
    if (context->fixed_width_stack.auto_pop) { ui_pop_fixed_width(); context->fixed_width_stack.auto_pop = false; };
    if (context->fixed_height_stack.auto_pop) { ui_pop_fixed_height(); context->fixed_height_stack.auto_pop = false; };
    if (context->width_stack.auto_pop) { ui_pop_width(); context->width_stack.auto_pop = false; };
    if (context->height_stack.auto_pop) { ui_pop_height(); context->height_stack.auto_pop = false; };
    if (context->padding_stack.auto_pop) { ui_pop_padding(); context->padding_stack.auto_pop = false; };
    if (context->layout_dir_stack.auto_pop) { ui_pop_layout_dir(); context->layout_dir_stack.auto_pop = false; };
    if (context->text_alignment_stack.auto_pop) { ui_pop_text_alignment(); context->text_alignment_stack.auto_pop = false; };
    if (context->rounding_00_stack.auto_pop) { ui_pop_rounding_00(); context->rounding_00_stack.auto_pop = false; };
    if (context->rounding_01_stack.auto_pop) { ui_pop_rounding_01(); context->rounding_01_stack.auto_pop = false; };
    if (context->rounding_10_stack.auto_pop) { ui_pop_rounding_10(); context->rounding_10_stack.auto_pop = false; };
    if (context->rounding_11_stack.auto_pop) { ui_pop_rounding_11(); context->rounding_11_stack.auto_pop = false; };
    if (context->border_size_stack.auto_pop) { ui_pop_border_size(); context->border_size_stack.auto_pop = false; };
    if (context->shadow_size_stack.auto_pop) { ui_pop_shadow_size(); context->shadow_size_stack.auto_pop = false; };
    if (context->texture_stack.auto_pop) { ui_pop_texture(); context->texture_stack.auto_pop = false; };
    if (context->font_stack.auto_pop) { ui_pop_font(); context->font_stack.auto_pop = false; };
    if (context->font_size_stack.auto_pop) { ui_pop_font_size(); context->font_size_stack.auto_pop = false; };
    if (context->color_background_stack.auto_pop) { ui_pop_color_background(); context->color_background_stack.auto_pop = false; };
    if (context->color_text_stack.auto_pop) { ui_pop_color_text(); context->color_text_stack.auto_pop = false; };
    if (context->color_border_stack.auto_pop) { ui_pop_color_border(); context->color_border_stack.auto_pop = false; };
    if (context->color_shadow_stack.auto_pop) { ui_pop_color_shadow(); context->color_shadow_stack.auto_pop = false; };
    if (context->color_hover_stack.auto_pop) { ui_pop_color_hover(); context->color_hover_stack.auto_pop = false; };
    if (context->color_active_stack.auto_pop) { ui_pop_color_active(); context->color_active_stack.auto_pop = false; };
    if (context->color_accent_stack.auto_pop) { ui_pop_color_accent(); context->color_accent_stack.auto_pop = false; };
    if (context->hover_cursor_stack.auto_pop) { ui_pop_hover_cursor(); context->hover_cursor_stack.auto_pop = false; };
    
}

// parent
function ui_frame_t*
ui_top_parent() {
    return ui_active()->parent_stack.top->v;
}

function ui_frame_t*
ui_push_parent(ui_frame_t* v) {
    ui_context_t* context = ui_active();
    ui_parent_node_t* node = context->parent_stack.free;
    if (node != nullptr) {
        stack_pop(context->parent_stack.free);
    } else {
        node = (ui_parent_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_parent_node_t));
    }
    ui_frame_t* old_value = context->parent_stack.top->v; node->v = v;
    stack_push(context->parent_stack.top, node);
    context->parent_stack.auto_pop = false;
    return old_value;
}

function ui_frame_t*
ui_pop_parent() {
    ui_context_t* context = ui_active();
    ui_parent_node_t* popped = context->parent_stack.top;
    if (popped != nullptr) {
        stack_pop(context->parent_stack.top);
        stack_push(context->parent_stack.free, popped);
        context->parent_stack.auto_pop = false;
    }
    return popped->v;
}

function ui_frame_t*
ui_set_next_parent(ui_frame_t* v) {
    ui_context_t* context = ui_active();
    ui_parent_node_t* node = context->parent_stack.free;
    if (node != nullptr) {
        stack_pop(context->parent_stack.free);
    } else {
        node = (ui_parent_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_parent_node_t));
    }
    ui_frame_t* old_value = context->parent_stack.top->v;
    node->v = v;
    stack_push(context->parent_stack.top, node);
    context->parent_stack.auto_pop = true;
    return old_value;
}

// flags
function ui_frame_flags
ui_top_flags() {
    return ui_active()->flags_stack.top->v;
}

function ui_frame_flags
ui_push_flags(ui_frame_flags v) {
    ui_context_t* context = ui_active();
    ui_flags_node_t* node = context->flags_stack.free;
    if (node != nullptr) {
        stack_pop(context->flags_stack.free);
    } else {
        node = (ui_flags_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_flags_node_t));
    }
    ui_frame_flags old_value = context->flags_stack.top->v; node->v = v;
    stack_push(context->flags_stack.top, node);
    context->flags_stack.auto_pop = false;
    return old_value;
}

function ui_frame_flags
ui_pop_flags() {
    ui_context_t* context = ui_active();
    ui_flags_node_t* popped = context->flags_stack.top;
    if (popped != nullptr) {
        stack_pop(context->flags_stack.top);
        stack_push(context->flags_stack.free, popped);
        context->flags_stack.auto_pop = false;
    }
    return popped->v;
}

function ui_frame_flags
ui_set_next_flags(ui_frame_flags v) {
    ui_context_t* context = ui_active();
    ui_flags_node_t* node = context->flags_stack.free;
    if (node != nullptr) {
        stack_pop(context->flags_stack.free);
    } else {
        node = (ui_flags_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_flags_node_t));
    }
    ui_frame_flags old_value = context->flags_stack.top->v;
    node->v = v;
    stack_push(context->flags_stack.top, node);
    context->flags_stack.auto_pop = true;
    return old_value;
}

// seed key
function ui_key_t
ui_top_seed_key() {
    return ui_active()->seed_key_stack.top->v;
}

function ui_key_t
ui_push_seed_key(ui_key_t v) {
    ui_context_t* context = ui_active();
    ui_seed_key_node_t* node = context->seed_key_stack.free;
    if (node != nullptr) {
        stack_pop(context->seed_key_stack.free);
    } else {
        node = (ui_seed_key_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_seed_key_node_t));
    }
    ui_key_t old_value = context->seed_key_stack.top->v; node->v = v;
    stack_push(context->seed_key_stack.top, node);
    context->seed_key_stack.auto_pop = false;
    return old_value;
}

function ui_key_t
ui_pop_seed_key() {
    ui_context_t* context = ui_active();
    ui_seed_key_node_t* popped = context->seed_key_stack.top;
    if (popped != nullptr) {
        stack_pop(context->seed_key_stack.top);
        stack_push(context->seed_key_stack.free, popped);
        context->seed_key_stack.auto_pop = false;
    }
    return popped->v;
}

function ui_key_t
ui_set_next_seed_key(ui_key_t v) {
    ui_context_t* context = ui_active();
    ui_seed_key_node_t* node = context->seed_key_stack.free;
    if (node != nullptr) {
        stack_pop(context->seed_key_stack.free);
    } else {
        node = (ui_seed_key_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_seed_key_node_t));
    }
    ui_key_t old_value = context->seed_key_stack.top->v;
    node->v = v;
    stack_push(context->seed_key_stack.top, node);
    context->seed_key_stack.auto_pop = true;
    return old_value;
}




// fixed_x
function f32
ui_top_fixed_x() {
    return ui_active()->fixed_x_stack.top->v;
}

function f32
ui_push_fixed_x(f32 v) {
    ui_context_t* context = ui_active();
    ui_fixed_x_node_t* node = context->fixed_x_stack.free;
    if (node != nullptr) {
        stack_pop(context->fixed_x_stack.free);
    } else {
        node = (ui_fixed_x_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_fixed_x_node_t));
    }
    f32 old_value = context->fixed_x_stack.top->v; node->v = v;
    stack_push(context->fixed_x_stack.top, node);
    context->fixed_x_stack.auto_pop = false;
    return old_value;
}

function f32
ui_pop_fixed_x() {
    ui_context_t* context = ui_active();
    ui_fixed_x_node_t* popped = context->fixed_x_stack.top;
    if (popped != nullptr) {
        stack_pop(context->fixed_x_stack.top);
        stack_push(context->fixed_x_stack.free, popped);
        context->fixed_x_stack.auto_pop = false;
    }
    return popped->v;
}

function f32
ui_set_next_fixed_x(f32 v) {
    ui_context_t* context = ui_active();
    ui_fixed_x_node_t* node = context->fixed_x_stack.free;
    if (node != nullptr) {
        stack_pop(context->fixed_x_stack.free);
    } else {
        node = (ui_fixed_x_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_fixed_x_node_t));
    }
    f32 old_value = context->fixed_x_stack.top->v;
    node->v = v;
    stack_push(context->fixed_x_stack.top, node);
    context->fixed_x_stack.auto_pop = true;
    return old_value;
}



// fixed_y
function f32
ui_top_fixed_y() {
    return ui_active()->fixed_y_stack.top->v;
}

function f32
ui_push_fixed_y(f32 v) {
    ui_context_t* context = ui_active();
    ui_fixed_y_node_t* node = context->fixed_y_stack.free;
    if (node != nullptr) {
        stack_pop(context->fixed_y_stack.free);
    } else {
        node = (ui_fixed_y_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_fixed_y_node_t));
    }
    f32 old_value = context->fixed_y_stack.top->v; node->v = v;
    stack_push(context->fixed_y_stack.top, node);
    context->fixed_y_stack.auto_pop = false;
    return old_value;
}

function f32
ui_pop_fixed_y() {
    ui_context_t* context = ui_active();
    ui_fixed_y_node_t* popped = context->fixed_y_stack.top;
    if (popped != nullptr) {
        stack_pop(context->fixed_y_stack.top);
        stack_push(context->fixed_y_stack.free, popped);
        context->fixed_y_stack.auto_pop = false;
    }
    return popped->v;
}

function f32
ui_set_next_fixed_y(f32 v) {
    ui_context_t* context = ui_active();
    ui_fixed_y_node_t* node = context->fixed_y_stack.free;
    if (node != nullptr) {
        stack_pop(context->fixed_y_stack.free);
    } else {
        node = (ui_fixed_y_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_fixed_y_node_t));
    }
    f32 old_value = context->fixed_y_stack.top->v;
    node->v = v;
    stack_push(context->fixed_y_stack.top, node);
    context->fixed_y_stack.auto_pop = true;
    return old_value;
}


// fixed_width
function f32
ui_top_fixed_width() {
    return ui_active()->fixed_width_stack.top->v;
}

function f32
ui_push_fixed_width(f32 v) {
    ui_context_t* context = ui_active();
    ui_fixed_width_node_t* node = context->fixed_width_stack.free;
    if (node != nullptr) {
        stack_pop(context->fixed_width_stack.free);
    } else {
        node = (ui_fixed_width_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_fixed_width_node_t));
    }
    f32 old_value = context->fixed_width_stack.top->v; node->v = v;
    stack_push(context->fixed_width_stack.top, node);
    context->fixed_width_stack.auto_pop = false;
    return old_value;
}

function f32
ui_pop_fixed_width() {
    ui_context_t* context = ui_active();
    ui_fixed_width_node_t* popped = context->fixed_width_stack.top;
    if (popped != nullptr) {
        stack_pop(context->fixed_width_stack.top);
        stack_push(context->fixed_width_stack.free, popped);
        context->fixed_width_stack.auto_pop = false;
    }
    return popped->v;
}

function f32
ui_set_next_fixed_width(f32 v) {
    ui_context_t* context = ui_active();
    ui_fixed_width_node_t* node = context->fixed_width_stack.free;
    if (node != nullptr) {
        stack_pop(context->fixed_width_stack.free);
    } else {
        node = (ui_fixed_width_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_fixed_width_node_t));
    }
    f32 old_value = context->fixed_width_stack.top->v;
    node->v = v;
    stack_push(context->fixed_width_stack.top, node);
    context->fixed_width_stack.auto_pop = true;
    return old_value;
}


// fixed_height
function f32
ui_top_fixed_height() {
    return ui_active()->fixed_height_stack.top->v;
}

function f32
ui_push_fixed_height(f32 v) {
    ui_context_t* context = ui_active();
    ui_fixed_height_node_t* node = context->fixed_height_stack.free;
    if (node != nullptr) {
        stack_pop(context->fixed_height_stack.free);
    } else {
        node = (ui_fixed_height_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_fixed_height_node_t));
    }
    f32 old_value = context->fixed_height_stack.top->v; node->v = v;
    stack_push(context->fixed_height_stack.top, node);
    context->fixed_height_stack.auto_pop = false;
    return old_value;
}

function f32
ui_pop_fixed_height() {
    ui_context_t* context = ui_active();
    ui_fixed_height_node_t* popped = context->fixed_height_stack.top;
    if (popped != nullptr) {
        stack_pop(context->fixed_height_stack.top);
        stack_push(context->fixed_height_stack.free, popped);
        context->fixed_height_stack.auto_pop = false;
    }
    return popped->v;
}

function f32
ui_set_next_fixed_height(f32 v) {
    ui_context_t* context = ui_active();
    ui_fixed_height_node_t* node = context->fixed_height_stack.free;
    if (node != nullptr) {
        stack_pop(context->fixed_height_stack.free);
    } else {
        node = (ui_fixed_height_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_fixed_height_node_t));
    }
    f32 old_value = context->fixed_height_stack.top->v;
    node->v = v;
    stack_push(context->fixed_height_stack.top, node);
    context->fixed_height_stack.auto_pop = true;
    return old_value;
}


// width
function ui_size_t
ui_top_width() {
    return ui_active()->width_stack.top->v;
}

function ui_size_t
ui_push_width(ui_size_t v) {
    ui_context_t* context = ui_active();
    ui_width_node_t* node = context->width_stack.free;
    if (node != nullptr) {
        stack_pop(context->width_stack.free);
    } else {
        node = (ui_width_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_width_node_t));
    }
    ui_size_t old_value = context->width_stack.top->v; node->v = v;
    stack_push(context->width_stack.top, node);
    context->width_stack.auto_pop = false;
    return old_value;
}

function ui_size_t
ui_pop_width() {
    ui_context_t* context = ui_active();
    ui_width_node_t* popped = context->width_stack.top;
    if (popped != nullptr) {
        stack_pop(context->width_stack.top);
        stack_push(context->width_stack.free, popped);
        context->width_stack.auto_pop = false;
    }
    return popped->v;
}

function ui_size_t
ui_set_next_width(ui_size_t v) {
    ui_context_t* context = ui_active();
    ui_width_node_t* node = context->width_stack.free;
    if (node != nullptr) {
        stack_pop(context->width_stack.free);
    } else {
        node = (ui_width_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_width_node_t));
    }
    ui_size_t old_value = context->width_stack.top->v;
    node->v = v;
    stack_push(context->width_stack.top, node);
    context->width_stack.auto_pop = true;
    return old_value;
}

// height
function ui_size_t
ui_top_height() {
    return ui_active()->height_stack.top->v;
}

function ui_size_t
ui_push_height(ui_size_t v) {
    ui_context_t* context = ui_active();
    ui_height_node_t* node = context->height_stack.free;
    if (node != nullptr) {
        stack_pop(context->height_stack.free);
    } else {
        node = (ui_height_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_height_node_t));
    }
    ui_size_t old_value = context->height_stack.top->v; node->v = v;
    stack_push(context->height_stack.top, node);
    context->height_stack.auto_pop = false;
    return old_value;
}

function ui_size_t
ui_pop_height() {
    ui_context_t* context = ui_active();
    ui_height_node_t* popped = context->height_stack.top;
    if (popped != nullptr) {
        stack_pop(context->height_stack.top);
        stack_push(context->height_stack.free, popped);
        context->height_stack.auto_pop = false;
    }
    return popped->v;
}

function ui_size_t
ui_set_next_height(ui_size_t v) {
    ui_context_t* context = ui_active();
    ui_height_node_t* node = context->height_stack.free;
    if (node != nullptr) {
        stack_pop(context->height_stack.free);
    } else {
        node = (ui_height_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_height_node_t));
    }
    ui_size_t old_value = context->height_stack.top->v;
    node->v = v;
    stack_push(context->height_stack.top, node);
    context->height_stack.auto_pop = true;
    return old_value;
}

// padding
function f32
ui_top_padding() {
    return ui_active()->padding_stack.top->v;
}

function f32
ui_push_padding(f32 v) {
    ui_context_t* context = ui_active();
    ui_padding_node_t* node = context->padding_stack.free;
    if (node != nullptr) {
        stack_pop(context->padding_stack.free);
    } else {
        node = (ui_padding_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_padding_node_t));
    }
    f32 old_value = context->padding_stack.top->v; node->v = v;
    stack_push(context->padding_stack.top, node);
    context->padding_stack.auto_pop = false;
    return old_value;
}

function f32
ui_pop_padding() {
    ui_context_t* context = ui_active();
    ui_padding_node_t* popped = context->padding_stack.top;
    if (popped != nullptr) {
        stack_pop(context->padding_stack.top);
        stack_push(context->padding_stack.free, popped);
        context->padding_stack.auto_pop = false;
    }
    return popped->v;
}

function f32
ui_set_next_padding(f32 v) {
    ui_context_t* context = ui_active();
    ui_padding_node_t* node = context->padding_stack.free;
    if (node != nullptr) {
        stack_pop(context->padding_stack.free);
    } else {
        node = (ui_padding_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_padding_node_t));
    }
    f32 old_value = context->padding_stack.top->v;
    node->v = v;
    stack_push(context->padding_stack.top, node);
    context->padding_stack.auto_pop = true;
    return old_value;
}


// layout_dir
function ui_dir
ui_top_layout_dir() {
    return ui_active()->layout_dir_stack.top->v;
}

function ui_dir
ui_push_layout_dir(ui_dir v) {
    ui_context_t* context = ui_active();
    ui_layout_dir_node_t* node = context->layout_dir_stack.free;
    if (node != nullptr) {
        stack_pop(context->layout_dir_stack.free);
    } else {
        node = (ui_layout_dir_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_layout_dir_node_t));
    }
    ui_dir old_value = context->layout_dir_stack.top->v; node->v = v;
    stack_push(context->layout_dir_stack.top, node);
    context->layout_dir_stack.auto_pop = false;
    return old_value;
}

function ui_dir
ui_pop_layout_dir() {
    ui_context_t* context = ui_active();
    ui_layout_dir_node_t* popped = context->layout_dir_stack.top;
    if (popped != nullptr) {
        stack_pop(context->layout_dir_stack.top);
        stack_push(context->layout_dir_stack.free, popped);
        context->layout_dir_stack.auto_pop = false;
    }
    return popped->v;
}

function ui_dir
ui_set_next_layout_dir(ui_dir v) {
    ui_context_t* context = ui_active();
    ui_layout_dir_node_t* node = context->layout_dir_stack.free;
    if (node != nullptr) {
        stack_pop(context->layout_dir_stack.free);
    } else {
        node = (ui_layout_dir_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_layout_dir_node_t));
    }
    ui_dir old_value = context->layout_dir_stack.top->v;
    node->v = v;
    stack_push(context->layout_dir_stack.top, node);
    context->layout_dir_stack.auto_pop = true;
    return old_value;
}



// text_alignment
function ui_text_alignment
ui_top_text_alignment() {
    return ui_active()->text_alignment_stack.top->v;
}

function ui_text_alignment
ui_push_text_alignment(ui_text_alignment v) {
    ui_context_t* context = ui_active();
    ui_text_alignment_node_t* node = context->text_alignment_stack.free;
    if (node != nullptr) {
        stack_pop(context->text_alignment_stack.free);
    } else {
        node = (ui_text_alignment_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_text_alignment_node_t));
    }
    ui_text_alignment old_value = context->text_alignment_stack.top->v; node->v = v;
    stack_push(context->text_alignment_stack.top, node);
    context->text_alignment_stack.auto_pop = false;
    return old_value;
}

function ui_text_alignment
ui_pop_text_alignment() {
    ui_context_t* context = ui_active();
    ui_text_alignment_node_t* popped = context->text_alignment_stack.top;
    if (popped != nullptr) {
        stack_pop(context->text_alignment_stack.top);
        stack_push(context->text_alignment_stack.free, popped);
        context->text_alignment_stack.auto_pop = false;
    }
    return popped->v;
}

function ui_text_alignment
ui_set_next_text_alignment(ui_text_alignment v) {
    ui_context_t* context = ui_active();
    ui_text_alignment_node_t* node = context->text_alignment_stack.free;
    if (node != nullptr) {
        stack_pop(context->text_alignment_stack.free);
    } else {
        node = (ui_text_alignment_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_text_alignment_node_t));
    }
    ui_text_alignment old_value = context->text_alignment_stack.top->v;
    node->v = v;
    stack_push(context->text_alignment_stack.top, node);
    context->text_alignment_stack.auto_pop = true;
    return old_value;
}



// rounding_00
function f32
ui_top_rounding_00() {
    return ui_active()->rounding_00_stack.top->v;
}

function f32
ui_push_rounding_00(f32 v) {
    ui_context_t* context = ui_active();
    ui_rounding_00_node_t* node = context->rounding_00_stack.free;
    if (node != nullptr) {
        stack_pop(context->rounding_00_stack.free);
    } else {
        node = (ui_rounding_00_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_rounding_00_node_t));
    }
    f32 old_value = context->rounding_00_stack.top->v; node->v = v;
    stack_push(context->rounding_00_stack.top, node);
    context->rounding_00_stack.auto_pop = false;
    return old_value;
}

function f32
ui_pop_rounding_00() {
    ui_context_t* context = ui_active();
    ui_rounding_00_node_t* popped = context->rounding_00_stack.top;
    if (popped != nullptr) {
        stack_pop(context->rounding_00_stack.top);
        stack_push(context->rounding_00_stack.free, popped);
        context->rounding_00_stack.auto_pop = false;
    }
    return popped->v;
}

function f32
ui_set_next_rounding_00(f32 v) {
    ui_context_t* context = ui_active();
    ui_rounding_00_node_t* node = context->rounding_00_stack.free;
    if (node != nullptr) {
        stack_pop(context->rounding_00_stack.free);
    } else {
        node = (ui_rounding_00_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_rounding_00_node_t));
    }
    f32 old_value = context->rounding_00_stack.top->v;
    node->v = v;
    stack_push(context->rounding_00_stack.top, node);
    context->rounding_00_stack.auto_pop = true;
    return old_value;
}

// rounding_01
function f32
ui_top_rounding_01() {
    return ui_active()->rounding_01_stack.top->v;
}

function f32
ui_push_rounding_01(f32 v) {
    ui_context_t* context = ui_active();
    ui_rounding_01_node_t* node = context->rounding_01_stack.free;
    if (node != nullptr) {
        stack_pop(context->rounding_01_stack.free);
    } else {
        node = (ui_rounding_01_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_rounding_01_node_t));
    }
    f32 old_value = context->rounding_01_stack.top->v; node->v = v;
    stack_push(context->rounding_01_stack.top, node);
    context->rounding_01_stack.auto_pop = false;
    return old_value;
}

function f32
ui_pop_rounding_01() {
    ui_context_t* context = ui_active();
    ui_rounding_01_node_t* popped = context->rounding_01_stack.top;
    if (popped != nullptr) {
        stack_pop(context->rounding_01_stack.top);
        stack_push(context->rounding_01_stack.free, popped);
        context->rounding_01_stack.auto_pop = false;
    }
    return popped->v;
}

function f32
ui_set_next_rounding_01(f32 v) {
    ui_context_t* context = ui_active();
    ui_rounding_01_node_t* node = context->rounding_01_stack.free;
    if (node != nullptr) {
        stack_pop(context->rounding_01_stack.free);
    } else {
        node = (ui_rounding_01_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_rounding_01_node_t));
    }
    f32 old_value = context->rounding_01_stack.top->v;
    node->v = v;
    stack_push(context->rounding_01_stack.top, node);
    context->rounding_01_stack.auto_pop = true;
    return old_value;
}

// rounding_10
function f32
ui_top_rounding_10() {
    return ui_active()->rounding_10_stack.top->v;
}

function f32
ui_push_rounding_10(f32 v) {
    ui_context_t* context = ui_active();
    ui_rounding_10_node_t* node = context->rounding_10_stack.free;
    if (node != nullptr) {
        stack_pop(context->rounding_10_stack.free);
    } else {
        node = (ui_rounding_10_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_rounding_10_node_t));
    }
    f32 old_value = context->rounding_10_stack.top->v; node->v = v;
    stack_push(context->rounding_10_stack.top, node);
    context->rounding_10_stack.auto_pop = false;
    return old_value;
}

function f32
ui_pop_rounding_10() {
    ui_context_t* context = ui_active();
    ui_rounding_10_node_t* popped = context->rounding_10_stack.top;
    if (popped != nullptr) {
        stack_pop(context->rounding_10_stack.top);
        stack_push(context->rounding_10_stack.free, popped);
        context->rounding_10_stack.auto_pop = false;
    }
    return popped->v;
}

function f32
ui_set_next_rounding_10(f32 v) {
    ui_context_t* context = ui_active();
    ui_rounding_10_node_t* node = context->rounding_10_stack.free;
    if (node != nullptr) {
        stack_pop(context->rounding_10_stack.free);
    } else {
        node = (ui_rounding_10_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_rounding_10_node_t));
    }
    f32 old_value = context->rounding_10_stack.top->v;
    node->v = v;
    stack_push(context->rounding_10_stack.top, node);
    context->rounding_10_stack.auto_pop = true;
    return old_value;
}

// rounding_11
function f32
ui_top_rounding_11() {
    return ui_active()->rounding_11_stack.top->v;
}

function f32
ui_push_rounding_11(f32 v) {
    ui_context_t* context = ui_active();
    ui_rounding_11_node_t* node = context->rounding_11_stack.free;
    if (node != nullptr) {
        stack_pop(context->rounding_11_stack.free);
    } else {
        node = (ui_rounding_11_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_rounding_11_node_t));
    }
    f32 old_value = context->rounding_11_stack.top->v; node->v = v;
    stack_push(context->rounding_11_stack.top, node);
    context->rounding_11_stack.auto_pop = false;
    return old_value;
}

function f32
ui_pop_rounding_11() {
    ui_context_t* context = ui_active();
    ui_rounding_11_node_t* popped = context->rounding_11_stack.top;
    if (popped != nullptr) {
        stack_pop(context->rounding_11_stack.top);
        stack_push(context->rounding_11_stack.free, popped);
        context->rounding_11_stack.auto_pop = false;
    }
    return popped->v;
}

function f32
ui_set_next_rounding_11(f32 v) {
    ui_context_t* context = ui_active();
    ui_rounding_11_node_t* node = context->rounding_11_stack.free;
    if (node != nullptr) {
        stack_pop(context->rounding_11_stack.free);
    } else {
        node = (ui_rounding_11_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_rounding_11_node_t));
    }
    f32 old_value = context->rounding_11_stack.top->v;
    node->v = v;
    stack_push(context->rounding_11_stack.top, node);
    context->rounding_11_stack.auto_pop = true;
    return old_value;
}

// border_size
function f32
ui_top_border_size() {
    return ui_active()->border_size_stack.top->v;
}

function f32
ui_push_border_size(f32 v) {
    ui_context_t* context = ui_active();
    ui_border_size_node_t* node = context->border_size_stack.free;
    if (node != nullptr) {
        stack_pop(context->border_size_stack.free);
    } else {
        node = (ui_border_size_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_border_size_node_t));
    }
    f32 old_value = context->border_size_stack.top->v; node->v = v;
    stack_push(context->border_size_stack.top, node);
    context->border_size_stack.auto_pop = false;
    return old_value;
}

function f32
ui_pop_border_size() {
    ui_context_t* context = ui_active();
    ui_border_size_node_t* popped = context->border_size_stack.top;
    if (popped != nullptr) {
        stack_pop(context->border_size_stack.top);
        stack_push(context->border_size_stack.free, popped);
        context->border_size_stack.auto_pop = false;
    }
    return popped->v;
}

function f32
ui_set_next_border_size(f32 v) {
    ui_context_t* context = ui_active();
    ui_border_size_node_t* node = context->border_size_stack.free;
    if (node != nullptr) {
        stack_pop(context->border_size_stack.free);
    } else {
        node = (ui_border_size_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_border_size_node_t));
    }
    f32 old_value = context->border_size_stack.top->v;
    node->v = v;
    stack_push(context->border_size_stack.top, node);
    context->border_size_stack.auto_pop = true;
    return old_value;
}

// shadow_size
function f32
ui_top_shadow_size() {
    return ui_active()->shadow_size_stack.top->v;
}

function f32
ui_push_shadow_size(f32 v) {
    ui_context_t* context = ui_active();
    ui_shadow_size_node_t* node = context->shadow_size_stack.free;
    if (node != nullptr) {
        stack_pop(context->shadow_size_stack.free);
    } else {
        node = (ui_shadow_size_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_shadow_size_node_t));
    }
    f32 old_value = context->shadow_size_stack.top->v; node->v = v;
    stack_push(context->shadow_size_stack.top, node);
    context->shadow_size_stack.auto_pop = false;
    return old_value;
}

function f32
ui_pop_shadow_size() {
    ui_context_t* context = ui_active();
    ui_shadow_size_node_t* popped = context->shadow_size_stack.top;
    if (popped != nullptr) {
        stack_pop(context->shadow_size_stack.top);
        stack_push(context->shadow_size_stack.free, popped);
        context->shadow_size_stack.auto_pop = false;
    }
    return popped->v;
}

function f32
ui_set_next_shadow_size(f32 v) {
    ui_context_t* context = ui_active();
    ui_shadow_size_node_t* node = context->shadow_size_stack.free;
    if (node != nullptr) {
        stack_pop(context->shadow_size_stack.free);
    } else {
        node = (ui_shadow_size_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_shadow_size_node_t));
    }
    f32 old_value = context->shadow_size_stack.top->v;
    node->v = v;
    stack_push(context->shadow_size_stack.top, node);
    context->shadow_size_stack.auto_pop = true;
    return old_value;
}

// texture
function gfx_handle_t
ui_top_texture() {
    return ui_active()->texture_stack.top->v;
}

function gfx_handle_t
ui_push_texture(gfx_handle_t v) {
    ui_context_t* context = ui_active();
    ui_texture_node_t* node = context->texture_stack.free;
    if (node != nullptr) {
        stack_pop(context->texture_stack.free);
    } else {
        node = (ui_texture_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_texture_node_t));
    }
    gfx_handle_t old_value = context->texture_stack.top->v; node->v = v;
    stack_push(context->texture_stack.top, node);
    context->texture_stack.auto_pop = false;
    return old_value;
}

function gfx_handle_t
ui_pop_texture() {
    ui_context_t* context = ui_active();
    ui_texture_node_t* popped = context->texture_stack.top;
    if (popped != nullptr) {
        stack_pop(context->texture_stack.top);
        stack_push(context->texture_stack.free, popped);
        context->texture_stack.auto_pop = false;
    }
    return popped->v;
}

function gfx_handle_t
ui_set_next_texture(gfx_handle_t v) {
    ui_context_t* context = ui_active();
    ui_texture_node_t* node = context->texture_stack.free;
    if (node != nullptr) {
        stack_pop(context->texture_stack.free);
    } else {
        node = (ui_texture_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_texture_node_t));
    }
    gfx_handle_t old_value = context->texture_stack.top->v;
    node->v = v;
    stack_push(context->texture_stack.top, node);
    context->texture_stack.auto_pop = true;
    return old_value;
}

// font
function font_handle_t
ui_top_font() {
    return ui_active()->font_stack.top->v;
}

function font_handle_t
ui_push_font(font_handle_t v) {
    ui_context_t* context = ui_active();
    ui_font_node_t* node = context->font_stack.free;
    if (node != nullptr) {
        stack_pop(context->font_stack.free);
    } else {
        node = (ui_font_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_font_node_t));
    }
    font_handle_t old_value = context->font_stack.top->v; node->v = v;
    stack_push(context->font_stack.top, node);
    context->font_stack.auto_pop = false;
    return old_value;
}

function font_handle_t
ui_pop_font() {
    ui_context_t* context = ui_active();
    ui_font_node_t* popped = context->font_stack.top;
    if (popped != nullptr) {
        stack_pop(context->font_stack.top);
        stack_push(context->font_stack.free, popped);
        context->font_stack.auto_pop = false;
    }
    return popped->v;
}

function font_handle_t
ui_set_next_font(font_handle_t v) {
    ui_context_t* context = ui_active();
    ui_font_node_t* node = context->font_stack.free;
    if (node != nullptr) {
        stack_pop(context->font_stack.free);
    } else {
        node = (ui_font_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_font_node_t));
    }
    font_handle_t old_value = context->font_stack.top->v;
    node->v = v;
    stack_push(context->font_stack.top, node);
    context->font_stack.auto_pop = true;
    return old_value;
}

// font_size
function f32
ui_top_font_size() {
    return ui_active()->font_size_stack.top->v;
}

function f32
ui_push_font_size(f32 v) {
    ui_context_t* context = ui_active();
    ui_font_size_node_t* node = context->font_size_stack.free;
    if (node != nullptr) {
        stack_pop(context->font_size_stack.free);
    } else {
        node = (ui_font_size_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_font_size_node_t));
    }
    f32 old_value = context->font_size_stack.top->v; node->v = v;
    stack_push(context->font_size_stack.top, node);
    context->font_size_stack.auto_pop = false;
    return old_value;
}

function f32
ui_pop_font_size() {
    ui_context_t* context = ui_active();
    ui_font_size_node_t* popped = context->font_size_stack.top;
    if (popped != nullptr) {
        stack_pop(context->font_size_stack.top);
        stack_push(context->font_size_stack.free, popped);
        context->font_size_stack.auto_pop = false;
    }
    return popped->v;
}

function f32
ui_set_next_font_size(f32 v) {
    ui_context_t* context = ui_active();
    ui_font_size_node_t* node = context->font_size_stack.free;
    if (node != nullptr) {
        stack_pop(context->font_size_stack.free);
    } else {
        node = (ui_font_size_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_font_size_node_t));
    }
    f32 old_value = context->font_size_stack.top->v;
    node->v = v;
    stack_push(context->font_size_stack.top, node);
    context->font_size_stack.auto_pop = true;
    return old_value;
}




// color_background
function color_t
ui_top_color_background() {
    return ui_active()->color_background_stack.top->v;
}

function color_t
ui_push_color_background(color_t v) {
    ui_context_t* context = ui_active();
    ui_color_background_node_t* node = context->color_background_stack.free;
    if (node != nullptr) {
        stack_pop(context->color_background_stack.free);
    } else {
        node = (ui_color_background_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_color_background_node_t));
    }
    color_t old_value = context->color_background_stack.top->v; node->v = v;
    stack_push(context->color_background_stack.top, node);
    context->color_background_stack.auto_pop = false;
    return old_value;
}

function color_t
ui_pop_color_background() {
    ui_context_t* context = ui_active();
    ui_color_background_node_t* popped = context->color_background_stack.top;
    if (popped != nullptr) {
        stack_pop(context->color_background_stack.top);
        stack_push(context->color_background_stack.free, popped);
        context->color_background_stack.auto_pop = false;
    }
    return popped->v;
}

function color_t
ui_set_next_color_background(color_t v) {
    ui_context_t* context = ui_active();
    ui_color_background_node_t* node = context->color_background_stack.free;
    if (node != nullptr) {
        stack_pop(context->color_background_stack.free);
    } else {
        node = (ui_color_background_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_color_background_node_t));
    }
    color_t old_value = context->color_background_stack.top->v;
    node->v = v;
    stack_push(context->color_background_stack.top, node);
    context->color_background_stack.auto_pop = true;
    return old_value;
}


// color_text
function color_t
ui_top_color_text() {
    return ui_active()->color_text_stack.top->v;
}

function color_t
ui_push_color_text(color_t v) {
    ui_context_t* context = ui_active();
    ui_color_text_node_t* node = context->color_text_stack.free;
    if (node != nullptr) {
        stack_pop(context->color_text_stack.free);
    } else {
        node = (ui_color_text_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_color_text_node_t));
    }
    color_t old_value = context->color_text_stack.top->v; node->v = v;
    stack_push(context->color_text_stack.top, node);
    context->color_text_stack.auto_pop = false;
    return old_value;
}

function color_t
ui_pop_color_text() {
    ui_context_t* context = ui_active();
    ui_color_text_node_t* popped = context->color_text_stack.top;
    if (popped != nullptr) {
        stack_pop(context->color_text_stack.top);
        stack_push(context->color_text_stack.free, popped);
        context->color_text_stack.auto_pop = false;
    }
    return popped->v;
}

function color_t
ui_set_next_color_text(color_t v) {
    ui_context_t* context = ui_active();
    ui_color_text_node_t* node = context->color_text_stack.free;
    if (node != nullptr) {
        stack_pop(context->color_text_stack.free);
    } else {
        node = (ui_color_text_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_color_text_node_t));
    }
    color_t old_value = context->color_text_stack.top->v;
    node->v = v;
    stack_push(context->color_text_stack.top, node);
    context->color_text_stack.auto_pop = true;
    return old_value;
}

// color_border
function color_t
ui_top_color_border() {
    return ui_active()->color_border_stack.top->v;
}

function color_t
ui_push_color_border(color_t v) {
    ui_context_t* context = ui_active();
    ui_color_border_node_t* node = context->color_border_stack.free;
    if (node != nullptr) {
        stack_pop(context->color_border_stack.free);
    } else {
        node = (ui_color_border_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_color_border_node_t));
    }
    color_t old_value = context->color_border_stack.top->v; node->v = v;
    stack_push(context->color_border_stack.top, node);
    context->color_border_stack.auto_pop = false;
    return old_value;
}

function color_t
ui_pop_color_border() {
    ui_context_t* context = ui_active();
    ui_color_border_node_t* popped = context->color_border_stack.top;
    if (popped != nullptr) {
        stack_pop(context->color_border_stack.top);
        stack_push(context->color_border_stack.free, popped);
        context->color_border_stack.auto_pop = false;
    }
    return popped->v;
}

function color_t
ui_set_next_color_border(color_t v) {
    ui_context_t* context = ui_active();
    ui_color_border_node_t* node = context->color_border_stack.free;
    if (node != nullptr) {
        stack_pop(context->color_border_stack.free);
    } else {
        node = (ui_color_border_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_color_border_node_t));
    }
    color_t old_value = context->color_border_stack.top->v;
    node->v = v;
    stack_push(context->color_border_stack.top, node);
    context->color_border_stack.auto_pop = true;
    return old_value;
}



// color_shadow
function color_t
ui_top_color_shadow() {
    return ui_active()->color_shadow_stack.top->v;
}

function color_t
ui_push_color_shadow(color_t v) {
    ui_context_t* context = ui_active();
    ui_color_shadow_node_t* node = context->color_shadow_stack.free;
    if (node != nullptr) {
        stack_pop(context->color_shadow_stack.free);
    } else {
        node = (ui_color_shadow_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_color_shadow_node_t));
    }
    color_t old_value = context->color_shadow_stack.top->v; node->v = v;
    stack_push(context->color_shadow_stack.top, node);
    context->color_shadow_stack.auto_pop = false;
    return old_value;
}

function color_t
ui_pop_color_shadow() {
    ui_context_t* context = ui_active();
    ui_color_shadow_node_t* popped = context->color_shadow_stack.top;
    if (popped != nullptr) {
        stack_pop(context->color_shadow_stack.top);
        stack_push(context->color_shadow_stack.free, popped);
        context->color_shadow_stack.auto_pop = false;
    }
    return popped->v;
}

function color_t
ui_set_next_color_shadow(color_t v) {
    ui_context_t* context = ui_active();
    ui_color_shadow_node_t* node = context->color_shadow_stack.free;
    if (node != nullptr) {
        stack_pop(context->color_shadow_stack.free);
    } else {
        node = (ui_color_shadow_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_color_shadow_node_t));
    }
    color_t old_value = context->color_shadow_stack.top->v;
    node->v = v;
    stack_push(context->color_shadow_stack.top, node);
    context->color_shadow_stack.auto_pop = true;
    return old_value;
}



// color_hover
function color_t
ui_top_color_hover() {
    return ui_active()->color_hover_stack.top->v;
}

function color_t
ui_push_color_hover(color_t v) {
    ui_context_t* context = ui_active();
    ui_color_hover_node_t* node = context->color_hover_stack.free;
    if (node != nullptr) {
        stack_pop(context->color_hover_stack.free);
    } else {
        node = (ui_color_hover_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_color_hover_node_t));
    }
    color_t old_value = context->color_hover_stack.top->v; node->v = v;
    stack_push(context->color_hover_stack.top, node);
    context->color_hover_stack.auto_pop = false;
    return old_value;
}

function color_t
ui_pop_color_hover() {
    ui_context_t* context = ui_active();
    ui_color_hover_node_t* popped = context->color_hover_stack.top;
    if (popped != nullptr) {
        stack_pop(context->color_hover_stack.top);
        stack_push(context->color_hover_stack.free, popped);
        context->color_hover_stack.auto_pop = false;
    }
    return popped->v;
}

function color_t
ui_set_next_color_hover(color_t v) {
    ui_context_t* context = ui_active();
    ui_color_hover_node_t* node = context->color_hover_stack.free;
    if (node != nullptr) {
        stack_pop(context->color_hover_stack.free);
    } else {
        node = (ui_color_hover_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_color_hover_node_t));
    }
    color_t old_value = context->color_hover_stack.top->v;
    node->v = v;
    stack_push(context->color_hover_stack.top, node);
    context->color_hover_stack.auto_pop = true;
    return old_value;
}



// color_active
function color_t
ui_top_color_active() {
    return ui_active()->color_active_stack.top->v;
}

function color_t
ui_push_color_active(color_t v) {
    ui_context_t* context = ui_active();
    ui_color_active_node_t* node = context->color_active_stack.free;
    if (node != nullptr) {
        stack_pop(context->color_active_stack.free);
    } else {
        node = (ui_color_active_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_color_active_node_t));
    }
    color_t old_value = context->color_active_stack.top->v; node->v = v;
    stack_push(context->color_active_stack.top, node);
    context->color_active_stack.auto_pop = false;
    return old_value;
}

function color_t
ui_pop_color_active() {
    ui_context_t* context = ui_active();
    ui_color_active_node_t* popped = context->color_active_stack.top;
    if (popped != nullptr) {
        stack_pop(context->color_active_stack.top);
        stack_push(context->color_active_stack.free, popped);
        context->color_active_stack.auto_pop = false;
    }
    return popped->v;
}

function color_t
ui_set_next_color_active(color_t v) {
    ui_context_t* context = ui_active();
    ui_color_active_node_t* node = context->color_active_stack.free;
    if (node != nullptr) {
        stack_pop(context->color_active_stack.free);
    } else {
        node = (ui_color_active_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_color_active_node_t));
    }
    color_t old_value = context->color_active_stack.top->v;
    node->v = v;
    stack_push(context->color_active_stack.top, node);
    context->color_active_stack.auto_pop = true;
    return old_value;
}

// color_accent
function color_t
ui_top_color_accent() {
    return ui_active()->color_accent_stack.top->v;
}

function color_t
ui_push_color_accent(color_t v) {
    ui_context_t* context = ui_active();
    ui_color_accent_node_t* node = context->color_accent_stack.free;
    if (node != nullptr) {
        stack_pop(context->color_accent_stack.free);
    } else {
        node = (ui_color_accent_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_color_accent_node_t));
    }
    color_t old_value = context->color_accent_stack.top->v; node->v = v;
    stack_push(context->color_accent_stack.top, node);
    context->color_accent_stack.auto_pop = false;
    return old_value;
}

function color_t
ui_pop_color_accent() {
    ui_context_t* context = ui_active();
    ui_color_accent_node_t* popped = context->color_accent_stack.top;
    if (popped != nullptr) {
        stack_pop(context->color_accent_stack.top);
        stack_push(context->color_accent_stack.free, popped);
        context->color_accent_stack.auto_pop = false;
    }
    return popped->v;
}

function color_t
ui_set_next_color_accent(color_t v) {
    ui_context_t* context = ui_active();
    ui_color_accent_node_t* node = context->color_accent_stack.free;
    if (node != nullptr) {
        stack_pop(context->color_accent_stack.free);
    } else {
        node = (ui_color_accent_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_color_accent_node_t));
    }
    color_t old_value = context->color_accent_stack.top->v;
    node->v = v;
    stack_push(context->color_accent_stack.top, node);
    context->color_accent_stack.auto_pop = true;
    return old_value;
}

// hover_cursor
function os_cursor
ui_top_hover_cursor() {
    return ui_active()->hover_cursor_stack.top->v;
}

function os_cursor
ui_push_hover_cursor(os_cursor v) {
    ui_context_t* context = ui_active();
    ui_hover_cursor_node_t* node = context->hover_cursor_stack.free;
    if (node != nullptr) {
        stack_pop(context->hover_cursor_stack.free);
    } else {
        node = (ui_hover_cursor_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_hover_cursor_node_t));
    }
    os_cursor old_value = context->hover_cursor_stack.top->v; node->v = v;
    stack_push(context->hover_cursor_stack.top, node);
    context->hover_cursor_stack.auto_pop = false;
    return old_value;
}

function os_cursor
ui_pop_hover_cursor() {
    ui_context_t* context = ui_active();
    ui_hover_cursor_node_t* popped = context->hover_cursor_stack.top;
    if (popped != nullptr) {
        stack_pop(context->hover_cursor_stack.top);
        stack_push(context->hover_cursor_stack.free, popped);
        context->hover_cursor_stack.auto_pop = false;
    }
    return popped->v;
}

function os_cursor
ui_set_next_hover_cursor(os_cursor v) {
    ui_context_t* context = ui_active();
    ui_hover_cursor_node_t* node = context->hover_cursor_stack.free;
    if (node != nullptr) {
        stack_pop(context->hover_cursor_stack.free);
    } else {
        node = (ui_hover_cursor_node_t*)arena_alloc(ui_build_arena(), sizeof(ui_hover_cursor_node_t));
    }
    os_cursor old_value = context->hover_cursor_stack.top->v;
    node->v = v;
    stack_push(context->hover_cursor_stack.top, node);
    context->hover_cursor_stack.auto_pop = true;
    return old_value;
}


// group stacks

// size
function void
ui_push_size(ui_size_t width, ui_size_t height) {
    ui_push_width(width);
    ui_push_height(height);
}

function void
ui_pop_size() {
    ui_pop_width();
    ui_pop_height();
}

function void
ui_set_next_size(ui_size_t width, ui_size_t height) {
    ui_set_next_width(width);
    ui_set_next_height(height);
}

// rect
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


// rounding

function vec4_t
ui_top_rounding() {
    f32 r_00 = ui_top_rounding_00();
    f32 r_01 = ui_top_rounding_01();
    f32 r_10 = ui_top_rounding_10();
    f32 r_11 = ui_top_rounding_11();
    return vec4(r_00, r_01, r_10, r_11);
}

function vec4_t
ui_push_rounding(vec4_t rounding) {
    f32 r_00 = ui_push_rounding_00(rounding.x);
    f32 r_01 = ui_push_rounding_01(rounding.y);
    f32 r_10 = ui_push_rounding_10(rounding.z);
    f32 r_11 = ui_push_rounding_11(rounding.w);
    return vec4(r_00, r_01, r_10, r_11);
}

function vec4_t
ui_pop_rounding() {
    f32 r_00 = ui_pop_rounding_00();
    f32 r_01 = ui_pop_rounding_01();
    f32 r_10 = ui_pop_rounding_10();
    f32 r_11 = ui_pop_rounding_11();
    return vec4(r_00, r_01, r_10, r_11);
}

function vec4_t
ui_set_next_rounding(vec4_t rounding) {
    f32 r_00 = ui_set_next_rounding_00(rounding.x);
    f32 r_01 = ui_set_next_rounding_01(rounding.y);
    f32 r_10 = ui_set_next_rounding_10(rounding.z);
    f32 r_11 = ui_set_next_rounding_11(rounding.w);
    return vec4(r_00, r_01, r_10, r_11);
}

// color

function color_t
ui_top_color(ui_color var) {
    color_t result = { 0 };
    switch (var) {
        case ui_color_background: { result = ui_top_color_background(); break; }
        case ui_color_text: { result = ui_top_color_text(); break; }
        case ui_color_border: { result = ui_top_color_border(); break; }
        case ui_color_shadow: { result = ui_top_color_shadow(); break; }
        case ui_color_hover: { result = ui_top_color_hover(); break; }
        case ui_color_active: { result = ui_top_color_active(); break; }
        case ui_color_accent: { result = ui_top_color_accent(); break; }
    }
    return result;
}

function color_t 
ui_push_color(ui_color var, color_t color) {
    color_t result = { 0 };
    switch (var) {
        case ui_color_background: { result = ui_push_color_background(color); break; }
        case ui_color_text: { result = ui_push_color_text(color); break; }
        case ui_color_border: { result = ui_push_color_border(color); break; }
        case ui_color_shadow: { result = ui_push_color_shadow(color); break; }
        case ui_color_hover: { result = ui_push_color_hover(color); break; }
        case ui_color_active: { result = ui_push_color_active(color); break; }
        case ui_color_accent: { result = ui_push_color_accent(color); break; }
    }
    return result;
}

function color_t 
ui_pop_color(ui_color var) {
    color_t result = { 0 };
    switch (var) {
        case ui_color_background: { result = ui_pop_color_background(); break; }
        case ui_color_text: { result = ui_pop_color_text(); break; }
        case ui_color_border: { result = ui_pop_color_border(); break; }
        case ui_color_shadow: { result = ui_pop_color_shadow(); break; }
        case ui_color_hover: { result = ui_pop_color_hover(); break; }
        case ui_color_active: { result = ui_pop_color_active(); break; }
        case ui_color_accent: { result = ui_pop_color_accent(); break; }
    }
    return result;
}

function color_t 
ui_set_next_color(ui_color var, color_t color) {
    color_t result = { 0 };
    switch (var) {
        case ui_color_background: { result = ui_set_next_color_background(color); break; }
        case ui_color_text: { result = ui_set_next_color_text(color); break; }
        case ui_color_border: { result = ui_set_next_color_border(color); break; }
        case ui_color_shadow: { result = ui_set_next_color_shadow(color); break; }
        case ui_color_hover: { result = ui_set_next_color_hover(color); break; }
        case ui_color_active: { result = ui_set_next_color_active(color); break; }
        case ui_color_accent: { result = ui_set_next_color_accent(color); break; }
    }
    return result;
}

#endif // UI_CPP