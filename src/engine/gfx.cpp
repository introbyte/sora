// gfx.cpp

#ifndef GFX_CPP
#define GFX_CPP

// per backend includes

#ifdef GFX_BACKEND_D3D11
#include "backends/gfx/gfx_d3d11.cpp"
#elif defined(GFX_BACKEND_OPENGL)
#include "backends/gfx/gfx_opengl.cpp"
#elif defined(GFX_BACKEND_METAL)
#include "backends/gfx/gfx_metal.cpp"
#endif 

#endif // GFX_CPP