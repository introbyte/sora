// sim.cpp

//- includes

#include "engine/core/base.h"
#include "engine/core/os.h"
#include "engine/core/gfx.h"
#include "engine/core/font.h"

#include "engine/core/base.cpp"
#include "engine/core/os.cpp"
#include "engine/core/gfx.cpp"
#include "engine/core/font.cpp"


#include "engine/utils/draw.h"
#include "engine/utils/draw_3d.h"
#include "engine/utils/ui.h"
#include "engine/utils/render_graph.h"
#include "engine/utils/mesh.h"
#include "engine/utils/camera.h"

#include "engine/utils/draw.cpp"
#include "engine/utils/draw_3d.cpp"
#include "engine/utils/ui.cpp"
#include "engine/utils/render_graph.cpp"
#include "engine/utils/mesh.cpp"
#include "engine/utils/camera.cpp"

//- globals

// context
global os_handle_t window;
global gfx_handle_t renderer;
global ui_context_t* ui;
global b8 quit = false;
global arena_t* arena;

// viewport 
global b8 viewport_is_focused;
global ui_view_t* viewport;
global uvec2_t viewport_size;
global gfx_handle_t viewport_render_target;

// camera
global camera_t* camera;
global mat4_t view_proj;

// render graph
global rg_graph_t* render_graph;
global rg_pass_t* scene_pass;

//global ng_graph_t* node_graph;

// assets
global gfx_handle_t shader;
global mesh_t* mesh;

//- functions

// app
function void app_init();
function void app_release();
function void app_frame();

// ui views
function void viewport_view();
function void render_graph_view();
function void console_view();

// custom draw 
function void render_graph_background_draw(ui_frame_t* frame);

// scene pass
function void scene_pass_setup(rg_pass_t* pass);
function void scene_pass_execute(rg_pass_t* pass);

// ui pass
function void ui_pass_setup(rg_pass_t* pass);
function void ui_pass_execute(rg_pass_t* pass);

//- app functions

function void
app_init() {
    
    window = os_window_open(str("sim"), 1440, 960);
    renderer = gfx_renderer_create(window, color(0x181818ff));
    ui = ui_context_create(window, renderer);
    arena = arena_create(gigabytes(4));
    
    // set frame function
	os_window_set_frame_function(window, app_frame);
    
    // setup panel layout
    ui->panel_root->split_axis = ui_axis_y;
    ui_panel_t* top = ui_panel_create(ui, 0.75f, ui_axis_x);
    ui_panel_t* bottom = ui_panel_create(ui, 0.25f, ui_axis_x);
    ui_panel_t* left = ui_panel_create(ui, 0.6f, ui_axis_y);
    ui_panel_t* right = ui_panel_create(ui, 0.4f, ui_axis_y);
    ui_panel_insert(ui->panel_root, bottom);
    ui_panel_insert(ui->panel_root, top);
    ui_panel_insert(top, right);
    ui_panel_insert(top, left);
    
    // create views
    viewport = ui_view_create(ui, str("Viewport"), viewport_view);
    ui_view_t* properties = ui_view_create(ui, str("Render Graph"), render_graph_view);
    ui_view_t* console = ui_view_create(ui, str("Console"), console_view);
    ui_view_insert(left, viewport);
    ui_view_insert(right, properties);
    ui_view_insert(bottom, console);
    
    // create render graph
    render_graph = rg_graph_create(renderer);
    scene_pass = rg_pass_create(render_graph, str("scene_pass"), scene_pass_setup, scene_pass_execute);
    rg_pass_connect(render_graph, scene_pass, render_graph->pass_output);
    rg_graph_build(render_graph);
    
    // create node graph
    /*node_graph = ng_graph_create();
    for (rg_pass_list_node_t* node = render_graph->execute_last; node != nullptr; node = node->prev) {
        rg_pass_t* pass = node->pass;
        vec2_t pos = vec2(random_f32_range(50.0f, 200.0f), random_f32_range(50.0f, 400.0f));
        ng_node_create(node_graph, pass->label, pos);
    }*/
    
    // create camera
    //camera = camera_create(arena, window, renderer, 75.0f, 0.01f, 100.0f);
    
    // load assets
    gfx_shader_attribute_t attributes[] = {
        { "POSITION", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex },
        { "NORMAL", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex },
        { "TANGENT", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex },
        { "BITANGENT", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex },
        { "TEXCOORD", 0, gfx_vertex_format_float2, gfx_vertex_class_per_vertex },
        { "COLOR", 0, gfx_vertex_format_float4, gfx_vertex_class_per_vertex },
    };
    
    shader = gfx_shader_load(str("res/shaders/shader_3d.hlsl"), attributes, 6);
    
    mesh = mesh_load(arena, str("res/models/cube.obj"));
    
}

