// ui.h

#ifndef UI_H
#define UI_H

// todo: 
// 
// [x] - layout pass.
// [x] - more frame customization (corners).
//     [x] - text padding.
//     [x] - individual corner rounding.
// [x] - basic widgets. 
//     [x] - button.
//     [x] - label.
//     [x] - checkbox.
//     [x] - expander.
//     [x] - slider.
//     [x] - sat/val picker.
//     [x] - color wheel.
//     [~] - textbox. // needs some touch ups
//         [x] - keyboard controls.
//         [ ] - mouse controls.
//     [ ] - tree list.
//     [ ] - tool tips.
//     [ ] - list box.
//     [ ] - combo box.
// [x] - fix rendering. sometimes things aren't ordered correctly (text is rendered behind quads).
// [x] - icon rendering.
// [x] - look into depth ordering. maybe come up with something better.
// [x] - render more things
//     [x] - bezier curves.
// [x] - change mouse cursor on hover.
// [~] - more ui events.
//     [x] - scrolling.
//     [x] - keyboard.
//     [x] - text ops
//     [ ] - nagivation. 
//     [x] - double and triple click.
// [ ] - frame focusing.
// [ ] - deal with tabs in fonts correctly.
// [ ] - better depth sorting.
// [ ] - fix font rendering. issues with alignment.
// [ ] - clean up pass.

//defines

#define ui_stack_node_decl(name, type) struct ui_##name##_node_t { ui_##name##_node_t* next; type v; };
#define ui_stack_decl(name) struct { ui_##name##_node_t* top; ui_##name##_node_t* free; b8 auto_pop; } name##_stack;
#define ui_stack_decl_default(name) ui_##name##_node_t name##_default_node;
#define ui_stack_top_func(name, type) function type ui_top_##name();
#define ui_stack_push_func(name, type) function type ui_push_##name(type);
#define ui_stack_pop_func(name, type) function type ui_pop_##name();
#define ui_stack_set_next_func(name, type) function type ui_set_next_##name(type);
#define ui_stack_func(name, type)\
ui_stack_top_func(name, type)\
ui_stack_push_func(name, type)\
ui_stack_pop_func(name, type)\
ui_stack_set_next_func(name, type)\


// icons enum
enum {
	icon_null          = ' ',
	icon_check         = 'X',
	icon_filledcircle  = '.',
	icon_filledstar    = '*',
	icon_hollowstar    = '8',
	icon_plus          = '+',
	icon_pencil        = 'e',
	icon_trashcan      = '3',
	icon_closedfolder  = 'M',
	icon_openfolder    = 'N',
	icon_fileplain     = 'f',
	icon_filecopy      = 'F',
	icon_filetxt       = 't',
	icon_fileimg       = 'i',
	icon_filesound     = ')',
	icon_filearchive   = 'z',
	icon_gears         = '@',
	icon_sound0        = 's',
	icon_sound1        = 'S',
	icon_sound2        = 'Z',
	icon_downtri       = 'd',
	icon_uptri         = 'u',
	icon_lefttri       = 'l',
	icon_righttri      = 'r',
	icon_leftcaret     = '<',
	icon_rightcaret    = '>',
	icon_upcaret       = '^',
	icon_downcaret     = 'v',
	icon_refresh       = 'R',
	icon_undo          = '{',
	icon_redo          = '}',
	icon_machine       = 'm',
	icon_x             = 'x',
	icon_maximize      = 'w',
	icon_restore       = 'b',
	icon_minimize      = 'g',
	icon_lock          = 'p',
	icon_unlock        = 'q',
	icon_tag           = 'T',
	icon_save          = 'D',
	icon_gamepad       = 'G',
	icon_keyboard      = 'k',
	icon_unfilledcheck = '!',
	icon_filledcheck   = '1',
	icon_warning       = 'W',
	icon_question      = '?',
	icon_info          = 'I',
	icon_eye           = 'V',
	icon_eyecovered    = 'H',
};

// typedefs
struct ui_frame_t;
typedef void ui_frame_custom_draw_func(ui_frame_t*);

// enums

typedef u32 ui_frame_flags;
enum {

