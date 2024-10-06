// gfx.h

#ifndef GFX_H
#define GFX_H

// includes
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <dwrite.h>

// todo:
//
// [~] - add basics back in.
//    [x] - textures.
//    [x] - shaders.
//    [x] - buffers.
//    [ ] - batches (improve).
//    [ ] - fonts (but redo).
// [ ] - add new stuff.
//    [ ] - meshes.
//    [ ] - render targets.
//    [ ] - compute shaders.
//    [ ] - asset hotloading.
// [ ] - render graph.
//

// defines

#define gfx_max_buffer_size kilobytes(64)

#define gfx_resource_list(type, name)\
type name##_first;\
type name##_last;\
type name##_free;\
u32 name##_count;

#define gfx_stack_node_decl(name, type) struct gfx_##name##_node_t { gfx_##name##_node_t* next; type v; };
#define gfx_stack_decl(name) struct { gfx_##name##_node_t* top; gfx_##name##_node_t* free; b8 auto_pop; } name##_stack;
#define gfx_stack_decl_default(name) gfx_##name##_node_t name##_default_node;
#define gfx_stack_top_func(name, type) function type gfx_top_##name();
#define gfx_stack_push_func(name, type) function type gfx_push_##name(type);
#define gfx_stack_pop_func(name, type) function type gfx_pop_##name();
#define gfx_stack_set_next_func(name, type) function type gfx_set_next_##name(type);
#define gfx_stack_func_decl(name, type)\
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

enum gfx_batch_type {
	gfx_batch_type_2d,
	gfx_batch_type_3d,
};

// structs

// resources

struct gfx_buffer_t {
	gfx_buffer_t* next;
	gfx_buffer_t* prev;
	ID3D11Buffer* id;
	gfx_usage_type usage;
	u32 size;
};

struct gfx_texture_t {
	gfx_texture_t* next;
	gfx_texture_t* prev;

	str_t name;
	ID3D11Texture2D* id;
	ID3D11ShaderResourceView* srv;
	gfx_usage_type usage;
	uvec2_t size;
	gfx_texture_format format;
};

struct gfx_shader_attribute_t {
	cstr name;
	u32 slot;
	gfx_vertex_format vertex_format;
	gfx_vertex_class classification;
};

struct gfx_shader_layout_t {
	gfx_shader_attribute_t attributes[16];
};

struct gfx_shader_t {
	gfx_shader_t* next;
	gfx_shader_t* prev;

	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader* pixel_shader;
	ID3D11InputLayout* input_layout;

	str_t name;
	gfx_shader_layout_t layout;
	b8 compiled;

};

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

struct gfx_vertex_t {
	vec3_t pos;
	vec3_t normal;
	vec3_t tangent;
	vec3_t bitangent;
	vec2_t uv;
	vec4_t color;
};

struct gfx_mesh_t {
	u32 vertex_count;
	u32 vertex_size;
	void* vertices;
};

// instances

struct gfx_2d_instance_t {
	rect_t bounding_box;
	rect_t uv;

	i32 type; // 0 - quad, 1 - line, 2 - radial, 3 - tri

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
};

// batches

struct gfx_2d_constants_t {
	vec2_t window_size;
	vec2_t time; // elasped_time, delta_time
};

struct gfx_3d_constants_t {
	mat4_t view_projection;
	mat4_t view;
	mat4_t projection;
	mat4_t inv_view;
	mat4_t inv_projection;
	vec2_t window_size;
	vec2_t time; // elasped_time, delta_time
};

struct gfx_batch_state_t {
	gfx_shader_t* shader;
	gfx_texture_t* texture;
	gfx_batch_type type;
	rect_t clip_mask;
	gfx_vertex_class vertex_class;
	union {
		u32 vertex_size;
		u32 instance_size;
	};

};

struct gfx_batch_t {
	gfx_batch_t* next;
	gfx_batch_t* prev;

