// draw.h

#ifndef DRAW_H
#define DRAW_H

// defines

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
	draw_shape_none,
	draw_shape_rect,
	draw_shape_quad,
	draw_shape_line,
	draw_shape_circle,
	draw_shape_ring,
	draw_shape_tri,
};

// structs

struct draw_constants_t {
	vec2_t window_size;
	vec2_t padding;
	rect_t clip_masks[128];
};

struct draw_instance_t {
	rect_t bbox;
	rect_t tex;
	vec2_t point0;
	vec2_t point1;
	vec2_t point2;
	vec2_t point3;
	vec4_t color0;
	vec4_t color1;
	vec4_t color2;
	vec4_t color3;
	vec4_t radii;
	f32 thickness;
	f32 softness;
	f32 omit_texture;
	i32 shape;
	i32 clip_index;
};

struct draw_batch_t {
	draw_batch_t* next;
	draw_batch_t* prev;

	draw_instance_t* instances;
	u32 instance_count;
};

// stacks
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
draw_stack_node_decl(clip_mask, rect_t);

struct draw_state_t {

	// assets
	gfx_buffer_t* instance_buffer;
	gfx_buffer_t* constant_buffer;
	draw_constants_t constants;
	i32 clip_mask_count;
	gfx_pipeline_t pipeline;
	gfx_shader_t* shader;
	font_t* font;
	
	// batches
	arena_t* batch_arena;
	draw_batch_t* batch_first;
	draw_batch_t* batch_last;

	// stacks
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

	draw_stack_decl(clip_mask);

	// stack defaults
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

	gfx_stack_default_decl(clip_mask);

};

// globals
global draw_state_t draw_state;

// functions

function void draw_init();
function void draw_release();
function void draw_begin(gfx_renderer_t*);
function void draw_end(gfx_renderer_t*);

function draw_instance_t* draw_get_instance();

function void draw_rect(rect_t);
function void draw_image(rect_t);
function void draw_quad(vec2_t, vec2_t, vec2_t, vec2_t);
function void draw_line(vec2_t, vec2_t);
function void draw_circle(vec2_t, f32, f32, f32);
function void draw_tri(vec2_t, vec2_t, vec2_t);
function void draw_text(str_t, vec2_t);

// stacks
function void draw_auto_pop_stacks();

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

draw_stack_func_decl(clip_mask, rect_t);

// group stacks
function void draw_push_color(color_t);
function void draw_set_next_color(color_t);
function void draw_pop_color();

function vec4_t draw_top_radii();
function void draw_push_radii(f32);
function void draw_push_radii(vec4_t);
function void draw_set_next_radii(f32);
function void draw_set_next_radii(vec4_t);
function void draw_pop_radii();

#endif // DRAW_H