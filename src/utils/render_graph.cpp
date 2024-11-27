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

	// release arena
	arena_release(render_state.arena);
	
}


// graph

function render_graph_t* 
render_graph_create(gfx_renderer_t* renderer) {

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
	render_pass_t* output_pass = render_graph_add_pass(graph, str("output"), _render_pass_output_function);
	graph->output_pass = output_pass;

	return graph;
}

function void 
render_graph_release(render_graph_t* graph) {
	dll_remove(render_state.render_graph_first, render_state.render_graph_last, graph);
	stack_push(render_state.render_graph_free, graph);
	arena_release(graph->pass_arena);
	arena_release(graph->list_arena);
}

function render_pass_t*
render_graph_add_pass(render_graph_t* graph, str_t label, render_pass_execute_func* execute_func) {

	// grab from free list
	render_pass_t* pass = graph->free_pass;
	if (pass != nullptr) {
		stack_pop(graph->free_pass);
	} else {
		pass = (render_pass_t*)arena_alloc(graph->pass_arena, sizeof(render_pass_t));
	}
	memset(pass, 0, sizeof(render_pass_t));
	
	pass->graph = graph;
	pass->arena = graph->pass_arena;
	pass->label = label;
	pass->execute = execute_func;
	pass->data.pass = pass;
	pass->data.graph = graph;

	return pass;
}

function void 
render_graph_remove_pass(render_graph_t* graph, render_pass_t* pass) {
	if (pass->parent != 0) {
		dll_remove(pass->parent->first, pass->parent->last, pass);
	}
	stack_push(graph->free_pass, pass);
}

function void
render_graph_pass_connect(render_pass_t* pass, render_pass_t* parent, render_pass_t* prev = nullptr) {
	dll_insert(parent->first, parent->last, prev, pass);
	pass->parent = parent;
}

function void
render_graph_build(render_graph_t* graph) {

	// clear list if needed
	arena_clear(graph->list_arena);

	// get ordered pass list
	graph->pass_list = render_pass_list_from_graph(graph->list_arena, graph);

}

function void
render_graph_execute(render_graph_t* graph) {

	for (render_pass_node_t* node = graph->pass_list.first; node != 0; node = node->next) {
		render_pass_data_t* prev_data = nullptr;
		if (node->prev != 0) {
			prev_data = &node->prev->node->data;
		}
		node->node->execute(prev_data, &node->node->data);
	}

}

// pass

function void
render_pass_add_target(render_pass_t* pass, gfx_texture_format format) {

	render_target_t* pass_target = (render_target_t*)arena_alloc(pass->arena, sizeof(render_target_t));
	pass_target->gfx_render_target = gfx_render_target_create(format, pass->graph->renderer->resolution, 0);
	dll_push_back(pass->data.target_first, pass->data.target_last, pass_target);

}


function render_pass_rec_t
render_pass_depth_first(render_pass_t* node) {
	render_pass_rec_t rec = { 0 };
	if (node->first != 0) {
		rec.next = node->first;
		rec.push_count++;
	} else for (render_pass_t* n = node; n != 0; n = n->parent) {
		if (n->next != 0) {
			rec.next = n->next;
			break;
		}
		rec.pop_count++;
	}
	return rec;
}

function render_pass_list_t 
render_pass_list_from_graph(arena_t* arena, render_graph_t* graph) {
	
	render_pass_list_t list = { 0 };

	for (render_pass_t* node = graph->output_pass; node != 0;) {
		render_pass_rec_t rec = render_pass_depth_first(node);
		render_pass_node_t* list_node = (render_pass_node_t*)arena_calloc(arena, sizeof(render_pass_node_t));
		list_node->node = node;
		dll_push_back(list.first, list.last, list_node);
		node = rec.next;
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
_render_pass_output_function(render_pass_data_t* in, render_pass_data_t* out) {
	if (in != nullptr && out != nullptr) {
		if (in->target_first != nullptr) {
			// blit in data to screen
			gfx_renderer_blit(out->graph->renderer, in->target_first->gfx_render_target->texture);
		}
	}
}

#endif // RENDER_GRAPH_CPP