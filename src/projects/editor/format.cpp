// format.cpp

#ifndef FORMAT_CPP
#define FORMAT_CPP

// implementation

// token functions

function token_t*
token_create(space_t* space) {

	token_t* token = space->token_free;

	if (token != nullptr) {
		stack_pop(space->token_free);
	} else {
		token = (token_t*)arena_alloc(space->arena, sizeof(token_t));
	}
	memset(token, 0, sizeof(token_t));

	return token;
}

function void 
token_release(space_t* space, token_t* token) {
	stack_push(space->token_free, token);
}

function void 
token_insert(line_t* line, token_t* token, i32 index) {
	dll_push_back(line->first, line->last, token);
	line->count++;
}

function void 
token_remove(line_t* line, token_t* token) {
	dll_remove(line->first, line->last, token);
	line->count--;
}

function token_t* 
token_from_index(line_t* line, i32 index) {

	token_t* result = nullptr;

	i32 count = 0;
	for (token_t* token = line->first; token != 0; token = token->next) {
		if (count == index) {
			result = token;
			break;
		}
		count++;
	}

	return result;
}

// line functions

function line_t* 
line_create(space_t* space) {

	line_t* line = space->line_free;

	if (line != nullptr) {
		stack_pop(space->line_free);
	} else {
		line = (line_t*)arena_alloc(space->arena, sizeof(line_t));
	}
	memset(line, 0, sizeof(line_t));

	return line;
}

function void 
line_release(space_t* space, line_t* line) {
	stack_push(space->line_free, line);
}

function void 
line_insert(space_t* space, line_t* line, i32 index) {
	dll_push_back(space->first, space->last, line);
	space->count++;
}

function void 
line_remove(space_t* space, line_t* line) {
	dll_remove(space->first, space->last, line);
	space->count--;
}

function line_t* 
line_from_index(space_t* space, i32 index) {

	line_t* result = nullptr;

	i32 count = 0;
	for (line_t* line = space->first; line != 0; line = line->next) {
		if (count == index) {
			result = line;
			break;
		}
		count++;
	}

	return result;

}

// space
function space_t* 
space_create() {
	arena_t* arena = arena_create(gigabytes(1));
	space_t* space = (space_t*)arena_calloc(arena, sizeof(space_t));
	space->arena = arena;

	// insert blank line and token
	line_t* line = line_create(space);
	line_insert(space, line, 0);

	token_t* token = token_create(space);
	token_insert(line, token, 0);

	return space;
}

function void
space_release(space_t* space) {
	arena_clear(space->arena);
}

#endif // FORMAT_CPP