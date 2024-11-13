// projection_test.cpp


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

global os_window_t* window;
global gfx_renderer_t* renderer;
global arena_t* resource_arena;
global arena_t* scratch_arena;

global gfx_shader_t* test_shader;
global mesh_t* test_mesh;

// funtions

function void
app_init() {
	
	resource_arena = arena_create(gigabytes(2));
	scratch_arena = arena_create(megabytes(1));

	global gfx_shader_attribute_t attributes[] = {
		{"POSITION", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex}
	};

	test_shader = gfx_shader_load(str("res/shaders/test.hlsl"), attributes, 1);
	test_mesh = mesh_load(resource_arena, scratch_arena, str("res/models/scene.obj"));

	mat4_t projection = mat4_perspective(80.0f, (f32)window->resolution.x / (f32)window->resolution.y, 0.01f, 1000.0f);
	mat4_t view = mat4_translate(vec3(0.0f, 0.0f, -5.0f));
	mat4_print(projection);
	mat4_print(view);
}

function void
app_update() {

}

function void
app_release() {

}

function void 
app_render_pass(gfx_render_target_t* current_render_target, gfx_render_target_t* prev_render_target) {

	mat4_t projection = mat4_perspective(80.0f, (f32)window->resolution.x / (f32)window->resolution.y, 1.0f, 100.0f);
	mat4_t view = mat4_translate(vec3(0.0f, 0.0f, 5.0f));
	mat4_t view_projection = mat4_mul(projection, view);

	mat4_t matrices[] = {
		view_projection,
		projection,
		view,
	};

	draw_push_constants(&matrices, sizeof(mat4_t) * 3, 0);

	gfx_pipeline_t pipeline = gfx_pipeline_create();
	pipeline.filter_mode = gfx_filter_nearest;
	draw_push_pipeline(pipeline);

	draw_set_next_shader(test_shader);
	draw_push_vertices(test_mesh->vertices, test_mesh->vertex_size, test_mesh->vertex_count);
	draw_submit();

	ui_begin_frame(renderer);

	ui_push_pref_width(ui_size_pixel(200.0f, 1.0f));
	ui_push_pref_height(ui_size_pixel(20.0f, 1.0f));

	ui_labelf("fuck everything.");


	ui_end_frame();

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

	// add render pass
	gfx_render_target_desc_t ui_render_target_desc = { 0 };
	ui_render_target_desc.size = renderer->resolution;
	ui_render_target_desc.sample_count = 1;
	ui_render_target_desc.flags = gfx_render_target_flag_no_depth;
	ui_render_target_desc.colorbuffer_format = gfx_texture_format_rgba8;
	gfx_renderer_add_pass(renderer, str("render_pass"), app_render_pass, ui_render_target_desc);

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