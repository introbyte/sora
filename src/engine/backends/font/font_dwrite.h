// font_dwrite.h

#ifndef FONT_DWRITE_H
#define FONT_DWRITE_H

// includes

#include <dwrite.h>

// structs

struct font_t {
	font_t* next;
	font_t* prev;

	IDWriteFontFile* file;
	IDWriteFontFace* face;
};

struct font_state_t {

	// arena
	arena_t* font_arena;
	arena_t* scratch_arena;

	// dwrite
	IDWriteFactory* dwrite_factory;
	IDWriteRenderingParams* rendering_params;
	IDWriteGdiInterop* gdi_interop;

	// font pool
	font_t* font_first;
	font_t* font_last;
	font_t* font_free;

	font_glyph_t* glyph_first;
	font_glyph_t* glyph_last;

	vec2_t root_size;
	font_atlas_node_t* root;
	gfx_texture_t* atlas_texture;
	
};

// globals

global font_state_t font_state;


#endif // FONT_DWRITE