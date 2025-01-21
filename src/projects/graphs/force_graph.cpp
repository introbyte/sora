// force_graph_cpp

#ifndef FORCE_GRAPH_CPP
#define FORCE_GRAPH_CPP

// implementation

// state

function fg_state_t* 
fg_create() {

	// allocate memory
	arena_t* arena = arena_create(megabytes(64));

	// create state
	fg_state_t* state = (fg_state_t*)arena_alloc(arena, sizeof(fg_state_t));

	// fill state
	state->arena = arena;
	state->node_first = nullptr;
	state->node_last = nullptr;
	state->node_free = nullptr;
	state->node_active = nullptr;
	
	state->link_first = nullptr;
	state->link_last = nullptr;
	state->link_free = nullptr;

	state->link_size = 75.0f;
	state->link_strength = 2.0f;
	state->repulsive_strength = 5000.0f;
	state->damping = 0.99f;
	state->iterations = 25;

	return state;
}

function void 
fg_release(fg_state_t* state) {

	// release memory
	arena_release(state->arena);
}

function void
fg_update(fg_state_t* state, f32 dt) {
	
	for (i32 i = 0; i < state->iterations; i++) {
		fg_apply_spring_forces(state, dt);
		fg_apply_repulsive_forces(state, dt);
		fg_update_position(state, dt);
	}

	// reset node active
	state->node_active = nullptr;

}

// simulation

function void
fg_apply_spring_forces(fg_state_t* state, f32 dt) {

	for (fg_link_t* link = state->link_first; link != nullptr; link = link->next) {

		fg_node_t* node_a = link->from;
		fg_node_t* node_b = link->to;
		
		// calculate forces
		vec2_t v = vec2_sub(node_b->pos, node_a->pos);
		f32 dist = vec2_length(v);
		vec2_t dir = vec2_normalize(v);
		f32 displacement = dist - link->length;
		f32 force = state->link_strength * displacement;

		// apply force
		node_a->vel = vec2_add(node_a->vel, vec2_mul(dir, force * dt));
		node_b->vel = vec2_sub(node_b->vel, vec2_mul(dir, force * dt));

	}

}

function void
fg_apply_repulsive_forces(fg_state_t* state, f32 dt) {

	for (fg_node_t* node_a = state->node_first; node_a != nullptr; node_a = node_a->next) {
		for (fg_node_t* node_b = state->node_first; node_b != nullptr; node_b = node_b->next) {
			if (node_a == node_b) { continue; }

			// calculate forces
			vec2_t v = vec2_sub(node_b->pos, node_a->pos);
			f32 dist = vec2_length(v);
			vec2_t dir = vec2_normalize(v);

			f32 force = state->repulsive_strength / (dist * dist);

			// apply forces
			node_a->vel = vec2_sub(node_a->vel, vec2_mul(dir, force * dt));
			node_b->vel = vec2_add(node_b->vel, vec2_mul(dir, force * dt));

		}
	}

}

function void
fg_update_position(fg_state_t* state, f32 dt) {

	for (fg_node_t* node = state->node_first; node != nullptr; node = node->next) {

		// damp velocity
		node->vel = vec2_mul(node->vel, state->damping);
		
		if (node == state->node_active) {
			continue;
		}

		// apply velocity
		node->pos = vec2_add(node->pos, vec2_mul(node->vel, dt));	

	}
}

// nodes

function fg_node_t* 
fg_node_create(fg_state_t* state, vec2_t pos, f32 size) {

	// grab from free list of allocate node
	fg_node_t* node = state->node_free;
	if (node != nullptr) {
		stack_pop(state->node_free);
	} else {
		node = (fg_node_t*)arena_alloc(state->arena, sizeof(fg_node_t));
	}
	memset(node, 0, sizeof(fg_node_t));
	dll_push_back(state->node_first, state->node_last, node);

	// fill struct
	node->pos = pos;
	node->vel = vec2(0.0f);
	node->size = size;

	return node;
}

function void 
fg_node_release(fg_state_t* state, fg_node_t* node) {

	// remove from list and push to free list
	dll_remove(state->node_first, state->node_last, node);
	stack_push(state->node_free, node);
}


// links

function fg_link_t* 
fg_link_create(fg_state_t* state, fg_node_t* from, fg_node_t* to, f32 length) {

	// grab from free list of allocate link
	fg_link_t* link = state->link_free;
	if (link != nullptr) {
		stack_pop(state->link_free);
	} else {
		link = (fg_link_t*)arena_alloc(state->arena, sizeof(fg_link_t));
	}
	memset(link, 0, sizeof(fg_link_t));
	dll_push_back(state->link_first, state->link_last, link);

	// fill struct
	link->from = from;
	link->to = to;
	link->length = length;

	return link;
}

function void 
fg_link_release(fg_state_t* state, fg_link_t* link) {

	// remove from list and push to free list
	dll_remove(state->link_first, state->link_last, link);
	stack_push(state->link_free, link);
}

// ui custom draw functions

function void 
fg_node_custom_draw(ui_frame_t* frame) {
	
	vec2_t frame_center = rect_center(frame->rect);
	f32 radius = rect_width(frame->rect) * 0.5f;

	color_t col = frame->palette.accent;
	col = color_lerp(col, color_blend(col, color(0xffffff45)), frame->hover_t);
	col = color_lerp(col, color_blend(col, color(0xffffff45)), frame->active_t);

	draw_set_next_softness(0.75f);
	draw_set_next_color(col);
	draw_circle(frame_center, radius, 0.0f, 360.0f);

	draw_set_next_softness(0.75f);
	draw_set_next_thickness(1.5f);
	draw_set_next_color(color(0x050505ff));
	draw_circle(frame_center, radius + 1.0f, 0.0f, 360.0f);
}

function void
fg_link_custom_draw(ui_frame_t* frame) {

	fg_link_draw_data_t* draw_data = (fg_link_draw_data_t*)frame->custom_draw_data;

	draw_set_next_color(color(0x45454580));
	draw_set_next_thickness(2.0f);
	draw_set_next_softness(0.75f);
	draw_line(draw_data->from, draw_data->to);

}



#endif // FORCE_GRAPH_CPP