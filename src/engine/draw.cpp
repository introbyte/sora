// draw.cpp

#ifndef DRAW_CPP
#define DRAW_CPP

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
		{ "STY",  0, gfx_vertex_format_float2, gfx_vertex_class_per_instance },
		{ "IDX",  0, gfx_vertex_format_uint,   gfx_vertex_class_per_instance },
	};
	draw_state.shader = gfx_shader_load(str("res/shaders/shader_2d.hlsl"), shader_2d_attributes, array_count(shader_2d_attributes));
	draw_state.font = font_open(str("res/fonts/segoe_ui.ttf"));
    
	u32 data = 0xffffffff;
	draw_state.texture = gfx_texture_create(uvec2(1, 1), gfx_texture_format_rgba8, &data);
    
	draw_state.pipeline = gfx_pipeline_create();
	draw_state.pipeline.depth_mode = gfx_depth_none;
	draw_state.pipeline.topology = gfx_topology_tri_strip;
    
	// stack defaults
	draw_state.color0_default_node.v = color(0xffffffff);
	draw_state.color1_default_node.v = color(0xffffffff);
	draw_state.color2_default_node.v = color(0xffffffff);
	draw_state.color3_default_node.v = color(0xffffffff);
    
	draw_state.radius0_default_node.v = 0.0f;
	draw_state.radius1_default_node.v = 0.0f;
	draw_state.radius2_default_node.v = 0.0f;
	draw_state.radius3_default_node.v = 0.0f;
    
	draw_state.thickness_default_node.v = 0.0f;
	draw_state.softness_default_node.v = 0.33f;
    
	draw_state.font_default_node.v = draw_state.font;
	draw_state.font_size_default_node.v = 9.0f;
    
	draw_state.clip_mask_default_node.v = rect(0.0f, 0.0f, 4096.0f, 4096.0f);
    
	draw_state.texture_default_node.v = font_state.atlas_texture;
    
}

function void 
draw_release() {
    
	// release buffer
	gfx_buffer_release(draw_state.constant_buffer);
	gfx_buffer_release(draw_state.instance_buffer);
    
	// release assets
	gfx_texture_release(draw_state.texture);
	gfx_shader_release(draw_state.shader);
	font_close(draw_state.font);
    
	// release arena
	arena_release(draw_state.batch_arena);
}

function void 
draw_begin(gfx_handle_t renderer) {
	
    prof_scope("draw_begin") {
        
        
        // clear batch arena
        arena_clear(draw_state.batch_arena);
        draw_state.batch_first = nullptr;
        draw_state.batch_last = nullptr;
        
        // update pipeline and constant buffer
        uvec2_t renderer_size = gfx_renderer_get_size(renderer);
        rect_t screen = rect(0.0f, 0.0f, (f32)renderer_size.x, (f32)renderer_size.y);
        draw_state.pipeline.viewport = screen;
        draw_state.pipeline.scissor = screen;
        draw_state.constants.window_size = vec2(screen.x1, screen.y1);
        
        // reset texture list
        memset(draw_state.texture_list, 0, sizeof(gfx_handle_t) * draw_max_textures);
        draw_state.texture_count = 0;
        
        // reset clip mask
        memset(draw_state.constants.clip_masks, 0, sizeof(rect_t) * 128);
        draw_state.clip_mask_count = 0;
        
        // reset stacks
        draw_state.color0_stack.top = &draw_state.color0_default_node; draw_state.color0_stack.free = 0; draw_state.color0_stack.auto_pop = 0;
        draw_state.color1_stack.top = &draw_state.color1_default_node; draw_state.color1_stack.free = 0; draw_state.color1_stack.auto_pop = 0;
        draw_state.color2_stack.top = &draw_state.color2_default_node; draw_state.color2_stack.free = 0; draw_state.color2_stack.auto_pop = 0;
        draw_state.color3_stack.top = &draw_state.color3_default_node; draw_state.color3_stack.free = 0; draw_state.color3_stack.auto_pop = 0;
        
        draw_state.radius0_stack.top = &draw_state.radius0_default_node; draw_state.radius0_stack.free = 0; draw_state.radius0_stack.auto_pop = 0;
        draw_state.radius1_stack.top = &draw_state.radius1_default_node; draw_state.radius1_stack.free = 0; draw_state.radius1_stack.auto_pop = 0;
        draw_state.radius2_stack.top = &draw_state.radius2_default_node; draw_state.radius2_stack.free = 0; draw_state.radius2_stack.auto_pop = 0;
        draw_state.radius3_stack.top = &draw_state.radius3_default_node; draw_state.radius3_stack.free = 0; draw_state.radius3_stack.auto_pop = 0;
        
        draw_state.thickness_stack.top = &draw_state.thickness_default_node; draw_state.thickness_stack.free = 0; draw_state.thickness_stack.auto_pop = 0;
        draw_state.softness_stack.top = &draw_state.softness_default_node; draw_state.softness_stack.free = 0; draw_state.softness_stack.auto_pop = 0;
        draw_state.font_stack.top = &draw_state.font_default_node; draw_state.font_stack.free = 0; draw_state.font_stack.auto_pop = 0;
        draw_state.font_size_stack.top = &draw_state.font_size_default_node; draw_state.font_size_stack.free = 0; draw_state.font_size_stack.auto_pop = 0;
        
        draw_state.clip_mask_stack.top = &draw_state.clip_mask_default_node; draw_state.clip_mask_stack.free = 0; draw_state.clip_mask_stack.auto_pop = 0;
        
        draw_state.texture_stack.top = &draw_state.texture_default_node; draw_state.texture_stack.free = 0; draw_state.texture_stack.auto_pop = 0;
        
        // push default clip mask and texture
        draw_push_clip_mask(rect(0.0f, 0.0f, (f32)renderer_size.x, (f32)renderer_size.y));
        draw_push_texture(draw_state.texture);
        
        
    }
}

