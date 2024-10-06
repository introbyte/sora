// gfx.cpp

#ifndef GFX_CPP
#define GFX_CPP

// include libs
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dwrite")

// defines

#define gfx_default_init(name, value) \
gfx_state.name##_default_node.v = value; \

#define gfx_stack_reset(name) \
gfx_state.name##_stack.top = &gfx_state.name##_default_node; gfx_state.name##_stack.free = 0; gfx_state.name##_stack.auto_pop = 0; \

// implementation

// state 

function void 
gfx_init() {

	// create arenas
	gfx_state.renderer_arena = arena_create(megabytes(64));
	gfx_state.resource_arena = arena_create(gigabytes(2));
	gfx_state.batch_arena = arena_create(gigabytes(2));
	gfx_state.scratch_arena = arena_create(gigabytes(2));

	// init list
	gfx_state.renderer_first = gfx_state.renderer_last = gfx_state.renderer_free = gfx_state.renderer_active = nullptr;
	gfx_state.texture_first = gfx_state.texture_last = gfx_state.texture_free = nullptr;
	gfx_state.shader_first = gfx_state.shader_last = gfx_state.shader_free = nullptr;
	gfx_state.buffer_first = gfx_state.buffer_last = gfx_state.buffer_free = nullptr;
	gfx_state.renderer_count = gfx_state.texture_count = gfx_state.shader_count = gfx_state.buffer_count = 0;

	// create device
	HRESULT hr = 0;
	u32 device_flags = 0;
	D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0 };

#ifdef BUILD_DEBUG
	device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // BUILD DEBUG

	hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, device_flags, feature_levels, array_count(feature_levels), D3D11_SDK_VERSION, &gfx_state.device, 0, &gfx_state.device_context);
	gfx_assert(hr, "failed to create device.");

	// get dxgi device, adaptor, and factory
	hr = gfx_state.device->QueryInterface(__uuidof(IDXGIDevice1), (void**)(&gfx_state.dxgi_device));
	gfx_assert(hr, "failed to get dxgi device.");
	hr = gfx_state.dxgi_device->GetAdapter(&gfx_state.dxgi_adapter);
	gfx_assert(hr, "failed to get dxgi adaptor.");
	hr = gfx_state.dxgi_adapter->GetParent(__uuidof(IDXGIFactory2), (void**)(&gfx_state.dxgi_factory));
	gfx_assert(hr, "failed to get dxgi factory.");

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
	gfx_assert(hr, "failed to create point sampler.");

	// linear sampler
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	hr = gfx_state.device->CreateSamplerState(&sampler_desc, &gfx_state.linear_sampler);
	gfx_assert(hr, "failed to create linear sampler.");

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
	hr = gfx_state.device->CreateDepthStencilState(&depth_stencil_desc, &gfx_state.depth_stencil_state);
	gfx_assert(hr, "failed to create depth stencil state.");

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
	hr = gfx_state.device->CreateDepthStencilState(&depth_stencil_desc, &gfx_state.no_depth_stencil_state);
	gfx_assert(hr, "failed to create non depth stencil state.");

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
	hr = gfx_state.device->CreateRasterizerState(&rasterizer_desc, &gfx_state.solid_rasterizer_state);
	gfx_assert(hr, "failed to create solid rasterizer state.");

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
	hr = gfx_state.device->CreateRasterizerState(&rasterizer_desc, &gfx_state.wireframe_rasterizer_state);
	gfx_assert(hr, "failed to create wireframe rasterizer state.");

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
	hr = gfx_state.device->CreateBlendState(&blend_state_desc, &gfx_state.blend_state);
	gfx_assert(hr, "failed to create blend state.");
	
	// create dwrite factory
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&gfx_state.dwrite_factory);
	gfx_assert(hr, "failed to create dwrite factory.");

	// create rendering params
	hr = gfx_state.dwrite_factory->CreateRenderingParams(&gfx_state.rendering_params);
	gfx_assert(hr, "failed to create dwrite rendering params.");
	f32 gamma = gfx_state.rendering_params->GetGamma();
	f32 enhanced_contrast = gfx_state.rendering_params->GetEnhancedContrast();
	f32 clear_type_level = gfx_state.rendering_params->GetClearTypeLevel();
	hr = gfx_state.dwrite_factory->CreateCustomRenderingParams(gamma, enhanced_contrast, clear_type_level, DWRITE_PIXEL_GEOMETRY_FLAT, DWRITE_RENDERING_MODE_DEFAULT, &gfx_state.rendering_params);
	gfx_assert(hr, "failed to create custom dwrite rendering params.");

	hr = gfx_state.dwrite_factory->GetGdiInterop(&gfx_state.gdi_interop);
	gfx_assert(hr, "failed to create gdi interop.");

	// create buffers
	D3D11_BUFFER_DESC buffer_desc = { 0 };

	// vertex buffer
	buffer_desc.ByteWidth = gfx_max_buffer_size;
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	hr = gfx_state.device->CreateBuffer(&buffer_desc, 0, &gfx_state.vertex_buffer);
	gfx_assert(hr, "failed to create vertex buffer.");


	// constant buffer
	buffer_desc.ByteWidth = 512;
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	hr = gfx_state.device->CreateBuffer(&buffer_desc, 0, &gfx_state.constant_buffer);
	gfx_assert(hr, "failed to create constant buffer.");

	// default texture
	u32 texture_data = 0xFFFFFFFF;
	gfx_state.default_texture = gfx_texture_create(str("default"), uvec2(1, 1), gfx_texture_format_rgba8, &texture_data);

	gfx_state.shader_2d = gfx_shader_load(str("res/shaders/shader_2d.hlsl"), { {
		{ "BBOX", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "UV",   0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "TYPE", 0, gfx_vertex_format_int,    gfx_vertex_class_per_instance },
		{ "POS",  0, gfx_vertex_format_float2, gfx_vertex_class_per_instance },
		{ "POS",  1, gfx_vertex_format_float2, gfx_vertex_class_per_instance },
		{ "POS",  2, gfx_vertex_format_float2, gfx_vertex_class_per_instance },
		{ "POS",  3, gfx_vertex_format_float2, gfx_vertex_class_per_instance },
		{ "COL",  0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "COL",  1, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "COL",  2, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "COL",  3, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "RAD",  0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "STY",  0, gfx_vertex_format_float2, gfx_vertex_class_per_instance },
	} });

	// stacks
	gfx_default_init(texture, gfx_state.default_texture);
	gfx_default_init(shader, gfx_state.shader_2d);
	gfx_default_init(clip, rect(0.0f, 0.0f, 0.0f, 0.0f));
	gfx_default_init(radius0, 0.0f);
	gfx_default_init(radius1, 0.0f);
	gfx_default_init(radius2, 0.0f);
	gfx_default_init(radius3, 0.0f);
	gfx_default_init(color0, color(0xffffffff));
	gfx_default_init(color1, color(0xffffffff));
	gfx_default_init(color2, color(0xffffffff));
	gfx_default_init(color3, color(0xffffffff));
	gfx_default_init(thickness, 0.0f);
	gfx_default_init(softness, 0.33f);

}

