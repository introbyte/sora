// editor.h

#ifndef EDITOR_H
#define EDITOR_H

// structs

struct editor_constants_t {
	vec2_t window_size;
};

struct editor_instance_t {
	vec2_t p0;
	vec2_t p1;
	vec2_t p2;
	vec2_t p3;
	rect_t uv;
	vec4_t color0;
	vec4_t color1;
	vec4_t color2;
	vec4_t color3;
	i32 omit_texture;
};

struct editor_batch_t {
	editor_batch_t* next;
	editor_batch_t* prev;

	u32 instance_size;
	u32 instance_count;
	u8* data;
};

struct editor_state_t {
	arena_t* arena;
	arena_t* scratch;

	space_t* space;
	point_t cursor;
	point_t mark;

	// rendering
	gfx_buffer_t* instance_buffer;
	gfx_buffer_t* constant_buffer;

	editor_constants_t constants;

	gfx_pipeline_t pipeline;
	gfx_shader_t* shader;
	font_t* font;

	arena_t* batch_arena;
	editor_batch_t* batch_first;
	editor_batch_t* batch_last;
};

// globals
global editor_state_t editor_state;

// functions

// editor state
function void editor_init();
function void editor_release();
function void editor_update();
function void editor_render(gfx_renderer_t*);

function editor_batch_t* editor_get_batch(u32);

function void editor_push_quad(rect_t, color_t);
function void editor_push_quad(vec2_t, vec2_t, vec2_t, vec2_t, vec4_t, vec4_t, vec4_t, vec4_t);
function void editor_push_text(str_t, vec2_t, color_t);




#endif // EDITOR_H