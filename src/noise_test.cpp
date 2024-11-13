// noise_test.cpp

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

#include "utils/noise.h"

global os_window_t* window;
global gfx_renderer_t* renderer;
global arena_t* resource_arena;
global arena_t* scratch_arena;

global u8* texture_data;
global gfx_texture_t* noise_texture;

global i32 octaves = 5;
global f32 gain = 0.5f;
global f32 lacunarity = 2.0f;

// funtions

function void
generate_texture() {

	u64 time_start = os_time_microseconds();

	vec2_t range = noise_fbm_range(2, octaves, lacunarity, gain);

	for (i32 y = 0; y < 256; y++) {
		for (i32 x = 0; x < 256; x++) {
			i32 index = (y * 256 + x) * 4;
			
			f32 value = remap(noise_fbm(vec2(x / 64.0f, y / 64.0f), octaves, lacunarity, gain), range.x, range.y, 0.0f, 1.0f);
			
			texture_data[index + 0] = (u8)(value * 255.0f); // red
			texture_data[index + 1] = (u8)(value * 255.0f); // green
			texture_data[index + 2] = (u8)(value * 255.0f); // blue
			texture_data[index + 3] = 255; // alpha
		}
	}

	u64 time_end = os_time_microseconds();
	printf("time elapsed: %f (ms)\n", (f64)(time_end - time_start) / 1000.0);

	gfx_texture_fill(noise_texture, texture_data);
}

function void
app_init() {

	resource_arena = arena_create(gigabytes(2));
	scratch_arena = arena_create(megabytes(1));

	noise_texture = gfx_texture_create(uvec2(256, 256));
	texture_data = (u8*)arena_alloc(resource_arena, sizeof(u8) * 256 * 256 *4);
	generate_texture();



	// test output range of simplex noise

	//u64 time_start = os_time_microseconds();

	//for (i32 i = 0; i < 1500000; i++) {

	//	f32 value = noise_simplex(vec3(i / 150.0f));

	//	if (value > 0.866f || value < -0.866f) {
	//		printf("outside range: %f\n", value);
	//	}

	//}

	//u64 time_end = os_time_microseconds();
	//printf("range time elapsed: %f (ms)\n", (f64)(time_end - time_start) / 1000.0);

}

function void
app_update() {

	if (os_key_press(window, os_key_space)) {
		generate_texture();
	}

}

function void
app_release() {

}

function void
app_render_pass(gfx_render_target_t* current_render_target, gfx_render_target_t* prev_render_target) {

	// draw texture
	gfx_pipeline_t pipeline = gfx_pipeline_create();
	pipeline.topology = gfx_topology_tri_strip;
	pipeline.depth_mode = gfx_depth_none;
	draw_push_pipeline(pipeline);
	draw_push_shader(ui_state.ui_shader);
	draw_push_constants(&ui_state.constants, sizeof(ui_constants_t), 0);
	draw_push_image(rect(0.0f, 0.0f, 512.0f, 512.0f), noise_texture);
	draw_submit();

	// ui
	ui_begin_frame(renderer);

	ui_push_pref_width(ui_size_pixel(300.0f, 1.0f));
	ui_push_pref_height(ui_size_pixel(20.0f, 1.0f));

	ui_interaction interation;

	interation |= ui_slider(str("octaves"), &octaves, 1, 10);
	interation |= ui_slider(str("lacunarity"), &lacunarity, 0.1f, 5.0f);
	interation |= ui_slider(str("gain"), &gain, 0.1f, 5.0f);

	if (interation & ui_interaction_left_dragging) {
		generate_texture();
	}

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