// ui.h

#ifndef UI_H
#define UI_H

// todo: 
// 
// [ ] - find memory leak.
// [~] - fix color palette situation.
// [~] - maybe change up the style system all together.
//     [~] - global way to turn off borders, shadows, etc.
//     [~] - can more easily change style and colors of individual wigets.  
// [~] - add panels.
//     [x] - calculating rects.
//     [x] - building panel tree.
//     [x] - resizing.
//     [x] - good api
//     [x] - clamp sizes.
//	   [ ] - add tabs.
//         [ ] - implement views.
//         [ ] - add views to panel list.
//         [ ] - 
// [~] - *fix clipping.
// [ ] - *support fancier text (colored)
// [ ] - deal with tabs in fonts correctly.
// [ ] - scroll bars.
// [x] - tooltips.
// [x] - cutoff text (...).
// [~] - widgets. 
//     [~] - textbox. // needs some touch ups
//         [x] - keyboard controls.
//         [ ] - mouse controls.
//     [ ] - tree list.
//     [ ] - tool tips.
//     [ ] - list box.
//     [~] - combo box.
// [~] - more ui events.
//     [x] - scrolling.
//     [x] - keyboard.
//     [x] - text ops
//     [ ] - nagivation. 
//     [x] - double and triple click.
// [ ] - frame focusing.
// [~] - clean up pass.
//     [ ] - color picker indicator animations.
//     [ ] - 
// 
// * - needs to be added to draw layer.
//

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

// enums

// icons
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

enum ui_size_type {
	ui_size_type_null,
	ui_size_type_pixel, // fixed pixel size
	ui_size_type_percent, // percent of parent's size
	ui_size_type_by_children, // children's size
	ui_size_type_text, // text size
};

typedef u32 ui_axis;
enum {
	ui_axis_x,
	ui_axis_y,
	ui_axis_count,
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

enum ui_color {
	ui_color_background,
	ui_color_text,
	ui_color_border,
	ui_color_hover,
	ui_color_active,
	ui_color_shadow,
	ui_color_accent,
	ui_color_count,
};

enum ui_color_group {
	ui_color_group_default,
	ui_color_group_label,
	ui_color_group_button,
	ui_color_group_slider,
	ui_color_group_checkbox,
	ui_color_group_expander,
	ui_color_group_number_edit,
	ui_color_group_color_picker,
	ui_color_group_text_edit,
	ui_color_group_combo_box,
	ui_color_group_panel,
	ui_color_group_view_tab,
	ui_color_group_tooltip,
	ui_color_group_popup,
	ui_color_group_count,
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

union ui_color_group_t {

	color_t colors[ui_color_count];

	struct {
		color_t background;
		color_t text;
		color_t border;
		color_t hover;
		color_t active;
		color_t shadow;
		color_t accent;
	};
};

struct ui_theme_t {

	// style
	b8 borders;
	b8 shadows;
	f32 rounding;
	f32 padding;

	// TODO: add more padding options (panel padding, text padding, widget padding).

	// colors
	union {
		ui_color_group_t groups[ui_color_group_count];

		struct {
			ui_color_group_t default;
			ui_color_group_t label;
			ui_color_group_t button;
			ui_color_group_t slider;
			ui_color_group_t checkbox;
			ui_color_group_t expander;
			ui_color_group_t number_edit;
			ui_color_group_t color_picker;
			ui_color_group_t text_edit;
			ui_color_group_t combo_box;
			ui_color_group_t panel;
			ui_color_group_t view_tab;
			ui_color_group_t tooltip;
			ui_color_group_t popup;
		};
	};
};

struct ui_event_t {
	ui_event_t* next;
	ui_event_t* prev;

	os_event_t* os_event;
	os_window_t* window;
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

// frame

struct ui_frame_t;
typedef void ui_frame_custom_draw_func(ui_frame_t*);

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
	ui_frame_flag_draw_background = (1 << 19),
	ui_frame_flag_draw_border = (1 << 20),
	ui_frame_flag_draw_shadow = (1 << 21),
	ui_frame_flag_draw_hover_effects = (1 << 22),
	ui_frame_flag_draw_active_effects = (1 << 23),
	ui_frame_flag_draw_custom = (1 << 24),
	ui_frame_flag_custom_hover_cursor = (1 << 25),

