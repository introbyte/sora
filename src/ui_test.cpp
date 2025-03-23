// ui_test.cpp

//- includes

// engine
#include "engine/core/base.h"
#include "engine/core/os.h"
#include "engine/core/gfx.h"
#include "engine/core/font.h"

#include "engine/core/base.cpp"
#include "engine/core/os.cpp"
#include "engine/core/gfx.cpp"
#include "engine/core/font.cpp"


#include "engine/utils/draw.h"
#include "engine/utils/ui.h"

#include "engine/utils/draw.cpp"
#include "engine/utils/ui.cpp"

//- globals

// contexts
global os_handle_t window;
global gfx_handle_t renderer;
global ui_context_t* context;
global b8 quit = false;

global arena_t* arena;

//- functions

function void app_init();
function void app_release();
function void app_frame();

function void widget_view();

//- implementation

// app

function void
app_init() {
    
	// open window and create renderer
	window = os_window_open(str("ui testing"), 1440, 960);
	renderer = gfx_renderer_create(window, color(0x131313ff));
	context = ui_context_create(window, renderer);
    arena = arena_create(megabytes(256));
    
	// set frame function
	os_window_set_frame_function(window, app_frame);
    
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
		gfx_renderer_begin(renderer);
		draw_begin(renderer);
		ui_begin(context);
        
        persist color_t col = color(0x985767ff);
        
        // color popup 
        ui_key_t color_popup_key = ui_key_from_string({ 0 }, str("color_popup"));
        if (ui_popup_begin(color_popup_key)) {
            
            ui_set_next_size(ui_size_pixels(200.0f), ui_size_pixels(200.0f));
            ui_color_sv_quad(str("color_sv_quad"), col.h, &col.s, &col.v);
            ui_spacer();
            ui_set_next_size(ui_size_pixels(200.0f), ui_size_pixels(20.0f));
            ui_color_hue_bar(str("color_hue_bar"), &col.h, col.s, col.v);
            
            ui_popup_end();
        }
        
        ui_push_size(ui_size_pixels(300.0f), ui_size_pixels(20.0f));
        
        // labels
        {
            ui_labelf("Hello, this is a label!");
            ui_spacer();
            ui_set_next_color_text(color(0xe28a8aff));
            ui_labelf("Colored label");
            ui_spacer();
        }
        
        // buttons
        {
            ui_buttonf("Button!");
            ui_spacer();
            
            ui_set_next_text_alignment(ui_text_alignment_center);
            ui_buttonf("Center aligned");
            ui_spacer();
            
            ui_set_next_text_alignment(ui_text_alignment_right);
            ui_buttonf("Right aligned");
            ui_spacer();
            
            ui_set_next_height(ui_size_pixels(100.0f));
            ui_set_next_color_background(color(0x452323ff));
            ui_set_next_text_alignment(ui_text_alignment_center);
            ui_buttonf("Large Red Button!");
            ui_spacer();
            
            if (ui_buttonf("Button with tooltip") & ui_interaction_hovered) {
                ui_tooltip_begin();
                ui_set_next_size(ui_size_text(2.0f), ui_size_text(2.0f));
                ui_labelf("This is a tooltip!");
                ui_tooltip_end();
            }
            ui_spacer();
        }
        
        // color widgets
        {
            
            ui_set_next_color_background(color_hsv_to_rgb(col));
            ui_key_t color_key = ui_key_from_string({ 0 }, str("color"));
            ui_frame_t* color_frame = ui_frame_from_key(ui_frame_flag_interactable | ui_frame_flag_draw_background,  color_key);
            ui_interaction color_interaction = ui_frame_interaction(color_frame);
            if (color_interaction & ui_interaction_left_clicked) {
                ui_popup_open(color_popup_key, context->mouse_pos);
            }
            
        }
        
        ui_pop_size();
        
        ui_end(context);
        draw_end(renderer);
        gfx_renderer_end(renderer);
    }
    
    
    // get close events
    if (os_event_get(os_event_type_window_close) != 0) {
        quit = true;
    }
}

//- entry point

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