	// interaction
	ui_frame_flag_clickable = (1 << 1),
	ui_frame_flag_click_to_focus = (1 << 2),
	ui_frame_flag_scroll = (1 << 3),
	ui_frame_flag_view_scroll_x = (1 << 4),
	ui_frame_flag_view_scroll_y = (1 << 5),
	ui_frame_flag_view_clamp_x = (1 << 6),
	ui_frame_flag_view_clamp_y = (1 << 7),
	ui_frame_flag_focus_hot = (1 << 8),
	ui_frame_flag_focus_active = (1 << 9),
	ui_frame_flag_focus_skip = (1 << 10),

	// layout
	ui_frame_flag_fixed_width = (1 << 11),
	ui_frame_flag_fixed_height = (1 << 12),
	ui_frame_flag_floating_x = (1 << 13),
	ui_frame_flag_floating_y = (1 << 14),
	ui_frame_flag_overflow_x = (1 << 15),
	ui_frame_flag_overflow_y = (1 << 16),

	// appearance
	ui_frame_flag_clip = (1 << 17),
	ui_frame_flag_draw_text = (1 << 18),
	ui_frame_flag_draw_background_light = (1 << 19),
	ui_frame_flag_draw_background_dark = (1 << 20),
	ui_frame_flag_draw_border_light = (1 << 21),
	ui_frame_flag_draw_border_dark = (1 << 22),
	ui_frame_flag_draw_shadow = (1 << 23),
	ui_frame_flag_draw_hover_effects = (1 << 24),
	ui_frame_flag_draw_active_effects = (1 << 25),
	ui_frame_flag_draw_custom = (1 << 26),
	ui_frame_flag_custom_hover_cursor = (1 << 27),

	// groups
	ui_frame_flag_draw =
		ui_frame_flag_draw_text | ui_frame_flag_draw_background_dark |
	    ui_frame_flag_draw_border_dark | ui_frame_flag_draw_shadow |
		ui_frame_flag_draw_hover_effects | ui_frame_flag_draw_active_effects,

	ui_frame_flag_view_scroll = ui_frame_flag_view_scroll_x | ui_frame_flag_view_scroll_y,
	ui_frame_flag_view_clamp = ui_frame_flag_view_clamp_x | ui_frame_flag_view_clamp_y,

	ui_frame_flag_fixed_size = ui_frame_flag_fixed_width << ui_frame_flag_fixed_height,

	ui_frame_flag_floating = ui_frame_flag_floating_x | ui_frame_flag_floating_y,
	ui_frame_flag_overflow = ui_frame_flag_overflow_x | ui_frame_flag_overflow_y,
};

enum ui_size_type {
	ui_size_type_null,
	ui_size_type_pixel,
	ui_size_type_percent,
	ui_size_type_by_children,
};

enum ui_layout_axis {
	ui_layout_axis_x,
	ui_layout_axis_y,
};

enum ui_text_alignment {
	ui_text_alignment_left,
	ui_text_alignment_center,
	ui_text_alignment_right,
};

typedef u32 ui_interaction;
enum {
	ui_interaction_none = 0,
	ui_interaction_left_pressed = (1 << 0),
	ui_interaction_middle_pressed = (1 << 1),
	ui_interaction_right_pressed = (1 << 2),
	ui_interaction_left_dragging = (1 << 3),
	ui_interaction_middle_dragging = (1 << 4),
	ui_interaction_right_dragging = (1 << 5),
	ui_interaction_left_double_dragging = (1 << 6),
	ui_interaction_middle_double_dragging = (1 << 7),
	ui_interaction_right_double_dragging = (1 << 8),
	ui_interaction_left_triple_dragging = (1 << 9),
	ui_interaction_middle_triple_dragging = (1 << 10),
	ui_interaction_right_triple_dragging = (1 << 11),
	ui_interaction_left_released = (1 << 12),
	ui_interaction_middle_released = (1 << 13),
	ui_interaction_right_released = (1 << 14),
	ui_interaction_left_clicked = (1 << 15),
	ui_interaction_middle_clicked = (1 << 16),
	ui_interaction_right_clicked = (1 << 17),
	ui_interaction_left_double_clicked = (1 << 18),
	ui_interaction_middle_double_clicked = (1 << 19),
	ui_interaction_right_double_clicked = (1 << 20),
	ui_interaction_left_triple_clicked = (1 << 21),
	ui_interaction_middle_triple_clicked = (1 << 22),
	ui_interaction_right_triple_clicked = (1 << 23),
	ui_interaction_keyboard_pressed = (1 << 24),
	ui_interaction_hovered = (1 << 25),
};

