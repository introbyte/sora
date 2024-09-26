// gfx.cpp

#ifndef GFX_CPP
#define GFX_CPP

// includes
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dwrite")

// defines

#define gfx_buffer_size megabytes(8)
#define gfx_batch_size megabytes(8)

// implementation

function void 
gfx_init() {

	// arenas
	gfx_state.renderer_arena = arena_create(megabytes(64));
	gfx_state.resource_arena = arena_create(gigabytes(2));
	gfx_state.scratch_arena = arena_create(megabytes(64));

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
	buffer_desc.ByteWidth = gfx_buffer_size;
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


	// create dwrite factory
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&gfx_state.dwrite_factory);

	// create rendering params
	hr = gfx_state.dwrite_factory->CreateRenderingParams(&gfx_state.rendering_params);
	f32 gamma = gfx_state.rendering_params->GetGamma();
	f32 enhanced_contrast = gfx_state.rendering_params->GetEnhancedContrast();
	f32 clear_type_level = gfx_state.rendering_params->GetClearTypeLevel();
	hr = gfx_state.dwrite_factory->CreateCustomRenderingParams(gamma, enhanced_contrast, clear_type_level, DWRITE_PIXEL_GEOMETRY_FLAT, DWRITE_RENDERING_MODE_DEFAULT, &gfx_state.rendering_params);

	gfx_state.dwrite_factory->GetGdiInterop(&gfx_state.gdi_interop);


	// default texture
	u32 texture_data = 0xFFFFFFFF;
	gfx_state.default_texture = gfx_texture_create(str("default"), 1, 1, &texture_data);


	gfx_state.quad_shader = gfx_shader_load(str("res/shaders/ui_quad.hlsl"), { {
		{ "POS", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "UV", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "COL", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "COL", 1, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "COL", 2, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "COL", 3, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "RAD", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "STY", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
	} });

	gfx_state.text_shader = gfx_shader_load(str("res/shaders/ui_text.hlsl"), { {
		{"POS", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{"UV", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{"COL", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance }
	} });

	gfx_state.line_shader = gfx_shader_load(str("res/shaders/ui_line.hlsl"), { {
		{"POS", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{"COL", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{"COL", 1, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{"PNT", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{"STY", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
	} });


	gfx_state.disk_shader = gfx_shader_load(str("res/shaders/ui_disk.hlsl"), { {
		{"POS", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{"COL", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{"COL", 1, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{"COL", 2, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{"COL", 3, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{"ANG", 0, gfx_vertex_format_float2, gfx_vertex_class_per_instance },
		{"STY", 0, gfx_vertex_format_float2, gfx_vertex_class_per_instance },
	} });

}

function void 
gfx_release() {

	// release arenas
	arena_release(gfx_state.renderer_arena);
	arena_release(gfx_state.resource_arena);
	arena_release(gfx_state.scratch_arena);

	// release default assets
	gfx_texture_release(gfx_state.default_texture);

	gfx_shader_release(gfx_state.quad_shader);
	gfx_shader_release(gfx_state.text_shader);
	gfx_shader_release(gfx_state.line_shader);
	gfx_shader_release(gfx_state.disk_shader);

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


// renderer functions

function gfx_renderer_t* 
gfx_renderer_create(os_window_t* window, color_t clear_color, u8 sample_count) {

	gfx_renderer_t* renderer = nullptr;
	renderer = gfx_state.renderer_free;
	if (renderer != nullptr) {
		stack_pop(gfx_state.renderer_free);
	} else {
		renderer = (gfx_renderer_t*)arena_malloc(gfx_state.renderer_arena, sizeof(gfx_renderer_t));
	}
	memset(renderer, 0, sizeof(gfx_renderer_t));
	dll_push_back(gfx_state.renderer_first, gfx_state.renderer_last, renderer);

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

	// arena
	renderer->batch_arena = arena_create(gigabytes(2));

	return renderer;

}

function void
gfx_renderer_release(gfx_renderer_t* renderer) {
	dll_remove(gfx_state.renderer_first, gfx_state.renderer_last, renderer);
	stack_push(gfx_state.renderer_free, renderer);

	if (renderer->depthbuffer != nullptr) { renderer->depthbuffer->Release(); }
	if (renderer->depthbuffer_dsv != nullptr) { renderer->depthbuffer_dsv->Release(); }
	if (renderer->framebuffer_rtv != nullptr) { renderer->framebuffer_rtv->Release(); }
	if (renderer->framebuffer != nullptr) { renderer->framebuffer->Release(); }
	if (renderer->swapchain != nullptr) { renderer->swapchain->Release(); }
	arena_release(renderer->batch_arena);

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

	// resize if needed
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
	gfx_state.device_context->OMSetDepthStencilState(gfx_state.no_depth_stencil_state, 1);
	gfx_state.device_context->OMSetRenderTargets(1, &renderer->framebuffer_rtv, renderer->depthbuffer_dsv);
	gfx_state.device_context->OMSetBlendState(gfx_state.blend_state, nullptr, 0xffffffff);

	// set rasterizer state
	gfx_state.device_context->RSSetState(gfx_state.solid_rasterizer_state);
	gfx_state.device_context->RSSetViewports(1, &renderer->viewport);
	gfx_state.device_context->RSSetScissorRects(1, &renderer->scissor_rect);

	// set input assembler state
	//gfx_state.device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set samplers
	gfx_state.device_context->PSSetSamplers(0, 1, &gfx_state.point_sampler);
	gfx_state.device_context->PSSetSamplers(1, 1, &gfx_state.linear_sampler);
	
	renderer->constant_data.window_size = vec2((f32)renderer->width, (f32)renderer->height);

}

function void
gfx_renderer_end_frame(gfx_renderer_t* renderer) {
	
	// load constant buffer
	D3D11_MAPPED_SUBRESOURCE mapped_subresource = { 0 };
	gfx_state.device_context->Map(gfx_state.constant_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	u8* ptr = (u8*)mapped_subresource.pData;
	memcpy(ptr, &renderer->constant_data, sizeof(renderer->constant_data));
	gfx_state.device_context->Unmap(gfx_state.constant_buffer, 0);

	gfx_state.device_context->VSSetConstantBuffers(0, 1, &gfx_state.constant_buffer);
	gfx_state.device_context->PSSetConstantBuffers(0, 1, &gfx_state.constant_buffer);
	gfx_state.device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// render batches
	for (gfx_batch_t* batch = renderer->batch_first; batch != 0; batch = batch->next) {

		u32 stride = batch->batch_state.instance_size;
		u32 offset = 0;

		// load vertex data into buffer
		D3D11_MAPPED_SUBRESOURCE mapped_subresource = { 0 };
		gfx_state.device_context->Map(gfx_state.vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
		u8* ptr = (u8*)mapped_subresource.pData;
		memcpy(ptr, batch->batch_data, batch->instance_count * batch->batch_state.instance_size);
		gfx_state.device_context->Unmap(gfx_state.vertex_buffer, 0);
		
		gfx_state.device_context->IASetInputLayout(batch->batch_state.shader->input_layout);
		gfx_state.device_context->IASetVertexBuffers(0, 1, &gfx_state.vertex_buffer, &stride, &offset);

		gfx_state.device_context->VSSetShader(batch->batch_state.shader->vertex_shader, 0 ,0);
		gfx_state.device_context->PSSetShader(batch->batch_state.shader->pixel_shader, 0, 0);

		gfx_state.device_context->PSSetShaderResources(0, 1, &batch->batch_state.texture->srv);

		gfx_state.device_context->DrawInstanced(4, batch->instance_count, 0, 0);

	}

	// clear batch list
	arena_clear(renderer->batch_arena);
	renderer->batch_first = renderer->batch_last = nullptr;
	
	// present
	renderer->swapchain->Present(1, 0);
	gfx_state.device_context->ClearState();
}



function gfx_batch_t*
gfx_batch_find(gfx_renderer_t* renderer, gfx_batch_state_t state, u32 count) {

	// search through current batches
	for (gfx_batch_t* batch = renderer->batch_first; batch != 0; batch = batch->next) {

		b8 batches_equal = memcmp((void*)&state, (void*)&batch->batch_state, sizeof(gfx_batch_state_t)) == 0;
		b8 batch_has_size = (batch->batch_state.instance_size * (batch->instance_count + count)) < gfx_batch_size;
		
		// if state matches, and we have room..
		if (batches_equal && batch_has_size) {
			return batch;
		}
	}

	// ..else, create a new batch
	
	gfx_batch_t* batch = (gfx_batch_t*)arena_calloc(renderer->batch_arena, sizeof(gfx_batch_t));
	
	batch->batch_state = state;
	batch->batch_data = arena_malloc(renderer->batch_arena, gfx_batch_size);
	batch->instance_count = 0;

	dll_push_back(renderer->batch_first, renderer->batch_last, batch);

	return batch;
}

function void 
gfx_renderer_push_quad(gfx_renderer_t* renderer, rect_t pos, gfx_quad_params_t params) {

	gfx_batch_state_t state;
	state.shader = gfx_state.quad_shader;
	state.instance_size = sizeof(gfx_quad_instance_t);
	state.texture = gfx_state.default_texture;
	state.clip_mask = rect(0.0f, 0.0f, (f32)renderer->width, (f32)renderer->height);
	gfx_batch_t* batch = gfx_batch_find(renderer, state, 1);
	gfx_quad_instance_t* instance = &((gfx_quad_instance_t*)batch->batch_data)[batch->instance_count++];

	rect_validate(pos);

	instance->pos = pos;
	instance->uv = rect(0.0f, 0.0f, 1.0f, 1.0f);

	instance->col0 = params.col0;
	instance->col1 = params.col1;
	instance->col2 = params.col2;
	instance->col3 = params.col3;

	instance->radii = params.radii;
	instance->style = {params.thickness, params.softness, 0.0f, 0.0f};

}

function void
gfx_renderer_push_line(gfx_renderer_t* renderer, vec2_t p0, vec2_t p1, gfx_line_params_t params) {

	gfx_batch_state_t state;
	state.shader = gfx_state.line_shader;
	state.instance_size = sizeof(gfx_line_instance_t);
	state.texture = gfx_state.default_texture;
	state.clip_mask = rect(0.0f, 0.0f, (f32)renderer->width, (f32)renderer->height);
	gfx_batch_t* batch = gfx_batch_find(renderer, state, 1);
	gfx_line_instance_t* instance = &((gfx_line_instance_t*)batch->batch_data)[batch->instance_count++];

	// calculate bounding box
	rect_t bbox = { p0.x, p0.y, p1.x, p1.y };
	rect_validate(bbox);
	bbox = rect_grow(bbox, params.thickness + params.softness);

	vec2_t c = rect_center(bbox);
	vec2_t c_p0 = vec2_sub(c, p0);
	vec2_t c_p1 = vec2_sub(c, p1);
	
	instance->pos = bbox;
	instance->col0 = params.col0;
	instance->col1 = params.col1;
	instance->points = { c_p0.x, c_p0.y, c_p1.x, c_p1.y };
	instance->style = { params.thickness, params.softness, 0.0f, 0.0f };

}

function void 
gfx_renderer_push_text(gfx_renderer_t* renderer, str_t string, vec2_t pos, gfx_text_params_t params) {
	
	gfx_batch_state_t state;
	state.shader = gfx_state.text_shader;
	state.instance_size = sizeof(gfx_text_instance_t);
	state.texture = params.font->atlas_texture;
	state.clip_mask = rect(0.0f, 0.0f, (f32)renderer->width, (f32)renderer->height);
	gfx_batch_t* batch = gfx_batch_find(renderer, state, string.size);
	
	for (u32 i = 0; i < string.size; i++) {
		gfx_text_instance_t* instance = &((gfx_text_instance_t*)batch->batch_data)[batch->instance_count++];

		u8 codepoint = *(string.data + i);
		gfx_font_glyph_t* glyph = gfx_font_get_glyph(params.font, codepoint, params.font_size);

		instance->pos = { pos.x, pos.y, pos.x + glyph->pos.x1, pos.y + glyph->pos.y1 };
		instance->uv = glyph->uv;
		instance->col = params.color;
		pos.x += glyph->advance;

	}




}

function void
gfx_renderer_push_text(gfx_renderer_t* renderer, str16_t string, vec2_t pos, gfx_text_params_t params) {

	gfx_batch_state_t state;
	state.shader = gfx_state.text_shader;
	state.instance_size = sizeof(gfx_text_instance_t);
	state.texture = params.font->atlas_texture;
	state.clip_mask = rect(0.0f, 0.0f, (f32)renderer->width, (f32)renderer->height);
	gfx_batch_t* batch = gfx_batch_find(renderer, state, string.size);

	for (u32 i = 0; i < string.size; i++) {
		gfx_text_instance_t* instance = &((gfx_text_instance_t*)batch->batch_data)[batch->instance_count++];

		u16 codepoint = *(string.data + i);
		gfx_font_glyph_t* glyph = gfx_font_get_glyph(params.font, codepoint, params.font_size);

		instance->pos = { pos.x, pos.y, pos.x + glyph->pos.x1, pos.y + glyph->pos.y1 };
		instance->uv = glyph->uv;
		instance->col = params.color;
		pos.x += glyph->advance;

	}

}

function void
gfx_renderer_push_disk(gfx_renderer_t* renderer, vec2_t pos, f32 radius, f32 start_angle, f32 end_angle, gfx_disk_params_t params) {

	gfx_batch_state_t state;
	state.shader = gfx_state.disk_shader;
	state.instance_size = sizeof(gfx_disk_instance_t);
	state.texture = gfx_state.default_texture;
	state.clip_mask = rect(0.0f, 0.0f, (f32)renderer->width, (f32)renderer->height);
	gfx_batch_t* batch = gfx_batch_find(renderer, state, 1);
	gfx_disk_instance_t* instance = &((gfx_disk_instance_t*)batch->batch_data)[batch->instance_count++];

	instance->pos = { pos.x - radius, pos.y - radius, pos.x + radius, pos.y + radius };
	instance->col0 = params.col0;
	instance->col1 = params.col1;
	instance->col2 = params.col2;
	instance->col3 = params.col3;
	instance->angles = { radians(start_angle), radians(end_angle) };
	instance->style = { params.thickness, params.softness };

}

// params

function gfx_quad_params_t 
gfx_quad_params(color_t color, f32 radius = 5.0f, f32 thickness = 0.0f, f32 softness = 0.33f) {
	gfx_quad_params_t params = { 0 };

	params.col0 = color;
	params.col1 = color;
	params.col2 = color;
	params.col3 = color;
	params.radii = { radius, radius, radius, radius };
	params.thickness = thickness;
	params.softness = softness;

	return params;
}

function gfx_line_params_t
gfx_line_params(color_t color, f32 thickness = 1.0f, f32 softness = 0.33f) {
	gfx_line_params_t params;
	params.col0 = params.col1 = color;
	params.thickness = thickness;
	params.softness = softness;
	return params;
}

function gfx_text_params_t 
gfx_text_params(color_t color, gfx_font_t* font, f32 size) {
	gfx_text_params_t params;
	params.color = color;
	params.font = font;
	params.font_size = size;
	return params;
}

function gfx_disk_params_t
gfx_disk_params(color_t color, f32 thickness = 0.0f, f32 softness = 0.33f) {
	gfx_disk_params_t params;
	params.col0 = params.col1 = params.col2 = params.col2 = color;
	params.thickness = thickness;
	params.softness = softness;
	return params;
}


// texture functions

function gfx_texture_t* 
gfx_texture_create(str_t name, u32 width, u32 height, void* data) {

	gfx_texture_t* texture = nullptr;
	texture = gfx_state.texture_free;
	if (texture != nullptr) {
		stack_pop(gfx_state.texture_free);
	} else {
		texture = (gfx_texture_t*)arena_malloc(gfx_state.resource_arena, sizeof(gfx_texture_t));
	}
	memset(texture, 0, sizeof(gfx_texture_t));
	dll_push_back(gfx_state.texture_first, gfx_state.texture_last, texture);
	
	texture->name = name;
	texture->width = width;
	texture->height = height;
	
	gfx_texture_load_buffer(texture, data);

	return texture;
}

function gfx_texture_t*
gfx_texture_load(str_t filepath) {

	// TODO: load png files

	gfx_texture_t* texture = nullptr;
	texture = gfx_state.texture_free;
	if (texture != nullptr) {
		stack_pop(gfx_state.texture_free);
	} else {
		texture = (gfx_texture_t*)arena_malloc(gfx_state.resource_arena, sizeof(gfx_texture_t));
	}
	memset(texture, 0, sizeof(gfx_texture_t));
	dll_push_back(gfx_state.texture_first, gfx_state.texture_last, texture);

	str_t file_name = str_get_file_name(filepath);

	texture->name = file_name;


	return texture;

}

function void
gfx_texture_release(gfx_texture_t* texture) {
	dll_remove(gfx_state.texture_first, gfx_state.texture_last, texture);
	stack_push(gfx_state.texture_free, texture);

	if (texture->id != nullptr) { texture->id->Release(); }
	if (texture->srv != nullptr) { texture->srv->Release(); }
}

function void
gfx_texture_load_buffer(gfx_texture_t* texture, void* data) {

	ID3D11Texture2D* id = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;

	HRESULT hr = 0;

	// set descriptors
	D3D11_TEXTURE2D_DESC texture_desc = { 0 };
	texture_desc.Width = texture->width;
	texture_desc.Height = texture->height;
	texture_desc.ArraySize = 1;
	texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MipLevels = 1;
	texture_desc.MiscFlags = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = { 0 };
	srv_desc.Format = texture_desc.Format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MostDetailedMip = 0;
	srv_desc.Texture2D.MipLevels = 1;

	D3D11_SUBRESOURCE_DATA texture_data = { 0 };
	texture_data.pSysMem = data;
	texture_data.SysMemPitch = texture->width * 4;

	// create texture
	hr = gfx_state.device->CreateTexture2D(&texture_desc, data ? &texture_data : nullptr, &id);

	if (hr != S_OK) {
		printf("[error] failed to create texture '%.*s' (%x)\n", texture->name.size, texture->name.data, hr);
	}

	// create srv
	hr = gfx_state.device->CreateShaderResourceView(id, &srv_desc, &srv);

	if (hr != S_OK) {
		printf("[error] failed to create srv '%.*s' (%x)\n", texture->name.size, texture->name.data, hr);
	}

	if (hr == S_OK) {

		// release old if needed
		if (texture->id != nullptr) { texture->id->Release(); }
		if (texture->srv != nullptr) { texture->srv->Release(); }

		// set new
		texture->id = id;
		texture->srv = srv;

		printf("[info] successfully created texture: '%.*s'\n", texture->name.size, texture->name.data);

	}

}

function void 
gfx_texture_fill(gfx_texture_t* texture, rect_t rect, void* data) {

	D3D11_BOX dst_box = {
	  (UINT)rect.x0, (UINT)rect.y0, 0,
	  (UINT)rect.x1, (UINT)rect.y1, 1,
	};

	if (dst_box.right > texture->width || dst_box.bottom > texture->height) {
		printf("[error] incorrect rect size.\n");
	} else {
		gfx_state.device_context->UpdateSubresource(texture->id, 0, &dst_box, data, (rect.x1 - rect.x0) * 4, 0);
	}
}

// shader functions

function gfx_shader_t*
gfx_shader_create(str_t name, str_t src, gfx_shader_layout_t layout) {

	gfx_shader_t* shader = nullptr;
	shader = gfx_state.shader_free;
	if (shader != nullptr) {
		stack_pop(gfx_state.shader_free);
	} else {
		shader = (gfx_shader_t*)arena_malloc(gfx_state.resource_arena, sizeof(gfx_shader_t));
	}
	memset(shader, 0, sizeof(gfx_shader_t));
	dll_push_back(gfx_state.shader_first, gfx_state.shader_last, shader);

	shader->name = name;
	shader->layout = layout;
	
	gfx_shader_build(shader, src);

	return shader;

}

function gfx_shader_t*
gfx_shader_load(str_t filepath, gfx_shader_layout_t layout) {

	str_t src = os_file_read_all(gfx_state.scratch_arena, filepath);
	str_t file_name = str_get_file_name(filepath);

	gfx_shader_t* shader = gfx_shader_create(file_name, src, layout);

	arena_clear(gfx_state.scratch_arena);

	return shader;
}

function void
gfx_shader_release(gfx_shader_t* shader) {
	dll_remove(gfx_state.shader_first, gfx_state.shader_last, shader);
	stack_push(gfx_state.shader_free, shader);
	
	if (shader->vertex_shader != nullptr) { shader->vertex_shader->Release(); }
	if (shader->pixel_shader != nullptr) { shader->pixel_shader->Release(); }
	if (shader->input_layout != nullptr) { shader->input_layout->Release(); }
	shader->compiled = false;
}

function b8
gfx_shader_build(gfx_shader_t* shader, str_t src) {

	HRESULT hr;
	b8 result = false;
	ID3DBlob* vs_blob = nullptr;
	ID3DBlob* ps_blob = nullptr;
	ID3D11VertexShader* vertex_shader = nullptr;
	ID3D11PixelShader* pixel_shader = nullptr;
	ID3DBlob* vs_error_blob = nullptr;
	ID3DBlob* ps_error_blob = nullptr;
	ID3D11InputLayout* input_layout = nullptr;

	u32 compile_flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;

	// compile vertex shader
	hr = D3DCompile(src.data, src.size, (char*)shader->name.data, 0, 0, "vs_main", "vs_5_0", compile_flags, 0, &vs_blob, &vs_error_blob);

	if (vs_error_blob) {
		cstr error_msg = (cstr)vs_error_blob->GetBufferPointer();
		printf("[error] failed to compile vertex shader:\n\n%s\n", error_msg);
		goto shader_build_cleanup;
	}

	hr = gfx_state.device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, &vertex_shader);

	// compile pixel shader
	hr = D3DCompile(src.data, src.size, (char*)shader->name.data, 0, 0, "ps_main", "ps_5_0", compile_flags, 0, &ps_blob, &ps_error_blob);

	if (ps_error_blob) {
		cstr error_msg = (cstr)ps_error_blob->GetBufferPointer();
		printf("[error] failed to compile pixel shader:\n\n%s\n", error_msg);
		goto shader_build_cleanup;
	}

	hr = gfx_state.device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, &pixel_shader);


	// create input element description
	gfx_shader_attribute_t* attributes = shader->layout.attributes;

	D3D11_INPUT_ELEMENT_DESC input_element_desc[8] = { 0 };
	u32 attribute_count = 0;

	for (i32 i = 0; i < 8; i++) {
		if (!attributes[i].name) { break; }
		input_element_desc[i] = {
			attributes[i].name,
			attributes[i].slot,
			d3d11_vertex_format_type_to_dxgi_format(attributes[i].vertex_format),
			0, D3D11_APPEND_ALIGNED_ELEMENT,
			d3d11_vertex_class_to_input_class(attributes[i].classification),
			(attributes[i].classification == gfx_vertex_class_per_vertex) ? (u32)0 : (u32)1
		};
		attribute_count++;
	}

	// create input layout
	hr = gfx_state.device->CreateInputLayout(input_element_desc, attribute_count, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &input_layout);
	if (FAILED(hr)) {
		printf("[error] failed to create shader input layout. (%x)\n", hr);
		goto shader_build_cleanup;
	}


	if (hr == S_OK) {

		// release old shaders if needed
		if (shader->vertex_shader != nullptr) { shader->vertex_shader->Release(); }
		if (shader->pixel_shader != nullptr) { shader->pixel_shader->Release(); }
		if (shader->input_layout != nullptr) { shader->input_layout->Release(); }

		// set new shaders
		shader->vertex_shader = vertex_shader;
		shader->pixel_shader = pixel_shader;
		shader->input_layout = input_layout;

		printf("[info] successfully created shader: '%.*s'\n", shader->name.size, shader->name.data);
	}
	
	// build success
	result = true;


shader_build_cleanup:
	arena_clear(gfx_state.scratch_arena);
	if (vs_blob != nullptr) { vs_blob->Release(); }
	if (ps_blob != nullptr) { ps_blob->Release(); }
	if (vs_error_blob != nullptr) { vs_error_blob->Release(); }
	if (ps_error_blob != nullptr) { ps_error_blob->Release(); }
	shader->compiled = result;
	return result;
}

function DXGI_FORMAT
d3d11_vertex_format_type_to_dxgi_format(gfx_vertex_format type) {

	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

	switch (type) {
		case gfx_vertex_format_float: { format = DXGI_FORMAT_R32_FLOAT; break; }
		case gfx_vertex_format_float2: { format = DXGI_FORMAT_R32G32_FLOAT; break; }
		case gfx_vertex_format_float3: { format = DXGI_FORMAT_R32G32B32_FLOAT; break; }
		case gfx_vertex_format_float4: { format = DXGI_FORMAT_R32G32B32A32_FLOAT; break; }
		case gfx_vertex_format_int: { format = DXGI_FORMAT_R32_SINT; break; }
		case gfx_vertex_format_int2: { format = DXGI_FORMAT_R32G32_SINT; break; }
		case gfx_vertex_format_int3: { format = DXGI_FORMAT_R32G32B32_SINT; break; }
		case gfx_vertex_format_int4: { format = DXGI_FORMAT_R32G32B32A32_SINT; break; }
		case gfx_vertex_format_uint: { format = DXGI_FORMAT_R32_UINT; break; }
		case gfx_vertex_format_uint2: { format = DXGI_FORMAT_R32G32_UINT; break; }
		case gfx_vertex_format_uint3: { format = DXGI_FORMAT_R32G32B32_UINT; break; }
		case gfx_vertex_format_uint4: { format = DXGI_FORMAT_R32G32B32A32_UINT; break; }
	}

	return format;

}

function D3D11_INPUT_CLASSIFICATION
d3d11_vertex_class_to_input_class(gfx_vertex_class classification) {
	D3D11_INPUT_CLASSIFICATION shader_classification;

	switch (classification) {
		case gfx_vertex_class_per_vertex: { shader_classification = D3D11_INPUT_PER_VERTEX_DATA; break; }
		case gfx_vertex_class_per_instance: { shader_classification = D3D11_INPUT_PER_INSTANCE_DATA; break; }
	}

	return shader_classification;
}



// font functions

function gfx_font_t*
gfx_font_open(str_t filepath) {

	gfx_font_t* font = (gfx_font_t*)arena_malloc(gfx_state.resource_arena, sizeof(gfx_font_t));

	// convert to wide path
	str16_t wide_filepath = str_to_str16(gfx_state.scratch_arena, filepath);

	// create font file and face
	gfx_state.dwrite_factory->CreateFontFileReference((WCHAR*)wide_filepath.data, 0, &(font->file));
	gfx_state.dwrite_factory->CreateFontFace(DWRITE_FONT_FACE_TYPE_TRUETYPE, 1, &font->file, 0, DWRITE_FONT_SIMULATIONS_NONE, &(font->face));

	// glyph cache
	font->glyph_first = nullptr;
	font->glyph_last = nullptr;

	// atlas nodes
	font->root_size = vec2(1024.0f, 1024.0f);
	font->root = (gfx_font_atlas_node_t*)arena_malloc(gfx_state.resource_arena, sizeof(gfx_font_atlas_node_t));
	font->root->max_free_size[0] =
		font->root->max_free_size[1] =
		font->root->max_free_size[2] =
		font->root->max_free_size[3] = vec2_mul(font->root_size, 0.5f);

	// atlas texture
	str_t font_name = str_get_file_name(filepath);
	font->atlas_texture = gfx_texture_create(font_name, 1024, 1024, nullptr);

	arena_clear(gfx_state.scratch_arena);

	return font;
}

function void
gfx_font_release(gfx_font_t* font) {
	if (font->face != nullptr) { font->face->Release(); }
	if (font->file != nullptr) { font->file->Release(); }
	gfx_texture_release(font->atlas_texture);

}

function u32 
gfx_font_glyph_hash(u32 codepoint, f32 size) {
	u32 float_bits = *(u32*)&size;
	u32 hash = float_bits ^ codepoint;

	hash ^= (hash >> 16);
	hash *= 0x45d9f3b;
	hash ^= (hash >> 16);
	hash *= 0x45d9f3b;
	hash ^= (hash >> 16);

	return hash;
}

function gfx_font_glyph_t*
gfx_font_get_glyph(gfx_font_t* font, u32 codepoint, f32 size) {

	gfx_font_glyph_t* glyph = nullptr;
	u32 hash = gfx_font_glyph_hash(codepoint, size);

	// try to find glyph in cache
	for (gfx_font_glyph_t* current = font->glyph_first; current != 0; current = current->next) {

		// we found a match
		if (current->hash == hash) {
			glyph = current;
			break;
		}
	}

	// if we did not find a match, add to cache
	if (glyph == nullptr) {

		// raster the glyph on scratch arena
		gfx_font_raster_t raster = gfx_font_glyph_raster(gfx_state.scratch_arena, font, codepoint, size);
		vec2_t raster_size = vec2((f32)raster.width, (f32)raster.height);

		// add to atlas
		vec2_t atlas_glyph_pos = gfx_font_atlas_add(font, raster_size);
		vec2_t atlas_glyph_size = vec2_add(atlas_glyph_pos, raster_size);
		rect_t region = { atlas_glyph_pos.x, atlas_glyph_pos.y, atlas_glyph_size.x, atlas_glyph_size.y };
		gfx_texture_fill(font->atlas_texture, region, raster.data);

		// add glyph to cache list
		glyph = (gfx_font_glyph_t*)arena_malloc(gfx_state.resource_arena, sizeof(gfx_font_glyph_t));
		glyph->hash = hash;
		glyph->advance = raster.advance;
		glyph->height = raster.height;
		glyph->pos = rect(0.0f, 0.0f, raster_size.x, raster_size.y);
		glyph->uv = rect(region.x0 / 1024.0f, region.y0 / 1024.0f, region.x1 / 1024.0f, region.y1/ 1024.0f);
		dll_push_back(font->glyph_first, font->glyph_last, glyph);

		arena_clear(gfx_state.scratch_arena);
	}

	return glyph;
}

function gfx_font_raster_t
gfx_font_glyph_raster(arena_t* arena, gfx_font_t* font, u32 codepoint, f32 size) {

	// get font metrics
	DWRITE_FONT_METRICS font_metrics = { 0 };
	font->face->GetMetrics(&font_metrics);
	f32 design_units_per_em = font_metrics.designUnitsPerEm;
	f32 ascent = (96.0f / 72.0f) * size * (f32)font_metrics.ascent / design_units_per_em;
	f32 descent = (96.0f / 72.0f) * size * (f32)font_metrics.descent / design_units_per_em;
	f32 pixel_per_design_unit = ((96.0f / 72.0f) * size) / (f32)font_metrics.designUnitsPerEm;

	// get glyph indices
	u16 glyph_index;
	font->face->GetGlyphIndicesA(&codepoint, 1, &glyph_index);

	// get metrics info
	DWRITE_GLYPH_METRICS glyph_metrics = { 0 };
	font->face->GetGdiCompatibleGlyphMetrics(size, 1.0f, 0, 1, &glyph_index, 1, &glyph_metrics, 0);

	// determine atlas size
	i32 atlas_dim_x = (i32)(96.0f / 72.0f) * size * glyph_metrics.advanceWidth / design_units_per_em;
	i32 atlas_dim_y = (i32)((96.0f / 72.0f) * size * (font_metrics.ascent + font_metrics.descent) / design_units_per_em) + 2.0f;
	f32 advance = (f32)glyph_metrics.advanceWidth * pixel_per_design_unit + 1.0f;
	atlas_dim_x += 7;
	atlas_dim_x -= atlas_dim_x % 8;
	atlas_dim_x += 4;

	// make bitmap for rendering
	IDWriteBitmapRenderTarget* render_target;
	gfx_state.gdi_interop->CreateBitmapRenderTarget(0, atlas_dim_x, atlas_dim_y, &render_target);
	HDC dc = render_target->GetMemoryDC();

	// draw glyph
	DWRITE_GLYPH_RUN glyph_run = { 0 };
	glyph_run.fontFace = font->face;
	glyph_run.fontEmSize = size * 96.0f / 72.0f;
	glyph_run.glyphCount = 1;
	glyph_run.glyphIndices = &glyph_index;

	RECT bounding_box = { 0 };
	vec2_t draw_pos = { 1.0f, (f32)atlas_dim_y - 2.0f - descent };
	render_target->DrawGlyphRun(draw_pos.x, draw_pos.y, DWRITE_MEASURING_MODE_NATURAL, &glyph_run, gfx_state.rendering_params, RGB(255, 255, 255), &bounding_box);

	// get bitmap
	DIBSECTION dib = { 0 };
	HBITMAP bitmap = (HBITMAP)GetCurrentObject(dc, OBJ_BITMAP);
	GetObject(bitmap, sizeof(dib), &dib);

	gfx_font_raster_t raster = { 0 };

	raster.width = atlas_dim_x;
	raster.height = atlas_dim_y;
	raster.advance = floorf(advance);

	raster.data = (u8*)arena_malloc(arena, sizeof(u8) * atlas_dim_x * atlas_dim_y * 4);

	u8* in_data = (u8*)dib.dsBm.bmBits;
	u32 in_pitch = (u32)dib.dsBm.bmWidthBytes;
	u8* out_data = raster.data;
	u32 out_pitch = atlas_dim_x * 4;

	u8* in_line = (u8*)in_data;
	u8* out_line = out_data;
	for (u64 y = 0; y < atlas_dim_y; y += 1) {
		u8* in_pixel = in_line;
		u8* out_pixel = out_line;
		for (u32 x = 0; x < atlas_dim_x; x += 1) {
			out_pixel[0] = 255;
			out_pixel[1] = 255;
			out_pixel[2] = 255;
			out_pixel[3] = in_pixel[1];
			in_pixel += 4;
			out_pixel += 4;
		}
		in_line += in_pitch;
		out_line += out_pitch;
	}

	render_target->Release();

	return raster;
}

function vec2_t
gfx_font_atlas_add(gfx_font_t* font, vec2_t needed_size) {

	// find node with best-fit size
	vec2_t region_p0 = { 0.0f, 0.0f };
	vec2_t region_size = { 0.0f, 0.0f };

	gfx_font_atlas_node_t* node = 0;
	i32 node_corner = -1;

	vec2_t n_supported_size = font->root_size;

	const vec2_t corner_vertices[4] = {
		vec2(0.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
	};

	for (gfx_font_atlas_node_t* n = font->root, *next = 0; n != 0; n = next, next = 0) {

		if (n->taken) {
			break;
		}

		b8 n_can_be_allocated = (n->child_count == 0);

		if (n_can_be_allocated) {
			region_size = n_supported_size;
		}

		vec2_t child_size = vec2_mul(n_supported_size, 0.5f);


		gfx_font_atlas_node_t* best_child = 0;

		if (child_size.x >= needed_size.x && child_size.y >= needed_size.y) {

			for (i32 i = 0; i < 4; i++) {

				if (n->children[i] == 0) {

					n->children[i] = (gfx_font_atlas_node_t*)arena_malloc(gfx_state.resource_arena, sizeof(gfx_font_atlas_node_t));
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

		for (gfx_font_atlas_node_t* p = node->parent; p != 0; p = p->parent) {
			p->child_count += 1;
			gfx_font_atlas_node_t* parent = p->parent;
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

function gfx_font_metrics_t
gfx_font_get_metrics(gfx_font_t* font, f32 size) {

	DWRITE_FONT_METRICS metrics = { 0 };
	font->face->GetMetrics(&metrics);
	f32 design_units_per_em = (f32)metrics.designUnitsPerEm;

	gfx_font_metrics_t result = { 0 };
	result.line_gap = (96.0f / 72.0f) * size * (f32)metrics.lineGap / design_units_per_em;
	result.ascent = (96.0f / 72.0f) * size * (f32)metrics.ascent / design_units_per_em;
	result.descent = (96.0f / 72.0f) * size * (f32)metrics.descent / design_units_per_em;
	result.capital_height = (96.0f / 72.0f) * size * (f32)metrics.capHeight / design_units_per_em;

	return result;
}


#endif // GFX_CPP