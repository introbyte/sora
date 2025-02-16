// gfx_opengl.cpp

#ifndef GFX_OPENGL_CPP
#define GFX_OPENGL_CPP

//- includes 

//- opengl loader 

// win32 loader
#if defined(OS_BACKEND_WIN32)

#define GL_FUNCS \
X(glGetError, GLenum, (void))\
X(glViewport, void, (GLint x, GLint y, GLsizei width, GLsizei height))\
X(glScissor, void, (GLint x, GLint y, GLsizei width, GLsizei height))\
X(glClearColor, void, (GLfloat r,GLfloat g,GLfloat b,GLfloat a))\
X(glClear, void, (GLbitfield mask))\
X(glBlendFunc, void, (GLenum sfactor, GLenum dfactor))\
X(glBlendFuncSeparate, void, (GLenum srcRGB,GLenum dstRGB,GLenum srcAlpha,GLenum dstAlpha))\
X(glBlendEquation, void, (GLenum mode))\
X(glBlendEquationSeparate, void, (GLenum modeRGB, GLenum modeAlpha))\
X(glDisable, void, (GLenum cap))\
X(glEnable, void, (GLenum cap))\
X(glPixelStorei, void, (GLenum pname, GLint param))\
X(glReadPixels, void, (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels))\
X(glGenBuffers, void, (GLsizei n, GLuint *buffers))\
X(glDeleteBuffers, void, (GLsizei n, const GLuint *buffers))\
X(glBindBuffer, void, (GLenum target, GLuint buffer))\
X(glBufferData, void, (GLenum target, GLsizeiptr size, const void *data, GLenum usage))\
X(glBufferSubData, void, (GLenum target, GLintptr offset, GLsizeiptr size, const void *data))\
X(glGenVertexArrays, void, (GLsizei n, GLuint *arrays))\
X(glDeleteVertexArrays, void, (GLsizei n, const GLuint *arrays))\
X(glBindVertexArray, void, (GLuint array))\
X(glActiveTexture, void, (GLenum texture))\
X(glGenTextures, void, (GLsizei n, GLuint *textures))\
X(glDeleteTextures, void, (GLsizei n, const GLuint *textures))\
X(glBindTexture, void, (GLenum target, GLuint texture))\
X(glIsTexture, GLboolean, (GLuint texture))\
X(glTexParameteri, void, (GLenum target, GLenum pname, GLint param))\
X(glTexImage1D, void, (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels))\
X(glTexImage2D, void, (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels))\
X(glTexSubImage1D, void, (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels))\
X(glTexSubImage2D, void, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels))\
X(glTexImage2DMultisample, void, (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations))\
X(glAttachShader, void, (GLuint program, GLuint shader))\
X(glCompileShader, void, (GLuint shader))\
X(glCreateProgram, GLuint, (void))\
X(glCreateShader, GLuint, (GLenum type))\
X(glDeleteProgram, void, (GLuint program))\
X(glDeleteShader, void, (GLuint shader))\
X(glGetProgramiv, void, (GLuint program, GLenum pname, GLint *params))\
X(glGetProgramInfoLog, void, (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog))\
X(glGetShaderiv, void, (GLuint shader, GLenum pname, GLint *params))\
X(glGetShaderInfoLog, void, (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog))\
X(glLinkProgram, void, (GLuint program))\
X(glShaderSource, void, (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length))\
X(glUseProgram, void, (GLuint program))\
X(glDrawArrays, void, (GLenum mode, GLint first, GLsizei count))\
X(glDrawElements, void, (GLenum mode, GLsizei count, GLenum type, const void *indices))\
X(glDrawArraysInstanced, void, (GLenum mode, GLint first, GLsizei count, GLsizei instancecount))\
X(glDrawElementsInstanced, void, (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount))\
X(glEnableVertexAttribArray, void, (GLuint index))\
X(glDisableVertexAttribArray, void, (GLuint index))\
X(glVertexAttribPointer, void, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer))\
X(glVertexAttribIPointer, void, (GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer))\
X(glVertexAttribDivisor, void, (GLuint index, GLuint divisor))\
X(glGetUniformLocation, GLint, (GLuint program, const GLchar *name))\
X(glUniform1f, void, (GLint location, GLfloat v0))\
X(glUniform2f, void, (GLint location, GLfloat v0, GLfloat v1))\
X(glUniform3f, void, (GLint location, GLfloat v0, GLfloat v1, GLfloat v2))\
X(glUniform4f, void, (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3))\
X(glUniform1i, void, (GLint location, GLint v0))\
X(glUniform1fv, void, (GLint location, GLsizei count, const GLfloat *value))\
X(glUniform2fv, void, (GLint location, GLsizei count, const GLfloat *value))\
X(glUniform3fv, void, (GLint location, GLsizei count, const GLfloat *value))\
X(glDrawBuffers, void, (GLsizei n, const GLenum *bufs))\
X(glGenFramebuffers, void, (GLsizei n, GLuint *framebuffers))\
X(glDeleteFramebuffers, void, (GLsizei n, const GLuint *framebuffers))\
X(glBindFramebuffer, void, (GLenum target, GLuint framebuffer))\
X(glIsFramebuffer, GLboolean, (GLuint framebuffer))\
X(glCheckFramebufferStatus, GLenum, (GLenum target))\
X(glFramebufferTexture1D, void, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level))\
X(glFramebufferTexture2D, void, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level))\
X(glFramebufferTexture3D, void, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset))\
X(glFramebufferRenderbuffer, void, (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer))\
X(glFramebufferTexture, void, (GLenum target, GLenum attachment, GLuint texture, GLint level))\
X(glBlitFramebuffer, void, (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter))\
X(glGetFramebufferAttachmentParameteriv, void, (GLenum target, GLenum attachment, GLenum pname, GLint *params))\

