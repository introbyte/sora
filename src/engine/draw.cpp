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

// functions

function void 
draw_init() {

	// allocate arena
	draw_state.batch_arena = arena_create(gigabytes(1));

	// create buffers
	draw_state.instance_buffer = gfx_buffer_create(gfx_buffer_type_vertex, kilobytes(256));
	draw_state.constant_buffer = gfx_buffer_create(gfx_buffer_type_constant, kilobytes(4));

	// assets
	gfx_shader_attribute_t shader_2d_attributes[] = {
		{ "BBOX", 0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "TEX",  0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "PNT",  0, gfx_vertex_format_float2, gfx_vertex_class_per_instance },
		{ "PNT",  1, gfx_vertex_format_float2, gfx_vertex_class_per_instance },
		{ "PNT",  2, gfx_vertex_format_float2, gfx_vertex_class_per_instance },
		{ "PNT",  3, gfx_vertex_format_float2, gfx_vertex_class_per_instance },
		{ "COL",  0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "COL",  1, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "COL",  2, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "COL",  3, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "RAD",  0, gfx_vertex_format_float4, gfx_vertex_class_per_instance },
		{ "STY",  0, gfx_vertex_format_float3, gfx_vertex_class_per_instance },
		{ "SHP",  0, gfx_vertex_format_int,    gfx_vertex_class_per_instance },
		{ "CLP",  0, gfx_vertex_format_int,    gfx_vertex_class_per_instance },
	};
	draw_state.shader = gfx_shader_load(str("res/shaders/shader_2d.hlsl"), shader_2d_attributes, 14);
	draw_state.font = font_open(str("res/fonts/segoe_ui.ttf"));

	draw_state.pipeline = gfx_pipeline_create();
	draw_state.pipeline.depth_mode = gfx_depth_none;
	draw_state.pipeline.topology = gfx_topology_tri_strip;

	// stack defaults
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

	draw_default_init(font, draw_state.font);
	draw_default_init(font_size, 9.0f);

	draw_default_init(clip_mask, rect(0.0f, 0.0f, 4096.0f, 4096.0f));

	draw_default_init(texture, font_state.atlas_texture);

}

function void 
draw_release() {

	// release buffer
	gfx_buffer_release(draw_state.constant_buffer);
	gfx_buffer_release(draw_state.instance_buffer);

	// release assets
	gfx_shader_release(draw_state.shader);
	font_close(draw_state.font);

	// release arena
	arena_release(draw_state.batch_arena);
}

function void 
draw_begin(gfx_renderer_t* renderer) {
	
	// clear batch arena
	arena_clear(draw_state.batch_arena);
	draw_state.batch_first = draw_state.batch_last = nullptr;

	// update pipeline and constant buffer
	rect_t screen = rect(0.0f, 0.0f, (f32)renderer->resolution.x, (f32)renderer->resolution.y);
	draw_state.pipeline.viewport = screen;
	draw_state.pipeline.scissor = screen;
	draw_state.constants.window_size = vec2(screen.x1, screen.y1);

	// reset clip mask
	memset(draw_state.constants.clip_masks, 0, sizeof(rect_t) * 128);
	draw_state.clip_mask_count = 0;

	// reset stacks
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

	draw_stack_reset(clip_mask);

	draw_stack_reset(texture);
	
	draw_push_clip_mask(rect(0.0f, 0.0f, (f32)renderer->resolution.x, (f32)renderer->resolution.y));
	draw_push_texture(font_state.atlas_texture);
}

function void 
draw_end(gfx_renderer_t* renderer) {
	
	// update constant buffer
	gfx_buffer_fill(draw_state.constant_buffer, &draw_state.constants, sizeof(draw_constants_t));

	// set state
	gfx_set_pipeline(draw_state.pipeline);
	gfx_set_shader(draw_state.shader);
	gfx_set_buffer(draw_state.constant_buffer);

	for (draw_batch_t* batch = draw_state.batch_first; batch != 0; batch = batch->next) {

		gfx_set_texture(batch->texture);

		// fill instance buffer
		gfx_buffer_fill(draw_state.instance_buffer, batch->instances, batch->instance_count * sizeof(draw_instance_t));
		gfx_set_buffer(draw_state.instance_buffer, 0, sizeof(draw_instance_t));

		gfx_draw_instanced(4, batch->instance_count);
	}
	
}

function draw_instance_t*
draw_get_instance(gfx_texture_t* texture) {

	// find a batch
	draw_batch_t* batch = nullptr;

	// search batch list
	for (draw_batch_t* b = draw_state.batch_first; b != 0; b = b->next) {

		// if batch has space, and texture matches
		if (((b->instance_count + 1) * sizeof(draw_instance_t)) < (kilobytes(256)) &&
			b->texture == texture) {
			batch = b;
			break;
		}
	}

	// else create one
	if (batch == nullptr) {

		batch = (draw_batch_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_batch_t));
		
		batch->texture = texture;
		batch->instances = (draw_instance_t*)arena_alloc(draw_state.batch_arena, kilobytes(256));
		batch->instance_count = 0;
		
		// add to batch list
		dll_push_back(draw_state.batch_first, draw_state.batch_last, batch);

	}

	// get instance
	draw_instance_t* instance = &batch->instances[batch->instance_count++];
	memset(instance, 0, sizeof(draw_instance_t));

	return instance;
}