	gfx_batch_state_t state;
	union {
		u32 vertex_count;
		u32 instance_count;
	};
	void* data;
};

struct gfx_batch_list_t {
	gfx_batch_t* first;
	gfx_batch_t* last;
	u32 count;
};

// stacks

gfx_stack_node_decl(texture, gfx_texture_t*)
gfx_stack_node_decl(shader, gfx_shader_t*)
gfx_stack_node_decl(clip, rect_t)
gfx_stack_node_decl(radius0, f32);
gfx_stack_node_decl(radius1, f32);
gfx_stack_node_decl(radius2, f32);
gfx_stack_node_decl(radius3, f32);
gfx_stack_node_decl(color0, color_t);
gfx_stack_node_decl(color1, color_t);
gfx_stack_node_decl(color2, color_t);
gfx_stack_node_decl(color3, color_t);
gfx_stack_node_decl(thickness, f32);
gfx_stack_node_decl(softness, f32);

// renderer

struct gfx_renderer_params_t {
	color_t clear_color;
	u32 sample_count;
};

struct gfx_renderer_t {
	gfx_renderer_t* next;
	gfx_renderer_t* prev;

	os_window_t* window;
	gfx_renderer_params_t params;
	uvec2_t resolution;

	// d3d11
	IDXGISwapChain1* swapchain;

	ID3D11Texture2D* framebuffer;
	ID3D11RenderTargetView* framebuffer_rtv;
	ID3D11ShaderResourceView* framebuffer_srv;

	ID3D11Texture2D* depthbuffer;
	ID3D11DepthStencilView* depthbuffer_dsv;
	ID3D11ShaderResourceView* depthbuffer_srv;
	
};

// state
struct gfx_state_t {

	// arenas
	arena_t* renderer_arena;
	arena_t* resource_arena;
	arena_t* batch_arena;
	arena_t* scratch_arena;

	// renderer list
	gfx_resource_list(gfx_renderer_t*, renderer);
	gfx_renderer_t* renderer_active;

	// resource list
	gfx_resource_list(gfx_texture_t*, texture);
	gfx_resource_list(gfx_shader_t*, shader);
	gfx_resource_list(gfx_buffer_t*, buffer);

	// batch list
	gfx_batch_list_t batch_list;

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

	// stacks
	gfx_stack_decl(texture);
	gfx_stack_decl(shader);
	gfx_stack_decl(clip);

	// 2d stack
	gfx_stack_decl(radius0);
	gfx_stack_decl(radius1);
	gfx_stack_decl(radius2);
	gfx_stack_decl(radius3);
	gfx_stack_decl(color0);
	gfx_stack_decl(color1);
	gfx_stack_decl(color2);
	gfx_stack_decl(color3);
	gfx_stack_decl(thickness);
	gfx_stack_decl(softness);

	// stack defaults
	gfx_stack_decl_default(texture);
	gfx_stack_decl_default(shader);
	gfx_stack_decl_default(clip);
	gfx_stack_decl_default(radius0);
	gfx_stack_decl_default(radius1);
	gfx_stack_decl_default(radius2);
	gfx_stack_decl_default(radius3);
	gfx_stack_decl_default(color0);
	gfx_stack_decl_default(color1);
	gfx_stack_decl_default(color2);
	gfx_stack_decl_default(color3);
	gfx_stack_decl_default(thickness);
	gfx_stack_decl_default(softness);

	// buffers
	ID3D11Buffer* vertex_buffer;
	ID3D11Buffer* constant_buffer;

	// constants
	gfx_2d_constants_t constants_2d;
	gfx_3d_constants_t constants_3d;

	// assets
	gfx_texture_t* default_texture;
	gfx_shader_t* shader_2d;
	
};

// global

global gfx_state_t gfx_state;

// functions

// states
function void gfx_init();
function void gfx_release();
function void gfx_update();
function void gfx_submit();

