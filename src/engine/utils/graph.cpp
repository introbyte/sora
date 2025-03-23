// graph.cpp

#ifndef GRAPH_CPP
#define GRAPH_CPP

//- implementation 

//- graph functions 

function graph_t*
graph_create() {
    
    arena_t* arena = arena_create(megabytes(64));
    graph_t* graph = (graph_t*)arena_alloc(arena, sizeof(graph_t));
    
    graph->arena = arena;
    
    graph->node_first = nullptr;
    graph->node_last = nullptr;
    graph->node_free = nullptr;
    
    graph->edge_first = nullptr;
    graph->edge_last = nullptr;
    graph->edge_free = nullptr;
    
    return graph;
}

function void 
graph_release(graph_t* graph) {
    
    // release arena
    arena_release(graph->arena);
    
}

//- key functions 

function graph_key_t 
graph_key_from_string(graph_key_t seed, str_t string) {
    graph_key_t key = { 0 };
    if (string.size != 0) {
        key.data[0] = str_hash(seed.data[0], string);
    }
    return key;
}

function graph_key_t
graph_key_from_stringf(graph_key_t seed, char* fmt, ...) {
    temp_t scratch = scratch_begin();
    
    va_list args;
    va_start(args, fmt);
    str_t string = str_formatv(scratch.arena, fmt, args);
    va_end(args);
    
    graph_key_t result = graph_key_from_string(seed, string);
    
    scratch_end(scratch);
    return result;
}

function b8
graph_key_equals(graph_key_t a, graph_key_t b){
    b8 result = false;
    if (a.data[0] == b.data[0]) {
        result = true;
    }
    return result;
}


//- node functions 

function graph_node_t*
graph_node_create(graph_t* graph, graph_key_t key, void* user_data) {
    
    // grab from free stack or allocate one
    graph_node_t* node = graph->node_free;
    if (node != nullptr) {
        stack_pop(graph->node_free);
    } else {
        node = (graph_node_t*)arena_alloc(graph->arena, sizeof(graph_node_t));
    }
    memset(node, 0, sizeof(graph_node_t));
    dll_push_back(graph->node_first, graph->node_last, node);
    
    // fill data
    node->key = key;
    node->user_data = user_data;
    
    return node;
}

function void 
graph_node_release(graph_t* graph, graph_node_t* node) {
    // remove from list and push to free stack
    dll_remove(graph->node_first, graph->node_last, node);
    stack_push(graph->node_free, node);
}

function graph_node_t*
graph_node_find(graph_t* graph, graph_key_t key) {
    graph_node_t* result = nullptr;
    for (graph_node_t* node = graph->node_first; node != nullptr; node = node->next) {
        if (graph_key_equals(node->key, key)) {
            result = node;
            break;
        }
    }
    
    return result;
}

function void 
graph_node_connect(graph_t* graph, graph_node_t* src, graph_node_t* dst) {
    
    // create edge
    graph_edge_t* edge = graph_edge_create(graph);
    
    
    
    
    src->edge_count++;
}



//- edge functions 

function graph_edge_t*
graph_edge_create(graph_t* graph, graph_key_t key) {
    // grab from free stack or allocate one
    graph_edge_t* edge = graph->edge_free;
    if (edge != nullptr) {
        stack_pop(graph->edge_free);
    } else {
        edge = (graph_edge_t*)arena_alloc(graph->arena, sizeof(graph_edge_t));
    }
    memset(edge, 0, sizeof(graph_edge_t));
    dll_push_back(graph->edge_first, graph->edge_last, edge);
    
    // fill data
    edge->key = key;
    
    return edge;
}

function void
graph_edge_release(graph_t* graph, graph_edge_t* edge) {
    // remove from list and push to free stack
    dll_remove(graph->edge_first, graph->edge_last, edge);
    stack_push(graph->edge_free, edge);
}

function graph_edge_t*
graph_edge_find(graph_t* graph, ) {
    graph_edge_t* result = nullptr;
    for (graph_edge_t* edge = graph->edge_first; edge != nullptr; edge = edge->next) {
        if (graph_key_equals(edge->key, key)) {
            result = edge;
            break;
        }
    }
    
    return result;
}

#endif // GRAPH_CPP