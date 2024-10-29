// ui_test_main.cpp

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

struct constants_2d_t {
	vec2_t window_size;
	vec2_t time;
};

// globals

global os_window_t* window;
global gfx_renderer_t* renderer;
global arena_t* resource_arena;
global arena_t* scratch_arena;
global frame_stats_t frame_stats;
global constants_2d_t constants_2d;

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

		constants_2d.window_size = vec2((f32)renderer->resolution.x, (f32)renderer->resolution.y);
		constants_2d.time = vec2((f32)window->elasped_time, (f32)window->delta_time);
	}

}

function void
ui_pass(gfx_render_target_t* current_render_target, gfx_render_target_t* prev_rander_target) {

	ui_begin_frame(renderer);

	ui_push_pref_width(ui_size_pixel(200.0f, 1.0f));
	ui_push_pref_height(ui_size_pixel(20.0f, 1.0f));

	if (ui_button(str("close")) & ui_interaction_left_clicked) {
		os_window_close(window);
	}
	ui_button(str("button"));
	persist f32 slider_value = 0.3f;
	ui_slider(str("slider"), &slider_value, 0.0f, 1.0f);

	cstr items[] = { "item_1", "item_2", "item_3" };
	persist i32 index = -1;
	ui_combo(str("combo"), &index, items, 3);
	persist b8 checkbox_value = false;
	ui_checkbox(str("checkbox"), &checkbox_value);

	persist b8 expander_value = false;
	ui_expander(str("expander"), &expander_value);
	if (expander_value) {
		persist color_t hsv_color = color(0.6f, 0.4f, 0.3f, 1.0f, color_format_hsv);
		ui_set_next_pref_height(ui_size_pixel(200.0f, 1.0f));
		ui_color_sat_val_quad(str("sat_val_quad"), hsv_color.h, &hsv_color.s, &hsv_color.v);

		ui_color_hue_bar(str("hue_quad"), &hsv_color.h, hsv_color.s, hsv_color.v);

		ui_set_next_pref_height(ui_size_pixel(200.0f, 1.0f));
		ui_color_wheel(str("color_wheel"), &hsv_color.h, &hsv_color.s, &hsv_color.v);

	}
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
	window = os_window_open(str("ui test"), 1280, 960, os_window_flag_custom_border);
	renderer = gfx_renderer_create(window, color(0x050505ff));
	
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