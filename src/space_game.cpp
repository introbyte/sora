// space_game.cpp

//- includes 

#include "engine/base.h"
#include "engine/os.h"
#include "engine/gfx.h"
#include "engine/draw_3d.h"

#include "engine/base.cpp"
#include "engine/os.cpp"
#include "engine/gfx.cpp"
#include "engine/draw_3d.cpp"

#include "projects/render/render_graph.h"
#include "projects/utils/mesh.h"
#include "projects/utils/camera.h"

#include "projects/render/render_graph.cpp"
#include "projects/utils/mesh.cpp"
#include "projects/utils/camera.cpp"



//- globals 

global os_handle_t window;
global gfx_handle_t renderer;
global b8 quit = false;
global arena_t* arena;

global camera_t* camera;

global rg_graph_t* render_graph;
global rg_pass_t* scene_pass;

global gfx_handle_t shader;
global mesh_t* mesh;

//- functions

// app
function void app_init();
function void app_release();
function void app_frame();

// scene pass
function void scene_pass_setup(rg_pass_t* pass);
function void scene_pass_execute(rg_pass_t* pass);

//- implementation 

function void 
app_init() {
    
    window = os_window_open(str("space sim"), 1440, 960);
    renderer = gfx_renderer_create(window, color(0x000000ff));
    arena = arena_create(gigabytes(4));
    
    // set frame function
	os_window_set_frame_function(window, app_frame);
    
    camera = camera_create(arena, window, renderer, 75.0f, 0.01f, 100.0f);
    
    // create render graph
    render_graph = rg_graph_create(renderer);
    scene_pass = rg_pass_create(render_graph, str("scene_pass"), scene_pass_setup, scene_pass_execute);
    rg_pass_connect(render_graph, scene_pass, render_graph->pass_output);
    rg_graph_build(render_graph);
    
    
    gfx_shader_attribute_t attributes[] = {
        { "POSITION", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex },
        { "NORMAL", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex },
        { "TANGENT", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex },
        { "BITANGENT", 0, gfx_vertex_format_float3, gfx_vertex_class_per_vertex },
        { "TEXCOORD", 0, gfx_vertex_format_float2, gfx_vertex_class_per_vertex },
        { "COLOR", 0, gfx_vertex_format_float4, gfx_vertex_class_per_vertex },
    };
    
    shader = gfx_shader_load(str("res/shaders/shader_3d.hlsl"), attributes, 6);
    
    mesh = mesh_load(arena, str("res/models/hull.obj"));
    
    
}

function void
app_release() {
    
    camera_release(camera);
    
    gfx_renderer_release(renderer);
    os_window_close(window);
}

function void
app_frame() {
    os_update();
    
    // quit events
    if (os_key_press(window, os_key_esc) || os_event_get(os_event_type_window_close) != 0) {
		quit = true;
	}
    
    // fullscreen
	if (os_key_press(window, os_key_F11)) {
		os_window_fullscreen(window);
	}
    
    uvec2_t renderer_size = gfx_renderer_get_size(renderer);
    
    // update
    camera_update(camera, renderer_size);
    
    // render
	if (!gfx_handle_equals(renderer, { 0 })) {
        gfx_renderer_begin(renderer);
        
        rg_graph_execute(render_graph);
        
        // blit render target to screen
        rg_resource_t* resource = rg_pass_get_resource_by_name(scene_pass, str("render_target0"));
        gfx_handle_t render_target = resource->render_target;
        gfx_renderer_blit(renderer, gfx_render_target_get_texture(render_target));
        
        gfx_renderer_end(renderer);
    }
    
    
}



function void 
scene_pass_setup(rg_pass_t* pass) {
    
    rg_resource_t* resource = rg_pass_add_resource(pass, rg_resource_type_render_target, str("render_target0"));
    
    gfx_render_target_desc_t desc = { 0 };
    desc.size = gfx_renderer_get_size(renderer);
    desc.sample_count = 1;
    desc.flags = gfx_render_target_flag_depth;
    desc.format = gfx_texture_format_rgba8;
    
    resource->render_target = gfx_render_target_create_ex(desc);
    
}

function void 
scene_pass_execute(rg_pass_t* pass) {
    
    // get resource
    rg_resource_t* resource = rg_pass_get_resource_by_name(pass, str("render_target0"));
    gfx_handle_t render_target = resource->render_target;
    
    // handle resize
    uvec2_t renderer_size = gfx_renderer_get_size(renderer);
    uvec2_t render_target_size = gfx_render_target_get_size(render_target);
    if (!uvec2_equals(renderer_size, render_target_size)) {
        gfx_render_target_resize(render_target, renderer_size);
    }
    
    // set pipeline
    gfx_pipeline_t pipeline = gfx_pipeline_create();
    pipeline.viewport = rect(0.0f, 0.0f, (f32)renderer_size.x, (f32)renderer_size.y);
    pipeline.scissor = rect(0.0f, 0.0f, (f32)renderer_size.x, (f32)renderer_size.y);
    pipeline.cull_mode = gfx_cull_mode_front;
    pipeline.depth_mode = gfx_depth;
    
    gfx_render_target_clear(render_target, color(0x010509ff));
    gfx_set_render_target(render_target);
    draw_3d_begin();
    draw_3d_set_constants(&camera->constants, sizeof(camera_constants_t), 0);
    
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
    draw_3d_init();
    
    // init app
    app_init();
    
    // main loop
    while (!quit) {
        app_frame();
    }
    
    // release app
    app_release();
    
    // release layers
    draw_3d_release();
    gfx_release();
    os_release();
    
    return 0;
}