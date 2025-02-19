// gfx_vulkan_win32.cpp

#ifndef GFX_VULKAN_WIN32_CPP
#define GFX_VULKAN_WIN32_CPP

function cstr* 
gfx_vk_w32_get_extensions(u32* count) {
    
    persist cstr vk_required_extensions[] = {
        "VK_KHR_win32_surface",
    };
    
    *count = array_count(vk_required_extensions);
    
    return vk_required_extensions;
}

function void
gfx_vk_w32_surface_create(gfx_vk_renderer_t* renderer) {
    
    // get w32 window
    os_w32_window_t* w32_window = os_w32_window_from_handle(renderer->window);
    
    VkWin32SurfaceCreateInfoKHR create_info = { 0 };
    create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.hwnd = w32_window->handle;
    create_info.hinstance = GetModuleHandle(nullptr);
    
    VkResult result = vkCreateWin32SurfaceKHR(gfx_vk_state.instance, &create_info, nullptr, &renderer->surface);
    if (result != VK_SUCCESS) {
        printf("[error] failed to create surface!\n");
    }
    
}


#endif // GFX_VULKAN_WIN32_CPP