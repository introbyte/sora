// ui_test_main.cpp

// includes

#include "base.h"
#include "os.h"
#include "gfx.h"
#include "ui.h"

#include "base.cpp"
#include "os.cpp"
#include "gfx.cpp"
#include "ui.cpp"

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
frame_stats_render() {
	//str_t text;
	//vec2_t pos = vec2(5.0f, 5.0f);

	//gfx_font_t* font = font_system;
	//const f32 font_size = 9.0f;

	//text = str_format(scratch, "frame_time: %.2f ms", frame_stats.dt);
	//gfx_renderer_push_text(renderer, text, vec2_add(pos, 1.0f), gfx_text_params(color(0x15151588), font, font_size));
	//gfx_renderer_push_text(renderer, text, pos, gfx_text_params(color(0xcfcfcfff), font, font_size));
	//pos = vec2_add(pos, vec2(0.0f, 18.0f));

	//text = str_format(scratch, "min: %.2f ms", frame_stats.min);
	//gfx_renderer_push_text(renderer, text, vec2_add(pos, 1.0f), gfx_text_params(color(0x15151588), font, font_size));
	//gfx_renderer_push_text(renderer, text, pos, gfx_text_params(color(0xcfcfcfff), font, font_size));
	//pos = vec2_add(pos, vec2(0.0f, 18.0f));

	//text = str_format(scratch, "max: %.2f ms", frame_stats.max);
	//gfx_renderer_push_text(renderer, text, vec2_add(pos, 1.0f), gfx_text_params(color(0x15151588), font, font_size));
	//gfx_renderer_push_text(renderer, text, pos, gfx_text_params(color(0xcfcfcfff), font, font_size));
	//pos = vec2_add(pos, vec2(0.0f, 18.0f));

	//text = str_format(scratch, "avg: %.2f ms (fps: %.1f)", frame_stats.avg, 1000.0f / frame_stats.avg);
	//gfx_renderer_push_text(renderer, text, vec2_add(pos, 1.0f), gfx_text_params(color(0x15151588), font, font_size));
	//gfx_renderer_push_text(renderer, text, pos, gfx_text_params(color(0xcfcfcfff), font, font_size));
	//pos = vec2_add(pos, vec2(0.0f, 18.0f));

	//arena_clear(scratch);
}




function void 
app_init() {

	// allocate arenas
	scratch = arena_create(megabytes(1));

	// init frame stats
	frame_stats.index = frame_stats.count = frame_stats.tick = 0;

}

function void
app_update_and_render() {

	// update 
	{
		frame_stats_update(window->delta_time * 1000.0f);

		// fullscreen
		if (os_key_release(window, os_key_F11)) {
			os_window_fullscreen(window);
		}

	}

	// render
	{
		ui_begin_frame(renderer);

		ui_set_next_fixed_width(100.0f);
		ui_set_next_fixed_height(20.0f);
		ui_set_next_fixed_x(200.0f);
		ui_set_next_fixed_y(15.0f);
		ui_set_next_rounding(2.0f);
		ui_set_next_text_alignment(ui_text_alignment_center);
		ui_frame_flags flags = 
			ui_frame_flag_draw_background | 
			ui_frame_flag_draw_hover_effects | 
			ui_frame_flag_draw_active_effects | 
			ui_frame_flag_draw_text | 
			ui_frame_flag_clip | 
			ui_frame_flag_interactable;
		ui_frame_t* frame = ui_frame_from_string(str("This text is too long and will clip"), flags);
		ui_interaction interaction = ui_frame_interaction(frame);

		if (interaction & ui_interaction_left_pressed) { printf("left pressed\n"); }
		if (interaction & ui_interaction_middle_pressed) { printf("middle pressed\n"); }
		if (interaction & ui_interaction_right_pressed) { printf("right pressed\n"); }
		if (interaction & ui_interaction_left_released) { printf("left released\n"); }
		if (interaction & ui_interaction_middle_released) { printf("middle released\n"); }
		if (interaction & ui_interaction_right_released) { printf("right released\n"); }
		if (interaction & ui_interaction_left_clicked) { printf("left clicked\n"); }
		if (interaction & ui_interaction_middle_clicked) { printf("middle clicked\n"); }
		if (interaction & ui_interaction_right_clicked) { printf("right clicked\n"); }
		if (interaction & ui_interaction_left_dragging) { printf("left dragging\n"); }
		if (interaction & ui_interaction_middle_dragging) { printf("middle dragging\n"); }
		if (interaction & ui_interaction_right_dragging) { printf("right dragging\n"); }

		frame_stats_render();
		ui_end_frame();
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
	window = os_window_open(str("sora rendering test"), 1280, 960);
	renderer = gfx_renderer_create(window, color(0x303030ff), 8);

	// init
	app_init();

	// main loop
	while (os_any_window_exist()) {
		os_update();
		gfx_renderer_begin_frame(renderer);
		app_update_and_render();
		gfx_renderer_end_frame(renderer);
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