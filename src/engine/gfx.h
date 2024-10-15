// gfx.h

#ifndef GFX_H
#define GFX_H

// enums

// NOTE: not sure if this will be used.
enum gfx_backend {
	_gfx_backend_null,
	gfx_backend_d3d11,
	gfx_backend_opengl,
	gfx_backend_metal,
};

enum gfx_usage {
	_gfx_usage_null,
	gfx_usage_static,
	gfx_usage_dynamic,
	gfx_usage_stream,
};

enum gfx_buffer_type {
	_gfx_buffer_type_null,
	gfx_buffer_type_vertex,
	gfx_buffer_type_index,
	gfx_buffer_type_constant,
};

enum gfx_texture_format {
	_gfx_texture_format_null,
	gfx_texture_format_r8,
	gfx_texture_format_rg8,
	gfx_texture_format_rgba8,
	gfx_texture_format_bgra8,
	gfx_texture_format_r16,
	gfx_texture_format_rgba16,
	gfx_texture_format_r32,
	gfx_texture_format_rg32,
	gfx_texture_format_rgba32,
	gfx_texture_format_d24s8,
	gfx_texture_format_d32,
};

enum gfx_texture_type {
	_gfx_texture_type_null,
	gfx_texture_type_2d,
	gfx_texture_type_3d,
};

enum gfx_filter_mode {
	_gfx_filter_null,
	gfx_filter_linear,
	gfx_filter_nearest,
};

enum gfx_wrap_mode {
	_gfx_wrap_null,
	gfx_wrap_repeat,
	gfx_wrap_clamp,
};

enum gfx_fill_mode {
	_gfx_fill_null,
	gfx_fill_solid,
	gfx_fill_wireframe,
};

enum gfx_depth_mode {
	_gfx_depth_null,
	gfx_depth,
	gfx_depth_none,
};

enum gfx_topology_type {
	_gfx_topology_null,
	gfx_topology_points,
	gfx_topology_lines,
	gfx_topology_line_strip,
	gfx_topology_tris,
	gfx_topology_tri_strip,
};

enum gfx_vertex_format {
	_gfx_vertex_format_null,
	gfx_vertex_format_float,
	gfx_vertex_format_float2,
	gfx_vertex_format_float3,
	gfx_vertex_format_float4,
	gfx_vertex_format_uint,
	gfx_vertex_format_uint2,
	gfx_vertex_format_uint3,
	gfx_vertex_format_uint4,
	gfx_vertex_format_int,
	gfx_vertex_format_int2,
	gfx_vertex_format_int3,
	gfx_vertex_format_int4,
};

enum gfx_vertex_class {
	_gfx_vertex_class_null,
	gfx_vertex_class_per_vertex,
	gfx_vertex_class_per_instance,
};

enum gfx_uniform_type {
	_gfx_uniform_type_null,
	gfx_uniform_type_float,
	gfx_uniform_type_float2,
	gfx_uniform_type_float3,
	gfx_uniform_type_float4,
	gfx_uniform_type_uint,
	gfx_uniform_type_uint2,
	gfx_uniform_type_uint3,
	gfx_uniform_type_uint4,
	gfx_uniform_type_int,
	gfx_uniform_type_int2,
	gfx_uniform_type_int3,
	gfx_uniform_type_int4,
	gfx_uniform_type_mat4,
};

enum gfx_cull_mode {
	_gfx_cull_mode_null,
	gfx_cull_mode_none,
	gfx_cull_mode_front,
	gfx_cull_mode_back,
};

// NOTE: not sure if this will be used.
enum gfx_face_winding {
	_gfx_face_winding_null,
	gfx_face_winding_ccw,
	gfx_face_winding_cw,
};

typedef u32 gfx_render_target_flags;
enum {
	_gfx_render_target_flag_null = 0,
	gfx_render_target_flag_fixed_size = (1 << 0),
	gfx_render_target_flag_no_depth = (1 << 1),
	gfx_render_target_flag_no_color = (1 << 2),
};

// structs

// buffer
struct gfx_buffer_desc_t {
	gfx_buffer_type type;
	u32 size;
	gfx_usage usage;
};
struct gfx_buffer_t; // defined in backends.

// texture
struct gfx_texture_desc_t {
	str_t name;
	uvec2_t size;
	gfx_texture_format format;
	gfx_texture_type type;
	u32 sample_count;
	gfx_usage usage;
	b8 render_target;
};
struct gfx_texture_t; // defined in backends.

// shader
struct gfx_shader_attribute_t {
	char* name;
	u32 slot;
	gfx_vertex_format format;
	gfx_vertex_class classification;
};
struct gfx_shader_desc_t {
	str_t name;
	str_t filepath;
	gfx_shader_attribute_t* attributes;
	u32 attribute_count;
};
struct gfx_shader_t; // defined in backends.

