// template_window.cpp

// includes

// engine
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
global os_handle_t window;
global gfx_handle_t renderer;
global ui_context_t* ui;
global b8 quit = false;

// functions

function void app_init();
function void app_release();
function void app_frame();

// implementation

function void
app_init() {
    
	// open window and create renderer
	window = os_window_open(str("app"), 1280, 960);
	renderer = gfx_renderer_create(window, color(0x000000ff));
	ui = ui_context_create(window, renderer);
    
	// set frame function
	os_window_set_frame_function(window, app_frame);
    
}

function void
app_release() {
	
	// release renderer and window
	ui_context_release(ui);
	gfx_renderer_release(renderer);
	os_window_close(window);
    
}

function void
app_frame() {
    
	// update layers
	os_update();
	gfx_update();
	ui_update();
    
	// hotkeys
	if (os_key_press(window, os_key_F11)) {
		os_window_fullscreen(window);
	}
    
    // close
	if (os_key_press(window, os_key_esc) || os_event_get(os_event_type_window_close) != 0) {
		quit = true;
	}
    
	// render
	if (!gfx_handle_equals(renderer, { 0 })) {
        
        uvec2_t renderer_size = gfx_renderer_get_size(renderer);
        
        
		gfx_renderer_begin(renderer);
		draw_begin(renderer);
		ui_begin(ui);
        
		ui_push_width(ui_size_pixels(200.0f, 1.0f));
		ui_push_height(ui_size_pixels(26.0f, 1.0f));
		ui_push_font_size(12.0f);
		ui_push_text_alignment(ui_text_alignment_center);
        
		if (ui_buttonf("button") & ui_interaction_left_clicked) {
			printf("clicked!\n");
		}
        
		draw_push_color0(color(1.0f, 0.0f, 0.0f, 1.0f));
		draw_push_color1(color(0.0f, 1.0f, 0.0f, 1.0f));
		draw_push_color2(color(0.0f, 0.0f, 1.0f, 1.0f));
		draw_push_color3(color(1.0f, 1.0f, 1.0f, 1.0f));
		draw_rect(rect(100.0f, 100.0f, (f32)renderer_size.x - 100.0f, (f32)renderer_size.y - 100.0f));
        
		ui_end(ui);
		draw_end(renderer);
		gfx_renderer_end(renderer);
	}
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
    
	// init
	app_init();
    
	// main loop
	while (!quit) {
		app_frame();
	}
    
	// release
	app_release();
    
	// release layers
	ui_release();
	draw_release();
	font_release();
	gfx_release();
	os_release();
    
	printf("[info] successfully closed.\n");
    
	return 0;
}