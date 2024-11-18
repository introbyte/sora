// gfx.cpp

#ifndef GFX_CPP
#define GFX_CPP

// pipeline

function gfx_pipeline_t 
gfx_pipeline_create() {
	gfx_pipeline_t result = { 0 };
	result.fill_mode = gfx_fill_solid;
	result.cull_mode = gfx_cull_mode_back;
	result.topology = gfx_topology_tris;
	result.filter_mode = gfx_filter_linear;
	result.wrap_mode = gfx_wrap_clamp;
	result.depth_mode = gfx_depth;
	result.viewport = rect(0.0f, 0.0f, 0.0f, 0.0f);
	result.scissor = rect(0.0f, 0.0f, 0.0f, 0.0f);
	return result;
}


// helper functions
function b8
_texture_format_is_depth(gfx_texture_format format) {
	b8 result = false;
	switch (format) {
		case gfx_texture_format_d24s8:
		case gfx_texture_format_d32: {
			result = true;
			break;
		}
	}
	return result;
}

// per backend includes

#ifdef GFX_BACKEND_D3D11
#include "backends/gfx/gfx_d3d11.cpp"
#elif defined(GFX_BACKEND_OPENGL)
#include "backends/gfx/gfx_opengl.cpp"
#elif defined(GFX_BACKEND_METAL)
#include "backends/gfx/gfx_metal.cpp"
#endif 

#endif // GFX_CPP