// renderer
function gfx_renderer_t* gfx_renderer_create(os_window_t*, gfx_renderer_params_t);
function void gfx_renderer_release(gfx_renderer_t*);
function void gfx_renderer_resize(gfx_renderer_t*, uvec2_t);
function void gfx_renderer_begin_frame(gfx_renderer_t*);
function void gfx_renderer_end_frame(gfx_renderer_t*);

// draw 

// 2d
function void gfx_draw_quad(rect_t);
function void gfx_draw_line(vec2_t, vec2_t);
function void gfx_draw_text(str_t, vec2_t, gfx_font_t*, f32);
function void gfx_draw_text(str16_t, vec2_t);
function void gfx_draw_radial(vec2_t, f32, f32, f32);
function void gfx_draw_tri(vec2_t, vec2_t, vec2_t);
function void gfx_draw_bezier(vec2_t, vec2_t, vec2_t, vec2_t);

// 3d
function void gfx_draw_mesh(gfx_mesh_t, gfx_shader_t*, mat4_t);


// batch
function b8 gfx_batch_state_equal(gfx_batch_state_t*, gfx_batch_state_t*);
function gfx_batch_t* gfx_batch_find(gfx_batch_state_t, u32);

// buffers
function gfx_buffer_t* gfx_buffer_create(gfx_usage_type, u32, void*);
function void gfx_buffer_release(gfx_buffer_t*);

// texture
function gfx_texture_t* gfx_texture_create(str_t, uvec2_t, gfx_texture_format = gfx_texture_format_rgba8, void* = nullptr);
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

// mesh
function gfx_mesh_t* gfx_mesh_create(str_t, u32, u32);
function gfx_mesh_t* gfx_mesh_load(str_t);
function void gfx_mesh_release(gfx_mesh_t*);


// stacks
function void gfx_auto_pop_stacks();
gfx_stack_func_decl(texture, gfx_texture_t*);
gfx_stack_func_decl(shader, gfx_shader_t*);
gfx_stack_func_decl(clip, rect_t);
gfx_stack_func_decl(radius0, f32);
gfx_stack_func_decl(radius1, f32);
gfx_stack_func_decl(radius2, f32);
gfx_stack_func_decl(radius3, f32);
gfx_stack_func_decl(color0, color_t);
gfx_stack_func_decl(color1, color_t);
gfx_stack_func_decl(color2, color_t);
gfx_stack_func_decl(color3, color_t);
gfx_stack_func_decl(thickness, f32);
gfx_stack_func_decl(softness, f32);
function void gfx_push_color(color_t);
function void gfx_set_next_color(color_t);
function void gfx_pop_color();

function void gfx_push_radius(f32);
function void gfx_set_next_radius(f32);
function void gfx_push_radii(vec4_t);
function void gfx_set_next_radii(vec4_t);


// enum helpers
function void d3d11_usage_type_to_d3d11_usage(gfx_usage_type, D3D11_USAGE*, UINT*);
function D3D11_PRIMITIVE_TOPOLOGY d3d11_topology_type_to_d3d11_topology(gfx_topology_type);
function DXGI_FORMAT d3d11_vertex_format_type_to_dxgi_format(gfx_vertex_format);
function DXGI_FORMAT d3d11_texture_format_to_dxgi_format(gfx_texture_format);
function u32 d3d11_texture_format_to_bytes(gfx_texture_format);
function D3D11_INPUT_CLASSIFICATION d3d11_vertex_class_to_input_class(gfx_vertex_class);

// error helpers

#ifdef BUILD_DEBUG
#define gfx_assert(hr, msg, ...) { if (FAILED(hr)) { printf("[error] "msg" (%x)\n", hr, __VA_ARGS__); os_abort(1); } }
#define gfx_check_error(hr, msg, ...) { if (FAILED(hr)) { printf("[error] "msg" (%x)\n", hr, __VA_ARGS__); } }
#else
#define gfx_assert(hr, msg, ...)
#define gfx_check_error(hr, msg, ...)
#endif


#endif // GFX_H