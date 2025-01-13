// ui.h

#ifndef UI_H
#define UI_H

// todo:
//
// big stuff:
// 
// [~] - finish panel and tabs.
//     [x] - close panels.
//     [x] - split panels.
//     [x] - implement tabs. 
//     [ ] - drag tabs to different panels.
// [x] - investigate ui animation system to animate things.
//     [x] - animation cache.
//     [x] - animate pos and size of frames.
// [ ] - support different directions in layout algorithm.
// [ ] - truncate long text (...).
// 
// minor stuff:
// 
// [ ] - style pass of everything.
// [x] - reduce memory arenas.
// [x] - rewrite theming system again. :(
// [ ] - add support for custom interaction shapes (circles, tris, etc).
// [ ] - investigate some layout scaling issues. (fixed spacer's between widgets).
// [x] - *fix clipping.
// [ ] - *support fancier text (colored)
// [~] - widgets. 
//     [~] - textbox. // needs some touch ups
//         [x] - keyboard controls.
//         [ ] - mouse controls.
//     [ ] - tree list.
//     [ ] - list box.
//     [~] - combo box.
//	   [ ] - scroll bars.
//     [x] - tooltips.
//     [ ] - popups.
// [~] - more ui events.
//     [x] - scrolling.
//     [x] - keyboard.
//     [x] - text ops
//     [ ] - nagivation. 
//     [x] - double and triple click.
// [x] - frame focusing.
// [~] - clean up pass.
//     [x] - color picker indicator animations.
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
	icon_user = ' ',
	icon_checkmark = '!',
	icon_cancel = '"',
	icon_plus = '#',
	icon_minus = '$',
	icon_help = '%',
	icon_info = '&',
	icon_home = '\'',
	icon_lock = '(',
	icon_lock_open = ')',
	icon_eye = '*',
	icon_eye_off = '+',
	icon_tag = ',',
	icon_tags = '-',
	icon_bookmark = '.',
	icon_bookmark_empty = '/',
	icon_flag = '0',
	icon_flag_empty = '1',
	icon_reply = '2',
	icon_reply_all = '3',
	icon_forward = '4',
	icon_pencil = '5',
	icon_repeat = '6',
	icon_attention = '7',
	icon_trash = '8',
	icon_document = '9',
	icon_document_text = ':',
	icon_folder = ';',
	icon_folder_open = '<',
	icon_box = '=',
	icon_menu = '>',
	icon_cog = '?',
	icon_cog_alt = '@',
	icon_wrench = 'A',
	icon_sliders = 'B',
	icon_block = 'C',
	icon_resize_full = 'D',
	icon_resize_full_alt = 'E',
	icon_resize_small = 'F',
	icon_resize_vertical = 'G',
	icon_resize_horizontal = 'H',
	icon_move = 'I',
	icon_zoom_in = 'J',
	icon_zoom_out = 'K',
	icon_down = 'L',
	icon_up = 'M',
	icon_left = 'N',
	icon_right = 'O',
	icon_down_open = 'P',
	icon_up_open = 'Q',
	icon_left_open = 'R',
	icon_right_open = 'S',
	icon_arrow_cw = 'T',
	icon_arrow_ccw = 'U',
	icon_arrows_cw = 'V',
	icon_shuffle = 'W',
	icon_play = 'X',
	icon_stop = 'Y',
	icon_pause = 'Z',
	icon_to_end = '[',
	icon_to_end_alt = '\\',
	icon_to_start = ']',
	icon_to_start_alt = '^',
	icon_fast_foward = '_',
	icon_fast_backward = '`',
	icon_desktop = 'a',
	icon_align_left = 'b',
	icon_align_center = 'c',
	icon_align_right = 'd',
	icon_align_justify = 'e',
	icon_list = 'f',
	icon_indent_left = 'g',
	icon_indent_right = 'h',
	icon_list_bullet = 'i',
	icon_ellipsis = 'j',
	icon_ellipsis_vertical = 'k',
	icon_off = 'l',
	icon_circle_fill = 'm',
	icon_circle = 'n',
	icon_sort = 'o',
	icon_sort_down = 'p',
	icon_sort_up = 'q',
	icon_sort_up_alt = 'r',
	icon_sort_down_alt = 's',
	icon_sort_name_up = 't',
	icon_sort_name_down = 'u',
	icon_sort_number_up = 'v',
	icon_sort_number_down = 'w',
	icon_sitemap = 'x',
	icon_cube = 'y',
	icon_cubes = 'z',
	icon_database = '{',
	icon_eyecropper = '|',
	icon_brush = '}',
};

