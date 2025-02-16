// node_graph.cpp

#ifndef NODE_GRAPH_CPP
#define NODE_GRAPH_CPP

//- key functions

function ng_key_t 
ng_key_from_string(ng_key_t seed, str_t string) {
    ng_key_t key = { 0 };
    u64 hash = str_hash(seed.data[0], string);
    key.data[0] = hash;
    return key;
}

function ng_key_t
ng_key_from_stringf(ng_key_t seed, char* fmt, ...) {
    temp_t scratch = scratch_begin();
    
    va_list args;
    va_start(args, fmt);
    str_t string = str_formatv(scratch.arena, fmt, args);
    va_end(args);
    
    ng_key_t result = ng_key_from_string(seed, string);
    
    scratch_end(scratch);
    return result;
}

//- node graph

function ng_graph_t*
ng_graph_create() {
    
	// allocate memory
	arena_t* arena = arena_create(megabytes(64));
    
	// allocate graph
	ng_graph_t* graph = (ng_graph_t*)arena_alloc(arena, sizeof(ng_graph_t));
    
	// fill struct
	graph->arena = arena;
    
	graph->node_first = nullptr;
	graph->node_last = nullptr;
	graph->node_free = nullptr;
    
	graph->link_first = nullptr;
	graph->link_last = nullptr;
	graph->link_free = nullptr;
    
	return graph;
}

function void
ng_graph_release(ng_graph_t* graph) {
	arena_release(graph->arena);
}


// node

function ng_node_t*
ng_node_create(ng_graph_t* graph, str_t label, vec2_t pos) {
    
	// grab from free list of allocate node
	ng_node_t* node = graph->node_free;
	if (node != nullptr) {
		stack_pop(graph->node_free);
	} else {
		node = (ng_node_t*)arena_alloc(graph->arena, sizeof(ng_node_t));
	}
	memset(node, 0, sizeof(ng_node_t));
	dll_push_back(graph->node_first, graph->node_last, node);
    
	// fill struct
	node->label = label;
	node->pos = pos;
	node->value = random_f32_range(0.0f, 10.0f);
    
	return node;
}

function void
ng_node_release(ng_graph_t* graph, ng_node_t* node) {
    
	// remove from list and push to free list
	dll_remove(graph->node_first, graph->node_last, node);
	stack_push(graph->node_free, node);
}

function void
ng_node_bring_to_front(ng_graph_t* graph, ng_node_t* node) {
    
	// reorder list
	dll_remove(graph->node_first, graph->node_last, node);
	dll_push_front(graph->node_first, graph->node_last, node);
}

// connection

function ng_edge_t*
ng_link_create(ng_graph_t* graph, ng_node_t* from, ng_node_t* to) {
    
	// grab from free list of allocate one.
	ng_edge_t* connection = graph->link_free;
	if (connection != nullptr) {
		stack_pop(graph->link_free);
	} else {
		connection = (ng_edge_t*)arena_alloc(graph->arena, sizeof(ng_edge_t));
	}
	memset(connection, 0, sizeof(ng_edge_t));
	dll_push_back(graph->link_first, graph->link_last, connection);
	graph->link_count++;
    
	// fill struct
	connection->from = from;
	connection->to = to;
    
	return connection;
}

function void
ng_link_release(ng_graph_t* graph, ng_edge_t* connection) {
	dll_remove(graph->link_first, graph->link_last, connection);
	stack_push(graph->link_free, connection);
	graph->link_count--;
}


// ui custom draw

function void
ng_node_background_draw_function(ui_frame_t* frame) {
    
	/*// draw grid
	f32 offset_x = fmodf(frame->view_offset_last.x, 25.0f);
	f32 offset_y = fmodf(frame->view_offset_last.y, 25.0f);
	for (f32 x = frame->rect.x0 - offset_x; x < frame->rect.x1 - offset_x + 25.0f; x += 25.0f) {
		for (f32 y = frame->rect.y0 - offset_y; y < frame->rect.y1 - offset_y + 25.0f; y += 25.0f) {
			f32 radius = 1.25f;
			draw_set_next_color(color(0x303030ff));
			draw_circle(vec2(x, y), radius, 0.0f, 360.0f);
		}
	}*/
    
}

function void
ng_link_draw_function(ui_frame_t* frame) {
    
	vec2_t p0 = frame->rect.v0;
	vec2_t p1 = frame->rect.v1;
	vec2_t c0 = vec2_add(p0, vec2(50.0f, 0.0f));
	vec2_t c1 = vec2_add(p1, vec2(-50.0f, 0.0f));
    
	draw_push_color(frame->palette.accent);
	draw_push_thickness(2.5f);
	draw_push_softness(0.5f);
	draw_bezier(p0, p1, c0, c1);
	draw_pop_softness();
	draw_pop_thickness();
	draw_pop_color();
    
}

function void
ng_port_draw_function(ui_frame_t* frame) {
    
	vec2_t center = rect_center(frame->rect);
    
	// animate radius
	f32 radius = roundf((min(rect_width(frame->rect), rect_height(frame->rect)) * 0.5f) - 2.0f);
	radius = lerp(radius, radius + 1.0f, frame->hover_t);
	f32 inner_radius = lerp(0.0f, radius - 5.0f, frame->active_t);
    
	// animate color
	color_t port_color = frame->palette.accent;
	port_color = color_lerp(port_color, color_blend(port_color, color(0xffffff45)), frame->hover_t);
	port_color = color_lerp(port_color, color_blend(port_color, color(0xffffff65)), frame->active_t);
    
	color_t inner_port_color = color_lerp(color(port_color.r, port_color.g, port_color.b, 0.0f), port_color, frame->active_t);
    
	// draw
	draw_set_next_color(port_color);
	draw_set_next_thickness(3.0f);
	draw_set_next_softness(0.75f);
	draw_circle(center, radius, 0.0f, 360.0f);
    
	// inner
	draw_set_next_color(inner_port_color);
	draw_set_next_softness(0.5f);
	draw_circle(center, inner_radius, 0.0f, 360.0f);
    
}


#endif // NODE_GRAPH_CPP