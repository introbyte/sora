// draw.cpp

#ifndef DRAW_CPP
#define DRAW_CPP

// defines
#define draw_default_init(name, value) \
draw_state.name##_default_node.v = value; \

#define draw_stack_reset(name) \
draw_state.name##_stack.top = &draw_state.name##_default_node; draw_state.name##_stack.free = 0; draw_state.name##_stack.auto_pop = 0; \

#define draw_stack_top_impl(name, type) \
function type \
draw_top_##name() { \
	return draw_state.name##_stack.top->v; \
} \

#define draw_stack_push_impl(name, type) \
function type \
draw_push_##name(type v) { \
draw_##name##_node_t* node = draw_state.name##_stack.free; \
if (node != 0) { \
	stack_pop(draw_state.name##_stack.free); \
} else { \
	node = (draw_##name##_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_##name##_node_t)); \
} \
type old_value = draw_state.name##_stack.top->v; \
node->v = v; \
stack_push(draw_state.name##_stack.top, node); \
draw_state.name##_stack.auto_pop = 0; \
return old_value; \
} \

#define draw_stack_pop_impl(name, type) \
function type \
draw_pop_##name() { \
draw_##name##_node_t* popped = draw_state.name##_stack.top; \
if (popped != 0) { \
	stack_pop(draw_state.name##_stack.top); \
	stack_push(draw_state.name##_stack.free, popped); \
	draw_state.name##_stack.auto_pop = 0; \
} \
return popped->v; \
} \

#define draw_stack_set_next_impl(name, type) \
function type \
draw_set_next_##name(type v) { \
draw_##name##_node_t* node = draw_state.name##_stack.free; \
if (node != 0) { \
	stack_pop(draw_state.name##_stack.free); \
} else { \
	node = (draw_##name##_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_##name##_node_t)); \
} \
type old_value = draw_state.name##_stack.top->v; \
node->v = v; \
stack_push(draw_state.name##_stack.top, node); \
draw_state.name##_stack.auto_pop = 1; \
return old_value; \
} \

#define draw_stack_auto_pop_impl(name) \
if (draw_state.name##_stack.auto_pop) { draw_pop_##name(); draw_state.name##_stack.auto_pop = 0; }

#define draw_stack_impl(name, type)\
draw_stack_top_impl(name, type)\
draw_stack_push_impl(name, type)\
draw_stack_pop_impl(name, type)\
draw_stack_set_next_impl(name, type)\

// implementation

function void 
draw_init() {

	// arenas
	draw_state.batch_arena = arena_create(gigabytes(2));
	draw_state.resource_arena = arena_create(gigabytes(2));
	draw_state.scratch_arena = arena_create(megabytes(64));

	// create buffers
	draw_state.vertex_buffer = gfx_buffer_create(gfx_buffer_type_vertex, draw_max_buffer_size, nullptr);

	// create constant buffers
	draw_state.constant_buffers[0] = gfx_buffer_create(gfx_buffer_type_constant, kilobytes(1), nullptr);
	draw_state.constant_buffers[1] = gfx_buffer_create(gfx_buffer_type_constant, kilobytes(1), nullptr);
	draw_state.constant_buffers[2] = gfx_buffer_create(gfx_buffer_type_constant, kilobytes(1), nullptr);
	draw_state.constant_buffers[3] = gfx_buffer_create(gfx_buffer_type_constant, kilobytes(1), nullptr);

	// default assets
	u32 default_texture_data = 0xffffffff;
	draw_state.default_texture = gfx_texture_create(uvec2(1,1),	gfx_texture_format_rgba8, &default_texture_data);
	draw_state.default_shader = gfx_shader_load(str("res/shaders/default.hlsl"), nullptr, 0);
	draw_state.default_pipeline.fill_mode = gfx_fill_solid;
	draw_state.default_pipeline.cull_mode = gfx_cull_mode_back;
	draw_state.default_pipeline.topology = gfx_topology_tris;
	draw_state.default_pipeline.filter_mode = gfx_filter_linear;
	draw_state.default_pipeline.wrap_mode = gfx_wrap_clamp;
	draw_state.default_pipeline.depth_mode = gfx_depth;
	draw_state.default_pipeline.viewport = rect(0.0f, 0.0f, 0.0f, 0.0f);
	draw_state.default_pipeline.scissor = rect(0.0f, 0.0f, 0.0f, 0.0f);

	// stack defaults
	draw_default_init(texture, draw_state.default_texture);
	draw_default_init(shader, draw_state.default_shader);
	draw_default_init(pipeline, draw_state.default_pipeline);

	draw_state.batch_first = draw_state.batch_last = nullptr;

}

function void 
draw_release() {

	// release buffers
	gfx_buffer_release(draw_state.vertex_buffer);
	gfx_buffer_release(draw_state.constant_buffers[0]);
	gfx_buffer_release(draw_state.constant_buffers[1]);
	gfx_buffer_release(draw_state.constant_buffers[2]);
	gfx_buffer_release(draw_state.constant_buffers[3]);

	// release arena
	arena_release(draw_state.batch_arena);
	arena_release(draw_state.resource_arena);
	arena_release(draw_state.scratch_arena);

}

function void 
draw_update() {

	// reset stacks
	draw_stack_reset(texture);
	draw_stack_reset(shader);
	draw_stack_reset(pipeline);

}

function void 
draw_submit() {
	
	// draw each batch
	for (draw_batch_t* batch = draw_state.batch_first; batch != 0; batch = batch->next) {

		// load into buffers
		gfx_buffer_fill(draw_state.vertex_buffer, batch->data, batch->state.vertex_size * batch->vertex_count);

		// set pipeline
		gfx_set_pipeline(batch->state.pipeline_state);


		// set buffers
		gfx_set_buffer(draw_state.vertex_buffer, 0, batch->state.vertex_size);
		gfx_set_buffer(draw_state.constant_buffers[0], 0);
		gfx_set_buffer(draw_state.constant_buffers[1], 1);
		gfx_set_buffer(draw_state.constant_buffers[2], 2);
		gfx_set_buffer(draw_state.constant_buffers[3], 3);

		// set textures
		gfx_set_texture(batch->state.textures, 0);
		gfx_set_shader(batch->state.shader);

		// draw
		gfx_draw(batch->vertex_count);

	}

	// clear batches
	arena_clear(draw_state.batch_arena);
	draw_state.batch_first = draw_state.batch_last = nullptr;

}

function void 
draw_set_constants(void* data, u32 size, u32 slot) {
	gfx_buffer_fill(draw_state.constant_buffers[slot], data, size);
}

function b8 
draw_batch_state_equals(draw_batch_state_t state_a, draw_batch_state_t state_b) {

	// this looks stupid. but it works better than memcmp
	if ((state_a.shader = state_b.shader) &&
		(state_a.textures = state_b.textures) &&
		(state_a.pipeline_state.fill_mode == state_b.pipeline_state.fill_mode) &&
		(state_a.pipeline_state.cull_mode == state_b.pipeline_state.cull_mode) &&
		(state_a.pipeline_state.topology == state_b.pipeline_state.topology) &&
		(state_a.pipeline_state.filter_mode == state_b.pipeline_state.filter_mode) &&
		(state_a.pipeline_state.wrap_mode == state_b.pipeline_state.wrap_mode) &&
		(state_a.pipeline_state.depth_mode == state_b.pipeline_state.depth_mode) &&
		(state_a.pipeline_state.viewport.x0 == state_b.pipeline_state.viewport.x0) &&
		(state_a.pipeline_state.viewport.x1 == state_b.pipeline_state.viewport.x1) &&
		(state_a.pipeline_state.viewport.y0 == state_b.pipeline_state.viewport.y0) &&
		(state_a.pipeline_state.viewport.y1 == state_b.pipeline_state.viewport.y1) &&
		(state_a.pipeline_state.scissor.x0 == state_b.pipeline_state.scissor.x0) &&
		(state_a.pipeline_state.scissor.x1 == state_b.pipeline_state.scissor.x1) &&
		(state_a.pipeline_state.scissor.y0 == state_b.pipeline_state.scissor.y0) &&
		(state_a.pipeline_state.scissor.y1 == state_b.pipeline_state.scissor.y1) &&
		(state_a.instanced == state_b.instanced) &&
		(state_a.vertex_size == state_b.vertex_size)) {
		return true;
	}
	return false;
}

function draw_batch_t*
draw_batch_find(draw_batch_state_t state, u32 count) {
	
	// try to find batch in batch list
	for (draw_batch_t* batch = draw_state.batch_first; batch != 0; batch = batch->next) {
		b8 batch_equals = draw_batch_state_equals(batch->state, state);
		b8 batch_has_space = ((batch->vertex_count + count) * batch->state.vertex_size <= draw_max_buffer_size);
		if (batch_equals && batch_has_space) {
			return batch;
		}
	}
	
	// else create one
	draw_batch_t* batch = (draw_batch_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_batch_t));
	
	// fill struct
	batch->state = state;
	batch->vertex_count = 0;
	batch->data = (u8*)arena_alloc(draw_state.batch_arena, draw_max_buffer_size);

	// add to batch list
	dll_push_back(draw_state.batch_first, draw_state.batch_last, batch);

	// return created batch
	return batch;
}

function void
draw_push_vertices(void* vertices, u32 vertex_size, u32 vertex_count) {

	// find batch
	draw_batch_state_t state = { 0 };
	state.textures = draw_top_texture();
	state.shader = draw_top_shader();
	state.pipeline_state = draw_top_pipeline();
	state.instanced = false;
	state.vertex_size = vertex_size;
	draw_batch_t* batch = draw_batch_find(state, vertex_count);

	// copy data
	memcpy(batch->data + (batch->vertex_count * batch->state.vertex_size), vertices, vertex_size * vertex_count);
	batch->vertex_count += vertex_count;

	draw_auto_pop_stacks();
}


// stack functions

function void 
draw_auto_pop_stacks() {
	draw_stack_auto_pop_impl(texture);
	draw_stack_auto_pop_impl(shader);
	draw_stack_auto_pop_impl(pipeline);
}

draw_stack_impl(texture, gfx_texture_t*);
draw_stack_impl(shader, gfx_shader_t*);
draw_stack_impl(pipeline, gfx_pipeline_t);


#endif // DRAW_CPP