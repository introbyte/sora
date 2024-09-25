// base.cpp

#ifndef BASE_CPP
#define BASE_CPP

// implementation

function arena_t*
arena_create(u32 size) {

	// roundup
	u32 size_roundup = megabytes(64);
	size += size_roundup - 1;
	size -= size % size_roundup;

	// reserve memory
	void* block = arena_impl_reserve(size);

	// initial commit
	u64 initial_commit_size = arena_commit_size;
	arena_impl_commit(block, initial_commit_size);

	// fill struct
	arena_t* arena = (arena_t*)block;
	arena->pos = sizeof(arena_t);
	arena->commit_pos = initial_commit_size;
	arena->align = 8;
	arena->size = size;

	return arena;
}

function void
arena_release(arena_t* arena) {
	arena_impl_release(arena, arena->size);
}

function void*
arena_malloc(arena_t* arena, u32 size) {

	void* result = nullptr;

	if (arena->pos + size <= arena->size) {

		u8* base = (u8*)arena;

		// align
		u32 post_align_pos = (arena->pos + (arena->align - 1));
		post_align_pos -= post_align_pos % arena->align;
		u32 align = post_align_pos - arena->pos;
		result = base + arena->pos + align;
		arena->pos += size + align;

		// commit
		if (arena->commit_pos < arena->pos) {
			u32 size_to_commit = arena->pos - arena->commit_pos;
			size_to_commit += arena_commit_size - 1;
			size_to_commit -= size_to_commit % arena_commit_size;
			arena_impl_commit(base + arena->commit_pos, size_to_commit);
			arena->commit_pos += size_to_commit;
		}

	} else {
		printf("[error] arena is full.\n");
	}

	return result;
}

function void*
arena_calloc(arena_t* arena, u32 size) {
	void* result = arena_malloc(arena, size);
	memset(result, 0, size);
	return result;
}

function void
arena_clear(arena_t* arena) {

	// set pos to min
	u32 min_pos = sizeof(arena_t);
	arena->pos = min_pos;

	// align pos
	u32 pos_aligned = arena->pos + arena_commit_size - 1;
	pos_aligned -= pos_aligned % arena_commit_size;

	// decommit
	if (pos_aligned + arena_decommit_size <= arena->commit_pos) {
		u8* base = (u8*)arena;
		u32 size_to_decommit = arena->commit_pos - pos_aligned;
		arena_impl_decommit(base + pos_aligned, size_to_decommit);
		arena->commit_pos -= size_to_decommit;
	}

}


// cstr functions

function u32
cstr_length(cstr cstr) {
	u32 i;
	for (i = 0; cstr[i] != '\0'; i++);
	return i;
}

function b8
cstr_equals(cstr cstr1, cstr cstr2) {
	while (*cstr1 && (*cstr1 == *cstr2)) {
		cstr1++;
		cstr2++;
	}
	return (*(const unsigned char*)cstr1 - *(const unsigned char*)cstr2) == 0;
}

// str functions

function str_t 
str(char* cstr) {
	str_t string;
	string.data = (u8*)cstr;
	string.size = cstr_length(cstr);
	return string;
}

function str_t
str(char* cstr, u32 size) {
	str_t string;
	string.data = (u8*)cstr;
	string.size = size;
	return string;
}

// color functions

function color_t 
color(u32 hex) {
	color_t result = { 0.0f };
	result.r = (f32)((hex & 0xff000000) >> 24) / 255.0f;
	result.g = (f32)((hex & 0x00ff0000) >> 16) / 255.0f;
	result.b = (f32)((hex & 0x0000ff00) >> 8) / 255.0f;
	result.a = (f32)((hex & 0x000000ff) >> 0) / 255.0f;
	return result;
}

function color_t
color(f32 r, f32 g, f32 b, f32 a) {
	return { r, g, b, a };
}

#endif // BASE_CPP