function void 
app_release() {
    
    rg_graph_release(render_graph);
    
    arena_release(arena);
    
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
        
        rg_graph_execute(render_graph);
        
        draw_begin(renderer);
        ui_begin(ui);
        ui_end(ui);
        draw_end(renderer);
        
		gfx_renderer_end(renderer);
    }
    
}

//- ui view functions 

function void 
render_graph_view() {
    
    ui_key_t context_menu_key = ui_key_from_stringf({ 0 }, "context_menu");
    if (ui_popup_begin(context_menu_key)) {
        ui_push_size(ui_size_pixels(100.0f), ui_size_pixels(20.0f));
        ui_push_color_border(color(0x00000000));
        ui_interaction interaction = 0;
        
        interaction |= ui_buttonf("new...");
        interaction |= ui_buttonf("edit...");
        interaction |= ui_buttonf("delete...");
        interaction |= ui_buttonf("copy");
        interaction |= ui_buttonf("cut");
        interaction |= ui_buttonf("paste");
        
        if (interaction & ui_interaction_left_clicked) {
            ui_popup_close();
        }
        
        ui_pop_color_border();
        ui_pop_size();
        ui_popup_end();
    }
    
    
    persist vec2_t view_offset = vec2(0.0f, 0.0f); 
    
    ui_set_next_size(ui_size_percent(1.0f), ui_size_percent(1.0f));
    ui_padding_begin(1.0f);
    
    ui_frame_flags background_flags = 
        ui_frame_flag_interactable |
        ui_frame_flag_draw_background | 
        ui_frame_flag_draw_clip;
    
    ui_set_next_size(ui_size_percent(1.0f), ui_size_percent(1.0f));
    ui_set_next_color_background(color(0x131313ff));
    ui_key_t background_key = ui_key_from_stringf({ 0 }, "background");
    ui_frame_t* background_frame = ui_frame_from_key(background_flags, background_key);
    background_frame->view_offset_target = view_offset;
    ui_frame_set_custom_draw(background_frame, render_graph_background_draw, nullptr);
    
    ui_push_parent(background_frame);
    ui_push_rounding(vec4(4.0f));
    
    /*for (ng_node_t* node = node_graph->node_first; node != nullptr; node = node->next) {
        
        // build container
        ui_frame_flags container_flags =
            ui_frame_flag_interactable |
            ui_frame_flag_draw_background | 
            ui_frame_flag_draw_border | 
            ui_frame_flag_draw_shadow;
        
        ui_set_next_fixed_x(node->pos.x);
        ui_set_next_fixed_y(node->pos.y);
        ui_set_next_size(ui_size_pixels(175.0f), ui_size_pixels(100.0f));
        ui_set_next_color_shadow(color(0x00000050));
        ui_set_next_color_border(color(0x383838ff));
        ui_set_next_shadow_size(8.0f);
        ui_key_t container_key = ui_key_from_stringf({0}, "node_%p", node);
        ui_frame_t* container_frame = ui_frame_from_key(container_flags, container_key);
        
        ui_push_parent(container_frame);
        
        // build title bar
        ui_frame_flags title_bar_flags = 
            ui_frame_flag_draw_background |
            ui_frame_flag_draw_text;
        
        color_t title_bar_color = ui_top_color_accent();
        title_bar_color.a = 0.25f;;
        
        ui_set_next_rounding_00(0.0f);
        ui_set_next_rounding_10(0.0f);
        ui_set_next_color_background(title_bar_color );
        ui_set_next_size(ui_size_percent(1.0f), ui_size_pixels(20.0f));
        ui_frame_t* title_bar_frame = ui_frame_from_string(title_bar_flags, { 0 });
        ui_frame_set_display_string(title_bar_frame, node->label);
        
        // build resources
        rg_pass_t* pass = rg_pass_find(render_graph, node->label);
        ui_push_size(ui_size_percent(1.0f), ui_size_pixels(20.0f));
        for (rg_resource_t* resource = pass->resource_first; resource != nullptr; resource = resource->next) {
            str_t resource_string = rg_resource_type_to_string(resource->type);
            ui_labelf("%.*s_%x", resource_string.size, resource_string.data, resource->key);
        }
        ui_pop_size();
        
        ui_pop_parent();
        
        ui_interaction container_interaction = ui_frame_interaction(container_frame);
        
        if (container_interaction & ui_interaction_left_pressed) {
            ng_node_bring_to_front(node_graph, node);
        }
        
        if (container_interaction & ui_interaction_left_dragging) {
            vec2_t mouse_delta = os_window_get_mouse_delta(window);
            node->pos.x += mouse_delta.x;
            node->pos.y += mouse_delta.y;
        }
        
    }*/
    
    ui_pop_rounding();
    ui_pop_parent();
    
    ui_interaction background_interaction = ui_frame_interaction(background_frame);
    
    if (background_interaction & ui_interaction_middle_dragging) {
        vec2_t mouse_delta = os_window_get_mouse_delta(window);
		view_offset.x -= mouse_delta.x;
		view_offset.y -= mouse_delta.y;
		os_set_cursor(os_cursor_resize_all);
    }
    
    if (background_interaction & ui_interaction_right_clicked) {
        vec2_t mouse_position = os_window_get_cursor_pos(window);
        ui_popup_open(context_menu_key, mouse_position);
    }
    
    ui_padding_end();
    
}

