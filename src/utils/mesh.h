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
function mesh_t* mesh_load(arena_t*, arena_t*, str_t);
function void mesh_release(mesh_t*);

function void mesh_calculate_normals(mesh_t*);
function void mesh_calculate_binormals(mesh_t*);

// implementation


function mesh_t*
mesh_create(arena_t* arena, str_t name, u32 vertex_size, u32 vertex_count) {

	mesh_t* mesh = (mesh_t*)arena_alloc(arena, sizeof(mesh_t));

	// fill struct
	mesh->vertex_size = vertex_size;
	mesh->vertex_count = vertex_count;
	mesh->vertices = arena_alloc(arena, vertex_size * vertex_count);

	return mesh;
}

function mesh_t*
mesh_load(arena_t* arena, arena_t* scratch, str_t filepath) {

	str_t filename = str_get_file_name(filepath);

	str_t data = os_file_read_all(scratch, filepath);
	u8 splits[] = { '\n' };
	str_list_t lines = str_split(scratch, data, splits, 1);

	// count vertices
	u32 position_count = 0;
	u32 tex_coord_count = 0;
	u32 normal_count = 0;
	u32 face_count = 0;
	u32 vertex_count = 0;

	for (str_node_t* line_node = lines.first; line_node != 0; line_node = line_node->next) {
		str_t line = line_node->string;
		if (line.data[0] == 'v' && line.data[1] == ' ') { position_count++; }
		if (line.data[0] == 'v' && line.data[1] == 't') { tex_coord_count++; }
		if (line.data[0] == 'v' && line.data[1] == 'n') { normal_count++; }
		if (line.data[0] == 'f') { face_count++; }
	}

	mesh_t* mesh = mesh_create(arena, filename, sizeof(vertex_t), face_count * 3);
	vertex_t* vertices = (vertex_t*)mesh->vertices;

	vec3_t* positions = (vec3_t*)arena_alloc(scratch, sizeof(vec3_t) * position_count);
	vec2_t* tex_coords = (vec2_t*)arena_alloc(scratch, sizeof(vec2_t) * tex_coord_count);
	vec3_t* normals = (vec3_t*)arena_alloc(scratch, sizeof(vec3_t) * normal_count);

	position_count = 0;
	tex_coord_count = 0;
	normal_count = 0;

	for (str_node_t* line_node = lines.first; line_node != 0; line_node = line_node->next) {
		str_t line = line_node->string;

		if (line.data[0] == 'v') {

			if (line.data[1] == ' ') { // position
				f32 x, y, z;
				str_scan(line, "v %f %f %f", &x, &y, &z);
				positions[position_count++] = { x, y, z };
			} else if (line.data[1] == 't') { // tex coords
				f32 u, v;
				str_scan(line, "vt %f %f", &u, &v);
				tex_coords[tex_coord_count++] = { u, v };
			} else if (line.data[1] == 'n') { // normals
				f32 x, y, z;
				str_scan(line, "vn %f %f %f", &x, &y, &z);
				normals[normal_count++] = { x, y, z };
			}

		}

		// face
		if (line.data[0] == 'f') {
			i32 i0, i1, i2;
			i32 t0, t1, t2;
			i32 n0, n1, n2;

			str_scan(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
				&i0, &t0, &n0,
				&i1, &t1, &n1,
				&i2, &t2, &n2);

			vertices[vertex_count++] = { positions[i0 - 1], normals[n0 - 1], vec3(0.0f), vec3(0.0f), tex_coords[t0 - 1], color(0xffffffff).vec };
			vertices[vertex_count++] = { positions[i1 - 1], normals[n1 - 1], vec3(0.0f), vec3(0.0f), tex_coords[t1 - 1], color(0xffffffff).vec };
			vertices[vertex_count++] = { positions[i2 - 1], normals[n2 - 1], vec3(0.0f), vec3(0.0f), tex_coords[t2 - 1], color(0xffffffff).vec };
			face_count++;
		}
	}

	arena_clear(scratch);

	// calculate normals and binormals
	//mesh_calculate_normals(mesh);
	mesh_calculate_binormals(mesh);

	return mesh;
}

function void
mesh_release(mesh_t* mesh) {
	
}

function void
mesh_calculate_normals(mesh_t* mesh) {

	for (i32 i = 0; i < mesh->vertex_count; i += 3) {

		vertex_t* vertices = (vertex_t*)mesh->vertices;

		vertex_t* vertex0 = &vertices[i + 0];
		vertex_t* vertex1 = &vertices[i + 1];
		vertex_t* vertex2 = &vertices[i + 2];

		vec3_t edge0 = vec3_sub(vertex1->pos, vertex0->pos);
		vec3_t edge1 = vec3_sub(vertex2->pos, vertex0->pos);

		vec3_t normal = vec3_normalize(vec3_cross(edge0, edge1));

		vertex0->normal = normal;
		vertex1->normal = normal;
		vertex2->normal = normal;
	}

}

function void
mesh_calculate_binormals(mesh_t* mesh) {

	for (i32 i = 0; i < mesh->vertex_count; i += 3) {

		vertex_t* vertices = (vertex_t*)mesh->vertices;

		vertex_t* vertex0 = &vertices[i + 0];
		vertex_t* vertex1 = &vertices[i + 1];
		vertex_t* vertex2 = &vertices[i + 2];

		vec3_t edge0 = vec3_sub(vertex1->pos, vertex0->pos);
		vec3_t edge1 = vec3_sub(vertex2->pos, vertex0->pos);

		f32 deltaU1 = vertex1->uv.x - vertex0->uv.x;
		f32 deltaV1 = vertex1->uv.y - vertex0->uv.y;

		f32 deltaU2 = vertex2->uv.x - vertex0->uv.x;
		f32 deltaV2 = vertex2->uv.y - vertex0->uv.y;

		f32 dividend = (deltaU1 * deltaV2 - deltaU2 * deltaV1);
		f32 fc = 1.0f / dividend;

		vec3_t tangent = vec3(
			(fc * (deltaV2 * edge0.x - deltaV1 * edge1.x)),
			(fc * (deltaV2 * edge0.y - deltaV1 * edge1.y)),
			(fc * (deltaV2 * edge0.z - deltaV1 * edge1.z))
		);

		tangent = vec3_normalize(tangent);


		f32 sx = deltaU1, sy = deltaU2;
		f32 tx = deltaV1, ty = deltaV2;
		f32 handedness = ((tx * sy - ty * sx) < 0.0f) ? -1.0f : 1.0f;

		vec3_t t4 = vec3_mul(tangent, handedness);

		vec3_t bitangent = vec3_cross(tangent, vertex0->normal);

		vertex0->tangent = t4;
		vertex1->tangent = t4;
		vertex2->tangent = t4;

		vertex0->bitangent = bitangent;
		vertex1->bitangent = bitangent;
		vertex2->bitangent = bitangent;
	}

}

#endif // MESH_H