enum ui_size_type {
	ui_size_type_null,
	ui_size_type_pixel, // by a fixed pixel size
	ui_size_type_percent, // by a percent of parent's size
	ui_size_type_by_children, // by children's size
	ui_size_type_text, // by text size
};

typedef u32 ui_axis;
enum {
	ui_axis_x,
	ui_axis_y,
	ui_axis_count,
};

typedef u32 ui_dir;
enum {
	ui_dir_left,
	ui_dir_up,
	ui_dir_right,
	ui_dir_down,
};

typedef u32 ui_side;
enum {
	ui_side_min,
	ui_side_max,
};

enum ui_text_alignment {
	ui_text_alignment_left,
	ui_text_alignment_center,
	ui_text_alignment_right,
	ui_text_alignment_justify, // TODO: implement this
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

enum ui_cmd_type {
	ui_cmd_type_split_panel,
	ui_cmd_type_close_panel,
};

typedef u32 ui_anim_flags;
enum {
	ui_anim_flag_none,
	ui_anim_flag_wrap,
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

union ui_palette_t {

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
	b8 frame_borders;
	b8 frame_shadows;
	b8 text_shadows;
	f32 rounding;
	f32 padding;
	f32 shadow_size;
	f32 border_size;

	// TODO: add more padding options (panel padding, text padding, widget padding).
	ui_palette_t pallete;

};

// events
struct ui_event_t {
	ui_event_t* next;
	ui_event_t* prev;

	os_event_t* os_event;
	os_handle_t window;
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

typedef u64 ui_frame_flags;
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
	ui_frame_flag_ignore_view_scroll_x = (1 << 17),
	ui_frame_flag_ignore_view_scroll_y = (1 << 18),

	// appearance
	ui_frame_flag_clip = (1 << 19),
	ui_frame_flag_draw_text = (1 << 20),
	ui_frame_flag_draw_background = (1 << 21),
	ui_frame_flag_draw_border = (1 << 22),
	ui_frame_flag_draw_shadow = (1 << 23),
	ui_frame_flag_draw_hover_effects = (1 << 24),
	ui_frame_flag_draw_active_effects = (1 << 25),
	ui_frame_flag_draw_custom = (1 << 26),
	ui_frame_flag_custom_hover_cursor = (1 << 27),
	ui_frame_flag_anim_pos_x = (1 << 28),
	ui_frame_flag_anim_pos_y = (1 << 29),
	ui_frame_flag_anim_width = (1 << 30),
	ui_frame_flag_anim_height = (1 << 31),

	// groups
	ui_frame_flag_draw =
	ui_frame_flag_draw_text | ui_frame_flag_draw_background |
	ui_frame_flag_draw_border | ui_frame_flag_draw_shadow |
	ui_frame_flag_draw_hover_effects | ui_frame_flag_draw_active_effects,

	ui_frame_flag_view_scroll = ui_frame_flag_view_scroll_x | ui_frame_flag_view_scroll_y,
	ui_frame_flag_view_clamp = ui_frame_flag_view_clamp_x | ui_frame_flag_view_clamp_y,

	ui_frame_flag_fixed_size = ui_frame_flag_fixed_width | ui_frame_flag_fixed_height,

