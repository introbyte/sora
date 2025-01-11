// draw.h

#ifndef DRAW_H
#define DRAW_H

// draw layer todos:
//
// [x] - combine all textures to an array and package index in instance data.
//       this keeps our one draw call feature.
// [ ] - add fancy text.
//
//

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

#define draw_max_clip_rects 128
#define draw_max_textures 16

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
	rect_t clip_masks[draw_max_clip_rects];
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
	u32 indices;
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

draw_stack_node_decl(font, font_handle_t);
draw_stack_node_decl(font_size, f32);
draw_stack_node_decl(clip_mask, rect_t);

draw_stack_node_decl(texture, gfx_handle_t);

struct draw_state_t {

	// assets
	gfx_handle_t instance_buffer;
	gfx_handle_t constant_buffer;
	draw_constants_t constants;
	i32 clip_mask_count;
	gfx_pipeline_t pipeline;
	gfx_handle_t shader;
	gfx_handle_t texture;
	font_handle_t font;	
	
	gfx_handle_t texture_list[draw_max_textures];
	u32 texture_count;

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

	draw_stack_decl(texture);

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

	gfx_stack_default_decl(texture);

};

// globals
global draw_state_t draw_state;

// functions

function void draw_init();
function void draw_release();
function void draw_begin(gfx_handle_t);
function void draw_end(gfx_handle_t);

function draw_instance_t* draw_get_instance();

function i32 draw_get_texture_index(gfx_handle_t texture);
function i32 draw_get_clip_mask_index(rect_t rect);

function void draw_rect(rect_t);
function void draw_image(rect_t);
function void draw_quad(vec2_t, vec2_t, vec2_t, vec2_t);
function void draw_line(vec2_t, vec2_t);
function void draw_circle(vec2_t, f32, f32, f32);
function void draw_tri(vec2_t, vec2_t, vec2_t);
function void draw_text(str_t, vec2_t);

function void draw_bezier(vec2_t p0, vec2_t p1, vec2_t c0, vec2_t c1);

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

draw_stack_func_decl(font, font_handle_t);
draw_stack_func_decl(font_size, f32);

draw_stack_func_decl(clip_mask, rect_t);

draw_stack_func_decl(texture, gfx_handle_t);

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