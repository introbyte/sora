// 3d_test.cpp

// includes

#include "engine/base.h"
#include "engine/os.h"
#include "engine/gfx.h"
#include "engine/draw.h"

#include "engine/base.cpp"
#include "engine/os.cpp"
#include "engine/gfx.cpp"
#include "engine/draw.cpp"

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
	resource_arena = arena_create(megabytes(64));
	scratch_arena = arena_create(megabytes(1));

	// init frame stats
	frame_stats.index = frame_stats.count = frame_stats.tick = 0;

	// load assets
	gfx_shader_attribute_t attributes[] = {
		{"POSITION", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex},
		{"NORMAL", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex},
		{"TANGENT", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex},
		{"BITANGENT", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex},
		{"TEXCOORD", 0, gfx_vertex_format_float2, gfx_vertex_class_per_vertex},
		{"COLOR", 0, gfx_vertex_format_float4, gfx_vertex_class_per_vertex},
	};
	shader = gfx_shader_load(str("res/shaders/shader_3d.hlsl"), attributes, 6);
	shadow_pass_shader = gfx_shader_load(str("res/shaders/shadow_pass.hlsl"), attributes, 6);

	mesh = mesh_load(resource_arena, scratch_arena, str("res/models/scene.obj"));

	// create camera
	camera = camera_create(resource_arena, renderer, 75.0f, 0.01f, 100.0f);

	// set light constants
	light_constants.light_dir = vec3(5.0f, 7.0f, 2.0f);
	mat4_t projection = mat4_orthographic(-10.0f, 10.0f, 10.0f, -10.0f, 0.01f, 100.0f);
	mat4_t view = mat4_lookat(light_constants.light_dir, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	light_constants.shadow_view_projection = mat4_mul(projection, view);

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
	}

}

function void
shadow_pass(gfx_render_target_t* prev_render_target) {
	
	// set light constants
	draw_set_constants(&light_constants, sizeof(light_constants_t));
	
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
scene_pass(gfx_render_target_t* prev_render_target) {

	// set constants
	draw_set_constants(&camera->constants, sizeof(camera_constants_t), 0);
	draw_set_constants(&light_constants, sizeof(light_constants), 1);

	// set pipeline
	gfx_pipeline_t pipeline = gfx_pipeline_create();
	draw_push_pipeline(pipeline);

	// draw scene
	draw_set_next_shader(shader);
	draw_push_vertices(mesh->vertices, mesh->vertex_size, mesh->vertex_count);

	// set shadow map
	gfx_set_texture(prev_render_target->depthbuffer, 1);

	// submit
	draw_submit();
}

function void
app_release() {

}

function i32
app_entry_point(i32 argc, char** argv) {

	// init layers
	os_init();
	gfx_init();
	draw_init();

	// create contexts
	window = os_window_open(str("3d test"), 1280, 960);
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
	scene_render_target_desc.sample_count = 8;
	scene_render_target_desc.flags = 0;
	scene_render_target_desc.colorbuffer_format = gfx_texture_format_rgba8;
	scene_render_target_desc.depthbuffer_format = gfx_texture_format_d32;
	gfx_renderer_add_pass(renderer, str("scene"), scene_pass, scene_render_target_desc);

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
	draw_release();
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