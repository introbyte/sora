// main.cpp

#include "base.h"
#include "os.h"
#include "gfx.h"

#include "base.cpp"
#include "os.cpp"
#include "gfx.cpp"

int main(int argc, char** argv) {

	os_init();
	gfx_init();

	os_window_t* window = os_window_open(str("sora rendering test"), 1280, 960);
	gfx_renderer_t* renderer = gfx_renderer_create(window, color(0x07090bff), 8);
	
	arena_t* scratch = arena_create(megabytes(1));

	gfx_font_t* font_deja = gfx_font_open(str("res/fonts/deja_vu_sans.ttf"));
	gfx_font_t* font_deja_bold = gfx_font_open(str("res/fonts/deja_vu_sans_bo.ttf"));
	gfx_font_t* font_unicode = gfx_font_open(str("res/fonts/ms_ui_gothic.ttf"));
	gfx_font_t* font_system = gfx_font_open(str("res/fonts/segoe_ui.ttf"));

	while (os_any_window_exist()) {
		os_update();
		gfx_renderer_begin_frame(renderer);

		// quad
		{ 
			rect_t layout = rect(0.0f, 20.0f, 100.0f, 120.0f);

			gfx_quad_params_t quad_params;
			quad_params.thickness = 0.0f;
			quad_params.softness = 0.33f;

			quad_params.col0 = quad_params.col1 = quad_params.col2 = quad_params.col3 = color(0xcececeff);
			quad_params.radii = { 5.0f, 5.0f, 5.0f, 5.0f };
			gfx_renderer_push_quad(renderer, rect_shrink(layout, 5.0f), quad_params);
			layout = rect_translate(layout, vec2(100.0f, 0.0f));

			quad_params.col0 = color(0xff2525ff);
			quad_params.col1 = color(0x25ff25ff);
			quad_params.col2 = color(0x2525ffff);
			quad_params.col3 = color(0xffffffff);
			quad_params.radii = { 5.0f, 5.0f, 5.0f, 5.0f };
			gfx_renderer_push_quad(renderer, rect_shrink(layout, 5.0f), quad_params);
			layout = rect_translate(layout, vec2(100.0f, 0.0f));

			quad_params.col0 = quad_params.col2 = color(0xef7867ff);
			quad_params.col1 = quad_params.col3 = color(0x563287ff);
			quad_params.radii = { 25.0f, 0.0f, 15.0f, 35.0f };
			gfx_renderer_push_quad(renderer, rect_shrink(layout, 5.0f), quad_params);
			layout = rect_translate(layout, vec2(100.0f, 0.0f));

			quad_params.col0 = quad_params.col1 = quad_params.col2 = quad_params.col3 = color(0x23ff54ff);
			quad_params.radii = { 5.0f, 5.0f, 5.0f, 5.0f };
			quad_params.thickness = 3.0f;
			gfx_renderer_push_quad(renderer, rect_shrink(layout, 5.0f), quad_params);
			layout = rect_translate(layout, vec2(100.0f, 0.0f));

			quad_params.col0 = quad_params.col1 = quad_params.col2 = quad_params.col3 = color(0x3535ffff);
			quad_params.radii = { 0.0f, 30.0f, 30.0f, 0.0f };
			quad_params.thickness = 10.0f;
			quad_params.softness = 5.0f;
			gfx_renderer_push_quad(renderer, rect_shrink(layout, 5.0f), quad_params);
			layout = rect_translate(layout, vec2(100.0f, 0.0f));
		}
		
		// disk
		{ 
			
			vec2_t pos = vec2(50.0f, 170.0f);

			gfx_disk_params_t disk_params;
			disk_params.col0 = disk_params.col1 = disk_params.col2 = disk_params.col3 = color(0xe16e1cff);
			disk_params.thickness = 0.0f;
			disk_params.softness = 0.33f;
			gfx_renderer_push_disk(renderer, pos, 45.0f, 0.0f, 360.0f, disk_params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

			disk_params.col0 = disk_params.col1 = disk_params.col2 = disk_params.col3 = color(0x803496ff);
			gfx_renderer_push_disk(renderer, pos, 45.0f, 45.0f, 360.0f, disk_params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));
			
			disk_params.col0 = disk_params.col1 = color(0x18ac70ff);
			disk_params.col2 = disk_params.col3 = color(0x184270ff);
			gfx_renderer_push_disk(renderer, pos, 45.0f, 135.1f, 405.0f, disk_params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

			disk_params.col0 = disk_params.col1 = disk_params.col2 = disk_params.col3 = color(0x186d6dff);
			disk_params.thickness = 15.0f;
			gfx_renderer_push_disk(renderer, pos, 45.0f, 0.0f, 360.0f, disk_params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));


			disk_params.col0 = disk_params.col2 = color(0xd9be13ff);
			disk_params.col1 = disk_params.col3 = color(0xd92713ff);
			disk_params.thickness = 10.0f;
			disk_params.softness = 3.0f;
			gfx_renderer_push_disk(renderer, pos, 45.0f, 90.0f, 360.0f, disk_params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

		}

		// line
		{

			vec2_t pos = vec2(5.0f, 230.0f);

			gfx_line_params_t params;
			params.col0 = params.col1 = color(0xffbf00ff);
			params.thickness = 1.0f;
			params.softness = 0.33f;
			gfx_renderer_push_line(renderer, pos, vec2_add(pos, vec2(90.0f, 90.0f)), params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

			params.col0 = color(0xff5634ff);
			params.col1 = color(0x34ff83ff);
			params.thickness = 2.0f;
			gfx_renderer_push_line(renderer, pos, vec2_add(pos, vec2(90.0f, 90.0f)), params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

			params.col0 = params.col1 = color(0xd15d84ff);
			params.thickness = 15.0f;
			gfx_renderer_push_line(renderer, pos, vec2_add(pos, vec2(90.0f, 90.0f)), params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

			params.col0 = params.col1 = color(0xc70039ff);
			params.thickness = 2.0f;
			params.softness = 3.0f;
			gfx_renderer_push_line(renderer, pos, vec2_add(pos, vec2(90.0f, 90.0f)), params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));


			params.col0 = color(0x56ff34ff);
			params.col1 = color(0x3483baff);
			params.thickness = 5.0f;
			params.softness = 0.33f;
			gfx_renderer_push_line(renderer, pos, vec2_add(pos, vec2(90.0f, 90.0f)), params);
			pos = vec2_add(pos, vec2(100.0f, 0.0f));

		}


		// text
		{ 

			vec2_t pos = vec2(5.0f, 330.0f);

			gfx_text_params_t text_params;
			
			
			text_params.color = color(0xffffffff);
			text_params.font = font_deja;
			text_params.font_size = 12.0f;
			gfx_renderer_push_text(renderer, str("Hello World!"), pos, text_params);
			pos = vec2_add(pos, vec2(0.0f, 20.0f));

			text_params.color = color(0xd6c837ff);
			text_params.font = font_deja_bold;
			text_params.font_size = 16.0f;
			gfx_renderer_push_text(renderer, str("Hello World!"), pos, text_params);
			pos = vec2_add(pos, vec2(0.0f, 30.0f));

			text_params.color = color(0x56a523ff);
			text_params.font = font_unicode;
			text_params.font_size = 12.0f;
			u16 some_text[] = { 0x305D, 0x3089, 0x3048,  0x3093, 0x3058, 0x3093 };
			gfx_renderer_push_text(renderer, str16(some_text, 6), pos, text_params);
			pos = vec2_add(pos, vec2(0.0f, 20.0f));

			text_params.color = color(0x959595ff);
			text_params.font = font_system;
			text_params.font_size = 9.0f;
			gfx_renderer_push_text(renderer, str("system font"), pos, text_params);
		}
		

		str_t frame_rate = str_format(scratch, "frame_time: %.2f ms", window->delta_time * 1000.0f);
		gfx_renderer_push_text(renderer, frame_rate, vec2(5.0f, 5.0f), gfx_text_params(color(0xabababff), font_system, 9.0f));

		gfx_renderer_end_frame(renderer);
		arena_clear(scratch);
	}

	gfx_renderer_release(renderer);
	os_release();

	return 0;
}