function void 
viewport_view() { 
    
    ui_set_next_size(ui_size_percent(1.0f), ui_size_percent(1.0f));
    ui_padding_begin(1.0f);
    
    // get texture from render target
    rg_resource_t* render_target_resource = rg_pass_get_resource_by_name(scene_pass, str("render_target0"));
    gfx_handle_t render_target = render_target_resource->render_target;
    gfx_handle_t texture = gfx_render_target_get_texture(render_target);
    
    // build viewport frame
    ui_set_next_texture(texture);
    ui_set_next_size(ui_size_percent(1.0f), ui_size_percent(1.0f));
    ui_set_next_color_background(color(0xffffffff));
    ui_key_t frame_key = ui_key_from_stringf({ 0 }, "viewport_frame");
    ui_frame_t* frame = ui_frame_from_key(ui_frame_flag_draw_background, frame_key);
    viewport_size = uvec2((u32)rect_width(frame->rect), (u32)rect_height(frame->rect));
    
    ui_padding_end();
}

function void 
console_view() {
    
    ui_set_next_size(ui_size_percent(1.0f), ui_size_percent(1.0f));
    ui_set_next_layout_dir(ui_dir_up);
    ui_padding_begin(8.0f);
    
    ui_set_next_size(ui_size_percent(1.0f), ui_size_percent(1.0f));
    ui_set_next_color_background(color(0x131313ff));
    ui_set_next_layout_dir(ui_dir_up);
    ui_frame_t* container_frame = ui_frame_from_key(ui_frame_flag_draw_background | ui_frame_flag_draw_clip, { 0 });
    
    ui_push_parent(container_frame);
    ui_push_size(ui_size_percent(1.0f), ui_size_pixels(20.0f));
    
    
    
    ui_pop_size();
    ui_pop_parent();
    
    
    ui_padding_end();
    
}

