// ui_test.cpp

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
#include "engine/ui.h"
#include "projects/graphs/force_graph.h"

#include "engine/ui.cpp"
#include "projects/graphs/force_graph.cpp"

// file iter queue

struct file_iter_node_t {
    file_iter_node_t* next;
    file_iter_node_t* prev;
    
    str_t path;
    fg_node_t* node;
};

struct file_iter_queue_t {
    file_iter_node_t* first;
    file_iter_node_t* last;
};

function file_iter_node_t* 
file_iter_queue_push(arena_t* arena, file_iter_queue_t* queue, str_t path, fg_node_t* node) {
    
    file_iter_node_t* file_iter_node = (file_iter_node_t*)arena_alloc(arena, sizeof(file_iter_node_t));
    queue_push(queue->first, queue->last, file_iter_node);
    
    file_iter_node->path = path;
    file_iter_node->node = node;
    
    return file_iter_node;
}

function file_iter_node_t* 
file_iter_queue_pop(file_iter_queue_t* queue) {
    file_iter_node_t* first_node = queue->first;
    queue_pop(queue->first, queue->last);
    return first_node;
}

function b8
file_iter_queue_is_empty(file_iter_queue_t* queue) {
    return (queue->first == nullptr);
}

// file graph

struct file_graph_node_t {
    file_graph_node_t* next;
    file_graph_node_t* prev;
    file_graph_node_t* parent;
    
    str_t filepath;
    b8 is_folder;
    fg_node_t* fg_node;
};

struct file_graph_state_t {
    arena_t* arena;
    fg_state_t* fg_state;
    
    file_graph_node_t* first; 
    file_graph_node_t* last;
    file_graph_node_t* free; 
};

function file_graph_state_t* 
file_graph_create() {
    
    arena_t* arena = arena_create(megabytes(64));
    file_graph_state_t* state = (file_graph_state_t*)arena_alloc(arena, sizeof(file_graph_state_t));
    
    state->arena = arena;
    state->fg_state = fg_create();
    state->first = nullptr;
    state->last = nullptr;
    state->free = nullptr;
    
    return state;
}

function void 
file_graph_release(file_graph_state_t* state) {
    fg_release(state->fg_state);
    arena_release(state->arena);
}

function file_graph_node_t*
file_graph_node_create(file_graph_state_t* state, str_t filepath) {
    
    file_graph_node_t* node = state->free;
    if (node != nullptr) {
        stack_pop(state->free);
    } else {
        node = (file_graph_node_t*)arena_alloc(state->arena, sizeof(file_graph_node_t));
    }
    memset(node, 0, sizeof(file_graph_node_t));
    dll_push_back(state->first, state->last, node);
    
    node->filepath = filepath;
    
    return node;
}

function void 
file_graph_node_release(file_graph_state_t* state, file_graph_node_t* node) {
    fg_node_release(state->fg_state, node->fg_node);
    dll_remove(state->first, state->last, node);
    stack_push(state->free, node);
}

// globals

// contexts
global os_handle_t window;
global gfx_handle_t renderer;
global ui_context_t* context;
global b8 quit = false;

global arena_t* arena;
global fg_state_t* fg_state;

// functions

function void app_init();
function void app_release();
function void app_frame();
function void app_render();

function void widget_view();
function void console_view();
function void node_graph_view();
function void force_graph_view();
function void editor_view();

// app

