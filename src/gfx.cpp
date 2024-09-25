// gfx.cpp

#ifndef GFX_CPP
#define GFX_CPP

// includes
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

// implementation

function void 
gfx_init() {

	// arenas
	gfx_state.renderer_arena = arena_create(megabytes(8));
	gfx_state.scratch_arena = arena_create(megabytes(8));

	u32 device_flags = 0;

	HRESULT hr = 0;

	// create device
	hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, device_flags, 0, 0, D3D11_SDK_VERSION, &gfx_state.device, 0, &gfx_state.device_context);
	hr = gfx_state.device->QueryInterface(__uuidof(IDXGIDevice1), (void**)(&gfx_state.dxgi_device));
	hr = gfx_state.dxgi_device->GetAdapter(&gfx_state.dxgi_adapter);
	hr = gfx_state.dxgi_adapter->GetParent(__uuidof(IDXGIFactory2), (void**)(&gfx_state.dxgi_factory));

	// create samplers

	D3D11_SAMPLER_DESC sampler_desc = { 0 };

	// point sampler
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MipLODBias = 0;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	hr = gfx_state.device->CreateSamplerState(&sampler_desc, &gfx_state.point_sampler);

	// linear sampler
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	gfx_state.device->CreateSamplerState(&sampler_desc, &gfx_state.linear_sampler);

	// create depth stencil states

	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = { 0 };

	// depth state
	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	depth_stencil_desc.StencilEnable = false;
	depth_stencil_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depth_stencil_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	gfx_state.device->CreateDepthStencilState(&depth_stencil_desc, &gfx_state.depth_stencil_state);

	// no depth state
	depth_stencil_desc.DepthEnable = false;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	depth_stencil_desc.StencilEnable = false;
	depth_stencil_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depth_stencil_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	gfx_state.device->CreateDepthStencilState(&depth_stencil_desc, &gfx_state.no_depth_stencil_state);

	// rasterizers 
	D3D11_RASTERIZER_DESC rasterizer_desc = { 0 };

	// solid rasterizer
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.FrontCounterClockwise = true;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0.0f;
	rasterizer_desc.SlopeScaledDepthBias = 0.0f;
	rasterizer_desc.DepthClipEnable = true;
	rasterizer_desc.ScissorEnable = true;
	rasterizer_desc.MultisampleEnable = true;
	rasterizer_desc.AntialiasedLineEnable = false;
	gfx_state.device->CreateRasterizerState(&rasterizer_desc, &gfx_state.solid_rasterizer_state);

	// wireframe rasterizer
	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	rasterizer_desc.FrontCounterClockwise = true;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0.0f;
	rasterizer_desc.SlopeScaledDepthBias = 0.0f;
	rasterizer_desc.DepthClipEnable = true;
	rasterizer_desc.ScissorEnable = true;
	rasterizer_desc.MultisampleEnable = true;
	rasterizer_desc.AntialiasedLineEnable = false;
	gfx_state.device->CreateRasterizerState(&rasterizer_desc, &gfx_state.wireframe_rasterizer_state);


	// create blend state
	D3D11_BLEND_DESC blend_state_desc = { 0 };
	blend_state_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend_state_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_state_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_state_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	gfx_state.device->CreateBlendState(&blend_state_desc, &gfx_state.blend_state);

	// create buffers

	D3D11_BUFFER_DESC buffer_desc = { 0 };

	// vertex buffer
	buffer_desc.ByteWidth = megabytes(1);
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	gfx_state.device->CreateBuffer(&buffer_desc, 0, &gfx_state.vertex_buffer);

	// constant buffer
	buffer_desc.ByteWidth = megabytes(1);
	buffer_desc.ByteWidth += 15;
	buffer_desc.ByteWidth -= buffer_desc.ByteWidth % 16;
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	gfx_state.device->CreateBuffer(&buffer_desc, 0, &gfx_state.constant_buffer);


}

