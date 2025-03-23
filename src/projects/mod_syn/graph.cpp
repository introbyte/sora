// graph.cpp

#ifndef GRAPH_CPP
#define GRAPH_CPP

//- implementation 

//- graph functions 

function graph_t*
graph_create(os_handle_t window) {
    
    arena_t* arena = arena_create(megabytes(256));
    graph_t* graph = (graph_t*)arena_alloc(arena, sizeof(graph_t));
    
    graph->arena = arena;
    graph->window = window;
    
    graph->data_first = nullptr;
    graph->data_last = nullptr;
    graph->data_free = nullptr;
    
    graph->node_first = nullptr;
    graph->node_last = nullptr;
    graph->node_free = nullptr;
    
    graph->port_free = nullptr;
    
    graph->process_list_arena = arena_create(kilobytes(8));
    graph->process_list.first = nullptr;
    graph->process_list.last = nullptr;
    
    return graph;
}

function void
graph_release(graph_t* graph) {
    arena_release(graph->process_list_arena);
    arena_release(graph->arena);
}

function void
graph_build(graph_t* graph) {
    
    arena_clear(graph->process_list_arena);
    
    // topological sort
    temp_t scratch = scratch_begin();
    
    // create list of node
    // TODO: right now we process everything including
    // node that are not connected to anything, but 
    // maybe we shouldnt do that.
    
    graph_ts_node_t* node_first = nullptr;
    graph_ts_node_t* node_last = nullptr;
    for (graph_node_t* node = graph->node_first; node != nullptr; node = node->next) {
        graph_ts_node_t* ts_node = (graph_ts_node_t*)arena_alloc(scratch.arena, sizeof(graph_ts_node_t));
        
        ts_node->node = node;
        ts_node->in_degree = 0;
        
        // count in degree
        for (graph_port_t* port = node->port_first; port != nullptr; port = port->next) {
            if (port->port_type == graph_port_type_input && port->connected_port != nullptr) {
                ts_node->in_degree++;
            }
        }
        
        dll_push_back(node_first, node_last, ts_node);
        
    }
    
    // create queue
    graph_ts_queue_t* queue_first = nullptr;
    graph_ts_queue_t* queue_last = nullptr;
    
    // loop over all nodes..
    for (graph_ts_node_t* node = node_first; node != nullptr; node = node->next) {
        // ..and add all passes with 0 in-degree to the queue.
        if (node->in_degree == 0) {
            graph_ts_queue_t* queue_node = (graph_ts_queue_t*)arena_alloc(scratch.arena, sizeof(graph_ts_queue_t));
            queue_node->node = node;
            queue_push(queue_first, queue_last, queue_node);
        }
    }
    
    while (queue_first != nullptr) {
        
        // grab the top of the queue
        graph_ts_queue_t* queue_top = queue_first;
        queue_pop(queue_first, queue_last);
        graph_node_t* node = queue_top->node->node;
        
        // add the current node to the list
        graph_node_list_node_t* list_node = (graph_node_list_node_t*)arena_alloc(graph->process_list_arena, sizeof(graph_node_list_node_t));
        list_node->node = node;
        dll_push_back(graph->process_list.first, graph->process_list.last, list_node);
        
        // update the in-degree for the connected nodes
        for (graph_port_t* port = node->port_first; port != nullptr; port = port->next) {
            
            if (port->port_type != graph_port_type_output || port->connected_port == nullptr) { continue; }
            
            // find the node in the ts_node_list
            graph_node_t* port_node = port->connected_port->node; // node to find
            graph_ts_node_t* ts_node = nullptr;
            for (graph_ts_node_t* ts_n = node_first; ts_n != nullptr; ts_n = ts_n->next) {
                if (ts_n->node == port_node) {
                    ts_node = ts_n;
                    break;
                }
            }
            
            if (ts_node != nullptr) {
                
                // update in-degrees
                ts_node->in_degree--;
                
                // if in degree is 0, push to queue
                if (ts_node->in_degree == 0) {
                    graph_ts_queue_t* queue_node = (graph_ts_queue_t*)arena_alloc(scratch.arena, sizeof(graph_ts_queue_t));
                    queue_node->node = ts_node;
                    queue_push(queue_first, queue_last, queue_node);
                }
                
            }
            
        }
        
        
    }
    
    
    
    scratch_end(scratch);
}

