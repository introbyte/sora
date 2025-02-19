//gfx_vulkan.cpp

#ifndef GFX_VULKAN_CPP
#define GFX_VULKAN_CPP

//- includes 

#pragma comment(lib, "vulkan-1.lib")

//- state functions

function void
gfx_init() {
    
    temp_t scratch = scratch_begin();
    
    // create arenas
	gfx_vk_state.resource_arena = arena_create(megabytes(64));
	gfx_vk_state.renderer_arena = arena_create(megabytes(64));
	
	// init resource list
	gfx_vk_state.resource_first = nullptr;
	gfx_vk_state.resource_last = nullptr;
	gfx_vk_state.resource_free = nullptr;
	
	// init renderer list
	gfx_vk_state.renderer_first = nullptr;
	gfx_vk_state.renderer_last = nullptr;
	gfx_vk_state.renderer_free = nullptr;
	gfx_vk_state.renderer_active = nullptr;
    
    // setup vulkan instance
    VkApplicationInfo app_info = { 0 };
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "sora engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;
    
    // get a list of extensions
    u32 extension_count = 0;
    cstr* extensions = gfx_vk_get_extensions(&extension_count);
    
    VkInstanceCreateInfo instance_create_info = { 0 };
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &app_info;
    instance_create_info.enabledExtensionCount = extension_count;
    instance_create_info.ppEnabledExtensionNames = extensions;
    instance_create_info.enabledLayerCount = 0;
    
    VkResult result = vkCreateInstance(&instance_create_info, nullptr, &gfx_vk_state.instance);
    
    if (result != VK_SUCCESS) {
        printf("[error] coult not craete vulkan instance!\n");
    }
    
    // supported extensions
    /*u32 extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    VkExtensionProperties* properties = (VkExtensionProperties*)arena_alloc(scratch.arena, sizeof(VkExtensionProperties) * extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, properties);
    printf("extensions: \n");
    for (i32 i = 0; i < extensionCount; i++) {
        printf("  %s\n", properties[i].extensionName);
    }*/
    
    // pick physical device
    u32 device_count = 0;
    vkEnumeratePhysicalDevices(gfx_vk_state.instance, &device_count, nullptr);
    VkPhysicalDevice* devices = (VkPhysicalDevice*)arena_alloc(scratch.arena, sizeof(VkPhysicalDevice) * device_count);
    vkEnumeratePhysicalDevices(gfx_vk_state.instance, &device_count, devices);
    
    for (i32 i = 0; i < device_count; i++) {
        
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(devices[i], & device_properties);
        
        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceFeatures(devices[i], &device_features);
        
    }
    
    gfx_vk_state.physical_device = devices[0];
    
    u32 queue_count = 0 ;
    vkGetPhysicalDeviceQueueFamilyProperties(gfx_vk_state.physical_device, &queue_count, nullptr);
    VkQueueFamilyProperties* queue_properties = (VkQueueFamilyProperties*)arena_alloc(scratch.arena, sizeof(VkQueueFamilyProperties) * queue_count);
    
    u32 family_index = 0;
    for (i32 i = 0; i < queue_count; i++) {
        if (queue_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            family_index = i;
        }
    }
    
    // create queue
    f32 queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_create_info = { 0 };
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = family_index;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    
    // create logical device
    VkPhysicalDeviceFeatures device_features = { 0 };
    VkDeviceCreateInfo device_create_info = { 0 };
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pQueueCreateInfos = &queue_create_info;
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pEnabledFeatures = &device_features;
    device_create_info.enabledLayerCount = 0;
    
    result = vkCreateDevice(gfx_vk_state.physical_device, &device_create_info, nullptr, &gfx_vk_state.device);
    if (result != VK_SUCCESS) {
        printf("[error] failed to create logical device!\n");
    }
    
    // get device queue
    vkGetDeviceQueue(gfx_vk_state.device, family_index, 0, &gfx_vk_state.graphics_queue);
    
    scratch_end(scratch);
}

function void
gfx_release() {
    
    arena_release(gfx_vk_state.resource_arena);
    arena_release(gfx_vk_state.renderer_arena);
    
    vkDestroyDevice(gfx_vk_state.device, nullptr);
    vkDestroyInstance(gfx_vk_state.instance, nullptr);
    
}

function void
gfx_update() {
    // TODO: remove
}


//- renderer 

function gfx_handle_t
gfx_renderer_create(os_handle_t window, color_t clear_color) {
    
    // get from resource pool or create one
	gfx_vk_renderer_t* renderer = gfx_vk_state.renderer_free;
	if (renderer != nullptr) {
		stack_pop(gfx_vk_state.renderer_free);
	} else {
		renderer = (gfx_vk_renderer_t*)arena_alloc(gfx_vk_state.renderer_arena, sizeof(gfx_vk_renderer_t));
	}
	memset(renderer, 0, sizeof(gfx_vk_renderer_t));
	dll_push_back(gfx_vk_state.renderer_first, gfx_vk_state.renderer_last, renderer);
    
    renderer->window = window;
	renderer->clear_color = clear_color;
	renderer->resolution = os_window_get_size(window);
    
    gfx_vk_surface_create(renderer);
    
    vkGetDeviceQueue(gfx_vk_state.device, 0, 0, &renderer->present_queue);
    
    
    gfx_handle_t handle = { 0 };
    handle = { (u64)renderer };
    
    return handle;
}

function void 
gfx_renderer_release(gfx_handle_t renderer) {
    
    // get renderer
	gfx_vk_renderer_t* vk_renderer = (gfx_vk_renderer_t*)renderer.data[0];
    
    // release surface
    vkDestroySurfaceKHR(gfx_vk_state.instance, vk_renderer->surface, nullptr);
    
	// push to free stack
	dll_remove(gfx_vk_state.renderer_first, gfx_vk_state.renderer_last, vk_renderer);
	stack_push(gfx_vk_state.renderer_free, vk_renderer);
    
}

function void
gfx_renderer_begin(gfx_handle_t renderer) {
    
}

function void
gfx_renderer_end(gfx_handle_t renderer) {
    
}


//- vulkan specific functions 



function cstr*
gfx_vk_get_extensions(u32* count) {
    cstr* result = nullptr;
#if OS_BACKEND_WIN32
    result = gfx_vk_w32_get_extensions(count);
#endif
    return result;
}

function void
gfx_vk_surface_create(gfx_vk_renderer_t* renderer){
#if OS_BACKEND_WIN32
    gfx_vk_w32_surface_create(renderer);
#endif
}

// include os specifics
#if OS_BACKEND_WIN32
#include "vulkan/gfx_vulkan_win32.cpp"
#elif OS_BACKEND_MACOS
#include "vulkan/gfx_vulkan_macos.cpp"
#elif OS_BACKEND_LINUX
#include "vulkan/gfx_vulkan_linux.cpp"
#endif



#endif // GFX_VULKAN_CPP

