// reverb_node.h

#ifndef GRAPH_REVERB_NODE_H
#define GRAPH_REVERB_NODE_H

//- functions 

function graph_node_t* graph_reverb_node_create(graph_t* graph, vec2_t pos);

function void graph_reverb_node_process(graph_node_t* node);

//- implementation 

function graph_node_t*
graph_reverb_node_create(graph_t* graph, vec2_t pos) {
    
    str_t label = str("reverb");
    graph_node_t* node = graph_node_create(graph, label, graph_reverb_node_process);
    
    graph_node_add_input(node, str("input"), graph_data_type_buffer);
    
    graph_port_t* port_room_size = graph_node_add_input(node, str("room size"), graph_data_type_range, graph_port_flag_edit);
    port_room_size->data->range_value = 0.7f;
    port_room_size->data->range_min = 0.0f;
    port_room_size->data->range_max = 0.0f;
    
    graph_port_t* port_wet = graph_node_add_input(node, str("wet"), graph_data_type_range, graph_port_flag_edit);
    port_wet->data->range_value = 0.33f;
    port_wet->data->range_min = 0.0f;
    port_wet->data->range_max = 0.0f;
    
    graph_port_t* port_dry = graph_node_add_input(node, str("dry"), graph_data_type_range, graph_port_flag_edit);
    port_dry->data->range_value = 0.0f;
    port_dry->data->range_min = 0.0f;
    port_dry->data->range_max = 0.0f;
    
    graph_port_t* port_width = graph_node_add_input(node, str("width"), graph_data_type_range, graph_port_flag_edit);
    port_width->data->range_value = 1.0f;
    port_width->data->range_min = 0.0f;
    port_width->data->range_max = 0.0f;
    
    graph_node_add_output(node, str("output"), graph_data_type_buffer);
    
    node->pos = pos;
    
    return node;
}

function void 
graph_reverb_node_process(graph_node_t* node) {
    
    graph_data_t* input = graph_port_find(node, str("input"))->data;
    graph_data_t* output = graph_port_find(node, str("output"))->data;
    
    graph_data_t* data_room_size = graph_port_find(node, str("room size"))->data;
    graph_data_t* data_wet = graph_port_find(node, str("wet"))->data;
    graph_data_t* data_dry = graph_port_find(node, str("dry"))->data;
    graph_data_t* data_width = graph_port_find(node, str("width"))->data;
    
    const i32 max_delay = 44;
    const f32 damping = 0.5f;
    
    i32 delay = (i32)(data_room_size->range_value * max_delay);
    if (delay < 1) delay = 1;
    
    // Create delay line buffer
    f32 delay_line_left[max_delay];
    f32 delay_line_right[max_delay];
    memset(delay_line_left, 0, sizeof(f32) * max_delay);
    memset(delay_line_right, 0, sizeof(f32) * max_delay);
    
    // Process the buffer
    for (int i = 0; i < graph_buffer_size / 2; i += 2) {
        
        f32 input_left = input->buffer[(i * 2) + 0];
        f32 input_right = input->buffer[(i * 2) + 1];
        
        f32 delayed_left = delay_line_left[i % delay];
        f32 delayed_right  = delay_line_right[i % delay];
        
        f32 feedback_left = delayed_left * damping;
        f32 feedback_right = delayed_right * damping;
        
        delay_line_left[i % delay] = input_left + feedback_left;
        delay_line_right[i % delay] = input_right + feedback_right;
        
        f32 output_left = delayed_left * data_wet->range_value + input_left * data_dry->range_value;
        f32 output_right = delayed_right * data_wet->range_value + input_right * data_dry->range_value;
        
        // Store result
        output->buffer[(i * 2) + 0] = output_left;
        output->buffer[(i * 2) + 1] = output_right;
    }
    
}

#endif // GRAPH_REVERB_NODE_H