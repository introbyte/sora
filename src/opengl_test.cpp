
//- includes

#define GFX_BACKEND_OPENGL 1

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

//- globals 

// context
global os_handle_t window;
global gfx_handle_t renderer;
global b8 quit = false;
global font_handle_t font;

//- functions 

// app
function void app_init();
function void app_release();
function void app_frame();

//- app functions

function void
app_init() {
    
    window = os_window_open(str("rendering test"), 1440, 960);
    renderer = gfx_renderer_create(window, color(0xcececeff));
    
    // set frame function
    os_window_set_frame_function(window, app_frame);
    
    // load font
    font = font_open(str("res/fonts/consola.ttf"));
    
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
    gfx_update();
    
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
        
        uvec2_t window_size = os_window_get_size(window);
        vec2_t window_center = vec2(window_size.x * 0.5f, window_size.y * 0.5f);
        vec2_t button_size = vec2(200.0f, 75.0f);
        rect_t button_rect = rect(window_center.x - button_size.x, window_center.y - button_size.y, window_center.x + button_size.x, window_center.y + button_size.y);
        f32 font_size = 36.0f;
        
        // shadow
        draw_set_next_color(color(0x00000080));
        draw_set_next_rounding(7.5f);
        draw_set_next_softness(15.0f);
        draw_rect(rect_translate(rect_grow(button_rect, 15.0f), 7.5f));
        
        // background
        draw_set_next_color0(color(0x6877FFff));
        draw_set_next_color1(color(0x3243DBff));
        draw_set_next_color2(color(0xAE62B2ff));
        draw_set_next_color3(color(0x4E2D8Eff));
        draw_set_next_rounding(15.0f);
        draw_set_next_softness(1.0f);
        draw_rect(button_rect);
        
        // border
        draw_set_next_color0(color(0x202D6Eff));
        draw_set_next_color1(color(0x202D8Eff));
        draw_set_next_color2(color(0x202D6Eff));
        draw_set_next_color3(color(0x202D8Eff));
        draw_set_next_rounding(15.0f);
        draw_set_next_softness(1.0f);
        draw_set_next_thickness(3.0f);
        draw_rect(button_rect);
        
        // text
        str_t font_text = str("Click Me!");
        vec2_t text_pos = font_align(font_text, font, font_size, button_rect);
        
        // text shadow
        draw_set_next_font(font);
        draw_set_next_font_size(font_size);
        draw_set_next_color(color(0x00000050));
        draw_text(font_text, vec2_add(text_pos, 3.0f));
        
        // text
        draw_set_next_font(font);
        draw_set_next_font_size(font_size);
        draw_text(font_text,text_pos);
        
        draw_end(renderer);
        gfx_renderer_end(renderer);
    }
    
}

//- app entry point 

function i32
app_entry_point(i32 argc, char** argv) {
    
    // init layers
    os_init();
    gfx_init();
    font_init();
    draw_init();
    
    // init app
    app_init();
    
    // main loop
    while (!quit) {
        app_frame();
    }
    
    // release app
    app_release();
    
    // release layers
    draw_release();
    font_release();
    gfx_release();
    os_release();
    
    return 0;
    
}