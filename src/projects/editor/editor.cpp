// editor.cpp

#ifndef EDITOR_CPP
#define EDITOR_CPP

// implementation

// editor state

function void
editor_init() {

	// allocate arenas
	editor_state.arena = arena_create(gigabytes(1));
	editor_state.scratch = arena_create(gigabytes(1));
	editor_state.batch_arena = arena_create(gigabytes(1));

	// load resources
	gfx_shader_attribute_t attributes[] = {
		{"POS", 0, gfx_vertex_format_float2, gfx_vertex_class_per_instance},
		{"POS", 1, gfx_vertex_format_float2, gfx_vertex_class_per_instance},
		{"POS", 2, gfx_vertex_format_float2, gfx_vertex_class_per_instance},
		{"POS", 3, gfx_vertex_format_float2, gfx_vertex_class_per_instance},
		{"UV", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance},
		{"COL", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance},
		{"COL", 1, gfx_vertex_format_float4, gfx_vertex_class_per_instance},
		{"COL", 2, gfx_vertex_format_float4, gfx_vertex_class_per_instance},
		{"COL", 3, gfx_vertex_format_float4, gfx_vertex_class_per_instance},
		{"TEX", 0, gfx_vertex_format_int, gfx_vertex_class_per_instance},
	};

	editor_state.shader = gfx_shader_load(str("res/shaders/editor_2d.hlsl"), attributes, 10);
	editor_state.font = font_open(str("res/fonts/consola.ttf"));

	// create buffers
	editor_state.instance_buffer = gfx_buffer_create(gfx_buffer_type_vertex, megabytes(8));
	editor_state.constant_buffer = gfx_buffer_create(gfx_buffer_type_constant, kilobytes(1));

	// create space
	editor_state.space = space_create();
}

function void
editor_release() {

	// release arenas
	arena_release(editor_state.arena);
	arena_release(editor_state.scratch);
	arena_release(editor_state.batch_arena);

	space_release(editor_state.space);
}

function void
editor_update() {

	arena_clear(editor_state.scratch);
	

}

function void
editor_render(gfx_renderer_t* renderer) {
	

	// render space
	space_t* space = editor_state.space;
	vec2_t pos = vec2(5.0f, 50.0f);
	u32 token_count = 0;
	for (line_t* line = space->first; line != 0; line = line->next) {

		for (token_t* token = line->first; token != 0; token = token->next) {

			str_t token_string = str((char*)token->label, token->label_size);
			f32 token_string_width = font_text_get_width(editor_state.font, 12.0f, token_string);

			editor_push_text(token_string, pos, color(1.0f, 1.0f, 1.0f, 1.0f));

			pos.x += token_string_width + 12.0f;
			token_count++;
		}

		pos.y += 20.0f;
	}



	// render stats
	str_t line_count_label = str_format(editor_state.scratch, "line_count: %u", space->count);
	editor_push_text(line_count_label, vec2(5.0f, 5.0f), color(1.0f, 1.0f, 1.0f, 1.0f));
	
	str_t token_count_label = str_format(editor_state.scratch, "token_count: %u", token_count);
	editor_push_text(token_count_label, vec2(5.0f, 25.0f), color(1.0f, 1.0f, 1.0f, 1.0f));

	// update constant buffer
	editor_state.constants.window_size = vec2((f32)renderer->resolution.x, (f32)renderer->resolution.y);
	gfx_buffer_fill(editor_state.constant_buffer, &editor_state.constants, sizeof(editor_constants_t));

	gfx_pipeline_t pipeline = gfx_pipeline_create();
	pipeline.topology = gfx_topology_tri_strip;
	pipeline.depth_mode = gfx_depth_none;
	gfx_set_pipeline(pipeline);
	gfx_set_texture(font_state.atlas_texture);
	gfx_set_shader(editor_state.shader);
	gfx_set_buffer(editor_state.constant_buffer);

	for (editor_batch_t* batch = editor_state.batch_first; batch != nullptr; batch = batch->next) {

		// load instance buffer
		gfx_buffer_fill(editor_state.instance_buffer, batch->data, batch->instance_count * batch->instance_size);
		gfx_set_buffer(editor_state.instance_buffer, 0, batch->instance_size);

		gfx_draw_instanced(4, batch->instance_count);
		
	}

	// clear batches
	arena_clear(editor_state.batch_arena);
	editor_state.batch_first = nullptr;
	editor_state.batch_last = nullptr;

}


function editor_batch_t* 
editor_get_batch(u32 count = 1) {

	editor_batch_t* batch = nullptr;

	for (editor_batch_t* b = editor_state.batch_first; b != 0; b = b->next) {
		b8 batch_has_space = ((b->instance_count + count) * sizeof(editor_instance_t)) <= megabytes(8);
		if (batch_has_space) {
			batch = b;
			break;
		}
	}

	if (batch == nullptr) {

		batch = (editor_batch_t*)arena_alloc(editor_state.batch_arena, sizeof(editor_batch_t));
		
		batch->instance_size = sizeof(editor_instance_t);
		batch->instance_count = 0;
		batch->data = (u8*)arena_alloc(editor_state.batch_arena, megabytes(8));

		dll_push_back(editor_state.batch_first, editor_state.batch_last, batch);
	}

	return batch;
}

function void 
editor_push_quad(rect_t rect, color_t color) {
	editor_push_quad(vec2(rect.x1, rect.y0), vec2(rect.x1, rect.y1), vec2(rect.x0, rect.y1), vec2(rect.x0, rect.y0), color.vec, color.vec, color.vec, color.vec);
}

function void 
editor_push_quad(vec2_t p0, vec2_t p1, vec2_t p2, vec2_t p3, vec4_t c0, vec4_t c1, vec4_t c2, vec4_t c3) {

	editor_batch_t* batch = editor_get_batch();
	editor_instance_t* instance = &((editor_instance_t*)batch->data)[batch->instance_count++];
	
	instance->p0 = p0;
	instance->p1 = p1;
	instance->p2 = p2;
	instance->p3 = p3;
	instance->uv = rect(0.0f, 0.0f, 0.0f, 0.0f);
	instance->color0 = c0;
	instance->color1 = c1;
	instance->color2 = c2;
	instance->color3 = c3;
	instance->omit_texture = 1;

}

function void 
editor_push_text(str_t text, vec2_t pos, color_t color) {

	editor_batch_t* batch = editor_get_batch(text.size);

	for (u32 i = 0; i < text.size; i++) {
		editor_instance_t* instance = &((editor_instance_t*)batch->data)[batch->instance_count++];

		u8 codepoint = *(text.data + i);
		font_glyph_t* glyph = font_get_glyph(editor_state.font, 12.0f, codepoint);

		instance->p0 = vec2(pos.x, pos.y + glyph->pos.y1);
		instance->p1 = vec2(pos.x + glyph->pos.x1, pos.y + glyph->pos.y1);
		instance->p2 = vec2(pos.x, pos.y);
		instance->p3 = vec2(pos.x + glyph->pos.x1, pos.y);
		instance->uv = glyph->uv;
		instance->color0 = color.vec;
		instance->color1 = color.vec;
		instance->color2 = color.vec;
		instance->color3 = color.vec;
		instance->omit_texture = 0;

		pos.x += glyph->advance;
	}

}

#endif // EDITOR_CPP