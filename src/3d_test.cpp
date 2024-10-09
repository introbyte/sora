// 3d_test.cpp

// includes

#include "engine/base.h"
#include "engine/os.h"
#include "engine/gfx.h"
#include "engine/ui.h"

#include "engine/base.cpp"
#include "engine/os.cpp"
#include "engine/gfx.cpp"
#include "engine/ui.cpp"

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

// globals

global os_window_t* window;
global gfx_renderer_t* renderer;
global arena_t* scratch;
global gfx_font_t* font_system;

global frame_stats_t frame_stats;

global gfx_mesh_t* mesh;
global gfx_shader_t* mesh_shader;

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
	scratch = arena_create(megabytes(1));

	// init frame stats
	frame_stats.index = frame_stats.count = frame_stats.tick = 0;

	mesh_shader = gfx_shader_load(str("res/shaders/shader_3d.hlsl"), { {
		{"POSITION", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex},
		{"NORMAL", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex},
		{"TANGENT", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex},
		{"BITANGENT", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex},
		{"TEXCOORD", 0, gfx_vertex_format_float2, gfx_vertex_class_per_vertex},
		{"COLOR", 0, gfx_vertex_format_float4, gfx_vertex_class_per_vertex},
	} });

	// load mesh
	mesh = gfx_mesh_load(str("res/models/scene.obj"));
}

function void
shadow_pass(gfx_render_pass_t* pass) {

	// set camera pos 
	gfx_3d_constants_t shadow_constants;
	shadow_constants.projection = mat4_orthographic(-30.0f, 30.0f, 30.0f, -30.0f, 0.01f, 100.0f);
	shadow_constants.view = mat4_lookat(vec3(10.0f, 15.0f, 7.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	shadow_constants.view_projection = mat4_mul(shadow_constants.projection, shadow_constants.view);
	gfx_set_constants(&shadow_constants, sizeof(gfx_3d_constants_t));

	// draw mesh
	gfx_set_next_shader(mesh_shader);
	gfx_draw_mesh(mesh, mat4_identity());

}

function void
scene_pass(gfx_render_pass_t* pass) {



	gfx_3d_constants_t scene_constants;
	f32 el = renderer->window->elasped_time;
	vec3_t from_pos = vec3(10.0f * sinf(el), 3.0f, 10.0f * cosf(el));
	scene_constants.projection = mat4_perspective(80.0f, (f32)renderer->resolution.x / (f32)renderer->resolution.y, 0.01f, 100.0f);
	scene_constants.view = mat4_lookat(from_pos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	scene_constants.view_projection = mat4_mul(scene_constants.projection, scene_constants.view);

	mat4_t shadow_projection = mat4_orthographic(-30.0f, 30.0f, 30.0f, -30.0f, 0.01f, 100.0f);
	mat4_t shadow_view = mat4_lookat(vec3(10.0f, 15.0f, 7.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	scene_constants.shadow_view_projection = mat4_mul(shadow_projection, shadow_view);

	gfx_set_constants(&scene_constants, sizeof(gfx_3d_constants_t));

	// draw mesh
	gfx_set_texture(pass->prev->render_target->depthbuffer, 1);
	gfx_set_next_shader(mesh_shader);
	gfx_draw_mesh(mesh, mat4_identity());

}

function void
ui_pass(gfx_render_pass_t* pass) {

	gfx_render_target_blit(pass->render_target, pass->prev->render_target);

	// draw ui
	ui_begin_frame(renderer);
	ui_push_pref_width(ui_size_pixel(300.0f, 1.0f));
	ui_push_pref_height(ui_size_pixel(21.0f, 1.0f));

	// frame stats
	{
		ui_labelf("avg: %.2f ms (fps: %.1f)", frame_stats.avg, 1000.0f / frame_stats.avg);
		ui_labelf("min: %.2f ms", frame_stats.min);
		ui_labelf("max: %.2f ms", frame_stats.max);
	}

	ui_pop_pref_width();
	ui_pop_pref_height();

	ui_end_frame();

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

	}

}

function void
app_release() {

}

function i32
app_entry_point(i32 argc, char** argv) {

	// init layers
	os_init();
	gfx_init();
	ui_init();

	// create contexts
	window = os_window_open(str("3d test"), 1280, 960);
	renderer = gfx_renderer_create(window, color(0x050505ff));

	// set up render passes
	gfx_render_pass_desc_t pass_desc = { 0 };

	pass_desc.name = str("shadow");
	pass_desc.size = uvec2(4096, 4096);
	pass_desc.samples = 1;
	pass_desc.flags = gfx_render_pass_flag_depth_only | gfx_render_pass_flag_fixed_size;
	pass_desc.colorbuffer_format = gfx_texture_format_null;
	pass_desc.depthbuffer_format = gfx_texture_format_d32;
	gfx_renderer_add_pass(renderer, pass_desc, shadow_pass);

	pass_desc.name = str("scene");
	pass_desc.size = renderer->resolution;
	pass_desc.samples = 1;
	pass_desc.flags = 0;
	pass_desc.colorbuffer_format = gfx_texture_format_rgba8;
	pass_desc.depthbuffer_format = gfx_texture_format_d32;
	gfx_renderer_add_pass(renderer, pass_desc, scene_pass );

	pass_desc.name = str("ui");
	pass_desc.size = renderer->resolution;
	pass_desc.samples = 1;
	pass_desc.flags = gfx_render_pass_flag_color_only;
	pass_desc.colorbuffer_format = gfx_texture_format_rgba8;
	pass_desc.depthbuffer_format = gfx_texture_format_null;
	gfx_renderer_add_pass(renderer, pass_desc, ui_pass );


	// init
	app_init();

	// main loop
	while (os_any_window_exist()) {

		// update layers
		os_update();
		gfx_update();
		app_update();

		// submit renderer
		gfx_renderer_submit(renderer);
	}

	// release
	app_release();
	gfx_renderer_release(renderer);

	// release layers
	ui_release();
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