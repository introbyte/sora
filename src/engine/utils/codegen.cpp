// codegen.cpp

#ifndef CODEGEN_CPP
#define CODEGEN_CPP

//~ implementation 

//- state functions 
function cg_state_t*
cg_init(u32 mem_size) {
    
    arena_t* arena = arena_create(mem_size);
    
    cg_state_t* state = (cg_state_t*)arena_alloc(arena, sizeof(cg_state_t));
    
    state->arena = arena;
    
    
    return state;
    
}

function void
cg_release(cg_state_t* state) {
    arena_release(state->arena);
}

function void
cg_add_keywords(cg_state_t* state, str_t* keywords, u32 keyword_count) {
    // TODO: maybe we want to copy the words into our own arena?
    state->keywords = keywords;
    state->keyword_count = keyword_count;
}


//- messages 
function void 
cg_msg_list_push(arena_t* arena, cg_msg_list_t* msg_list, cg_msg_type type, str_t string) {
    cg_msg_t* msg = (cg_msg_t*)arena_alloc(arena, sizeof(cg_msg_t));
    msg->type = type;
    msg->string = string;
    queue_push(msg_list->first, msg_list->last, msg);
    msg_list->count++;
}

function void 
cg_msg_list_pushf(arena_t* arena, cg_msg_list_t* msg_list, cg_msg_type type, char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    str_t string = str_format(arena, fmt, args);
    cg_msg_list_push(arena, msg_list, type, string);
    va_end(args);
}

//- nodes 

function cg_node_t* 
cg_node_create(cg_state_t* state, cg_node_type type) {
    
    cg_node_t* node = state->node_free;
    if (node != nullptr) {
        stack_pop(state->node_free);
    } else {
        node = (cg_node_t*)arena_alloc(state->arena, sizeof(cg_node_t));
    }
    memset(node, 0, sizeof(cg_node_t));
    
    node->type = type;
    
    return node;
}

function void
cg_node_release(cg_state_t* state, cg_node_t* node) {
    
    // remove from parent if needed
    if (node->parent != nullptr) {
        cg_node_remove(node->parent, node);
    }
    
    stack_push(state->node_free, node);
    
}

function void
cg_node_insert(cg_node_t* parent, cg_node_t* node, cg_node_t* prev) {
    dll_insert(parent->first, parent->last, prev, node);
    node->parent = parent;
}

function void 
cg_node_remove(cg_node_t* parent, cg_node_t* node) {
    dll_remove(parent->first, parent->last, node);
    node->parent = nullptr;
}

function cg_node_rec_t 
cg_node_rec_depth_first_pre(cg_node_t* node, cg_node_t* root) {
    cg_node_rec_t rec = { 0 };
    
    if (node->first != nullptr) {
        rec.next = node->first;
        rec.push_count = 1;
    } else for (cg_node_t* n = node; n != nullptr && n != root; n = n->parent) {
        if (n->next != nullptr) {
            rec.next = n->next;
            break;
        }
        rec.pop_count++;
    }
    
    return rec;
}

function cg_node_rec_t 
cg_node_rec_depth_first_post(cg_node_t* node) {
    
    cg_node_rec_t rec = { 0 };
    
    if (node->last != nullptr) {
        rec.next = node->last;
        rec.push_count = 1;
    } else for (cg_node_t* n = node; n != nullptr; n = n->parent) {
        if (n->prev != nullptr) {
            rec.next = n->prev;
            break;
        }
        rec.pop_count++;
    }
    
    return rec;
}

//- node stack 

function cg_node_stack_t *
cg_node_stack_create(arena_t* arena) {
    cg_node_stack_t* stack = (cg_node_stack_t*)arena_alloc(arena, sizeof(cg_node_stack_t));
    stack->arena = arena;
    stack->top = nullptr;
    stack->free = nullptr;
    return stack;
}

function cg_node_t*
cg_node_stack_top(cg_node_stack_t* stack) {
    cg_node_t* result = nullptr;
    if (stack->top != nullptr) {
        result = stack->top->node;
    }
    return result;
}

function cg_node_t*
cg_node_stack_push(cg_node_stack_t* stack, cg_node_t* node) {
    cg_node_stack_node_t* stack_node = stack->free;
    if (stack_node != nullptr) {
        stack_pop(stack->free);
    } else {
        stack_node = (cg_node_stack_node_t*) arena_alloc(stack->arena, sizeof(cg_node_stack_node_t));
    }
    memset(stack_node, 0, sizeof(cg_node_stack_node_t));
    stack_node->node = node;
    cg_node_t* old_node = nullptr;
    if (stack->top != nullptr) {
        old_node = stack->top->node;
    }
    stack_push(stack->top, stack_node);
    return old_node;
}

function cg_node_t*
cg_node_stack_pop(cg_node_stack_t* stack) {
    cg_node_stack_node_t* popped_stack_node = stack->top;
    cg_node_t* result = nullptr;
    if (popped_stack_node != nullptr) {
        result = popped_stack_node->node;
        stack_pop(stack->top);
        stack_push(stack->free, popped_stack_node);
    }
    return result;
}


//- op stack 