	// groups
	ui_frame_flag_draw =
	ui_frame_flag_draw_text | ui_frame_flag_draw_background |
	ui_frame_flag_draw_border | ui_frame_flag_draw_shadow |
	ui_frame_flag_draw_hover_effects | ui_frame_flag_draw_active_effects,

	ui_frame_flag_view_scroll = ui_frame_flag_view_scroll_x | ui_frame_flag_view_scroll_y,
	ui_frame_flag_view_clamp = ui_frame_flag_view_clamp_x | ui_frame_flag_view_clamp_y,

	ui_frame_flag_fixed_size = ui_frame_flag_fixed_width << ui_frame_flag_fixed_height,

	ui_frame_flag_floating = ui_frame_flag_floating_x | ui_frame_flag_floating_y,
	ui_frame_flag_overflow = ui_frame_flag_overflow_x | ui_frame_flag_overflow_y,
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
	ui_size_t pref_size[2];
	ui_text_alignment text_alignment;
	f32 text_padding;
	os_cursor hover_cursor;
	ui_axis layout_axis;
	vec4_t rounding;
	ui_color_group_t color_group;
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

struct ui_frame_rec_t {
	ui_frame_t* next;
	i32 push_count;
	i32 pop_count;
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

// stack nodes
ui_stack_node_decl(parent, ui_frame_t*)
ui_stack_node_decl(flags, ui_frame_flags)
ui_stack_node_decl(seed_key, ui_key_t)
ui_stack_node_decl(fixed_x, f32)
ui_stack_node_decl(fixed_y, f32)
ui_stack_node_decl(fixed_width, f32)
ui_stack_node_decl(fixed_height, f32)
ui_stack_node_decl(pref_width, ui_size_t)
ui_stack_node_decl(pref_height, ui_size_t)
ui_stack_node_decl(text_alignment, ui_text_alignment)
ui_stack_node_decl(text_padding, f32)
ui_stack_node_decl(hover_cursor, os_cursor)
ui_stack_node_decl(layout_axis, ui_axis)
ui_stack_node_decl(rounding_00, f32)
ui_stack_node_decl(rounding_01, f32)
ui_stack_node_decl(rounding_10, f32)
ui_stack_node_decl(rounding_11, f32)
ui_stack_node_decl(color_group, ui_color_group_t)
ui_stack_node_decl(font, font_t*)
ui_stack_node_decl(font_size, f32)
ui_stack_node_decl(focus_hot, ui_focus_type)
ui_stack_node_decl(focus_active, ui_focus_type)
ui_stack_node_decl(texture, gfx_texture_t*)

// view

typedef void view_ui_function(void);

struct ui_view_t {

	// list of all views
	ui_view_t* list_next;
	ui_view_t* list_prev;

	// list of views just in a panel
	ui_view_t* next;
	ui_view_t* prev;
	
	// info
	str_t label;
	view_ui_function* ui_function;

};

// panel

struct ui_panel_t {
	ui_panel_t* next;
	ui_panel_t* prev;
	ui_panel_t* child_first;
	ui_panel_t* child_last;
	ui_panel_t* parent;
	u32 child_count;

	// info
	f32 percent_of_parent;
	ui_axis split_axis;

	// frame
	ui_frame_t* frame;

	// views
	ui_view_t* view_first;
	ui_view_t* view_last;
	ui_view_t* view_focus;
};

struct ui_panel_rec_t {
	ui_panel_t* next;
	u32 push_count;
	u32 pop_count;
};

struct ui_panel_rect_t {
	ui_panel_rect_t* next;
	rect_t rect;
};

// context

struct ui_context_t {
	ui_context_t* next;
	ui_context_t* prev;

	// contexts
	os_window_t* window;
	gfx_renderer_t* renderer;

	// arenas
	arena_t* arena;
	arena_t* event_arena;
	arena_t* per_frame_arena;
	arena_t* drag_state_arena;
	arena_t* scratch_arena;

