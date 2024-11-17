// gfx_d3d11.cpp

#ifndef GFX_D3D11_CPP
#define GFX_D3D11_CPP

// include libs
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dwrite")

// implementation

// state
function void 
gfx_init() {

	// create arenas
	gfx_state.resource_arena = arena_create(gigabytes(1));
	gfx_state.scratch_arena = arena_create(megabytes(64));
	
	// init list
	gfx_state.buffer_first = gfx_state.buffer_last = gfx_state.buffer_free = nullptr;
	gfx_state.texture_first = gfx_state.texture_last = gfx_state.texture_free = nullptr;
	gfx_state.shader_first = gfx_state.shader_last = gfx_state.shader_free = nullptr;
	gfx_state.render_target_first = gfx_state.render_target_last = gfx_state.render_target_free = nullptr;

	gfx_state.renderer_active = nullptr;
	gfx_state.render_target_active = nullptr;

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


	// create pipeline assets

	// samplers
	{
		D3D11_SAMPLER_DESC sampler_desc = { 0 };
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.MipLODBias = 0;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		hr = gfx_state.device->CreateSamplerState(&sampler_desc, &gfx_state.nearest_wrap_sampler);
		gfx_assert(hr, "faield to create nearest wrap sampler.");

		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.MipLODBias = 0;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		hr = gfx_state.device->CreateSamplerState(&sampler_desc, &gfx_state.linear_wrap_sampler);
		gfx_assert(hr, "faield to create linear wrap sampler.");

		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.MipLODBias = 0;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		hr = gfx_state.device->CreateSamplerState(&sampler_desc, &gfx_state.linear_clamp_sampler);
		gfx_assert(hr, "faield to create linear clamp sampler.");

		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.MipLODBias = 0;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		hr = gfx_state.device->CreateSamplerState(&sampler_desc, &gfx_state.nearest_clamp_sampler);
		gfx_assert(hr, "faield to create nearest clamp sampler.");
	}

	// depth stencil states
	{
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = { 0 };
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

		depth_stencil_desc.DepthEnable = false;
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
		hr = gfx_state.device->CreateDepthStencilState(&depth_stencil_desc, &gfx_state.no_depth_stencil_state);
		gfx_assert(hr, "failed to create non depth stencil state.");
	}

	// rasterizers 
	{
		D3D11_RASTERIZER_DESC rasterizer_desc = { 0 };
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.FrontCounterClockwise = true;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0.0f;
		rasterizer_desc.SlopeScaledDepthBias = 0.0f;
		rasterizer_desc.DepthClipEnable = true;
		rasterizer_desc.ScissorEnable = true;
		rasterizer_desc.MultisampleEnable = true;
		rasterizer_desc.AntialiasedLineEnable = true;
		hr = gfx_state.device->CreateRasterizerState(&rasterizer_desc, &gfx_state.solid_cull_none_rasterizer);
		gfx_assert(hr, "failed to create solid cull none rasterizer state.");
		
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_FRONT;
		rasterizer_desc.FrontCounterClockwise = true;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0.0f;
		rasterizer_desc.SlopeScaledDepthBias = 0.0f;
		rasterizer_desc.DepthClipEnable = true;
		rasterizer_desc.ScissorEnable = true;
		rasterizer_desc.MultisampleEnable = true;
		rasterizer_desc.AntialiasedLineEnable = true;
		hr = gfx_state.device->CreateRasterizerState(&rasterizer_desc, &gfx_state.solid_cull_front_rasterizer);
		gfx_assert(hr, "failed to create solid cull front rasterizer state.");

		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.FrontCounterClockwise = true;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0.0f;
		rasterizer_desc.SlopeScaledDepthBias = 0.0f;
		rasterizer_desc.DepthClipEnable = true;
		rasterizer_desc.ScissorEnable = true;
		rasterizer_desc.MultisampleEnable = true;
		rasterizer_desc.AntialiasedLineEnable = true;
		hr = gfx_state.device->CreateRasterizerState(&rasterizer_desc, &gfx_state.solid_cull_back_rasterizer);
		gfx_assert(hr, "failed to create solid cull back rasterizer state.");

		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.FrontCounterClockwise = true;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0.0f;
		rasterizer_desc.SlopeScaledDepthBias = 0.0f;
		rasterizer_desc.DepthClipEnable = true;
		rasterizer_desc.ScissorEnable = true;
		rasterizer_desc.MultisampleEnable = true;
		rasterizer_desc.AntialiasedLineEnable = true;
		hr = gfx_state.device->CreateRasterizerState(&rasterizer_desc, &gfx_state.wireframe_cull_none_rasterizer);
		gfx_assert(hr, "failed to create wireframe cull none rasterizer state.");

		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_FRONT;
		rasterizer_desc.FrontCounterClockwise = true;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0.0f;
		rasterizer_desc.SlopeScaledDepthBias = 0.0f;
		rasterizer_desc.DepthClipEnable = true;
		rasterizer_desc.ScissorEnable = true;
		rasterizer_desc.MultisampleEnable = true;
		rasterizer_desc.AntialiasedLineEnable = true;
		hr = gfx_state.device->CreateRasterizerState(&rasterizer_desc, &gfx_state.wireframe_cull_front_rasterizer);
		gfx_assert(hr, "failed to create wireframe cull front rasterizer state.");

		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.FrontCounterClockwise = true;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0.0f;
		rasterizer_desc.SlopeScaledDepthBias = 0.0f;
		rasterizer_desc.DepthClipEnable = true;
		rasterizer_desc.ScissorEnable = true;
		rasterizer_desc.MultisampleEnable = true;
		rasterizer_desc.AntialiasedLineEnable = true;
		hr = gfx_state.device->CreateRasterizerState(&rasterizer_desc, &gfx_state.wireframe_cull_back_rasterizer);
		gfx_assert(hr, "failed to create wireframe cull back rasterizer state.");
	}

	// blend state
	{
		D3D11_BLEND_DESC blend_state_desc = { 0 };
		//blend_state_desc.AlphaToCoverageEnable = TRUE;
		//blend_state_desc.IndependentBlendEnable = FALSE;
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
	}

}

