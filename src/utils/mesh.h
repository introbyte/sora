// mesh.h

#ifndef MESH_H
#define MESH_H

// structs

struct vertex_t {
	vec3_t pos;
	vec3_t normal;
	vec3_t tangent;
	vec3_t bitangent;
	vec2_t uv;
	vec4_t color;
};

struct mesh_t {
	u32 vertex_count;
	u32 vertex_size;
	void* vertices;
};

// functions
function mesh_t* mesh_create(arena_t*, str_t, u32, u32);
function mesh_t* mesh_load(arena_t*, str_t);
function void mesh_release(mesh_t*);

function void mesh_calculate_normals(mesh_t*);
function void mesh_calculate_binormals(mesh_t*);

#endif // MESH_H