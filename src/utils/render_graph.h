// render_graph.h

#ifndef RENDER_GRAPH_H
#define RENDER_GRAPH_H

struct render_graph_t;
struct render_pass_t;

// structs

struct render_target_t {
	render_target_t* next;
	render_target_t* prev;
	gfx_render_target_t* gfx_render_target;
};

struct render_pass_data_t {
	render_target_t* target_first;
	render_target_t* target_last;
	u32 target_count;
	render_pass_t* pass;
	render_graph_t* graph;
};

struct render_graph_t;
typedef void render_pass_execute_func(render_pass_data_t*, render_pass_data_t*);

struct render_pass_t {
	render_pass_t* next;
	render_pass_t* prev;
	render_pass_t* first;
	render_pass_t* last;
	render_pass_t* parent;

	arena_t* arena;
	render_graph_t* graph;
	str_t label;
	render_pass_data_t data;
	render_pass_execute_func* execute;
};

struct render_pass_node_t {
	render_pass_node_t* next;
	render_pass_node_t* prev;
	render_pass_t* node;
};

struct render_pass_list_t {
	render_pass_node_t* first;
	render_pass_node_t* last;
};

struct render_pass_rec_t {
	render_pass_t* next;
	u32 push_count;
	u32 pop_count;
};

struct render_graph_t {
	render_graph_t* next;
	render_graph_t* prev;

	arena_t* pass_arena;
	arena_t* list_arena;

	gfx_renderer_t* renderer;

	render_pass_t* free_pass;
	render_pass_t* output_pass;

	render_pass_list_t pass_list;

};

struct render_state_t {

	arena_t* arena;

	render_graph_t* render_graph_first;
	render_graph_t* render_graph_last;
	render_graph_t* render_graph_free;
	render_graph_t* render_graph_active;
	
};

// global

global render_state_t render_state;

// functions

// state
function void render_init();
function void render_release();

// graph
function render_graph_t* render_graph_create(gfx_renderer_t* renderer);
function void render_graph_release(render_graph_t* graph);


function render_pass_t* render_graph_add_pass(render_graph_t* graph, str_t label, render_pass_execute_func* execute_func);
function void render_graph_remove_pass(render_graph_t* graph, render_pass_t* pass);
function void render_pass_connect(render_pass_t* pass, render_pass_t* parent, render_pass_t* prev);

// pass

function void render_pass_add_target(render_pass_t* pass, gfx_texture_format format);


function render_pass_rec_t render_pass_depth_first(render_pass_t* pass);
function render_pass_list_t render_pass_list_from_graph(arena_t* arena, render_graph_t* graph);

// internal

function void _render_pass_output_function(render_pass_data_t*, render_pass_data_t*);

#endif // RENDER_GRAPH_H