function void
gfx_release() {

	// release d3d11 devices
	if (gfx_state.dxgi_factory != nullptr) { gfx_state.dxgi_factory->Release(); }
	if (gfx_state.dxgi_adapter != nullptr) { gfx_state.dxgi_adapter->Release(); }
	if (gfx_state.dxgi_device != nullptr) { gfx_state.dxgi_device->Release(); }
	if (gfx_state.device_context != nullptr) { gfx_state.device_context->Release(); }
	if (gfx_state.device != nullptr) { gfx_state.device->Release(); }

	// release arenas
	arena_release(gfx_state.resource_arena);
	arena_release(gfx_state.scratch_arena);

}

function void
gfx_update() {
	
	// update renderers
	for (gfx_renderer_t* renderer = gfx_state.renderer_first; renderer != 0; renderer = renderer->next) {
		if (!uvec2_equals(renderer->resolution, renderer->window->resolution)) {
			gfx_renderer_resize(renderer, renderer->window->resolution);
		}
	}

	// hotload shaders
	for (gfx_shader_t* shader = gfx_state.shader_first; shader != 0; shader = shader->next) {

		// if shader was created from file
		if (shader->desc.filepath.size != 0) {

			// check if file has been updated
			os_file_attributes_t attributes = os_file_get_attributes(shader->desc.filepath);

			// recompile 
			if (shader->last_modified != attributes.last_modified) {

				// get new source
				str_t src = os_file_read_all(gfx_state.scratch_arena, shader->desc.filepath);

				// try to compile
				gfx_shader_compile(shader, src);

				// set new last updated
				shader->last_modified = attributes.last_modified;
			}
		}
	}



}



function void
gfx_draw(u32 vertex_count, u32 start_index) {
	gfx_state.device_context->Draw(vertex_count, start_index);
}

function void
gfx_draw_indexed(u32 index_count, u32 start_index, u32 offset) {
	gfx_state.device_context->DrawIndexed(index_count, start_index, offset);
}

function void
gfx_draw_instanced(u32 vertex_count, u32 instance_count, u32 start_vertex_index, u32 start_instance_index) {
	gfx_state.device_context->DrawInstanced(vertex_count, instance_count, start_vertex_index, start_instance_index);
}


// pipeline

function void 
gfx_set_sampler(gfx_filter_mode filter_mode, gfx_wrap_mode wrap_mode, u32 slot) {

	// choose samplers
	ID3D11SamplerState* sampler = nullptr;

	if (filter_mode == gfx_filter_linear && wrap_mode == gfx_wrap_repeat) {
		sampler = gfx_state.linear_wrap_sampler;
	} else if (filter_mode == gfx_filter_linear && wrap_mode == gfx_wrap_clamp) {
		sampler = gfx_state.linear_clamp_sampler;
	} else if (filter_mode == gfx_filter_nearest && wrap_mode == gfx_wrap_repeat) {
		sampler = gfx_state.nearest_wrap_sampler;
	} else if (filter_mode == gfx_filter_nearest && wrap_mode == gfx_wrap_clamp) {
		sampler = gfx_state.nearest_clamp_sampler;
	}
	
	// bind sampler
	gfx_state.device_context->PSSetSamplers(slot, 1, &sampler);
}

function void 
gfx_set_topology(gfx_topology_type topology_type) {
	D3D11_PRIMITIVE_TOPOLOGY topology = _topology_type_to_d3d11_topology(topology_type);
	gfx_state.device_context->IASetPrimitiveTopology(topology);
}

function void 
gfx_set_rasterizer(gfx_fill_mode fill_mode, gfx_cull_mode cull_mode) {

	ID3D11RasterizerState* rasterizer = nullptr;

	// get rasterizer state
	if (fill_mode == gfx_fill_solid) {
		if (cull_mode == gfx_cull_mode_none) {
			rasterizer = gfx_state.solid_cull_none_rasterizer;
		} else if (cull_mode == gfx_cull_mode_front) {
			rasterizer = gfx_state.solid_cull_front_rasterizer;
		} else if (cull_mode == gfx_cull_mode_back) {
			rasterizer = gfx_state.solid_cull_back_rasterizer;
		}
	} else if (fill_mode == gfx_fill_wireframe) {
		if (cull_mode == gfx_cull_mode_none) {
			rasterizer = gfx_state.wireframe_cull_none_rasterizer;
		} else if (cull_mode == gfx_cull_mode_front) {
			rasterizer = gfx_state.wireframe_cull_front_rasterizer;
		} else if (cull_mode == gfx_cull_mode_back) {
			rasterizer = gfx_state.wireframe_cull_back_rasterizer;
		}
	}

	// bind rasterizer state
	gfx_state.device_context->RSSetState(rasterizer);
}

function void 
gfx_set_viewport(rect_t viewport) {
	D3D11_VIEWPORT d3d11_viewport = { viewport.x0, viewport.y0, viewport.x1, viewport.y1, 0.0f, 1.0f };
	gfx_state.device_context->RSSetViewports(1, &d3d11_viewport);
}

function void 
gfx_set_scissor(rect_t scissor) {
	D3D11_RECT d3d11_rect = {
		(i32)scissor.x0, (i32)scissor.y0,
		(i32)scissor.x1 , (i32)scissor.y1
	};
	gfx_state.device_context->RSSetScissorRects(1, &d3d11_rect);
}

