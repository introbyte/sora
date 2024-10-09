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
	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader* pixel_shader;
	ID3D11InputLayout* input_layout;
};

// render target

struct gfx_render_target_t {
	gfx_render_target_t* next;
	gfx_render_target_t* prev;

	gfx_render_target_desc_t desc;
	gfx_texture_t* colorbuffer;
	gfx_texture_t* depthbuffer;
	ID3D11RenderTargetView* colorbuffer_rtv;
	ID3D11DepthStencilView* depthbuffer_dsv;
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
};

// global

global gfx_state_t gfx_state;






// helper functions
function b8 _texture_format_is_depth(gfx_texture_format);

// enum functions
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