	ui_frame_flag_floating = ui_frame_flag_floating_x | ui_frame_flag_floating_y,
	ui_frame_flag_overflow = ui_frame_flag_overflow_x | ui_frame_flag_overflow_y,
	ui_frame_flag_ignore_view_scroll = ui_frame_flag_ignore_view_scroll_x | ui_frame_flag_ignore_view_scroll_y,
	ui_frame_flag_anim_pos = ui_frame_flag_anim_pos_x | ui_frame_flag_anim_pos_y,
	ui_frame_flag_anim_size = ui_frame_flag_anim_width | ui_frame_flag_anim_height,
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
	vec2_t anim_position;
	vec2_t anim_size;
	ui_size_t pref_size[2];
	ui_text_alignment text_alignment;
	f32 text_padding;
	os_cursor hover_cursor;
	ui_axis layout_axis;
	vec4_t rounding;
	f32 shadow_size;
	f32 border_size;
	ui_palette_t palette;
	gfx_handle_t texture;
	font_handle_t font;
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
	vec2_t view_offset_last;
	vec2_t view_offset_target;
	vec2_t view_bounds;
	ui_key_t nav_focus_hot_key;
	ui_key_t nav_focus_active_key;
	ui_key_t nav_focus_next_hot_key;
	ui_key_t nav_focus_next_active_key;
	void* user_data;

};

struct ui_frame_rec_t {
	ui_frame_t* next;
	i32 push_count;
	i32 pop_count;
};

// stack nodes

ui_stack_node_decl(parent, ui_frame_t*)
ui_stack_node_decl(flags, ui_frame_flags)
ui_stack_node_decl(seed_key, ui_key_t)

// layout
ui_stack_node_decl(fixed_x, f32)
ui_stack_node_decl(fixed_y, f32)
ui_stack_node_decl(fixed_width, f32)
ui_stack_node_decl(fixed_height, f32)
ui_stack_node_decl(pref_width, ui_size_t)
ui_stack_node_decl(pref_height, ui_size_t)
ui_stack_node_decl(text_alignment, ui_text_alignment)
ui_stack_node_decl(text_padding, f32)
ui_stack_node_decl(layout_axis, ui_axis)

// styling
ui_stack_node_decl(hover_cursor, os_cursor)
ui_stack_node_decl(rounding_00, f32)
ui_stack_node_decl(rounding_01, f32)
ui_stack_node_decl(rounding_10, f32)
ui_stack_node_decl(rounding_11, f32)
ui_stack_node_decl(shadow_size, f32)
ui_stack_node_decl(border_size, f32)
ui_stack_node_decl(font, font_handle_t)
ui_stack_node_decl(font_size, f32)
ui_stack_node_decl(focus_hot, ui_focus_type)
ui_stack_node_decl(focus_active, ui_focus_type)
ui_stack_node_decl(texture, gfx_handle_t)

// colors
ui_stack_node_decl(color_background, color_t)
ui_stack_node_decl(color_text, color_t)
ui_stack_node_decl(color_border, color_t)
ui_stack_node_decl(color_hover, color_t)
ui_stack_node_decl(color_active, color_t)
ui_stack_node_decl(color_shadow, color_t)
ui_stack_node_decl(color_accent, color_t)

// animation

struct ui_anim_params_t {
	f32 initial;
	f32 target;
	f32 rate;
};

struct ui_anim_node_t {
	ui_anim_node_t* list_next;
	ui_anim_node_t* list_prev;

	ui_anim_node_t* lru_next;
	ui_anim_node_t* lru_prev;

	u64 first_build_index;
	u64 last_build_index;

	ui_key_t key;
	ui_anim_params_t params;
	f32 current;
};

// view

struct ui_view_t;
typedef void ui_view_function(ui_view_t*);

struct ui_view_t {

	// list of all views
	ui_view_t* global_next;
	ui_view_t* global_prev;

	// list of views just in a panel
	ui_view_t* next;
	ui_view_t* prev;
	
	// info
	str_t label;

	// view function
	ui_view_function* view_func;

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
	u32 view_count;
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
	os_handle_t window;
	gfx_handle_t renderer;

	// arenas
	arena_t* arena;
	arena_t* per_frame_arena;
	arena_t* drag_state_arena;

	// state
	u64 build_index;
	b8 tooltip_open;
	f32 fast_anim_rate;
	f32 slow_anim_rate;

	// TODO: maybe move this out of context.
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