function cg_op_stack_t *
cg_op_stack_create(arena_t* arena) {
    cg_op_stack_t* stack = (cg_op_stack_t*)arena_alloc(arena, sizeof(cg_op_stack_t));
    stack->arena = arena;
    stack->top = nullptr;
    stack->free = nullptr;
    return stack;
}

function cg_op_type
cg_op_stack_top(cg_op_stack_t* stack) {
    cg_op_type result = cg_op_type_null;
    if (stack->top != nullptr) {
        result = stack->top->op;
    }
    return result;
}

function cg_op_type
cg_op_stack_push(cg_op_stack_t* stack, cg_op_type op) {
    cg_op_stack_node_t* stack_op = stack->free;
    if (stack_op != nullptr) {
        stack_pop(stack->free);
    } else {
        stack_op = (cg_op_stack_node_t*) arena_alloc(stack->arena, sizeof(cg_op_stack_node_t));
    }
    memset(stack_op, 0, sizeof(cg_op_stack_node_t));
    stack_op->op = op;
    cg_op_type old_op = cg_op_type_null;
    if (stack->top != nullptr) {
        old_op = stack->top->op;
    }
    stack_push(stack->top, stack_op);
    return old_op;
}

function  cg_op_type
cg_op_stack_pop(cg_op_stack_t* stack) {
    cg_op_stack_node_t* popped_stack_op = stack->top;
    cg_op_type result = cg_op_type_null;
    if (popped_stack_op != nullptr) {
        result = popped_stack_op->op;
        stack_pop(stack->top);
        stack_push(stack->free, popped_stack_op);
    }
    return result;
}


//- lexer 

