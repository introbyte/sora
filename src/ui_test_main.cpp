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
		persist char buffer[128] = "Hello World this is a textbox";
		persist u32 string_size = 29;
		persist b8 basic_widgets_group = false;
		persist b8 color_picker_group = false;
		persist b8 scroll_group = false;
		persist b8 checkbox_value = false;
		persist f32 slider_1_value = 0.75f;
		persist color_t hsv_col = color(0.6f, 0.5f, 0.9f, 1.0f, color_format_hsv);
		
		ui_begin_frame(renderer);
		ui_push_pref_width(ui_size_pixel(300.0f, 1.0f));
		ui_push_pref_height(ui_size_pixel(21.0f, 1.0f));

		// frame stats
		{
			ui_labelf("frame_time: %.2f ms", frame_stats.dt);
			ui_labelf("min: %.2f ms", frame_stats.min);
			ui_labelf("max: %.2f ms", frame_stats.max);
			ui_labelf("avg: %.2f ms (fps: %.1f)", frame_stats.avg, 1000.0f / frame_stats.avg);
		}

		ui_text_edit(str("text_edit"), buffer, 128, &string_size);

		// basic widgets
		ui_expander(str("Basic Widgets"), &basic_widgets_group);
		if (basic_widgets_group) {
			ui_interaction interaction = ui_button(str("Button##1"));
			
			ui_set_next_text_alignment(ui_text_alignment_center);
			ui_button(str("Button##2"));

			ui_set_next_text_alignment(ui_text_alignment_right);
			ui_button(str("Button##3"));

			ui_checkbox(str("Checkbox"), &checkbox_value);

			ui_slider(str("Slider"), &slider_1_value, 0.0f, 1.0f);
		}

		ui_expander(str("Color Picker Widgets"), &color_picker_group);
		if (color_picker_group) {
			ui_set_next_pref_height(ui_size_pixel(200.0f, 1.0f));
			ui_color_wheel(str("color_wheel"), &hsv_col.h, &hsv_col.s, &hsv_col.v);

			ui_set_next_pref_height(ui_size_pixel(200.0f, 1.0f));
			ui_color_hue_sat_circle(str("color_circle"), &hsv_col.h, &hsv_col.s, hsv_col.v);

			ui_color_val_bar(str("color_hue_bar"), hsv_col.h, hsv_col.s, &hsv_col.v);

			ui_set_next_pref_height(ui_size_pixel(200.0f, 1.0f));
			ui_color_sat_val_quad(str("color_quad"), hsv_col.h, &hsv_col.s, &hsv_col.v);

			ui_color_hue_bar(str("color_val_bar"), &hsv_col.h, hsv_col.s, hsv_col.v);

			color_t rgb_col = color_hsv_to_rgb(hsv_col);

			ui_interaction interaction = ui_interaction_none; 
			ui_row_begin();
			ui_set_next_pref_width(ui_size_percent(0.25f));
			ui_label(str("red:")); 
			ui_set_next_pref_width(ui_size_percent(0.75f));
			interaction |= ui_slider(str("slider_red"), &rgb_col.r, 0.0f, 1.0f);
			ui_row_end();
			ui_row_begin();
			ui_set_next_pref_width(ui_size_percent(0.25f));
			ui_label(str("green:"));
			ui_set_next_pref_width(ui_size_percent(0.75f));
			interaction |= ui_slider(str("slider_green"), &rgb_col.g, 0.0f, 1.0f);
			ui_row_end();
			ui_row_begin();
			ui_set_next_pref_width(ui_size_percent(0.25f));
			ui_label(str("blue:"));
			ui_set_next_pref_width(ui_size_percent(0.75f));
			interaction |= ui_slider(str("slider_blue"), &rgb_col.b, 0.0f, 1.0f);
			ui_row_end();
		
			if (interaction & ui_interaction_left_dragging) {
				hsv_col = color_rgb_to_hsv(rgb_col);
			}

			ui_row_begin();
			ui_set_next_pref_width(ui_size_percent(0.25f));
			ui_label(str("hue:"));
			ui_set_next_pref_width(ui_size_percent(0.75f));
			ui_slider(str("slider_hue"), &hsv_col.h, 0.0f, 1.0f);
			ui_row_end();
			ui_row_begin();
			ui_set_next_pref_width(ui_size_percent(0.25f));
			ui_label(str("sat:"));
			ui_set_next_pref_width(ui_size_percent(0.75f));
			ui_slider(str("slider_sat"), &hsv_col.s, 0.0f, 1.0f);
			ui_row_end();
			ui_row_begin();
			ui_set_next_pref_width(ui_size_percent(0.25f));
			ui_label(str("val:"));
			ui_set_next_pref_width(ui_size_percent(0.75f));
			ui_slider(str("slider_val"), &hsv_col.v, 0.0f, 1.0f);
			ui_row_end();
		}


		ui_expander(str("Scroll Regions"), &scroll_group);
		if (scroll_group) {
			ui_set_next_pref_height(ui_size_pixel(210.0f, 1.0f));
			ui_frame_t* holder = ui_frame_from_string(str("scroll_holder"), ui_frame_flag_draw_background_dark | ui_frame_flag_view_scroll | ui_frame_flag_view_clamp | ui_frame_flag_clip);
			ui_interaction interaction = ui_frame_interaction(holder);
		
			ui_push_parent(holder);

			for (i32 i = 0; i < 25; i++) {
				ui_buttonf("Test Button %u", i);
			}

			ui_pop_parent();
		}

		
		ui_pop_pref_width();
		ui_pop_pref_height();

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
	window = os_window_open(str("sora ui test"), 1280, 960);
	renderer = gfx_renderer_create(window, { color(0x303030ff), 1 });

	// init
	app_init();

	// main loop
	while (os_any_window_exist()) {

		// update layers
		os_update();
		gfx_update();

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