// draw_3d.cpp

#ifndef DRAW_3D_CPP
#define DRAW_3D_CPP

// implementation

function void 
draw_3d_init() {
    
	// allocate arenas
	draw_3d_state.arena = arena_create(gigabytes(8));
    
	draw_3d_state.batch_first = nullptr;
	draw_3d_state.batch_last = nullptr;
    
	// create buffers
	draw_3d_state.vertex_buffer = gfx_buffer_create(gfx_buffer_type_vertex, draw_3d_max_buffer_size);
    
	for (i32 i = 0; i < draw_3d_max_constant_buffers; i++) {
		draw_3d_state.constant_buffers[i] = gfx_buffer_create(gfx_buffer_type_constant, kilobytes(4));
	}
    
    // stack defaults
    draw_3d_state.pipeline_default_node.v = gfx_pipeline_create();
    draw_3d_state.shader_default_node.v = { 0 };
    draw_3d_state.texture_default_node.v = draw_state.texture;
    
    
}

function void 
draw_3d_release() {
    
	// release arenas
	arena_release(draw_3d_state.arena);
    
	// release buffers
	gfx_buffer_release(draw_3d_state.vertex_buffer);
}

function void
draw_3d_begin() {
    
	// reset batches
	arena_clear(draw_3d_state.arena);
	draw_3d_state.batch_first = nullptr;
	draw_3d_state.batch_last = nullptr;
    
    // reset stacks
    draw_3d_state.pipeline_stack.top = &draw_3d_state.pipeline_default_node; 
    draw_3d_state.pipeline_stack.free = nullptr;
    draw_3d_state.pipeline_stack.auto_pop = false;
    
    draw_3d_state.shader_stack.top = &draw_3d_state.shader_default_node; 
    draw_3d_state.shader_stack.free = nullptr;
    draw_3d_state.shader_stack.auto_pop = false;
    
    draw_3d_state.texture_stack.top = &draw_3d_state.texture_default_node; 
    draw_3d_state.texture_stack.free = nullptr;
    draw_3d_state.texture_stack.auto_pop = false;
    
}

function void
draw_3d_end() {
    
	for (draw_3d_batch_t* batch = draw_3d_state.batch_first; batch != nullptr; batch = batch->next) {
        
		draw_3d_batch_state_t batch_state = batch->state;
        
		gfx_set_pipeline(batch_state.pipeline);
		gfx_set_shader(batch_state.shader);
		gfx_set_texture(batch_state.texture);
        
		for (i32 i = 0; i < draw_3d_max_constant_buffers; i++) {
			gfx_set_buffer(draw_3d_state.constant_buffers[i], i);
		}
        
        gfx_buffer_fill(draw_3d_state.vertex_buffer, batch->vertices, batch->vertex_count * batch->vertex_size);
        gfx_set_buffer(draw_3d_state.vertex_buffer, 0, batch->vertex_size);
        
		gfx_draw(batch->vertex_count);
        
	}
    
    
}

function void 
draw_3d_set_constants(void* data, u32 size, u32 slot) {
	gfx_buffer_fill(draw_3d_state.constant_buffers[slot], data, size);
}

function draw_3d_batch_t* 
draw_3d_find_batch(draw_3d_batch_state_t state, u32 vertex_size, u32 vertex_count) {
    
	draw_3d_batch_t* result = nullptr;
    
	// search through existing batches
	for (draw_3d_batch_t* batch = draw_3d_state.batch_first; batch != nullptr; batch = batch->next) {
		if (gfx_pipeline_equals(batch->state.pipeline, state.pipeline) &&
			gfx_handle_equals(batch->state.shader, state.shader) &&
			gfx_handle_equals(batch->state.texture, state.texture) &&
			batch->vertex_size == vertex_size &&
			((batch->vertex_count + vertex_count) * batch->vertex_size) < draw_3d_max_buffer_size) {
			result = batch;
			break;
		}
	}
    
	// else create a new one
	if (result == nullptr) {
        
		// allocate on arena
		result = (draw_3d_batch_t*)arena_calloc(draw_3d_state.arena, sizeof(draw_3d_batch_t));
        
		// fill struct
		result->state = state;
		result->vertex_size = vertex_size;
        
		// allocate memory for vertices
		result->vertices = arena_calloc(draw_3d_state.arena, draw_3d_max_buffer_size);
        
		// add to batch list
		dll_push_back(draw_3d_state.batch_first, draw_3d_state.batch_last, result);
	}
    
	return result;
}


function void 
draw_3d_push_mesh(void* data, u32 vertex_size, u32 vertex_count) {
	
	// find batch
	draw_3d_batch_state_t state;
    state.pipeline = draw_3d_top_pipeline();
    state.shader = draw_3d_top_shader();
    state.texture = draw_3d_top_texture();
    draw_3d_batch_t* batch = draw_3d_find_batch(state, vertex_size, vertex_count);
    
	// copy data
	memcpy((u8*)batch->vertices + (batch->vertex_count * batch->vertex_size), data, vertex_size * vertex_count);
	batch->vertex_count += vertex_count;
    
    draw_3d_auto_pop_stacks();
    
}


// stacks

function void
draw_3d_auto_pop_stacks() {
    
    if (draw_3d_state.pipeline_stack.auto_pop) { draw_3d_pop_pipeline(); draw_3d_state.pipeline_stack.auto_pop = false; }
    if (draw_3d_state.shader_stack.auto_pop) { draw_3d_pop_shader(); draw_3d_state.shader_stack.auto_pop = false; }
    if (draw_3d_state.texture_stack.auto_pop) { draw_3d_pop_texture(); draw_3d_state.texture_stack.auto_pop = false; }
    
}

