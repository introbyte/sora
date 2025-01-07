// lexer.cpp

#ifndef LEXER_CPP
#define LEXER_CPP

// implementation

function token_list_t 
token_list_from_string(arena_t* arena, str_t string) {

	token_list_t token_list;
	token_list.first = nullptr;
	token_list.last = nullptr;
	token_list.count = 0;

	u32 current_row = 1;
	u32 current_col = 1;

	token_type active_token_type = token_type_null;
	u32 active_token_start = 0;
	u32 active_token_row = 0;
	u32 active_token_col = 0;

	u32 advance = 0;
	b8 comment_is_single_line = false;

	for (i32 i = 0; i <= string.size; i += advance) {

		char c = string.data[i + 0]; // current char
		char c_n = string.data[i + 1]; // next char

		advance = 1;
		b8 end_token = false;

		// end token if we are at end.
		if (i == string.size && active_token_type != token_type_null) {
			end_token = true;
			advance = 1;
		}

		switch (active_token_type) {

			// figure out what token we are
			case token_type_null: {

				b8 found = false;

				// identifiers
				if (char_is_alpha(c) || c == '_') {
					active_token_type = token_type_identifier;
					found = true;
				}

				// comments
				else if (c == '/' && (c_n == '/' || c_n == '*')) {
					active_token_type = token_type_comment;
					comment_is_single_line = (c_n == '/');
					advance = 2;
					found = true;
				}

				// numerical literals
				else if (char_is_digit(c) || (c == '.' && char_is_digit(c_n))) {
					active_token_type = token_type_numeric;
					found = true;
				}

				// string literals
				else if (c == '\"') {
					active_token_type = token_type_string;
					found = true;
				}

				// symbols
				else if (char_is_symbol(c) && c != '#') {
					active_token_type = token_type_symbol;
					found = true;
				}

				// meta
				else if (c == '#') {
					active_token_type = token_type_meta;
					found = true;
				}
				
				if (found) {
					active_token_start = i;
					active_token_row = current_row;
					active_token_col = current_col;
				}

				break;
			}

			// handle identifiers
			case token_type_identifier: {
				if (!char_is_alpha(c) && !char_is_digit(c) && c != '_') {
					end_token = true;
					advance = 0;
				}
				break;
			}

			// handle number literals
			case token_type_numeric: {
				if (!char_is_digit(c) && !char_is_alpha(c) && c != '.') {
					end_token = true;
					advance = 0;
				}
				break;
			}

			// handle string literals
			case token_type_string: {
				if (c == '"') {
					end_token = true;
					advance = 1;
				}
				break;
			}

			// handle symbols
			case token_type_symbol: {
				if (!char_is_symbol(c)) {
					end_token = true;
					advance = 0;
				}
				break;
			}

			// handle comments
			case token_type_comment: {
				if (comment_is_single_line && (c == '\n' || (c == '\r' && c_n == '\n'))) {
					end_token = true;
					advance = 0;
				} else if (!comment_is_single_line && c == '*' && c_n == '/') {
					end_token = true;
					advance = 2;
				}
				break;
			}

			// handle meta
			case token_type_meta: {
				if (c == '\n' || c == '\r') {
					end_token = true;
					advance = 1;
				}
				break;
			}
		}

		// submit token if we have one
		if (end_token) {

			// check for keywords
			if (active_token_type == token_type_identifier) {

				str_t token_string = str_substr(string, active_token_start, i + advance);

				for (u32 keyword_index = 0; keyword_index < array_count(c_keywords); keyword_index++) {
					if (str_match(c_keywords[keyword_index], token_string)) {
						active_token_type = token_type_keyword;
					}

				}
			}

			//if (active_token_type != token_type_comment) {

				// fill struct
				token_t* token = (token_t*)arena_alloc(arena, sizeof(token_t));
				token->type = active_token_type;
				u32 start_pos = active_token_start;
				u32 end_pos = i + advance;
				token->string = str_substr(string, start_pos, end_pos);
				token->row_pos = active_token_row;
				token->col_pos = active_token_col;

				// push to list
				dll_push_back(token_list.first, token_list.last, token);
				token_list.count++;
			//}

			// reset active token
			active_token_type = token_type_null;
			active_token_start = i;
			active_token_row = current_row;
			active_token_col = current_col;
		}

		// row and col count
		current_col += advance;
		if (c == '\n' || (c == '\r' && c_n == '\n')) {
			current_row++;
			current_col = 1;
			if (c == '\r') {
				advance = 2;
			}
		}
	}

	return token_list;
}

function expr_t*
expression_from_token_list(arena_t* arena, token_list_t token_list) {



}

// helpers

// expressions
function void 
expr_insert(expr_t* parent, expr_t* expr, expr_t* prev) {
	dll_insert(parent->first, parent->last, prev, expr);
	expr->parent = parent;
}

function void
expr_remove(expr_t* parent, expr_t* expr) {
	dll_remove(parent->first, parent->last, expr);
	expr->parent = nullptr;
	expr->next = nullptr;
	expr->prev = nullptr;
}


// type strings
function str_t 
string_from_token_type(token_type type) {
	switch (type) {
		case token_type_null: { return str("null"); }
		case token_type_error: { return str("error"); }
		case token_type_keyword: { return str("keyword"); }
		case token_type_identifier: { return str("identifier"); }
		case token_type_numeric: { return str("numeric"); }
		case token_type_string: { return str("string"); }
		case token_type_symbol: { return str("symbol"); }
		case token_type_comment: { return str("comment"); }
		case token_type_meta: { return str("meta"); }
		default: { return str("unknown token type"); }
	}
}



#endif // LEXER_CPP