// draw_3d.h

#ifndef DRAW_3D_H
#define DRAW_3D_H

#define draw_3d_max_buffer_size (2<<15)
#define draw_3d_max_constant_buffers 8
// structs

struct draw_3d_batch_state_t {
	gfx_pipeline_t pipeline;
	gfx_handle_t shader;
	gfx_handle_t texture;
};

struct draw_3d_batch_t {
	draw_3d_batch_t* next;
	draw_3d_batch_t* prev;

	draw_3d_batch_state_t state;

	void* vertices;
	u32 vertex_count;
	u32 vertex_size;
};

struct draw_3d_state_t {

	arena_t* arena;

	// buffers
	gfx_handle_t vertex_buffer;
	gfx_handle_t constant_buffers[draw_3d_max_constant_buffers];

	// batches
	draw_3d_batch_t* batch_first;
	draw_3d_batch_t* batch_last;

};

// globals

global draw_3d_state_t draw_3d_state;

// functions

function void draw_3d_init();
function void draw_3d_release();
function void draw_3d_begin();
function void draw_3d_end();

function draw_3d_batch_t* draw_3d_find_batch(draw_3d_batch_state_t state, u32 vertex_size, u32 vertex_count);

function void draw_3d_set_constants(void* data, u32 size, u32 slot = 0);

function void draw_push_mesh(void* data, u32 vertex_size, u32 vertex_count);



#endif // DRAW_3D_H