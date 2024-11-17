// format.h

#ifndef FORMAT_H
#define FORMAT_H

// defines

#define TOKEN_LABEL_MAX_SIZE 1024

// structs

struct token_t {
	token_t* next;
	token_t* prev;

	char label[TOKEN_LABEL_MAX_SIZE];
	u32 label_size;
};

struct line_t {
	line_t* next;
	line_t* prev;

	token_t* first;
	token_t* last;
	u32 count;
	
};

struct space_t {
	line_t* first;
	line_t* last;
	u32 count;

	arena_t* arena;

	// free stacks
	line_t* line_free;
	token_t* token_free;
};

struct point_t {
	i32 line_index;
	i32 token_index;
	i32 char_index;
};

struct range_t {
	point_t min;
	point_t max;
};

// functions

function token_t* token_create(space_t*);
function void token_release(space_t*, token_t*);
function void token_insert(line_t*, token_t*, i32);
function void token_remove(line_t*, token_t*);
function token_t* token_from_index(line_t*, i32);

function line_t* line_create(space_t*);
function void line_release(space_t*, line_t*);
function void line_insert(space_t*, line_t*, i32);
function void line_remove(space_t*, line_t*);
function line_t* line_from_index(space_t*, i32);


#endif // FORMAT_H