function void 
gfx_release() {
	
	// release buffers
	if (gfx_state.vertex_buffer != nullptr) { gfx_state.vertex_buffer->Release(); }
	if (gfx_state.constant_buffer != nullptr) { gfx_state.constant_buffer->Release(); }

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

	// release devices
	if (gfx_state.dxgi_factory != nullptr) { gfx_state.dxgi_factory->Release(); }
	if (gfx_state.dxgi_adapter != nullptr) { gfx_state.dxgi_adapter->Release(); }
	if (gfx_state.dxgi_device != nullptr) { gfx_state.dxgi_device->Release(); }
	if (gfx_state.device_context != nullptr) { gfx_state.device_context->Release(); }
	if (gfx_state.device != nullptr) { gfx_state.device->Release(); }

	// release arenas
	arena_release(gfx_state.scratch_arena);
	arena_release(gfx_state.batch_arena);
	arena_release(gfx_state.resource_arena);
	arena_release(gfx_state.renderer_arena);

}

function void
gfx_update() {

	// clear arenas
	arena_clear(gfx_state.batch_arena);
	arena_clear(gfx_state.scratch_arena);

	// TODO: hotloading

}

function void 
gfx_submit() {

	gfx_renderer_t* renderer = gfx_state.renderer_active;
	const FLOAT clear_color_array[] = { renderer->params.clear_color.r, renderer->params.clear_color.g, renderer->params.clear_color.b, renderer->params.clear_color.a };
	D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (f32)renderer->resolution.x, (f32)renderer->resolution.y, 0.0f, 1.0f };

	// clear
	gfx_state.device_context->OMSetRenderTargets(1, &renderer->framebuffer_rtv, renderer->depthbuffer_dsv);
	gfx_state.device_context->ClearRenderTargetView(renderer->framebuffer_rtv, clear_color_array);
	gfx_state.device_context->ClearDepthStencilView(renderer->depthbuffer_dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// set up common state
	gfx_state.device_context->OMSetBlendState(gfx_state.blend_state, nullptr, 0xffffffff);
	gfx_state.device_context->RSSetState(gfx_state.solid_rasterizer_state);
	gfx_state.device_context->RSSetViewports(1, &viewport);
	gfx_state.device_context->PSSetSamplers(0, 1, &gfx_state.point_sampler);
	gfx_state.device_context->PSSetSamplers(1, 1, &gfx_state.linear_sampler);

	// render 3d batches
	{
		// load constant buffer
		D3D11_MAPPED_SUBRESOURCE mapped_subresource = { 0 };
		gfx_state.device_context->Map(gfx_state.constant_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
		u8* ptr = (u8*)mapped_subresource.pData;
		memcpy(ptr, &gfx_state.constants_3d, sizeof(gfx_3d_constants_t));
		gfx_state.device_context->Unmap(gfx_state.constant_buffer, 0);

		// setup state
		gfx_state.device_context->OMSetDepthStencilState(gfx_state.depth_stencil_state, 1);
		gfx_state.device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		gfx_state.device_context->VSSetConstantBuffers(0, 1, &gfx_state.constant_buffer);
		gfx_state.device_context->PSSetConstantBuffers(0, 1, &gfx_state.constant_buffer);

		// render batches
		for (gfx_batch_t* batch = gfx_state.batch_list.first; batch != 0; batch = batch->next) {

			// skip non 3d batches
			if (batch->state.type != gfx_batch_type_3d) {
				continue;
			}

			// load vertex data into buffer
			u32 stride = batch->state.vertex_size;
			u32 offset = 0;
			D3D11_MAPPED_SUBRESOURCE mapped_subresource = { 0 };
			gfx_state.device_context->Map(gfx_state.vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
			u8* ptr = (u8*)mapped_subresource.pData;
			memcpy(ptr, batch->data, batch->vertex_count * batch->state.vertex_size);
			gfx_state.device_context->Unmap(gfx_state.vertex_buffer, 0);
			gfx_state.device_context->IASetVertexBuffers(0, 1, &gfx_state.vertex_buffer, &stride, &offset);

			// set texture
			gfx_state.device_context->PSSetShaderResources(0, 1, &batch->state.texture->srv);

			// set shader
			gfx_state.device_context->VSSetShader(batch->state.shader->vertex_shader, 0, 0);
			gfx_state.device_context->PSSetShader(batch->state.shader->pixel_shader, 0, 0);
			gfx_state.device_context->IASetInputLayout(batch->state.shader->input_layout);

			// set clip
			D3D11_RECT scissor_rect = {
				(i32)batch->state.clip_mask.x0, (i32)batch->state.clip_mask.y0,
				(i32)batch->state.clip_mask.x1 , (i32)batch->state.clip_mask.y1
			};
			gfx_state.device_context->RSSetScissorRects(1, &scissor_rect);

			// draw
			switch (batch->state.vertex_class) {
				case gfx_vertex_class_per_vertex: {
					gfx_state.device_context->Draw(batch->vertex_count, 0);
					break;
				}
			}

		}
	}

	// render 2d batches
	{
		// load constant buffer
		D3D11_MAPPED_SUBRESOURCE mapped_subresource = { 0 };
		gfx_state.device_context->Map(gfx_state.constant_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
		u8* ptr = (u8*)mapped_subresource.pData;
		memcpy(ptr, &gfx_state.constants_2d, sizeof(gfx_2d_constants_t));
		gfx_state.device_context->Unmap(gfx_state.constant_buffer, 0);

		// setup state
		gfx_state.device_context->OMSetDepthStencilState(gfx_state.no_depth_stencil_state, 1);
		gfx_state.device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		gfx_state.device_context->VSSetConstantBuffers(0, 1, &gfx_state.constant_buffer);
		gfx_state.device_context->PSSetConstantBuffers(0, 1, &gfx_state.constant_buffer);

		// render batches
		for (gfx_batch_t* batch = gfx_state.batch_list.first; batch != 0; batch = batch->next) {

			// skip non 2d batches
			if (batch->state.type != gfx_batch_type_2d) {
				continue;
			}

			// load vertex data into buffer
			u32 stride = batch->state.vertex_size;
			u32 offset = 0;
			D3D11_MAPPED_SUBRESOURCE mapped_subresource = { 0 };
			gfx_state.device_context->Map(gfx_state.vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
			u8* ptr = (u8*)mapped_subresource.pData;
			memcpy(ptr, batch->data, batch->vertex_count * batch->state.vertex_size);
			gfx_state.device_context->Unmap(gfx_state.vertex_buffer, 0);
			gfx_state.device_context->IASetVertexBuffers(0, 1, &gfx_state.vertex_buffer, &stride, &offset);

			// set texture
			gfx_state.device_context->PSSetShaderResources(0, 1, &batch->state.texture->srv);

			// set shader
			gfx_state.device_context->VSSetShader(batch->state.shader->vertex_shader, 0, 0);
			gfx_state.device_context->PSSetShader(batch->state.shader->pixel_shader, 0, 0);
			gfx_state.device_context->IASetInputLayout(batch->state.shader->input_layout);

			// set clip
			D3D11_RECT scissor_rect = {
				(i32)batch->state.clip_mask.x0, (i32)batch->state.clip_mask.y0,
				(i32)batch->state.clip_mask.x1 , (i32)batch->state.clip_mask.y1
			};
			gfx_state.device_context->RSSetScissorRects(1, &scissor_rect);

			switch (batch->state.vertex_class) {
				case gfx_vertex_class_per_vertex: {
					gfx_state.device_context->Draw(batch->vertex_count, 0);
					break;
				}
				case gfx_vertex_class_per_instance:	{
					// TODO: for now we assume that the vertex 
					// count is always 4. but that might not
					// always be true.
					gfx_state.device_context->DrawInstanced(4, batch->instance_count, 0, 0);
					break;
				}
			}
		}
	}

	// present
	renderer->swapchain->Present(1, 0);
	gfx_state.device_context->ClearState();

	// clear batches
	gfx_state.batch_list.count = 0;
	gfx_state.batch_list.first = nullptr;
	gfx_state.batch_list.last = nullptr;

}


// renderer

function gfx_renderer_t* 
gfx_renderer_create(os_window_t* window, gfx_renderer_params_t params) {

	// get or create renderer
	gfx_renderer_t* renderer = nullptr;
	renderer = gfx_state.renderer_free;
	if (renderer != nullptr) {
		stack_pop(gfx_state.renderer_free);
	} else {
		renderer = (gfx_renderer_t*)arena_alloc(gfx_state.renderer_arena, sizeof(gfx_renderer_t));
	}
	memset(renderer, 0, sizeof(gfx_renderer_t*));
	dll_push_back(gfx_state.renderer_first, gfx_state.renderer_last, renderer);
	gfx_state.renderer_count++;

	// fill renderer
	renderer->window = window;
	renderer->params = params;
	renderer->resolution = window->resolution;

	HRESULT hr = 0;

	// create swapchain
	DXGI_SWAP_CHAIN_DESC1 swapchain_desc = { 0 };
	swapchain_desc.Width = renderer->resolution.x;
	swapchain_desc.Height = renderer->resolution.y;
	swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchain_desc.Stereo = FALSE;
	swapchain_desc.SampleDesc.Count = params.sample_count;
	swapchain_desc.SampleDesc.Quality = 0;
	swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchain_desc.BufferCount = 2;
	swapchain_desc.Scaling = DXGI_SCALING_STRETCH;
	swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapchain_desc.Flags = 0;

	hr = gfx_state.dxgi_factory->CreateSwapChainForHwnd(gfx_state.device, window->handle, &swapchain_desc, 0, 0, &renderer->swapchain);
	gfx_assert(hr, "failed to create swapchain.");

	// create framebuffer and render view target
	hr = renderer->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&renderer->framebuffer));
	gfx_assert(hr, "failed to get framebuffer texture.");

	hr = gfx_state.device->CreateRenderTargetView(renderer->framebuffer, 0, &renderer->framebuffer_rtv);
	gfx_assert(hr, "failed to create render target view.");

	// create depth target
	D3D11_TEXTURE2D_DESC depthbuffer_desc = { 0 };
	depthbuffer_desc = { 0 };
	depthbuffer_desc.Width = renderer->resolution.x;
	depthbuffer_desc.Height = renderer->resolution.y;
	depthbuffer_desc.MipLevels = 1;
	depthbuffer_desc.ArraySize = 1;
	depthbuffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthbuffer_desc.SampleDesc.Count = params.sample_count;
	depthbuffer_desc.SampleDesc.Quality = 0;
	depthbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	depthbuffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthbuffer_desc.CPUAccessFlags = 0;
	depthbuffer_desc.MiscFlags = 0;
	hr = gfx_state.device->CreateTexture2D(&depthbuffer_desc, 0, &renderer->depthbuffer);
	gfx_assert(hr, "failed to create depthbuffer.");

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_dsv_desc = { 0 };
	depth_dsv_desc.Format = depthbuffer_desc.Format;
	depth_dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	hr = gfx_state.device->CreateDepthStencilView(renderer->depthbuffer, &depth_dsv_desc, &renderer->depthbuffer_dsv);
	gfx_assert(hr, "failed to create depth stencil view.");

	return renderer;
}

function void
gfx_renderer_release(gfx_renderer_t* renderer) {

	if (renderer->depthbuffer_srv != nullptr) { renderer->depthbuffer_srv->Release(); }
	if (renderer->depthbuffer_dsv != nullptr) { renderer->depthbuffer_dsv->Release(); }
	if (renderer->depthbuffer != nullptr) { renderer->depthbuffer->Release(); }

	if (renderer->framebuffer_srv != nullptr) { renderer->framebuffer_srv->Release(); }
	if (renderer->framebuffer_rtv != nullptr) { renderer->framebuffer_rtv->Release(); }
	if (renderer->framebuffer != nullptr) { renderer->framebuffer->Release(); }

	if (renderer->swapchain != nullptr) { renderer->swapchain->Release(); }

	dll_remove(gfx_state.renderer_first, gfx_state.renderer_last, renderer);
	gfx_state.renderer_count--;
	stack_push(gfx_state.renderer_free, renderer);
}

function void 
gfx_renderer_resize(gfx_renderer_t* renderer, uvec2_t resolution) {

	// skip is invalid resolution
	if (resolution.x == 0 || resolution.y == 0) {
		return;
	}

	gfx_state.device_context->OMSetRenderTargets(0, 0, 0);
	HRESULT hr = 0;

	// release buffers
	if (renderer->depthbuffer_srv != nullptr) { renderer->depthbuffer_srv->Release(); renderer->depthbuffer_srv = nullptr; }
	if (renderer->depthbuffer_dsv != nullptr) { renderer->depthbuffer_dsv->Release(); renderer->depthbuffer_dsv = nullptr; }
	if (renderer->depthbuffer != nullptr) { renderer->depthbuffer->Release(); renderer->depthbuffer = nullptr; }

	if (renderer->framebuffer_srv != nullptr) { renderer->framebuffer_srv->Release(); renderer->framebuffer_srv = nullptr; }
	if (renderer->framebuffer_rtv != nullptr) { renderer->framebuffer_rtv->Release(); renderer->framebuffer_rtv = nullptr; }
	if (renderer->framebuffer != nullptr) { renderer->framebuffer->Release(); renderer->framebuffer = nullptr; }

	// resize framebuffer
	hr = renderer->swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	gfx_assert(hr, "failed to resize framebuffer.");
	renderer->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&renderer->framebuffer));
	gfx_state.device->CreateRenderTargetView(renderer->framebuffer, 0, &renderer->framebuffer_rtv);

	// resize depthbuffer
	D3D11_TEXTURE2D_DESC depthbuffer_desc = { 0 };
	renderer->framebuffer->GetDesc(&depthbuffer_desc);
	depthbuffer_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthbuffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hr = gfx_state.device->CreateTexture2D(&depthbuffer_desc, 0, &renderer->depthbuffer);
	gfx_assert(hr, "failed to resize depthbuffer.");

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_dsv_desc = { 0 };
	depth_dsv_desc.Flags = 0;
	depth_dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_dsv_desc.Texture2D.MipSlice = 0;
	gfx_state.device->CreateDepthStencilView(renderer->depthbuffer, &depth_dsv_desc, &renderer->depthbuffer_dsv);
	gfx_assert(hr, "failed to create new depth stencil view.");

	// set new resolution
	renderer->resolution = resolution;
}

