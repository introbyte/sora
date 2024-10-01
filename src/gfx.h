// gfx.h

#ifndef GFX_H
#define GFX_H

// includes
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <dwrite.h>


// defines

#define gfx_buffer_size kilobytes(64)

#define gfx_stack_node_decl(name, type) struct gfx_##name##_node_t { gfx_##name##_node_t* next; type v; };
#define gfx_stack_decl(name) struct { gfx_##name##_node_t* top; gfx_##name##_node_t* free; b8 auto_pop; } name##_stack;
#define gfx_stack_decl_default(name) gfx_##name##_node_t name##_default_node;
#define gfx_stack_top_func(name, type) function type gfx_top_##name();
#define gfx_stack_push_func(name, type) function type gfx_push_##name(type);
#define gfx_stack_pop_func(name, type) function type gfx_pop_##name();
#define gfx_stack_set_next_func(name, type) function type gfx_set_next_##name(type);
#define gfx_stack_func(name, type)\
gfx_stack_top_func(name, type)\
gfx_stack_push_func(name, type)\
gfx_stack_pop_func(name, type)\
gfx_stack_set_next_func(name, type)\


// enums

enum gfx_usage_type {
	gfx_usage_type_static,
	gfx_usage_type_dynamic,
	gfx_usage_type_stream,
};

enum gfx_sample_type {
	gfx_sample_type_nearest,
	gfx_sample_type_linear,
};

enum gfx_topology_type {
	gfx_topology_type_lines,
	gfx_topology_type_line_strip,
	gfx_topology_type_tris,
	gfx_topology_type_tri_strip,
};

enum gfx_texture_format {
	gfx_texture_format_r8,
	gfx_texture_format_rg8,
	gfx_texture_format_rgba8,
	gfx_texture_format_bgra8,
	gfx_texture_format_r16,
	gfx_texture_format_rgba16,
	gfx_texture_format_r32,
	gfx_texture_format_rg32,
	gfx_texture_format_rgba32,
};

enum gfx_vertex_format {
	gfx_vertex_format_float,
	gfx_vertex_format_float2,
	gfx_vertex_format_float3,
	gfx_vertex_format_float4,
	gfx_vertex_format_int,
	gfx_vertex_format_int2,
	gfx_vertex_format_int3,
	gfx_vertex_format_int4,
	gfx_vertex_format_uint,
	gfx_vertex_format_uint2,
	gfx_vertex_format_uint3,
	gfx_vertex_format_uint4,
};

enum gfx_vertex_class {
	gfx_vertex_class_per_vertex,
	gfx_vertex_class_per_instance,
};

enum gfx_batch_type {
	gfx_batch_type_quad,
	gfx_batch_type_circle,
	gfx_batch_type_ring,
	gfx_batch_type_line,
	gfx_batch_type_tri,
};

// structs

// buffers
struct gfx_buffer_t {
	gfx_buffer_t* next;
	ID3D11Buffer* id;
	gfx_usage_type usage;
	u32 size;
};

// textures
struct gfx_texture_t {
	gfx_texture_t* next;
	
	str_t name;
	ID3D11Texture2D* id;
	ID3D11ShaderResourceView* srv;
	gfx_usage_type usage;
	u32 width, height;
	gfx_texture_format format;
};

// shaders
struct gfx_shader_attribute_t {
	cstr name;
	u32 slot;
	gfx_vertex_format vertex_format;
	gfx_vertex_class classification;
};

struct gfx_shader_layout_t {
	gfx_shader_attribute_t attributes[8];
};

struct gfx_shader_t {
	gfx_shader_t* next;

	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader* pixel_shader;
	ID3D11InputLayout* input_layout;

	str_t name;
	gfx_shader_layout_t layout;
	b8 compiled;

};

// fonts
struct gfx_font_metrics_t {
	f32 line_gap;
	f32 ascent;
	f32 descent;
	f32 capital_height;
	f32 x_height;
};

struct gfx_font_glyph_t {
	gfx_font_glyph_t* next;
	gfx_font_glyph_t* prev;

	u32 hash;

	rect_t pos;
	rect_t uv;
	f32 advance;
	f32 height;
};

struct gfx_font_atlas_node_t {
	gfx_font_atlas_node_t* parent;
	gfx_font_atlas_node_t* children[4];
	u32 child_count;

	vec2_t max_free_size[4];
	b8 taken;
};

struct gfx_font_raster_t {
	u32 width;
	u32 height;
	f32 advance;
	u8* data;
};

struct gfx_font_t {

	IDWriteFontFile* file;
	IDWriteFontFace* face;

	gfx_font_glyph_t* glyph_first;
	gfx_font_glyph_t* glyph_last;

	vec2_t root_size;
	gfx_font_atlas_node_t* root;
	gfx_texture_t* atlas_texture;

};

