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

// struct

struct list_item_t {
    list_item_t* next;
    list_item_t* prev;
    
    str_t label;
};

struct list_state_t {
    arena_t* arena;
    
    list_item_t* first;
    list_item_t* last;
    list_item_t* free;
};

global list_state_t list_state;

function list_item_t* 
list_item_create(str_t label) {
    
    list_item_t* item = list_state.free;
    if (item != nullptr) {
        stack_pop(list_state.free, item);
    } else {
        item = (list_item_t*)arena_alloc(list_state.arena, sizeof(list_item_t));
    }
    memset(item, 0, sizeof(list_item_t));
    
    item->label = label;
    
    return item ;
}

function void 
list_item_release(list_item_t* item) {
    stack_push(list_state.free, item);
}

function void 
list_item_insert(list_item_t* item, list_item_t* prev = nullptr) {
    dll_insert(list_state.first,list_state.last, prev, item);
}

function void 
list_item_remove(list_item_t* item) {
    dll_remove(list_state.first, list_state.last, item);
}









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
	renderer = gfx_renderer_create(window, color(0x131313ff));
	context = ui_context_create(window, renderer);
    
	// set frame function
	os_window_set_frame_function(window, app_frame);
    
    // create panels
    ui_panel_t* left = ui_panel_create(context, 0.5f, ui_axis_y);
    ui_panel_t* right = ui_panel_create(context, 0.5f, ui_axis_y);
    ui_panel_insert(context->panel_root, left);
    ui_panel_insert(context->panel_root, right, left);
    
    // insert view
    ui_view_t* view = ui_view_create(context, str("Tab"), console_view);
    ui_view_insert(left, view);
    
    // list state
    list_state.arena = arena_create(megabytes(4));
    for (i32 i = 0; i < 6; i++) {
        str_t formatted_string = str_format(list_state.arena, "item %u", i);
        list_item_t* item = list_item_create(formatted_string);
        list_item_insert(item, list_state.last);
    }
    
    
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
    
    // build popup
    ui_key_t popup_key = ui_key_from_stringf({ 0 }, "test popup");
    if (ui_popup_begin(popup_key)) {
        ui_set_next_size(ui_size_text(0.0f), ui_size_text(0.0f));
        ui_labelf("This is a popup.");
        ui_popup_end();
    }
    
    ui_set_next_size(ui_size_percent(1.0f), ui_size_percent(1.0f));
    ui_padding_begin(8.0f);
    ui_push_size(ui_size_percent(1.0f), ui_size_pixels(20.0f, 1.0f));
    
    // label
    ui_labelf("Hello, This is a label!");
    ui_spacer();
    
    // button with tooltip
    ui_interaction button_interaction = ui_buttonf("This is a button");
    if (button_interaction & ui_interaction_hovered) {
        ui_tooltip_begin();
        ui_set_next_size(ui_size_text(0.0f), ui_size_text(0.0f));
        ui_labelf("This is a tooltip.");
        ui_tooltip_end();
    }
    
    ui_spacer();
    
    if (ui_buttonf("This button has a popup.") & ui_interaction_left_clicked) {
        ui_frame_t* button_frame = ui_last_frame();
        ui_key_t popup_key = ui_key_from_stringf({ 0 }, "test popup");
        ui_popup_open(popup_key, vec2(button_frame->rect.x0,  button_frame->rect.y1));
    }
    
    ui_spacer();
    
    persist f32 value = 0.5f;
    ui_slider(&value, 0.0f, 1.0f, str("slider"));
    
    ui_spacer();
    
    // expander
    if (ui_expander_begin(str("Expander"))) {
        
        ui_set_next_size(ui_size_by_children(1.0f), ui_size_by_children(1.0f));
        ui_padding_begin(4.0f);
        
        ui_labelf("the expander works!");
        ui_buttonf("test button 1");
        ui_buttonf("test button 2");
        ui_buttonf("test button 3");
        
        ui_padding_end();
        
        ui_expander_end();
    }
    
    ui_spacer();
    
    // redorable list
    for (list_item_t* item = list_state.first; item != nullptr; item = item->next) {
        
        ui_set_next_flags(ui_frame_flag_anim_pos_y);
        ui_interaction interaction = ui_button(item->label);
        
        if (interaction & ui_interaction_left_dragging) {
            
            
            
        }
        ui_spacer();
    }
    
    ui_pop_size();
    ui_padding_end();
    
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