// draw.cpp

#ifndef DRAW_CPP
#define DRAW_CPP

// defines
#define draw_default_init(name, value) \
draw_state.name##_default_node.v = value; \

#define draw_stack_reset(name) \
draw_state.name##_stack.top = &draw_state.name##_default_node; draw_state.name##_stack.free = 0; draw_state.name##_stack.auto_pop = 0; \

#define draw_stack_top_impl(name, type) \
function type \
draw_top_##name() { \
	return draw_state.name##_stack.top->v; \
} \

#define draw_stack_push_impl(name, type) \
function type \
draw_push_##name(type v) { \
draw_##name##_node_t* node = draw_state.name##_stack.free; \
if (node != 0) { \
	stack_pop(draw_state.name##_stack.free); \
} else { \
	node = (draw_##name##_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_##name##_node_t)); \
} \
type old_value = draw_state.name##_stack.top->v; \
node->v = v; \
stack_push(draw_state.name##_stack.top, node); \
draw_state.name##_stack.auto_pop = 0; \
return old_value; \
} \

#define draw_stack_pop_impl(name, type) \
function type \
draw_pop_##name() { \
draw_##name##_node_t* popped = draw_state.name##_stack.top; \
if (popped != 0) { \
	stack_pop(draw_state.name##_stack.top); \
	stack_push(draw_state.name##_stack.free, popped); \
	draw_state.name##_stack.auto_pop = 0; \
} \
return popped->v; \
} \

#define draw_stack_set_next_impl(name, type) \
function type \
draw_set_next_##name(type v) { \
draw_##name##_node_t* node = draw_state.name##_stack.free; \
if (node != 0) { \
	stack_pop(draw_state.name##_stack.free); \
} else { \
	node = (draw_##name##_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_##name##_node_t)); \
} \
type old_value = draw_state.name##_stack.top->v; \
node->v = v; \
stack_push(draw_state.name##_stack.top, node); \
draw_state.name##_stack.auto_pop = 1; \
return old_value; \
} \

#define draw_stack_auto_pop_impl(name) \
if (draw_state.name##_stack.auto_pop) { draw_pop_##name(); draw_state.name##_stack.auto_pop = 0; }

#define draw_stack_impl(name, type)\
draw_stack_top_impl(name, type)\
draw_stack_push_impl(name, type)\
draw_stack_pop_impl(name, type)\
draw_stack_set_next_impl(name, type)\

// implementation

function void 
draw_init() {

	// arenas
	draw_state.batch_arena = arena_create(gigabytes(2));
	draw_state.resource_arena = arena_create(gigabytes(2));
	draw_state.scratch_arena = arena_create(megabytes(64));

	// create buffers
	draw_state.vertex_buffer = gfx_buffer_create(gfx_buffer_type_vertex, draw_max_buffer_size, nullptr);

	// create constant buffers
	draw_state.constant_buffers[0] = gfx_buffer_create(gfx_buffer_type_constant, kilobytes(1), nullptr);
	draw_state.constant_buffers[1] = gfx_buffer_create(gfx_buffer_type_constant, kilobytes(1), nullptr);
	draw_state.constant_buffers[2] = gfx_buffer_create(gfx_buffer_type_constant, kilobytes(1), nullptr);
	draw_state.constant_buffers[3] = gfx_buffer_create(gfx_buffer_type_constant, kilobytes(1), nullptr);

	// default assets
	u32 default_texture_data = 0xffffffff;
	draw_state.default_texture = gfx_texture_create(uvec2(1,1),	gfx_texture_format_rgba8, &default_texture_data);
	draw_state.default_shader = gfx_shader_load(str("res/shaders/default.hlsl"), nullptr, 0);
	draw_state.default_pipeline.fill_mode = gfx_fill_solid;
	draw_state.default_pipeline.cull_mode = gfx_cull_mode_back;
	draw_state.default_pipeline.topology = gfx_topology_tris;
	draw_state.default_pipeline.filter_mode = gfx_filter_linear;
	draw_state.default_pipeline.wrap_mode = gfx_wrap_clamp;
	draw_state.default_pipeline.depth_mode = gfx_depth;
	draw_state.default_pipeline.viewport = rect(0.0f, 0.0f, 0.0f, 0.0f);
	draw_state.default_pipeline.scissor = rect(0.0f, 0.0f, 0.0f, 0.0f);

	draw_state.default_font = font_open(str("res/fonts/segoe_ui.ttf"));

	// stack defaults
	draw_default_init(texture, draw_state.default_texture);
	draw_default_init(shader, draw_state.default_shader);
	draw_default_init(pipeline, draw_state.default_pipeline);

	draw_default_init(color0, color(0xffffffff));
	draw_default_init(color1, color(0xffffffff));
	draw_default_init(color2, color(0xffffffff));
	draw_default_init(color3, color(0xffffffff));

	draw_default_init(radius0, 0.0f);
	draw_default_init(radius1, 0.0f);
	draw_default_init(radius2, 0.0f);
	draw_default_init(radius3, 0.0f);

	draw_default_init(thickness, 0.0f);
	draw_default_init(softness, 0.33f);

	draw_default_init(font, draw_state.default_font);
	draw_default_init(font_size, 9.0f);

	draw_state.batch_first = draw_state.batch_last = nullptr;

}

function void 
draw_release() {

	// release buffers
	gfx_buffer_release(draw_state.vertex_buffer);
	gfx_buffer_release(draw_state.constant_buffers[0]);
	gfx_buffer_release(draw_state.constant_buffers[1]);
	gfx_buffer_release(draw_state.constant_buffers[2]);
	gfx_buffer_release(draw_state.constant_buffers[3]);

	// release arena
	arena_release(draw_state.batch_arena);
	arena_release(draw_state.resource_arena);
	arena_release(draw_state.scratch_arena);

}

function void 
draw_update() {
	
	// reset stack
	draw_stack_reset(texture);
	draw_stack_reset(shader);
	draw_stack_reset(pipeline);

	draw_stack_reset(color0);
	draw_stack_reset(color1);
	draw_stack_reset(color2);
	draw_stack_reset(color3);

	draw_stack_reset(radius0);
	draw_stack_reset(radius1);
	draw_stack_reset(radius2);
	draw_stack_reset(radius3);

	draw_stack_reset(thickness);
	draw_stack_reset(softness);

	draw_stack_reset(font);
	draw_stack_reset(font_size);

}

function void 
draw_submit() {
	
	// draw each batch
	for (draw_batch_t* batch = draw_state.batch_first; batch != 0; batch = batch->next) {

		// set pipeline (topology, cull mode, filter, etc.)
		gfx_set_pipeline(batch->state.pipeline_state);

		// set constant buffers
		gfx_set_buffer(draw_state.constant_buffers[0], 0);
		gfx_set_buffer(draw_state.constant_buffers[1], 1);
		gfx_set_buffer(draw_state.constant_buffers[2], 2);
		gfx_set_buffer(draw_state.constant_buffers[3], 3);

		// set shader and textures
		gfx_set_shader(batch->state.shader);
		gfx_set_texture(batch->state.textures, 0);

		// draw
		switch (batch->state.instanced) {
			case false: {
				// fill vertex buffer and draw
				gfx_buffer_fill(draw_state.vertex_buffer, batch->data, batch->state.vertex_size * batch->vertex_count);
				gfx_set_buffer(draw_state.vertex_buffer, 0, batch->state.vertex_size);
				gfx_draw(batch->vertex_count);
				break;
			}
			case true: {
				// fill index buffer and draw
				gfx_buffer_fill(draw_state.vertex_buffer, batch->data, batch->state.instance_size * batch->instance_count);
				gfx_set_buffer(draw_state.vertex_buffer, 0, batch->state.instance_size);
				gfx_draw_instanced(batch->state.instance_vertex_count, batch->instance_count);
				break;
			}
		}

	}

	// clear batches
	arena_clear(draw_state.batch_arena);
	draw_state.batch_first = draw_state.batch_last = nullptr;

	// reset stack
	draw_stack_reset(texture);
	draw_stack_reset(shader);
	draw_stack_reset(pipeline);

	draw_stack_reset(color0);
	draw_stack_reset(color1);
	draw_stack_reset(color2);
	draw_stack_reset(color3);

	draw_stack_reset(radius0);
	draw_stack_reset(radius1);
	draw_stack_reset(radius2);
	draw_stack_reset(radius3);

	draw_stack_reset(thickness);
	draw_stack_reset(softness);

	draw_stack_reset(font);
	draw_stack_reset(font_size);

}

function void 
draw_push_constants(void* data, u32 size, u32 slot) {
	gfx_buffer_fill(draw_state.constant_buffers[slot], data, size);
}

function b8 
draw_batch_state_equals(draw_batch_state_t state_a, draw_batch_state_t state_b) {

	// this looks stupid. but it works better than memcmp
	if ((state_a.shader = state_b.shader) &&
		(state_a.textures = state_b.textures) &&
		(state_a.pipeline_state.fill_mode == state_b.pipeline_state.fill_mode) &&
		(state_a.pipeline_state.cull_mode == state_b.pipeline_state.cull_mode) &&
		(state_a.pipeline_state.topology == state_b.pipeline_state.topology) &&
		(state_a.pipeline_state.filter_mode == state_b.pipeline_state.filter_mode) &&
		(state_a.pipeline_state.wrap_mode == state_b.pipeline_state.wrap_mode) &&
		(state_a.pipeline_state.depth_mode == state_b.pipeline_state.depth_mode) &&
		(state_a.pipeline_state.viewport.x0 == state_b.pipeline_state.viewport.x0) &&
		(state_a.pipeline_state.viewport.x1 == state_b.pipeline_state.viewport.x1) &&
		(state_a.pipeline_state.viewport.y0 == state_b.pipeline_state.viewport.y0) &&
		(state_a.pipeline_state.viewport.y1 == state_b.pipeline_state.viewport.y1) &&
		(state_a.pipeline_state.scissor.x0 == state_b.pipeline_state.scissor.x0) &&
		(state_a.pipeline_state.scissor.x1 == state_b.pipeline_state.scissor.x1) &&
		(state_a.pipeline_state.scissor.y0 == state_b.pipeline_state.scissor.y0) &&
		(state_a.pipeline_state.scissor.y1 == state_b.pipeline_state.scissor.y1) &&
		(state_a.instanced == state_b.instanced) &&
		(state_a.vertex_size == state_b.vertex_size)) {
		return true;
	}
	return false;
}

function draw_batch_t*
draw_batch_find(draw_batch_state_t state, u32 count) {
	
	// try to find batch in batch list
	for (draw_batch_t* batch = draw_state.batch_first; batch != 0; batch = batch->next) {
		b8 batch_equals = draw_batch_state_equals(batch->state, state);
		b8 batch_has_space = ((batch->vertex_count + count) * batch->state.vertex_size) <= draw_max_buffer_size;
		if (batch_equals && batch_has_space) {
			return batch;
		}
	}
	
	// else create one
	draw_batch_t* batch = (draw_batch_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_batch_t));
	
	// fill struct
	batch->state = state;
	batch->vertex_count = 0;
	batch->data = (u8*)arena_alloc(draw_state.batch_arena, draw_max_buffer_size);

	// add to batch list
	dll_push_back(draw_state.batch_first, draw_state.batch_last, batch);

	// return created batch
	return batch;
}

function void
draw_push_vertices(void* vertices, u32 vertex_size, u32 vertex_count) {

	// find batch
	draw_batch_state_t state = { 0 };
	state.textures = draw_top_texture();
	state.shader = draw_top_shader();
	state.pipeline_state = draw_top_pipeline();
	state.instanced = false;
	state.vertex_size = vertex_size;
	draw_batch_t* batch = draw_batch_find(state, vertex_count);

	// copy data
	memcpy(batch->data + (batch->vertex_count * batch->state.vertex_size), vertices, vertex_size * vertex_count);
	batch->vertex_count += vertex_count;

	draw_auto_pop_stacks();
}


// 2d draw

function void 
draw_push_rect(rect_t pos) {

	// find batch
	draw_batch_state_t state = { 0 };
	state.textures = draw_top_texture();
	state.shader = draw_top_shader();
	state.pipeline_state = draw_top_pipeline();
	state.instanced = true;
	state.instance_vertex_count = 4;
	state.instance_size = sizeof(draw_2d_instance_t);
	draw_batch_t* batch = draw_batch_find(state, 1);

	// push data
	draw_2d_instance_t* instance = &((draw_2d_instance_t*)batch->data)[batch->instance_count];

	rect_validate(pos);

	instance->bbox = pos;
	instance->uv = rect(0.0f, 0.0f, 1.0f, 1.0f);
	instance->shape = draw_shape_rect;

	instance->p0 = vec2(0.0f);
	instance->p1 = vec2(0.0f);
	instance->p2 = vec2(0.0f);
	instance->p3 = vec2(0.0f);

	instance->col0 = draw_top_color0().vec;
	instance->col1 = draw_top_color1().vec;
	instance->col2 = draw_top_color2().vec;
	instance->col3 = draw_top_color3().vec;

	instance->r0 = draw_top_radius0();
	instance->r1 = draw_top_radius1();
	instance->r2 = draw_top_radius2();
	instance->r3 = draw_top_radius3();

	instance->thickness = draw_top_thickness();
	instance->softness = draw_top_softness();
	instance->omit_texture = 1.0f;

	batch->instance_count++;
	draw_auto_pop_stacks();
}

function void
draw_push_image(rect_t pos, gfx_texture_t* texture) {

	// find batch
	draw_batch_state_t state = { 0 };
	state.textures = texture;
	state.shader = draw_top_shader();
	state.pipeline_state = draw_top_pipeline();
	state.instanced = true;
	state.instance_vertex_count = 4;
	state.instance_size = sizeof(draw_2d_instance_t);
	draw_batch_t* batch = draw_batch_find(state, 1);

	// push data
	draw_2d_instance_t* instance = &((draw_2d_instance_t*)batch->data)[batch->instance_count];

	rect_validate(pos);

	instance->bbox = pos;
	instance->uv = rect(0.0f, 0.0f, 1.0f, 1.0f);
	instance->shape = draw_shape_rect;

	instance->p0 = vec2(0.0f);
	instance->p1 = vec2(0.0f);
	instance->p2 = vec2(0.0f);
	instance->p3 = vec2(0.0f);

	instance->col0 = draw_top_color0().vec;
	instance->col1 = draw_top_color1().vec;
	instance->col2 = draw_top_color2().vec;
	instance->col3 = draw_top_color3().vec;

	instance->r0 = draw_top_radius0();
	instance->r1 = draw_top_radius1();
	instance->r2 = draw_top_radius2();
	instance->r3 = draw_top_radius3();

	instance->thickness = draw_top_thickness();
	instance->softness = draw_top_softness();
	instance->omit_texture = 0.0f;

	batch->instance_count++;
	draw_auto_pop_stacks();
}

function void 
draw_push_text(str_t text, vec2_t pos) {

	// find batch
	draw_batch_state_t state = { 0 };
	state.textures = draw_top_texture();
	state.shader = draw_top_shader();
	state.pipeline_state = draw_top_pipeline();
	state.instanced = true;
	state.instance_vertex_count = 4;
	state.instance_size = sizeof(draw_2d_instance_t);
	draw_batch_t* batch = draw_batch_find(state, text.size);

	font_t* font = draw_top_font();
	f32 font_size = draw_top_font_size();

	for (u32 i = 0; i < text.size; i++) {

		// get instance
		draw_2d_instance_t* instance = &((draw_2d_instance_t*)batch->data)[batch->instance_count];

		u8 codepoint = *(text.data + i);
		font_glyph_t* glyph = font_get_glyph(font, font_size, codepoint);

		// fill instance
		instance->bbox = { pos.x, pos.y, pos.x + glyph->pos.x1, pos.y + glyph->pos.y1 };
		instance->uv = glyph->uv;
		instance->shape = draw_shape_rect;

		instance->col0 = draw_top_color0().vec;
		instance->col1 = draw_top_color1().vec;
		instance->col2 = draw_top_color2().vec;
		instance->col3 = draw_top_color3().vec;

		instance->r0 = 0.0f;
		instance->r1 = 0.0f;
		instance->r2 = 0.0f;
		instance->r3 = 0.0f;

		instance->thickness = 0.0f;
		instance->softness = 0.0f;
		instance->omit_texture = 0.0f;

		pos.x += glyph->advance;
		batch->instance_count++;
	}

	draw_auto_pop_stacks();
}

function void 
draw_push_circle(vec2_t pos, f32 radius, f32 start_angle, f32 end_angle) {

	// find batch
	draw_batch_state_t state = { 0 };
	state.textures = draw_top_texture();
	state.shader = draw_top_shader();
	state.pipeline_state = draw_top_pipeline();
	state.instanced = true;
	state.instance_vertex_count = 4;
	state.instance_size = sizeof(draw_2d_instance_t);
	draw_batch_t* batch = draw_batch_find(state, 1);

	// push data
	draw_2d_instance_t* instance = &((draw_2d_instance_t*)batch->data)[batch->instance_count];

	instance->bbox = { pos.x - radius, pos.y - radius, pos.x + radius, pos.y + radius };
	instance->uv = rect(0.0f, 0.0f, 1.0f, 1.0f);
	instance->shape = draw_shape_circle;

	instance->p0 = { radians(start_angle), radians(end_angle) };

	instance->col0 = draw_top_color0().vec;
	instance->col1 = draw_top_color1().vec;
	instance->col2 = draw_top_color2().vec;
	instance->col3 = draw_top_color3().vec;

	instance->thickness = draw_top_thickness();
	instance->softness = draw_top_softness();
	instance->omit_texture = 1.0f;

	batch->instance_count++;
	draw_auto_pop_stacks();
}

function void
draw_push_tri(vec2_t p0, vec2_t p1, vec2_t p2) {

	// find batch
	draw_batch_state_t state = { 0 };
	state.textures = draw_top_texture();
	state.shader = draw_top_shader();
	state.pipeline_state = draw_top_pipeline();
	state.instanced = true;
	state.instance_vertex_count = 4;
	state.instance_size = sizeof(draw_2d_instance_t);
	draw_batch_t* batch = draw_batch_find(state, 1);

	// push data
	draw_2d_instance_t* instance = &((draw_2d_instance_t*)batch->data)[batch->instance_count];

	// calculate bounding box
	vec2_t points[3] = { p0, p1, p2 };
	rect_t bbox = rect_bbox(points, 3);
	f32 thickness = draw_top_thickness();
	f32 softness = draw_top_softness();
	bbox = rect_grow(bbox, 5.0f * roundf(thickness + softness));

	vec2_t c = rect_center(bbox);
	vec2_t c_p0 = vec2_sub(p0, c);
	vec2_t c_p1 = vec2_sub(p1, c);
	vec2_t c_p2 = vec2_sub(p2, c);

	instance->bbox = bbox;
	instance->uv = rect(0.0f, 0.0f, 1.0f, 1.0f);
	instance->shape = draw_shape_tri;

	instance->p0 = c_p0;
	instance->p1 = c_p1;
	instance->p2 = c_p2;

	instance->col0 = draw_top_color0().vec;
	instance->col1 = draw_top_color1().vec;
	instance->col2 = draw_top_color2().vec;

	instance->thickness = thickness;
	instance->softness = softness;
	instance->omit_texture = 1.0f;

	batch->instance_count++;
	draw_auto_pop_stacks();
}


// stack functions

function void 
draw_auto_pop_stacks() {
	draw_stack_auto_pop_impl(texture);
	draw_stack_auto_pop_impl(shader);
	draw_stack_auto_pop_impl(pipeline);

	draw_stack_auto_pop_impl(color0);
	draw_stack_auto_pop_impl(color1);
	draw_stack_auto_pop_impl(color2);
	draw_stack_auto_pop_impl(color3);

	draw_stack_auto_pop_impl(radius0);
	draw_stack_auto_pop_impl(radius1);
	draw_stack_auto_pop_impl(radius2);
	draw_stack_auto_pop_impl(radius3);

	draw_stack_auto_pop_impl(thickness);
	draw_stack_auto_pop_impl(softness);

	draw_stack_auto_pop_impl(font);
	draw_stack_auto_pop_impl(font_size);

}

draw_stack_impl(texture, gfx_texture_t*);
draw_stack_impl(shader, gfx_shader_t*);
draw_stack_impl(pipeline, gfx_pipeline_t);

draw_stack_impl(color0, color_t);
draw_stack_impl(color1, color_t);
draw_stack_impl(color2, color_t);
draw_stack_impl(color3, color_t);

draw_stack_impl(radius0, f32);
draw_stack_impl(radius1, f32);
draw_stack_impl(radius2, f32);
draw_stack_impl(radius3, f32);

draw_stack_impl(thickness, f32);
draw_stack_impl(softness, f32);

draw_stack_impl(font, font_t*);
draw_stack_impl(font_size, f32);

function void 
draw_push_color(color_t color) {
	draw_push_color0(color);
	draw_push_color1(color);
	draw_push_color2(color);
	draw_push_color3(color);
}

function void 
draw_set_next_color(color_t color) {
	draw_set_next_color0(color);
	draw_set_next_color1(color);
	draw_set_next_color2(color);
	draw_set_next_color3(color);
}

function void 
draw_pop_color() {
	draw_pop_color0();
	draw_pop_color1();
	draw_pop_color2();
	draw_pop_color3();
}

function void 
draw_push_radius(f32 radius) {
	draw_push_radius0(radius);
	draw_push_radius1(radius);
	draw_push_radius2(radius);
	draw_push_radius3(radius);
}

function void 
draw_set_next_radius(f32 radius) {
	draw_set_next_radius0(radius);
	draw_set_next_radius1(radius);
	draw_set_next_radius2(radius);
	draw_set_next_radius3(radius);
}

function void 
draw_push_radius(vec4_t radii) {
	draw_push_radius0(radii.x);
	draw_push_radius1(radii.y);
	draw_push_radius2(radii.z);
	draw_push_radius3(radii.w);
}

function void 
draw_set_next_radius(vec4_t radii) {
	draw_set_next_radius0(radii.x);
	draw_set_next_radius1(radii.y);
	draw_set_next_radius2(radii.z);
	draw_set_next_radius3(radii.w);
}

function void 
draw_pop_radius() {
	draw_pop_radius0();
	draw_pop_radius1();
	draw_pop_radius2();
	draw_pop_radius3();
}


#endif // DRAW_CPP