function void 
draw_end(gfx_handle_t renderer) {
	prof_scope("draw_end") {
        
        // update constant buffer
        gfx_buffer_fill(draw_state.constant_buffer, &draw_state.constants, sizeof(draw_constants_t));
        
        // set state
        gfx_set_pipeline(draw_state.pipeline);
        gfx_set_shader(draw_state.shader);
        gfx_set_buffer(draw_state.constant_buffer);
        gfx_set_texture_array(draw_state.texture_list, draw_state.texture_count, 0, gfx_texture_usage_ps);
        
        for (draw_batch_t* batch = draw_state.batch_first; batch != 0; batch = batch->next) {
            
            // fill instance buffer
            gfx_buffer_fill(draw_state.instance_buffer, batch->instances, batch->instance_count * sizeof(draw_instance_t));
            gfx_set_buffer(draw_state.instance_buffer, 0, sizeof(draw_instance_t));
            
            gfx_draw_instanced(4, batch->instance_count);
        }
        
    }
}


function draw_instance_t*
draw_get_instance() {
    
	// find a batch
	draw_batch_t* batch = nullptr;
    
	// search batch list
	for (draw_batch_t* b = draw_state.batch_first; b != 0; b = b->next) {
        
		// if batch has space, and texture matches
		if (((b->instance_count + 1) * sizeof(draw_instance_t)) < (kilobytes(256))) {
			batch = b;
			break;
		}
	}
    
	// else create one
	if (batch == nullptr) {
        
		batch = (draw_batch_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_batch_t));
		
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
draw_get_texture_index(gfx_handle_t texture) {
    
	// find index if in list
	i32 index = 0;
	for (; index < draw_state.texture_count; index++) {
		if (gfx_handle_equals(texture, draw_state.texture_list[index])) {
			break;
		}
	}
    
	// we didn't find one, add to list
	if (index == draw_state.texture_count) {
		draw_state.texture_list[draw_state.texture_count] = texture;
		draw_state.texture_count++;
	}
    
	return index;
    
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

inlnfunc u32
draw_pack_indices(u32 shape, u32 texture, u32 clip) {
	return (shape << 24) | (texture << 16) | (clip << 8);
}



function void 
draw_rect(rect_t rect) {
    
	draw_instance_t* instance = draw_get_instance();
    
	rect_validate(rect);
    
	instance->bbox = rect;
	instance->tex = {0.0f, 0.0f, 1.0f, 1.0f};
	
	instance->color0 = draw_top_color0().vec;
	instance->color1 = draw_top_color1().vec;
	instance->color2 = draw_top_color2().vec;
	instance->color3 = draw_top_color3().vec;
    
	instance->radii = draw_top_rounding();
	instance->thickness = draw_top_thickness();
	instance->softness = draw_top_softness();
    
	instance->indices = draw_pack_indices(
                                          draw_shape_rect,
                                          draw_get_texture_index(draw_top_texture()),
                                          draw_get_clip_mask_index(draw_top_clip_mask())
                                          );
    
	draw_auto_pop_stacks();
}

function void
draw_image(rect_t rect) {
    
	draw_instance_t* instance = draw_get_instance();
    
	rect_validate(rect);
    
	instance->bbox = rect;
	instance->tex = { 0.0f, 0.0f, 1.0f, 1.0f };
    
	instance->color0 = draw_top_color0().vec;
	instance->color1 = draw_top_color1().vec;
	instance->color2 = draw_top_color2().vec;
	instance->color3 = draw_top_color3().vec;
    
	instance->radii = draw_top_rounding();
	instance->thickness = draw_top_thickness();
	instance->softness = draw_top_softness();
    
	instance->indices = draw_pack_indices(
                                          draw_shape_rect,
                                          draw_get_texture_index(draw_top_texture()),
                                          draw_get_clip_mask_index(draw_top_clip_mask())
                                          );
    
	draw_auto_pop_stacks();
    
}

function void
draw_quad(vec2_t p0, vec2_t p1, vec2_t p2, vec2_t p3) {
    
	// order: (0, 0), (0, 1), (1, 1), (1, 0);
    
	draw_instance_t* instance = draw_get_instance();
    
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
    
	instance->indices = draw_pack_indices(
                                          draw_shape_quad,
                                          draw_get_texture_index(draw_top_texture()),
                                          draw_get_clip_mask_index(draw_top_clip_mask())
                                          );
    
	draw_auto_pop_stacks();
}

function void
draw_line(vec2_t p0, vec2_t p1) {
    
	draw_instance_t* instance = draw_get_instance();
    
	f32 thickness = draw_top_thickness();
	f32 softness = draw_top_softness();
    
	f32 min_x = min(p0.x, p1.x);
	f32 min_y = min(p0.y, p1.y);
	f32 max_x = max(p0.x, p1.x);
	f32 max_y = max(p0.y, p1.y);
    
	rect_t bbox = rect_grow(rect(min_x, min_y, max_x, max_y), softness + thickness + 2.0f);
    
	vec2_t c = rect_center(bbox);
	vec2_t c_p0 = vec2_sub(c, p0);
	vec2_t c_p1 = vec2_sub(c, p1);
    
	instance->bbox = bbox;
    
	instance->color0 = draw_top_color0().vec;
	instance->color1 = draw_top_color1().vec;
    
	instance->point0 = c_p0;
	instance->point1 = c_p1;
    
	instance->thickness = thickness;
	instance->softness = softness;
    
	instance->indices = draw_pack_indices(
                                          draw_shape_line,
                                          draw_get_texture_index(draw_top_texture()),
                                          draw_get_clip_mask_index(draw_top_clip_mask())
                                          );
    
	draw_auto_pop_stacks();
}

function void 
draw_circle(vec2_t pos, f32 radius, f32 start_angle, f32 end_angle) {
	
	draw_instance_t* instance = draw_get_instance();
	
	f32 softness = draw_top_softness();
    
	instance->bbox = rect_grow(rect(pos.x - radius, pos.y - radius, pos.x + radius, pos.y + radius), softness);
    
	instance->color0 = draw_top_color0().vec;
	instance->color1 = draw_top_color1().vec;
	instance->color2 = draw_top_color2().vec;
	instance->color3 = draw_top_color3().vec;
    
	instance->point0 = vec2(radians(start_angle), radians(end_angle));
	
	instance->thickness = draw_top_thickness();
	instance->softness = softness;
    
	instance->indices = draw_pack_indices(
                                          draw_shape_circle,
                                          draw_get_texture_index(draw_top_texture()),
                                          draw_get_clip_mask_index(draw_top_clip_mask())
                                          );
    
    
	draw_auto_pop_stacks();
} 

function void
draw_tri(vec2_t p0, vec2_t p1, vec2_t p2) {
    
	draw_instance_t* instance = draw_get_instance();
    
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
    
	instance->radii = draw_top_rounding();
    
	instance->thickness = draw_top_thickness();
	instance->softness = softness;
    
	instance->indices = draw_pack_indices(
                                          draw_shape_tri,
                                          draw_get_texture_index(draw_top_texture()),
                                          draw_get_clip_mask_index(draw_top_clip_mask())
                                          );
    
    
	draw_auto_pop_stacks();
}

function void
draw_text(str_t text, vec2_t pos) {
    
	f32 font_size = draw_top_font_size();
	font_handle_t font = draw_top_font();
    
	for (u32 i = 0; i < text.size; i++) {
        
		draw_instance_t* instance = draw_get_instance();
		
		u8 codepoint = *(text.data + i);
		font_glyph_t* glyph = font_get_glyph(font, font_size, codepoint);
        
		instance->bbox = rect(pos.x, pos.y, pos.x + glyph->pos.x1, pos.y + glyph->pos.y1);
		instance->tex = glyph->uv;
        
		instance->color0 = draw_top_color0().vec;
		instance->color1 = draw_top_color1().vec;
		instance->color2 = draw_top_color2().vec;
		instance->color3 = draw_top_color3().vec;
        
		instance->indices = draw_pack_indices(
                                              draw_shape_rect,
                                              draw_get_texture_index(font_state.atlas_texture),
                                              draw_get_clip_mask_index(draw_top_clip_mask())
                                              );
        
		pos.x += glyph->advance;
	}
    
	draw_auto_pop_stacks();
}

function void
draw_bezier(vec2_t p0, vec2_t p1, vec2_t c0, vec2_t c1) {
    
	const i32 count = 32;
	vec2_t prev_point = p0;
    
	for (i32 i = 1; i < count; i++) {
        
		f32 t = (f32)i / (f32)count;
        
		vec2_t v0 = vec2_lerp(p0, c0, t);
		vec2_t v1 = vec2_lerp(c0, c1, t);
		vec2_t v2 = vec2_lerp(c1, p1, t);
        
		vec2_t v3 = vec2_lerp(v0, v1, t);
		vec2_t v4 = vec2_lerp(v1, v2, t);
        
		vec2_t curve_point = vec2_lerp(v3, v4, t);
        
		draw_line(prev_point, curve_point);
        
		prev_point = curve_point;
	}
    
}

// stack functions

function void
draw_auto_pop_stacks() {
    
    if (draw_state.color0_stack.auto_pop) { draw_pop_color0(); draw_state.color0_stack.auto_pop = false; }
    if (draw_state.color1_stack.auto_pop) { draw_pop_color1(); draw_state.color1_stack.auto_pop = false; }
    if (draw_state.color2_stack.auto_pop) { draw_pop_color2(); draw_state.color2_stack.auto_pop = false; }
    if (draw_state.color3_stack.auto_pop) { draw_pop_color3(); draw_state.color3_stack.auto_pop = false; }
    if (draw_state.radius0_stack.auto_pop) { draw_pop_radius0(); draw_state.radius0_stack.auto_pop = false; }
    if (draw_state.radius1_stack.auto_pop) { draw_pop_radius1(); draw_state.radius1_stack.auto_pop = false; }
    if (draw_state.radius2_stack.auto_pop) { draw_pop_radius2(); draw_state.radius2_stack.auto_pop = false; }
    if (draw_state.radius3_stack.auto_pop) { draw_pop_radius3(); draw_state.radius3_stack.auto_pop = false; }
    if (draw_state.thickness_stack.auto_pop) { draw_pop_thickness(); draw_state.thickness_stack.auto_pop = false; }
    if (draw_state.softness_stack.auto_pop) { draw_pop_softness(); draw_state.softness_stack.auto_pop = false; }
    if (draw_state.font_stack.auto_pop) { draw_pop_font(); draw_state.font_stack.auto_pop = false; }
    if (draw_state.font_size_stack.auto_pop) { draw_pop_font_size(); draw_state.font_size_stack.auto_pop = false; }
    if (draw_state.clip_mask_stack.auto_pop) { draw_pop_clip_mask(); draw_state.clip_mask_stack.auto_pop = false; }
    if (draw_state.texture_stack.auto_pop) { draw_pop_texture(); draw_state.texture_stack.auto_pop = false; }
    
}

// color0
function color_t 
draw_top_color0() {
	return draw_state.color0_stack.top->v;
}

function color_t 
draw_push_color0(color_t v) {
	draw_color0_node_t* node = draw_state.color0_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.color0_stack.free);
	} else {
		node = (draw_color0_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_color0_node_t));
	} 
	color_t old_value = draw_state.color0_stack.top->v; 
	node->v = v; stack_push(draw_state.color0_stack.top, node);
	draw_state.color0_stack.auto_pop = false; 
	return old_value;
}