#define WGL_FUNCS \
X(wglCreateContext,  HGLRC, (HDC dc))\
X(wglDeleteContext,  BOOL,  (HGLRC glrc))\
X(wglMakeCurrent,    BOOL,  (HDC dc,HGLRC glrc))\
X(wglGetProcAddress, PROC,  (LPCSTR name))\

#define WGL_EXT_FUNCS \
X(wglChoosePixelFormatARB, BOOL, (HDC dc,const int*atri,const FLOAT*atrf,UINT max_fmts,int*fmts,UINT *num_fmts))\
X(wglCreateContextAttribsARB, HGLRC, (HDC dc,HGLRC share,const int*atri))\

// generate opengl typedef functions
#define X(name, return, params) typedef return gl_##name params;
GL_FUNCS
#undef X

// generate opengl function pointers
#define X(name, return, params) global gl_##name* name = 0;
GL_FUNCS
#undef X

// generate wgl type functions
#define X(name, return, params) typedef return w32_##name params;
//WGL_FUNCS
WGL_EXT_FUNCS
#undef X

// generate wgl function pointers
#define X(name, return, parmas) global w32_##name* name = 0;
//WGL_FUNCS
WGL_EXT_FUNCS
#undef X

// macos
#elif defined(OS_BACKEND_MACOS) 

// linux
#elif defined(OS_BACKEND_LINUX)

#endif 


//- state functions 

function void
gfx_init() {
    
    // create arenas
    gfx_ogl_state.resource_arena = arena_create(megabytes(256));
    gfx_ogl_state.renderer_arena = arena_create(megabytes(4));
    
    // init resource list
    gfx_ogl_state.resource_first = nullptr;
    gfx_ogl_state.resource_last = nullptr;
    gfx_ogl_state.resource_free = nullptr;
    
    // init renderer list
    gfx_ogl_state.renderer_first = nullptr;
    gfx_ogl_state.renderer_last = nullptr;
    gfx_ogl_state.renderer_free = nullptr;
    
}

function void
gfx_release() {
    
    arena_release(gfx_ogl_state.resource_arena);
    arena_release(gfx_ogl_state.renderer_arena);
    
}

function void
gfx_update() {
    
}


//- renderer 

function gfx_handle_t
gfx_renderer_create(os_handle_t window, color_t clear_color) {
    gfx_handle_t handle = { 0 };
    return handle;
}

function void 
gfx_renderer_release(gfx_handle_t renderer) {
    
}

function void
gfx_renderer_resize(gfx_handle_t renderer, uvec2_t size) {
    
}

function void
gfx_renderer_begin(gfx_handle_t renderer) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

function void 
gfx_renderer_end(gfx_handle_t renderer) {
    
}




#endif // GFX_OPENGL_CPP
