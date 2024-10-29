// lighting_test.cpp

// includes

#include "engine/base.h"
#include "engine/os.h"
#include "engine/gfx.h"
#include "engine/font.h"
#include "engine/draw.h"
#include "engine/ui.h"

#include "engine/base.cpp"
#include "engine/os.cpp"
#include "engine/gfx.cpp"
#include "engine/font.cpp"
#include "engine/draw.cpp"
#include "engine/ui.cpp"

#include "utils/mesh.h"
#include "utils/camera.h"

struct frame_stats_t {
	f32 dt;
	f32 min;
	f32 max;
	f32 avg;

	f32 times[64];
	u32 index;
	u32 count;

	u32 tick;
};

struct light_constants_t {
	mat4_t shadow_view_projection;
	vec3_t light_dir;
	f32 unused;
};

struct constants_2d_t {
	vec2_t window_size;
	vec2_t time;
};

struct material_constants_t {
	i32 type; // -1 = none, 0 = vertex, 1 = pixel, 2 = texel
	f32 roughness;
	f32 metallic;
};

// globals

global os_window_t* window;
global gfx_renderer_t* renderer;
global arena_t* resource_arena;
global arena_t* scratch_arena;
global frame_stats_t frame_stats;
global camera_t* camera;
global gfx_shader_t* shader;
global gfx_shader_t* shadow_pass_shader;
global mesh_t* mesh;
global light_constants_t light_constants;
global constants_2d_t constants_2d;
global material_constants_t material_constants;
global gfx_texture_t* texture;


function void
frame_stats_update(f32 dt) {

	// frame times
	frame_stats.dt = dt;
	frame_stats.times[frame_stats.index] = dt;
	frame_stats.index = (frame_stats.index + 1) % 64;
	if (frame_stats.count < 64) {
		frame_stats.count++;
	}

	// update stats every 144 frames
	frame_stats.tick++;
	if (frame_stats.tick >= 144 / 4) {
		frame_stats.tick = 0;

		// reset stats
		frame_stats.min = f32_max;
		frame_stats.max = f32_min;
		frame_stats.avg = 0.0f;

		// calculate frame stats
		for (i32 i = 0; i < frame_stats.count; i++) {
			f32 time = frame_stats.times[i];
			if (time < frame_stats.min) { frame_stats.min = time; }
			if (time > frame_stats.max) { frame_stats.max = time; }
			frame_stats.avg += time;
		}
		frame_stats.avg /= frame_stats.count;

	}

}