// pipeline
function gfx_pipeline_t 
draw_3d_top_pipeline() {
	return draw_3d_state.pipeline_stack.top->v;
}

function gfx_pipeline_t 
draw_3d_push_pipeline(gfx_pipeline_t v) {
	draw_3d_pipeline_node_t* node = draw_3d_state.pipeline_stack.free; 
	if (node != 0) {
		stack_pop(draw_3d_state.pipeline_stack.free);
	} else {
		node = (draw_3d_pipeline_node_t*)arena_alloc(draw_3d_state.arena, sizeof(draw_3d_pipeline_node_t));
	} 
	gfx_pipeline_t old_value = draw_3d_state.pipeline_stack.top->v; 
	node->v = v; stack_push(draw_3d_state.pipeline_stack.top, node);
	draw_3d_state.pipeline_stack.auto_pop = false; 
	return old_value;
}

function gfx_pipeline_t 
draw_3d_pop_pipeline() {
	draw_3d_pipeline_node_t* popped = draw_3d_state.pipeline_stack.top; 
	if (popped != 0) {
		stack_pop(draw_3d_state.pipeline_stack.top); 
		stack_push(draw_3d_state.pipeline_stack.free, popped);
		draw_3d_state.pipeline_stack.auto_pop = false;
	} 
	return popped->v;
}

function gfx_pipeline_t 
draw_3d_set_next_pipeline(gfx_pipeline_t v) {
	draw_3d_pipeline_node_t* node = draw_3d_state.pipeline_stack.free; 
	if (node != 0) {
		stack_pop(draw_3d_state.pipeline_stack.free);
	} else {
		node = (draw_3d_pipeline_node_t*)arena_alloc(draw_3d_state.arena, sizeof(draw_3d_pipeline_node_t));
	} 
	gfx_pipeline_t old_value = draw_3d_state.pipeline_stack.top->v;
	node->v = v; 
	stack_push(draw_3d_state.pipeline_stack.top, node);
	draw_3d_state.pipeline_stack.auto_pop = true; 
	return old_value;
};


// shader
function gfx_handle_t 
draw_3d_top_shader() {
	return draw_3d_state.shader_stack.top->v;
}

function gfx_handle_t 
draw_3d_push_shader(gfx_handle_t v) {
	draw_3d_shader_node_t* node = draw_3d_state.shader_stack.free; 
	if (node != 0) {
		stack_pop(draw_3d_state.shader_stack.free);
	} else {
		node = (draw_3d_shader_node_t*)arena_alloc(draw_3d_state.arena, sizeof(draw_3d_shader_node_t));
	} 
	gfx_handle_t old_value = draw_3d_state.shader_stack.top->v; 
	node->v = v; stack_push(draw_3d_state.shader_stack.top, node);
	draw_3d_state.shader_stack.auto_pop = false; 
	return old_value;
}

function gfx_handle_t 
draw_3d_pop_shader() {
	draw_3d_shader_node_t* popped = draw_3d_state.shader_stack.top; 
	if (popped != 0) {
		stack_pop(draw_3d_state.shader_stack.top); 
		stack_push(draw_3d_state.shader_stack.free, popped);
		draw_3d_state.shader_stack.auto_pop = false;
	} 
	return popped->v;
}

function gfx_handle_t 
draw_3d_set_next_shader(gfx_handle_t v) {
	draw_3d_shader_node_t* node = draw_3d_state.shader_stack.free; 
	if (node != 0) {
		stack_pop(draw_3d_state.shader_stack.free);
	} else {
		node = (draw_3d_shader_node_t*)arena_alloc(draw_3d_state.arena, sizeof(draw_3d_shader_node_t));
	} 
	gfx_handle_t old_value = draw_3d_state.shader_stack.top->v;
	node->v = v; 
	stack_push(draw_3d_state.shader_stack.top, node);
	draw_3d_state.shader_stack.auto_pop = true; 
	return old_value;
};



// texture
function gfx_handle_t 
draw_3d_top_texture() {
	return draw_3d_state.texture_stack.top->v;
}

function gfx_handle_t 
draw_3d_push_texture(gfx_handle_t v) {
	draw_3d_texture_node_t* node = draw_3d_state.texture_stack.free; 
	if (node != 0) {
		stack_pop(draw_3d_state.texture_stack.free);
	} else {
		node = (draw_3d_texture_node_t*)arena_alloc(draw_3d_state.arena, sizeof(draw_3d_texture_node_t));
	} 
	gfx_handle_t old_value = draw_3d_state.texture_stack.top->v; 
	node->v = v; stack_push(draw_3d_state.texture_stack.top, node);
	draw_3d_state.texture_stack.auto_pop = false; 
	return old_value;
}

function gfx_handle_t 
draw_3d_pop_texture() {
	draw_3d_texture_node_t* popped = draw_3d_state.texture_stack.top; 
	if (popped != 0) {
		stack_pop(draw_3d_state.texture_stack.top); 
		stack_push(draw_3d_state.texture_stack.free, popped);
		draw_3d_state.texture_stack.auto_pop = false;
	} 
	return popped->v;
}

function gfx_handle_t 
draw_3d_set_next_texture(gfx_handle_t v) {
	draw_3d_texture_node_t* node = draw_3d_state.texture_stack.free; 
	if (node != 0) {
		stack_pop(draw_3d_state.texture_stack.free);
	} else {
		node = (draw_3d_texture_node_t*)arena_alloc(draw_3d_state.arena, sizeof(draw_3d_texture_node_t));
	} 
	gfx_handle_t old_value = draw_3d_state.texture_stack.top->v;
	node->v = v; 
	stack_push(draw_3d_state.texture_stack.top, node);
	draw_3d_state.texture_stack.auto_pop = true; 
	return old_value;
};



#endif // DRAW_3D_CPP