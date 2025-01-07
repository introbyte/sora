// parser.cpp

// includes
#include "engine/base.h"
#include "engine/os.h"

#include "engine/base.cpp"
#include "engine/os.cpp"

#include "utils/lexer.h"
#include "utils/lexer.cpp"

function i32 
app_entry_point(i32 argc, char** argv) {

	arena_t* arena = arena_create(gigabytes(2));
	os_file_t file = os_file_open(str("src/parser.cpp"));
	str_t file_contents = os_file_read_all(arena, file);

	token_list_t token_list = token_list_from_string(arena, file_contents);

	// list tokens
	for (token_t* token = token_list.first; token != nullptr; token = token->next) {
		str_t type_string = string_from_token_type(token->type);
		printf("[%s] %.*s\n", type_string.data, token->string.size, token->string.data);
	}

	os_file_close(file);

	printf("[info] program exited successfully.\n");

	return 0;
}
