// draw.h

#ifndef DRAW_H
#define DRAW_H

// defines

#define draw_max_buffer_size kilobytes(64)

#define draw_stack_node_decl(name, type) struct draw_##name##_node_t { draw_##name##_node_t* next; type v; };
#define draw_stack_decl(name) struct { draw_##name##_node_t* top; draw_##name##_node_t* free; b8 auto_pop; } name##_stack;
#define gfx_stack_default_decl(name) draw_##name##_node_t name##_default_node;
#define draw_stack_top_func(name, type) function type draw_top_##name();
#define draw_stack_push_func(name, type) function type draw_push_##name(type);
#define draw_stack_pop_func(name, type) function type draw_pop_##name();
#define draw_stack_set_next_func(name, type) function type draw_set_next_##name(type);
#define draw_stack_func_decl(name, type)\
draw_stack_top_func(name, type)\
draw_stack_push_func(name, type)\
draw_stack_pop_func(name, type)\
draw_stack_set_next_func(name, type)\


// structs

struct draw_batch_state_t {
	gfx_shader_t* shader;
	gfx_texture_t* textures;
	gfx_pipeline_t pipeline_state;
	b8 instanced;
	union {
		u32 vertex_size;
		u32 instance_size;
	};
};

struct draw_batch_t {
	draw_batch_t* next;
	draw_batch_t* prev;

	draw_batch_state_t state;
	
	union {
		u32 vertex_count;
		u32 instance_count;
	};
	u8* data;
};

// stacks
draw_stack_node_decl(texture, gfx_texture_t*);
draw_stack_node_decl(shader, gfx_shader_t*);
draw_stack_node_decl(pipeline, gfx_pipeline_t);

struct draw_state_t {

	// arenas
	arena_t* batch_arena;
	arena_t* resource_arena;
	arena_t* scratch_arena;

	// buffer
	gfx_buffer_t* vertex_buffer;
	gfx_buffer_t* constant_buffers[4];

	// default assets
	gfx_texture_t* default_texture;
	gfx_shader_t* default_shader;
	gfx_pipeline_t default_pipeline;

	// batches
	draw_batch_t* batch_first;
	draw_batch_t* batch_last;

	// stacks
	draw_stack_decl(texture);
	draw_stack_decl(shader);
	draw_stack_decl(pipeline);

	// stack defaults
	gfx_stack_default_decl(texture);
	gfx_stack_default_decl(shader);
	gfx_stack_default_decl(pipeline);

};

// globals

global draw_state_t draw_state;

// functions

function void draw_init();
function void draw_release();
function void draw_update();
function void draw_submit();
function void draw_set_constants(void*, u32, u32 = 0);

function b8 draw_batch_state_equals(draw_batch_state_t, draw_batch_state_t);
function draw_batch_t* draw_batch_find(draw_batch_state_t, u32 = 1);

function void draw_push_vertices(void*, u32, u32);


// stack functions

function void draw_auto_pop_stacks();

draw_stack_func_decl(texture, gfx_texture_t*);
draw_stack_func_decl(shader, gfx_shader_t*);
draw_stack_func_decl(pipeline, gfx_pipeline_t);



#endif // DRAW_H