function void 
gfx_set_depth_mode(gfx_depth_mode depth_mode) {
	ID3D11DepthStencilState* state = nullptr;
	switch (depth_mode) {
		case gfx_depth: { state = gfx_state.depth_stencil_state; break; }
		case gfx_depth_none: { state = gfx_state.no_depth_stencil_state; break; }
	}
	gfx_state.device_context->OMSetDepthStencilState(state, 1);
}

function void 
gfx_set_pipeline(gfx_pipeline_t pipeline) {
	gfx_set_sampler(pipeline.filter_mode, pipeline.wrap_mode, 0);
	gfx_set_topology(pipeline.topology);
	gfx_set_rasterizer(pipeline.fill_mode, pipeline.cull_mode);
	gfx_set_viewport(pipeline.viewport);
	gfx_set_scissor(pipeline.scissor);
}

function void
gfx_set_buffer(gfx_buffer_t* buffer, u32 slot, u32 stride) {

	switch (buffer->desc.type) {
		case gfx_buffer_type_vertex: {
			u32 offset = 0;
			gfx_state.device_context->IASetVertexBuffers(slot, 1, &buffer->id, &stride, &offset);
			break;
		}
		case gfx_buffer_type_index:	{
			gfx_state.device_context->IASetIndexBuffer(buffer->id, DXGI_FORMAT_R32_UINT, 0);
			break;
		}
		case gfx_buffer_type_constant: {
			gfx_state.device_context->VSSetConstantBuffers(slot, 1, &buffer->id);
			gfx_state.device_context->PSSetConstantBuffers(slot, 1, &buffer->id);
			gfx_state.device_context->CSSetConstantBuffers(slot, 1, &buffer->id);
			break;
		}
	}

}

function void
gfx_set_texture(gfx_texture_t* texture, u32 slot) {
	gfx_state.device_context->PSSetShaderResources(slot, 1, &texture->srv);
}

function void
gfx_set_shader(gfx_shader_t* shader) {
	gfx_state.device_context->VSSetShader(shader->vertex_shader, 0, 0);
	gfx_state.device_context->PSSetShader(shader->pixel_shader, 0, 0);
	gfx_state.device_context->IASetInputLayout(shader->input_layout);
}

function void
gfx_set_render_target(gfx_render_target_t* render_target) {

	if (render_target == nullptr) {
		gfx_state.device_context->OMSetRenderTargets(0, nullptr, nullptr);
		gfx_state.render_target_active = nullptr;
		return;
	}

	color_t clear_color = gfx_state.renderer_active->clear_color;
	const FLOAT clear_color_array[] = { clear_color.r, clear_color.g, clear_color.b, clear_color.a };

	gfx_state.render_target_active = render_target;

	// Set the render targets and clear buffers
	//if (render_target->colorbuffer != nullptr) {
	//	gfx_state.device_context->OMSetRenderTargets(1, &render_target->colorbuffer_rtv, render_target->depthbuffer_dsv);
	//	gfx_state.device_context->ClearRenderTargetView(render_target->colorbuffer_rtv, clear_color_array);
	//} else {
	//	gfx_state.device_context->OMSetRenderTargets(0, nullptr, render_target->depthbuffer_dsv);
	//}

	//if (render_target->depthbuffer != nullptr) {
	//	gfx_state.device_context->ClearDepthStencilView(render_target->depthbuffer_dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//}

}

// renderer

function gfx_renderer_t*
gfx_renderer_create(os_window_t* window, color_t clear_color) {

	// get from resource pool or create one
	gfx_renderer_t* renderer = gfx_state.renderer_free;
	if (renderer != nullptr) {
		stack_pop(gfx_state.renderer_free);
	} else {
		renderer = (gfx_renderer_t*)arena_alloc(gfx_state.resource_arena, sizeof(gfx_renderer_t));
	}
	memset(renderer, 0, sizeof(gfx_renderer_t));
	dll_push_back(gfx_state.renderer_first, gfx_state.renderer_last, renderer);

	// fill
	renderer->window = window;
	renderer->clear_color = clear_color;
	renderer->resolution = window->resolution;

	// allocate graph arena
	renderer->graph_arena = arena_create(megabytes(64));

	// create swapchain
	HRESULT hr = 0;
	DXGI_SWAP_CHAIN_DESC1 swapchain_desc = { 0 };
	swapchain_desc.Width = renderer->resolution.x;
	swapchain_desc.Height = renderer->resolution.y;
	swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchain_desc.Stereo = FALSE;
	swapchain_desc.SampleDesc.Count = 1;
	swapchain_desc.SampleDesc.Quality = 0;
	swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchain_desc.BufferCount = 2;
	swapchain_desc.Scaling = DXGI_SCALING_STRETCH;
	swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapchain_desc.Flags = 0;

	hr = gfx_state.dxgi_factory->CreateSwapChainForHwnd(gfx_state.device, window->handle, &swapchain_desc, 0, 0, &renderer->swapchain);
	gfx_assert(hr, "failed to create swapchain.");

	// get framebuffer from swapchain
	hr = renderer->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&renderer->framebuffer));
	gfx_assert(hr, "failed to get framebuffer texture.");

	// create render target view
	hr = gfx_state.device->CreateRenderTargetView(renderer->framebuffer, 0, &renderer->framebuffer_rtv);
	gfx_assert(hr, "failed to create render target view.");

	return renderer;
}

