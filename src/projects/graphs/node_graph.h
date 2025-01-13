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

struct ng_connection_t {
	ng_connection_t* next;
	ng_connection_t* prev;

	vec2_t from_pos;
	vec2_t to_pos;

	ng_node_t* from;
	ng_node_t* to;

	ui_frame_t* frame;
};

struct ng_state_t {

	arena_t* arena;

	ng_node_t* node_first;
	ng_node_t* node_last;
	ng_node_t* node_free;
	u32 node_count;

	ng_connection_t* connection_first;
	ng_connection_t* connection_last;
	ng_connection_t* connection_free;
	u32 connection_count;

};

// functions

// state
function ng_state_t* ng_init();
function void ng_release(ng_state_t* state);

// node
function ng_node_t* ng_node_create(ng_state_t* state, str_t label, vec2_t pos);
function void ng_node_release(ng_state_t* state, ng_node_t* node);
function void ng_node_bring_to_front(ng_state_t* state, ng_node_t* node);

// connection
function ng_connection_t* ng_connection_create(ng_state_t* state, ng_node_t* from, ng_node_t* to);
function void ng_connection_release(ng_state_t* state, ng_connection_t* connection);

// ui custom draw
function void ng_node_background_draw_function(ui_frame_t* frame);
function void ng_connection_draw_function(ui_frame_t* frame);
function void ng_port_draw_function(ui_frame_t* frame);

#endif // NODE_GRAPH_H