//- custom draw functions

function void
render_graph_background_draw(ui_frame_t* frame) {
    
    f32 offset_x = fmodf(frame->view_offset_prev.x, 25.0f);
	f32 offset_y = fmodf(frame->view_offset_prev.y, 25.0f);
	for (f32 x = frame->rect.x0 - offset_x; x < frame->rect.x1 - offset_x + 25.0f; x += 25.0f) {
		for (f32 y = frame->rect.y0 - offset_y; y < frame->rect.y1 - offset_y + 25.0f; y += 25.0f) {
			f32 radius = 1.25f;
			draw_set_next_color(color(0x303030ff));
			draw_circle(vec2(x, y), radius, 0.0f, 360.0f);
		}
	}
    
}




//- scene pass functions 

function void 
scene_pass_setup(rg_pass_t* pass) {
    
    rg_resource_t* resource = rg_pass_add_resource(pass, rg_resource_type_render_target, str("render_target0"));
    
    gfx_render_target_desc_t desc = { 0 };
    desc.size = uvec2(585, 691);
    desc.sample_count = 1;
    desc.flags = 0;
    desc.format = gfx_texture_format_rgba8;
    
    resource->render_target = gfx_render_target_create_ex(desc);
    
    if (!gfx_handle_equals(resource->render_target, { 0 })) {
        log_info("render target create successfully.");
    }
    
}

function void 
scene_pass_execute(rg_pass_t* pass) {
    
    // get resource
    rg_resource_t* resource = rg_pass_get_resource_by_name(pass, str("render_target0"));
    gfx_handle_t render_target = resource->render_target;
    
    // handle resize
    uvec2_t render_target_size = gfx_render_target_get_size(render_target);
    if (!uvec2_equals(viewport_size, render_target_size)) {
        gfx_render_target_resize(render_target, viewport_size);
    }
    
    // set view projection
    f32 et = os_window_get_elapsed_time(window);
    mat4_t view = mat4_translate(vec3(10.0f * sinf(et), 0.5f, 10.0f * cosf(et)));
    mat4_t proj = mat4_perspective(80.0f, (f32)viewport_size.x / (f32)viewport_size.y, 0.01f, 100.0f);
    view_proj = mat4_mul(proj, view);
    
    // set pipeline
    gfx_pipeline_t pipeline = gfx_pipeline_create();
    pipeline.viewport = rect(0.0f, 0.0f, (f32)viewport_size.x, (f32)viewport_size.y);
    pipeline.scissor = rect(0.0f, 0.0f, (f32)viewport_size.x, (f32)viewport_size.y);
    
    // draw
    gfx_render_target_clear(render_target, color(0x010509ff));
    gfx_set_render_target(render_target);
    draw_3d_begin();
    draw_3d_set_constants(&view_proj, sizeof(mat4_t), 0);
    
    draw_3d_set_next_pipeline(pipeline);
    draw_3d_set_next_shader(shader);
    draw_3d_push_mesh(mesh->vertices, mesh->vertex_size, mesh->vertex_count);
    
    draw_3d_end();
    gfx_set_render_target({ 0 });
    
}

//- entry point 

function i32
app_entry_point(i32 argc, char** argv) {
    
    // init layers
    os_init();
    gfx_init();
    font_init();
    draw_init();
    draw_3d_init();
    ui_init();
    
    // init app
    app_init();
    
    // main loop
    while (!quit) {
        app_frame();
    }
    
    // release app
    app_release();
    
    // release layers
    ui_release();
    draw_3d_release();
    draw_release();
    font_release();
    gfx_release();
    os_release();
    
    return 0;
    
}