function color_t 
draw_pop_color0() {
	draw_color0_node_t* popped = draw_state.color0_stack.top; 
    color_t result = { 0 };
	if (popped != 0) {
        result = popped->v;
		stack_pop(draw_state.color0_stack.top); 
		stack_push(draw_state.color0_stack.free, popped);
		draw_state.color0_stack.auto_pop = false;
	} 
	return result;
}

function color_t 
draw_set_next_color0(color_t v) {
	draw_color0_node_t* node = draw_state.color0_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.color0_stack.free);
	} else {
		node = (draw_color0_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_color0_node_t));
	} 
	color_t old_value = draw_state.color0_stack.top->v;
	node->v = v; 
	stack_push(draw_state.color0_stack.top, node);
	draw_state.color0_stack.auto_pop = true; 
	return old_value;
};


// color1
function color_t 
draw_top_color1() {
	return draw_state.color1_stack.top->v;
}

function color_t 
draw_push_color1(color_t v) {
	draw_color1_node_t* node = draw_state.color1_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.color1_stack.free);
	} else {
		node = (draw_color1_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_color1_node_t));
	} 
	color_t old_value = draw_state.color1_stack.top->v; 
	node->v = v; stack_push(draw_state.color1_stack.top, node);
	draw_state.color1_stack.auto_pop = false; 
	return old_value;
}