function void 
gfx_renderer_release(gfx_renderer_t* renderer) {

	// release d3d11
	if (renderer->framebuffer_rtv != nullptr) { renderer->framebuffer_rtv->Release(); }
	if (renderer->framebuffer != nullptr) { renderer->framebuffer->Release(); }
	if (renderer->swapchain != nullptr) { renderer->swapchain->Release(); }

	// release arena
	arena_clear(renderer->graph_arena);

	// push to free stack
	dll_remove(gfx_state.renderer_first, gfx_state.renderer_last, renderer);
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
	if (renderer->framebuffer_rtv != nullptr) { renderer->framebuffer_rtv->Release(); renderer->framebuffer_rtv = nullptr; }
	if (renderer->framebuffer != nullptr) { renderer->framebuffer->Release(); renderer->framebuffer = nullptr; }

	// resize framebuffer
	hr = renderer->swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	gfx_assert(hr, "failed to resize framebuffer.");
	renderer->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&renderer->framebuffer));
	gfx_state.device->CreateRenderTargetView(renderer->framebuffer, 0, &renderer->framebuffer_rtv);

	// resize all passes
	for (gfx_render_pass_t* pass = renderer->pass_first; pass != 0; pass = pass->next) {
		// only resize if not fixed size
		//if (!(pass->render_target->desc.flags & gfx_render_target_flag_fixed_size)) {
		//	gfx_render_target_resize(pass->render_target, resolution);
		//}
	}

	// set new resolution
	renderer->resolution = resolution;
}

function gfx_render_pass_t*
gfx_renderer_add_pass(gfx_renderer_t* renderer, str_t name, gfx_render_pass_func* pass_function, gfx_render_target_desc_t desc) {

	gfx_render_pass_t* pass = (gfx_render_pass_t*)arena_alloc(renderer->graph_arena, sizeof(gfx_render_pass_t));
	
	// create render target
	pass->name = name;
	pass->pass_function = pass_function;
	pass->render_target = gfx_render_target_create_ex(desc);

	// add to list
	dll_push_back(renderer->pass_first, renderer->pass_last, pass);

	return pass;
}

function void
gfx_renderer_submit(gfx_renderer_t* renderer) {

	gfx_state.renderer_active = renderer;
	gfx_state.device_context->OMSetBlendState(gfx_state.blend_state, nullptr, 0xffffffff);

	// go through pass list
	for (gfx_render_pass_t* pass = renderer->pass_first; pass != 0; pass = pass->next) {

		gfx_set_render_target(pass->render_target);

		// get previous render target if can.
		gfx_render_target_t* prev_render_target = nullptr;
		if (pass->prev != nullptr && pass->prev->render_target != nullptr) {
			prev_render_target = pass->prev->render_target;
		}

		pass->pass_function(pass->render_target, prev_render_target);

	}
	// copy last pass to screen
	//if (renderer->pass_last->render_target->desc.sample_count > 1) {
	//	gfx_state.device_context->ResolveSubresource(renderer->framebuffer, 0, renderer->pass_last->render_target->colorbuffer->id, 0, _texture_format_to_dxgi_format(renderer->pass_last->render_target->desc.colorbuffer_format));
	//} else {
	//	gfx_state.device_context->CopyResource(renderer->framebuffer, renderer->pass_last->render_target->colorbuffer->id);
	//}
	renderer->swapchain->Present(1, 0);
	gfx_state.device_context->ClearState();

	gfx_state.renderer_active = nullptr;
}


// buffer
function gfx_buffer_t* 
gfx_buffer_create_ex(gfx_buffer_desc_t desc, void* data) {

	// get from resource pool or create one
	gfx_buffer_t* buffer = gfx_state.buffer_free;
	if (buffer != nullptr) {
		stack_pop(gfx_state.buffer_free);
	} else {
		buffer = (gfx_buffer_t*)arena_alloc(gfx_state.resource_arena, sizeof(gfx_buffer_t));
	}
	memset(buffer, 0, sizeof(gfx_buffer_t));
	dll_push_back(gfx_state.buffer_first, gfx_state.buffer_last, buffer);

	// fill description
	buffer->desc = desc;
	
	// create buffer
	HRESULT hr = 0;
	D3D11_BUFFER_DESC buffer_desc = { 0 };
	buffer_desc.ByteWidth = desc.size;
	buffer_desc.Usage = _usage_to_d3d11_usage(desc.usage);
	buffer_desc.BindFlags = _buffer_type_to_bind_flag(desc.type);
	buffer_desc.CPUAccessFlags = _usage_to_access_flags(desc.usage);
	buffer_desc.MiscFlags = 0;
	
	// initial data
	D3D11_SUBRESOURCE_DATA buffer_data = { 0 };
	if (data != nullptr) {
		buffer_data.pSysMem = data;
	}

	hr = gfx_state.device->CreateBuffer(&buffer_desc, data ? &buffer_data : nullptr, &buffer->id);
	gfx_assert(hr, "failed to create buffer.");

	return buffer;
}

function gfx_buffer_t* 
gfx_buffer_create(gfx_buffer_type type, u32 size, void* data){
	return gfx_buffer_create_ex({ type, size, gfx_usage_stream }, data);
}

function void 
gfx_buffer_release(gfx_buffer_t* buffer) {
	
	// release d3d11
	if (buffer->id != nullptr) { buffer->id->Release(); buffer->id = nullptr; }

	// push to free stack
	dll_remove(gfx_state.buffer_first, gfx_state.buffer_last, buffer);
	stack_push(gfx_state.buffer_free, buffer);

}