// params types
struct gfx_quad_params_t {
	color_t col0;
	color_t col1;
	color_t col2;
	color_t col3;
	vec4_t radii;
	f32 thickness;
	f32 softness;
};

struct gfx_line_params_t {
	color_t col0;
	color_t col1;
	f32 thickness;
	f32 softness;
};

struct gfx_text_params_t {
	color_t color;
	gfx_font_t* font;
	f32 font_size;
};

struct gfx_disk_params_t {
	color_t col0;
	color_t col1;
	color_t col2;
	color_t col3;
	f32 thickness;
	f32 softness;
};

struct gfx_tri_params_t {
	color_t col0;
	color_t col1;
	color_t col2;
	f32 thickness;
	f32 softness;
};


// instance types
struct gfx_quad_instance_t {
	rect_t pos;
	rect_t uv;
	vec4_t col0;
	vec4_t col1;
	vec4_t col2;
	vec4_t col3;
	vec4_t radii;
	vec4_t style; // (thickness, softness, unused, unused)
};

struct gfx_line_instance_t {
	rect_t pos;
	vec4_t col0;
	vec4_t col1;
	vec4_t points;
	vec4_t style; // (thickness, softness, unused, unused)
};

struct gfx_disk_instance_t {
	rect_t pos;
	vec4_t col0;
	vec4_t col1;
	vec4_t col2;
	vec4_t col3;
	vec2_t angles; // (start_angle, end_angle)
	vec2_t style; // (thickness, softness)
};

struct gfx_tri_instance_t {
	rect_t pos;
	vec4_t col0;
	vec4_t col1;
	vec4_t col2;
	vec2_t p0;
	vec2_t p1;
	vec2_t p2;
	vec2_t style; // (thickness, softness)
};

// batches
struct gfx_batch_state_t {
	gfx_texture_t* texture;
	gfx_shader_t* shader;
	rect_t clip_mask;
	i32 depth;
	u32 instance_size;
};

struct gfx_batch_t {
	gfx_batch_t* next;
	gfx_batch_t* prev;
	gfx_batch_state_t state;
	void* data;
	u32 instance_count;
};

struct gfx_batch_list_t {
	gfx_batch_t* first;
	gfx_batch_t* last;
	u32 count;
};

struct gfx_batch_array_t {
	gfx_batch_t* batches;
	u32 count;
};

// stacks

gfx_stack_node_decl(texture, gfx_texture_t*)
gfx_stack_node_decl(shader, gfx_shader_t*)
gfx_stack_node_decl(clip, rect_t)
gfx_stack_node_decl(depth, i32)
gfx_stack_node_decl(instance_size, u32)


struct gfx_constant_data_t {
	vec2_t window_size;
};

struct gfx_renderer_t {
	gfx_renderer_t* next;

	os_window_t* window;
	u32 width;
	u32 height;

	color_t clear_color;

	// d3d11
	IDXGISwapChain1* swapchain;
	ID3D11Texture2D* framebuffer;
	ID3D11RenderTargetView* framebuffer_rtv;
	ID3D11Texture2D* depthbuffer;
	ID3D11DepthStencilView* depthbuffer_dsv;


};

struct gfx_state_t {

	// arenas
	arena_t* resource_arena;
	arena_t* scratch_arena;
	arena_t* per_frame_arena;

	// renderer list
	gfx_renderer_t* renderer_free;

	// resource list
	gfx_texture_t* texture_free;
	gfx_shader_t* shader_free;
	gfx_buffer_t* buffer_free;

	// d3d11
	ID3D11Device* device;
	ID3D11DeviceContext* device_context;
	IDXGIDevice1* dxgi_device;
	IDXGIAdapter* dxgi_adapter;
	IDXGIFactory2* dxgi_factory;
	ID3D11RasterizerState* solid_rasterizer_state;
	ID3D11RasterizerState* wireframe_rasterizer_state;
	ID3D11BlendState* blend_state;
	ID3D11BlendState* no_blend_state;
	ID3D11SamplerState* point_sampler;
	ID3D11SamplerState* linear_sampler;
	ID3D11DepthStencilState* depth_stencil_state;
	ID3D11DepthStencilState* no_depth_stencil_state;

	// dwrite
	IDWriteFactory* dwrite_factory;
	IDWriteRenderingParams* rendering_params;
	IDWriteGdiInterop* gdi_interop;

	gfx_renderer_t* active_renderer;

	// batch list
	gfx_batch_list_t batch_list;
	u32 batch_count;

	// stacks
	gfx_stack_decl(texture);
	gfx_stack_decl(shader);
	gfx_stack_decl(clip);
	gfx_stack_decl(depth);
	gfx_stack_decl(instance_size);

	// stack defaults
	gfx_stack_decl_default(texture)
	gfx_stack_decl_default(shader)
	gfx_stack_decl_default(clip)
	gfx_stack_decl_default(depth)
	gfx_stack_decl_default(instance_size)