function color_t 
draw_pop_color1() {
	draw_color1_node_t* popped = draw_state.color1_stack.top; 
	color_t result = { 0 };
	if (popped != 0) {
        result = popped->v;
		stack_pop(draw_state.color1_stack.top); 
		stack_push(draw_state.color1_stack.free, popped);
		draw_state.color1_stack.auto_pop = false;
	} 
	return result;
}

function color_t 
draw_set_next_color1(color_t v) {
	draw_color1_node_t* node = draw_state.color1_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.color1_stack.free);
	} else {
		node = (draw_color1_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_color1_node_t));
	} 
	color_t old_value = draw_state.color1_stack.top->v;
	node->v = v; 
	stack_push(draw_state.color1_stack.top, node);
	draw_state.color1_stack.auto_pop = true; 
	return old_value;
};

// color2

function color_t 
draw_top_color2() {
	return draw_state.color2_stack.top->v;
}

function color_t 
draw_push_color2(color_t v) {
	draw_color2_node_t* node = draw_state.color2_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.color2_stack.free);
	} else {
		node = (draw_color2_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_color2_node_t));
	} 
	color_t old_value = draw_state.color2_stack.top->v; 
	node->v = v; stack_push(draw_state.color2_stack.top, node);
	draw_state.color2_stack.auto_pop = false; 
	return old_value;
}

