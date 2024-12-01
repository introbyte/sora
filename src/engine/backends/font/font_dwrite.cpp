// font_dwrite.cpp

#ifndef FONT_DWRITE_CPP
#define FONT_DWRITE_CPP

// include lib
#pragma comment(lib, "dwrite")

// implementation

// state functions
function void
font_init() {

	// create arenas
	font_state.font_arena = arena_create(megabytes(64));
	font_state.scratch_arena = arena_create(megabytes(64));
	HRESULT hr = 0;

	// create dwrite factory
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&font_state.dwrite_factory);
	gfx_assert(hr, "failed to create dwrite factory.");

	// create rendering params
	hr = font_state.dwrite_factory->CreateRenderingParams(&font_state.rendering_params);
	gfx_assert(hr, "failed to create dwrite rendering params.");

	f32 gamma = font_state.rendering_params->GetGamma();
	f32 enhanced_contrast = font_state.rendering_params->GetEnhancedContrast();
	f32 clear_type_level = font_state.rendering_params->GetClearTypeLevel();
	hr = font_state.dwrite_factory->CreateCustomRenderingParams(gamma, enhanced_contrast, clear_type_level, DWRITE_PIXEL_GEOMETRY_FLAT, DWRITE_RENDERING_MODE_GDI_NATURAL, &font_state.rendering_params);
	gfx_assert(hr, "failed to create custom dwrite rendering params.");

	// create gdi interop
	hr = font_state.dwrite_factory->GetGdiInterop(&font_state.gdi_interop);
	gfx_assert(hr, "failed to create gdi interop.");
	
	// glyph cache
	font_state.glyph_first = nullptr;
	font_state.glyph_last = nullptr;

	// atlas nodes
	font_state.root_size = vec2(font_atlas_size, font_atlas_size);
	font_state.root = (font_atlas_node_t*)arena_alloc(font_state.font_arena, sizeof(font_atlas_node_t));
	font_state.root->max_free_size[0] =
	font_state.root->max_free_size[1] =
	font_state.root->max_free_size[2] =
	font_state.root->max_free_size[3] = vec2_mul(font_state.root_size, 0.5f);

	// atlas texture
	font_state.atlas_texture = gfx_texture_create(uvec2((u32)font_atlas_size, (u32)font_atlas_size));

}

function void
font_release() {

	// release dwrite
	if (font_state.gdi_interop != nullptr) { font_state.gdi_interop->Release();  font_state.gdi_interop = nullptr; }
	if (font_state.rendering_params != nullptr) { font_state.rendering_params->Release(); font_state.rendering_params = nullptr; }
	if (font_state.dwrite_factory != nullptr) { font_state.dwrite_factory->Release(); font_state.dwrite_factory = nullptr; }

	// release assets
	gfx_texture_release(font_state.atlas_texture);

	// release arenas
	arena_release(font_state.font_arena);
	arena_release(font_state.scratch_arena);

}


//interface functions
function font_t*
font_open(str_t filepath) {

	// get from font pool or create one
	font_t* font = font_state.font_free;
	if (font != nullptr) {
		font = stack_pop(font_state.font_free);
	} else {
		font = (font_t*)arena_alloc(font_state.font_arena, sizeof(font_t));
	}
	memset(font, 0, sizeof(font_t));
	dll_push_back(font_state.font_first, font_state.font_last, font);

	// convert to wide path
	str16_t wide_filepath = str_to_str16(font_state.scratch_arena, filepath);

	// create font file and face
	font_state.dwrite_factory->CreateFontFileReference((WCHAR*)wide_filepath.data, 0, &font->file);
	font_state.dwrite_factory->CreateFontFace(DWRITE_FONT_FACE_TYPE_TRUETYPE, 1, &font->file, 0, DWRITE_FONT_SIMULATIONS_NONE, &(font->face));

	str_t font_name = str_get_file_name(filepath);
	printf("[info] successfully opened font: '%.*s'\n", font_name.size, font_name.data);

	return font;
}