function void
app_init() {
    
	// open window and create renderer
	window = os_window_open(str("new ui layout"), 1440, 960);
	renderer = gfx_renderer_create(window, color(0x131313ff));
	context = ui_context_create(window, renderer);
    arena = arena_create(megabytes(256));
    
	// set frame function
	os_window_set_frame_function(window, app_frame);
    
    // create panels
    ui_panel_t* top = ui_panel_create(context, 0.7f, ui_axis_x);
    ui_panel_t* bottom = ui_panel_create(context, 0.3f, ui_axis_x);
    ui_panel_t* left = ui_panel_create(context, 0.25f, ui_axis_y);
    ui_panel_t* right = ui_panel_create(context, 0.75f, ui_axis_y);
    
    context->panel_root->split_axis = ui_axis_y;
    ui_panel_insert(context->panel_root, top);
    ui_panel_insert(context->panel_root, bottom, top);
    ui_panel_insert(top, left);
    ui_panel_insert(top, right, left);
    
    // insert view
    ui_view_t* view0 = ui_view_create(context, str("Widgets"), widget_view);
    //ui_view_t* view1 = ui_view_create(context, str("Node Graph"), node_graph_view);
    ui_view_t* view2 = ui_view_create(context, str("Force Graph"), force_graph_view);
    //ui_view_t* view3 = ui_view_create(context, str("Editor"), editor_view);
    ui_view_t* view4 = ui_view_create(context, str("Console"), console_view);
    
    ui_view_insert(left, view0);
    //ui_view_insert(right, view1);
    ui_view_insert(right, view2);
    //ui_view_insert(right, view3);
    ui_view_insert(bottom, view4);
    
    // force graph
    fg_state = fg_create();
    
    // create file graph
    {
        temp_t scratch = scratch_begin();
        str_t filepath = str("src");
        file_iter_queue_t queue = { 0 };
        
        // start with root folder
        vec2_t pos = vec2(random_f32_range(50.0f, 500.0f), random_f32_range(50.0f, 500.0f));
        fg_node_t* node = fg_node_create(fg_state, pos, 15.0f, filepath.data);
        file_iter_queue_push(scratch.arena, &queue, filepath, node);
        
        while (!file_iter_queue_is_empty(&queue)) {
            
            // iterate through files
            os_handle_t file_iter = os_file_iter_begin(queue.first->path);
            for (os_file_info_t file_info = { 0 }; os_file_iter_next(arena, file_iter, &file_info);) {
                
                b8 is_folder = file_info.flags & os_file_flag_is_folder;
                fg_node_t* new_node = nullptr;
                
                // push new file iter to queue if folder
                if (is_folder) {
                    vec2_t pos = vec2(random_f32_range(50.0f, 500.0f), random_f32_range(50.0f, 500.0f));
                    new_node = fg_node_create(fg_state, pos, 11.0f, file_info.name.data);
                    str_t new_file_path = str_format(scratch.arena, "%.*s\\%.*s", queue.first->path.size, queue.first->path.data, file_info.name.size, file_info.name.data);
                    file_iter_queue_push(scratch.arena, &queue, new_file_path, new_node);
                } else {
                    vec2_t pos = vec2(random_f32_range(50.0f, 500.0f), random_f32_range(50.0f, 500.0f));
                    new_node = fg_node_create(fg_state, pos, 7.0f, file_info.name.data);
                }
                
                // link
                fg_link_create(fg_state, queue.first->node, new_node, is_folder ? 125.0f : 75.0f);
                
            }
            
            os_file_iter_end(file_iter);
            file_iter_queue_pop(&queue);
            
        }
        scratch_end(scratch);
    }
    
    
}