function color_t 
draw_pop_color2() {
	draw_color2_node_t* popped = draw_state.color2_stack.top; 
	color_t result = { 0 };
	if (popped != 0) {
        result = popped->v;
		stack_pop(draw_state.color2_stack.top); 
		stack_push(draw_state.color2_stack.free, popped);
		draw_state.color2_stack.auto_pop = false;
	} 
	return result;
}

function color_t 
draw_set_next_color2(color_t v) {
	draw_color2_node_t* node = draw_state.color2_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.color2_stack.free);
	} else {
		node = (draw_color2_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_color2_node_t));
	} 
	color_t old_value = draw_state.color2_stack.top->v;
	node->v = v; 
	stack_push(draw_state.color2_stack.top, node);
	draw_state.color2_stack.auto_pop = true; 
	return old_value;
};

// color3
function color_t 
draw_top_color3() {
	return draw_state.color3_stack.top->v;
}

function color_t 
draw_push_color3(color_t v) {
	draw_color3_node_t* node = draw_state.color3_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.color3_stack.free);
	} else {
		node = (draw_color3_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_color3_node_t));
	} 
	color_t old_value = draw_state.color3_stack.top->v; 
	node->v = v; stack_push(draw_state.color3_stack.top, node);
	draw_state.color3_stack.auto_pop = false; 
	return old_value;
}

function color_t 
draw_pop_color3() {
	draw_color3_node_t* popped = draw_state.color3_stack.top; 
	color_t result = { 0 };
	if (popped != 0) {
        result = popped->v;
		stack_pop(draw_state.color3_stack.top); 
		stack_push(draw_state.color3_stack.free, popped);
		draw_state.color3_stack.auto_pop = false;
	} 
	return result;
}

function color_t 
draw_set_next_color3(color_t v) {
	draw_color3_node_t* node = draw_state.color3_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.color3_stack.free);
	} else {
		node = (draw_color3_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_color3_node_t));
	} 
	color_t old_value = draw_state.color3_stack.top->v;
	node->v = v; 
	stack_push(draw_state.color3_stack.top, node);
	draw_state.color3_stack.auto_pop = true; 
	return old_value;
};

// radius0

function f32 
draw_top_radius0() {
	return draw_state.radius0_stack.top->v;
}

function f32 
draw_push_radius0(f32 v) {
	draw_radius0_node_t* node = draw_state.radius0_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.radius0_stack.free);
	} else {
		node = (draw_radius0_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_radius0_node_t));
	} 
	f32 old_value = draw_state.radius0_stack.top->v; 
	node->v = v; stack_push(draw_state.radius0_stack.top, node);
	draw_state.radius0_stack.auto_pop = false; 
	return old_value;
}

function f32 
draw_pop_radius0() {
	draw_radius0_node_t* popped = draw_state.radius0_stack.top; 
    f32 result = { 0 };
	if (popped != 0) {
        result = popped->v;
		stack_pop(draw_state.radius0_stack.top); 
		stack_push(draw_state.radius0_stack.free, popped);
		draw_state.radius0_stack.auto_pop = false;
	} 
	return result;
}

function f32 
draw_set_next_radius0(f32 v) {
	draw_radius0_node_t* node = draw_state.radius0_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.radius0_stack.free);
	} else {
		node = (draw_radius0_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_radius0_node_t));
	} 
	f32 old_value = draw_state.radius0_stack.top->v;
	node->v = v; 
	stack_push(draw_state.radius0_stack.top, node);
	draw_state.radius0_stack.auto_pop = true; 
	return old_value;
};


