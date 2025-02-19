// lexer.h

#ifndef LEXER_H
#define LEXER_H

// enums

enum token_type {
	token_type_null,
	token_type_error,
	token_type_keyword,
	token_type_identifier,
	token_type_numeric,
	token_type_string,
	token_type_symbol,
	token_type_comment,
	token_type_meta,
	token_type_count,
};

enum expr_type {
	expr_type_null,
	expr_type_ref,
	expr_type_array_index,
	expr_type_member_access,
	expr_type_deref,
	expr_type_address,
	expr_type_cast,
	expr_type_pos,
	expr_type_neg,
	expr_type_log_not,
	expr_type_bit_not,
	expr_type_mul,
	expr_type_div,
	expr_type_mod,
	expr_type_add,
	expr_type_sub,
	expr_type_lshift,
	expr_type_rshift,
	expr_type_less,
	expr_type_less_equal,
	expr_type_greater,
	expr_type_greater_equal,
	expr_type_equal_equal,
	expr_type_not_equal,
	expr_type_ptr,
	expr_type_array,
	expr_type_func,
	expr_type_define,
	expr_type_count,
};

// structs

// token
struct token_t {
	token_t* next;
	token_t* prev;
	
	token_type type;
	str_t string;

	u32 row_pos;
	u32 col_pos;
};

// token list
struct token_list_t {
	token_t* first;
	token_t* last;
	u32 count;
};

// expression
struct expr_t {
	expr_t* next;
	expr_t* prev;
	expr_t* first;
	expr_t* last;
	expr_t* parent;

	expr_type type;
};

// expression recursion
struct expr_rec_t {
	expr_t* next;
	u32 push_count;
	u32 pop_count;
};

// expression list
struct expr_list_t {
	expr_t* first;
	expr_t* last;
	u32 count;
};

global str_t c_keywords[] = {
	str("alignas"),
	str("alignof"),
	str("and"),
	str("and_eq"),
	str("asm"),
	str("auto"),
	str("bitand"),
	str("bitor"),
	str("bool"),
	str("break"),
	str("case"),
	str("catch"),
	str("char"),
	str("char8_t"),
	str("char16_t"),
	str("char32_t"),
	str("class"),
	str("compl"),
	str("concept"),
	str("const"),
	str("consteval"),
	str("constexpr"),
	str("constinit"),
	str("const_cast"),
	str("continue"),
	str("co_await"),
	str("co_return"),
	str("co_yeild"),
	str("decltype"),
	str("default"),
	str("delete"),
	str("do"),
	str("double"),
	str("dynamic_cast"),
	str("else"),
	str("enum"),
	str("explicit"),
	str("export"),
	str("extern"),
	str("false"),
	str("final"),
	str("float"),
	str("for"),
	str("friend"),
	str("goto"),
	str("if"),
	str("inline"),
	str("int"),
	str("long"),
	str("mutable"),
	str("namespace"),
	str("new"),
	str("noexcept"),
	str("not"),
	str("not_eq"),
	str("nullptr"),
	str("operator"),
	str("or"),
	str("or_eq"),
	str("private"),
	str("protected"),
	str("public"),
	str("register"),
	str("reinterpret_cast"),
	str("requires"),
	str("return"),
	str("short"),
	str("signed"),
	str("sizeof"),
	str("static"),
	str("static_assert"),
	str("static_cast"),
	str("struct"),
	str("switch"),
	str("template"),
	str("this"),
	str("thread_local"),
	str("throw"),
	str("true"),
	str("try"),
	str("typedef"),
	str("typeid"),
	str("typename"),
	str("union"),
	str("unsigned"),
	str("using"),
	str("virtual"),
	str("void"),
	str("volatile"),
	str("wchar_t"),
	str("while"),
	str("xor"),
	str("xor_eq")
};

// functions

function token_list_t token_list_from_string(arena_t* arena, str_t string);
function expr_list_t expressions_from_token_list(arena_t* arena, token_list_t token_list);

// helpers

// expressions
function void expr_insert(expr_t* parent, expr_t* expr, expr_t* prev = nullptr);
function void expr_remove(expr_t* parent, expr_t* expr);


// type strings
function str_t string_from_token_type(token_type type);


// internal
function expr_t* parse_expression_from_token_list_prec(arena_t* arena, )


#endif // LEXER_H