function void
font_close(font_t* font) {

	// release dwrite
	if (font->face != nullptr) { font->face->Release(); font->face = nullptr; }
	if (font->file!= nullptr) { font->file->Release(); font->file = nullptr; }

	// push to free stack
	dll_remove(font_state.font_first, font_state.font_last, font);
	stack_push(font_state.font_free, font);
}

function font_metrics_t 
font_get_metrics(font_t* font, f32 size) {

	DWRITE_FONT_METRICS metrics = { 0 };
	font->face->GetMetrics(&metrics);

	f32 pixel_per_em = size * (96.0f / 72.0f); // we assume dpi = 96.0f
	f32 pixel_per_design_unit = pixel_per_em / ((f32)metrics.designUnitsPerEm);

	font_metrics_t result = { 0 };
	result.line_gap = (f32)metrics.lineGap * pixel_per_design_unit;
	result.ascent = (f32)metrics.ascent * pixel_per_design_unit;
	result.descent = (f32)metrics.descent * pixel_per_design_unit;
	result.capital_height = (f32)metrics.capHeight * pixel_per_design_unit;
	result.x_height =  (f32)metrics.xHeight * pixel_per_design_unit;

	return result;
}



// helper

function font_raster_t 
font_glyph_raster(arena_t* arena, font_t* font, f32 size, u32 codepoint) {

	// get font metrics
	DWRITE_FONT_METRICS font_metrics = { 0 };
	font->face->GetMetrics(&font_metrics);
	f32 pixel_per_em = size * (96.0f / 72.0f); // we assume dpi = 96.0f
	f32 pixel_per_design_unit = pixel_per_em / ((f32)font_metrics.designUnitsPerEm);
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
	font_state.gdi_interop->CreateBitmapRenderTarget(0, atlas_dim_x, atlas_dim_y, &render_target);
	HDC dc = render_target->GetMemoryDC();

	// draw glyph
	DWRITE_GLYPH_RUN glyph_run = { 0 };
	glyph_run.fontFace = font->face;
	glyph_run.fontEmSize = size * 96.0f / 72.0f;
	glyph_run.glyphCount = 1;
	glyph_run.glyphIndices = &glyph_index;

	RECT bounding_box = { 0 };
	vec2_t draw_pos = { 1.0f, (f32)atlas_dim_y - descent };
	render_target->DrawGlyphRun(draw_pos.x, draw_pos.y, DWRITE_MEASURING_MODE_GDI_NATURAL, &glyph_run, font_state.rendering_params, RGB(255, 255, 255), &bounding_box);

	// get bitmap
	DIBSECTION dib = { 0 };
	HBITMAP bitmap = (HBITMAP)GetCurrentObject(dc, OBJ_BITMAP);
	GetObject(bitmap, sizeof(dib), &dib);

	// fill raster result
	font_raster_t raster = { 0 };
	raster.size = vec2((f32)atlas_dim_x, (f32)atlas_dim_y);
	raster.advance = (f32)ceilf(advance);
	raster.data = (u8*)arena_alloc(arena, sizeof(u8) * raster.size.x * raster.size.y * 4);

	u8* in_data = (u8*)dib.dsBm.bmBits;
	u32 in_pitch = (u32)dib.dsBm.bmWidthBytes;
	u8* out_data = raster.data;
	u32 out_pitch = (u32)raster.size.x * 4;
	
	u8* in_line = (u8*)in_data;
	u8* out_line = out_data;
	for (u32 y = 0; y < (u32)raster.size.y; y += 1) {
		u8* in_pixel = in_line;
		u8* out_pixel = out_line;
		for (u32 x = 0; x < (u32)raster.size.x; x += 1) {
			//u8 alpha = (in_pixel[0] + in_pixel[1] + in_pixel[2]) / 3;
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


#endif // FONT_DWRITE_CPP