// radius1

function f32 
draw_top_radius1() {
	return draw_state.radius1_stack.top->v;
}

function f32 
draw_push_radius1(f32 v) {
	draw_radius1_node_t* node = draw_state.radius1_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.radius1_stack.free);
	} else {
		node = (draw_radius1_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_radius1_node_t));
	} 
	f32 old_value = draw_state.radius1_stack.top->v; 
	node->v = v; stack_push(draw_state.radius1_stack.top, node);
	draw_state.radius1_stack.auto_pop = false; 
	return old_value;
}

function f32 
draw_pop_radius1() {
	draw_radius1_node_t* popped = draw_state.radius1_stack.top; 
    f32 result = 0.0f;
	if (popped != 0) {
        result = popped->v;
		stack_pop(draw_state.radius1_stack.top); 
		stack_push(draw_state.radius1_stack.free, popped);
		draw_state.radius1_stack.auto_pop = false;
	} 
	return result;
}

function f32 
draw_set_next_radius1(f32 v) {
	draw_radius1_node_t* node = draw_state.radius1_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.radius1_stack.free);
	} else {
		node = (draw_radius1_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_radius1_node_t));
	} 
	f32 old_value = draw_state.radius1_stack.top->v;
	node->v = v; 
	stack_push(draw_state.radius1_stack.top, node);
	draw_state.radius1_stack.auto_pop = true; 
	return old_value;
};


// radius2

function f32 
draw_top_radius2() {
	return draw_state.radius2_stack.top->v;
}

function f32 
draw_push_radius2(f32 v) {
	draw_radius2_node_t* node = draw_state.radius2_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.radius2_stack.free);
	} else {
		node = (draw_radius2_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_radius2_node_t));
	} 
	f32 old_value = draw_state.radius2_stack.top->v; 
	node->v = v; stack_push(draw_state.radius2_stack.top, node);
	draw_state.radius2_stack.auto_pop = false; 
	return old_value;
}

function f32 
draw_pop_radius2() {
	draw_radius2_node_t* popped = draw_state.radius2_stack.top; 
    f32 result = 0.0f;
	if (popped != 0) {
        result = popped->v;
		stack_pop(draw_state.radius2_stack.top); 
		stack_push(draw_state.radius2_stack.free, popped);
		draw_state.radius2_stack.auto_pop = false;
	} 
	return result;
}

function f32 
draw_set_next_radius2(f32 v) {
	draw_radius2_node_t* node = draw_state.radius2_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.radius2_stack.free);
	} else {
		node = (draw_radius2_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_radius2_node_t));
	} 
	f32 old_value = draw_state.radius2_stack.top->v;
	node->v = v; 
	stack_push(draw_state.radius2_stack.top, node);
	draw_state.radius2_stack.auto_pop = true; 
	return old_value;
};


// radius3

function f32 
draw_top_radius3() {
	return draw_state.radius3_stack.top->v;
}

function f32 
draw_push_radius3(f32 v) {
	draw_radius3_node_t* node = draw_state.radius3_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.radius3_stack.free);
	} else {
		node = (draw_radius3_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_radius3_node_t));
	} 
	f32 old_value = draw_state.radius3_stack.top->v; 
	node->v = v; stack_push(draw_state.radius3_stack.top, node);
	draw_state.radius3_stack.auto_pop = false; 
	return old_value;
}

function f32 
draw_pop_radius3() {
	draw_radius3_node_t* popped = draw_state.radius3_stack.top; 
    f32 result = 0.0f;
	if (popped != 0) {
        result = popped->v;
		stack_pop(draw_state.radius3_stack.top); 
		stack_push(draw_state.radius3_stack.free, popped);
		draw_state.radius3_stack.auto_pop = false;
	} 
	return result;
}

function f32 
draw_set_next_radius3(f32 v) {
	draw_radius3_node_t* node = draw_state.radius3_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.radius3_stack.free);
	} else {
		node = (draw_radius3_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_radius3_node_t));
	} 
	f32 old_value = draw_state.radius3_stack.top->v;
	node->v = v; 
	stack_push(draw_state.radius3_stack.top, node);
	draw_state.radius3_stack.auto_pop = true; 
	return old_value;
};





// thickness

function f32 
draw_top_thickness() {
	return draw_state.thickness_stack.top->v;
}

function f32 
draw_push_thickness(f32 v) {
	draw_thickness_node_t* node = draw_state.thickness_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.thickness_stack.free);
	} else {
		node = (draw_thickness_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_thickness_node_t));
	} 
	f32 old_value = draw_state.thickness_stack.top->v; 
	node->v = v; stack_push(draw_state.thickness_stack.top, node);
	draw_state.thickness_stack.auto_pop = false; 
	return old_value;
}

