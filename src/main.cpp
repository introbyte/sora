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

	os_window_t* window = os_window_open(str("title"), 1280, 960);
	gfx_renderer_t* renderer = gfx_renderer_create(window, color(0x07090bff), 8);
	

	while (os_any_window_exist()) {
		os_update();
		gfx_renderer_begin_frame(renderer);

		rect_t area = rect(0.0f, 0.0f, 100.0f, 100.0f);

		gfx_quad_params_t params;
		params.col0 = color(0xcececeff);
		params.col1 = color(0xcececeff);
		params.col2 = color(0xcececeff);
		params.col3 = color(0xcececeff);
		params.radii = { 5.0f, 5.0f, 5.0f, 5.0f };
		params.style = { 0.0f, 0.33f, 0.0f, 0.0f };
		gfx_renderer_push_quad(renderer, rect_shrink(area, 5.0f), params);
		area = rect_translate(area, vec2(100.0f, 0.0f));

		params.col0 = color(0xff3535ff);
		params.col1 = color(0x35ff35ff);
		params.col2 = color(0x3535ffff);
		params.col3 = color(0xffffffff);
		params.radii = { 5.0f, 5.0f, 5.0f, 5.0f };
		params.style = { 0.0f, 0.33f, 0.0f, 0.0f };
		gfx_renderer_push_quad(renderer, rect_shrink(area, 5.0f), params);
		area = rect_translate(area, vec2(100.0f, 0.0f));

		params.col0 = color(0xff35ffff);
		params.col1 = color(0x35ff35ff);
		params.col2 = color(0xff35ffff);
		params.col3 = color(0x35ff35ff);
		params.radii = { 25.0f, 0.0f, 15.0f, 35.0f };
		params.style = { 0.0f, 0.33f, 0.0f, 0.0f };
		gfx_renderer_push_quad(renderer, rect_shrink(area, 5.0f), params);
		area = rect_translate(area, vec2(100.0f, 0.0f));

		params.col0 = color(0xffceceff);
		params.col1 = color(0xffceceff);
		params.col2 = color(0xffceceff);
		params.col3 = color(0xffceceff);
		params.radii = { 5.0f, 5.0f, 5.0f, 5.0f };
		params.style = { 3.0f, 0.33f, 0.0f, 0.0f };
		gfx_renderer_push_quad(renderer, rect_shrink(area, 5.0f), params);
		area = rect_translate(area, vec2(100.0f, 0.0f));


		params.col0 = color(0x3535ffff);
		params.col1 = color(0x3535ffff);
		params.col2 = color(0x3535ffff);
		params.col3 = color(0x3535ffff);
		params.radii = { 0.0f, 30.0f, 30.0f, 0.0f };
		params.style = { 10.0f, 5.33f, 0.0f, 0.0f };
		gfx_renderer_push_quad(renderer, rect_shrink(area, 5.0f), params);
		area = rect_translate(area, vec2(100.0f, 0.0f));

		gfx_renderer_end_frame(renderer);
		
	}

	gfx_renderer_release(renderer);
	os_release();

	return 0;
}