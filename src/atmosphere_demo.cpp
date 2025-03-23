// atmosphere_demo.cpp

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
#include "engine/utils/ui.h"
#include "engine/utils/draw.cpp"
#include "engine/utils/ui.cpp"

#include "engine/utils/camera.h"
#include "engine/utils/camera.cpp"

//- structs 

struct atmosphere_constants_t {
    mat4_t view_proj;
    mat4_t inv_view;
    mat4_t inv_proj;
    vec2_t window_size;
    
    vec4_t atmosphere_tint;
    vec4_t rayleigh_coeff;
    vec4_t mie_coeff;
    vec4_t ozone_coeff;
    
    f32 density;
    f32 exposure;
    f32 height;
    f32 unused;
};

//- globals 

global arena_t* arena;
global os_handle_t window;
global gfx_handle_t renderer;
global ui_context_t* ui;
global b8 quit = false;

global camera_t* camera;
global gfx_handle_t render_texture;
global gfx_handle_t compute_shader;
global gfx_handle_t blue_noise;

global atmosphere_constants_t atmosphere_constants;
global gfx_handle_t constant_buffer;

global b8 show_controls_panel;
global b8 show_settings_panel;
global b8 show_console_panel;

//- functions 

// app
function void app_init();
function void app_release();
function void app_frame();

//- app functions

function void
app_init() {
    
    // allocate arena
    arena = arena_create(megabytes(64));
    
    // create window and renderer
    window = os_window_open(str("atmosphere demo"), 1440, 960);
    renderer = gfx_renderer_create(window, color(0x000000000));
    ui = ui_context_create(window, renderer);
    os_window_set_frame_function(window, app_frame);
    
    // create assets
    render_texture = gfx_texture_create(uvec2(1440, 960), gfx_texture_format_rgba8, nullptr);
    blue_noise = gfx_texture_load(str("res/textures/blue_noise.png"));
    compute_shader = gfx_compute_shader_load(str("res/shaders/compute/atmosphere.hlsl"));
    camera = camera_create(arena, window, renderer, 80.0f, 0.01f, 100.0f);
    camera->target_position = vec3(-12.0f, 2.7f, -10.7f);
    camera->target_orientation = quat(-0.1f, -0.4f, -0.0f, 0.9f);
    
    // create constant buffer
    constant_buffer = gfx_buffer_create(gfx_buffer_type_constant, kilobytes(1), nullptr);
    
    atmosphere_constants.atmosphere_tint = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    atmosphere_constants.rayleigh_coeff = vec4(0.92f, 0.84f, 0.97f, 1.0f);
    atmosphere_constants.mie_coeff = vec4(0.036f, 0.024f, 0.027f, 1.0f);
    atmosphere_constants.density = 1.0f;
    atmosphere_constants.exposure = 15.0f;
    atmosphere_constants.height = 5.0f;
    
    // panels
    show_controls_panel = true;
    show_settings_panel = true;
    show_console_panel = true;
    
}

