// gfx.h

#ifndef GFX_H
#define GFX_H

// includes
#include <d3d11_1.h>
#include <d3dcompiler.h>

// structs

struct gfx_renderer_t {
	gfx_renderer_t* next;
	gfx_renderer_t* prev;

	os_window_t* window;
	u32 width;
	u32 height;

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

};

struct gfx_state_t {

	// arenas
	arena_t* renderer_arena;
	arena_t* scratch_arena;

	// renderer list
	gfx_renderer_t* first_renderer;
	gfx_renderer_t* last_renderer;
	gfx_renderer_t* free_renderer;

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


#endif // GFX_H