function void
graph_process(graph_t* graph) {
    
    for (graph_node_list_node_t* list_node = graph->process_list.first; list_node != nullptr; list_node = list_node->next) {
        
        graph_node_t* node = list_node->node;
        
        // update the port data
        for (graph_port_t* port = node->port_first; port != nullptr; port = port->next) {
            if (port->port_type == graph_port_type_output || port->connected_port == nullptr) { continue; }
            
            // copy data
            memcpy(port->data->buffer, port->connected_port->data->buffer, sizeof(f32) * graph_buffer_size);
            
        }
        
        // process func
        if (node->process_func != nullptr) {
            node->process_func(node);
        }
    }
    
}

function void
graph_render(graph_t* graph) {
    
    os_handle_t window = graph->window;
    
    // TODO: I don't like using the draw layer directly
    // maybe we can use the ui layer to draw this
    
    // draw active port line
    if (ui_drag_is_active()) {
        
        graph_port_t* active_port = *(graph_port_t**)ui_drag_get_data();
        
        vec2_t port_center = rect_center(active_port->frame->rect);
        vec2_t mouse_pos = os_window_get_cursor_pos(window);
        vec2_t control_1;
        vec2_t control_2;
        if (active_port->port_type == graph_port_type_input) {
            control_1 = vec2_add(port_center, vec2(0.0f, -50.0f));
            control_2 = vec2_add(mouse_pos, vec2(0.0f, 50.0f));
        } else {
            control_1 = vec2_add(port_center, vec2(0.0f, 50.0f));
            control_2 = vec2_add(mouse_pos, vec2(0.0f, -50.0f));
        }
        color_t line_color = graph_data_type_colors[active_port->data->type]; 
        
        draw_push_thickness(2.0f);
        draw_push_color(line_color);
        draw_push_softness(0.75f);
        draw_bezier(port_center, mouse_pos, control_1, control_2);
        draw_pop_softness();
        draw_pop_thickness();
        draw_pop_color();
        
    }
    
    // port lines
    for (graph_node_t* node = graph->node_first; node != nullptr; node = node->next) {
        if (node->frame == nullptr) { continue; }
        
        for (graph_port_t* port = node->port_first; port != nullptr; port = port->next) {
            
            // skip
            if (port->frame == nullptr || 
                port->port_type == graph_port_type_output ||
                port->connected_port == nullptr) {
                continue;
            }
            
            vec2_t port_center = rect_center(port->frame->rect);
            vec2_t connected_port_center = rect_center(port->connected_port->frame->rect);
            vec2_t control_1 = vec2_add(port_center, vec2(0.0f, -50.0f));
            vec2_t control_2 = vec2_add(connected_port_center, vec2(0.0f, 50.0f));
            color_t line_color = graph_data_type_colors[port->data->type]; 
            
            draw_push_thickness(2.0f);
            draw_push_color(line_color);
            draw_push_softness(0.75f);
            draw_bezier(port_center, connected_port_center, control_1, control_2);
            draw_pop_softness();
            draw_pop_thickness();
            draw_pop_color();
            
        }
        
    }
    
    // node
    ui_key_t background_key = ui_key_from_stringf({0}, "background");
    ui_set_next_size(ui_size_percent(1.0f), ui_size_percent(1.0f));
    ui_frame_t* background = ui_frame_from_key(ui_frame_flag_interactable, background_key);
    ui_push_parent(background);
    
    for (graph_node_t* node = graph->node_first; node != nullptr; node = node->next) {
        
        // background frame
        ui_frame_flags frame_flags = 
            ui_frame_flag_interactable | 
            ui_frame_flag_draw_background |
            ui_frame_flag_draw_shadow;
        
        ui_key_t node_key = ui_key_from_stringf(ui_top_seed_key(), "%p", node);
        
        ui_set_next_fixed_x(node->pos.x);
        ui_set_next_fixed_y(node->pos.y);
        ui_set_next_size(ui_size_pixels(150.0f), ui_size_by_children(1.0f));
        ui_set_next_shadow_size(8.0f);
        ui_set_next_rounding(vec4(4.0f));
        ui_set_next_color_background(color(0x28282eff));
        node->frame = ui_frame_from_key(frame_flags, node_key);
        ui_push_parent(node->frame);
        
        // input port bar
        ui_set_next_size(ui_size_percent(1.0f), ui_size_pixels(12.0f));
        ui_set_next_color_background(color(0x202024ff));
        ui_set_next_rounding(vec4(0.0f, 4.0f, 0.0f, 4.0f));
        ui_set_next_layout_dir(ui_dir_right);
        ui_set_next_padding(1.0f);
        ui_frame_t* input_port_frame = ui_frame_from_key(ui_frame_flag_draw_background, { 0 });
        
        // node content
        {
            ui_set_next_size(ui_size_percent(1.0f), ui_size_by_children(1.0f));
            ui_set_next_padding(2.0f);
            ui_frame_t* content_frame = ui_frame_from_key(0, { 0 });
            ui_push_parent(content_frame);
            
            ui_push_size(ui_size_percent(1.0f), ui_size_text(1.0f));
            
            // label
            ui_label(node->label);
            
            // widgets
            for (graph_port_t* port = node->port_first; port != nullptr; port = port->next) {
                if (!(port->flags & graph_port_flag_edit) && !(port->flags & graph_port_flag_display)) {
                    continue;
                }
                
                if (port->flags & graph_port_flag_edit) {
                    
                    
                    switch (port->data->type) {
                        case graph_data_type_range: {
                            
                            str_t float_edit_string = str_format(ui_build_arena(), "%p_float_edit", port);
                            ui_float_edit(float_edit_string, &port->data->range_value, 0.01f, port->data->range_min, port->data->range_max);
                            
                            
                            break;
                        }
                    }
                    
                }
                
                if (port->flags & graph_port_flag_display) {
                    
                    switch (port->data->type) {
                        
                        // scope view
                        case graph_data_type_buffer: {
                            
                            ui_set_next_size(ui_size_percent(1.0f), ui_size_pixels(48.0f));
                            ui_key_t scope_key = ui_key_from_stringf(node_key, "%p_scope_%p", port, port->data);
                            ui_frame_t* scope_frame = ui_frame_from_key(0, scope_key);
                            ui_frame_set_custom_draw(scope_frame, graph_data_display_buffer, port->data);
                            
                            break;
                        }
                        
                    }
                    
                    
                    //ui_labelf("%s : %.3f", port->label.data, port->f32_value);
                }
                
                ui_spacer();
            }
            
            ui_pop_size();
            ui_pop_parent();
        }
        
        
        // output port bar
        ui_set_next_size(ui_size_percent(1.0f), ui_size_pixels(12.0f));
        ui_set_next_color_background(color(0x202024ff));
        ui_set_next_rounding(vec4(4.0f, 0.0f, 4.0f, 0.0f));
        ui_set_next_layout_dir(ui_dir_right);
        ui_set_next_padding(1.0f);
        ui_frame_t* output_port_frame = ui_frame_from_key(ui_frame_flag_draw_background, { 0 });
        
        // ports
        for (graph_port_t* port = node->port_first; port != nullptr; port = port->next) {
            ui_push_parent((port->port_type == graph_port_type_input) ? input_port_frame : output_port_frame);
            ui_spacer(ui_size_pixels(2.0f));
            
            ui_frame_flags port_flags = 
                ui_frame_flag_interactable |
                ui_frame_flag_draggable | 
                ui_frame_flag_draw_background |
                ui_frame_flag_draw_hover_effects |
                ui_frame_flag_draw_active_effects;
            
            ui_key_t port_key = ui_key_from_stringf(node_key, "%p", port);
            
            color_t port_color = graph_data_type_colors[port->data->type];
            
            ui_set_next_color_background(port_color);
            ui_set_next_size(ui_size_pixels(18.0f), ui_size_percent(1.0f));
            ui_set_next_rounding(vec4(4.0f));
            port->frame = ui_frame_from_key(port_flags, port_key); 
            
            ui_interaction port_interaction = ui_frame_interaction(port->frame);
            
            if (port_interaction & ui_interaction_hovered) {
                ui_tooltip_begin();
                ui_set_next_size(ui_size_text(1.0f), ui_size_text(1.0f));
                ui_label(port->label);
                ui_tooltip_end();
            }
            
            if (port_interaction & ui_interaction_left_pressed) {
                ui_drag_begin();
                
                // store node
                ui_drag_store_data(&port, sizeof(graph_port_t));
                //log_infof("[graph] drag started.. (%p)", port);
            }
            
            if (ui_drag_is_active() && port_interaction & ui_interaction_left_released) {
                
                // get data
                graph_port_t* dropped_port = *(graph_port_t**)ui_drag_get_data();
                
                // make sure we can drop:
                //  - not the same port
                //  - not the same port type (input/output)
                //  - not the same node 
                b8 can_drop = true;
                if (dropped_port == port) {
                    can_drop = false;
                } else if (dropped_port->port_type == port->port_type) {
                    can_drop = false;
                    //log_infof("[graph] can't connect ports of same type!");
                } else if (dropped_port->node == port->node) {
                    can_drop = false;
                    //log_infof("[graph] can't connect ports of same node!");
                } else if (dropped_port->data->type != port->data->type) {
                    can_drop = false;
                    //log_infof("[graph] can't connect ports of different data types!");
                }
                
                if (can_drop) {
                    //log_infof("[graph] drag dropped! (%p)", dropped_port);
                    dropped_port->connected_port = port;
                    port->connected_port = dropped_port;
                    graph_build(graph);
                }
                
            }
            
            ui_spacer(ui_size_pixels(2.0f));
            ui_pop_parent();
        }
        
        ui_pop_parent();
        
        // frame interaction 
        ui_interaction frame_interaction = ui_frame_interaction(node->frame);
        
        // bring node to front
        if (frame_interaction & ui_interaction_left_pressed) {
            graph_node_bring_to_front(node);
        }
        
        // drag
        if (frame_interaction & ui_interaction_left_dragging) {
            vec2_t mouse_delta = os_window_get_mouse_delta(graph->window);
            node->pos.x += mouse_delta.x;
            node->pos.y += mouse_delta.y;
        }
        
    }
    
    ui_pop_parent();
    
    ui_interaction background_interaction = ui_frame_interaction(background);
    
    if (background_interaction & ui_interaction_middle_dragging) {
        vec2_t mouse_delta = os_window_get_mouse_delta(graph->window);
        background->view_offset_target.x -= mouse_delta.x;
        background->view_offset_target.y -= mouse_delta.y;
        os_set_cursor(os_cursor_resize_all);
    }
    
}