	// buffers
	ID3D11Buffer* vertex_buffer;
	ID3D11Buffer* constant_buffer;

	// constant data
	gfx_constant_data_t constant_data;

	// default assets
	gfx_texture_t* default_texture;

	// assets
	gfx_shader_t* quad_shader;
	gfx_shader_t* line_shader;
	gfx_shader_t* disk_shader;
	gfx_shader_t* tri_shader;

};

// globals

global gfx_state_t gfx_state;

// functions

function void gfx_init();
function void gfx_release();

// renderer
function gfx_renderer_t* gfx_renderer_create(os_window_t*, color_t, u8 = 1);
function void gfx_renderer_release(gfx_renderer_t*);
function void gfx_renderer_resize(gfx_renderer_t*);
function void gfx_renderer_begin_frame(gfx_renderer_t*);
function void gfx_renderer_end_frame(gfx_renderer_t*);

// stacks
function void gfx_auto_pop_stacks();
gfx_stack_func(texture, gfx_texture_t*)
gfx_stack_func(shader, gfx_shader_t*)
gfx_stack_func(clip, rect_t)
gfx_stack_func(depth, i32)
gfx_stack_func(instance_size, u32)

function void gfx_draw_quad(rect_t, gfx_quad_params_t);
function void gfx_draw_line(vec2_t, vec2_t, gfx_line_params_t);
function void gfx_draw_text(str_t, vec2_t, gfx_text_params_t);
function void gfx_draw_text(str16_t, vec2_t, gfx_text_params_t);
function void gfx_draw_disk(vec2_t, f32, f32, f32, gfx_disk_params_t);
function void gfx_draw_tri(vec2_t, vec2_t, vec2_t, gfx_tri_params_t);


// batch
function i32 gfx_batch_compare_depth(const void*, const void*);
function b8 gfx_batch_state_equal(gfx_batch_state_t*, gfx_batch_state_t*);
function gfx_batch_t* gfx_batch_find(gfx_renderer_t*, gfx_batch_state_t, u32);
function gfx_batch_array_t gfx_batch_list_to_batch_array(gfx_batch_list_t);

// instance params
function gfx_quad_params_t gfx_quad_params(color_t, f32, f32, f32);
function gfx_line_params_t gfx_line_params(color_t, f32, f32);
function gfx_text_params_t gfx_text_params(color_t, gfx_font_t*, f32);
function gfx_disk_params_t gfx_disk_params(color_t, f32, f32);
function gfx_tri_params_t gfx_tri_params(color_t, f32, f32);

// buffer
function gfx_buffer_t* gfx_buffer_create(gfx_usage_type, u32, void*);
function void gfx_buffer_release(gfx_buffer_t*);

// texture
function gfx_texture_t* gfx_texture_create(str_t, u32, u32, gfx_texture_format = gfx_texture_format_rgba8, void* = nullptr);
function gfx_texture_t* gfx_texture_load(str_t);
function void gfx_texture_release(gfx_texture_t*);
function void gfx_texture_load_buffer(gfx_texture_t*, void*);
function void gfx_texture_fill(gfx_texture_t*, rect_t, void*);

// shader
function gfx_shader_t* gfx_shader_create(str_t, str_t, gfx_shader_layout_t);
function gfx_shader_t* gfx_shader_load(str_t, gfx_shader_layout_t);
function void gfx_shader_release(gfx_shader_t*);
function b8 gfx_shader_build(gfx_shader_t*, str_t);

// fonts
function gfx_font_t* gfx_font_load(str_t, arena_t*);
function void gfx_font_release(gfx_font_t*);
function gfx_font_metrics_t gfx_font_get_metrics(gfx_font_t*, f32);
function u32 gfx_font_glyph_hash(u32, f32);
function gfx_font_glyph_t* gfx_font_get_glyph(gfx_font_t*, u32, f32);
function gfx_font_raster_t gfx_font_glyph_raster(arena_t*, gfx_font_t*, u32, f32);
function vec2_t gfx_font_atlas_add(gfx_font_t*, vec2_t);
function f32 gfx_font_text_width(gfx_font_t*, f32, str_t);
function f32 gfx_font_text_height(gfx_font_t*, f32);

// enum helpers

function void d3d11_usage_type_to_d3d11_usage(gfx_usage_type, D3D11_USAGE*, UINT*);
function D3D11_PRIMITIVE_TOPOLOGY d3d11_topology_type_to_d3d11_topology(gfx_topology_type);
function DXGI_FORMAT d3d11_vertex_format_type_to_dxgi_format(gfx_vertex_format);
function DXGI_FORMAT d3d11_texture_format_to_dxgi_format(gfx_texture_format);
function u32 d3d11_texture_format_to_bytes(gfx_texture_format);
function D3D11_INPUT_CLASSIFICATION d3d11_vertex_class_to_input_class(gfx_vertex_class);


#endif // GFX_H