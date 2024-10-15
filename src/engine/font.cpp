// font.cpp

#ifndef FONT_CPP
#define FONT_CPP


// interface functions

function font_glyph_t*
font_get_glyph(font_t* font, f32 size, u32 codepoint) {

	font_glyph_t* glyph = nullptr;
	u32 hash = font_glyph_hash(font, size, codepoint);

	// try to find glyph in cache
	for (font_glyph_t* current = font_state.glyph_first; current != 0; current = current->next) {

		// we found a match
		if (current->hash == hash) {
			glyph = current;
			break;
		}
	}

	// if we did not find a match, add to cache
	if (glyph == nullptr) {

		// raster the glyph on scratch arena
		font_raster_t raster = font_glyph_raster(gfx_state.scratch_arena, font, size, codepoint);
		
		// add to atlas
		vec2_t atlas_glyph_pos = font_atlas_add(raster.size);
		vec2_t atlas_glyph_size = vec2_add(atlas_glyph_pos, raster.size);
		rect_t region = { atlas_glyph_pos.x, atlas_glyph_pos.y, atlas_glyph_size.x, atlas_glyph_size.y };
		gfx_texture_fill_region(font_state.atlas_texture, region, raster.data);

		// add glyph to cache list
		glyph = (font_glyph_t*)arena_alloc(font_state.font_arena, sizeof(font_glyph_t));
		glyph->hash = hash;
		glyph->advance = raster.advance;
		glyph->pos = rect(0.0f, 0.0f, raster.size.x, raster.size.y);
		glyph->uv = rect(region.x0 / font_atlas_size, region.y0 / font_atlas_size, region.x1 / font_atlas_size, region.y1 / font_atlas_size);
		dll_push_back(font_state.glyph_first, font_state.glyph_last, glyph);

	}

	return glyph;
}

function f32 
font_text_get_width(font_t* font, f32 size, str_t string) {
	f32 width = 0.0f;
	for (u32 offset = 0; offset < string.size; offset++) {
		char c = *(string.data + offset);
		font_glyph_t* glyph = font_get_glyph(font, size, (u8)c);
		width += glyph->advance;
	}
	return width;
}

function f32 
font_text_get_height(font_t* font, f32 size) {
	font_metrics_t metrics = font_get_metrics(font, size);
	f32 h = (metrics.ascent + metrics.descent);
	return h;
}



// helper functions

function u32
font_glyph_hash(font_t* font, f32 size, u32 codepoint) {
	u32 h = *(u32*)font;
	h ^= *(u32*)&size;
	h ^= codepoint;
	return h * 2654435761u;
}

function vec2_t
font_atlas_add(vec2_t needed_size) {

	// find node with best-fit size
	vec2_t region_p0 = { 0.0f, 0.0f };
	vec2_t region_size = { 0.0f, 0.0f };

	font_atlas_node_t* node = nullptr;
	i32 node_corner = -1;

	vec2_t n_supported_size = font_state.root_size;

	const vec2_t corner_vertices[4] = {
		vec2(0.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
	};

	for (font_atlas_node_t* n = font_state.root, *next = 0; n != 0; n = next, next = 0) {

		if (n->taken) {
			break;
		}

		b8 n_can_be_allocated = (n->child_count == 0);

		if (n_can_be_allocated) {
			region_size = n_supported_size;
		}

		vec2_t child_size = vec2_mul(n_supported_size, 0.5f);


		font_atlas_node_t* best_child = nullptr;

		if (child_size.x >= needed_size.x && child_size.y >= needed_size.y) {

			for (i32 i = 0; i < 4; i++) {

				if (n->children[i] == 0) {

					n->children[i] = (font_atlas_node_t*)arena_alloc(font_state.font_arena, sizeof(font_atlas_node_t));
					n->children[i]->parent = n;
					n->children[i]->max_free_size[0] =
						n->children[i]->max_free_size[1] =
						n->children[i]->max_free_size[2] =
						n->children[i]->max_free_size[3] = vec2_mul(child_size, 0.5f);

				}

				if (n->max_free_size[i].x >= needed_size.x && n->max_free_size[i].y >= needed_size.y) {
					best_child = n->children[i];
					node_corner = i;
					vec2_t side_vertex = corner_vertices[i];
					region_p0.x += side_vertex.x * child_size.x;
					region_p0.y += side_vertex.y * child_size.y;
					break;
				}
			}
		}

		if (n_can_be_allocated && best_child == 0) {
			node = n;
		} else {
			next = best_child;
			n_supported_size = child_size;
		}

	}

	if (node != 0 && node_corner != -1) {
		node->taken = true;

		if (node->parent != 0) {
			memset(&node->parent->max_free_size[node_corner], 0, sizeof(vec2_t));
		}

		for (font_atlas_node_t* p = node->parent; p != 0; p = p->parent) {
			p->child_count += 1;
			font_atlas_node_t* parent = p->parent;
			if (parent != 0) {
				i32 p_corner = (
					p == parent->children[0] ? 0 :
					p == parent->children[1] ? 1 :
					p == parent->children[2] ? 2 :
					p == parent->children[3] ? 3 :
					-1
					);

				parent->max_free_size[p_corner].x = max(max(p->max_free_size[0].x,
					p->max_free_size[1].x),
					max(p->max_free_size[2].x,
						p->max_free_size[3].x));
				parent->max_free_size[p_corner].y = max(max(p->max_free_size[0].y,
					p->max_free_size[1].y),
					max(p->max_free_size[2].y,
						p->max_free_size[3].y));
			}
		}
	}

	vec2_t result = region_p0;

	return result;

}


// include backends

#if defined(FONT_BACKEND_DWRITE)
#include "backends/font/font_dwrite.cpp"
#elif defined(FONT_BACKEND_CORE_TEXT)
	// not implemented
#elif defined(FONT_BACKEND_FREETYPE)
	// not implemented
#endif 



#endif // FONT_CPP