function void 
gfx_release() {
	arena_release(gfx_state.renderer_arena);
	arena_release(gfx_state.scratch_arena);

	// release samplers
	if (gfx_state.point_sampler != nullptr) { gfx_state.point_sampler->Release(); }
	if (gfx_state.linear_sampler != nullptr) { gfx_state.linear_sampler->Release(); }

	// release depth stencil state
	if (gfx_state.depth_stencil_state != nullptr) { gfx_state.depth_stencil_state->Release(); }
	if (gfx_state.no_depth_stencil_state != nullptr) { gfx_state.no_depth_stencil_state->Release(); }

	// release rasterizers
	if (gfx_state.solid_rasterizer_state != nullptr) { gfx_state.solid_rasterizer_state->Release(); }
	if (gfx_state.wireframe_rasterizer_state != nullptr) { gfx_state.wireframe_rasterizer_state->Release(); }

	// release blend state
	if (gfx_state.blend_state != nullptr) { gfx_state.blend_state->Release(); }

	// release buffers
	if (gfx_state.vertex_buffer != nullptr) { gfx_state.vertex_buffer->Release(); }
	if (gfx_state.constant_buffer != nullptr) { gfx_state.constant_buffer->Release(); }

	// release devices
	if (gfx_state.dxgi_factory != nullptr) { gfx_state.dxgi_factory->Release(); }
	if (gfx_state.dxgi_adapter != nullptr) { gfx_state.dxgi_adapter->Release(); }
	if (gfx_state.dxgi_device != nullptr) { gfx_state.dxgi_device->Release(); }
	if (gfx_state.device_context != nullptr) { gfx_state.device_context->Release(); }
	if (gfx_state.device != nullptr) { gfx_state.device->Release(); }

}



function gfx_renderer_t* 
gfx_renderer_create(os_window_t* window, color_t clear_color, u8 sample_count) {

	gfx_renderer_t* renderer = nullptr;
	renderer = gfx_state.free_renderer;
	if (renderer != nullptr) {
		stack_pop(gfx_state.free_renderer);
	} else {
		renderer = (gfx_renderer_t*)arena_malloc(gfx_state.renderer_arena, sizeof(gfx_renderer_t));
	}
	memset(renderer, 0, sizeof(gfx_renderer_t));
	dll_push_back(gfx_state.first_renderer, gfx_state.last_renderer, renderer);

	renderer->window = window;
	renderer->clear_color = clear_color;
	renderer->width = window->width;
	renderer->height = window->height;

	HRESULT hr = 0;

	// create swapchain
	DXGI_SWAP_CHAIN_DESC1 swapchain_desc = { 0 };

	swapchain_desc.Width = window->width;
	swapchain_desc.Height = window->height;
	swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchain_desc.Stereo = FALSE;
	swapchain_desc.SampleDesc.Count = sample_count;
	swapchain_desc.SampleDesc.Quality = 0;
	swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchain_desc.BufferCount = 2;
	swapchain_desc.Scaling = DXGI_SCALING_STRETCH;
	swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapchain_desc.Flags = 0;
	hr = gfx_state.dxgi_factory->CreateSwapChainForHwnd(gfx_state.device, window->handle, &swapchain_desc, 0, 0, &renderer->swapchain);

	// create render target view
	renderer->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&renderer->framebuffer));
	gfx_state.device->CreateRenderTargetView(renderer->framebuffer, 0, &renderer->framebuffer_rtv);

	// create depth buffer
	renderer->depthbuffer_desc = { 0 };
	renderer->depthbuffer_desc.Width = (u32)window->width;
	renderer->depthbuffer_desc.Height = (u32)window->height;
	renderer->depthbuffer_desc.MipLevels = 1;
	renderer->depthbuffer_desc.ArraySize = 1;
	renderer->depthbuffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	renderer->depthbuffer_desc.SampleDesc.Count = sample_count;
	renderer->depthbuffer_desc.SampleDesc.Quality = 0;
	renderer->depthbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	renderer->depthbuffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	renderer->depthbuffer_desc.CPUAccessFlags = 0;
	renderer->depthbuffer_desc.MiscFlags = 0;
	gfx_state.device->CreateTexture2D(&renderer->depthbuffer_desc, nullptr, &renderer->depthbuffer);

	// craete depth stencil view
	renderer->dsv_desc = { 0 };
	renderer->dsv_desc.Format = renderer->depthbuffer_desc.Format;
	if (sample_count > 1) {
		renderer->dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	} else {
		renderer->dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	}
	gfx_state.device->CreateDepthStencilView(renderer->depthbuffer, &renderer->dsv_desc, &renderer->depthbuffer_dsv);

	// viewport
	renderer->viewport = { 0.0f, 0.0f, (f32)window->width, (f32)window->height, 0.0f, 1.0f };

	// scissor rect
	renderer->scissor_rect = { 0, 0, (i32)window->width, (i32)window->height };

	return renderer;

}