function void
gfx_renderer_begin_frame(gfx_renderer_t* renderer) {

	// set active renderer
	gfx_state.renderer_active = renderer;
	
	// resize if needed
	if (!uvec2_equals(renderer->resolution, renderer->window->resolution)) {
		gfx_renderer_resize(renderer, renderer->window->resolution);
	}

	// set default clip
	gfx_state.clip_default_node.v = rect(0.0f, 0.0f, (f32)renderer->resolution.x, (f32)renderer->resolution.y);

	// update constants
	gfx_state.constants_2d.window_size = vec2((f32)renderer->resolution.x, (f32)renderer->resolution.y);
	gfx_state.constants_2d.time = vec2(renderer->window->elasped_time, renderer->window->delta_time);

	// reset stacks
	gfx_stack_reset(texture);
	gfx_stack_reset(shader);
	gfx_stack_reset(clip);
	
	gfx_stack_reset(radius0);
	gfx_stack_reset(radius1);
	gfx_stack_reset(radius2);
	gfx_stack_reset(radius3);
	gfx_stack_reset(color0);
	gfx_stack_reset(color1);
	gfx_stack_reset(color2);
	gfx_stack_reset(color3);
	gfx_stack_reset(thickness);
	gfx_stack_reset(softness);

}

function void
gfx_renderer_end_frame(gfx_renderer_t* renderer) {

	// submit to gfx state
	gfx_submit();

	// clear active renderer
	gfx_state.renderer_active = nullptr;
}