enum ui_event_type {
	ui_event_type_null,
	ui_event_type_key_press,
	ui_event_type_key_release,
	ui_event_type_mouse_press,
	ui_event_type_mouse_release,
	ui_event_type_text,
	ui_event_type_navigate,
	ui_event_type_edit,
	ui_event_type_mouse_move,
	ui_event_type_mouse_scroll,
};

typedef u32 ui_event_flags;
enum {
	ui_event_flag_keep_mark = (1 << 0),
	ui_event_flag_delete = (1 << 1),
	ui_event_flag_copy = (1 << 2),
	ui_event_flag_paste = (1 << 3),
	ui_event_flag_pick_side = (1 << 4),
	ui_event_flag_zero_delta = (1 << 5),
};

enum ui_event_delta_unit {
	ui_event_delta_unit_null,
	ui_event_delta_unit_char,
	ui_event_delta_unit_word,
	ui_event_delta_unit_line,
	ui_event_delta_unit_page,
	ui_event_delta_unit_whole,
};

enum ui_focus_type {
	ui_focus_type_null,
	ui_focus_type_off,
	ui_focus_type_on,
	ui_focus_type_root,
};

typedef u32 ui_text_op_flags;
enum {
	ui_text_op_flag_invalid = (1 << 0),
	ui_text_op_flag_copy = (1 << 1),
};

// structs

struct ui_key_t {
	u64 data[1];
};

struct ui_size_t {
	ui_size_type type;
	f32 value;
	f32 strictness;
};

// TODO: come up with better naming system for colors.
struct ui_palette_t {
	color_t dark_background;
	color_t dark_border;
	color_t light_background;
	color_t light_border;
	color_t shadow;
	color_t hover;
	color_t active;
	color_t text;
	color_t accent;
};


struct ui_event_t {
	ui_event_t* next;
	ui_event_t* prev;

	ui_event_type type;
	ui_event_flags flags;
	ui_event_delta_unit delta_unit;
	os_key key;
	os_mouse_button mouse;
	os_modifiers modifiers;
	u32 character;
	vec2_t position;
	vec2_t scroll;
	ivec2_t delta;
};

struct ui_event_list_t {
	ui_event_t* first;
	ui_event_t* last;
	u32 count;
};

struct ui_event_binding_t {
	os_key key;
	os_modifiers modifiers;

	ui_event_type result_type;
	ui_event_flags result_flags;
	ui_event_delta_unit result_delta_unit;
	ivec2_t result_delta;
};

struct ui_text_point_t {
	i32 line;
	i32 column;
};

struct ui_text_range_t {
	ui_text_point_t min;
	ui_text_point_t max;
};

struct ui_text_op_t {
	ui_text_op_flags flags;
	str_t replace;
	str_t copy;
	ui_text_range_t range;
	ui_text_point_t cursor;
	ui_text_point_t mark;
};

struct ui_frame_t {

	// frame list
	ui_frame_t* hash_next;
	ui_frame_t* hash_prev;

	// frame tree
	ui_frame_t* tree_next;
	ui_frame_t* tree_prev;
	ui_frame_t* tree_parent;
	ui_frame_t* tree_child_first;
	ui_frame_t* tree_child_last;
	u32 child_count;

	// per frame data
	ui_key_t key;
	ui_frame_flags flags;
	str_t string;
	vec2_t fixed_position;
	vec2_t fixed_size;
	ui_size_t pref_width;
	ui_size_t pref_height;
	ui_text_alignment text_alignment;
	f32 text_padding;
	os_cursor hover_cursor;
	ui_layout_axis layout_axis;
	vec4_t rounding;
	ui_palette_t* palette;
	gfx_texture_t* texture;
	font_t* font;
	f32 font_size;
	ui_frame_custom_draw_func* custom_draw_func;
	void* custom_draw_data;

	// per frame layout
	rect_t rect;
	b8 is_transient;

