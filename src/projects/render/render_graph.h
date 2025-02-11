// render_graph.h

#ifndef RENDER_GRAPH_H
#define RENDER_GRAPH_H

//- forward declare 

struct rg_pass_t;
struct rg_edge_t;
struct rg_graph_t;

//- enums 

enum rg_resource_type {
    rg_resource_type_none,
    rg_resource_type_texture,
    rg_resource_type_shader,
    rg_resource_type_render_target,
    rg_resource_type_count,
};

//- typedefs 

typedef void rg_pass_setup_func(rg_pass_t*);
typedef void rg_pass_execute_func(rg_pass_t*);

//- structs 

// key
struct rg_key_t {
    u64 data[1];
};

// resource
struct rg_resource_t {
    rg_resource_t* next;
    rg_resource_t* prev;
    
    rg_key_t key;
    rg_resource_type type;
    
    union {
        gfx_handle_t texture;
        gfx_handle_t shader;
        gfx_handle_t render_target;
    };
};

// edge
struct rg_edge_t {
    rg_edge_t* next;
    rg_edge_t* prev;
    
    rg_pass_t* dst;
};

// pass
struct rg_pass_t {
    
    str_t label;
    
    // list
    rg_pass_t* next;
    rg_pass_t* prev;
    
    // edges
    rg_edge_t* edge_first;
    rg_edge_t* edge_last;
    
    // info
    rg_graph_t* graph;
    rg_key_t key;
    u32 in_degree;
    
    // arena
    arena_t* arena;
    
    // functions
    rg_pass_setup_func* setup_func;
    rg_pass_execute_func* execute_func;
    
    // resources
    rg_resource_t* resource_first;
    rg_resource_t* resource_last;
    rg_resource_t* resource_free;
    
};

// topological sort node
struct rg_ts_node_t {
    rg_ts_node_t* next;
    rg_ts_node_t* prev;
    
    rg_pass_t* pass;
    u32 in_degree;
};

// topological sort queue
struct rg_ts_queue_t {
    rg_ts_queue_t* next;
    rg_ts_queue_t* prev;
    
    rg_ts_node_t* node;
};

// pass list
struct rg_pass_list_node_t {
    rg_pass_list_node_t* next;
    rg_pass_list_node_t* prev;
    rg_pass_t* pass;
};

// graph
struct rg_graph_t {
    
    arena_t* arena;
    arena_t* list_arena;
    
    gfx_handle_t renderer;
    
    // pass list
    rg_pass_t* pass_first;
    rg_pass_t* pass_last;
    rg_pass_t* pass_free;
    
    // edge free list
    rg_edge_t* edge_free;
    
    // execute list
    rg_pass_list_node_t* execute_first;
    rg_pass_list_node_t* execute_last;
    
    // output pass
    rg_pass_t* pass_output;
    
};

//- functions 

// graph
function rg_graph_t* rg_graph_create(gfx_handle_t renderer);
function void rg_graph_release(rg_graph_t* render_graph);
function void rg_graph_build(rg_graph_t* render_graph); 
function void rg_graph_execute(rg_graph_t* render_graph);

// pass
function rg_pass_t* rg_pass_create(rg_graph_t* render_graph, str_t label, rg_pass_setup_func* setup_func, rg_pass_execute_func* execute_func);
function void rg_pass_release(rg_pass_t* pass);
function void rg_pass_connect(rg_graph_t* render_graph, rg_pass_t* src, rg_pass_t* dst);
function void rg_pass_disconnect(rg_graph_t* render_graph, rg_pass_t* src, rg_pass_t* dst);

function rg_pass_t* rg_pass_find(rg_graph_t* render_graph, str_t name);

function rg_resource_t* rg_pass_add_resource(rg_pass_t* pass, rg_resource_type resource_type, str_t name);
function void rg_pass_remove_resource(rg_pass_t* pass, rg_key_t key);
function rg_resource_t* rg_pass_get_resource_by_key(rg_pass_t* pass, rg_key_t key);
function rg_resource_t* rg_pass_get_resource_by_name(rg_pass_t* pass, str_t name);

// key
function rg_key_t rg_key_from_string(rg_key_t seed, str_t string);
function rg_key_t rg_key_from_stringf(rg_key_t seed, char* fmt, ...);
function b8 rg_key_equals(rg_key_t a, rg_key_t b);

// type helpers

function str_t rg_resource_type_to_string(rg_resource_type type);

// output pass
function void _rg_output_pass_setup(rg_pass_t* pass);
function void _rg_output_pass_execute(rg_pass_t* pass);

#endif // RENDER_GRAPH_H