// draw functions

function void
gfx_draw_quad(rect_t pos) {
	
	// find a batch
	gfx_batch_state_t state = { 0 };
	state.shader = gfx_state.shader_2d;
	state.texture = gfx_top_texture();
	state.clip_mask = gfx_top_clip();
	state.type = gfx_batch_type_2d;
	state.vertex_class = gfx_vertex_class_per_instance;
	state.instance_size = sizeof(gfx_2d_instance_t);
	gfx_batch_t* batch = gfx_batch_find(state, 1);

	// get instance
	gfx_2d_instance_t* instance = &((gfx_2d_instance_t*)batch->data)[batch->instance_count++];
	
	rect_validate(pos);

	// fill instance
	instance->bounding_box = pos;
	instance->uv = rect(0.0f, 0.0f, 1.0f, 1.0f);
	instance->type = 0;

	instance->col0 = gfx_top_color0().vec;
	instance->col1 = gfx_top_color1().vec;
	instance->col2 = gfx_top_color2().vec;
	instance->col3 = gfx_top_color3().vec;

	instance->r0 = gfx_top_radius0();
	instance->r1 = gfx_top_radius1();
	instance->r2 = gfx_top_radius2();
	instance->r3 = gfx_top_radius3();

	instance->thickness = gfx_top_thickness();
	instance->softness = gfx_top_softness();

	gfx_auto_pop_stacks();
}

function void
gfx_draw_line(vec2_t p0, vec2_t p1) {

	// find a batch
	gfx_batch_state_t state = { 0 };
	state.shader = gfx_state.shader_2d;
	state.texture = gfx_top_texture();
	state.clip_mask = gfx_top_clip();
	state.type = gfx_batch_type_2d;
	state.vertex_class = gfx_vertex_class_per_instance;
	state.instance_size = sizeof(gfx_2d_instance_t);
	gfx_batch_t* batch = gfx_batch_find(state, 1);

	// get instance
	gfx_2d_instance_t* instance = &((gfx_2d_instance_t*)batch->data)[batch->instance_count++];

	// calculate bounding box
	rect_t bbox = { p0.x, p0.y, p1.x, p1.y };
	rect_validate(bbox);
	f32 thickness = gfx_top_thickness();
	f32 softness = gfx_top_softness();
	bbox = rect_grow(bbox, roundf(thickness + softness));

	vec2_t c = rect_center(bbox);
	vec2_t c_p0 = vec2_sub(c, p0);
	vec2_t c_p1 = vec2_sub(c, p1);

	// fill instance
	instance->bounding_box = bbox;
	instance->uv = rect(0.0f, 0.0f, 1.0f, 1.0f);
	instance->type = 1;

	instance->col0 = gfx_top_color0().vec;
	instance->col1 = gfx_top_color1().vec;

	instance->p0 = { c_p0.x, c_p0.y };
	instance->p1 = { c_p1.x, c_p1.y };

	instance->thickness = thickness;
	instance->softness = softness;

	gfx_auto_pop_stacks();
}

function void
gfx_draw_text(str_t string, vec2_t pos, gfx_font_t* font, f32 font_size) {

	// find a batch
	gfx_batch_state_t state = { 0 };
	state.shader = gfx_state.shader_2d;
	state.texture = font->atlas_texture;
	state.clip_mask = gfx_top_clip();
	state.type = gfx_batch_type_2d;
	state.vertex_class = gfx_vertex_class_per_instance;
	state.instance_size = sizeof(gfx_2d_instance_t);
	gfx_batch_t* batch = gfx_batch_find(state, string.size);

	for (u32 i = 0; i < string.size; i++) {

		// get instance
		gfx_2d_instance_t* instance = &((gfx_2d_instance_t*)batch->data)[batch->instance_count++];

		u8 codepoint = *(string.data + i);
		gfx_font_glyph_t* glyph = gfx_font_get_glyph(font, codepoint, font_size);

		// fill instance
		instance->bounding_box = { pos.x, pos.y, pos.x + glyph->pos.x1, pos.y + glyph->pos.y1 };
		instance->uv = glyph->uv;
		instance->type = 0;

		instance->col0 = gfx_top_color0().vec;
		instance->col1 = gfx_top_color0().vec;
		instance->col2 = gfx_top_color0().vec;
		instance->col3 = gfx_top_color0().vec;

		instance->r0 = 0.0f;
		instance->r1 = 0.0f;
		instance->r2 = 0.0f;
		instance->r3 = 0.0f;

		instance->thickness = 0.0f;
		instance->softness = 0.0f;

		pos.x += glyph->advance;

	}

	gfx_auto_pop_stacks();
}

function void
gfx_draw_radial(vec2_t pos, f32 radius, f32 start_angle, f32 end_angle) {

	// find a batch
	gfx_batch_state_t state = { 0 };
	state.shader = gfx_state.shader_2d;
	state.texture = gfx_top_texture();
	state.clip_mask = gfx_top_clip();
	state.type = gfx_batch_type_2d;
	state.vertex_class = gfx_vertex_class_per_instance;
	state.instance_size = sizeof(gfx_2d_instance_t);
	gfx_batch_t* batch = gfx_batch_find(state, 1);

	// get instance
	gfx_2d_instance_t* instance = &((gfx_2d_instance_t*)batch->data)[batch->instance_count++];
	
	instance->bounding_box = { pos.x - radius, pos.y - radius, pos.x + radius, pos.y + radius };
	instance->uv = rect(0.0f, 0.0f, 1.0f, 1.0f);
	instance->type = 2;

	instance->p0 = { radians(start_angle), radians(end_angle) };

	instance->col0 = gfx_top_color0().vec;
	instance->col1 = gfx_top_color1().vec;
	instance->col2 = gfx_top_color2().vec;
	instance->col3 = gfx_top_color3().vec;

	instance->thickness = gfx_top_thickness();
	instance->softness = gfx_top_softness();

	gfx_auto_pop_stacks();
}

