// codegen.h

#ifndef CODEGEN_H
#define CODEGEN_H

// TODO:
//
// [ ] - add error messages into everything.
// [ ] - implement the state to store the keywords and grammer rules
// [ ] - implement the parser to convert tokens to an AST based on the grammer rules
// [ ] - implement converts the AST into IR
//

// how grammar rules could be defined:
//
//     rule     |          pattern           
// --------------------------------------------
//     iden     | [a-z,A-Z,_][a-z,A-Z,0-9,_]    
//     expr     | 
//      op      | <expr> [+  -, *, /] <expr>
//  func_call   | <iden> ( [<iden>] )        
//    assign    | <iden> := <expr>           
//         

// 
// define range of character: [a-z, 0-9, _] 
//
// identifiers: [a-z,A-Z,_][a-z,A-Z,0-9,_]
// numerical literals: 
//
//
//
//
//


//- enums 

enum cg_msg_type {
    cg_msg_type_null,
    cg_msg_type_info,
    cg_msg_type_warning,
    cg_msg_type_error,
    cg_msg_type_fatal,
    cg_msg_type_count,
};

enum cg_token_type {
    cg_token_type_null,
    cg_token_type_error,
    cg_token_type_keyword,
    cg_token_type_identifier,
    cg_token_type_numeric,
    cg_token_type_string,
    cg_token_type_symbol, // TODO: do we seperate out all of symbols into types?
    cg_token_type_comment,
    cg_token_type_meta,
    cg_token_type_count,
};

enum cg_node_type {
    cg_node_type_null,
    cg_node_type_root,
    cg_node_type_numeric,
    cg_node_type_operator,
    cg_node_type_count,
};

enum cg_op_type {
    cg_op_type_null,
    cg_op_type_add,
    cg_op_type_sub,
    cg_op_type_mul,
    cg_op_type_div,
    cg_op_type_l_paren,
    cg_op_type_r_paren,
    cg_op_type_count,
};

enum cg_parse_state {
    cg_parse_state_null,
    cg_parse_state_operator,
    cg_parse_state_complete,
    cg_parse_type_count,
};


//- structs 

// messages
struct cg_msg_t {
    cg_msg_t* next;
    cg_msg_type type;
    str_t string;
};

struct cg_msg_list_t {
    cg_msg_t* first;
    cg_msg_t* last;
    u32 count;
};

// tokens
struct cg_token_t {
    cg_token_t* next;
    cg_token_t* prev;
    cg_token_type type;
    str_t string;
    u32 row;
    u32 col;
};

struct cg_token_list_t {
    cg_token_t* first;
    cg_token_t* last;
    u32 count;
};

// node
struct cg_node_t {
    cg_node_t* next;
    cg_node_t* prev;
    cg_node_t* parent;
    cg_node_t* first;
    cg_node_t* last;
    
    cg_node_type type;
    
    union {
        
        union {
            f32 _f32;
            u32 _u32;
        } value;
        
        cg_op_type op_type;
        
    };
    
};

struct cg_node_rec_t {
    cg_node_t* next;
    i32 push_count;
    i32 pop_count;
};

// node stack
struct cg_node_stack_node_t {
    cg_node_stack_node_t* next;
    cg_node_t* node;
};

struct cg_node_stack_t {
    arena_t* arena;
    cg_node_stack_node_t* top;
    cg_node_stack_node_t* free;
};

// op stack
struct cg_op_stack_node_t {
    cg_op_stack_node_t* next;
    cg_op_type op;
};

struct cg_op_stack_t {
    arena_t* arena;
    cg_op_stack_node_t* top;
    cg_op_stack_node_t* free;
};


// state
struct cg_state_t {
    
    arena_t* arena;
    
    str_t* keywords;
    u32 keyword_count;
    
    cg_node_t* node_first;
    cg_node_t* node_last;
    cg_node_t* node_free;
    
};


//- functions 

// state
function cg_state_t* cg_init(u32 mem_size = megabytes(128));
function void cg_release(cg_state_t* state);
function void cg_add_keywords(cg_state_t* state, cstr* keywords, u32 keyword_count);

// messages
function void cg_msg_list_push(arena_t* arena, cg_msg_list_t* msg_list, cg_msg_type type, str_t string);
function void cg_msg_list_pushf(arena_t* arena, cg_msg_list_t* msg_list, cg_msg_type type, char* fmt, ...);

// nodes
function cg_node_t* cg_node_create(cg_state_t* state, cg_node_type type);
function void cg_node_release(cg_state_t* state, cg_node_t* node);
function void cg_node_insert(cg_node_t* parent, cg_node_t* node, cg_node_t* prev = nullptr);
function void cg_node_remove(cg_node_t* parent, cg_node_t* node);
function cg_node_rec_t cg_node_rec_depth_first_pre(cg_node_t* node, cg_node_t* root);
function cg_node_rec_t cg_node_rec_depth_first_post(cg_node_t* node);

// node stack
function cg_node_stack_t* cg_node_stack_create(arena_t* arena);
function cg_node_t* cg_node_stack_top(cg_node_stack_t* stack);
function cg_node_t* cg_node_stack_push(cg_node_stack_t* stack, cg_node_t* node);
function cg_node_t* cg_node_stack_pop(cg_node_stack_t* stack);

// op stack
function cg_op_stack_t* cg_op_stack_create(arena_t* arena);
function cg_op_type cg_op_stack_top(cg_op_stack_t* stack);
function cg_op_type cg_op_stack_push(cg_op_stack_t* stack, cg_op_type op);
function cg_op_type cg_op_stack_pop(cg_op_stack_t* stack);

// lexer
function cg_token_list_t cg_token_list_from_string(cg_state_t* state, str_t string);

// parser
function cg_node_t* cg_ast_from_token_list(cg_state_t* state, cg_token_list_t token_list);


// helper functions
function str_t cg_string_from_token_type(cg_token_type type);
function str_t cg_string_from_node_type(cg_node_type type);

function cg_op_type cg_op_type_from_token(cg_token_t* token);
function i32 cg_precedence_from_op_type(cg_op_type type);

#endif // CODEGEN_H