// gfx_d3d11.h

#ifndef GFX_D3D11_H
#define GFX_D3D11_H

// includes
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <dwrite.h>

// defines

#ifdef BUILD_DEBUG
#define gfx_assert(hr, msg, ...) { if (FAILED(hr)) { printf("[error] "msg" (%x)\n", __VA_ARGS__, hr); os_abort(1); } }
#define gfx_check_error(hr, msg, ...) { if (FAILED(hr)) { printf("[error] "msg" (%x)\n", __VA_ARGS__, hr); } }
#else
#define gfx_assert(hr, msg, ...)
#define gfx_check_error(hr, msg, ...)
#endif

// structs

// buffer

struct gfx_buffer_t {
	gfx_buffer_t* next;
	gfx_buffer_t* prev;

	gfx_buffer_desc_t desc;
	ID3D11Buffer* id;
};

// texture

struct gfx_texture_t {
	gfx_texture_t* next;
	gfx_texture_t* prev;

	gfx_texture_desc_t desc;
	ID3D11Texture2D* id;
	ID3D11ShaderResourceView* srv;
};

// shader 

struct gfx_shader_t {
	gfx_shader_t* next;
	gfx_shader_t* prev;

	gfx_shader_desc_t desc;
	u32 last_modified;
	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader* pixel_shader;
	ID3D11InputLayout* input_layout;
};

// render target

struct gfx_render_target_t {
	gfx_render_target_t* next;
	gfx_render_target_t* prev;

	uvec2_t size;
	u32 sample_count;
	gfx_render_target_flags flags;
	gfx_texture_t* texture;
	gfx_texture_format format;
	union {
		ID3D11RenderTargetView* rtv;
		ID3D11DepthStencilView* dsv;
	};
};

// renderer

struct gfx_renderer_t {
	gfx_renderer_t* next;
	gfx_renderer_t* prev;

	// context
	os_window_t* window;
	color_t clear_color;
	uvec2_t resolution;

	// d3d11
	IDXGISwapChain1* swapchain;
	ID3D11Texture2D* framebuffer;
	ID3D11RenderTargetView* framebuffer_rtv;
};

// state

struct gfx_state_t {

	// arenas
	arena_t* resource_arena;
	arena_t* scratch_arena;

	// d3d11
	ID3D11Device* device;
	ID3D11DeviceContext* device_context;
	IDXGIDevice1* dxgi_device;
	IDXGIAdapter* dxgi_adapter;
	IDXGIFactory2* dxgi_factory;

	// resources
	gfx_buffer_t* buffer_first;
	gfx_buffer_t* buffer_last;
	gfx_buffer_t* buffer_free;

	gfx_texture_t* texture_first;
	gfx_texture_t* texture_last;
	gfx_texture_t* texture_free;

	gfx_shader_t* shader_first;
	gfx_shader_t* shader_last;
	gfx_shader_t* shader_free;

	gfx_render_target_t* render_target_first;
	gfx_render_target_t* render_target_last;
	gfx_render_target_t* render_target_free;

	// renderer
	gfx_renderer_t* renderer_first;
	gfx_renderer_t* renderer_last;
	gfx_renderer_t* renderer_free;
	gfx_renderer_t* renderer_active;

	// pipeline assets
	ID3D11SamplerState* linear_wrap_sampler;
	ID3D11SamplerState* linear_clamp_sampler;
	ID3D11SamplerState* nearest_wrap_sampler;
	ID3D11SamplerState* nearest_clamp_sampler;

	ID3D11DepthStencilState* depth_stencil_state;
	ID3D11DepthStencilState* no_depth_stencil_state;

	ID3D11RasterizerState* solid_cull_none_rasterizer;
	ID3D11RasterizerState* solid_cull_front_rasterizer;
	ID3D11RasterizerState* solid_cull_back_rasterizer;
	ID3D11RasterizerState* wireframe_cull_none_rasterizer;
	ID3D11RasterizerState* wireframe_cull_front_rasterizer;
	ID3D11RasterizerState* wireframe_cull_back_rasterizer;

	ID3D11BlendState* blend_state;

};

// global

global gfx_state_t gfx_state;


// d3d11 enum functions
function D3D11_USAGE _usage_to_d3d11_usage(gfx_usage);
function UINT _usage_to_access_flags(gfx_usage);
function D3D11_BIND_FLAG _buffer_type_to_bind_flag(gfx_buffer_type);
function DXGI_FORMAT _texture_format_to_dxgi_format(gfx_texture_format);
function DXGI_FORMAT _texture_format_to_srv_dxgi_format(gfx_texture_format);
function DXGI_FORMAT _texture_format_to_dsv_dxgi_format(gfx_texture_format);
function u32 _texture_format_to_bytes(gfx_texture_format);
function D3D11_PRIMITIVE_TOPOLOGY _topology_type_to_d3d11_topology(gfx_topology_type);
function DXGI_FORMAT _vertex_format_to_dxgi_format(gfx_vertex_format);
function DXGI_FORMAT _uniform_type_to_dxgi_format(gfx_uniform_type);
function D3D11_INPUT_CLASSIFICATION _vertex_class_to_input_class(gfx_vertex_class);


#endif // GFX_D3D11_H