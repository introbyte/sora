// node_graph.h

#ifndef NODE_GRAPH_H
#define NODE_GRAPH_H

// structs

struct ng_node_t {
	ng_node_t* next;
	ng_node_t* prev;
    
	str_t label;
	vec2_t pos;
	ui_frame_t* frame;
	f32 value;
};

struct ng_link_t {
	ng_link_t* next;
	ng_link_t* prev;
    
	vec2_t from_pos;
	vec2_t to_pos;
    
	ng_node_t* from;
	ng_node_t* to;
    
	ui_frame_t* frame;
};

struct ng_graph_t {
    
	arena_t* arena;
    
	ng_node_t* node_first;
	ng_node_t* node_last;
	ng_node_t* node_free;
	u32 node_count;
    
	ng_link_t* link_first;
	ng_link_t* link_last;
	ng_link_t* link_free;
	u32 link_count;
    
};

// functions

// graph
function ng_graph_t* ng_graph_create();
function void ng_graph_release(ng_graph_t* graph);

// node
function ng_node_t* ng_node_create(ng_graph_t* graph, str_t label, vec2_t pos);
function void ng_node_release(ng_graph_t* graph, ng_node_t* node);
function void ng_node_bring_to_front(ng_graph_t* graph, ng_node_t* node);

// connection
function ng_link_t* ng_link_create(ng_graph_t* graph, ng_node_t* from, ng_node_t* to);
function void ng_link_release(ng_graph_t* graph, ng_link_t* link);

// ui custom draw
function void ng_node_background_draw_function(ui_frame_t* frame);
function void ng_connection_draw_function(ui_frame_t* frame);
function void ng_port_draw_function(ui_frame_t* frame);

#endif // NODE_GRAPH_H