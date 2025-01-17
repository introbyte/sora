// new_layout.cpp

// includes

// engine
#include "engine/base.h"
#include "engine/os.h"
#include "engine/gfx.h"
#include "engine/font.h"
#include "engine/draw.h"

#include "engine/base.cpp"
#include "engine/os.cpp"
#include "engine/gfx.cpp"
#include "engine/font.cpp"
#include "engine/draw.cpp"

// project
#include "projects/ui/ui.h"
#include "projects/ui/ui.cpp"

// globals

// contexts
global os_handle_t window;
global gfx_handle_t renderer;
global ui_context_t* context;
global b8 quit = false;


// functions

function void app_init();
function void app_release();
function void app_frame();
function void app_render();

function void console_view();


// implementation

// app

function void
app_init() {
    
	// open window and create renderer
	window = os_window_open(str("new ui layout"), 1440, 960);
	renderer = gfx_renderer_create(window, color(0x000000ff));
	context = ui_context_create(window, renderer);
    
	// set frame function
	os_window_set_frame_function(window, app_frame);
    
    // create panels
    ui_panel_t* left = ui_panel_create(context, 1.0f, ui_axis_y);
    //ui_panel_t* right = ui_panel_create(context, 0.5f, ui_axis_y);
    ui_panel_insert(context->panel_root, left);
    //ui_panel_insert(context->panel_root, right, left);
    
    // insert view
    ui_view_t* view = ui_view_create(context, str("Tab"), console_view);
    ui_view_insert(left, view);
    
}

function void
app_release() {
	
	// release renderer and window
	ui_context_release(context);
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
    
	if (os_key_press(window, os_key_esc)) {
		quit = true;
	}
    
	// render
	if (!gfx_handle_equals(renderer, { 0 })) {
		temp_t scratch = scratch_begin();
        
		gfx_renderer_begin(renderer);
		draw_begin(renderer);
		ui_begin(context);
		app_render();
		ui_end(context);
		draw_end(renderer);
		gfx_renderer_end(renderer);
        
		scratch_end(scratch);
	}
    
	// get close events
	if (os_event_get(os_event_type_window_close) != 0) {
		quit = true;
	}
}

function void 
app_render() {
	
    
    
}

function void
console_view() {
    
    ui_set_next_padding(ui_size_pixels(8.0f));
    ui_frame_t* padding_frame = ui_frame_from_key(0, { 0 });
    ui_push_parent(padding_frame);
    
    ui_push_size(ui_size_percent(1.0f), ui_size_pixels(20.0f, 1.0f));
    
    ui_labelf("Hello, This is a label!");
    ui_interaction interaction = ui_buttonf("This is a button");
    
    if (interaction & ui_interaction_hovered) {
        //ui_tooltip_begin();
        
        //ui_set_next_size(ui_size_text(2.0f), ui_size_text(2.0f));
        //ui_labelf("This is a tooltip");
        
        //ui_tooltip_end();
    }
    
    ui_pop_size();
    
    ui_pop_parent();
    
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