	// animation cache
	ui_anim_node_t* anim_node_free;
	ui_anim_node_t* anim_node_first;
	ui_anim_node_t* anim_node_last;
	ui_anim_node_t* anim_node_lru;
	ui_anim_node_t* anim_node_mru;

	// frame list
	ui_frame_t* frame_first;
	ui_frame_t* frame_last;
	ui_frame_t* frame_free;

	// views list
	ui_view_t* view_first;
	ui_view_t* view_last;
	ui_view_t* view_free;

	// state
	ui_key_t hovered_frame_key;
	ui_key_t last_hovered_key;
	ui_key_t active_frame_key[os_mouse_button_count];
	ui_key_t focused_frame_key;
	ui_key_t nav_root_key;

	// panel tree
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
	font_handle_t default_font;
	font_handle_t icon_font;

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
	ui_stack_decl_default(layout_axis);

	ui_stack_decl_default(hover_cursor);
	ui_stack_decl_default(rounding_00);
	ui_stack_decl_default(rounding_01);
	ui_stack_decl_default(rounding_10);
	ui_stack_decl_default(rounding_11);
	ui_stack_decl_default(shadow_size);
	ui_stack_decl_default(border_size);
	ui_stack_decl_default(font);
	ui_stack_decl_default(font_size);
	ui_stack_decl_default(focus_hot);
	ui_stack_decl_default(focus_active);
	ui_stack_decl_default(texture);

	ui_stack_decl_default(color_background);
	ui_stack_decl_default(color_text);
	ui_stack_decl_default(color_border);
	ui_stack_decl_default(color_hover);
	ui_stack_decl_default(color_active);
	ui_stack_decl_default(color_shadow);
	ui_stack_decl_default(color_accent);

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
	ui_stack_decl(layout_axis);

	ui_stack_decl(hover_cursor);
	ui_stack_decl(rounding_00);
	ui_stack_decl(rounding_01);
	ui_stack_decl(rounding_10);
	ui_stack_decl(rounding_11);
	ui_stack_decl(shadow_size);
	ui_stack_decl(border_size);
	ui_stack_decl(font);
	ui_stack_decl(font_size);
	ui_stack_decl(focus_hot);
	ui_stack_decl(focus_active);
	ui_stack_decl(texture);

	ui_stack_decl(color_background);
	ui_stack_decl(color_text);
	ui_stack_decl(color_border);
	ui_stack_decl(color_hover);
	ui_stack_decl(color_active);
	ui_stack_decl(color_shadow);
	ui_stack_decl(color_accent);
	
};

// commands

struct ui_cmd_t {
	ui_cmd_t* next;
	ui_cmd_t* prev;

	ui_cmd_type type;
	ui_context_t* context;

	// command params
	ui_panel_t* panel;
	ui_dir dir;

};

// state

struct ui_state_t {
	
	arena_t* context_arena;	
	arena_t* event_arena;
	arena_t* command_arena;

	// context
	ui_context_t* ui_first;
	ui_context_t* ui_last;
	ui_context_t* ui_free;
	ui_context_t* ui_active;

	// commands
	ui_cmd_t* command_first;
	ui_cmd_t* command_last;
	ui_cmd_t* command_free;

	// events
	ui_event_list_t event_list;
	u32 click_counter[3];
	u64 last_click_time[3];
	u32 last_click_index[3];
	
	// event bindings
	ui_event_binding_t event_bindings[64];
	
