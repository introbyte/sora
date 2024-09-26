// gfx.h

#ifndef GFX_H
#define GFX_H

// includes
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <dwrite.h>

// enums

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

// structs

struct gfx_constant_data_t {
	vec2_t window_size;
};


struct gfx_texture_t {
	gfx_texture_t* next;
	gfx_texture_t* prev;
	
	ID3D11Texture2D* id;
	ID3D11ShaderResourceView* srv;
	str_t name;
	u32 width, height;
};

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



// instance types

struct gfx_quad_params_t {
	color_t col0;
	color_t col1;
	color_t col2;
	color_t col3;
	vec4_t radii;
	vec4_t style; // (thickness, softness, unused, unused)
};

struct gfx_quad_instance_t {
	rect_t pos;
	rect_t uv;
	color_t col0;
	color_t col1;
	color_t col2;
	color_t col3;
	vec4_t radii;
	vec4_t style; // (thickness, softness, unused, unused)
};

struct gfx_line_params_t {
	color_t col0;
	color_t col1;
	f32 thickness;
	f32 softness;
};

struct gfx_line_instance_t {
	rect_t pos;
	color_t col0;
	color_t col1;
	vec4_t points;
	vec4_t style; // (thickness, softness, unused, unused)
};


struct gfx_text_params_t {
	color_t color;
	gfx_font_t* font;
	f32 font_size;
};

struct gfx_text_instance_t {
	rect_t pos;
	rect_t uv;
	color_t col;
};


struct gfx_batch_state_t {
	gfx_shader_t* shader;
	gfx_texture_t* texture;
	rect_t clip_mask;
	u32 instance_size;
};

struct gfx_batch_t {
	gfx_batch_t* next;
	gfx_batch_t* prev;

	gfx_batch_state_t batch_state;
	void* batch_data;
	u32 instance_count;
};

struct gfx_renderer_t {
	gfx_renderer_t* next;
	gfx_renderer_t* prev;

	os_window_t* window;
	u32 width;
	u32 height;

	// arenas
	arena_t* batch_arena;

	color_t clear_color;

	IDXGISwapChain1* swapchain;

	ID3D11Texture2D* framebuffer;
	ID3D11RenderTargetView* framebuffer_rtv;

	D3D11_TEXTURE2D_DESC depthbuffer_desc;
	ID3D11Texture2D* depthbuffer;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	ID3D11DepthStencilView* depthbuffer_dsv;

	D3D11_VIEWPORT viewport;
	D3D11_RECT scissor_rect;

	// batches
	gfx_batch_t* batch_first;
	gfx_batch_t* batch_last;

	gfx_constant_data_t constant_data;

};

struct gfx_state_t {

	// arenas
	arena_t* renderer_arena;
	arena_t* resource_arena;
	arena_t* scratch_arena;

	// renderer list
	gfx_renderer_t* renderer_first;
	gfx_renderer_t* renderer_last;
	gfx_renderer_t* renderer_free;

	// resource list
	gfx_texture_t* texture_first;
	gfx_texture_t* texture_last;
	gfx_texture_t* texture_free;

	gfx_shader_t* shader_first;
	gfx_shader_t* shader_last;
	gfx_shader_t* shader_free;

	// d3d11
	ID3D11Device* device;
	ID3D11DeviceContext* device_context;

	IDXGIDevice1* dxgi_device;
	IDXGIAdapter* dxgi_adapter;
	IDXGIFactory2* dxgi_factory;

	ID3D11DepthStencilState* depth_stencil_state;
	ID3D11DepthStencilState* no_depth_stencil_state;

	ID3D11SamplerState* point_sampler;
	ID3D11SamplerState* linear_sampler;

	ID3D11RasterizerState* solid_rasterizer_state;
	ID3D11RasterizerState* wireframe_rasterizer_state;

	ID3D11BlendState* blend_state;

	ID3D11Buffer* vertex_buffer;
	ID3D11Buffer* constant_buffer;

	// dwrite
	IDWriteFactory* dwrite_factory;
	IDWriteRenderingParams* rendering_params;
	IDWriteGdiInterop* gdi_interop;

	// default assets
	gfx_texture_t* default_texture;

	// ui assets
	gfx_shader_t* quad_shader;
	gfx_shader_t* text_shader;
	gfx_shader_t* line_shader;
	gfx_shader_t* disk_shader;


};


// globals

global gfx_state_t gfx_state;

// functions

function void gfx_init();
function void gfx_release();


function gfx_renderer_t* gfx_renderer_create(os_window_t*, color_t, u8);
function void gfx_renderer_release(gfx_renderer_t*);
function void gfx_renderer_resize(gfx_renderer_t*);
function void gfx_renderer_begin_frame(gfx_renderer_t*);
function void gfx_renderer_end_frame(gfx_renderer_t*);


function gfx_batch_t* gfx_batch_find(gfx_renderer_t*, gfx_batch_state_t, u32);

function void gfx_renderer_push_quad(gfx_renderer_t*, rect_t, gfx_quad_params_t);
function void gfx_renderer_push_line(gfx_renderer_t*, vec2_t, vec2_t, gfx_line_params_t);


function gfx_texture_t* gfx_texture_create(str_t, u32, u32, void*);
function gfx_texture_t* gfx_texture_load(str_t);
function void gfx_texture_release(gfx_texture_t*);
function void gfx_texture_load_buffer(gfx_texture_t*, void*);
function void gfx_texture_fill(gfx_texture_t*, rect_t, void*);


function gfx_shader_t* gfx_shader_create(str_t, str_t, gfx_shader_layout_t);
function gfx_shader_t* gfx_shader_load(str_t, gfx_shader_layout_t);
function void gfx_shader_release(gfx_shader_t*);
function b8 gfx_shader_build(gfx_shader_t*, str_t);


function DXGI_FORMAT d3d11_vertex_format_type_to_dxgi_format(gfx_vertex_format);
function D3D11_INPUT_CLASSIFICATION d3d11_vertex_class_to_input_class(gfx_vertex_class);

// font

function gfx_font_t* gfx_font_open(str_t, arena_t*);
function void gfx_font_release(gfx_font_t*);

function gfx_font_metrics_t gfx_font_get_metrics(gfx_font_t*, f32);

function u32 gfx_font_glyph_hash(u32, f32);
function gfx_font_glyph_t* gfx_font_get_glyph(gfx_font_t*, u32, f32);
function gfx_font_raster_t gfx_font_glyph_raster(arena_t*, gfx_font_t*, u32, f32);
function vec2_t gfx_font_atlas_add(gfx_font_t*, vec2_t);

#endif // GFX_H