	// persistant data
	u64 first_build_index;
	u64 last_build_index;
	f32 hover_t;
	f32 active_t;
	f32 disabled_t;
	f32 focus_hot_t;
	f32 focus_active_t;
	f32 focus_disabled_t;
	vec2_t view_offset;
	vec2_t view_offset_target;
	vec2_t view_bounds;
	ui_key_t nav_focus_hot_key;
	ui_key_t nav_focus_active_key;
	ui_key_t nav_focus_next_hot_key;
	ui_key_t nav_focus_next_active_key;

};

struct ui_frame_node_t {
	ui_frame_node_t* next;
	ui_frame_node_t* prev;
	ui_frame_t* frame;
};

struct ui_frame_list_t {
	ui_frame_node_t* first;
	ui_frame_node_t* last;
	u32 count;
};

struct ui_frame_rec_t {
	ui_frame_t* next;
	i32 push_count;
	i32 pop_count;
};

// stack nodes
ui_stack_node_decl(parent, ui_frame_t*)
ui_stack_node_decl(flags, ui_frame_flags)
ui_stack_node_decl(fixed_x, f32)
ui_stack_node_decl(fixed_y, f32)
ui_stack_node_decl(fixed_width, f32)
ui_stack_node_decl(fixed_height, f32)
ui_stack_node_decl(pref_width, ui_size_t)
ui_stack_node_decl(pref_height, ui_size_t)
ui_stack_node_decl(text_alignment, ui_text_alignment)
ui_stack_node_decl(text_padding, f32)
ui_stack_node_decl(hover_cursor, os_cursor)
ui_stack_node_decl(layout_axis, ui_layout_axis)
ui_stack_node_decl(rounding_00, f32)
ui_stack_node_decl(rounding_01, f32)
ui_stack_node_decl(rounding_10, f32)
ui_stack_node_decl(rounding_11, f32)
ui_stack_node_decl(palette, ui_palette_t*)
ui_stack_node_decl(texture, gfx_texture_t*)
ui_stack_node_decl(font, font_t*)
ui_stack_node_decl(font_size, f32)
ui_stack_node_decl(focus_hot, ui_focus_type)
ui_stack_node_decl(focus_active, ui_focus_type)

struct ui_constants_t {
	vec2_t window_size;
	vec2_t time;
};

struct ui_state_t {
	
	// contexts
	os_window_t* window;
	gfx_renderer_t* renderer;

	// arenas
	arena_t* frame_arena;
	arena_t* event_arena;
	arena_t* per_frame_arena;
	arena_t* drag_state_arena;
	arena_t* scratch_arena;

	// build index
	u64 build_index;

	// event bindings
	ui_event_binding_t event_bindings[64];

	// event list
	ui_event_list_t event_list;

	// input
	vec2_t mouse_pos;
	vec2_t mouse_delta;

	u32 click_counter[3];
	u64 last_click_time[3];
	u32 last_click_index[3];

	// text cursor and mark
	ui_text_point_t cursor;
	ui_text_point_t mark;
	vec2_t cursor_pos;
	vec2_t cursor_target_pos;
	vec2_t mark_pos;
	vec2_t mark_target_pos;

	// drag state
	void* drag_state_data;
	u32 drag_state_size;

	// frame list
	ui_frame_t* frame_first;
	ui_frame_t* frame_last;
	ui_frame_t* frame_free;
	
	// state
	ui_key_t hovered_frame_key;
	ui_key_t active_frame_key[os_mouse_button_count];
	ui_key_t nav_root_key;
	ui_key_t focused_frame_key;
		
	// frame tree
	ui_frame_t* root;

	// assets
	gfx_shader_t* ui_shader;
	ui_constants_t constants;

	// defaults
	ui_palette_t default_palette;
	gfx_texture_t* default_texture;
	font_t* default_font;
	font_t* default_icon_font;

