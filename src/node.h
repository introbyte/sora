// node.h

#ifndef NODE_H
#define NODE_H

// structs
struct node_t;

struct node_port_t {
	node_t* parent;
	str_t label;
	ui_key_t key;
	node_port_t* connected_port;
};

struct node_t {
	node_t* next;
	node_t* prev;
	
	node_port_t inputs[8];
	u32 input_count;
	node_port_t outputs[8];
	u32 output_count;

	str_t label;
	vec2_t pos;
};

struct node_state_t {

	arena_t* node_arena;
	arena_t* scratch_arena;

	node_t* first;
	node_t* last;
	u32 count;
};

struct node_port_draw_data_t {
	node_port_t* connected_port;
	ui_interaction interaction;
};

global node_state_t node_state;

// functions

function void node_state_init();
function void node_state_release();
function void node_state_update();

function node_t* node_create(str_t, vec2_t);
function void node_release(node_t*);
function void node_add_input(node_t*);
function void node_add_output(node_t*);

function ui_interaction node_port(str_t, node_port_t*);
function void node_port_draw_function(ui_frame_t*);

#endif // NODE_H