function void
gfx_buffer_fill(gfx_buffer_t* buffer, void* data, u32 size) {
	D3D11_MAPPED_SUBRESOURCE mapped_subresource = { 0 };
	HRESULT hr = gfx_state.device_context->Map(buffer->id, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	gfx_assert(hr, "failed to map buffer.");
	u8* ptr = (u8*)mapped_subresource.pData;
	memcpy(ptr, data, size);
	gfx_state.device_context->Unmap(buffer->id, 0);
}


// texture
function gfx_texture_t* 
gfx_texture_create_ex(gfx_texture_desc_t desc, void* data) {

	// get from resource pool or create one.
	gfx_texture_t* texture = gfx_state.texture_free;
	if (texture != nullptr) {
		stack_pop(gfx_state.texture_free);
	} else {
		texture = (gfx_texture_t*)arena_alloc(gfx_state.resource_arena, sizeof(gfx_texture_t));
	}
	memset(texture, 0, sizeof(gfx_texture_t));
	dll_push_back(gfx_state.texture_first, gfx_state.texture_last, texture);

	// fill description
	texture->desc = desc;

	// create d3d11 texture
	switch (desc.type) {

		// 2d texture
		case gfx_texture_type_2d: {

			HRESULT hr = 0;

			// determine bind flags
			D3D11_BIND_FLAG bind_flags = D3D11_BIND_SHADER_RESOURCE;
			if (desc.render_target) {
				if (_texture_format_is_depth(desc.format)) {
					bind_flags = (D3D11_BIND_FLAG)(bind_flags | D3D11_BIND_DEPTH_STENCIL);
				} else {
					bind_flags = (D3D11_BIND_FLAG)(bind_flags | D3D11_BIND_RENDER_TARGET);
				}
			}

			// fill out description
			D3D11_TEXTURE2D_DESC texture_desc = { 0 };
			texture_desc.Width = desc.size.x;
			texture_desc.Height = desc.size.y;
			texture_desc.ArraySize = 1;
			texture_desc.Format = _texture_format_to_dxgi_format(desc.format);
			texture_desc.SampleDesc.Count = desc.sample_count;
			texture_desc.SampleDesc.Quality = (desc.sample_count > 1) ? D3D11_STANDARD_MULTISAMPLE_PATTERN : 0;
			texture_desc.Usage = _usage_to_d3d11_usage(desc.usage);
			texture_desc.BindFlags = bind_flags;
			texture_desc.CPUAccessFlags = _usage_to_access_flags(desc.usage);
			texture_desc.MipLevels = 1;
			texture_desc.MiscFlags = 0;

			// initial data
			D3D11_SUBRESOURCE_DATA texture_data = { 0 };
			if (data != nullptr) {
				texture_data.pSysMem = data;
				texture_data.SysMemPitch = texture->desc.size.x * _texture_format_to_bytes(desc.format);
			}

			// create texture2d
			hr = gfx_state.device->CreateTexture2D(&texture_desc, data ? &texture_data : nullptr, &texture->id);
			gfx_assert(hr, "failed to create texture: '%.*s'.", desc.name.size, desc.name.data);


			// create srv
			D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = { 0 };
			srv_desc.Format = _texture_format_to_srv_dxgi_format(desc.format);
			if (desc.sample_count > 1) {
				srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			} else {
				srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srv_desc.Texture2D.MipLevels = 1; // TODO: support mips.
			}
			hr = gfx_state.device->CreateShaderResourceView(texture->id, &srv_desc, &texture->srv);
			gfx_assert(hr, "failed to create shader resource view for texture: '%.*s'.", desc.name.size, desc.name.data);

			break;
		}

		case gfx_texture_type_3d: {
			// TODO: implement 3d textures.
			break;
		}
	}
	

	return texture;
}

function gfx_texture_t* 
gfx_texture_create(uvec2_t size, gfx_texture_format format, void* data) {

	// fill description
	gfx_texture_desc_t desc = { 0 };
	desc.name = str("");
	desc.size = size;
	desc.format = format;
	desc.type = gfx_texture_type_2d;
	desc.sample_count = 1;
	desc.usage = gfx_usage_dynamic;
	desc.render_target = false;

	// create and return texture
	gfx_texture_t* texture = gfx_texture_create_ex(desc, data);

	return texture;
}

function gfx_texture_t*
gfx_texture_load(str_t filepath) {
	
	// load file
	i32 width = 0;
	i32 height = 0;
	i32 bpp = 0;

	stbi_set_flip_vertically_on_load(1);
	unsigned char* buffer = stbi_load((char*)filepath.data, &width, &height, &bpp, 4);

	// fill description
	gfx_texture_desc_t desc = { 0 };
	desc.name = str_get_file_name(filepath);
	desc.size = uvec2(width, height);
	desc.format = gfx_texture_format_rgba8;
	desc.type = gfx_texture_type_2d;
	desc.sample_count = 1;
	desc.usage = gfx_usage_dynamic;
	desc.render_target = false;

	// create and return texture
	gfx_texture_t* texture = gfx_texture_create_ex(desc, buffer);

	return texture;
}

function void
gfx_texture_release(gfx_texture_t* texture) {

	// release d3d11
	if (texture->id != nullptr) { texture->id->Release(); texture->id = nullptr; }
	if (texture->srv != nullptr) { texture->srv->Release(); texture->srv = nullptr; }

	// push to free stack
	dll_remove(gfx_state.texture_first, gfx_state.texture_last, texture);
	stack_push(gfx_state.texture_free, texture);
}

function void 
gfx_texture_fill(gfx_texture_t* texture, void* data) {
	D3D11_BOX dst_box = { 0, 0, 0, texture->desc.size.x, texture->desc.size.y, 1 };
	u32 src_row_pitch = texture->desc.size.x * _texture_format_to_bytes(texture->desc.format);
	gfx_state.device_context->UpdateSubresource(texture->id, 0, &dst_box, data, src_row_pitch, 0);
}

function void 
gfx_texture_fill_region(gfx_texture_t* texture, rect_t region, void* data) {
	D3D11_BOX dst_box = {
	  (UINT)region.x0, (UINT)region.y0, 0,
	  (UINT)region.x1, (UINT)region.y1, 1,
	};

	if (dst_box.right > texture->desc.size.x || dst_box.bottom > texture->desc.size.y) {
		printf("[error] incorrect region size.\n");
		return;
	}

	u32 bytes = _texture_format_to_bytes(texture->desc.format);
	u32 src_row_pitch = (region.x1 - region.x0) * bytes;
	gfx_state.device_context->UpdateSubresource(texture->id, 0, &dst_box, data, src_row_pitch, 0);
}

function void 
gfx_texture_blit(gfx_texture_t* texture_dst, gfx_texture_t* texture_src) {
	if (texture_dst->desc.format == texture_src->desc.format &&
		texture_dst->desc.size.x == texture_src->desc.size.x &&
		texture_dst->desc.size.y == texture_src->desc.size.y &&
		texture_dst->desc.type == texture_dst->desc.type) {

		// resolve if higher sample count
		if (texture_src->desc.sample_count > 1) {
			gfx_state.device_context->ResolveSubresource(texture_dst->id, 0, texture_src->id, 0, _texture_format_to_dxgi_format(texture_dst->desc.format));
		} else {
			gfx_state.device_context->CopyResource(texture_dst->id, texture_src->id);
		}
	}
}


// shaders
function gfx_shader_t* 
gfx_shader_create_ex(str_t src, gfx_shader_desc_t desc) {

	// get from resource pool or create
	gfx_shader_t* shader = gfx_state.shader_free;
	if (shader != nullptr) {
		stack_pop(gfx_state.shader_free);
	} else {
		shader = (gfx_shader_t*)arena_alloc(gfx_state.resource_arena, sizeof(gfx_shader_t));
	}
	memset(shader, 0, sizeof(gfx_shader_t));
	dll_push_back(gfx_state.shader_first, gfx_state.shader_last, shader);

	// fill description
	shader->desc = desc;

	// compile vertex shader
	gfx_shader_compile(shader, src);
	
	return shader;
}

function gfx_shader_t* 
gfx_shader_create(str_t src, str_t name, gfx_shader_attribute_t* attribute_list, u32 attribute_count) {

	// fill description
	gfx_shader_desc_t desc = { 0 };
	desc.name = name;
	desc.filepath = str("");
	desc.attributes = attribute_list;
	desc.attribute_count = attribute_count;

	// create and return shader
	gfx_shader_t* shader = gfx_shader_create_ex(src, desc);

	return shader;
}

function gfx_shader_t* 
gfx_shader_load(str_t filepath, gfx_shader_attribute_t* attribute_list, u32 attribute_count) {

	// load src from file
	str_t src = os_file_read_all(gfx_state.scratch_arena, filepath);

	// fill description
	gfx_shader_desc_t desc = { 0 };
	desc.name = str_get_file_name(filepath);
	desc.filepath = filepath;
	desc.attributes = attribute_list;
	desc.attribute_count = attribute_count;

	// create and return shader
	gfx_shader_t* shader = gfx_shader_create_ex(src, desc);

	// get last modified time
	os_file_attributes_t file_attributes = os_file_get_attributes(filepath);
	shader->last_modified = file_attributes.last_modified;

	return shader;
}

function void
gfx_shader_release(gfx_shader_t* shader) {

	// release d3d11
	if (shader->vertex_shader != nullptr) { shader->vertex_shader->Release(); shader->vertex_shader = nullptr; }
	if (shader->pixel_shader != nullptr) { shader->pixel_shader->Release(); shader->pixel_shader = nullptr; }
	if (shader->input_layout != nullptr) { shader->input_layout->Release(); shader->input_layout = nullptr; }

	// push to free stack
	dll_remove(gfx_state.shader_first, gfx_state.shader_last, shader);
	stack_push(gfx_state.shader_free, shader);
}

function void
gfx_shader_compile(gfx_shader_t* shader, str_t src) {

	HRESULT hr;
	b8 success = false;
	ID3DBlob* vs_blob = nullptr;
	ID3DBlob* ps_blob = nullptr;
	ID3DBlob* vs_error_blob = nullptr;
	ID3DBlob* ps_error_blob = nullptr;
	ID3D11VertexShader* vertex_shader = nullptr;
	ID3D11PixelShader* pixel_shader = nullptr;
	ID3D11InputLayout* input_layout = nullptr;

	u32 compile_flags = 0;

#if defined(BUILD_DEBUG)
	compile_flags |= D3DCOMPILE_DEBUG;
#endif 

	if (src.size == 0) {
		goto shader_build_cleanup;
	}

	// compile vertex shader
	hr = D3DCompile(src.data, src.size, (char*)shader->desc.name.data, 0, 0, "vs_main", "vs_5_0", compile_flags, 0, &vs_blob, &vs_error_blob);
	if (vs_error_blob) {
		cstr error_msg = (cstr)vs_error_blob->GetBufferPointer();
		printf("[error] failed to compile vertex shader:\n\n%s\n", error_msg);
		goto shader_build_cleanup;
	}
	hr = gfx_state.device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, &vertex_shader);

	// compile pixel shader
	hr = D3DCompile(src.data, src.size, (char*)shader->desc.name.data, 0, 0, "ps_main", "ps_5_0", compile_flags, 0, &ps_blob, &ps_error_blob);
	if (ps_error_blob) {
		cstr error_msg = (cstr)ps_error_blob->GetBufferPointer();
		printf("[error] failed to compile pixel shader:\n\n%s\n", error_msg);
		goto shader_build_cleanup;
	}
	hr = gfx_state.device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, &pixel_shader);

	// create input layout
	gfx_shader_attribute_t* attributes = shader->desc.attributes;

	// allocate input element array
	D3D11_INPUT_ELEMENT_DESC* input_element_desc = (D3D11_INPUT_ELEMENT_DESC*)arena_alloc(gfx_state.scratch_arena, sizeof(D3D11_INPUT_ELEMENT_DESC) * shader->desc.attribute_count);

	for (i32 i = 0; i < shader->desc.attribute_count; i++) {
		input_element_desc[i] = {
			(char*)attributes[i].name,
			attributes[i].slot,
			_vertex_format_to_dxgi_format(attributes[i].format),
			0, D3D11_APPEND_ALIGNED_ELEMENT,
			_vertex_class_to_input_class(attributes[i].classification),
			(attributes[i].classification == gfx_vertex_class_per_vertex) ? (u32)0 : (u32)1
		};
	}

	hr = gfx_state.device->CreateInputLayout(input_element_desc, shader->desc.attribute_count, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &input_layout);
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

		success = true;

		printf("[info] successfully created shader: '%.*s'\n", shader->desc.name.size, shader->desc.name.data);
	}

