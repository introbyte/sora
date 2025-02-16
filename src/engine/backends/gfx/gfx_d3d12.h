// gfx_d3d12.h

#ifndef GFX_D3D12_H
#define GFX_D3D12_H

//- includes 

#include <d3d12.h>
#include <d3dcompiler.h>

//- defines 

//- enums 

//- structs 

struct gfx_d3d12_buffer_t {
    
};



struct gfx_d3d12_renderer_t {
    gfx_d3d12_renderer_t* next;
    gfx_d3d12_renderer_t* prev;
    
    // context
    os_handle_t window;
    color_t clear_color;
    uvec2_t resolution;
    
    // d3d12
    
    
};

struct gfx_d3d12_state_t {
    
};

//- globals 

global gfx_d3d12_state_t gfx_d3d12_state;

//- d3d12 specific functions 





#endif // GFX_D3D12_H