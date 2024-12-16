// font.h

#ifndef FONT_H
#define FONT_H

#define font_atlas_size 2048.0f

// structs

struct font_metrics_t {
	f32 line_gap;
	f32 ascent;
	f32 descent;
	f32 capital_height;
	f32 x_height;
};

struct font_glyph_t {
	font_glyph_t* next;
	font_glyph_t* prev;

	u32 hash;

	rect_t pos;
	rect_t uv;
	f32 advance;
	f32 height;
};

struct font_atlas_node_t {
	font_atlas_node_t* parent;
	font_atlas_node_t* children[4];
	u32 child_count;

	vec2_t max_free_size[4];
	b8 taken;
};

struct font_raster_t {
	vec2_t size;
	f32 advance;
	u8* data;
};

struct font_t; // backend

struct font_state_t; // backend

// functions

// state
function void font_init(); // backend
function void font_release(); // backend

// interface
function font_t* font_open(str_t); // backend
function void font_close(font_t*); // backend
function font_metrics_t font_get_metrics(font_t*, f32); // backend
function font_glyph_t* font_get_glyph(font_t*, f32, u32);
function f32 font_text_get_width(font_t*, f32, str_t);
function f32 font_text_get_height(font_t*, f32, str_t);

function str_t font_text_truncate(arena_t* arena, font_t* font, f32 font_size, str_t string, f32 max_width, str_t trail_string);

// helper
function u32 font_glyph_hash(font_t*, f32, u32);
function font_raster_t font_glyph_raster(arena_t*, font_t*, f32, u32); // backend
function vec2_t font_atlas_add(vec2_t);

// include backends

#define FONT_BACKEND_DWRITE

#if defined(FONT_BACKEND_DWRITE)
	#include "backends/font/font_dwrite.h"
#elif defined(FONT_BACKEND_CORE_TEXT)
	// not implemented
#elif defined(FONT_BACKEND_FREETYPE)
	// not implemented
#endif 

#endif // FONT_H