function void
app_release() {
    
    // release assets
    gfx_buffer_release(constant_buffer);
    camera_release(camera);
    gfx_compute_shader_release(compute_shader);
    gfx_texture_release(blue_noise);
    gfx_texture_release(render_texture);
    
    // release window and renderer
    gfx_renderer_release(renderer);
    os_window_close(window);
    
    // release arena
    arena_release(arena);
    
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
    
    // toggle controls panel
    if (os_key_press(window, os_key_C)) {
        show_controls_panel = !show_controls_panel;
    }
    
    // toggle settings panel
    if (os_key_press(window, os_key_V)) {
        show_settings_panel = !show_settings_panel;
    }
    
    // fullscreen
    if (os_key_press(window, os_key_F11)) {
        os_window_fullscreen(window);
    }
    
    // screen show
    if (os_key_press(window, os_key_F2)) {
        gfx_texture_write(render_texture, str("output.png"));
    }
    
    // update camera 
    uvec2_t texture_size = gfx_texture_get_size(render_texture);
    camera_update(camera, texture_size);
    
    // update constant buffer
    atmosphere_constants.view_proj = camera->constants.view_projection;
    atmosphere_constants.inv_view = camera->constants.inv_view;
    atmosphere_constants.inv_proj = camera->constants.inv_projection;
    atmosphere_constants.window_size = vec2((f32)texture_size.x, (f32)texture_size.y);
    gfx_buffer_fill(constant_buffer, &atmosphere_constants, sizeof(atmosphere_constants_t));
    
    // render
    if (!gfx_handle_equals(renderer, { 0 })) {
        
        uvec2_t renderer_size = gfx_renderer_get_size(renderer);
        
        // resize if needed
        if (!uvec2_equals(texture_size, renderer_size)) {
            gfx_texture_resize(render_texture, renderer_size);
        }
        
        // dispatch compute shader
        {
            gfx_set_compute_shader(compute_shader);
            gfx_set_texture(render_texture, 0, gfx_texture_usage_cs);
            gfx_set_texture(blue_noise, 1, gfx_texture_usage_cs);
            gfx_set_buffer(constant_buffer);
            gfx_dispatch(texture_size.x / 32 + 1, texture_size.y / 32 + 1, 1);
            gfx_set_texture({ 0 }, 0);
            gfx_set_texture({ 0 }, 1);
            gfx_set_compute_shader({ 0 });
        }
        
        // blit result to framebuffer
        gfx_renderer_begin(renderer);
        gfx_renderer_blit(renderer, render_texture);
        
        // draw ui
        draw_begin(renderer);
        ui_begin(ui);
        ui_push_color_border(color(0x424242ff));
        
        // settings panel
        if (show_settings_panel) {
            
            ui_set_next_fixed_x(5.0f);ui_set_next_fixed_y(5.0f);
            ui_set_next_size(ui_size_by_children(1.0f), ui_size_by_children(1.0f));
            ui_set_next_padding(4.0f);
            ui_frame_t* settings_panel = ui_frame_from_key(ui_frame_flag_draw_background | ui_frame_flag_draw_border, { 0 });
            
            ui_push_parent(settings_panel);
            ui_push_size(ui_size_pixels(250.0f), ui_size_pixels(20.0f));
            
            ui_set_next_text_alignment(ui_text_alignment_center);
            ui_labelf("settings");
            
            f32 dt = os_window_get_delta_time(window);
            ui_labelf("frame_time: %.1f ms (%.1f fps)", dt * 1000.0f, (1.0f / dt));
            ui_labelf("camera_pos: %.1f, %.1f, %.1f", camera->position.x, camera->position.y, camera->position.z);
            ui_labelf("camera_dir: %.1f, %.1f, %.1f, %.1f", camera->orientation.x, camera->orientation.y, camera->orientation.z, camera->orientation.w);
            
            ui_labelf("tint");
            ui_float3_edit(str("tint"), &atmosphere_constants.atmosphere_tint.xyz, 0.01f, 0.0f, 1.0f);
            
            ui_labelf("rayleigh");
            ui_float3_edit(str("rayleigh"), &atmosphere_constants.rayleigh_coeff.xyz);
            
            ui_labelf("mie");
            ui_float3_edit(str("mie"), &atmosphere_constants.mie_coeff.xyz);
            
            //ui_labelf("ozone");
            //ui_float3_edit(str("ozone"), &atmosphere_constants.ozone_coeff.xyz);
            
            ui_labelf("density");
            ui_float_edit(str("density"), &atmosphere_constants.density, 0.01f);
            
            ui_labelf("exposure");
            ui_float_edit(str("exposure"), &atmosphere_constants.exposure, 0.01f);
            
            ui_labelf("height");
            ui_float_edit(str("height"), &atmosphere_constants.height, 0.01f);
            
            ui_pop_size();
            ui_pop_parent();
        }
        
        // control panel
        if (show_controls_panel) {
            
            ui_set_next_fixed_x((f32)renderer_size.x - 255.0f);
            ui_set_next_fixed_y(5.0f);
            ui_set_next_size(ui_size_pixels(250.0f), ui_size_by_children(1.0f));
            ui_set_next_padding(4.0f);
            ui_frame_t* control_panel = ui_frame_from_key(ui_frame_flag_draw_background | ui_frame_flag_draw_border, { 0 });
            
            ui_push_parent(control_panel);
            
            ui_set_next_size(ui_size_percent(1.0f), ui_size_pixels(20.0f));
            ui_set_next_text_alignment(ui_text_alignment_center);
            ui_labelf("controls");
            ui_spacer();
            
            struct key_pair_t {
                str_t key_text;
                str_t action_text;
            };
            
            key_pair_t pairs[] = {
                {str("'right click'"),  str("rotate camera") },
                {str("'mouse wheel'"),  str("zoom in/out") },
                {str("'w + s'"),        str("move forward/backward") },
                {str("'a + d'"),        str("move left/right") },
                {str("'space + ctrl'"), str("move up/down") },
                {str("'q + e'"),        str("roll left/right") },
                {str("'shift'"),        str("increase speed") },
                {str("'c'"),            str("toggle controls panel") },
                {str("'v'"),            str("toggle settings panel") },
                {str("'f2'"),           str("screenshot") },
                {str("'f11'"),          str("toggle fullscreen") },
                {str("'esc'"),          str("close demo") },
            };
            
            for (i32 i = 0; i < array_count(pairs); i++) {
                
                ui_set_next_size(ui_size_percent(1.0f), ui_size_pixels(20.0f));
                ui_row_begin();
                
                // key binding text
                ui_set_next_color_background(color(0x202020ff));
                ui_set_next_color_border(color(0x252525ff));
                ui_set_next_size(ui_size_text(2.0f), ui_size_percent(1.0f));
                ui_set_next_flags(ui_frame_flag_draw_background | ui_frame_flag_draw_border);
                ui_label(pairs[i].key_text);
                
                ui_spacer(ui_size_percent(1.0f));
                
                // action text
                ui_set_next_size(ui_size_text(2.0f), ui_size_percent(1.0f));
                ui_label(pairs[i].action_text);
                
                ui_row_end();
                
                ui_spacer();
                
            }
            
            ui_pop_parent();
            
        }
        
        // console panel
        if (show_console_panel) {
            
            /*ui_set_next_fixed_x(5.0f);
            ui_set_next_fixed_y(renderer_size.y - 355.0f);
            ui_set_next_size(ui_size_pixels(750.0f), ui_size_pixels(350.0f));
            ui_set_next_layout_dir(ui_dir_up);
            
            ui_frame_t* console_frame = ui_frame_from_stringf(ui_frame_flag_draw_background, "console_frame");
            
            ui_push_parent(console_frame);
            
            ui_push_size(ui_size_percent(1.0f), ui_size_pixels(20.0f));
            
            for (log_t* log = log_state.log_first; log != 0; log = log->next) {
                ui_label(log->string);
            }
            ui_pop_size();
            
            ui_pop_parent();*/
            
        }
        
        
        ui_pop_color_border();
        ui_end(ui);
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
    draw_release();
    font_release();
    gfx_release();
    os_release();
    
    return 0;
    
}