shader_build_cleanup:

	if (vs_blob != nullptr) { vs_blob->Release(); }
	if (ps_blob != nullptr) { ps_blob->Release(); }
	if (vs_error_blob != nullptr) { vs_error_blob->Release(); }
	if (ps_error_blob != nullptr) { ps_error_blob->Release(); }

	if (!success) {
		if (vertex_shader != nullptr) { vertex_shader->Release(); }
		if (pixel_shader != nullptr) { pixel_shader->Release(); }
		if (input_layout != nullptr) { input_layout->Release(); }
	}

}



// render target
function gfx_render_target_t* 
gfx_render_target_create_ex(gfx_render_target_desc_t desc) {

	// get from resource pool or create one
	gfx_render_target_t* render_target = gfx_state.render_target_free;
	if (render_target != nullptr) {
		stack_pop(gfx_state.render_target_free);
	} else {
		render_target = (gfx_render_target_t*)arena_alloc(gfx_state.resource_arena, sizeof(gfx_render_target_t));
	}
	memset(render_target, 0, sizeof(gfx_render_target_t));
	dll_push_back(gfx_state.render_target_first, gfx_state.render_target_last, render_target);

	// fill description
	//render_target->desc = desc;

	// create textures
	_gfx_render_target_create_resources(render_target);

	return render_target;

}

