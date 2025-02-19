//gfx_vulkan.h

#ifndef GFX_VULKAN_H
#define GFX_VULKAN_H

//- includes 

#undef function
#include <vulkan/vulkan_core.h>
#define function static

//- structs 

struct gfx_vk_buffer_t {
    
};

struct gfx_vk_texture_t {
    
};

struct gfx_vk_shader_t {
    
};

struct gfx_vk_compute_shader_t {
    
};

struct gfx_vk_render_target_t {
    
};

// resource
struct gfx_vk_resource_t {
    gfx_vk_resource_t* next;
    gfx_vk_resource_t* prev;
    
    gfx_resource_type type;
    
    // resource descriptions
	union {
		gfx_buffer_desc_t buffer_desc;
		gfx_texture_desc_t texture_desc;
		gfx_shader_desc_t shader_desc;
		gfx_compute_shader_desc_t compute_shader_desc;
		gfx_render_target_desc_t render_target_desc;
	};
    
    // resource members
	union {
		gfx_vk_buffer_t buffer;
		gfx_vk_texture_t texture;
		gfx_vk_shader_t shader;
		gfx_vk_compute_shader_t compute_shader;
		gfx_vk_render_target_t render_target;
	};
    
};

// renderer
struct gfx_vk_renderer_t {
    gfx_vk_renderer_t* next;
    gfx_vk_renderer_t* prev;
    
    // context
	os_handle_t window;
	color_t clear_color;
	uvec2_t resolution;
    
    // vulkan
    VkSurfaceKHR surface;
    VkQueue present_queue;
    VkSwapchainKHR swapchain;
    
};

// state
struct gfx_vk_state_t {
    
    // arenas
	arena_t* renderer_arena;
	arena_t* resource_arena;
    
    // resources
	gfx_vk_resource_t* resource_first;
	gfx_vk_resource_t* resource_last;
	gfx_vk_resource_t* resource_free;
	
	// renderer
	gfx_vk_renderer_t* renderer_first;
	gfx_vk_renderer_t* renderer_last;
	gfx_vk_renderer_t* renderer_free;
	gfx_vk_renderer_t* renderer_active;
    
    // vulkan
    VkInstance instance;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkQueue graphics_queue;
    
};

//- globals 

global gfx_vk_state_t gfx_vk_state;

//- vullkan specific functions 

function cstr* gfx_vk_get_extensions(u32* count);
function void gfx_vk_surface_create(gfx_vk_renderer_t* renderer);

//- include os specifics 

#if OS_BACKEND_WIN32
#include <vulkan/vulkan_win32.h>
#include "vulkan/gfx_vulkan_win32.h"
#elif OS_BACKEND_MACOS
#include "vulkan/gfx_vulkan_macos.h"
#elif OS_BACKEND_LINUX
#include "vulkan/gfx_vulkan_linux.h"
#endif

#endif // GFX_VULKAN_H

