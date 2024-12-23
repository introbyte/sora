// render_graph.cpp

#ifndef RENDER_GRAPH_CPP
#define RENDER_GRAPH_CPP

// implementation

// state

function void 
render_init() {

	// allocate arena
	render_state.arena = arena_create(gigabytes(2));
	
	render_state.render_graph_first = nullptr;
	render_state.render_graph_last = nullptr;
	render_state.render_graph_free = nullptr;
	render_state.render_graph_active = nullptr;
	
}

function void 
render_release() {

	// release all graphs
	for (render_graph_t* graph = render_state.render_graph_first; graph != nullptr; graph = graph->next) {
		render_graph_release(graph);
	}

	// release arena
	arena_release(render_state.arena);
	
}


// graph

function render_graph_t* 
render_graph_create(gfx_renderer_t* renderer) {

	// find from free stack or create one
	render_graph_t* graph = render_state.render_graph_free;
	if (graph != nullptr) {
		stack_pop(render_state.render_graph_free);
	} else {
		graph = (render_graph_t*)arena_alloc(render_state.arena, sizeof(render_graph_t));
	}
	memset(graph, 0, sizeof(render_graph_t));
	
	// allocate arena
	graph->pass_arena = arena_create(megabytes(8));
	graph->list_arena = arena_create(megabytes(8));

	// add to list
	dll_push_back(render_state.render_graph_first, render_state.render_graph_last, graph);

	// fill
	graph->renderer = renderer;

	// add output pass
	render_pass_desc_t desc = { 0 };
	desc.label = str("output");
	desc.setup_func = _render_pass_output_setup;
	desc.execute_func = _render_pass_output_execute;
	desc.release_func = _render_pass_output_release;
	graph->output_pass = render_graph_add_pass(graph, desc);

	return graph;
}

function void 
render_graph_release(render_graph_t* graph) {

	// release user data in all passes
	for (render_pass_t* pass = graph->pass_first; pass != nullptr; pass = pass->list_next) {
		if (pass->release != nullptr) {
			pass->release(pass);
		}
	}

	// release arenas
	arena_release(graph->pass_arena);
	arena_release(graph->list_arena);
	
	// remove from list and add to free stack
	dll_remove(render_state.render_graph_first, render_state.render_graph_last, graph);
	stack_push(render_state.render_graph_free, graph);

}

function render_pass_t*
render_graph_add_pass(render_graph_t* graph, render_pass_desc_t desc) {

	// grab from free stack or create one
	render_pass_t* pass = graph->pass_free;
	if (pass != nullptr) {
		stack_pop_n(graph->pass_free, list_next);
	} else {
		pass = (render_pass_t*)arena_alloc(graph->pass_arena, sizeof(render_pass_t));
	}
	memset(pass, 0, sizeof(render_pass_t));
	
	// set arena
	pass->arena = graph->pass_arena;

	// fill struct
	pass->label = desc.label;
	pass->setup = desc.setup_func;
	pass->execute = desc.execute_func;
	pass->release = desc.release_func;

	// add to graph's pass list
	dll_push_back_np(graph->pass_first, graph->pass_last, pass, list_next, list_prev);
	graph->pass_count++;

	return pass;
}

function void 
render_graph_remove_pass(render_graph_t* graph, render_pass_t* pass) {

	// remove from tree
	render_graph_pass_disconnect(pass);

	// release any user data
	if (pass->release != nullptr) {
		pass->release(pass);
	}

	// remove from graph's pass list 
	dll_remove_np(graph->pass_first, graph->pass_last, pass, list_next, list_prev);
	graph->pass_count--;

	// add to free stack
	stack_push_n(graph->pass_free, pass, list_next);

}

function void
render_graph_pass_connect(render_pass_t* pass, render_pass_t* parent, render_pass_t* prev = nullptr) {
	dll_insert_np(parent->tree_first, parent->tree_last, prev, pass, tree_next, tree_prev);
	pass->tree_parent = parent;
}

function void
render_graph_pass_disconnect(render_pass_t* pass) {
	dll_remove_np(pass->tree_parent->tree_first, pass->tree_parent->tree_last, pass, tree_next, tree_prev);
	pass->tree_next = pass->tree_prev = pass->tree_parent = nullptr;
}

function void
render_graph_build(render_graph_t* graph) {

	// clear list if needed
	arena_clear(graph->list_arena);

	// create execution order pass list
	graph->execute_list = render_pass_list_from_graph(graph->list_arena, graph);

	// setup 
	for (render_pass_node_t* node = graph->execute_list.first; node != 0; node = node->next) {
		if (node->pass->setup != nullptr) {
			node->pass->setup(node->pass);
		}
	}
}

function void
render_graph_execute(render_graph_t* graph) {

	for (render_pass_node_t* node = graph->execute_list.first; node != 0; node = node->next) {

		void* prev_data = nullptr;
		if (node->prev != nullptr) {
			prev_data = &node->prev->pass->data;
		}
		
		if (node->pass->execute != nullptr) {
			node->pass->execute(prev_data, &node->pass->data);
		}
	}

}

// pass

function render_pass_rec_t
render_pass_depth_first(render_pass_t* node) {
	// pre order depth first search

	render_pass_rec_t rec = { 0 };
	if (node->tree_first != 0) {
		rec.next = node->tree_first;
		rec.push_count++;
	} else for (render_pass_t* n = node; n != 0; n = n->tree_parent) {
		if (n->tree_next != 0) {
			rec.next = n->tree_next;
			break;
		}
		rec.pop_count++;
	}
	return rec;
}

function render_pass_list_t 
render_pass_list_from_graph(arena_t* arena, render_graph_t* graph) {
	
	render_pass_list_t list = { 0 };

	// make list in depth first order
	for (render_pass_t* pass = graph->output_pass; pass != 0;) {
		render_pass_rec_t rec = render_pass_depth_first(pass);
		render_pass_node_t* list_node = (render_pass_node_t*)arena_calloc(arena, sizeof(render_pass_node_t));
		list_node->pass = pass;
		dll_push_back(list.first, list.last, list_node);
		pass = rec.next;
 	}

	// reverse the list
	render_pass_node_t* current = list.first;
	render_pass_node_t* temp = nullptr;
	while (current != nullptr) {
		temp = current->next;
		current->next = current->prev;
		current->prev = temp;
		current = temp;
	}

	temp = list.first;
	list.first = list.last; 
	list.last = temp;
	
	return list;
}

// internal

function void
_render_pass_output_setup(render_pass_t* pass) {

}

function void
_render_pass_output_execute(void* input_data, void* output_data) {
	if (input_data != nullptr && output_data != nullptr) {
		//if (input_data->render_target != nullptr) {
		//	// blit in data to screen
		//	gfx_renderer_blit(out->graph->renderer, in->render_target->color_texture);
		//}
	}
}

function void
_render_pass_output_release(render_pass_t* pass) {

}


#endif // RENDER_GRAPH_CPP