//- key functions 

function graph_key_t 
graph_key_from_string(graph_key_t seed, str_t string) {
    graph_key_t key = { 0 };
    if (string.size != 0) {
        key.data[0] = str_hash(seed.data[0], string);
    }
    return key;
}

function graph_key_t
graph_key_from_stringf(graph_key_t seed, char* fmt, ...) {
    temp_t scratch = scratch_begin();
    
    va_list args;
    va_start(args, fmt);
    str_t string = str_formatv(scratch.arena, fmt, args);
    va_end(args);
    
    graph_key_t result = graph_key_from_string(seed, string);
    
    scratch_end(scratch);
    return result;
}

function b8
graph_key_equals(graph_key_t a, graph_key_t b){
    b8 result = false;
    if (a.data[0] == b.data[0]) {
        result = true;
    }
    return result;
}

//- data functions 

function graph_data_t* 
graph_data_from_key(graph_t* graph, graph_data_type type, graph_key_t key) {
    
    // try to find data first
    graph_data_t* data = nullptr;
    for (graph_data_t* d = graph->data_first; d != nullptr; d = d->next) {
        if (graph_key_equals(d->key, key)) {
            data = d;
            break;
        }
    }
    
    if (data == nullptr) {
        // grab from free stack or allocate one
        data = graph->data_free;
        if (data != nullptr) {
            stack_pop(graph->data_free);
        } else {
            data = (graph_data_t*)arena_alloc(graph->arena, sizeof(graph_data_t));
        }
        memset(data, 0, sizeof(graph_data_t));
        dll_push_back(graph->data_first, graph->data_last, data);
    }
    
    data->key = key;
    data->type = type;
    
    return data;
}