function gfx_render_target_t* 
gfx_render_target_create(uvec2_t size, gfx_render_target_flags flags, gfx_texture_format colorbuffer_format, gfx_texture_format depthbuffer_format) {

	// fill description
	gfx_render_target_desc_t desc = { 0 };
	desc.size = size;
	desc.sample_count = 1;
	if (colorbuffer_format == _gfx_texture_format_null) {
		desc.flags |= gfx_render_target_flag_no_color;
	}
	if (depthbuffer_format == _gfx_texture_format_null) {
		desc.flags |= gfx_render_target_flag_no_depth;
	}
	desc.colorbuffer_format = colorbuffer_format;
	desc.depthbuffer_format = depthbuffer_format;

	// create and return render target
	gfx_render_target_t* render_target = gfx_render_target_create_ex(desc);

	return render_target;
}

function void 
gfx_render_target_resize(gfx_render_target_t* render_target, uvec2_t size) {
	//if (size.x > 0 && size.y > 0) {
	//	render_target->desc.size = size;
	//	_gfx_render_target_create_resources(render_target);
	//}
}

function void 
_gfx_render_target_create_resources(gfx_render_target_t* render_target) {

	////gfx_render_target_desc_t desc = render_target->desc;
	//HRESULT hr = 0;

	//// create colorbuffer
	//if (!(desc.flags & gfx_render_target_flag_no_color)) {

	//	gfx_texture_desc_t colorbuffer_desc = { 0 };
	//	colorbuffer_desc.name = str("");
	//	colorbuffer_desc.size = desc.size;
	//	colorbuffer_desc.format = desc.colorbuffer_format;
	//	colorbuffer_desc.type = gfx_texture_type_2d;
	//	colorbuffer_desc.sample_count = desc.sample_count;
	//	colorbuffer_desc.usage = gfx_usage_dynamic;
	//	colorbuffer_desc.render_target = true;
	//	//render_target->colorbuffer = gfx_texture_create_ex(colorbuffer_desc);

	//	// create rtv
	//	D3D11_RENDER_TARGET_VIEW_DESC rtv_desc = { 0 };
	//	rtv_desc.Format = _texture_format_to_dxgi_format(desc.colorbuffer_format);
	//	if (desc.sample_count > 1) {
	//		rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	//	} else {
	//		rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	//		rtv_desc.Texture2D.MipSlice = 0;
	//	}

	//	//hr = gfx_state.device->CreateRenderTargetView(render_target->colorbuffer->id, &rtv_desc, &render_target->colorbuffer_rtv);
	//	gfx_check_error(hr, "failed to create the color rtv for render target.");

	//}

	//// create depth buffer
	//if (!(desc.flags & gfx_render_target_flag_no_depth)) {

	//	gfx_texture_desc_t depthbuffer_desc = { 0 };
	//	depthbuffer_desc.name = str("");
	//	depthbuffer_desc.size = desc.size;
	//	depthbuffer_desc.format = desc.depthbuffer_format;
	//	depthbuffer_desc.type = gfx_texture_type_2d;
	//	depthbuffer_desc.sample_count = desc.sample_count;
	//	depthbuffer_desc.usage = gfx_usage_dynamic;
	//	depthbuffer_desc.render_target = true;
	//	render_target->depthbuffer = gfx_texture_create_ex(depthbuffer_desc);
	//	
	//	// create depth stencil view
	//	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc = { 0 };
	//	dsv_desc.Format = _texture_format_to_dsv_dxgi_format(desc.depthbuffer_format);
	//	if (desc.sample_count > 1) {
	//		dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	//	} else {
	//		dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//		dsv_desc.Texture2D.MipSlice = 0;
	//	}
	//	hr = gfx_state.device->CreateDepthStencilView(render_target->depthbuffer->id, &dsv_desc, &render_target->depthbuffer_dsv);
	//	gfx_check_error(hr, "failed to create the depth dsv for render target.");

	//}

}




