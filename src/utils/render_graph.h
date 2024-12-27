// render_graph.h

#ifndef RENDER_GRAPH_H
#define RENDER_GRAPH_H

struct render_pass_t;
struct render_graph_t;

// structs

// render pass

typedef void render_pass_setup_func(render_pass_t*);
typedef void render_pass_execute_func(void*, void*);
typedef void render_pass_release_func(render_pass_t*);

struct render_pass_desc_t {
	str_t label;
	render_pass_setup_func* setup_func;
	render_pass_execute_func* execute_func;
	render_pass_release_func* release_func;
};

struct render_pass_t {

	// list ptrs
	render_pass_t* list_next;
	render_pass_t* list_prev;

	// tree ptrs
	render_pass_t* tree_next;
	render_pass_t* tree_prev;
	render_pass_t* tree_first;
	render_pass_t* tree_last;
	render_pass_t* tree_parent;

	// arena
	arena_t* arena;

	// info
	str_t label;
	render_pass_setup_func* setup;
	render_pass_execute_func* execute;
	render_pass_release_func* release;
	void* data;

};

struct render_pass_rec_t {
	render_pass_t* next;
	u32 push_count;
	u32 pop_count;
};

// render pass list

struct render_pass_node_t {
	render_pass_node_t* next;
	render_pass_node_t* prev;
	render_pass_t* pass;
};

struct render_pass_list_t {
	render_pass_node_t* first;
	render_pass_node_t* last;
};


// render graph

struct render_graph_t {
	render_graph_t* next;
	render_graph_t* prev;

	// arenas
	arena_t* pass_arena;
	arena_t* list_arena;

	gfx_renderer_t* renderer;
	
	// pass list (containes all passes)
	render_pass_t* pass_first;
	render_pass_t* pass_last;
	render_pass_t* pass_free;

	// execute list (contains execution order)
	render_pass_list_t execute_list;
	
	// output pass
	render_pass_t* output_pass;

	// debug
	u32 pass_count;

};

// state

struct render_state_t {

	arena_t* arena;

	render_graph_t* render_graph_first;
	render_graph_t* render_graph_last;
	render_graph_t* render_graph_free;
	render_graph_t* render_graph_active;
	
};

// output render pass data

struct output_render_pass_data_t {
	gfx_renderer_t* renderer;
	gfx_render_target_t* render_target;
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

function render_pass_t* render_graph_add_pass(render_graph_t* graph, render_pass_desc_t desc);
function void render_graph_remove_pass(render_graph_t* graph, render_pass_t* pass);
function void render_graph_pass_connect(render_pass_t* pass, render_pass_t* parent, render_pass_t* prev);
function void render_graph_pass_disconnect(render_pass_t* pass);

// pass
function render_pass_rec_t render_pass_depth_first(render_pass_t* pass);
function render_pass_list_t render_pass_list_from_graph(arena_t* arena, render_graph_t* graph);

// internal
function void _render_pass_output_setup(render_pass_t* pass);
function void _render_pass_output_execute(void* input_data, void* output_data);
function void _render_pass_output_release(render_pass_t* pass);

#endif // RENDER_GRAPH_H