function i32
draw_get_clip_mask_index(rect_t rect) {

	// find index if in list
	i32 index = 0;
	for (; index < draw_state.clip_mask_count; index++) {
		if (rect_equals(rect, draw_state.constants.clip_masks[index])) {
			break;
		}
	}

	// we didn't find one, add to list
	if (index == draw_state.clip_mask_count) {
		draw_state.constants.clip_masks[draw_state.clip_mask_count] = rect;
		draw_state.clip_mask_count++;
	}

	return index;
}



function void 
draw_rect(rect_t rect) {

	draw_instance_t* instance = draw_get_instance(draw_top_texture());

	rect_validate(rect);

	instance->bbox = rect;
	instance->tex = {0.0f, 0.0f, 1.0f, 1.0f};
	
	instance->color0 = draw_top_color0().vec;
	instance->color1 = draw_top_color1().vec;
	instance->color2 = draw_top_color2().vec;
	instance->color3 = draw_top_color3().vec;

	instance->radii = draw_top_radii();
	instance->thickness = draw_top_thickness();
	instance->softness = draw_top_softness();
	instance->omit_texture = 1.0f;

	instance->shape = draw_shape_rect;
	instance->clip_index = draw_get_clip_mask_index(draw_top_clip_mask());

	draw_auto_pop_stacks();
}

function void
draw_image(rect_t rect) {

	draw_instance_t* instance = draw_get_instance(draw_top_texture());

	rect_validate(rect);

	instance->bbox = rect;
	instance->tex = { 0.0f, 0.0f, 1.0f, 1.0f };

	instance->color0 = draw_top_color0().vec;
	instance->color1 = draw_top_color1().vec;
	instance->color2 = draw_top_color2().vec;
	instance->color3 = draw_top_color3().vec;

	instance->radii = draw_top_radii();
	instance->thickness = draw_top_thickness();
	instance->softness = draw_top_softness();
	instance->omit_texture = 0.0f;

	instance->shape = draw_shape_rect;
	instance->clip_index = draw_get_clip_mask_index(draw_top_clip_mask());

	draw_auto_pop_stacks();

}

function void
draw_quad(vec2_t p0, vec2_t p1, vec2_t p2, vec2_t p3) {

	// order: (0, 0), (0, 1), (1, 1), (1, 0);

	draw_instance_t* instance = draw_get_instance(draw_top_texture());

	f32 softness = draw_top_softness();

	f32 min_x = min(min(min(p0.x, p1.x), p2.x), p3.x);
	f32 min_y = min(min(min(p0.y, p1.y), p2.y), p3.y);
	f32 max_x = max(max(max(p0.x, p1.x), p2.x), p3.x);
	f32 max_y = max(max(max(p0.y, p1.y), p2.y), p3.y);

	rect_t bbox = rect_grow(rect(min_x, min_y, max_x, max_y), softness);

	vec2_t c = rect_center(bbox);
	vec2_t c_p0 = vec2_sub(p0, c);
	vec2_t c_p1 = vec2_sub(p1, c);
	vec2_t c_p2 = vec2_sub(p2, c);
	vec2_t c_p3 = vec2_sub(p3, c);

	instance->bbox = bbox;

	instance->color0 = draw_top_color0().vec;
	instance->color1 = draw_top_color1().vec;
	instance->color2 = draw_top_color2().vec;
	instance->color3 = draw_top_color3().vec;

	instance->point0 = c_p0;
	instance->point1 = c_p1;
	instance->point2 = c_p2;
	instance->point3 = c_p3;

	instance->thickness = draw_top_thickness();
	instance->softness = softness;
	instance->omit_texture = 1.0f;

	instance->shape = draw_shape_quad;
	instance->clip_index = draw_get_clip_mask_index(draw_top_clip_mask());

	draw_auto_pop_stacks();
}

function void
draw_line(vec2_t p0, vec2_t p1) {

	draw_instance_t* instance = draw_get_instance(draw_top_texture());

	f32 softness = draw_top_softness();

	f32 min_x = min(p0.x, p1.x);
	f32 min_y = min(p0.y, p1.y);
	f32 max_x = max(p0.x, p1.x);
	f32 max_y = max(p0.y, p1.y);

	rect_t bbox = rect_grow(rect(min_x, min_y, max_x, max_y), softness);

	vec2_t c = rect_center(bbox);
	vec2_t c_p0 = vec2_sub(c, p0);
	vec2_t c_p1 = vec2_sub(c, p1);

	instance->bbox = bbox;

	instance->color0 = draw_top_color0().vec;
	instance->color1 = draw_top_color1().vec;

	instance->point0 = c_p0;
	instance->point1 = c_p1;

	instance->thickness = draw_top_thickness();
	instance->softness = softness;
	instance->omit_texture = 1.0f;

	instance->shape = draw_shape_line;
	instance->clip_index = draw_get_clip_mask_index(draw_top_clip_mask());

	draw_auto_pop_stacks();
}

