// app_template.cpp

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

// globals

global os_window_t* window;
global gfx_renderer_t* renderer;
global arena_t* resource_arena;
global arena_t* scratch_arena;

function void
app_init() {

	// allocate arenas
	resource_arena = arena_create(gigabytes(1));
	scratch_arena = arena_create(gigabytes(1));

}

function void
app_update() {

	// fullscreen
	if (os_key_release(window, os_key_F11)) {
		os_window_fullscreen(window);
	}

}

function void
main_pass(gfx_render_target_t* current_render_target, gfx_render_target_t* prev_rander_target) {

	ui_begin_frame(renderer);

	ui_push_pref_width(ui_size_pixel(200.0f, 1.0f));
	ui_push_pref_height(ui_size_pixel(20.0f, 1.0f));

	ui_button(str("button"));

	ui_end_frame();

}

function void
app_release() {

	// release arenas
	arena_release(resource_arena);
	arena_release(scratch_arena);

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
	window = os_window_open(str("app template"), 1280, 960);
	renderer = gfx_renderer_create(window, color(0x050505ff));

	// add ui pass
	gfx_render_target_desc_t render_target_desc = { 0 };
	render_target_desc.size = renderer->resolution;
	render_target_desc.sample_count = 1;
	render_target_desc.flags = gfx_render_target_flag_no_depth;
	render_target_desc.colorbuffer_format = gfx_texture_format_rgba8;
	gfx_renderer_add_pass(renderer, str("main_pass"), main_pass, render_target_desc);

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