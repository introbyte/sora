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
	render_pass_desc_t desc = { 0 };
	desc.label = str("output");
	desc.size = renderer->resolution;
	desc.execute_func = _render_pass_output_function;
	desc.format = gfx_texture_format_rgba8;
	desc.flags = 0;
	graph->output_pass = render_graph_add_pass(graph, desc);

	return graph;
}

function void 
render_graph_release(render_graph_t* graph) {

	// remove render targets in passes
	for (render_pass_node_t* node = graph->pass_list.first; node != 0; node = node->next) {
		gfx_render_target_release(node->pass->data.render_target);
	}
	
	dll_remove(render_state.render_graph_first, render_state.render_graph_last, graph);
	stack_push(render_state.render_graph_free, graph);
	arena_release(graph->pass_arena);
	arena_release(graph->list_arena);
}

function render_pass_t*
render_graph_add_pass(render_graph_t* graph, render_pass_desc_t desc) {

	// grab from free list
	render_pass_t* pass = graph->free_pass;
	if (pass != nullptr) {
		stack_pop(graph->free_pass);
	} else {
		pass = (render_pass_t*)arena_alloc(graph->pass_arena, sizeof(render_pass_t));
	}
	memset(pass, 0, sizeof(render_pass_t));
	graph->pass_count++;
	
	pass->arena = graph->pass_arena;
	pass->execute = desc.execute_func;
	pass->desc = desc;
	pass->data.render_target = gfx_render_target_create(desc.format, desc.size, desc.flags);
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
	graph->pass_count--;
	gfx_render_target_release(pass->data.render_target);
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
_render_pass_output_function(render_pass_data_t* in, render_pass_data_t* out) {
	if (in != nullptr && out != nullptr) {
		if (in->render_target != nullptr) {
			// blit in data to screen
			gfx_renderer_blit(out->graph->renderer, in->render_target->color_texture);
		}
	}
}

#endif // RENDER_GRAPH_CPP