function graph_data_t*
graph_data_from_string(graph_t* graph, graph_data_type type, str_t string) {
    graph_key_t key = graph_key_from_string({0}, string);
    graph_data_t* result = graph_data_from_key(graph, type, key);
    return result;
}

function graph_data_t*
graph_data_from_stringf(graph_t* graph, graph_data_type type, char* fmt, ...) {
    temp_t scratch = scratch_begin();
    
    va_list args;
    va_start(args, fmt);
    str_t string = str_formatv(scratch.arena, fmt, args);
    va_end(args);
    
    graph_key_t key = graph_key_from_string({0}, string);
    graph_data_t* result = graph_data_from_key(graph, type, key);
    
    scratch_end(scratch);
    return result;
}

function void
graph_data_release(graph_t* graph, graph_data_t* data) {
    dll_remove(graph->data_first, graph->data_last, data);
    stack_push(graph->data_free, data);
}

//- port functions 

function graph_port_t* 
graph_port_create(graph_t* graph) {
    
    // grab from free stack or allocate one
    graph_port_t* port = graph->port_free;
    if (port != nullptr) {
        stack_pop(graph->port_free);
    } else {
        port = (graph_port_t*)arena_alloc(graph->arena, sizeof(graph_port_t));
    }
    memset(port, 0, sizeof(graph_port_t));
    
    return port;
}

function void 
graph_port_release(graph_t* graph, graph_port_t* port) {
    stack_push(graph->port_free, port);
}

function graph_port_t* 
graph_port_find(graph_node_t* node, str_t label) {
    graph_key_t port_key = graph_key_from_stringf(node->key, "%.*s", label.size, label.data);
    graph_port_t* result = nullptr;
    for (graph_port_t* port = node->port_first; port != nullptr; port = port->next) {
        if (graph_key_equals(port_key, port->key)) {
            result = port;
            break;
        }
    }
    return result;
}

