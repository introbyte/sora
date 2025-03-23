// main.cpp

//- includes

// core
#include "engine/core/base.h"
#include "engine/core/os.h"
#include "engine/core/audio.h"
#include "engine/core/gfx.h"
#include "engine/core/font.h"

#include "engine/core/base.cpp"
#include "engine/core/os.cpp"
#include "engine/core/audio.cpp"
#include "engine/core/gfx.cpp"
#include "engine/core/font.cpp"

// utils
#include "engine/utils/log.h"
#include "engine/utils/draw.h"
#include "engine/utils/ui.h"

#include "engine/utils/log.cpp"
#include "engine/utils/draw.cpp"
#include "engine/utils/ui.cpp"

// projects
#include "projects/mod_syn/graph.h"
#include "projects/mod_syn/graph.cpp"

#include "projects/mod_syn/nodes/output_node.h"
#include "projects/mod_syn/nodes/osc_node.h"
#include "projects/mod_syn/nodes/reverb_node.h"


//- globals

global os_handle_t window;
global gfx_handle_t renderer;
global ui_context_t* ui;
global b8 quit = false;

global f32 font_size;
global color_t accent_color;

global u32 audio_buffer_index;

global graph_t* graph;
global graph_node_t* output_node;

//- functions 

function void app_init();
function void app_release();
function void app_frame();

function void audio_callback_func(f32* buffer, u32 count);

//- implementation

function void
app_init() {
    
	// open window and create renderer
	window = os_window_open(str("app"), 1280, 960);
	renderer = gfx_renderer_create(window, color(0x131316ff));
	ui = ui_context_create(window, renderer);
    
    // set default ui colors
    ui->color_background_default_node.v = color(0x404049ff);
    ui->color_text_default_node.v = color(0xe2e2e9ff);
    
    // set frame function
    os_window_set_frame_function(window, app_frame);
    
    font_size = 9.0f;
    
    accent_color = color(0x3BD3B7ff);
    
    // graph
    graph = graph_create(window);
    output_node = graph_output_node_create(graph, vec2(550.0f, 300.0f));
    graph_osc_node_create(graph, vec2(250.0f, 150.0f));
    graph_reverb_node_create(graph, vec2(450.0f, 200.0f));
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
    
    if (os_key_is_down(os_key_ctrl) && os_window_is_active(window)) {
        if (os_event_get(os_event_type_mouse_scroll)) {
            font_size += os_mouse_scroll(window) * 0.25f;
            font_reset();
        }
    }
    
    // render
    if (!gfx_handle_equals(renderer, { 0 })) {
        
        uvec2_t renderer_size = gfx_renderer_get_size(renderer);
        
        gfx_renderer_begin(renderer);
		draw_begin(renderer);
		ui_begin(ui);
        ui_push_font_size(font_size);
        
        graph_render(graph);
        
        ui_pop_font_size();
        ui_end(ui);
        draw_end(renderer);
		gfx_renderer_end(renderer);
    }
    
}

function void 
audio_callback_func(f32* buffer, u32 count) {
    
    if (output_node == nullptr) {return; }
    
    for (i32 i = 0; i < count; i++) {
        
        graph_port_t* output_port = graph_port_find(output_node, str("input"));
        
        if (output_port != nullptr) {
            graph_data_t* audio_buffer = output_port->data; 
            
            buffer[i * 2 + 0] = audio_buffer->buffer[audio_buffer_index++];
            buffer[i * 2 + 1] = audio_buffer->buffer[audio_buffer_index++];
            
            if (audio_buffer_index == graph_buffer_size) {
                graph_process(graph);
                audio_buffer_index = 0;
            }
        }
        
    }
    
}

//- entry point 

function i32 
app_entry_point(i32 argc, char** argv) {
    
    // init layers
    os_init();
    audio_init({44100, audio_format_f32, 2, 256, audio_callback_func});
    gfx_init();
    font_init();
    draw_init();
    ui_init();
    log_init();
    
    // init
    app_init();
    
    // main loop
    while (!quit) {
        app_frame();
    }
    
    // release
    app_release();
    
    // release layers
    log_release();
    ui_release();
    draw_release();
    font_release();
    gfx_release();
    os_release();
    
    return 0;
}