function void
app_init() {

	// allocate arenas
	resource_arena = arena_create(gigabytes(2));
	scratch_arena = arena_create(megabytes(1));

	// init frame stats
	frame_stats.index = frame_stats.count = frame_stats.tick = 0;

	// load assets
	global gfx_shader_attribute_t attributes[] = {
		{"POSITION", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex},
		{"NORMAL", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex},
		{"TANGENT", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex},
		{"BITANGENT", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex},
		{"TEXCOORD", 0, gfx_vertex_format_float2, gfx_vertex_class_per_vertex},
		{"COLOR", 0, gfx_vertex_format_float4, gfx_vertex_class_per_vertex},
	};
	shader = gfx_shader_load(str("res/shaders/shader_texel_pbr.hlsl"), attributes, 6);
	shadow_pass_shader = gfx_shader_load(str("res/shaders/shadow_pass.hlsl"), attributes, 6);

	// texture
	texture = gfx_texture_load(str("res/textures/texture.png"));

	// mesh
	mesh = mesh_load(resource_arena, scratch_arena, str("res/models/scene.obj"));

	// create camera
	camera = camera_create(resource_arena, renderer, 75.0f, 0.01f, 100.0f);
	
	// set light constants
	light_constants.light_dir = vec3(5.0f, 7.0f, 2.0f);
	mat4_t projection = mat4_orthographic(-10.0f, 10.0f, 10.0f, -10.0f, 0.01f, 100.0f);
	mat4_t view = mat4_lookat(light_constants.light_dir, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	light_constants.shadow_view_projection = mat4_mul(projection, view);
	
	// set material constants
	material_constants.type = 1;
	material_constants.roughness = 0.1f;
	material_constants.metallic = 0.25f;


}

function void
app_update() {

	// update 
	{
		frame_stats_update(window->delta_time * 1000.0f);

		// fullscreen
		if (os_key_release(window, os_key_F11)) {
			os_window_fullscreen(window);
		}

		camera_update(camera);

		// update light
		if (os_key_press(window, os_key_L)) {
			light_constants.light_dir = camera->position;
			mat4_t projection = mat4_orthographic(-10.0f, 10.0f, 10.0f, -10.0f, 0.01f, 100.0f);
			mat4_t view = mat4_lookat(light_constants.light_dir, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
			light_constants.shadow_view_projection = mat4_mul(projection, view);
		}

		constants_2d.window_size = vec2((f32)renderer->resolution.x, (f32)renderer->resolution.y);
		constants_2d.time = vec2((f32)window->elasped_time, (f32)window->delta_time);
		
	}

}

function void
shadow_pass(gfx_render_target_t* current_render_target, gfx_render_target_t* prev_render_target) {
	
	// set light constants
	draw_push_constants(&light_constants, sizeof(light_constants_t));
	
	// set pipeline
	gfx_pipeline_t pipeline = gfx_pipeline_create();
	pipeline.cull_mode = gfx_cull_mode_front;
	draw_push_pipeline(pipeline);

	// draw scene
	draw_set_next_shader(shadow_pass_shader);
	draw_push_vertices(mesh->vertices, mesh->vertex_size, mesh->vertex_count);

	// submit
	draw_submit();
}

function void
scene_pass(gfx_render_target_t* current_render_target, gfx_render_target_t* prev_render_target) {

	// set constants
	draw_push_constants(&camera->constants, sizeof(camera->constants), 0);
	draw_push_constants(&light_constants, sizeof(light_constants), 1);
	draw_push_constants(&material_constants, sizeof(material_constants), 2);

	// set pipeline
	gfx_pipeline_t pipeline = gfx_pipeline_create();
	pipeline.filter_mode = gfx_filter_nearest;
	draw_push_pipeline(pipeline);

	// draw scene
	draw_set_next_shader(shader);
	draw_push_texture(texture);
	draw_push_vertices(mesh->vertices, mesh->vertex_size, mesh->vertex_count);

	// set shadow map
	gfx_set_texture(font_state.atlas_texture, 0);
	gfx_set_texture(prev_render_target->depthbuffer, 1);

	// submit
	draw_submit();
}

function void
ui_pass(gfx_render_target_t* current_render_target, gfx_render_target_t* prev_rander_target) {

	// copy last render target
	gfx_texture_blit(current_render_target->colorbuffer, prev_rander_target->colorbuffer);

	ui_begin_frame(renderer);

	ui_push_pref_width(ui_size_pixel(200.0f, 1.0f));
	ui_push_pref_height(ui_size_pixel(20.0f, 1.0f));

	// frame stats
	ui_labelf("avg: %.2f ms (fps: %.1f)", frame_stats.avg, 1000.0f / frame_stats.avg);
	ui_labelf("min: %.2f ms", frame_stats.min);
	ui_labelf("max: %.2f ms", frame_stats.max);


	cstr types[] = { "per vertex", "per pixel", "per texel" };
	ui_combo(str("lighting type"), &material_constants.type, types, 3);
	
	ui_pop_pref_width();
	ui_pop_pref_height();

	ui_end_frame();

}

function void
app_release() {

}

// entry point

function i32
app_entry_point(i32 argc, char** argv) {

	// init layers
	os_init();
	gfx_init();
	font_init();
	draw_init();
	ui_init();

	// create contexts
	window = os_window_open(str("3d test"), 1280, 960, os_window_flag_custom_border);
	renderer = gfx_renderer_create(window, color(0x050505ff));

	// add shadow pass
	gfx_render_target_desc_t shadow_render_target_desc = { 0 };
	shadow_render_target_desc.size = uvec2(4096, 4096);
	shadow_render_target_desc.sample_count = 1;
	shadow_render_target_desc.flags = gfx_render_target_flag_fixed_size | gfx_render_target_flag_no_color;
	shadow_render_target_desc.depthbuffer_format = gfx_texture_format_d32;
	gfx_renderer_add_pass(renderer, str("shadow"), shadow_pass, shadow_render_target_desc);

	// add scene pass
	gfx_render_target_desc_t scene_render_target_desc = { 0 };
	scene_render_target_desc.size = renderer->resolution;
	scene_render_target_desc.sample_count = 1;
	scene_render_target_desc.flags = 0;
	scene_render_target_desc.colorbuffer_format = gfx_texture_format_rgba8;
	scene_render_target_desc.depthbuffer_format = gfx_texture_format_d24s8;
	gfx_renderer_add_pass(renderer, str("scene"), scene_pass, scene_render_target_desc);

	// add ui pass
	gfx_render_target_desc_t ui_render_target_desc = { 0 };
	ui_render_target_desc.size = renderer->resolution;
	ui_render_target_desc.sample_count = 1;
	ui_render_target_desc.flags = gfx_render_target_flag_no_depth;
	ui_render_target_desc.colorbuffer_format = gfx_texture_format_rgba8;
	gfx_renderer_add_pass(renderer, str("ui"), ui_pass, ui_render_target_desc);

	// init
	app_init();

	// main loop
	while (os_window_is_running(window)) {

		// update layers
		os_update();
		gfx_update();
		draw_update();

		// update app
		app_update();

		// submit to renderer
		gfx_renderer_submit(renderer);
	}

	// release
	app_release();

	// release context
	gfx_renderer_release(renderer);
	os_window_close(window);

	// release layers
	ui_release();
	draw_release();
	font_release();
	gfx_release();
	os_release();

	return 0;
}

// entry point

#if defined(BUILD_DEBUG)
int main(int argc, char** argv) {
	return app_entry_point(argc, argv);
}
#elif defined(BUILD_RELEASE)
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	return app_entry_point(__argc, __argv);
}
#endif 