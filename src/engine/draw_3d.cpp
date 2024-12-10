// draw_3d.cpp

#ifndef DRAW_3D_CPP
#define DRAW_3D_CPP

// implementation

function void 
draw_3d_init() {

	// allocate arenas
	draw_3d_state.scratch_arena = arena_create(megabytes(64));
	draw_3d_state.batch_arena = arena_create(gigabytes(2));
	draw_3d_state.mesh_arena = arena_create(gigabytes(2));

	draw_3d_state.batch_first = nullptr;
	draw_3d_state.batch_last = nullptr;

	// create buffers
	draw_3d_state.vertex_buffer = gfx_buffer_create(gfx_buffer_type_vertex, sizeof(vertex_t) * draw_3d_max_buffer_size);
	draw_3d_state.constant_buffer = gfx_buffer_create(gfx_buffer_type_constant, kilobytes(1));

}

function void 
draw_3d_release() {

	// release arenas
	arena_release(draw_3d_state.scratch_arena);
	arena_release(draw_3d_state.batch_arena);
	arena_release(draw_3d_state.mesh_arena);

	// release buffers
	gfx_buffer_release(draw_3d_state.vertex_buffer);
	gfx_buffer_release(draw_3d_state.constant_buffer);
}

function void
draw_3d_begin() {

}

function void
draw_3d_end() {

}




#endif // DRAW_3D_CPP