	// stack defaults
	ui_stack_decl_default(parent);
	ui_stack_decl_default(flags);
	ui_stack_decl_default(fixed_x);
	ui_stack_decl_default(fixed_y);
	ui_stack_decl_default(fixed_width);
	ui_stack_decl_default(fixed_height);
	ui_stack_decl_default(pref_width);
	ui_stack_decl_default(pref_height);
	ui_stack_decl_default(text_alignment);
	ui_stack_decl_default(text_padding);
	ui_stack_decl_default(hover_cursor);
	ui_stack_decl_default(layout_axis);
	ui_stack_decl_default(rounding_00);
	ui_stack_decl_default(rounding_01);
	ui_stack_decl_default(rounding_10);
	ui_stack_decl_default(rounding_11);
	ui_stack_decl_default(palette);
	ui_stack_decl_default(texture);
	ui_stack_decl_default(font);
	ui_stack_decl_default(font_size);
	ui_stack_decl_default(focus_hot);
	ui_stack_decl_default(focus_active);

	// stacks
	ui_stack_decl(parent);
	ui_stack_decl(flags);
	ui_stack_decl(fixed_x);
	ui_stack_decl(fixed_y);
	ui_stack_decl(fixed_width);
	ui_stack_decl(fixed_height);
	ui_stack_decl(pref_width);
	ui_stack_decl(pref_height);
	ui_stack_decl(text_alignment);
	ui_stack_decl(text_padding);
	ui_stack_decl(hover_cursor);
	ui_stack_decl(layout_axis);
	ui_stack_decl(rounding_00);
	ui_stack_decl(rounding_01);
	ui_stack_decl(rounding_10);
	ui_stack_decl(rounding_11);
	ui_stack_decl(palette);
	ui_stack_decl(texture);
	ui_stack_decl(font);
	ui_stack_decl(font_size);
	ui_stack_decl(focus_hot);
	ui_stack_decl(focus_active);

};


// widget structs

struct ui_slider_data_t {
	f32 value;
};

struct ui_color_data_t {
	f32 hue;
	f32 sat;
	f32 val;
};


// globals

global ui_state_t ui_state;

// functions

// state
function void ui_init();
function void ui_release();
function void ui_begin_frame(gfx_renderer_t*);
function void ui_end_frame();

// widgets

function ui_interaction ui_button(str_t);
function ui_interaction ui_buttonf(char*, ...);
function ui_interaction ui_label(str_t);
function ui_interaction ui_labelf(char*, ...);
function ui_interaction ui_slider(str_t, f32*, f32, f32);
function ui_interaction ui_checkbox(str_t, b8*);
function ui_interaction ui_expander(str_t, b8*);
function ui_interaction ui_color_sat_val_quad(str_t, f32, f32*, f32*);
function ui_interaction ui_color_hue_bar(str_t, f32*, f32, f32);
function ui_interaction ui_color_wheel(str_t, f32*, f32*, f32*);
function ui_interaction ui_color_hue_sat_circle(str_t, f32*, f32*, f32*);
function ui_interaction ui_color_val_bar(str_t, f32, f32, f32*);
function ui_interaction ui_text_edit(str_t, char*, u32, u32*);
function ui_interaction ui_combo(str_t, i32*, char**, u32);

// widget draw functions
function void ui_slider_draw_function(ui_frame_t*);
function void ui_color_hue_bar(ui_frame_t*);
function void ui_color_sat_val_quad_draw_function(ui_frame_t*);
function void ui_color_wheel_draw_function(ui_frame_t*);
function void ui_color_hue_sat_circle_draw_function(ui_frame_t*);
function void ui_color_val_bar_draw_function(ui_frame_t*);
function void ui_text_edit_draw_function(ui_frame_t*);

// string
function str_t ui_string_display_format(str_t);
function str_t ui_string_hash_part(str_t);
function str_t ui_string_replace_range(arena_t*, str_t, ui_text_range_t, str_t);
function i32   ui_string_find_word_index(str_t, i32, i32);
function void  ui_string_find_word_boundaries(str_t, i32, i32*, i32*);

// key
function ui_key_t ui_key_from_string(ui_key_t, str_t);
function ui_key_t ui_key_from_stringf(ui_key_t, char*, ...);
function b8 ui_key_equals(ui_key_t, ui_key_t);

// size
function ui_size_t ui_size(ui_size_type, f32, f32);
function ui_size_t ui_size_pixel(f32, f32);
function ui_size_t ui_size_percent(f32);
function ui_size_t ui_size_by_child(f32);
function ui_size_t ui_size_em(f32, f32);

