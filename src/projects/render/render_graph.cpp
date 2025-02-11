// render_graph.cpp

#ifndef RENDER_GRAPH_CPP
#define RENDER_GRAPH_CPP

//- render graph functions 

function rg_graph_t*
rg_graph_create(gfx_handle_t renderer) {
    
    // create arena
    arena_t* arena = arena_create(megabytes(64));
    
    // allocate render graph
    rg_graph_t* render_graph = (rg_graph_t*)arena_alloc(arena, sizeof(rg_graph_t));
    render_graph->arena = arena;
    render_graph->list_arena = arena_create(megabytes(4));
    render_graph->renderer = renderer;
    
    // create output pass
    render_graph->pass_output = rg_pass_create(render_graph, str("output"), _rg_output_pass_setup, _rg_output_pass_execute);
    
    return render_graph;
}

function void 
rg_graph_release(rg_graph_t* render_graph) {
    
    // release all passes
    for (rg_pass_t* pass = render_graph->pass_first; pass != nullptr; pass = pass->next) {
        rg_pass_release(pass);
    }
    
    // release arena
    arena_release(render_graph->list_arena);
    arena_release(render_graph->arena);
    
}

function void 
rg_graph_build(rg_graph_t* render_graph) {
    
    // clear current list
    arena_clear(render_graph->list_arena);
    render_graph->execute_first = nullptr;
    render_graph->execute_last = nullptr;
    
    // topological sort
    temp_t scratch = scratch_begin();
    
    // create list of nodes
    rg_ts_node_t* node_first = nullptr;
    rg_ts_node_t* node_last = nullptr;
    for (rg_pass_t* pass = render_graph->pass_first; pass != nullptr; pass = pass->next) {
        rg_ts_node_t* node = (rg_ts_node_t*)arena_alloc(scratch.arena, sizeof(rg_ts_node_t));
        node->pass = pass;
        node->in_degree = pass->in_degree;
        dll_push_back(node_first, node_last, node);
    }
    
    // create queue
    rg_ts_queue_t* queue_first = nullptr;
    rg_ts_queue_t* queue_last = nullptr;
    
    // loop over all nodes..
    for (rg_ts_node_t* node = node_first; node != nullptr; node = node->next) {
        // ..and add all passes with 0 in-degree to the queue.
        if (node->in_degree == 0) {
            rg_ts_queue_t* queue_pass = (rg_ts_queue_t*)arena_alloc(scratch.arena, sizeof(rg_ts_queue_t));
            queue_pass->node = node;
            queue_push(queue_first, queue_last, queue_pass);
        }
    }
    
    // while the queue isn't empty
    while (queue_first != nullptr) {
        
        // grab the top of the queue
        rg_ts_queue_t* queue_top = queue_first;
        queue_pop(queue_first, queue_last);
        rg_pass_t* pass = queue_top->node->pass;
        
        // add the current pass to the list
        rg_pass_list_node_t* output_node = (rg_pass_list_node_t*)arena_alloc(render_graph->list_arena, sizeof(rg_pass_list_node_t));
        output_node->pass = pass;
        dll_push_back(render_graph->execute_first, render_graph->execute_last, output_node);
        
        // update the in-degrees for its connected passes
        for (rg_edge_t* edge = pass->edge_first; edge != nullptr; edge = edge->next) {
            
            // find pass in node list
            rg_ts_node_t* node = nullptr;
            for (rg_ts_node_t* n = node_first; n != nullptr; n = n->next) {
                if (n->pass == edge->dst) {
                    node = n;
                    break;
                }
            }
            
            if (node != nullptr) {
                
                // update in-degres
                node->in_degree--;
                
                // if in-degree is 0, push to queue
                if (node->in_degree == 0) {
                    rg_ts_queue_t* queue_pass = (rg_ts_queue_t*)arena_alloc(scratch.arena, sizeof(rg_ts_queue_t));
                    queue_pass->node = node;
                    queue_push(queue_first, queue_last, queue_pass);
                }
                
            }
            
        }
        
    }
    
    scratch_end(scratch);
    
    // call setup functions
    for (rg_pass_list_node_t* node = render_graph->execute_first; node != nullptr; node = node->next) {
        rg_pass_t* pass = node->pass;
        
        if (pass->setup_func != nullptr) {
            pass->setup_func(pass);
        }
        
    }
    
}

function void
rg_graph_execute(rg_graph_t* render_graph) {
    
    // call execute functions
    for (rg_pass_list_node_t* node = render_graph->execute_first; node != nullptr; node = node->next) {
        rg_pass_t* pass = node->pass;
        
        if (pass->execute_func != nullptr) {
            pass->execute_func(pass);
        }
        
    }
    
}


//- render pass functions 

function rg_pass_t*
rg_pass_create(rg_graph_t* render_graph, str_t label, rg_pass_setup_func* setup_func, rg_pass_execute_func* execute_func) {
    
    // grab from free stack or allocate
    rg_pass_t* pass = render_graph->pass_free;
    if (pass != nullptr) {
        stack_pop(render_graph->pass_free);
    } else {
        pass = (rg_pass_t*)arena_alloc(render_graph->arena, sizeof(rg_pass_t));
    }
    memset(pass, 0, sizeof(rg_pass_t));
    dll_push_back(render_graph->pass_first, render_graph->pass_last, pass);
    
    // fill pass
    pass->graph = render_graph;
    pass->label = label;
    pass->key = rg_key_from_string({0}, label);
    pass->arena = arena_create(megabytes(64));
    pass->setup_func = setup_func;
    pass->execute_func = execute_func;
    
    return pass;
}

