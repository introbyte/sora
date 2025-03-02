
//- includes

#define GFX_BACKEND_VULKAN 1

#include "engine/base.h"
#include "engine/os.h"
#include "engine/gfx.h"

#include "engine/base.cpp"
#include "engine/os.cpp"
#include "engine/gfx.cpp"

//- globals 

// context
global os_handle_t window;
global gfx_handle_t renderer;
global b8 quit = false;

//- functions 

// app
function void app_init();
function void app_release();
function void app_frame();

//- app functions

function void
app_init() {
    
    window = os_window_open(str("rendering test"), 1440, 960);
    renderer = gfx_renderer_create(window, color(0x500000ff));
    
    // set frame function
    os_window_set_frame_function(window, app_frame);
    
}

function void
app_release() {
    gfx_renderer_release(renderer);
    os_window_close(window);
}

function void 
app_frame() {
    
    // update layers
    os_update();
    
    // quit events
    if (os_key_press(window, os_key_esc) || os_event_get(os_event_type_window_close) != 0) {
        quit = true;
    }
    
    // fullscreen
    if (os_key_press(window, os_key_F11)) {
        os_window_fullscreen(window);
    }
    
    // render
    if (!gfx_handle_equals(renderer, { 0 })) {
        gfx_renderer_begin(renderer);
        
        
        
        gfx_renderer_end(renderer);
    }
    
}

//- app entry point 

function i32
app_entry_point(i32 argc, char** argv) {
    
    // init layers
    os_init();
    gfx_init();
    
    // init app
    app_init();
    
    // main loop
    while (!quit) {
        app_frame();
    }
    
    // release app
    app_release();
    
    // release layers
    gfx_release();
    os_release();
    
    return 0;
    
}