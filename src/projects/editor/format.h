// format.h

#ifndef FORMAT_H
#define FORMAT_H

#define token_data_max 256

// colors

#define color_identifier (0xbcaf85ff)
#define color_function (0xff8000ff)
#define color_keyword (0x569cd6ff)
#define color_macro (0xa86bc9ff)
#define color_type (0xf9c206ff)
#define color_comment (0x57a64aff)
#define color_text (0xc8c8c8ff)
#define color_string (0xd69d85ff)

// enum

enum space_event_type {
	space_event_type_null, 
	space_event_type_key_press,
	space_event_type_key_release,
	space_event_type_mouse_press,
	space_event_type_mouse_release,
	space_event_type_text,
	space_event_type_navigate,
	space_event_type_edit,
	space_event_type_mouse_move,
	space_event_type_mouse_scroll,
};

typedef u32 space_event_flags;
enum {
	space_event_flag_keep_mark = (1 << 0),
	space_event_flag_delete = (1 << 1),
	space_event_flag_copy = (1 << 2),
	space_event_flag_paste = (1 << 3),
	space_event_flag_pick_side = (1 << 4),
	space_event_flag_zero_delta = (1 << 5),
};


typedef u32 space_op_flags;
enum {
	space_op_flag_invalid = (1 << 0),
	space_op_flag_copy = (1 << 1),
};

// structs

struct point_t {
	u32 char_index;
	u32 token_index;
	u32 line_index;
};

struct point_range_t {
	point_t min;
	point_t max;
};

struct token_t {
	token_t* next;
	token_t* prev;

	char* data;
	u32 size;
};

struct line_t {
	line_t* next;
	line_t* prev;

	token_t* token_first;
	token_t* token_last;
};

struct space_event_t {
	space_event_t* next;
	space_event_t* prev;

	os_event_t* os_event;
	os_window_t* window;
	space_event_type type;
	space_event_flags flags;
	os_key key;
	os_mouse_button mouse_button;
	os_modifiers modifiers;
	u32 character;
	vec2_t position;
	vec2_t scroll;
	ivec2_t delta;
};

struct space_op_t {
	space_op_flags flags;
	str_t replace;
	str_t copy;
	point_range_t range;
	point_t cursor;
	point_t mark;
};

struct space_t {

	arena_t* entity_arena;
	arena_t* text_arena;
	arena_t* event_arena;

	// lines
	line_t* line_first;
	line_t* line_last;

	// free list
	line_t* line_free;
	token_t* token_free;

	space_event_t* event_first;
	space_event_t* event_last;

	line_t* line_active;
	token_t* token_active;
	
	point_t cursor;
	point_t mark;
	vec2_t cursor_pos_t;


	// assets
	font_t* font;
};

// globals

global space_t space;

// functions

function void space_init();
function void space_release();
function void space_update();
function void space_render(f32 dt);

function void space_event_push(space_event_t* event);
function void space_event_pop(space_event_t* event);

function f32 offset_from_point(point_t point);

function space_op_t space_op_from_event(space_event_t* event, point_t cursor, point_t mark);

// point
function b8 point_equals(point_t a, point_t b);
function b8 point_less_than(point_t a, point_t b);
function point_t point_min(point_t a, point_t b);
function point_t point_max(point_t a, point_t b);

// point range
function point_range_t point_range(point_t min, point_t max);
function point_range_t point_range_intersects(point_range_t a, point_range_t b);
function point_range_t point_range_union(point_range_t a, point_range_t b);
function b8 point_range_contains(point_range_t r, point_t p);

// token
function token_t* token_create();
function void token_release(token_t* token);
function token_t* token_from_point(point_t point);

// line
function line_t* line_create();
function void line_release(line_t* line);
function line_t* line_from_point(point_t point);



#endif // FORMAT_H