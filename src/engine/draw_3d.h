// draw_3d.h

#ifndef DRAW_3D_H
#define DRAW_3D_H

#define draw_3d_max_buffer_size (2<<15)

// structs

struct draw_3d_constants_t {


};

struct vertex_t {
	vec3_t pos;
	vec3_t normal;
	vec3_t tangent;
	vec3_t bitangent;
	vec2_t uv;
	vec4_t color;
};

struct mesh_t {
	vertex_t* vertices;
	u32 vertex_count;
};

struct draw_3d_batch_t {
	draw_3d_batch_t* next;
	draw_3d_batch_t* prev;

	gfx_pipeline_t pipeline;
	gfx_shader_t* shader;
	gfx_texture_t* textures;
	u32 texture_count;

	vertex_t* vertices;
	u32 vertex_count;
};

struct draw_3d_state_t {

	arena_t* scratch_arena;

	gfx_buffer_t* vertex_buffer;
	gfx_buffer_t* constant_buffer;
	draw_3d_constants_t constants;

	// mesh
	arena_t* mesh_arena;

	// batches
	arena_t* batch_arena;
	draw_3d_batch_t* batch_first;
	draw_3d_batch_t* batch_last;

};

// globals

global draw_3d_state_t draw_3d_state;

// functions

function void draw_3d_init();
function void draw_3d_release();
function void draw_3d_begin();
function void draw_3d_end();

function void draw_push_mesh(mesh_t* mesh, mat4_t transform);

// mesh
function mesh_t mesh_create(str_t name, u32 vertex_count);
function mesh_t* mesh_load(str_t filepath);
function void mesh_release(mesh_t* mesh);
function void mesh_calculate_normals(mesh_t* mesh);
function void mesh_calculate_binormals(mesh_t* mesh);


#endif // DRAW_3D_H