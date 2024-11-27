// editor.h

#ifndef EDITOR_H
#define EDITOR_H

// enums

typedef u32 node_flags;
enum {
	node_flag_none = (0),
	node_flag_draw_text = (1 << 0),
};

enum node_type {
	node_type_none,
	node_type_program,
	node_type_keyword,
	node_type_identifier,
	node_type_literal,
	node_type_variable_declaration,
	node_type_function_declaration,
	node_type_block,
	node_type_expression,
};

enum editor_event_type {
	editor_event_type_null, 
	editor_event_type_key_press,
	editor_event_type_key_release,
	editor_event_type_mouse_press,
	editor_event_type_mouse_release,
	editor_event_type_mouse_move,
	editor_event_type_mouse_scroll,
	editor_event_type_text,
};

// structs

struct node_t {
	node_t* next;
	node_t* prev;
	node_t* first;
	node_t* last;
	node_t* parent;
	
	node_type type;
	node_flags flags;
	str_t label;
};

struct node_rec_t {
	node_t* next; 
	u32 push_count;
	u32 pop_count;
};

struct space_t {
	space_t* next;
	space_t* prev;

	arena_t* node_arena;
	
	node_t* node_free;
	node_t* node_root;

	vec2_t layout_pos;
};

struct editor_event_t {
	editor_event_t* next;
	editor_event_t* prev;

	os_event_t* os_event;
	editor_event_type type;
	os_key key;
	os_mouse_button mouse_button;
	os_modifiers modifiers;
	u32 character;
	vec2_t position;
	vec2_t scroll;
};

struct editor_event_list_t {
	editor_event_t* first;
	editor_event_t* last;
};

struct editor_state_t {
	arena_t* space_arena;

	space_t* space_first;
	space_t* space_last;
	space_t* space_free;
	u32 space_count;
};

global editor_state_t editor_state;

// functions

// editor
function void editor_init();
function void editor_release();
function void editor_update();

// space
function space_t* space_create();
function void space_release(space_t* space);
function void space_render(space_t* space);

function void space_push_text(space_t* space, str_t text);
function void space_push_newline(space_t* space);

// node
function node_t* node_create(space_t* space, str_t label, node_type type, node_flags flags = 0);
function void node_release(space_t* space, node_t* node);
function void node_insert(node_t* parent, node_t* node, node_t* prev = nullptr);
function void node_remove(node_t* parent, node_t* node);
function node_rec_t node_rec_depth_first(node_t* node);


// draw




#endif // EDITOR_H