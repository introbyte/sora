// draw_3d.h

#ifndef DRAW_3D_H
#define DRAW_3D_H

#define draw_3d_max_buffer_size (2<<15)
#define draw_3d_max_constant_buffers 8

// structs

// batches

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

// stacks
struct draw_3d_pipeline_node_t { draw_3d_pipeline_node_t* next; gfx_pipeline_t v; };
struct draw_3d_pipeline_stack_t { draw_3d_pipeline_node_t* top; draw_3d_pipeline_node_t* free; b8 auto_pop; };

struct draw_3d_shader_node_t { draw_3d_shader_node_t* next; gfx_handle_t v; };
struct draw_3d_shader_stack_t { draw_3d_shader_node_t* top; draw_3d_shader_node_t* free; b8 auto_pop; };

struct draw_3d_texture_node_t { draw_3d_texture_node_t* next; gfx_handle_t v; };
struct draw_3d_texture_stack_t { draw_3d_texture_node_t* top; draw_3d_texture_node_t* free; b8 auto_pop; };


// state
struct draw_3d_state_t {
    
	arena_t* arena;
    
	// buffers
	gfx_handle_t vertex_buffer;
	gfx_handle_t constant_buffers[draw_3d_max_constant_buffers];
    
	// batches
	draw_3d_batch_t* batch_first;
	draw_3d_batch_t* batch_last;
    
    // stacks
    draw_3d_pipeline_stack_t pipeline_stack;
    draw_3d_shader_stack_t shader_stack;
    draw_3d_texture_stack_t texture_stack;
    
    // stack defaults
    draw_3d_pipeline_node_t pipeline_default_node;
    draw_3d_shader_node_t shader_default_node;
    draw_3d_texture_node_t texture_default_node;
    
    // default assets
    gfx_handle_t default_texture;
    
};

// globals

global draw_3d_state_t draw_3d_state;

// functions

function void draw_3d_init();
function void draw_3d_release();
function void draw_3d_begin();
function void draw_3d_end();
function void draw_3d_set_constants(void* data, u32 size, u32 slot = 0);

function draw_3d_batch_t* draw_3d_find_batch(draw_3d_batch_state_t state, u32 vertex_size, u32 vertex_count);

function void draw_3d_push_mesh(void* data, u32 vertex_size, u32 vertex_count);

// stack functions
function void draw_3d_auto_pop_stacks();

function gfx_pipeline_t draw_3d_top_pipeline();
function gfx_pipeline_t draw_3d_push_pipeline(gfx_pipeline_t);
function gfx_pipeline_t draw_3d_pop_pipeline();
function gfx_pipeline_t draw_3d_set_next_pipeline(gfx_pipeline_t);

function gfx_handle_t draw_3d_top_shader();
function gfx_handle_t draw_3d_push_shader(gfx_handle_t);
function gfx_handle_t draw_3d_pop_shader();
function gfx_handle_t draw_3d_set_next_shader(gfx_handle_t);

function gfx_handle_t draw_3d_top_texture();
function gfx_handle_t draw_3d_push_texture(gfx_handle_t);
function gfx_handle_t draw_3d_pop_texture();
function gfx_handle_t draw_3d_set_next_texture(gfx_handle_t);

#endif // DRAW_3D_H