// render target
struct gfx_render_target_desc_t {
	uvec2_t size;
	u32 sample_count;
	gfx_render_target_flags flags;
	gfx_texture_format colorbuffer_format;
	gfx_texture_format depthbuffer_format;
};
struct gfx_render_target_t; // defined in backends.


// render graph
struct gfx_resource_t;
struct gfx_resource_list_t;
struct gfx_render_pass_t;
typedef void gfx_render_pass_func(gfx_render_target_t*, gfx_render_target_t*);

// renderer
struct gfx_renderer_t; // define in backends.

// pipeline
struct gfx_pipeline_t {
	gfx_fill_mode fill_mode;
	gfx_cull_mode cull_mode;
	gfx_topology_type topology;
	gfx_filter_mode filter_mode;
	gfx_wrap_mode wrap_mode;
	gfx_depth_mode depth_mode;
	rect_t viewport;
	rect_t scissor;
};

// state
struct gfx_state_t; // defined in backends.

// functions

// state
function void gfx_init();
function void gfx_release();
function void gfx_update();

function void gfx_draw(u32, u32 = 0);
function void gfx_draw_indexed(u32, u32 = 0, u32 = 0);
function void gfx_draw_instanced(u32, u32, u32 = 0, u32 = 0);

// pipeline
function gfx_pipeline_t gfx_pipeline_create();

function void gfx_set_sampler(gfx_filter_mode, gfx_wrap_mode, u32);
function void gfx_set_topology(gfx_topology_type);
function void gfx_set_rasterizer(gfx_fill_mode, gfx_cull_mode);
function void gfx_set_viewport(rect_t);
function void gfx_set_scissor(rect_t);
function void gfx_set_depth_mode(gfx_depth_mode);
function void gfx_set_pipeline(gfx_pipeline_t);
function void gfx_set_buffer(gfx_buffer_t*, u32 = 0, u32 = 0);
function void gfx_set_texture(gfx_texture_t*, u32 = 0);
function void gfx_set_shader(gfx_shader_t*);
function void gfx_set_render_target(gfx_render_target_t* = nullptr);


// renderer
function gfx_renderer_t* gfx_renderer_create(os_window_t*, color_t);
function void gfx_renderer_release(gfx_renderer_t*);
function void gfx_renderer_resize(gfx_renderer_t*, uvec2_t);
function gfx_render_pass_t* gfx_renderer_add_pass(gfx_renderer_t*, str_t, gfx_render_pass_func*, gfx_render_target_desc_t);
function void gfx_renderer_submit(gfx_renderer_t*);

// buffer
function gfx_buffer_t* gfx_buffer_create_ex(gfx_buffer_desc_t, void*);
function gfx_buffer_t* gfx_buffer_create(gfx_buffer_type, u32, void* = nullptr);
function void gfx_buffer_release(gfx_buffer_t*);

// texture
function gfx_texture_t* gfx_texture_create_ex(gfx_texture_desc_t, void* = nullptr);
function gfx_texture_t* gfx_texture_create(uvec2_t, gfx_texture_format = gfx_texture_format_rgba8, void* = nullptr);
function void gfx_texture_release(gfx_texture_t*);
function void gfx_texture_fill(gfx_texture_t*, void*);
function void gfx_texture_fill_region(gfx_texture_t*, rect_t, void*);
function void gfx_texture_blit(gfx_texture_t*, gfx_texture_t*);

// shaders
function gfx_shader_t* gfx_shader_create_ex(str_t, gfx_shader_desc_t);
function gfx_shader_t* gfx_shader_create(str_t, str_t, gfx_shader_attribute_t*, u32);
function gfx_shader_t* gfx_shader_load(str_t, gfx_shader_attribute_t*, u32);
function void gfx_shader_release(gfx_shader_t*);
function void gfx_shader_compile(gfx_shader_t*, str_t);

// render target
function gfx_render_target_t* gfx_render_target_create_ex(gfx_render_target_desc_t);
function gfx_render_target_t* gfx_render_target_create(uvec2_t, gfx_render_target_flags = 0, gfx_texture_format = gfx_texture_format_rgba8, gfx_texture_format = gfx_texture_format_d32);
function void gfx_render_target_resize(gfx_render_target_t*, uvec2_t);
function void _gfx_render_target_create_resources(gfx_render_target_t*);

// helper functions
function b8 _texture_format_is_depth(gfx_texture_format);

// per backend includes

#define GFX_BACKEND_D3D11

#ifdef GFX_BACKEND_D3D11
	#include "backends/gfx/gfx_d3d11.h"
#elif defined(GFX_BACKEND_OPENGL)
	#include "backends/gfx/gfx_opengl.h"
#elif defined(GFX_BACKEND_METAL)
	#include "backends/gfx/gfx_metal.h"
#endif 

#endif // GFX_H 