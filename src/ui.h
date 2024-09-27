// ui.h

#ifndef UI_H
#define UI_H

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

typedef u32 ui_frame_flags;
enum {
	ui_frame_flag_clip = (1 << 0),
	ui_frame_flag_draw_text = (1 << 1),
	ui_frame_flag_draw_background = (1 << 2),
	ui_frame_flag_draw_hover_effects = (1 << 3),
	ui_frame_flag_draw_active_effects = (1 << 4),
	ui_frame_flag_draw_custom_draw = (1 << 5),

	ui_frame_flag_clickable = (1 << 6),

	ui_frame_flag_view_scroll = (1 << 6),

	ui_frame_flag_floating_x = (1 << 7),
	ui_frame_flag_floating_y = (1 << 8),

	ui_frame_flag_floating = ui_frame_flag_floating_x | ui_frame_flag_floating_y,
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

// structs

struct ui_key_t {
	u64 data[1];
};

struct ui_size_t {
	ui_size_type type;
	f32 value;
	f32 strictness;
};

struct ui_palette_t {
	color_t background;
	color_t border;
	color_t text;
};

struct ui_frame_t {

	// list
	ui_frame_t* hash_next;
	ui_frame_t* hash_prev;

	// tree
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
	ui_layout_axis layout_axis;
	f32 rounding;
	ui_palette_t* palette;
	gfx_texture_t* texture;
	gfx_font_t* font;
	f32 font_size;

	// per frame layout
	rect_t rect;

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

};

// this is used for doing recursive depth first search
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
ui_stack_node_decl(layout_axis, ui_layout_axis)
ui_stack_node_decl(rounding, f32)
ui_stack_node_decl(palette, ui_palette_t*)
ui_stack_node_decl(texture, gfx_texture_t*)
ui_stack_node_decl(font, gfx_font_t*)
ui_stack_node_decl(font_size, f32)

struct ui_state_t {
	
	// contexts
	os_window_t* window;
	gfx_renderer_t* renderer;

	// arenas
	arena_t* frame_arena;
	arena_t* per_frame_arena;
	arena_t* scratch_arena;

	// build index
	u64 build_index;

	// frame list
	ui_frame_t* frame_first;
	ui_frame_t* frame_last;
	ui_frame_t* frame_free;

	// frame tree
	ui_frame_t* root;

	// defaults
	ui_palette_t default_palette;
	gfx_texture_t* default_texture;
	gfx_font_t* default_font;

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
	ui_stack_decl_default(layout_axis);
	ui_stack_decl_default(rounding);
	ui_stack_decl_default(palette);
	ui_stack_decl_default(texture);
	ui_stack_decl_default(font);
	ui_stack_decl_default(font_size);

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
	ui_stack_decl(layout_axis);
	ui_stack_decl(rounding);
	ui_stack_decl(palette);
	ui_stack_decl(texture);
	ui_stack_decl(font);
	ui_stack_decl(font_size);

};

// globals

global ui_state_t ui_state;

// functions

// state
function void ui_init();
function void ui_release();
function void ui_begin_frame(gfx_renderer_t*);
function void ui_end_frame();

// size
function ui_size_t ui_size(ui_size_type, f32, f32);
function ui_size_t ui_size_pixel(f32, f32);
function ui_size_t ui_size_percent(f32);

// alignment
function vec2_t ui_text_align(gfx_font_t*, f32, str_t, rect_t, ui_text_alignment);

// key
function ui_key_t ui_key_from_string(ui_key_t, str_t);
function ui_key_t ui_key_from_stringf(ui_key_t, char*, ...);
function b8 ui_key_equals(ui_key_t, ui_key_t);

// string
function str_t ui_display_string(str_t);
function str_t ui_hash_string(str_t);

// frames
function ui_frame_t* ui_frame_find(ui_key_t);
function ui_frame_t* ui_frame_from_key(ui_key_t, ui_frame_flags = 0);
function ui_frame_t* ui_frame_from_string(str_t, ui_frame_flags = 0);
function ui_frame_rec_t ui_frame_rec_depth_first(ui_frame_t*, ui_frame_t*, u32, u32);

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
ui_stack_func(layout_axis, ui_layout_axis)
ui_stack_func(rounding, f32)
ui_stack_func(palette, ui_palette_t*)
ui_stack_func(texture, gfx_texture_t*)
ui_stack_func(font, gfx_font_t*)
ui_stack_func(font_size, f32)

#endif // UI_H