function void 
rg_pass_release(rg_pass_t* pass) {
    
    // release all resources
    for (rg_resource_t* resource = pass->resource_first; resource != nullptr; resource = resource->next) {
        switch (resource->type) {
            case rg_resource_type_texture: { gfx_texture_release(resource->texture); break; }
            case rg_resource_type_shader: { gfx_shader_release(resource->shader); break; }
            case rg_resource_type_render_target: { gfx_render_target_release(resource->render_target); break; }
        }
    }
    
    // release arena
    arena_release(pass->arena);
    
    // remove from list and push to free list
    rg_graph_t* render_graph = pass->graph;
    dll_remove(render_graph->pass_first, render_graph->pass_last, pass);
    stack_push(render_graph->pass_free, pass);
    
}

function void
rg_pass_connect(rg_graph_t* render_graph, rg_pass_t* src, rg_pass_t* dst) {
    
    // grab from free stack or allocate
    rg_edge_t* edge = render_graph->edge_free;
    if (edge != nullptr) {
        stack_pop(render_graph->edge_free);
    } else {
        edge = (rg_edge_t*)arena_alloc(render_graph->arena, sizeof(rg_edge_t));
    }
    memset(edge, 0, sizeof(rg_edge_t));
    dll_push_back(src->edge_first, src->edge_last, edge);
    
    // fill edge
    edge->dst = dst;
    
    // update in-degree
    dst->in_degree++;
    
}

function void 
rg_pass_disconnect(rg_graph_t* render_graph, rg_pass_t* src, rg_pass_t* dst) {
    
    // find edge
    rg_edge_t* edge = nullptr;
    for (rg_edge_t* e = src->edge_first; e != nullptr; e = e->next) {
        if (e->dst == dst) {
            edge = e;
            break;
        }
    }
    
    if (edge != nullptr) {
        
        // remove from outgoing list and push to free stack
        dll_remove(src->edge_first, src->edge_last, edge);
        stack_push(render_graph->edge_free, edge);
        
        // update in degree
        dst->in_degree--;
        
    }
    
}

function rg_pass_t* 
rg_pass_find(rg_graph_t* render_graph, str_t name) {
    
    rg_key_t key = rg_key_from_string({ 0 }, name);
    rg_pass_t* result = nullptr;
    
    for (rg_pass_t* pass = render_graph->pass_first; pass != nullptr; pass = pass->next) {
        if (rg_key_equals(key, pass->key)) {
            result = pass;
            break;
        }
    }
    
    return result;
}

function rg_resource_t*
rg_pass_add_resource(rg_pass_t* pass, rg_resource_type resource_type, str_t name) {
    
    // grab from free stack or allocate
    rg_resource_t* resource = pass->resource_free;
    if (resource != nullptr) {
        stack_pop(pass->resource_free);
    } else {
        resource = (rg_resource_t*)arena_alloc(pass->arena, sizeof(rg_resource_t));
    }
    memset(resource, 0, sizeof(rg_resource_t));
    dll_push_back(pass->resource_first, pass->resource_last, resource);
    
    // fill resource
    resource->key = rg_key_from_string(pass->key, name);;
    resource->type = resource_type;
    
    return resource;
}

function void 
rg_pass_remove_resource(rg_pass_t* pass, rg_key_t key) {
    
    rg_resource_t* resource = rg_pass_get_resource_by_key(pass, key);
    
    // release resource
    switch (resource->type) {
        case rg_resource_type_texture: { gfx_texture_release(resource->texture); break; }
        case rg_resource_type_shader: { gfx_shader_release(resource->shader); break; }
        case rg_resource_type_render_target: { gfx_shader_release(resource->render_target); break; }
    }
    
    // remove from list and push to free stack
    dll_remove(pass->resource_first, pass->resource_last, resource);
    stack_push(pass->resource_free, resource);
}

function rg_resource_t*
rg_pass_get_resource_by_key(rg_pass_t* pass, rg_key_t key) {
    rg_resource_t* result = nullptr;
    for (rg_resource_t* resource = pass->resource_first; resource != nullptr; resource = resource->next) {
        if (rg_key_equals(resource->key, key)) {
            result = resource;
            break;
        }
    }
    return result;
}

function rg_resource_t*
rg_pass_get_resource_by_name(rg_pass_t* pass, str_t name) {
    rg_key_t key = rg_key_from_string(pass->key, name);
    return rg_pass_get_resource_by_key(pass, key);
}

//- key functions 

function rg_key_t 
rg_key_from_string(rg_key_t seed, str_t string) {
    rg_key_t key = { 0 };
    if (string.size != 0) {
        u64 result = seed.data[0];
        for (u64 i = 0; i < string.size; i += 1) {
            result = ((result << 5) + result) + string.data[i];
        }
        key.data[0] = result;
    }
    return key;
}

function rg_key_t 
rg_key_from_stringf(rg_key_t seed, char* fmt, ...) {
    temp_t scratch = scratch_begin();
    
    va_list args;
    va_start(args, fmt);
    str_t string = str_formatv(scratch.arena, fmt, args);
    va_end(args);
    
    rg_key_t result = rg_key_from_string(seed, string);
    
    scratch_end(scratch);
    return result;
}

function b8 
rg_key_equals(rg_key_t a, rg_key_t b) {
    return (a.data[0] == b.data[0]);
}


//- helpers

function str_t
rg_resource_type_to_string(rg_resource_type type) {
    str_t result;
    switch(type) {
        case rg_resource_type_texture: { result = str("texture"); break;}
        case rg_resource_type_shader: { result = str("shader"); break;}
        case rg_resource_type_render_target: { result = str("render_target"); break;}
        default: { result = str("unknown"); break; }
    }
    return result;
}

//- output pass 

function void 
_rg_output_pass_setup(rg_pass_t* pass) {
    
}

function void 
_rg_output_pass_execute(rg_pass_t* pass) {
    
}


#endif // RENDER_GRAPH_CPP