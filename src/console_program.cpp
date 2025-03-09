// console_program.cpp

//- includes 
#include "engine/base.h"
#include "engine/profile.h"
#include "engine/os.h"
#include "engine/gfx.h"
#include "engine/font.h"
#include "engine/draw.h"
#include "engine/ui.h"

#include "engine/base.cpp"
#include "engine/profile.cpp"
#include "engine/os.cpp"
#include "engine/gfx.cpp"
#include "engine/font.cpp"
#include "engine/draw.cpp"
#include "engine/ui.cpp"

//- structs 

//- globals 

global os_handle_t window;
global gfx_handle_t renderer;
global ui_context_t* ui;
global arena_t* arena;
global b8 quit = false;

global u64 elapsed_time;

//- functions

// app
function void app_init();
function void app_release();
function void app_frame();

//- implementation 

function void 
app_init() {
    
    window = os_window_open(str("thread vis"), 1440, 960);
    renderer = gfx_renderer_create(window, color(0x181818ff));
    ui = ui_context_create(window, renderer);
    
    // set frame function
    os_window_set_frame_function(window, app_frame);
    
}

function void 
app_release() {
    
    ui_context_release(ui);
    gfx_renderer_release(renderer);
    os_window_close(window);
}

function void 
app_frame() {
    prof_scope("app_frame") {
        
        os_update();
        gfx_update();
        ui_update();
        
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
            draw_begin(renderer);
            ui_begin(ui);
            
            ui_push_size(ui_size_percent(1.0f), ui_size_pixels(20.0f));
            for (pf_entry_t* entry = pf_state.entry_first; entry != nullptr; entry = entry->next) {
                ui_labelf("%s : %u us", entry->name, entry->elapsed_time);
            }
            
            ui_end(ui);
            draw_end(renderer);
            gfx_renderer_end(renderer);
        }
        
    }
}

//- entry point 

function i32 
app_entry_point(i32 argc, char** argv) {
    
    // init layers
    os_init();
    pf_init();
    gfx_init();
    font_init();
    draw_init();
    ui_init();
    
    // run app
    app_init();
    
    while (!quit) {
        app_frame();
    }
    
    app_release();
    
    // release layers
    ui_release();
    draw_release();
    font_release();
    gfx_release();
    pf_release();
    os_release();
    
    return 0;
}