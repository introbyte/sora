// node_graph.h

#ifndef NODE_GRAPH_H
#define NODE_GRAPH_H

//- enums 



//- structs

struct ng_node_t;
struct ng_port_t;
struct ng_edge_t;

// key
struct ng_key_t {
    u64 data[1];
};

// port
struct ng_port_t {
    
    // info
    ng_node_t* node;
    str_t label;
    ng_key_t key;
    
};

// port list

struct ng_port_list_node_t {
    ng_port_list_node_t* next;
    ng_port_list_node_t* prev;
    ng_port_t* port;
};

struct ng_port_list_t {
    ng_port_list_node_t* first;
    ng_port_list_node_t* last;
    u32 count;
}

// node
struct ng_node_t {
    
    // global node list
    ng_node_t* next;
    ng_node_t* prev;
    
    // info
    str_t label;
    ng_key_t key;
    
    // ports
    ng_port_list_t* in_port_list;
    ng_port_list_t* out_port_list;
    
};

// edge
struct ng_edge_t {
    
    // global edge list
    ng_edge_t* next;
    ng_edge_t* prev;
    
    // ports
    ng_port_t* from;
    ng_port_t* to;
    
};


struct ng_node_t {
	ng_node_t* next;
	ng_node_t* prev;
    
	str_t label;
	vec2_t pos;
	ui_frame_t* frame;
	f32 value;
};

struct ng_edge_t {
	ng_edge_t* next;
	ng_edge_t* prev;
    
	vec2_t from_pos;
	vec2_t to_pos;
    
	ng_node_t* from;
	ng_node_t* to;
    
	ui_frame_t* frame;
};

struct ng_graph_t {
    
	arena_t* arena;
    
    // node list
	ng_node_t* node_first;
	ng_node_t* node_last;
	ng_node_t* node_free;
    
    // edge list
	ng_edge_t* link_first;
	ng_edge_t* link_last;
	ng_edge_t* link_free;
    
};

//- functions

// key
function ng_key_t ng_key_from_string(ng_key_t seed, str_t string);
function ng_key_t ng_key_from_stringf(ng_key_t seed, char* fmt, ...);

// node
function ng_node_t* ng_node_create(ng_graph_t* graph, str_t label);
function void ng_node_release(ng_graph_t* graph, ng_node_t* node);

// ports
function ng_port_t* ng_port_create(ng_node_t* node, str_t label);
function void ng_port_release(ng_port_t* port)



// graph
function ng_graph_t* ng_graph_create();
function void ng_graph_release(ng_graph_t* graph);





// node
function ng_node_t* ng_node_create(ng_graph_t* graph, str_t label, vec2_t pos);
function void ng_node_release(ng_graph_t* graph, ng_node_t* node);
function void ng_node_bring_to_front(ng_graph_t* graph, ng_node_t* node);

// connection
function ng_edge_t* ng_link_create(ng_graph_t* graph, ng_node_t* from, ng_node_t* to);
function void ng_link_release(ng_graph_t* graph, ng_edge_t* link);

// ui custom draw
function void ng_node_background_draw_function(ui_frame_t* frame);
function void ng_connection_draw_function(ui_frame_t* frame);
function void ng_port_draw_function(ui_frame_t* frame);

#endif // NODE_GRAPH_H