// d3d11 enum functions
function D3D11_USAGE
_usage_to_d3d11_usage(gfx_usage usage) {
	D3D11_USAGE result_usage;
	switch (usage) {
		case gfx_usage_static: { result_usage = D3D11_USAGE_IMMUTABLE; break; }
		case gfx_usage_dynamic: { result_usage = D3D11_USAGE_DEFAULT; break; }
		case gfx_usage_stream: { result_usage = D3D11_USAGE_DYNAMIC; break; }
	}
	return result_usage;
}

function UINT
_usage_to_access_flags(gfx_usage usage) {
	UINT access_flag;
	switch (usage) {
		case gfx_usage_static: { access_flag = 0; break; }
		case gfx_usage_dynamic: { access_flag = 0; break; }
		case gfx_usage_stream: { access_flag = D3D11_CPU_ACCESS_WRITE; break; }
	}
	return access_flag;
}

function D3D11_BIND_FLAG 
_buffer_type_to_bind_flag(gfx_buffer_type type) {
	D3D11_BIND_FLAG flag;
	switch (type) {
		case gfx_buffer_type_vertex: { flag = D3D11_BIND_VERTEX_BUFFER; break; }
		case gfx_buffer_type_index: { flag = D3D11_BIND_INDEX_BUFFER; break; }
		case gfx_buffer_type_constant: { flag = D3D11_BIND_CONSTANT_BUFFER; break; }
	}
	return flag;
}

function DXGI_FORMAT 
_texture_format_to_dxgi_format(gfx_texture_format format) {
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
		case gfx_texture_format_d24s8: { result = DXGI_FORMAT_R24G8_TYPELESS; break; }
		case gfx_texture_format_d32: { result = DXGI_FORMAT_R32_TYPELESS; break; }
	}
	return result;
}

function DXGI_FORMAT
_texture_format_to_srv_dxgi_format(gfx_texture_format format) {
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
		case gfx_texture_format_d24s8: { result = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break; }
		case gfx_texture_format_d32: { result = DXGI_FORMAT_R32_FLOAT; break; }
	}
	return result;
}

function DXGI_FORMAT
_texture_format_to_dsv_dxgi_format(gfx_texture_format format) {
	DXGI_FORMAT result = DXGI_FORMAT_R8G8B8A8_UNORM;
	switch (format) {
		case gfx_texture_format_d24s8: { result = DXGI_FORMAT_D24_UNORM_S8_UINT; break; }
		case gfx_texture_format_d32: { result = DXGI_FORMAT_D32_FLOAT; break; }
	}
	return result;
}

function u32 
_texture_format_to_bytes(gfx_texture_format format) {
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
		case gfx_texture_format_d24s8: { result = 16; break; }
		case gfx_texture_format_d32: { result = 8; break; }
	}
	return result;
}

function D3D11_PRIMITIVE_TOPOLOGY 
_topology_type_to_d3d11_topology(gfx_topology_type type) {
	D3D11_PRIMITIVE_TOPOLOGY topology;
	switch (type) {
		case gfx_topology_points: { topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST; break; }
		case gfx_topology_lines: { topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST; break; }
		case gfx_topology_line_strip: { topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP; break; }
		case gfx_topology_tris: { topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break; }
		case gfx_topology_tri_strip: { topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break; }
	}
	return topology;
}

function DXGI_FORMAT 
_vertex_format_to_dxgi_format(gfx_vertex_format format) {
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
_uniform_type_to_dxgi_format(gfx_uniform_type type) {
	DXGI_FORMAT result = DXGI_FORMAT_UNKNOWN;
	switch (type) {
		case gfx_uniform_type_float: { result = DXGI_FORMAT_R32_FLOAT; break; }
		case gfx_uniform_type_float2: { result = DXGI_FORMAT_R32G32_FLOAT; break; }
		case gfx_uniform_type_float3: { result = DXGI_FORMAT_R32G32B32_FLOAT; break; }
		case gfx_uniform_type_float4: { result = DXGI_FORMAT_R32G32B32A32_FLOAT; break; }
		case gfx_uniform_type_int: { result = DXGI_FORMAT_R32_SINT; break; }
		case gfx_uniform_type_int2: { result = DXGI_FORMAT_R32G32_SINT; break; }
		case gfx_uniform_type_int3: { result = DXGI_FORMAT_R32G32B32_SINT; break; }
		case gfx_uniform_type_int4: { result = DXGI_FORMAT_R32G32B32A32_SINT; break; }
		case gfx_uniform_type_uint: { result = DXGI_FORMAT_R32_UINT; break; }
		case gfx_uniform_type_uint2: { result = DXGI_FORMAT_R32G32_UINT; break; }
		case gfx_uniform_type_uint3: { result = DXGI_FORMAT_R32G32B32_UINT; break; }
		case gfx_uniform_type_uint4: { result = DXGI_FORMAT_R32G32B32A32_UINT; break; }
	}
	return result;
}

function D3D11_INPUT_CLASSIFICATION 
_vertex_class_to_input_class(gfx_vertex_class vertex_class) {
	D3D11_INPUT_CLASSIFICATION shader_classification;
	switch (vertex_class) {
		case gfx_vertex_class_per_vertex: { shader_classification = D3D11_INPUT_PER_VERTEX_DATA; break; }
		case gfx_vertex_class_per_instance: { shader_classification = D3D11_INPUT_PER_INSTANCE_DATA; break; }
	}
	return shader_classification;
}


#endif // GFX_D3D11_CPP