	// state
	u64 build_index;
	b8 tooltip_open;

	// text cursor and mark
	ui_text_point_t cursor;
	ui_text_point_t mark;
	vec2_t cursor_pos;
	vec2_t cursor_target_pos;
	vec2_t mark_pos;
	vec2_t mark_target_pos;

	// input
	vec2_t mouse_pos;
	vec2_t mouse_delta;

	// drag state
	void* drag_state_data;
	u32 drag_state_size;
	vec2_t drag_start_pos;

	// frame list
	ui_frame_t* frame_first;
	ui_frame_t* frame_last;
	ui_frame_t* frame_free;

	// state
	ui_key_t hovered_frame_key;
	ui_key_t active_frame_key[os_mouse_button_count];
	ui_key_t nav_root_key;
	ui_key_t focused_frame_key;

	// panel
	ui_panel_t* panel_free;
	ui_panel_t* panel_root;
	ui_panel_t* panel_focused;
	ui_panel_rect_t* panel_rect_top;
	ui_panel_rect_t* panel_rect_free;

	// frame tree
	ui_frame_t* root;
	ui_frame_t* tooltip_root;
	ui_frame_t* context_root;

	// defaults
	ui_theme_t theme;
	font_t* font;
	font_t* icon_font;

	// stack defaults
	ui_stack_decl_default(parent);
	ui_stack_decl_default(flags);
	ui_stack_decl_default(seed_key);
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
	ui_stack_decl_default(color_group);
	ui_stack_decl_default(font);
	ui_stack_decl_default(font_size);
	ui_stack_decl_default(focus_hot);
	ui_stack_decl_default(focus_active);
	ui_stack_decl_default(texture);

	// stacks
	ui_stack_decl(parent);
	ui_stack_decl(flags);
	ui_stack_decl(seed_key);
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
	ui_stack_decl(color_group);
	ui_stack_decl(font);
	ui_stack_decl(font_size);
	ui_stack_decl(focus_hot);
	ui_stack_decl(focus_active);
	ui_stack_decl(texture);


};

// state

struct ui_state_t {
	
	arena_t* arena;	
	arena_t* event_arena;

	ui_context_t* ui_first;
	ui_context_t* ui_last;
	ui_context_t* ui_free;
	ui_context_t* ui_active;

	// event list
	ui_event_list_t event_list;

	// default theme
	ui_theme_t default_theme;

	// event bindings
	ui_event_binding_t event_bindings[64];