function f32 
draw_pop_thickness() {
	draw_thickness_node_t* popped = draw_state.thickness_stack.top; 
	f32 result = 0.0f;
	if (popped != 0) {
        result = popped->v;
		stack_pop(draw_state.thickness_stack.top); 
		stack_push(draw_state.thickness_stack.free, popped);
		draw_state.thickness_stack.auto_pop = false;
	} 
	return result;
}

function f32 
draw_set_next_thickness(f32 v) {
	draw_thickness_node_t* node = draw_state.thickness_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.thickness_stack.free);
	} else {
		node = (draw_thickness_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_thickness_node_t));
	} 
	f32 old_value = draw_state.thickness_stack.top->v;
	node->v = v; 
	stack_push(draw_state.thickness_stack.top, node);
	draw_state.thickness_stack.auto_pop = true; 
	return old_value;
};


// softness

function f32 
draw_top_softness() {
	return draw_state.softness_stack.top->v;
}

function f32 
draw_push_softness(f32 v) {
	draw_softness_node_t* node = draw_state.softness_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.softness_stack.free);
	} else {
		node = (draw_softness_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_softness_node_t));
	} 
	f32 old_value = draw_state.softness_stack.top->v; 
	node->v = v; stack_push(draw_state.softness_stack.top, node);
	draw_state.softness_stack.auto_pop = false; 
	return old_value;
}

function f32 
draw_pop_softness() {
	draw_softness_node_t* popped = draw_state.softness_stack.top; 
	f32 result = 0.0f;
	if (popped != 0) {
        result = popped->v;
		stack_pop(draw_state.softness_stack.top); 
		stack_push(draw_state.softness_stack.free, popped);
		draw_state.softness_stack.auto_pop = false;
	} 
	return result;
}

function f32 
draw_set_next_softness(f32 v) {
	draw_softness_node_t* node = draw_state.softness_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.softness_stack.free);
	} else {
		node = (draw_softness_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_softness_node_t));
	} 
	f32 old_value = draw_state.softness_stack.top->v;
	node->v = v; 
	stack_push(draw_state.softness_stack.top, node);
	draw_state.softness_stack.auto_pop = true; 
	return old_value;
};


// font

function font_handle_t 
draw_top_font() {
	return draw_state.font_stack.top->v;
}

function font_handle_t 
draw_push_font(font_handle_t v) {
	draw_font_node_t* node = draw_state.font_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.font_stack.free);
	} else {
		node = (draw_font_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_font_node_t));
	} 
	font_handle_t old_value = draw_state.font_stack.top->v; 
	node->v = v; stack_push(draw_state.font_stack.top, node);
	draw_state.font_stack.auto_pop = false; 
	return old_value;
}

function font_handle_t 
draw_pop_font() {
	draw_font_node_t* popped = draw_state.font_stack.top; 
    font_handle_t result = { 0 };
	if (popped != 0) {
        result = popped->v;
		stack_pop(draw_state.font_stack.top); 
		stack_push(draw_state.font_stack.free, popped);
		draw_state.font_stack.auto_pop = false;
	} 
	return result;
}

function font_handle_t 
draw_set_next_font(font_handle_t v) {
	draw_font_node_t* node = draw_state.font_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.font_stack.free);
	} else {
		node = (draw_font_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_font_node_t));
	} 
	font_handle_t old_value = draw_state.font_stack.top->v;
	node->v = v; 
	stack_push(draw_state.font_stack.top, node);
	draw_state.font_stack.auto_pop = true; 
	return old_value;
};


// font_size

function f32 
draw_top_font_size() {
	return draw_state.font_size_stack.top->v;
}

function f32 
draw_push_font_size(f32 v) {
	draw_font_size_node_t* node = draw_state.font_size_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.font_size_stack.free);
	} else {
		node = (draw_font_size_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_font_size_node_t));
	} 
	f32 old_value = draw_state.font_size_stack.top->v; 
	node->v = v; stack_push(draw_state.font_size_stack.top, node);
	draw_state.font_size_stack.auto_pop = false; 
	return old_value;
}

function f32 
draw_pop_font_size() {
	draw_font_size_node_t* popped = draw_state.font_size_stack.top; 
	f32 result = 0.0f;
	if (popped != 0) {
        result = popped->v;
		stack_pop(draw_state.font_size_stack.top); 
		stack_push(draw_state.font_size_stack.free, popped);
		draw_state.font_size_stack.auto_pop = false;
	} 
	return result;
}

function f32 
draw_set_next_font_size(f32 v) {
	draw_font_size_node_t* node = draw_state.font_size_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.font_size_stack.free);
	} else {
		node = (draw_font_size_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_font_size_node_t));
	} 
	f32 old_value = draw_state.font_size_stack.top->v;
	node->v = v; 
	stack_push(draw_state.font_size_stack.top, node);
	draw_state.font_size_stack.auto_pop = true; 
	return old_value;
};



