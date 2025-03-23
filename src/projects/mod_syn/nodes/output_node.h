// output_node.h

#ifndef GRAPH_OUTPUT_NODE_H
#define GRAPH_OUTPUT_NODE_H

//- functions 

function graph_node_t* graph_output_node_create(graph_t* graph, vec2_t pos);

//- implementation 

function graph_node_t*
graph_output_node_create(graph_t* graph, vec2_t pos) {
    
    str_t label = str("output");
    graph_node_t* node = graph_node_create(graph, label, nullptr);
    graph_node_add_input(node, str("input"), graph_data_type_buffer, graph_port_flag_display);
    
    node->pos = pos;
    
    return node;
}

#endif // GRAPH_OUTPUT_NODE_H