// alignment
function vec2_t ui_text_align(font_t*, f32, str_t, rect_t, ui_text_alignment);
function f32 ui_text_offset_from_index(font_t*, f32, str_t, u32);
function u32 ui_text_index_from_offset(font_t*, f32, str_t, f32);

// text point
function b8 ui_text_point_equals(ui_text_point_t, ui_text_point_t);
function b8 ui_text_point_less_than(ui_text_point_t, ui_text_point_t);
function ui_text_point_t ui_text_point_min(ui_text_point_t, ui_text_point_t);
function ui_text_point_t ui_text_point_max(ui_text_point_t, ui_text_point_t);

// text range
function ui_text_range_t ui_text_range(ui_text_point_t, ui_text_point_t);
function ui_text_range_t ui_text_range_intersects(ui_text_range_t, ui_text_range_t);
function ui_text_range_t ui_text_range_union(ui_text_range_t, ui_text_range_t);
function b8 ui_text_range_contains(ui_text_range_t, ui_text_point_t);

// events
function void ui_event_push(ui_event_t*);
function void ui_event_pop(ui_event_t*);
function b8 ui_key_pressed(os_key, os_modifiers);
function b8 ui_key_released(os_key, os_modifiers);
function b8 ui_text(u32);
function ui_event_binding_t* ui_event_get_binding(os_key, os_modifiers);
function ui_text_op_t ui_event_to_text_op(arena_t*, ui_event_t*, str_t, ui_text_point_t, ui_text_point_t);

// drag state
function void ui_store_drag_data(void*, u32);
function void* ui_get_drag_data();
function void ui_clear_drag_data();

// layout 
function void ui_layout_solve_independent(ui_frame_t*);
function void ui_layout_solve_upward_dependent(ui_frame_t*);
function void ui_layout_solve_downward_dependent(ui_frame_t*);
function void ui_layout_solve_violations(ui_frame_t*);
function void ui_layout_solve_set_positions(ui_frame_t*);

function ui_frame_t* ui_row_begin();
function ui_interaction ui_row_end();
function ui_frame_t* ui_column_begin();
function ui_interaction ui_column_end();

// frames
function ui_frame_t* ui_frame_find(ui_key_t);
function ui_frame_t* ui_frame_from_key(ui_key_t, ui_frame_flags = 0);
function ui_frame_t* ui_frame_from_string(str_t, ui_frame_flags = 0);
function ui_frame_rec_t ui_frame_rec_depth_first(ui_frame_t*, ui_frame_t*, u32, u32);
function ui_interaction ui_frame_interaction(ui_frame_t*);
function void ui_frame_set_display_text(ui_frame_t*, str_t);
function void ui_frame_set_custom_draw(ui_frame_t*, ui_frame_custom_draw_func*, void*);

// frame list
function void ui_frame_list_push(arena_t*, ui_frame_list_t*, ui_frame_t*);

// stack
function void ui_auto_pop_stacks();
ui_stack_func(parent, ui_frame_t*)
ui_stack_func(flags, ui_frame_flags)
ui_stack_func(fixed_x, f32)
ui_stack_func(fixed_y, f32)
ui_stack_func(fixed_width, f32)
ui_stack_func(fixed_height, f32)
ui_stack_func(pref_width, ui_size_t)
ui_stack_func(pref_height, ui_size_t)
ui_stack_func(text_alignment, ui_text_alignment)
ui_stack_func(text_padding, f32)
ui_stack_func(hover_cursor, os_cursor)
ui_stack_func(layout_axis, ui_layout_axis)
ui_stack_func(rounding_00, f32)
ui_stack_func(rounding_01, f32)
ui_stack_func(rounding_10, f32)
ui_stack_func(rounding_11, f32)
ui_stack_func(palette, ui_palette_t*)
ui_stack_func(texture, gfx_texture_t*)
ui_stack_func(font, font_t*)
ui_stack_func(font_size, f32)
ui_stack_func(focus_hot, ui_focus_type)
ui_stack_func(focus_active, ui_focus_type)

// groups
function void ui_push_rounding(f32);
function void ui_pop_rounding();
function void ui_set_next_rounding(f32);

#endif // UI_H