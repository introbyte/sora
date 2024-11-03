// draw.h

#ifndef DRAW_H
#define DRAW_H

// this layer manages mesh batching 
// by grouping data based on shader, 
// textures, and pipeline state. it 
// submits the batched data to the
// gfx layer for rendering.
// additionally, it offers a simple 
// 2d drawing api, primarily used by 
// the ui layer.
//
// todo:
// 
// [ ] - add 2d drawing api.
//    [ ] - add draw stacks (color, rounding, etc).
//    [ ] - get clipping to work in shader.
//    [ ] - add rect, quad, and text.
//    [ ] - add other shapes.
//


// defines

#define draw_max_buffer_size kilobytes(256)

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

// enums

enum draw_shape {
	_draw_shape_null,
	draw_shape_rect,
	draw_shape_quad,
	draw_shape_line,
	draw_shape_circle,
	draw_shape_arc,
	draw_shape_tri,
};

// structs

// 2d instance
struct draw_2d_instance_t {

	rect_t bbox;
	rect_t uv;
	
	// position
	vec2_t p0;
	vec2_t p1;
	vec2_t p2;
	vec2_t p3;

	// color
	vec4_t col0;
	vec4_t col1;
	vec4_t col2;
	vec4_t col3;

	// radius
	f32 r0;
	f32 r1;
	f32 r2;
	f32 r3;

	// styling
	f32 thickness;
	f32 softness;
	f32 omit_texture;
	draw_shape shape;
};


// batches
struct draw_batch_state_t {
	gfx_shader_t* shader;
	gfx_texture_t* textures;
	gfx_pipeline_t pipeline_state;
	b8 instanced;

	u32 vertex_size;

	u32 instance_size;
	u32 instance_vertex_count;
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

draw_stack_node_decl(color0, color_t);
draw_stack_node_decl(color1, color_t);
draw_stack_node_decl(color2, color_t);
draw_stack_node_decl(color3, color_t);

draw_stack_node_decl(radius0, f32);
draw_stack_node_decl(radius1, f32);
draw_stack_node_decl(radius2, f32);
draw_stack_node_decl(radius3, f32);

draw_stack_node_decl(thickness, f32);
draw_stack_node_decl(softness, f32);

draw_stack_node_decl(font, font_t*);
draw_stack_node_decl(font_size, f32);

// state
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
	font_t* default_font;

	// batches
	draw_batch_t* batch_first;
	draw_batch_t* batch_last;

	// stacks
	draw_stack_decl(texture);
	draw_stack_decl(shader);
	draw_stack_decl(pipeline);

	draw_stack_decl(color0);
	draw_stack_decl(color1);
	draw_stack_decl(color2);
	draw_stack_decl(color3);
	
	draw_stack_decl(radius0);
	draw_stack_decl(radius1);
	draw_stack_decl(radius2);
	draw_stack_decl(radius3);

	draw_stack_decl(thickness);
	draw_stack_decl(softness);

	draw_stack_decl(font);
	draw_stack_decl(font_size);


	// stack defaults
	gfx_stack_default_decl(texture);
	gfx_stack_default_decl(shader);
	gfx_stack_default_decl(pipeline);
	
	gfx_stack_default_decl(color0);
	gfx_stack_default_decl(color1);
	gfx_stack_default_decl(color2);
	gfx_stack_default_decl(color3);

	gfx_stack_default_decl(radius0);
	gfx_stack_default_decl(radius1);
	gfx_stack_default_decl(radius2);
	gfx_stack_default_decl(radius3);

	gfx_stack_default_decl(thickness);
	gfx_stack_default_decl(softness);

	gfx_stack_default_decl(font);
	gfx_stack_default_decl(font_size);

};

// globals

global draw_state_t draw_state;

// functions

function void draw_init();
function void draw_release();
function void draw_update();
function void draw_submit();

function b8 draw_batch_state_equals(draw_batch_state_t, draw_batch_state_t);
function draw_batch_t* draw_batch_find(draw_batch_state_t, u32 = 1);

function void draw_push_constants(void*, u32, u32 = 0);
function void draw_push_vertices(void*, u32, u32);

function void draw_push_rect(rect_t);
function void draw_push_quad();
function void draw_push_line();
function void draw_push_text(str_t text, vec2_t pos);
function void draw_push_circle(vec2_t, f32);
function void draw_push_arc();
function void draw_push_tri(vec2_t p0, vec2_t p1, vec2_t p2);

// stack functions

function void draw_auto_pop_stacks();

// batch state stacks
draw_stack_func_decl(texture, gfx_texture_t*);
draw_stack_func_decl(shader, gfx_shader_t*);
draw_stack_func_decl(pipeline, gfx_pipeline_t);

// draw stacks
draw_stack_func_decl(color0, color_t);
draw_stack_func_decl(color1, color_t);
draw_stack_func_decl(color2, color_t);
draw_stack_func_decl(color3, color_t);

draw_stack_func_decl(radius0, f32);
draw_stack_func_decl(radius1, f32);
draw_stack_func_decl(radius2, f32);
draw_stack_func_decl(radius3, f32);

draw_stack_func_decl(thickness, f32);
draw_stack_func_decl(softness, f32);

draw_stack_func_decl(font, font_t*);
draw_stack_func_decl(font_size, f32);

// group stacks
function void draw_push_color(color_t);
function void draw_set_next_color(color_t);
function void draw_pop_color();

function void draw_push_radius(f32);
function void draw_set_next_radius(f32);
function void draw_push_radius(vec4_t);
function void draw_set_next_radius(vec4_t);
function void draw_pop_radius();

// draw stacks



#endif // DRAW_H