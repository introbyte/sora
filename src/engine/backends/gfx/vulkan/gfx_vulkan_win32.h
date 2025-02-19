// gfx_vulkan_win32.h

#ifndef GFX_VULKAN_WIN32_H
#define GFX_VULKAN_WIN32_H

//- functions 

function cstr* gfx_vk_w32_get_extensions(u32* count);
function void gfx_vk_w32_surface_create(gfx_vk_renderer_t* renderer);

#endif // GFX_VULKAN_WIN32_H