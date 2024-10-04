// rendering_test_main.cpp

// includes

#include "base.h"
#include "os.h"
#include "gfx.h"

#include "base.cpp"
#include "os.cpp"
#include "gfx.cpp"

struct frame_stats_t {
	f32 min;
	f32 max;
	f32 avg;

	f32 frame_times[64];
	u32 frame_index;
	u32 frame_count;
};

function i32 
app_main(i32 argc, char** argv) {
	os_init();
	gfx_init();

	os_window_t* window = os_window_open(str("sora rendering test"), 1280, 960);
	gfx_renderer_t* renderer = gfx_renderer_create(window, color(0x303030ff), 1);

	arena_t* scratch = arena_create(megabytes(1));

	gfx_font_t* font_deja = gfx_font_load(str("res/fonts/deja_vu_sans.ttf"));
	gfx_font_t* font_deja_bold = gfx_font_load(str("res/fonts/deja_vu_sans_bo.ttf"));
	gfx_font_t* font_unicode = gfx_font_load(str("res/fonts/ms_ui_gothic.ttf"));
	gfx_font_t* font_system = gfx_font_load(str("res/fonts/segoe_ui.ttf"));

	frame_stats_t stats;
	stats.frame_index = 0;
	stats.frame_count = 0;

	u32 tick = 0;

	while (os_any_window_exist()) {
		os_update();
		gfx_renderer_begin_frame(renderer);

		// fullscreen
		if (os_key_release(window, os_key_F11)) {
			os_window_fullscreen(window);
		}

		// frame times
		stats.frame_times[stats.frame_index] = window->delta_time * 1000.0f;
		stats.frame_index = (stats.frame_index + 1) % 64;
		if (stats.frame_count < 64) {
			stats.frame_count++;
		}

		tick++;
		if (tick >= 144 / 4) {
			tick = 0;

			// reset stats
			stats.min = f32_max;
			stats.max = f32_min;
			stats.avg = 0.0f;

			// calculate frame stats
			for (i32 i = 0; i < stats.frame_count; i++) {

				f32 time = stats.frame_times[i];

				if (time < stats.min) { stats.min = time; }
				if (time > stats.max) { stats.max = time; }

				stats.avg += time;
			}
			stats.avg /= stats.frame_count;

		}

		// quad
		{
			rect_t layout = rect(0.0f, 100.0f, 100.0f, 200.0f);

			gfx_quad_params_t quad_params;
			quad_params.thickness = 0.0f;
			quad_params.softness = 0.33f;

			quad_params.col0 = quad_params.col1 = quad_params.col2 = quad_params.col3 = color(0xcececeff);
			quad_params.radii = { 5.0f, 5.0f, 5.0f, 5.0f };
			gfx_draw_quad(rect_shrink(layout, 5.0f), quad_params);
			layout = rect_translate(layout, vec2(100.0f, 0.0f));

			quad_params.col0 = color(0xff2525ff);
			quad_params.col1 = color(0x25ff25ff);
			quad_params.col2 = color(0x2525ffff);
			quad_params.col3 = color(0xffffffff);
			quad_params.radii = { 5.0f, 5.0f, 5.0f, 5.0f };
			gfx_draw_quad(rect_shrink(layout, 5.0f), quad_params);
			layout = rect_translate(layout, vec2(100.0f, 0.0f));

			quad_params.col0 = quad_params.col2 = color(0xef7867ff);
			quad_params.col1 = quad_params.col3 = color(0x563287ff);
			quad_params.radii = { 25.0f, 0.0f, 15.0f, 35.0f };
			gfx_draw_quad(rect_shrink(layout, 5.0f), quad_params);
			layout = rect_translate(layout, vec2(100.0f, 0.0f));

			quad_params.col0 = quad_params.col1 = quad_params.col2 = quad_params.col3 = color(0x23ff54ff);
			quad_params.radii = { 5.0f, 5.0f, 5.0f, 5.0f };
			quad_params.thickness = 3.0f;
			gfx_draw_quad(rect_shrink(layout, 5.0f), quad_params);
			layout = rect_translate(layout, vec2(100.0f, 0.0f));

			quad_params.col0 = quad_params.col1 = quad_params.col2 = quad_params.col3 = color(0x3535ffff);
			quad_params.radii = { 0.0f, 30.0f, 30.0f, 0.0f };
			quad_params.thickness = 10.0f;
			quad_params.softness = 5.0f;
			gfx_draw_quad(rect_shrink(layout, 5.0f), quad_params);
			layout = rect_translate(layout, vec2(100.0f, 0.0f));

		}

		// disk
		{

			vec2_t pos = vec2(50.0f, 270.0f);

			gfx_radial_params_t disk_params;
			disk_params.col0 = disk_params.col1 = disk_params.col2 = disk_params.col3 = color(0xe16e1cff);
			disk_params.thickness = 0.0f;
			disk_params.softness = 0.33f;
			gfx_draw_disk(pos, 45.0f, 0.0f, 360.0f, disk_params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

			disk_params.col0 = disk_params.col1 = disk_params.col2 = disk_params.col3 = color(0x803496ff);
			gfx_draw_disk(pos, 45.0f, 45.0f, 360.0f, disk_params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

			disk_params.col0 = disk_params.col1 = color(0x18ac70ff);
			disk_params.col2 = disk_params.col3 = color(0x184270ff);
			gfx_draw_disk(pos, 45.0f, 135.1f, 405.0f, disk_params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

			disk_params.col0 = disk_params.col1 = disk_params.col2 = disk_params.col3 = color(0x186d6dff);
			disk_params.thickness = 15.0f;
			gfx_draw_disk(pos, 45.0f, 0.0f, 360.0f, disk_params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

			disk_params.col0 = disk_params.col2 = color(0xd9be13ff);
			disk_params.col1 = disk_params.col3 = color(0xd92713ff);
			disk_params.thickness = 10.0f;
			disk_params.softness = 3.0f;
			gfx_draw_disk(pos, 45.0f, 90.0f, 360.0f, disk_params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

		}

		// line
		{

			vec2_t pos = vec2(5.0f, 330.0f);

			gfx_line_params_t params;
			params.col0 = params.col1 = color(0xffbf00ff);
			params.thickness = 1.0f;
			params.softness = 0.33f;
			gfx_draw_line(pos, vec2_add(pos, vec2(90.0f, 90.0f)), params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

			params.col0 = color(0xff5634ff);
			params.col1 = color(0x34ff83ff);
			params.thickness = 2.0f;
			gfx_draw_line(pos, vec2_add(pos, vec2(90.0f, 90.0f)), params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

			params.col0 = params.col1 = color(0xd15d84ff);
			params.thickness = 15.0f;
			gfx_draw_line(pos, vec2_add(pos, vec2(90.0f, 90.0f)), params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

			params.col0 = params.col1 = color(0xc70039ff);
			params.thickness = 2.0f;
			params.softness = 3.0f;
			gfx_draw_line(pos, vec2_add(pos, vec2(90.0f, 90.0f)), params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));


			params.col0 = color(0x56ff34ff);
			params.col1 = color(0x3483baff);
			params.thickness = 5.0f;
			params.softness = 0.33f;
			gfx_draw_line(pos, vec2_add(pos, vec2(90.0f, 90.0f)), params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

		}

		// triangles
		{
			vec2_t pos = vec2(5.0f, 430.0f);
			gfx_tri_params_t tri_params;
			tri_params.thickness = 0.0f;
			tri_params.softness = 0.33f;

			tri_params.col0 = tri_params.col1 = tri_params.col2 = color(0xde8763ff);
			gfx_draw_tri(vec2_add(pos, vec2(0.0f, 90.0f)), vec2_add(pos, vec2(45.0f, 0.0f)), vec2_add(pos, vec2(90.0f, 90.0f)), tri_params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

			tri_params.col0 = color(0xde3763ff);
			tri_params.col1 = color(0x37de63ff);
			tri_params.col2 = color(0x3763deff);
			gfx_draw_tri(vec2_add(pos, vec2(0.0f, 90.0f)), vec2_add(pos, vec2(45.0f, 0.0f)), vec2_add(pos, vec2(90.0f, 90.0f)), tri_params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

			tri_params.col1 = color(0x37de63ff);
			tri_params.col0 = tri_params.col2 = color(0x3763deff);
			gfx_draw_tri(vec2_add(pos, vec2(0.0f, 90.0f)), vec2_add(pos, vec2(45.0f, 0.0f)), vec2_add(pos, vec2(90.0f, 90.0f)), tri_params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

			tri_params.thickness = 5.0f;
			tri_params.col0 = tri_params.col1 = tri_params.col2 = color(0x37de63ff);
			gfx_draw_tri(vec2_add(pos, vec2(0.0f, 90.0f)), vec2_add(pos, vec2(45.0f, 0.0f)), vec2_add(pos, vec2(90.0f, 90.0f)), tri_params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

			tri_params.col0 = tri_params.col1 = tri_params.col2 = color(0xe6479aff);
			tri_params.softness = 1.0f;
			tri_params.thickness = 1.0f;
			gfx_draw_tri(vec2_add(pos, vec2(0.0f, 90.0f)), vec2_add(pos, vec2(45.0f, 0.0f)), vec2_add(pos, vec2(90.0f, 90.0f)), tri_params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));
		}

		// text
		{

			vec2_t pos = vec2(5.0f, 530.0f);

			gfx_text_params_t text_params;


			text_params.color = color(0xffffffff);
			text_params.font = font_deja;
			text_params.font_size = 12.0f;
			gfx_draw_text(str("Hello World!"), pos, text_params);
			pos = vec2_add(pos, vec2(0.0f, 20.0f));

			text_params.color = color(0xd6c837ff);
			text_params.font = font_deja_bold;
			text_params.font_size = 16.0f;
			gfx_draw_text(str("Hello World!"), pos, text_params);
			pos = vec2_add(pos, vec2(0.0f, 30.0f));

			text_params.color = color(0x56a523ff);
			text_params.font = font_unicode;
			text_params.font_size = 12.0f;
			u16 some_text[] = { 0x305D, 0x3089, 0x3048,  0x3093, 0x3058, 0x3093 };
			gfx_draw_text(str16(some_text, 6), pos, text_params);
			pos = vec2_add(pos, vec2(0.0f, 20.0f));

			text_params.color = color(0x959595ff);
			text_params.font = font_system;
			text_params.font_size = 9.0f;
			gfx_draw_text(str("system font"), pos, text_params);
		}

		// frame times
		{

			str_t text;
			vec2_t pos = vec2(5.0f, 5.0f);

			gfx_font_t* font = font_system;
			const f32 font_size = 9.0f;

			text = str_format(scratch, "frame_time: %.2f ms", window->delta_time * 1000.0f);
			gfx_draw_text(text, vec2_add(pos, 1.0f), gfx_text_params(color(0x15151588), font, font_size));
			gfx_draw_text(text, pos, gfx_text_params(color(0xcfcfcfff), font, font_size));
			pos = vec2_add(pos, vec2(0.0f, 18.0f));

			text = str_format(scratch, "min: %.2f ms", stats.min);
			gfx_draw_text(text, vec2_add(pos, 1.0f), gfx_text_params(color(0x15151588), font, font_size));
			gfx_draw_text(text, pos, gfx_text_params(color(0xcfcfcfff), font, font_size));
			pos = vec2_add(pos, vec2(0.0f, 18.0f));

			text = str_format(scratch, "max: %.2f ms", stats.max);
			gfx_draw_text(text, vec2_add(pos, 1.0f), gfx_text_params(color(0x15151588), font, font_size));
			gfx_draw_text(text, pos, gfx_text_params(color(0xcfcfcfff), font, font_size));
			pos = vec2_add(pos, vec2(0.0f, 18.0f));

			text = str_format(scratch, "avg: %.2f ms", stats.avg);
			gfx_draw_text(text, vec2_add(pos, 1.0f), gfx_text_params(color(0x15151588), font, font_size));
			gfx_draw_text(text, pos, gfx_text_params(color(0xcfcfcfff), font, font_size));
			pos = vec2_add(pos, vec2(0.0f, 18.0f));



			arena_clear(scratch);
		}

		gfx_renderer_end_frame(renderer);
	}

	gfx_renderer_release(renderer);
	os_release();

	return 0;
}

// entry point

#if defined(BUILD_DEBUG)
int main(int argc, char** argv) {
	return app_main(argc, argv);
}
#elif defined(BUILD_RELEASE)
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	return app_main(__argc, __argv);
}
#endif 