	u32 click_counter[3];
	u64 last_click_time[3];
	u32 last_click_index[3];

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
function void ui_update();

// context
function ui_context_t* ui_context_create(gfx_renderer_t* renderer);
function void ui_context_release(ui_context_t* context);
function void ui_begin_frame(ui_context_t* context);
function void ui_end_frame(ui_context_t* context);

// string
function str_t ui_string_display_format(str_t string);
function str_t ui_string_hash_format(str_t string);
function str_t ui_string_replace_range(arena_t* arena, str_t string, ui_text_range_t range, str_t replace);
function i32   ui_string_find_word_index(str_t string, i32 start_index, i32 end_index);
function void  ui_string_find_word_boundaries(str_t string, i32 index, i32* word_start, i32* word_end);

// key
function u64 ui_hash_string(u64 seed, str_t string);
function ui_key_t ui_key_from_string(ui_key_t seed_key, str_t string);
function ui_key_t ui_key_from_stringf(ui_key_t seed_key, char* fmt, ...);
function b8 ui_key_equals(ui_key_t a, ui_key_t b);

// size
function ui_size_t ui_size(ui_size_type type, f32 value, f32 strictness);
function ui_size_t ui_size_pixel(f32 value, f32 strictness);
function ui_size_t ui_size_percent(f32 percent);
function ui_size_t ui_size_by_child(f32 strictness);
function ui_size_t ui_size_em(f32 value, f32 strictness);
function ui_size_t ui_size_text(f32 padding);

// text alignment
function vec2_t ui_text_align(font_t* font, f32 font_size, str_t text, rect_t rect, ui_text_alignment alignment);
function vec2_t ui_text_size(font_t* font, f32 font_size, str_t text);
function f32 ui_text_offset_from_index(font_t* font, f32 font_size, str_t text, u32 index);
function u32 ui_text_index_from_offset(font_t* font, f32 font_size, str_t text, f32 offset);

// text point
function b8 ui_text_point_equals(ui_text_point_t a, ui_text_point_t b);
function b8 ui_text_point_less_than(ui_text_point_t a, ui_text_point_t b);
function ui_text_point_t ui_text_point_min(ui_text_point_t a, ui_text_point_t b);
function ui_text_point_t ui_text_point_max(ui_text_point_t a, ui_text_point_t b);

// text range
function ui_text_range_t ui_text_range(ui_text_point_t min, ui_text_point_t max);
function ui_text_range_t ui_text_range_intersects(ui_text_range_t a, ui_text_range_t b);
function ui_text_range_t ui_text_range_union(ui_text_range_t a, ui_text_range_t b);
function b8 ui_text_range_contains(ui_text_range_t r, ui_text_point_t pt);

// events
function void ui_event_push(ui_event_t* event);
function void ui_event_pop(ui_event_t* event);
function b8 ui_key_pressed(os_key key, os_modifiers modifiers);
function b8 ui_key_released(os_key key, os_modifiers modifiers);
function b8 ui_text(u32 codepoint);
function ui_event_binding_t* ui_event_get_binding(os_key key, os_modifiers modifiers);
function ui_text_op_t ui_event_to_text_op(arena_t* arena, ui_event_t* event, str_t string, ui_text_point_t cursor, ui_text_point_t mark);

// drag state
function void ui_store_drag_data(void* data, u32 size);
function void* ui_get_drag_data();
function vec2_t ui_get_drag_delta();
function void ui_clear_drag_data();

// layout 
function void ui_layout_solve_independent(ui_frame_t* frame, ui_axis axis);
function void ui_layout_solve_upward_dependent(ui_frame_t* frame, ui_axis axis);
function void ui_layout_solve_downward_dependent(ui_frame_t* frame, ui_axis axis);
function void ui_layout_solve_violations(ui_frame_t* frame, ui_axis axis);
function void ui_layout_solve_set_positions(ui_frame_t* frame, ui_axis axis);

function ui_frame_t* ui_row_begin();
function ui_interaction ui_row_end();
function ui_frame_t* ui_column_begin();
function ui_interaction ui_column_end();

// frames
function ui_frame_t* ui_frame_find(ui_key_t key);
function ui_frame_t* ui_frame_from_key(ui_key_t key, ui_frame_flags flags = 0);
function ui_frame_t* ui_frame_from_string(str_t string, ui_frame_flags flags = 0);
function ui_frame_rec_t ui_frame_rec_depth_first(ui_frame_t* frame);
function ui_interaction ui_frame_interaction(ui_frame_t* frame);
function void ui_frame_set_display_text(ui_frame_t* frame, str_t display_text);
function void ui_frame_set_custom_draw(ui_frame_t* frame, ui_frame_custom_draw_func* draw_function, void* draw_data);

// frame list
function void ui_frame_list_push(arena_t* arena, ui_frame_list_t* frame_list, ui_frame_t* frame);

// views
function ui_view_t* ui_view_create(str_t label, view_ui_function* ui_function);
function void ui_view_release(ui_view_t* view);


// panels
function ui_panel_t* ui_panel_create(ui_context_t* context, f32 percent = 0.5f, ui_axis split_axis = ui_axis_x);
function void ui_panel_release(ui_context_t*, ui_panel_t*);
function ui_frame_t* ui_panel_begin(ui_panel_t* panel);
function void ui_panel_end();
function void ui_panel_insert(ui_panel_t* parent, ui_panel_t* panel, ui_panel_t* prev);
function void ui_panel_remove(ui_panel_t* parent, ui_panel_t* panel);
function ui_panel_rec_t ui_panel_rec_depth_first(ui_panel_t* panel);
function rect_t ui_rect_from_panel_child(ui_panel_t* panel, rect_t rect);
function rect_t ui_rect_from_panel(arena_t* scratch, ui_panel_t* panel, rect_t root_rect);

// widgets

function ui_interaction ui_label(str_t label);
function ui_interaction ui_labelf(char* fmt, ...);
function ui_interaction ui_button(str_t label);
function ui_interaction ui_buttonf(char* fmt, ...);
function ui_interaction ui_image(str_t label, gfx_texture_t* texture);
function ui_interaction ui_slider(str_t label, i32* value, i32 min, i32 max);
function ui_interaction ui_slider(str_t label, f32* value, f32 min, f32 max);
function ui_interaction ui_checkbox(str_t label, b8* value);
function ui_interaction ui_expander(str_t label, b8* is_expanded);
function ui_interaction ui_float_edit(str_t label, f32* value);
function ui_interaction ui_color_sat_val_quad(str_t label, f32 hue, f32* sat, f32* val);
function ui_interaction ui_color_hue_bar(str_t label, f32* hue, f32 sat, f32 val);
function ui_interaction ui_color_sat_bar(str_t label, f32 hue, f32* sat, f32 val);
function ui_interaction ui_color_val_bar(str_t label, f32 hue, f32 sat, f32* val);
function ui_interaction ui_color_wheel(str_t label, f32* hue, f32* sat, f32* val);
function ui_interaction ui_color_hue_sat_circle(str_t label, f32* hue, f32* sat, f32 val);
function ui_interaction ui_color_hex(str_t label, f32* hue, f32* sat, f32 val);
function ui_interaction ui_text_edit(str_t label, char* buffer, u32 buffer_max_size, u32* out_size);
function ui_interaction ui_combo(str_t label, i32* current, char** items, u32 item_count);

// widget draw functions
function void ui_slider_draw_function(ui_frame_t* frame);

function void ui_color_hue_bar_draw_function(ui_frame_t* frame);
function void ui_color_sat_bar_draw_function(ui_frame_t* frame);
function void ui_color_val_bar_draw_function(ui_frame_t* frame);

function void ui_color_sat_val_quad_draw_function(ui_frame_t* frame);
function void ui_color_wheel_draw_function(ui_frame_t* frame);
function void ui_color_hue_sat_circle_draw_function(ui_frame_t* frame);
function void ui_color_hex_draw_function(ui_frame_t* frame);

function void ui_text_edit_draw_function(ui_frame_t* frame);

// stacks
function void ui_auto_pop_stacks();
ui_stack_func(parent, ui_frame_t*)
ui_stack_func(flags, ui_frame_flags)
ui_stack_func(seed_key, ui_key_t)
ui_stack_func(fixed_x, f32)
ui_stack_func(fixed_y, f32)
ui_stack_func(fixed_width, f32)
ui_stack_func(fixed_height, f32)
ui_stack_func(pref_width, ui_size_t)
ui_stack_func(pref_height, ui_size_t)
ui_stack_func(text_alignment, ui_text_alignment)
ui_stack_func(text_padding, f32)
ui_stack_func(hover_cursor, os_cursor)
ui_stack_func(layout_axis, ui_axis)
ui_stack_func(rounding_00, f32)
ui_stack_func(rounding_01, f32)
ui_stack_func(rounding_10, f32)
ui_stack_func(rounding_11, f32)
ui_stack_func(color_group, ui_color_group_t)
ui_stack_func(font, font_t*)
ui_stack_func(font_size, f32)
ui_stack_func(focus_hot, ui_focus_type)
ui_stack_func(focus_active, ui_focus_type)
ui_stack_func(texture, gfx_texture_t*)

// groups
function void ui_push_rounding(f32 rounding);
function void ui_pop_rounding();
function void ui_set_next_rounding(f32 rounding);

function void ui_push_rect(rect_t rect);
function void ui_pop_rect();
function void ui_set_next_rect(rect_t rect);

// TODO: color var 
function void ui_push_color_var(ui_color color_id, color_t color);
function void ui_pop_color_var(ui_color color_id);
function void ui_set_next_color_var(ui_color color_id, color_t color);


#endif // UI_H