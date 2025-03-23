// graph.h

#ifndef GRAPH_H
#define GRAPH_H

//- structs 

struct graph_key_t {
    u64 data[1];
};

struct graph_t;
struct graph_edge_t;

struct graph_node_t {
    graph_key_t key;
    graph_t* graph;
    
    graph_node_t* next;
    graph_node_t* prev;
    
    graph_edge_t* edge_first;
    graph_edge_t* edge_last;
    u32 edge_count;
    
    void* user_data;
};

struct graph_edge_t {
    graph_edge_t* global_next;
    graph_edge_t* global_prev;
    
    graph_edge_t* next;
    graph_edge_t* prev;
    
    graph_node_t* src;
    graph_node_t* dst;
};

struct graph_t {
    
    arena_t* arena;
    
    graph_node_t* node_first;
    graph_node_t* node_last;
    graph_node_t* node_free;
    
    graph_edge_t* edge_first;
    graph_edge_t* edge_last;
    graph_edge_t* edge_free;
    
};

//- function 

// graph 
function graph_t* graph_create();
function void graph_release(graph_t* graph);

// key
function graph_key_t graph_key_from_string(graph_key_t seed, str_t string);
function graph_key_t graph_key_from_stringf(graph_key_t seed, char* fmt, ...);
function b8 graph_key_equals(graph_key_t a, graph_key_t b);

// nodes
function graph_node_t* graph_node_create(graph_t* graph, graph_key_t key, void* user_data = nullptr);
function void  graph_node_release(graph_t* graph, graph_node_t* node);
function graph_node_t* graph_node_find(graph_t* graph, graph_key_t key);

// edge 
function graph_edge_t* graph_edge_create(graph_t* graph, graph_key_t key);
function void graph_edge_release(graph_t* graph, graph_edge_t* edge);
function graph_edge_t* graph_edge_find(graph_t* graph, graph_key_t key);

#endif // GRAPH_H