function void 
draw_circle(vec2_t pos, f32 radius, f32 start_angle, f32 end_angle) {
	
	draw_instance_t* instance = draw_get_instance(draw_top_texture());
	
	f32 softness = draw_top_softness();

	instance->bbox = rect_grow(rect(pos.x - radius, pos.y - radius, pos.x + radius, pos.y + radius), softness);

	instance->color0 = draw_top_color0().vec;
	instance->color1 = draw_top_color1().vec;
	instance->color2 = draw_top_color2().vec;
	instance->color3 = draw_top_color3().vec;

	instance->point0 = vec2(radians(start_angle), radians(end_angle));
	
	instance->thickness = draw_top_thickness();
	instance->softness = softness;
	instance->omit_texture = 1.0f;

	instance->shape = draw_shape_circle;
	instance->clip_index = draw_get_clip_mask_index(draw_top_clip_mask());

	draw_auto_pop_stacks();
} 

function void
draw_tri(vec2_t p0, vec2_t p1, vec2_t p2) {

	draw_instance_t* instance = draw_get_instance(draw_top_texture());

	f32 softness = draw_top_softness();

	f32 min_x = min(min(p0.x, p1.x), p2.x);
	f32 min_y = min(min(p0.y, p1.y), p2.y);
	f32 max_x = max(max(p0.x, p1.x), p2.x);
	f32 max_y = max(max(p0.y, p1.y), p2.y);

	rect_t bbox = rect_grow(rect(min_x, min_y, max_x, max_y), softness);

	vec2_t c = rect_center(bbox);
	vec2_t c_p0 = vec2_sub(p0, c);
	vec2_t c_p1 = vec2_sub(p1, c);
	vec2_t c_p2 = vec2_sub(p2, c);

	instance->bbox = bbox;

	instance->color0 = draw_top_color0().vec;
	instance->color1 = draw_top_color1().vec;
	instance->color2 = draw_top_color2().vec;

	instance->point0 = c_p0;
	instance->point1 = c_p1;
	instance->point2 = c_p2;

	instance->radii = draw_top_radii();

	instance->thickness = draw_top_thickness();
	instance->softness = softness;
	instance->omit_texture = 1.0f;

	instance->shape = draw_shape_tri;
	instance->clip_index = draw_get_clip_mask_index(draw_top_clip_mask());

	draw_auto_pop_stacks();
}

function void
draw_text(str_t text, vec2_t pos) {

	f32 font_size = draw_top_font_size();
	font_t* font = draw_top_font();

	for (u32 i = 0; i < text.size; i++) {

		draw_instance_t* instance = draw_get_instance(draw_top_texture());
		
		u8 codepoint = *(text.data + i);
		font_glyph_t* glyph = font_get_glyph(font, font_size, codepoint);

		instance->bbox = rect(pos.x, pos.y, pos.x + glyph->pos.x1, pos.y + glyph->pos.y1);
		instance->tex = glyph->uv;

		instance->color0 = draw_top_color0().vec;
		instance->color1 = draw_top_color1().vec;
		instance->color2 = draw_top_color2().vec;
		instance->color3 = draw_top_color3().vec;

		instance->shape = draw_shape_rect;
		instance->clip_index = draw_get_clip_mask_index(draw_top_clip_mask());

		pos.x += glyph->advance;
	}

	draw_auto_pop_stacks();
}

// stack functions

function void
draw_auto_pop_stacks() {
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

	draw_stack_auto_pop_impl(clip_mask);

	draw_stack_auto_pop_impl(texture);

}

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

draw_stack_impl(clip_mask, rect_t);

draw_stack_impl(texture, gfx_texture_t*);

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

function vec4_t 
draw_top_radii() {
	f32 x = draw_top_radius0();
	f32 y = draw_top_radius1();
	f32 z = draw_top_radius2();
	f32 w = draw_top_radius3();
	return vec4(x, y, z, w);
}

function void
draw_push_radii(f32 radius) {
	draw_push_radius0(radius);
	draw_push_radius1(radius);
	draw_push_radius2(radius);
	draw_push_radius3(radius);
}

function void
draw_push_radii(vec4_t radii) {
	draw_push_radius0(radii.x);
	draw_push_radius1(radii.y);
	draw_push_radius2(radii.z);
	draw_push_radius3(radii.w);
}

function void
draw_set_next_radii(f32 radius) {
	draw_set_next_radius0(radius);
	draw_set_next_radius1(radius);
	draw_set_next_radius2(radius);
	draw_set_next_radius3(radius);
}

function void
draw_set_next_radii(vec4_t radii) {
	draw_set_next_radius0(radii.x);
	draw_set_next_radius1(radii.y);
	draw_set_next_radius2(radii.z);
	draw_set_next_radius3(radii.w);
}

function void
draw_pop_radii() {
	draw_pop_radius0();
	draw_pop_radius1();
	draw_pop_radius2();
	draw_pop_radius3();
}


#endif // DRAW_CPP