function void
gfx_draw_tri(vec2_t p0, vec2_t p1, vec2_t p2) {

	// find a batch
	gfx_batch_state_t state = { 0 };
	state.shader = gfx_state.shader_2d;
	state.texture = gfx_top_texture();
	state.clip_mask = gfx_top_clip();
	state.type = gfx_batch_type_2d;
	state.vertex_class = gfx_vertex_class_per_instance;
	state.instance_size = sizeof(gfx_2d_instance_t);
	gfx_batch_t* batch = gfx_batch_find(state, 1);

	// get instance
	gfx_2d_instance_t* instance = &((gfx_2d_instance_t*)batch->data)[batch->instance_count++];

	// calculate bounding box
	vec2_t points[3] = { p0, p1, p2 };
	rect_t bbox = rect_bbox(points, 3);
	f32 thickness = gfx_top_thickness();
	f32 softness = gfx_top_softness();
	bbox = rect_grow(bbox, 5.0f * roundf(thickness + softness));

	vec2_t c = rect_center(bbox);
	vec2_t c_p0 = vec2_sub(p0, c);
	vec2_t c_p1 = vec2_sub(p1, c);
	vec2_t c_p2 = vec2_sub(p2, c);

	instance->bounding_box = bbox;
	instance->uv = rect(0.0f, 0.0f, 1.0f, 1.0f);
	instance->type = 3;

	instance->p0 = c_p0;
	instance->p1 = c_p1;
	instance->p2 = c_p2;

	instance->col0 = gfx_top_color0().vec;
	instance->col1 = gfx_top_color1().vec;
	instance->col2 = gfx_top_color2().vec;

	instance->thickness = thickness;
	instance->softness = softness;

	gfx_auto_pop_stacks();
}

// batch functions

function b8
gfx_batch_state_equal(gfx_batch_state_t* state_a, gfx_batch_state_t* state_b) {
	b8 result = false;
	if ((state_a->shader == state_b->shader) &&
		(state_a->texture == state_b->texture) &&
		(state_a->type == state_b->type) &&
		(state_a->clip_mask.x0 == state_b->clip_mask.x0) &&
		(state_a->clip_mask.y0 == state_b->clip_mask.y0) &&
		(state_a->clip_mask.x1 == state_b->clip_mask.x1) &&
		(state_a->clip_mask.y1 == state_b->clip_mask.y1) &&
		(state_a->vertex_class == state_b->vertex_class) &&
		(state_a->vertex_size == state_b->vertex_size)) {
		result = true;
	}
	return result;
}

function gfx_batch_t*
gfx_batch_find(gfx_batch_state_t state, u32 count) {

	// search through current batches
	for (gfx_batch_t* batch = gfx_state.batch_list.first; batch != 0; batch = batch->next) {

		b8 batches_equal = gfx_batch_state_equal(&state, &batch->state);
		b8 batch_has_size = ((batch->vertex_count + count)* batch->state.vertex_size) < gfx_max_buffer_size;

		//if (!batches_equal) {
		//	printf("---------------------------\n");
		//	printf("shader: %.*s    ---   shader: %.*s\n", state.shader->name.size, state.shader->name.data, batch->state.shader->name.size, batch->state.shader->name.data);
		//	printf("texture: %.*s   ---   texture: %.*s\n", state.texture->name.size, state.texture->name.data, batch->state.texture->name.size, batch->state.texture->name.data);
		//	printf("type: %u        ---   type: %u\n", state.type, batch->state.type);
		//	printf("class: %u       ---   class: %u\n", state.vertex_class, batch->state.vertex_class);
		//	printf("size: %u        ---   size: %u\n", state.vertex_size, batch->state.vertex_size);
		//	printf("result: %s\n", batches_equal ? "equals" : "not equal");
		//	printf("---------------------------\n");
		//}
		
		if (batches_equal && batch_has_size) {
			return batch;
		}
	}

	// create a new batch if we didn't find one
	gfx_batch_t* batch = (gfx_batch_t*)arena_calloc(gfx_state.batch_arena, sizeof(gfx_batch_t));

	batch->state = state;
	batch->data = arena_alloc(gfx_state.batch_arena, gfx_max_buffer_size);
	batch->vertex_count = 0;

	dll_push_back(gfx_state.batch_list.first, gfx_state.batch_list.last, batch);
	gfx_state.batch_list.count++;

	return batch;
}

// buffer functions

function gfx_buffer_t*
gfx_buffer_create(gfx_usage_type usage, u32 size, void* data) {

	gfx_buffer_t* buffer = nullptr;
	buffer = gfx_state.buffer_free;
	if (buffer != nullptr) {
		stack_pop(gfx_state.buffer_free);
	} else {
		buffer = (gfx_buffer_t*)arena_alloc(gfx_state.resource_arena, sizeof(gfx_buffer_t));
	}
	memset(buffer, 0, sizeof(gfx_buffer_t));
	dll_push_back(gfx_state.buffer_first, gfx_state.buffer_last, buffer);
	gfx_state.buffer_count++;

	D3D11_USAGE d3d11_usage = D3D11_USAGE_DEFAULT;
	u32 cpu_access_flags = 0;
	d3d11_usage_type_to_d3d11_usage(usage, &d3d11_usage, &cpu_access_flags);

	D3D11_SUBRESOURCE_DATA initial_data = { 0 };
	initial_data.pSysMem = data;

	D3D11_BUFFER_DESC desc = { 0 };
	desc.ByteWidth = size;
	desc.Usage = d3d11_usage;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = cpu_access_flags;

	HRESULT hr = gfx_state.device->CreateBuffer(&desc, (data != nullptr) ? &initial_data : nullptr, &buffer->id);
	gfx_assert(hr, "failed to create buffer.");
		
	buffer->usage = usage;
	buffer->size = size;

	return buffer;
}

function void
gfx_buffer_release(gfx_buffer_t* buffer) {
	if (buffer->id != nullptr) { buffer->id->Release(); }
	dll_remove(gfx_state.buffer_first, gfx_state.buffer_last, buffer);
	gfx_state.buffer_count--;
	stack_push(gfx_state.buffer_free, buffer);
}



// texture functions

function gfx_texture_t*
gfx_texture_create(str_t name, uvec2_t size, gfx_texture_format format, void* data) {

	// get or create texture
	gfx_texture_t* texture = nullptr;
	texture = gfx_state.texture_free;
	if (texture != nullptr) {
		stack_pop(gfx_state.texture_free);
	} else {
		texture = (gfx_texture_t*)arena_alloc(gfx_state.resource_arena, sizeof(gfx_texture_t));
	}
	memset(texture, 0, sizeof(gfx_texture_t));
	dll_push_back(gfx_state.texture_first, gfx_state.texture_last, texture);
	gfx_state.texture_count++;

	// fill struct
	texture->name = name;
	texture->size = size;
	texture->format = format;

	// load data
	gfx_texture_load_buffer(texture, data);

	return texture;
}

function gfx_texture_t*
gfx_texture_load(str_t filepath) {

	// TODO: load png files
	gfx_texture_t* texture = nullptr;



	return texture;
}

function void
gfx_texture_release(gfx_texture_t* texture) {
	if (texture->id != nullptr) { texture->id->Release(); }
	if (texture->srv != nullptr) { texture->srv->Release(); }
	dll_remove(gfx_state.texture_first, gfx_state.texture_last, texture);
	gfx_state.texture_count--;
	stack_push(gfx_state.texture_free, texture);
}