	// icon font
	font_handle_t icon_font;


};

// widget structs

struct ui_slider_data_t {
	f32 percent;
};

struct ui_color_data_t {
	f32 hue;
	f32 sat;
	f32 val;
};

// globals

global ui_state_t ui_state;

// debug
global b8 ui_debug_frame = false;
global b8 ui_debug_text = false;
global b8 ui_show_hovered = false;
global b8 ui_show_active = false;
global b8 ui_show_focused = false;

// functions

// state
function void ui_init();
function void ui_release();
function void ui_update();
function ui_context_t* ui_active();

// context
function ui_context_t* ui_context_create(os_handle_t window, gfx_handle_t renderer);
function void ui_context_release(ui_context_t* context);
function void ui_begin_frame(ui_context_t* context);
function void ui_end_frame(ui_context_t* context);

// commands
function ui_cmd_t* ui_cmd_push(ui_cmd_type type, ui_context_t* context);
function void ui_cmd_pop(ui_cmd_t* cmd);


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
function b8 ui_key_is_hovered(ui_key_t key);
function b8 ui_key_is_active(ui_key_t key);
function b8 ui_key_is_focused(ui_key_t key);


// size
function ui_size_t ui_size(ui_size_type type, f32 value, f32 strictness);
function ui_size_t ui_size_pixel(f32 value, f32 strictness);
function ui_size_t ui_size_percent(f32 percent);
function ui_size_t ui_size_by_child(f32 strictness);
function ui_size_t ui_size_em(f32 value, f32 strictness);
function ui_size_t ui_size_text(f32 padding);

// direction
inlnfunc ui_axis ui_axis_from_dir(ui_dir dir);
inlnfunc ui_side ui_side_from_dir(ui_dir dir);

// text alignment
function vec2_t ui_text_align(font_handle_t font, f32 font_size, str_t text, rect_t rect, ui_text_alignment alignment);
function vec2_t ui_text_size(font_handle_t font, f32 font_size, str_t text);
function f32 ui_text_offset_from_index(font_handle_t font, f32 font_size, str_t text, u32 index);
function u32 ui_text_index_from_offset(font_handle_t font, f32 font_size, str_t text, f32 offset);

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
function void ui_kill_action();

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

function void ui_padding_begin(f32 size = 2.0f);
function void ui_padding_end();

// frames
function ui_frame_t* ui_frame_find(ui_key_t key);
function ui_frame_t* ui_frame_from_key(ui_key_t key, ui_frame_flags flags = 0);
function ui_frame_t* ui_frame_from_string(str_t string, ui_frame_flags flags = 0);
function ui_frame_rec_t ui_frame_rec_depth_first(ui_frame_t* frame);
function ui_interaction ui_frame_interaction(ui_frame_t* frame);

inlnfunc b8 ui_frame_is_hovered(ui_frame_t* frame);
inlnfunc b8 ui_frame_is_active(ui_frame_t* frame);
inlnfunc b8 ui_frame_is_focused(ui_frame_t* frame);

function void ui_frame_set_display_text(ui_frame_t* frame, str_t display_text);
function void ui_frame_set_custom_draw(ui_frame_t* frame, ui_frame_custom_draw_func* draw_function, void* draw_data);
function void ui_frame_set_user_data(ui_frame_t* frame, u32 size);

// views
function ui_view_t* ui_view_create(ui_context_t* context, str_t label, ui_view_function* func);
function void ui_view_release(ui_context_t* context, ui_view_t* view);

// panels
function ui_panel_t* ui_panel_create(ui_context_t* context, f32 percent = 0.5f, ui_axis split_axis = ui_axis_x);
function void ui_panel_release(ui_context_t*, ui_panel_t*);

function void ui_panel_insert(ui_panel_t* parent, ui_panel_t* panel, ui_panel_t* prev = nullptr);
function void ui_panel_remove(ui_panel_t* parent, ui_panel_t* panel);

function ui_panel_rec_t ui_panel_rec_depth_first(ui_panel_t* panel);
function rect_t ui_rect_from_panel_child(ui_panel_t* panel, rect_t rect);
function rect_t ui_rect_from_panel(arena_t* scratch, ui_panel_t* panel, rect_t root_rect);

function void ui_panel_insert_view(ui_panel_t* panel, ui_view_t* view, ui_view_t* prev_view = nullptr);
function void ui_panel_remove_view(ui_panel_t* panel, ui_view_t* view);

// tooltips
function void ui_tooltip_begin();
inlnfunc void ui_tooltip_end();

// animation
function ui_anim_params_t ui_anim_params_create(f32 initial, f32 target, f32 rate = ui_active()->fast_anim_rate);
function f32 ui_anim_ex(ui_key_t key, ui_anim_params_t params);
function f32 ui_anim(ui_key_t key, f32 initial, f32 target);


// widgets
function void ui_spacer(ui_size_t size = ui_size_pixel(2.0f, 1.0f));
function ui_interaction ui_label(str_t label);
function ui_interaction ui_labelf(char* fmt, ...);
function ui_interaction ui_button(str_t label);
function ui_interaction ui_buttonf(char* fmt, ...);
function ui_interaction ui_image(str_t label, gfx_handle_t texture);
function ui_interaction ui_slider(str_t label, i32* value, i32 min, i32 max);
function ui_interaction ui_slider(str_t label, f32* value, f32 min, f32 max);
function ui_interaction ui_checkbox(str_t label, b8* value);
function ui_interaction ui_expander(str_t label, b8* is_expanded);
function ui_interaction ui_expander_begin(str_t label, b8* is_expanded);
function void ui_expander_end();
function ui_interaction ui_float_edit(str_t label, f32* value, f32 delta = 0.01f, f32 min = 0.0f, f32 max = 0.0f);
function ui_interaction ui_vec2_edit(str_t label, vec2_t* value);
function ui_interaction ui_vec3_edit(str_t label, vec3_t* value);
function ui_interaction ui_vec4_edit(str_t label, vec4_t* value);
function ui_interaction ui_color_quad(str_t label, f32 hue, f32* sat, f32* val);
function ui_interaction ui_color_hue_bar(str_t label, f32* hue, f32 sat, f32 val);
function ui_interaction ui_color_sat_bar(str_t label, f32 hue, f32* sat, f32 val);
function ui_interaction ui_color_val_bar(str_t label, f32 hue, f32 sat, f32* val);
function ui_interaction ui_color_ring(str_t label, f32* hue, f32* sat, f32* val);
function ui_interaction ui_color_wheel(str_t label, f32* hue, f32* sat, f32 val);
function ui_interaction ui_color_hex(str_t label, f32* hue, f32* sat, f32 val);
function ui_interaction ui_text_edit(str_t label, char* buffer, u32 buffer_max_size, u32* out_size);
function ui_interaction ui_combo(str_t label, i32* current, char** items, u32 item_count);

// widget draw functions

function void ui_slider_draw_function(ui_frame_t* frame);

function void ui_color_hue_bar_draw_function(ui_frame_t* frame);
function void ui_color_sat_bar_draw_function(ui_frame_t* frame);
function void ui_color_val_bar_draw_function(ui_frame_t* frame);

function void ui_color_quad_draw_function(ui_frame_t* frame);
function void ui_color_ring_draw_function(ui_frame_t* frame);
function void ui_color_wheel_draw_function(ui_frame_t* frame);
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
ui_stack_func(layout_axis, ui_axis)

ui_stack_func(hover_cursor, os_cursor)
ui_stack_func(rounding_00, f32)
ui_stack_func(rounding_01, f32)
ui_stack_func(rounding_10, f32)
ui_stack_func(rounding_11, f32)
ui_stack_func(shadow_size, f32)
ui_stack_func(border_size, f32)
ui_stack_func(font, font_handle_t)
ui_stack_func(font_size, f32)
ui_stack_func(focus_hot, ui_focus_type)
ui_stack_func(focus_active, ui_focus_type)
ui_stack_func(texture, gfx_handle_t)

ui_stack_func(color_background, color_t)
ui_stack_func(color_text, color_t)
ui_stack_func(color_border, color_t)
ui_stack_func(color_hover, color_t)
ui_stack_func(color_active, color_t)
ui_stack_func(color_shadow, color_t)
ui_stack_func(color_accent, color_t)

// groups
function void ui_push_rounding(f32 rounding);
function void ui_pop_rounding();
function void ui_set_next_rounding(f32 rounding);

function void ui_push_rect(rect_t rect);
function void ui_pop_rect();
function void ui_set_next_rect(rect_t rect);

function void ui_push_pref_size(ui_size_t width, ui_size_t height);
function void ui_pop_pref_size();
function void ui_set_next_pref_size(ui_size_t width, ui_size_t height);

function void ui_push_color_var(ui_color var, color_t color);
function void ui_pop_color_var(ui_color var);
function void ui_set_next_color_var(ui_color var, color_t color);



#endif // UI_H