function void
gfx_renderer_release(gfx_renderer_t* renderer) {
	dll_remove(gfx_state.first_renderer, gfx_state.last_renderer, renderer);
	stack_push(gfx_state.free_renderer, renderer);

	if (renderer->depthbuffer != nullptr) { renderer->depthbuffer->Release(); }
	if (renderer->depthbuffer_dsv != nullptr) { renderer->depthbuffer_dsv->Release(); }
	if (renderer->framebuffer_rtv != nullptr) { renderer->framebuffer_rtv->Release(); }
	if (renderer->framebuffer != nullptr) { renderer->framebuffer->Release(); }
	if (renderer->swapchain != nullptr) { renderer->swapchain->Release(); }

}

function void 
gfx_renderer_resize(gfx_renderer_t* renderer) {
	if (renderer->width == 0 || renderer->height == 0) {
		return;
	}

	gfx_state.device_context->OMSetRenderTargets(0, 0, 0);

	// release buffers
	renderer->framebuffer_rtv->Release();
	renderer->framebuffer->Release();
	renderer->depthbuffer_dsv->Release();
	renderer->depthbuffer->Release();

	// resize framebuffer
	renderer->swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	renderer->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&renderer->framebuffer));
	gfx_state.device->CreateRenderTargetView(renderer->framebuffer, 0, &renderer->framebuffer_rtv);

	// resize depth buffer
	renderer->depthbuffer_desc.Width = renderer->window->width;
	renderer->depthbuffer_desc.Height = renderer->window->height;
	gfx_state.device->CreateTexture2D(&renderer->depthbuffer_desc, 0, &renderer->depthbuffer);
	gfx_state.device->CreateDepthStencilView(renderer->depthbuffer, &renderer->dsv_desc, &renderer->depthbuffer_dsv);

	// reset viewports
	renderer->viewport = { 0.0f, 0.0f, (f32)renderer->window->width, (f32)renderer->window->height, 0.0f, 1.0f };
	renderer->scissor_rect = { 0, 0, (i32)renderer->window->width, (i32)renderer->window->height };
}

function void
gfx_renderer_begin_frame(gfx_renderer_t* renderer) {
	if (renderer->width != renderer->window->width || renderer->height != renderer->window->height) {
		renderer->width = renderer->window->width;
		renderer->height = renderer->window->height;
		gfx_renderer_resize(renderer);
	}

	// clear
	FLOAT clear_color_array[] = {renderer->clear_color.r, renderer->clear_color.g, renderer->clear_color.b, renderer->clear_color.a };
	gfx_state.device_context->ClearRenderTargetView(renderer->framebuffer_rtv, clear_color_array);
	gfx_state.device_context->ClearDepthStencilView(renderer->depthbuffer_dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// set output merger state
	gfx_state.device_context->OMSetDepthStencilState(gfx_state.depth_stencil_state, 1);
	gfx_state.device_context->OMSetRenderTargets(1, &renderer->framebuffer_rtv, renderer->depthbuffer_dsv);
	gfx_state.device_context->OMSetBlendState(gfx_state.blend_state, nullptr, 0xffffffff);

	// set rasterizer state
	gfx_state.device_context->RSSetState(gfx_state.solid_rasterizer_state);
	gfx_state.device_context->RSSetViewports(1, &renderer->viewport);
	gfx_state.device_context->RSSetScissorRects(1, &renderer->scissor_rect);

	// set input assembler state
	gfx_state.device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	gfx_state.device_context->PSSetSamplers(0, 1, &gfx_state.point_sampler);
	gfx_state.device_context->PSSetSamplers(1, 1, &gfx_state.linear_sampler);

}

function void
gfx_renderer_end_frame(gfx_renderer_t* renderer) {

	// present
	renderer->swapchain->Present(1, 0);
	gfx_state.device_context->ClearState();
}



#endif // GFX_CPP