function void
gfx_texture_load_buffer(gfx_texture_t* texture, void* data) {

	ID3D11Texture2D* id = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;

	HRESULT hr = 0;

	// set descriptors
	D3D11_TEXTURE2D_DESC texture_desc = { 0 };
	texture_desc.Width = texture->size.x;
	texture_desc.Height = texture->size.y;
	texture_desc.ArraySize = 1;
	texture_desc.Format = d3d11_texture_format_to_dxgi_format(texture->format);
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
	texture_data.SysMemPitch = texture->size.x * d3d11_texture_format_to_bytes(texture->format);

	// create texture
	hr = gfx_state.device->CreateTexture2D(&texture_desc, data ? &texture_data : nullptr, &id);
	gfx_check_error(hr, "failed to create texture: '%.*s'", texture->name.size, texture->name.data);

	// create srv
	hr = gfx_state.device->CreateShaderResourceView(id, &srv_desc, &srv);
	gfx_check_error(hr, "failed to create shader resource view for texture: '%.*s'.", texture->name.size, texture->name.data);

	if (SUCCEEDED(hr)) {

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

	if (dst_box.right > texture->size.x || dst_box.bottom > texture->size.y) {
		printf("[error] incorrect rect size.\n");
		return;
	} 

	u32 bytes = d3d11_texture_format_to_bytes(texture->format);
	gfx_state.device_context->UpdateSubresource(texture->id, 0, &dst_box, data, (rect.x1 - rect.x0) * bytes, 0);
}



// shader functions

function gfx_shader_t*
gfx_shader_create(str_t name, str_t src, gfx_shader_layout_t layout) {

	// get or create shader
	gfx_shader_t* shader = nullptr;
	shader = gfx_state.shader_free;
	if (shader != nullptr) {
		stack_pop(gfx_state.shader_free);
	} else {
		shader = (gfx_shader_t*)arena_alloc(gfx_state.resource_arena, sizeof(gfx_shader_t));
	}
	memset(shader, 0, sizeof(gfx_shader_t));
	dll_push_back(gfx_state.shader_first, gfx_state.shader_last, shader);
	gfx_state.shader_count++;

	// fill struct
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

	return shader;
}

function void
gfx_shader_release(gfx_shader_t* shader) {
	if (shader->vertex_shader != nullptr) { shader->vertex_shader->Release(); }
	if (shader->pixel_shader != nullptr) { shader->pixel_shader->Release(); }
	if (shader->input_layout != nullptr) { shader->input_layout->Release(); }
	dll_remove(gfx_state.shader_first, gfx_state.shader_last, shader);
	gfx_state.shader_count--;
	stack_push(gfx_state.shader_free, shader);
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

	D3D11_INPUT_ELEMENT_DESC input_element_desc[16] = { 0 };
	u32 attribute_count = 0;

	for (i32 i = 0; i < 16; i++) {
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
	if (vs_blob != nullptr) { vs_blob->Release(); }
	if (ps_blob != nullptr) { ps_blob->Release(); }
	if (vs_error_blob != nullptr) { vs_error_blob->Release(); }
	if (ps_error_blob != nullptr) { ps_error_blob->Release(); }
	shader->compiled = result;
	return result;
}



// font functions

function gfx_font_t*
gfx_font_load(str_t filepath) {

	gfx_font_t* font = (gfx_font_t*)arena_alloc(gfx_state.resource_arena, sizeof(gfx_font_t));

	// convert to wide path
	str16_t wide_filepath = str_to_str16(gfx_state.scratch_arena, filepath);

	// create font file and face
	gfx_state.dwrite_factory->CreateFontFileReference((WCHAR*)wide_filepath.data, 0, &font->file);
	gfx_state.dwrite_factory->CreateFontFace(DWRITE_FONT_FACE_TYPE_TRUETYPE, 1, &font->file, 0, DWRITE_FONT_SIMULATIONS_NONE, &(font->face));

	// glyph cache
	font->glyph_first = nullptr;
	font->glyph_last = nullptr;

	// atlas nodes
	font->root_size = vec2(1024.0f, 1024.0f);
	font->root = (gfx_font_atlas_node_t*)arena_alloc(gfx_state.resource_arena, sizeof(gfx_font_atlas_node_t));
	font->root->max_free_size[0] =
		font->root->max_free_size[1] =
		font->root->max_free_size[2] =
		font->root->max_free_size[3] = vec2_mul(font->root_size, 0.5f);

	// atlas texture
	str_t font_name = str_get_file_name(filepath);
	font->atlas_texture = gfx_texture_create(font_name, uvec2(1024, 1024), gfx_texture_format_rgba8, nullptr);

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
		glyph = (gfx_font_glyph_t*)arena_alloc(gfx_state.resource_arena, sizeof(gfx_font_glyph_t));
		glyph->hash = hash;
		glyph->advance = raster.advance;
		glyph->height = raster.height;
		glyph->pos = rect(0.0f, 0.0f, raster_size.x, raster_size.y);
		glyph->uv = rect(region.x0 / 1024.0f, region.y0 / 1024.0f, region.x1 / 1024.0f, region.y1 / 1024.0f);
		dll_push_back(font->glyph_first, font->glyph_last, glyph);

	}

	return glyph;
}

function gfx_font_raster_t
gfx_font_glyph_raster(arena_t* arena, gfx_font_t* font, u32 codepoint, f32 size) {

	// get font metrics
	DWRITE_FONT_METRICS font_metrics = { 0 };
	font->face->GetMetrics(&font_metrics);
	f32 design_units_per_em = font_metrics.designUnitsPerEm;
	f32 pixel_per_design_unit = ((96.0f / 72.0f) * size) / (f32)font_metrics.designUnitsPerEm;
	f32 ascent = (f32)font_metrics.ascent * pixel_per_design_unit;
	f32 descent = (f32)font_metrics.descent * pixel_per_design_unit;
	f32 capital_height = (f32)font_metrics.capHeight * pixel_per_design_unit;

	// get glyph indices
	u16 glyph_index;
	font->face->GetGlyphIndicesA(&codepoint, 1, &glyph_index);

	// get metrics info
	DWRITE_GLYPH_METRICS glyph_metrics = { 0 };
	font->face->GetGdiCompatibleGlyphMetrics(size, 1.0f, 0, 1, &glyph_index, 1, &glyph_metrics, 0);

	// determine atlas size
	i32 atlas_dim_x = (i32)(glyph_metrics.advanceWidth * pixel_per_design_unit);
	i32 atlas_dim_y = (i32)((font_metrics.ascent + font_metrics.descent) * pixel_per_design_unit);
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
	vec2_t draw_pos = { 1.0f, (f32)atlas_dim_y - descent };
	render_target->DrawGlyphRun(draw_pos.x, draw_pos.y, DWRITE_MEASURING_MODE_NATURAL, &glyph_run, gfx_state.rendering_params, RGB(255, 255, 255), &bounding_box);

	// get bitmap
	DIBSECTION dib = { 0 };
	HBITMAP bitmap = (HBITMAP)GetCurrentObject(dc, OBJ_BITMAP);
	GetObject(bitmap, sizeof(dib), &dib);

	gfx_font_raster_t raster = { 0 };

	raster.width = atlas_dim_x;
	raster.height = atlas_dim_y;
	raster.advance = floorf(advance);

	raster.data = (u8*)arena_alloc(arena, sizeof(u8) * atlas_dim_x * atlas_dim_y * 4);

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

					n->children[i] = (gfx_font_atlas_node_t*)arena_alloc(gfx_state.resource_arena, sizeof(gfx_font_atlas_node_t));
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
	result.x_height = (96.0f / 72.0f) * size * (f32)metrics.xHeight / design_units_per_em;

	return result;
}

function f32
gfx_font_text_width(gfx_font_t* font, f32 size, str_t string) {
	f32 width = 0.0f;
	for (u32 offset = 0; offset < string.size; offset++) {
		char c = *(string.data + offset);
		gfx_font_glyph_t* glyph = gfx_font_get_glyph(font, (u8)c, size);
		width += glyph->advance;
	}
	return width;
}

function f32
gfx_font_text_height(gfx_font_t* font, f32 size) {
	gfx_font_metrics_t metrics = gfx_font_get_metrics(font, size);
	f32 h = (metrics.ascent + metrics.descent);
	return h;
}

// mesh functions

function gfx_mesh_t*
gfx_mesh_create(str_t name, u32 vertex_size, u32 vertex_count) {

	gfx_mesh_t* mesh = (gfx_mesh_t*)arena_alloc(gfx_state.resource_arena, sizeof(gfx_mesh_t));

	// fill struct
	mesh->vertex_size = vertex_size;
	mesh->vertex_count = vertex_count;
	mesh->vertices = arena_alloc(gfx_state.resource_arena, vertex_size * vertex_count);

	return mesh;
}

function gfx_mesh_t* 
gfx_mesh_load(str_t filepath) {

	str_t filename = str_get_file_name(filepath);

	str_t data = os_file_read_all(gfx_state.scratch_arena, filepath);
	u8 splits[] = { '\n' };
	str_list_t lines = str_split(gfx_state.scratch_arena, data, splits, 1);

	// count vertices
	u32 position_count = 0;
	u32 tex_coord_count = 0;
	u32 normal_count = 0;
	u32 face_count = 0;

	for (str_node_t* line_node = lines.first; line_node != 0; line_node = line_node->next) {
		str_t line = line_node->string;
		if (line.data[0] == 'v' && line.data[1] == ' ') { position_count++; }
		if (line.data[0] == 'v' && line.data[1] == 't') { tex_coord_count++; }
		if (line.data[0] == 'v' && line.data[1] == 'n') { normal_count++; }
		if (line.data[0] == 'f') { face_count++; }
	}

	gfx_mesh_t* mesh = gfx_mesh_create(filename, sizeof(gfx_vertex_t), face_count * 3);
	gfx_vertex_t* vertices = (gfx_vertex_t*)mesh->vertices;

	vec3_t* positions = (vec3_t*)arena_alloc(gfx_state.scratch_arena, sizeof(vec3_t) * position_count);
	vec2_t* tex_coords = (vec2_t*)arena_alloc(gfx_state.scratch_arena, sizeof(vec2_t) * tex_coord_count);
	vec3_t* normals = (vec3_t*)arena_alloc(gfx_state.scratch_arena, sizeof(vec3_t) * normal_count);

	position_count = 0;
	tex_coord_count = 0;
	normal_count = 0;

	for (str_node_t* line_node = lines.first; line_node != 0; line_node = line_node->next) {
		str_t line = line_node->string;

		if (line.data[0] == 'v') {

			if (line.data[1] == ' ') { // position
				f32 x, y, z;
				str_scan(line, "v %f %f %f", &x, &y, &z);
				positions[position_count++] = { x, y, z };
			} else if (line.data[1] == 't') { // tex coords
				f32 u, v;
				str_scan(line, "vt %f %f", &u, &v);
				tex_coords[tex_coord_count++] = { u, v };
			} else if (line.data[1] == 'n') { // normals
				f32 x, y, z;
				str_scan(line, "vn %f %f %f", &x, &y, &z);
				normals[normal_count++] = { x, y, z };
			}

		}

		// face
		if (line.data[0] == 'f') {
			i32 i0, i1, i2;
			i32 t0, t1, t2;
			i32 n0, n1, n2;

			str_scan(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
				&i0, &t0, &n0,
				&i1, &t1, &n1,
				&i2, &t2, &n2);

			vertices[mesh->vertex_count++] = { positions[i0 - 1], normals[n0 - 1], vec3(0.0f), vec3(0.0f), tex_coords[t0 - 1], color(0xffffffff).vec };
			vertices[mesh->vertex_count++] = { positions[i1 - 1], normals[n1 - 1], vec3(0.0f), vec3(0.0f), tex_coords[t1 - 1], color(0xffffffff).vec };
			vertices[mesh->vertex_count++] = { positions[i2 - 1], normals[n2 - 1], vec3(0.0f), vec3(0.0f), tex_coords[t2 - 1], color(0xffffffff).vec };
			face_count++;
		}
	}


	return mesh;
}

function void 
gfx_mesh_release(gfx_mesh_t*) {

}


// stack functions

#define gfx_stack_top_impl(name, type) \
function type \
gfx_top_##name() { \
	return gfx_state.name##_stack.top->v; \
} \