// clip_mask

function rect_t 
draw_top_clip_mask() {
	return draw_state.clip_mask_stack.top->v;
}

function rect_t 
draw_push_clip_mask(rect_t v) {
	draw_clip_mask_node_t* node = draw_state.clip_mask_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.clip_mask_stack.free);
	} else {
		node = (draw_clip_mask_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_clip_mask_node_t));
	} 
	rect_t old_value = draw_state.clip_mask_stack.top->v; 
	node->v = v; stack_push(draw_state.clip_mask_stack.top, node);
	draw_state.clip_mask_stack.auto_pop = false; 
	return old_value;
}

function rect_t 
draw_pop_clip_mask() {
	draw_clip_mask_node_t* popped = draw_state.clip_mask_stack.top; 
    rect_t result = { 0 };
	if (popped != 0) {
        result = popped->v;
		stack_pop(draw_state.clip_mask_stack.top); 
		stack_push(draw_state.clip_mask_stack.free, popped);
		draw_state.clip_mask_stack.auto_pop = false;
	} 
	return result;
}

function rect_t 
draw_set_next_clip_mask(rect_t v) {
	draw_clip_mask_node_t* node = draw_state.clip_mask_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.clip_mask_stack.free);
	} else {
		node = (draw_clip_mask_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_clip_mask_node_t));
	} 
	rect_t old_value = draw_state.clip_mask_stack.top->v;
	node->v = v; 
	stack_push(draw_state.clip_mask_stack.top, node);
	draw_state.clip_mask_stack.auto_pop = true; 
	return old_value;
};


// texture

function gfx_handle_t 
draw_top_texture() {
	return draw_state.texture_stack.top->v;
}

function gfx_handle_t 
draw_push_texture(gfx_handle_t v) {
	draw_texture_node_t* node = draw_state.texture_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.texture_stack.free);
	} else {
		node = (draw_texture_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_texture_node_t));
	} 
	gfx_handle_t old_value = draw_state.texture_stack.top->v; 
	node->v = v; stack_push(draw_state.texture_stack.top, node);
	draw_state.texture_stack.auto_pop = false; 
	return old_value;
}

function gfx_handle_t 
draw_pop_texture() {
	draw_texture_node_t* popped = draw_state.texture_stack.top; 
    gfx_handle_t result = { 0 };
	if (popped != 0) {
        result = popped->v;
		stack_pop(draw_state.texture_stack.top); 
		stack_push(draw_state.texture_stack.free, popped);
		draw_state.texture_stack.auto_pop = false;
	} 
	return result;
}

function gfx_handle_t 
draw_set_next_texture(gfx_handle_t v) {
	draw_texture_node_t* node = draw_state.texture_stack.free; 
	if (node != 0) {
		stack_pop(draw_state.texture_stack.free);
	} else {
		node = (draw_texture_node_t*)arena_alloc(draw_state.batch_arena, sizeof(draw_texture_node_t));
	} 
	gfx_handle_t old_value = draw_state.texture_stack.top->v;
	node->v = v; 
	stack_push(draw_state.texture_stack.top, node);
	draw_state.texture_stack.auto_pop = true; 
	return old_value;
};

// group stacks

// colors
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

// rounding
function vec4_t 
draw_top_rounding() {
	f32 x = draw_top_radius0();
	f32 y = draw_top_radius1();
	f32 z = draw_top_radius2();
	f32 w = draw_top_radius3();
	return vec4(x, y, z, w);
}

function void
draw_push_rounding(f32 radius) {
	draw_push_radius0(radius);
	draw_push_radius1(radius);
	draw_push_radius2(radius);
	draw_push_radius3(radius);
}

function void
draw_push_rounding(vec4_t radii) {
	draw_push_radius0(radii.x);
	draw_push_radius1(radii.y);
	draw_push_radius2(radii.z);
	draw_push_radius3(radii.w);
}

function void
draw_set_next_rounding(f32 radius) {
	draw_set_next_radius0(radius);
	draw_set_next_radius1(radius);
	draw_set_next_radius2(radius);
	draw_set_next_radius3(radius);
}

function void
draw_set_next_rounding(vec4_t radii) {
	draw_set_next_radius0(radii.x);
	draw_set_next_radius1(radii.y);
	draw_set_next_radius2(radii.z);
	draw_set_next_radius3(radii.w);
}

function void
draw_pop_rounding() {
	draw_pop_radius0();
	draw_pop_radius1();
	draw_pop_radius2();
	draw_pop_radius3();
}


#endif // DRAW_CPP