//- node functions 

function graph_node_t*
graph_node_create(graph_t* graph, str_t label, graph_node_process_func* process_func) {
    
    // grab from free stack or allocate one
    graph_node_t* node = graph->node_free;
    if (node != nullptr) {
        stack_pop(graph->node_free);
    } else {
        node = (graph_node_t*)arena_alloc(graph->arena, sizeof(graph_node_t));
    }
    memset(node, 0, sizeof(graph_node_t));
    dll_push_back(graph->node_first, graph->node_last, node);
    
    // fill data
    node->graph = graph;
    node->key = graph_key_from_stringf({ 0 }, "%.*s_%p", label.size, label.data, node);
    node->label = label;
    node->process_func = process_func;
    
    return node;
}

function void
graph_node_release(graph_node_t* node) {
    
    graph_t* graph = node->graph;
    
    // remove from list and push to free stack
    dll_remove(graph->node_first, graph->node_last, node);
    stack_push(graph->node_free, node);
    
}

function graph_port_t*
graph_node_add_input(graph_node_t* node, str_t label, graph_data_type data_type, graph_port_flags flags) {
    
    // grab from free stack or allocate one
    graph_port_t* port = graph_port_create(node->graph);
    dll_push_back(node->port_first, node->port_last, port);
    
    // create data
    graph_data_t* data = graph_data_from_stringf(node->graph, data_type, "%p_%s_input", node, label.data);
    
    // fill struct
    port->key = graph_key_from_stringf(node->key, "%.*s", label.size, label.data);
    port->flags = flags;
    port->label = label;
    port->port_type = graph_port_type_input;
    port->node = node;
    port->data = data;
    
    return port;
}

function graph_port_t*
graph_node_add_output(graph_node_t* node, str_t label, graph_data_type data_type, graph_port_flags flags) {
    
    // grab from free stack or allocate one
    graph_port_t* port = graph_port_create(node->graph);
    dll_push_back(node->port_first, node->port_last, port);
    
    // create data
    graph_data_t* data = graph_data_from_stringf(node->graph, data_type, "%p_%s_output", node, label.data);
    
    // fill struct
    port->key = graph_key_from_stringf(node->key, "%.*s", label.size, label.data);
    port->flags = flags;
    port->label = label;
    port->port_type = graph_port_type_output;
    port->node = node;
    port->data = data;
    
    return port;
}

function void 
graph_node_bring_to_front(graph_node_t* node) {
    
    graph_t* graph = node->graph;
    
    dll_remove(graph->node_first, graph->node_last, node);
    dll_push_front(graph->node_first, graph->node_last, node);
}


//- data displays 

function void
graph_data_display_buffer(ui_frame_t* frame) {
    
    graph_data_t* data = (graph_data_t*)frame->custom_draw_data;
    u32 sample_count = graph_buffer_size / 2;
    
    draw_set_next_color(color(0x181818ff));
    draw_set_next_rounding(frame->rounding);
    draw_rect(frame->rect);
    
    // find max and min values
    f32 max_value = f32_min;
    f32 min_value = f32_max;
    for (i32 i = 0; i < sample_count; i++) {
        f32 value = data->buffer[i * 2];
        if (value > max_value) { max_value = value; }
        if (value < min_value) { min_value = value; }
    }
    
    f32 range = max_value - min_value;
    if (range == 0.0f) { range = 1.0f; }
    
    f32 padding = 0.2f;
    
    f32 width = rect_width(frame->rect);
    f32 height = rect_height(frame->rect);
    f32 padded_height = height * (1.0f - 2.0f * padding);
    
    f32 segment_width = width / (f32)(sample_count - 1);
    f32 vertical_scale = padded_height;
    f32 vertical_offset = frame->rect.y0 + height * padding + padded_height / 2.0f - ((max_value + min_value) / 2.0f) * vertical_scale;
    
    draw_push_thickness(1.5f);
    draw_push_softness(0.5f);
    for (i32 i = 0; i < sample_count - 1; i++) {
        
        vec2_t p0 = vec2(frame->rect.x0 + (i + 0) * segment_width, vertical_offset + data->buffer[(i + 0) * 2] * vertical_scale);
        vec2_t p1 = vec2(frame->rect.x0 + (i + 1) * segment_width, vertical_offset + data->buffer[(i + 1) * 2] * vertical_scale);
        
        draw_line(p0, p1);
    }
    draw_pop_thickness();
    draw_pop_softness();
    
}


#endif // GRAPH_CPP