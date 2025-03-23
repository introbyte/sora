// osc_node.h

#ifndef GRAPH_OSC_NODE_H
#define GRAPH_OSC_NODE_H

//- enums 

enum osc_type {
	osc_type_phase,
	osc_type_sin,
	osc_type_saw,
	osc_type_tri,
	osc_type_square,
	osc_type_noise,
};

//- functions 

function graph_node_t* graph_osc_node_create(graph_t* graph, vec2_t pos);
function void graph_osc_node_process(graph_node_t* node);

function f32 graph_osc_node_get_amplitude(osc_type type, f32 phase);

//- implementation 

function graph_node_t*
graph_osc_node_create(graph_t* graph, vec2_t pos) {
    
    str_t label = str("osc");
    graph_node_t* node = graph_node_create(graph, label, graph_osc_node_process);
    
    graph_port_t* port_type = graph_node_add_input(node, str("type"), graph_data_type_u32);
    port_type->data->u32_value = 3;
    
    graph_port_t* port_gain = graph_node_add_input(node, str("gain"), graph_data_type_range, graph_port_flag_edit);
    port_gain->data->range_value = 1.0f;
    port_gain->data->range_min = 0.0f;
    port_gain->data->range_max = 1.0f;
    
    graph_port_t* port_pan = graph_node_add_input(node, str("pan"), graph_data_type_range, graph_port_flag_edit);
    port_pan->data->range_value = 0.0f;
    port_pan->data->range_min = -1.0f;
    port_pan->data->range_max = 1.0f;
    
    graph_port_t* port_freq = graph_node_add_input(node, str("freq"), graph_data_type_range, graph_port_flag_edit);
    port_freq->data->range_value = 400.0f;
    port_freq->data->range_min = 10.0f;
    port_freq->data->range_max = 20000.0f;
    
    graph_node_add_output(node, str("output"), graph_data_type_buffer);
    
    node->pos = pos;
    
    return node;
}

function void
graph_osc_node_process(graph_node_t* node) {
    
    graph_t* graph = node->graph;
    
    graph_data_t* data_type = graph_port_find(node, str("type"))->data;
    graph_data_t* data_gain_target = graph_port_find(node, str("gain"))->data;
    graph_data_t* data_pan_target = graph_port_find(node, str("pan"))->data;
    graph_data_t* data_freq = graph_port_find(node, str("freq"))->data;
    graph_data_t* data_output = graph_port_find(node, str("output"))->data;
    
    // get phase
    graph_key_t key_phase  = graph_key_from_string(node->key, str("phase"));
    graph_data_t* data_phase = graph_data_from_key(graph, graph_data_type_f32, key_phase);
    
    // get gain
    graph_key_t key_gain = graph_key_from_string(node->key, str("gain_current"));
    graph_data_t* data_gain = graph_data_from_key(graph, graph_data_type_f32, key_gain);
    
    // get panning
    graph_key_t key_pan = graph_key_from_string(node->key, str("pan_current"));
    graph_data_t* data_pan = graph_data_from_key(graph, graph_data_type_f32, key_pan);
    
    for (i32 i = 0 ; i < graph_buffer_size / 2; i++) {
        
        // panning
        data_pan->f32_value +=(data_pan_target->range_value - data_pan->f32_value) * 0.001f;
        data_pan->f32_value = clamp(data_pan->f32_value, -1.0f, 1.0f);
        f32 left_amp = (1.0f - data_pan->f32_value) * 0.5f;
        f32 right_amp = (1.0f + data_pan->f32_value) * 0.5f;
        
        // get amplitude 
        f32 amplitude = graph_osc_node_get_amplitude((osc_type)data_type->u32_value, data_phase->range_value);
        
        // update phase
        data_phase->f32_value += data_freq->range_value * graph_audio_sample_rate;
        if (data_phase->f32_value >= 1.0f) { data_phase->range_value -= 1.0f; }
        
        // smooth gain
        data_gain->f32_value += (data_gain_target->range_value - data_gain->f32_value) * 0.001f;
        
        // output to buffer
        data_output->buffer[i * 2 + 0] = amplitude * left_amp * data_gain->f32_value;
        data_output->buffer[i * 2 + 1] = amplitude * right_amp * data_gain->f32_value;
    }
    
}

function f32
graph_osc_node_get_amplitude(osc_type type, f32 phase) {
    f32 amplitude = 0.0f;
    switch (type) {
        case osc_type_phase:  { amplitude = phase; break; }
        case osc_type_sin:    { amplitude = sinf(phase * f32_pi * 2.0f); break; }
        case osc_type_saw:    { amplitude = 1.0f - 2.0f * phase; break; }
        case osc_type_tri:    { amplitude = (fabs(phase - 0.5f) - 0.25f) * 4.0f; break; }
        case osc_type_square: { amplitude = phase < 0.5f ? -1.0f : 1.0f; break; }
        case osc_type_noise:  { amplitude = 1.0f - 2.0f * (random_f32_range(0.0f, 1.0f)); break; }
    }
    return amplitude;
}


#endif // GRAPH_OSC_NODE_H
