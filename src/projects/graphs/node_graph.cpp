// node_graph.cpp

#ifndef NODE_GRAPH_CPP
#define NODE_GRAPH_CPP


// implementation

// node state

function ng_state_t*
ng_init() {

	// allocate memory
	arena_t* arena = arena_create(megabytes(64));

	// allocate state
	ng_state_t* state = (ng_state_t*)arena_alloc(arena, sizeof(ng_state_t));

	// fill struct
	state->arena = arena;

	state->node_first = nullptr;
	state->node_last = nullptr;
	state->node_free = nullptr;
	state->node_count = 0;

	state->connection_first = nullptr;
	state->connection_last = nullptr;
	state->connection_free = nullptr;
	state->connection_count = 0;

	return state;
}

function void
ng_release(ng_state_t* state) {
	arena_release(state->arena);
}


// node

function ng_node_t*
ng_node_create(ng_state_t* state, str_t label, vec2_t pos) {

	// grab from free list of allocate node
	ng_node_t* node = state->node_free;
	if (node != nullptr) {
		stack_pop(state->node_free);
	} else {
		node = (ng_node_t*)arena_alloc(state->arena, sizeof(ng_node_t));
	}
	memset(node, 0, sizeof(ng_node_t));
	dll_push_back(state->node_first, state->node_last, node);
	state->node_count++;

	// fill struct
	node->label = label;
	node->pos = pos;
	node->value = random_f32_range(0.0f, 10.0f);

	return node;
}

function void
ng_node_release(ng_state_t* state, ng_node_t* node) {

	// remove from list and push to free list
	dll_remove(state->node_first, state->node_last, node);
	stack_push(state->node_free, node);
	state->node_count--;
}

function void
ng_node_bring_to_front(ng_state_t* state, ng_node_t* node) {

	// reorder list
	dll_remove(state->node_first, state->node_last, node);
	dll_push_front(state->node_first, state->node_last, node);
}

// connection

function ng_connection_t*
ng_connection_create(ng_state_t* state, ng_node_t* from, ng_node_t* to) {

	// grab from free list of allocate one.
	ng_connection_t* connection = state->connection_free;
	if (connection != nullptr) {
		stack_pop(state->connection_free);
	} else {
		connection = (ng_connection_t*)arena_alloc(state->arena, sizeof(ng_connection_t));
	}
	memset(connection, 0, sizeof(ng_connection_t));
	dll_push_back(state->connection_first, state->connection_last, connection);
	state->connection_count++;

	// fill struct
	connection->from = from;
	connection->to = to;

	return connection;
}

function void
ng_connection_release(ng_state_t* state, ng_connection_t* connection) {
	dll_remove(state->connection_first, state->connection_last, connection);
	stack_push(state->connection_free, connection);
	state->connection_count--;
}


// ui custom draw

function void
ng_node_background_draw_function(ui_frame_t* frame) {

	// draw grid
	f32 offset_x = fmodf(frame->view_offset_last.x, 25.0f);
	f32 offset_y = fmodf(frame->view_offset_last.y, 25.0f);
	for (f32 x = frame->rect.x0 - offset_x; x < frame->rect.x1 - offset_x + 25.0f; x += 25.0f) {
		for (f32 y = frame->rect.y0 - offset_y; y < frame->rect.y1 - offset_y + 25.0f; y += 25.0f) {
			f32 radius = 1.25f;
			draw_set_next_color(color(0x303030ff));
			draw_circle(vec2(x, y), radius, 0.0f, 360.0f);
		}
	}

}

function void
ng_connection_draw_function(ui_frame_t* frame) {

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