function cg_token_list_t 
cg_token_list_from_string(cg_state_t* state, str_t string) {
    
    cg_token_list_t token_list;
	token_list.first = nullptr;
	token_list.last = nullptr;
	token_list.count = 0;
    
	u32 current_row = 1;
	u32 current_col = 1;
    
	cg_token_type active_token_type = cg_token_type_null;
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
		if (i == string.size && active_token_type != cg_token_type_null) {
			end_token = true;
			advance = 1;
		}
        
		switch (active_token_type) {
            
			// figure out what token we are
			case cg_token_type_null: {
                
				b8 found = false;
                
				// identifiers
				if (char_is_alpha(c) || c == '_') {
					active_token_type = cg_token_type_identifier;
					found = true;
				}
                
				// comments
				else if (c == '/' && (c_n == '/' || c_n == '*')) {
					active_token_type = cg_token_type_comment;
					comment_is_single_line = (c_n == '/');
					advance = 2;
					found = true;
				}
                
				// numerical literals
				else if (char_is_digit(c) || (c == '.' && char_is_digit(c_n))) {
					active_token_type = cg_token_type_numeric;
					found = true;
				}
                
				// string literals
				else if (c == '\"') {
					active_token_type = cg_token_type_string;
					found = true;
				}
                
				// symbols
				else if (char_is_symbol(c) && c != '#') {
					active_token_type = cg_token_type_symbol;
					found = true;
				}
                
				// meta
				else if (c == '#') {
					active_token_type = cg_token_type_meta;
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
			case cg_token_type_identifier: {
				if (!char_is_alpha(c) && !char_is_digit(c) && c != '_') {
					end_token = true;
					advance = 0;
				}
				break;
			}
            
			// handle number literals
			case cg_token_type_numeric: {
				if (!char_is_digit(c) && !char_is_alpha(c) && c != '.') {
					end_token = true;
					advance = 0;
				}
				break;
			}
            
			// handle string literals
			case cg_token_type_string: {
				if (c == '"') {
					end_token = true;
					advance = 1;
				}
				break;
			}
            
			// handle symbols
			case cg_token_type_symbol: {
				if (!char_is_symbol(c)) {
					end_token = true;
					advance = 0;
				}
				break;
			}
            
			// handle comments
			case cg_token_type_comment: {
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
			case cg_token_type_meta: {
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
			if (active_token_type == cg_token_type_identifier) {
                
				str_t token_string = str_substr(string, active_token_start, i + advance);
                
				for (u32 keyword_index = 0; keyword_index < state->keyword_count; keyword_index++) {
					if (str_match(state->keywords[keyword_index], token_string)) {
						active_token_type = cg_token_type_keyword;
					}
                    
				}
			}
            
			//if (active_token_type != token_type_comment) {
            
            // fill struct
            cg_token_t* token = (cg_token_t*)arena_alloc(state->arena, sizeof(cg_token_t));
            token->type = active_token_type;
            u32 start_pos = active_token_start;
            u32 end_pos = i + advance;
            token->string = str_substr(string, start_pos, end_pos);
            token->row = active_token_row;
            token->col = active_token_col;
            
            // push to list
            dll_push_back(token_list.first, token_list.last, token);
            token_list.count++;
            
			//}
            
			// reset active token
			active_token_type = cg_token_type_null;
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


function cg_node_t*
cg_ast_from_token_list(cg_state_t* state, cg_token_list_t list) {
    
    temp_t scratch = scratch_begin();
    
    // setup state and stack
    cg_token_t* token = list.first; 
    cg_parse_state parse_state = cg_parse_state_null;
    cg_node_stack_t* node_stack = cg_node_stack_create(scratch.arena);
    cg_op_stack_t* op_stack = cg_op_stack_create(scratch.arena);
    
    while (parse_state != cg_parse_state_complete) {
        
        switch (parse_state) {
            
            // state: initial
            case cg_parse_state_null: {
                
                if (token->type == cg_token_type_numeric) {
                    // create node and push to stack
                    cg_node_t* node = cg_node_create(state, cg_node_type_numeric);
                    node->value._f32 = f32_from_str(token->string);
                    cg_node_stack_push(node_stack, node);
                    parse_state = cg_parse_state_operator;
                    token = token->next; // advance
                    continue;
                } else if (token->type == cg_token_type_symbol) {
                    
                    // left paren
                    if (str_match(token->string, str("("))) {
                        token = token->next;
                        cg_op_stack_push(op_stack, cg_op_type_l_paren);
                        continue;
                    }
                    
                }
                
                token = token->next; // advance
                break;
            }
            
            // state: operator
            case cg_parse_state_operator: {
                
                cg_op_type op_type = cg_op_type_from_token(token);
                i32 precedence = cg_precedence_from_op_type(op_type);
                
                while (op_stack->top != nullptr) {
                    
                    cg_op_type top_op_type = cg_op_stack_top(op_stack);
                    i32 top_precedence = cg_precedence_from_op_type(top_op_type);
                    if (top_precedence < precedence) {
                        break;
                    }
                    
                    cg_node_t* right = cg_node_stack_pop(node_stack);
                    cg_node_t* left = cg_node_stack_pop(node_stack);
                    cg_node_t* op_node = cg_node_create(state, cg_node_type_operator);
                    op_node->op_type = cg_op_stack_pop(op_stack);
                    cg_node_insert(op_node, left);
                    cg_node_insert(op_node, right, left);
                    cg_node_stack_push(node_stack, op_node);
                    
                }
                
                if (op_type != cg_op_type_null) {
                    cg_op_stack_push(op_stack, op_type);
                }
                
                if (token != nullptr) {
                    token = token->next;
                }
                
                parse_state = cg_parse_state_null;
                break;
            }
            
        }
        
        // break out when finished
        if (token == nullptr) {
            parse_state = cg_parse_state_complete;
            continue;
        }
        
    }
    
    // get root node
    cg_node_t* root_node = cg_node_stack_top(node_stack);
    
    scratch_end(scratch);
    
    return root_node;
}




//- helper functions 

function str_t 
cg_string_from_token_type(cg_token_type type) {
    str_t result;
    switch (type) {
		case cg_token_type_null: { result = str("null"); break; }
		case cg_token_type_error: { result =  str("error"); break;}
		case cg_token_type_keyword: { result =  str("keyword"); break;}
		case cg_token_type_identifier: { result =  str("identifier");break; }
		case cg_token_type_numeric: { result =  str("numeric"); break;}
		case cg_token_type_string: { result =  str("string");break; }
		case cg_token_type_symbol: { result =  str("symbol"); break;}
		case cg_token_type_comment: { result =  str("comment"); break;}
		case cg_token_type_meta: { result =  str("meta");break; }
		default: { result =  str("unknown token type");break; }
	}
    return result;
}

function str_t 
cg_string_from_node_type(cg_node_type type) {
    str_t result;
    switch (type) {
        case cg_node_type_null: { result = str("null"); break; }
        case cg_node_type_numeric: { result = str("numeric"); break; }
        case cg_node_type_operator: { result = str("operator"); break; }
        default: {result = str("unknown node type"); break;}
    }
    return result;
}

function str_t
cg_string_from_op_type(cg_op_type type) {
    str_t result;
    switch (type) {
        case cg_op_type_null: { result = str("null"); break; }
        case cg_op_type_add: { result = str("add"); break; }
        case cg_op_type_sub: { result = str("sub"); break; }
        case cg_op_type_mul: { result = str("mul"); break; }
        case cg_op_type_div: { result = str("div"); break; }
        default: {result = str("unknown op type"); break;}
    }
    return result;
}


function cg_op_type
cg_op_type_from_token(cg_token_t* token) {
    cg_op_type result = cg_op_type_null;
    if (token != nullptr && token->type == cg_token_type_symbol) {
        if (str_match(token->string, str("+"))) {
            result = cg_op_type_add;
        } else if (str_match(token->string, str("-"))) {
            result = cg_op_type_sub;
        } else if (str_match(token->string, str("*"))) {
            result = cg_op_type_mul;
        } else if (str_match(token->string, str("/"))) {
            result = cg_op_type_div;
        }
    }
    return result;
}

function i32 
cg_precedence_from_op_type(cg_op_type type) {
    i32 result = 0;
    switch (type) {
        case cg_op_type_null: { result = 0; break; }
        case cg_op_type_add: { result = 1; break; }
        case cg_op_type_sub: { result = 1; break; }
        case cg_op_type_mul: { result = 2; break; }
        case cg_op_type_div: { result = 2; break; }
    }
    return result;
}

#endif // CODEGEN_CPP