function void
app_release() {
	
    fg_release(fg_state);
    
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
    
    // update force graph
    f32 dt = os_window_get_delta_time(window);
    fg_update(fg_state, dt);
    
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
widget_view() {
    
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
    
    
    ui_labelf("mouse_pos_x: %.0f", context->mouse_pos.x);
    ui_labelf("mouse_pos_y: %.0f", context->mouse_pos.y);
    ui_labelf("panel_count: %u", context->panel_count);
    ui_labelf("drag_state: %u", context->drag_state);
    
    
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
    
    persist b8 checkbox_value = true;
    ui_checkbox(&checkbox_value, str("checkbox label"));
    ui_spacer();
    
    persist char buffer[256] = "Hello, World!";
    persist u32 size = 13;
    ui_text_edit(str("text_edit"), buffer, &size, 256);
    ui_spacer();
    
    persist f32 float_edit_value = 10.0f;
    ui_float_edit(str("float edit"), &float_edit_value);
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
    
    
    ui_pop_size();
    ui_padding_end();
    
}

function void
node_graph_view() {
    
    ui_set_next_size(ui_size_percent(1.0f), ui_size_percent(1.0f));
    ui_padding_begin(2.0f);
    
    ui_padding_end();
    
}

function void 
force_graph_view() {
    
    persist vec2_t view_offset = vec2(0.0f, 0.0f);
    
    ui_set_next_size(ui_size_percent(1.0f), ui_size_percent(1.0f));
    ui_padding_begin(2.0f);
    
    ui_frame_flags background_flags = 
        ui_frame_flag_interactable |
        ui_frame_flag_draw_background |
        ui_frame_flag_draw_clip |
        ui_frame_flag_view_scroll |
        ui_frame_flag_draw_custom;
    
    ui_set_next_size(ui_size_percent(1.0f), ui_size_percent(1.0f));
    ui_set_next_color_background(color(0x131313ff));
    ui_key_t background_key = ui_key_from_stringf({ 0 }, "force_graph_background");
    ui_frame_t* background_frame = ui_frame_from_key(background_flags, background_key);
    background_frame->view_offset = view_offset;
    ui_push_parent(background_frame);
    
	// draw nodes
	for (fg_node_t* node = fg_state->node_first; node != nullptr; node = node->next) {
        
		ui_frame_flags node_flags = 
			ui_frame_flag_interactable | 
			ui_frame_flag_draw_custom | 
			ui_frame_flag_floating;
		
		f32 node_size = node->size;
		ui_set_next_rect(rect(node->pos.x - node_size, node->pos.y - node_size, node->pos.x + node_size, node->pos.y + node_size));
		ui_key_t node_key = ui_key_from_stringf(background_key, "%p_frame", node);
		ui_frame_t* node_frame = ui_frame_from_key(node_flags, node_key);
		node->frame = node_frame;
		ui_frame_set_custom_draw(node_frame, fg_node_custom_draw, nullptr);
        
		ui_interaction node_interaction = ui_frame_interaction(node_frame);
        
		if (node_interaction & ui_interaction_left_dragging) {
			fg_state->node_active = node;
			vec2_t mouse_delta = os_window_get_mouse_delta(window);
			node->pos.x += mouse_delta.x;// - background_frame->rect.x0 + background_frame->view_offset.x;
			node->pos.y += mouse_delta.y;// - background_frame->rect.y0 + background_frame->view_offset.y;
		}
        
        if (node_interaction & ui_interaction_hovered) {
            ui_tooltip_begin();
            ui_set_next_size(ui_size_text(0.0f), ui_size_text(0.0f));
            char* name = (char*)node->data;
            ui_labelf("%s", name);
            ui_tooltip_end();
        }
        
	}
    
    // draw connections
	for (fg_link_t* link = fg_state->link_first; link != nullptr; link = link->next) {
        
		// calculate points
		vec2_t from = vec2(0.0f);
		vec2_t to = vec2(0.0f);
		if (link->from->frame != nullptr && link->to->frame != nullptr) {
			from = rect_center(link->from->frame->rect);
			to = rect_center(link->to->frame->rect);
		}
        
		// calculate bounding rect
		vec2_t points[2] = { from, to };
		rect_t link_rect = rect_grow(rect_bbox(points, 2), 2.0f);
        
		ui_set_next_rect(link_rect);
		ui_key_t node_key = ui_key_from_stringf(background_key, "%p_frame", link);
		ui_frame_t* link_frame = ui_frame_from_key(ui_frame_flag_draw_custom | ui_frame_flag_floating, node_key);
		fg_link_draw_data_t* data = (fg_link_draw_data_t*)arena_alloc(ui_build_arena(), sizeof(fg_link_draw_data_t));
		ui_frame_set_custom_draw(link_frame, fg_link_custom_draw, data);
        
		data->from = from;
		data->to = to;
	}
    
    
    ui_pop_parent();
    
    ui_interaction interaction = ui_frame_interaction(background_frame);
    if (interaction & ui_interaction_middle_dragging) {
        vec2_t mouse_delta = os_window_get_mouse_delta(window);
		view_offset.x -= mouse_delta.x;
		view_offset.y -= mouse_delta.y;
		os_set_cursor(os_cursor_resize_all);
    }
    
    ui_padding_end();
    
}

function void
console_view() {
    
    ui_set_next_size(ui_size_percent(1.0f), ui_size_percent(1.0f));
    ui_set_next_layout_dir(ui_dir_up);
    ui_padding_begin(8.0f);
    
    ui_set_next_size(ui_size_percent(1.0f), ui_size_pixels(20.0f));
    ui_buttonf("text box goes here...");
    ui_spacer(ui_size_pixels(8.0f));
    
    ui_set_next_size(ui_size_percent(1.0f), ui_size_percent(1.0f));
    ui_set_next_color_background(color(0x131313ff));
    ui_set_next_layout_dir(ui_dir_up);
    ui_frame_t* container_frame = ui_frame_from_key(ui_frame_flag_draw_background | ui_frame_flag_draw_clip, { 0 });
    
    ui_push_parent(container_frame);
    ui_push_size(ui_size_percent(1.0f), ui_size_pixels(20.0f));
    
    for (i32 i = 0; i < 4; i++) {
        ui_labelf("[info] this is an info message.");
    }
    
    ui_set_next_color_text(color(0xd35654ff));
    ui_labelf("[error] this is an error message!");
    
    ui_pop_size();
    ui_pop_parent();
    
    ui_padding_end();
    
}

function void
editor_view() {
    
    
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