#define gfx_stack_push_impl(name, type) \
function type \
gfx_push_##name(type v) { \
gfx_##name##_node_t* node = gfx_state.name##_stack.free; \
if (node != 0) { \
	stack_pop(gfx_state.name##_stack.free); \
} else { \
	node = (gfx_##name##_node_t*)arena_alloc(gfx_state.batch_arena, sizeof(gfx_##name##_node_t)); \
} \
type old_value = gfx_state.name##_stack.top->v; \
node->v = v; \
stack_push(gfx_state.name##_stack.top, node); \
gfx_state.name##_stack.auto_pop = 0; \
return old_value; \
} \

#define gfx_stack_pop_impl(name, type) \
function type \
gfx_pop_##name() { \
gfx_##name##_node_t* popped = gfx_state.name##_stack.top; \
if (popped != 0) { \
	stack_pop(gfx_state.name##_stack.top); \
	stack_push(gfx_state.name##_stack.free, popped); \
	gfx_state.name##_stack.auto_pop = 0; \
} \
return popped->v; \
} \

#define gfx_stack_set_next_impl(name, type) \
function type \
gfx_set_next_##name(type v) { \
gfx_##name##_node_t* node = gfx_state.name##_stack.free; \
if (node != 0) { \
	stack_pop(gfx_state.name##_stack.free); \
} else { \
	node = (gfx_##name##_node_t*)arena_alloc(gfx_state.batch_arena, sizeof(gfx_##name##_node_t)); \
} \
type old_value = gfx_state.name##_stack.top->v; \
node->v = v; \
stack_push(gfx_state.name##_stack.top, node); \
gfx_state.name##_stack.auto_pop = 1; \
return old_value; \
} \

#define gfx_stack_auto_pop_impl(name) \
if (gfx_state.name##_stack.auto_pop) { gfx_pop_##name(); gfx_state.name##_stack.auto_pop = 0; }

#define gfx_stack_impl(name, type)\
gfx_stack_top_impl(name, type)\
gfx_stack_push_impl(name, type)\
gfx_stack_pop_impl(name, type)\
gfx_stack_set_next_impl(name, type)\

function void
gfx_auto_pop_stacks() {

	gfx_stack_auto_pop_impl(texture);
	gfx_stack_auto_pop_impl(shader);
	gfx_stack_auto_pop_impl(clip);
	gfx_stack_auto_pop_impl(radius0);
	gfx_stack_auto_pop_impl(radius1);
	gfx_stack_auto_pop_impl(radius2);
	gfx_stack_auto_pop_impl(radius3);
	gfx_stack_auto_pop_impl(color0);
	gfx_stack_auto_pop_impl(color1);
	gfx_stack_auto_pop_impl(color2);
	gfx_stack_auto_pop_impl(color3);
	gfx_stack_auto_pop_impl(thickness);
	gfx_stack_auto_pop_impl(softness);

}

gfx_stack_impl(texture, gfx_texture_t*);
gfx_stack_impl(shader, gfx_shader_t*);
gfx_stack_impl(clip, rect_t);
gfx_stack_impl(radius0, f32);
gfx_stack_impl(radius1, f32);
gfx_stack_impl(radius2, f32);
gfx_stack_impl(radius3, f32);
gfx_stack_impl(color0, color_t);
gfx_stack_impl(color1, color_t);
gfx_stack_impl(color2, color_t);
gfx_stack_impl(color3, color_t);
gfx_stack_impl(thickness, f32);
gfx_stack_impl(softness, f32);

function void 
gfx_push_color(color_t color) {
	gfx_push_color0(color);
	gfx_push_color1(color);
	gfx_push_color2(color);
	gfx_push_color3(color);
}

function void 
gfx_set_next_color(color_t color) {
	gfx_set_next_color0(color);
	gfx_set_next_color1(color);
	gfx_set_next_color2(color);
	gfx_set_next_color3(color);
}

function void gfx_pop_color() {
	gfx_pop_color0();
	gfx_pop_color1();
	gfx_pop_color2();
	gfx_pop_color3();
}

function void
gfx_push_radius(f32 radius) {
	gfx_push_radius0(radius);
	gfx_push_radius1(radius);
	gfx_push_radius2(radius);
	gfx_push_radius3(radius);
}

function void 
gfx_set_next_radius(f32 radius) {
	gfx_set_next_radius0(radius);
	gfx_set_next_radius1(radius);
	gfx_set_next_radius2(radius);
	gfx_set_next_radius3(radius);
}

function void 
gfx_push_radii(vec4_t radii) {
	gfx_push_radius0(radii.x);
	gfx_push_radius1(radii.y);
	gfx_push_radius2(radii.z);
	gfx_push_radius3(radii.w);
}

function void 
gfx_set_next_radii(vec4_t radii) {
	gfx_set_next_radius0(radii.x);
	gfx_set_next_radius1(radii.y);
	gfx_set_next_radius2(radii.z);
	gfx_set_next_radius3(radii.w);
}

// enum helper functions

function void
d3d11_usage_type_to_d3d11_usage(gfx_usage_type type, D3D11_USAGE* out_d3d11_usage, UINT* out_cpu_access_flags) {
	switch (type) {
		case gfx_usage_type_static: {
			*out_d3d11_usage = D3D11_USAGE_IMMUTABLE;
			*out_cpu_access_flags = 0;
			break;
		}

		case gfx_usage_type_dynamic: {
			*out_d3d11_usage = D3D11_USAGE_DEFAULT;
			*out_cpu_access_flags = 0;
			break;
		}

		case gfx_usage_type_stream: {
			*out_d3d11_usage = D3D11_USAGE_DYNAMIC;
			*out_cpu_access_flags = D3D11_CPU_ACCESS_WRITE;
			break;
		}
	}
}

function D3D11_PRIMITIVE_TOPOLOGY
d3d11_topology_type_to_d3d11_topology(gfx_topology_type type) {
	D3D11_PRIMITIVE_TOPOLOGY topology;
	switch (type) {
		case gfx_topology_type_lines: { topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST; break; }
		case gfx_topology_type_line_strip: { topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP; break; }
		case gfx_topology_type_tris: { topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break; }
		case gfx_topology_type_tri_strip: { topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break; }
	}
	return topology;
}

function DXGI_FORMAT
d3d11_vertex_format_type_to_dxgi_format(gfx_vertex_format format) {
	DXGI_FORMAT result = DXGI_FORMAT_UNKNOWN;
	switch (format) {
		case gfx_vertex_format_float: { result = DXGI_FORMAT_R32_FLOAT; break; }
		case gfx_vertex_format_float2: { result = DXGI_FORMAT_R32G32_FLOAT; break; }
		case gfx_vertex_format_float3: { result = DXGI_FORMAT_R32G32B32_FLOAT; break; }
		case gfx_vertex_format_float4: { result = DXGI_FORMAT_R32G32B32A32_FLOAT; break; }
		case gfx_vertex_format_int: { result = DXGI_FORMAT_R32_SINT; break; }
		case gfx_vertex_format_int2: { result = DXGI_FORMAT_R32G32_SINT; break; }
		case gfx_vertex_format_int3: { result = DXGI_FORMAT_R32G32B32_SINT; break; }
		case gfx_vertex_format_int4: { result = DXGI_FORMAT_R32G32B32A32_SINT; break; }
		case gfx_vertex_format_uint: { result = DXGI_FORMAT_R32_UINT; break; }
		case gfx_vertex_format_uint2: { result = DXGI_FORMAT_R32G32_UINT; break; }
		case gfx_vertex_format_uint3: { result = DXGI_FORMAT_R32G32B32_UINT; break; }
		case gfx_vertex_format_uint4: { result = DXGI_FORMAT_R32G32B32A32_UINT; break; }
	}
	return result;
}

function DXGI_FORMAT
d3d11_texture_format_to_dxgi_format(gfx_texture_format format) {
	DXGI_FORMAT result = DXGI_FORMAT_R8G8B8A8_UNORM;
	switch (format) {
		case gfx_texture_format_r8: { result = DXGI_FORMAT_R8_UNORM; break; }
		case gfx_texture_format_rg8: { result = DXGI_FORMAT_R8G8_UNORM; break; }
		case gfx_texture_format_rgba8: { result = DXGI_FORMAT_R8G8B8A8_UNORM; break; }
		case gfx_texture_format_bgra8: { result = DXGI_FORMAT_B8G8R8A8_UNORM; break; }
		case gfx_texture_format_r16: { result = DXGI_FORMAT_R16_UNORM; break; }
		case gfx_texture_format_rgba16: { result = DXGI_FORMAT_R16G16B16A16_UNORM; break; }
		case gfx_texture_format_r32: { result = DXGI_FORMAT_R32_FLOAT; break; }
		case gfx_texture_format_rg32: { result = DXGI_FORMAT_R32G32_FLOAT; break; }
		case gfx_texture_format_rgba32: { result = DXGI_FORMAT_R32G32B32A32_FLOAT; break; }
	}
	return result;
}

function u32
d3d11_texture_format_to_bytes(gfx_texture_format format) {
	u32 result = 0;
	switch (format) {
		case gfx_texture_format_r8: { result = 1; break; }
		case gfx_texture_format_rg8: { result = 2; break; }
		case gfx_texture_format_rgba8: { result = 4; break; }
		case gfx_texture_format_bgra8: { result = 4; break; }
		case gfx_texture_format_r16: { result = 2; break; }
		case gfx_texture_format_rgba16: { result = 8; break; }
		case gfx_texture_format_r32: { result = 4; break; }
		case gfx_texture_format_rg32: { result = 8; break; }
		case gfx_texture_format_rgba32: { result = 16; break; }
	}
	return result;
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



#endif // GFX_CPP