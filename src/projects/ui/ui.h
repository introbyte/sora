// ui.h

#ifndef UI_H
#define UI_H

// todo:
// 
// [x] - animation cache
// [x] - custom drawing
// [x] - commands
// [x] - tooltips
// [x] - popups contexts
// [ ] - investigate animating layout 
//

//- enums

enum ui_size_type {
	ui_size_type_none,
	ui_size_type_pixel,
	ui_size_type_percent,
	ui_size_type_by_children,
	ui_size_type_text,
};

typedef u32 ui_axis;
enum {
	ui_axis_x,
	ui_axis_y,
	ui_axis_count,
};

typedef u32 ui_side;
enum {
    ui_side_min,
    ui_side_max,
    ui_side_count,
};

typedef i32 ui_dir;
enum {
    ui_dir_none = -1,
	ui_dir_left,
	ui_dir_up,
	ui_dir_right,
	ui_dir_down,
	ui_dir_count,
};

enum ui_color {
    ui_color_background,
    ui_color_text,
    ui_color_border,
    ui_color_shadow,
    ui_color_hover,
    ui_color_active,
    ui_color_accent,
    ui_color_count,
};

enum ui_text_alignment {
    ui_text_alignment_left,
    ui_text_alignment_center,
    ui_text_alignment_right,
};

enum ui_event_type {
    ui_event_type_none,
    ui_event_type_key_press,
    ui_event_type_key_release,
    ui_event_type_mouse_press,
    ui_event_type_mouse_release,
    ui_event_type_text,
    ui_event_type_navigate,
    ui_event_type_edit,
    ui_event_type_mouse_move,
    ui_event_type_mouse_scroll,
    ui_event_type_count,
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

enum ui_data_type {
    ui_data_type_none,
    ui_data_type_b8,
    ui_data_type_u8,
    ui_data_type_u16,
    ui_data_type_u32,
    ui_data_type_u64,
    ui_data_type_i8,
    ui_data_type_i16,
    ui_data_type_i32,
    ui_data_type_i64,
    ui_data_type_f32,
    ui_data_type_f64,
    ui_data_type_count,
};

enum ui_drag_state {
    ui_drag_state_none,
    ui_drag_state_dragging,
    ui_drag_state_dropping,
    ui_drag_state_count,
};

typedef u64 ui_frame_flags;
enum {
	ui_frame_flag_none = (0 << 0),
	ui_frame_flag_is_transient = (1 << 0),
    
	// interactions
	ui_frame_flag_interactable = (1 << 1),
	ui_frame_flag_scrollable = (1 << 2),
    ui_frame_flag_draggable = (1 << 3),
	ui_frame_flag_view_scroll_x = (1 << 4),
	ui_frame_flag_view_scroll_y = (1 << 5),
	ui_frame_flag_view_clamp_x = (1 << 6),
	ui_frame_flag_view_clamp_y = (1 << 7),
    
	// layout
	ui_frame_flag_fixed_width = (1 << 8),
	ui_frame_flag_fixed_height = (1 << 9),
	ui_frame_flag_floating_x = (1 << 10),
	ui_frame_flag_floating_y = (1 << 11),
	ui_frame_flag_overflow_x = (1 << 12),
	ui_frame_flag_overflow_y = (1 << 13),
	ui_frame_flag_ignore_view_scroll_x = (1 << 14),
	ui_frame_flag_ignore_view_scroll_y = (1 << 15),
    
	// appearance
	ui_frame_flag_draw_clip = (1 << 16),
	ui_frame_flag_draw_background = (1 << 17),
	ui_frame_flag_draw_text = (1 << 18),
	ui_frame_flag_draw_border = (1 << 19),
	ui_frame_flag_draw_shadow = (1 << 20),
	ui_frame_flag_draw_hover_effects = (1 << 21),
	ui_frame_flag_draw_active_effects = (1 << 22),
	ui_frame_flag_draw_custom = (1 << 23),
	ui_frame_flag_hover_cursor = (1 << 24),
	ui_frame_flag_anim_pos_x = (1 << 25),
	ui_frame_flag_anim_pos_y = (1 << 26),
	ui_frame_flag_anim_width = (1 << 27),
	ui_frame_flag_anim_height = (1 << 28),
    
	// groups
	ui_frame_flag_draw =
        ui_frame_flag_draw_background | ui_frame_flag_draw_text |
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

typedef u32 ui_interaction;
enum {
    ui_interaction_none = (0 << 0),
    
    ui_interaction_hovered = (1 << 1),
    ui_interaction_mouse_over = (1 << 2),
    
    ui_interaction_left_pressed = (1 << 3),
    ui_interaction_middle_pressed = (1 << 4),
    ui_interaction_right_pressed = (1 << 5),
    
    ui_interaction_left_released = (1 << 6),
    ui_interaction_middle_released = (1 << 7),
    ui_interaction_right_released = (1 << 8),
    
    ui_interaction_left_clicked = (1 << 9),
    ui_interaction_middle_clicked = (1 << 10),
    ui_interaction_right_clicked = (1 << 11),
    
    ui_interaction_left_double_clicked = (1 << 12),
    ui_interaction_middle_double_clicked = (1 << 13),
    ui_interaction_right_double_clicked = (1 << 14),
    
    ui_interaction_left_triple_clicked = (1 << 15),
    ui_interaction_middle_triple_clicked = (1 << 16),
    ui_interaction_right_triple_clicked = (1 << 17),
    
    ui_interaction_left_dragging = (1 << 18),
    ui_interaction_middle_dragging = (1 << 19),
    ui_interaction_right_dragging = (1 << 20),
    
    ui_interaction_left_double_dragging = (1 << 21),
    ui_interaction_middle_double_dragging = (1 << 22),
    ui_interaction_right_double_dragging = (1 << 23),
    
    ui_interaction_left_triple_dragging = (1 << 24),
    ui_interaction_middle_triple_dragging = (1 << 25),
    ui_interaction_right_triple_dragging = (1 << 26),
};

enum ui_cmd_type {
    ui_cmd_type_none,
    ui_cmd_type_close_panel,
    ui_cmd_type_split_panel,
};



//- typedefs

struct ui_frame_t;
typedef void ui_frame_custom_draw_func(ui_frame_t*);

struct ui_view_t;
typedef void ui_view_function();

//- structs

// key
struct ui_key_t {
	u64 data[1];
};

// size
struct ui_size_t {
	ui_size_type type;
	f32 value;
	f32 strictness;
};

// palette
union ui_palette_t {
    
    color_t colors[ui_color_count];
    
    struct {
        color_t background;
        color_t text;
        color_t border;
        color_t shadow;
        color_t hover;
        color_t active;
        color_t accent;
    };
};

// theme
struct ui_theme_t {
    
    b8 frame_borders;
    b8 frame_shadows;
    b8 text_shadows;
    f32 frame_rounding;
    f32 frame_padding;
    f32 shadow_size;
    f32 border_size;
    
    ui_palette_t palette;
    
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

// frames
struct ui_frame_t {
    
    // list
    ui_frame_t* list_next;
    ui_frame_t* list_prev;
    
    // tree
    ui_frame_t* tree_next;
    ui_frame_t* tree_prev;
    ui_frame_t* tree_parent;
    ui_frame_t* tree_first;
    ui_frame_t* tree_last;
    
    // info
    ui_key_t key;
    str_t label;
    ui_frame_flags flags;
    
    // layout
    vec2_t pos_target;
    vec2_t pos;
    ui_size_t size_wanted[2];
    vec2_t size_target;
    vec2_t size;
    f32 padding;
    ui_dir layout_dir;
    ui_text_alignment text_alignment;
    vec2_t view_bounds;
    vec2_t view_offset;
    vec2_t view_offset_target;
    rect_t rect;
    
    // appearance
    f32 hover_t;
    f32 active_t;
    vec4_t rounding;
    f32 border_size;
    f32 shadow_size;
    gfx_handle_t texture;
    font_handle_t font;
    f32 font_size;
    ui_palette_t palette;
    os_cursor hover_cursor;
    ui_frame_custom_draw_func* custom_draw_func;
    void* custom_draw_data;
    
    u64 first_build_index;
    u64 last_build_index;
};

struct ui_frame_rec_t {
    ui_frame_t* next;
    u32 push_count;
    u32 pop_count;
};

// views

struct ui_view_t {
    
    // global list
    ui_view_t* global_next;
    ui_view_t* global_prev;
    
    // list
    ui_view_t* next;
    ui_view_t* prev;
    
    // info
    str_t label;
    ui_view_function* view_func;
};

// panels

struct ui_panel_t {
    
    // list
    ui_panel_t* list_next;
    ui_panel_t* list_prev;
    
    // tree
    ui_panel_t* tree_next;
    ui_panel_t* tree_prev;
    ui_panel_t* tree_parent;
    ui_panel_t* tree_first;
    ui_panel_t* tree_last;
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

// drop site

struct ui_drop_site_t {
    ui_key_t key;
    ui_dir split_dir;
    rect_t rect;
};

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

// data 

struct ui_data_node_t {
    ui_data_node_t* list_next;
    ui_data_node_t* list_prev;
    
    ui_data_node_t* lru_next;
    ui_data_node_t* lru_prev;
    
    u64 first_build_index;
    u64 last_build_index;
    
    ui_key_t key;
    ui_data_type type;
    
    union {
        b8 b8_value;
        i8 i8_value;
        i16 i16_value;
        i32 i32_value;
        i64 i64_value;
        u8 u8_value;
        u16 u16_value;
        u32 u32_value;
        u64 u64_value;
        f32 f32_value;
        f64 f64_value;
        void* ptr_value;
    };
    
};

// stacks

struct ui_parent_node_t { ui_parent_node_t* next; ui_frame_t* v; };
struct ui_parent_stack_t { ui_parent_node_t* top; ui_parent_node_t* free; b8 auto_pop; };

struct ui_flags_node_t { ui_flags_node_t* next; ui_frame_flags v; };
struct ui_flags_stack_t { ui_flags_node_t* top; ui_flags_node_t* free; b8 auto_pop; };

struct ui_seed_key_node_t { ui_seed_key_node_t* next; ui_key_t v; };
struct ui_seed_key_stack_t { ui_seed_key_node_t* top; ui_seed_key_node_t* free; b8 auto_pop; };

// layout stacks

struct ui_fixed_x_node_t { ui_fixed_x_node_t* next; f32 v; };
struct ui_fixed_x_stack_t { ui_fixed_x_node_t* top; ui_fixed_x_node_t* free; b8 auto_pop; };

struct ui_fixed_y_node_t { ui_fixed_y_node_t* next; f32 v; };
struct ui_fixed_y_stack_t { ui_fixed_y_node_t* top; ui_fixed_y_node_t* free; b8 auto_pop; };

struct ui_fixed_width_node_t { ui_fixed_width_node_t* next; f32 v; };
struct ui_fixed_width_stack_t { ui_fixed_width_node_t* top; ui_fixed_width_node_t* free; b8 auto_pop; };

struct ui_fixed_height_node_t { ui_fixed_height_node_t* next; f32 v; };
struct ui_fixed_height_stack_t { ui_fixed_height_node_t* top; ui_fixed_height_node_t* free; b8 auto_pop; };

struct ui_width_node_t { ui_width_node_t* next; ui_size_t v; };
struct ui_width_stack_t { ui_width_node_t* top; ui_width_node_t* free; b8 auto_pop; };

struct ui_height_node_t { ui_height_node_t* next; ui_size_t v; };
struct ui_height_stack_t { ui_height_node_t* top; ui_height_node_t* free; b8 auto_pop; };

struct ui_padding_node_t { ui_padding_node_t* next; f32 v; };
struct ui_padding_stack_t { ui_padding_node_t* top; ui_padding_node_t* free; b8 auto_pop; };

struct ui_layout_dir_node_t { ui_layout_dir_node_t* next; ui_dir v; };
struct ui_layout_dir_stack_t { ui_layout_dir_node_t* top; ui_layout_dir_node_t* free; b8 auto_pop; };

struct ui_text_alignment_node_t { ui_text_alignment_node_t* next; ui_text_alignment v; };
struct ui_text_alignment_stack_t { ui_text_alignment_node_t* top; ui_text_alignment_node_t* free; b8 auto_pop; };

// appearance stacks

struct ui_rounding_00_node_t { ui_rounding_00_node_t* next; f32 v; };
struct ui_rounding_00_stack_t { ui_rounding_00_node_t* top; ui_rounding_00_node_t* free; b8 auto_pop; };

struct ui_rounding_01_node_t { ui_rounding_01_node_t* next; f32 v; };
struct ui_rounding_01_stack_t { ui_rounding_01_node_t* top; ui_rounding_01_node_t* free; b8 auto_pop; };

struct ui_rounding_10_node_t { ui_rounding_10_node_t* next; f32 v; };
struct ui_rounding_10_stack_t { ui_rounding_10_node_t* top; ui_rounding_10_node_t* free; b8 auto_pop; };

struct ui_rounding_11_node_t { ui_rounding_11_node_t* next; f32 v; };
struct ui_rounding_11_stack_t { ui_rounding_11_node_t* top; ui_rounding_11_node_t* free; b8 auto_pop; };

struct ui_border_size_node_t { ui_border_size_node_t* next; f32 v; };
struct ui_border_size_stack_t { ui_border_size_node_t* top; ui_border_size_node_t* free; b8 auto_pop; };

struct ui_shadow_size_node_t { ui_shadow_size_node_t* next; f32 v; };
struct ui_shadow_size_stack_t { ui_shadow_size_node_t* top; ui_shadow_size_node_t* free; b8 auto_pop; };

struct ui_texture_node_t { ui_texture_node_t* next; gfx_handle_t v; };
struct ui_texture_stack_t { ui_texture_node_t* top; ui_texture_node_t* free; b8 auto_pop; };

struct ui_font_node_t { ui_font_node_t* next; font_handle_t v; };
struct ui_font_stack_t { ui_font_node_t* top; ui_font_node_t* free; b8 auto_pop; };

struct ui_font_size_node_t { ui_font_size_node_t* next; f32 v; };
struct ui_font_size_stack_t { ui_font_size_node_t* top; ui_font_size_node_t* free; b8 auto_pop; };

struct ui_color_background_node_t { ui_color_background_node_t* next; color_t v; };
struct ui_color_background_stack_t { ui_color_background_node_t* top; ui_color_background_node_t* free; b8 auto_pop; };

struct ui_color_text_node_t { ui_color_text_node_t* next; color_t v; };
struct ui_color_text_stack_t { ui_color_text_node_t* top; ui_color_text_node_t* free; b8 auto_pop; };

struct ui_color_border_node_t { ui_color_border_node_t* next; color_t v; };
struct ui_color_border_stack_t { ui_color_border_node_t* top; ui_color_border_node_t* free; b8 auto_pop; };

struct ui_color_shadow_node_t { ui_color_shadow_node_t* next; color_t v; };
struct ui_color_shadow_stack_t { ui_color_shadow_node_t* top; ui_color_shadow_node_t* free; b8 auto_pop; };

struct ui_color_hover_node_t { ui_color_hover_node_t* next; color_t v; };
struct ui_color_hover_stack_t { ui_color_hover_node_t* top; ui_color_hover_node_t* free; b8 auto_pop; };

struct ui_color_active_node_t { ui_color_active_node_t* next; color_t v; };
struct ui_color_active_stack_t { ui_color_active_node_t* top; ui_color_active_node_t* free; b8 auto_pop; };

struct ui_color_accent_node_t { ui_color_accent_node_t* next; color_t v; };
struct ui_color_accent_stack_t { ui_color_accent_node_t* top; ui_color_accent_node_t* free; b8 auto_pop; };

struct ui_hover_cursor_node_t { ui_hover_cursor_node_t* next; os_cursor v; };
struct ui_hover_cursor_stack_t { ui_hover_cursor_node_t* top; ui_hover_cursor_node_t* free; b8 auto_pop; };

// context
struct ui_context_t {
    ui_context_t* next;
    ui_context_t* prev;
    
    // arena
    arena_t* arena;
    arena_t* build_arena[2];
    arena_t* drag_state_arena;
    
    // context
    os_handle_t window;
    gfx_handle_t renderer;
    
    // build state
    u64 build_index;
    
    // frame list and tree
    ui_frame_t* frame_first;
    ui_frame_t* frame_last;
    ui_frame_t* frame_free;
    ui_frame_t* frame_root;
    ui_frame_t* frame_tooltip;
    ui_frame_t* frame_popup;
    ui_frame_t* frame_mrc;
    
    // view list
    ui_view_t* view_first;
    ui_view_t* view_last;
    ui_view_t* view_free;
    ui_view_t* view_drag;
    
    // panel list
    ui_panel_t* panel_first;
    ui_panel_t* panel_last;
    ui_panel_t* panel_free;
    ui_panel_t* panel_root;
    
    // animation cache
    ui_anim_node_t* anim_node_first;
    ui_anim_node_t* anim_node_last;
    ui_anim_node_t* anim_node_free;
    ui_anim_node_t* anim_node_lru;
    ui_anim_node_t* anim_node_mru;
    
    // data nodes
    ui_data_node_t* data_node_first;
    ui_data_node_t* data_node_last;
    ui_data_node_t* data_node_free;
    ui_data_node_t* data_node_lru;
    ui_data_node_t* data_node_mru;
    
    // keys
    ui_key_t key_hovered;
    ui_key_t key_hovered_prev;
    ui_key_t key_active[3];
    ui_key_t key_focused;
    ui_key_t key_popup;
    
    // input state
    vec2_t mouse_pos;
    vec2_t mouse_delta;
    
    // drag state
    ui_drag_state drag_state;
	void* drag_state_data;
	u32 drag_state_size;
	vec2_t drag_start_pos;
    
    // animation
    f32 anim_slow_rate;
    f32 anim_fast_rate;
    
    // popup state
    vec2_t popup_pos;
    b8 popup_is_open;
    b8 popup_updated_this_frame;
    
    // stacks
    ui_parent_node_t parent_default_node;
    ui_parent_stack_t parent_stack;
    
    ui_flags_node_t flags_default_node;
    ui_flags_stack_t flags_stack;
    
    ui_seed_key_node_t seed_key_default_node;
    ui_seed_key_stack_t seed_key_stack;
    
    ui_fixed_x_node_t fixed_x_default_node;
    ui_fixed_x_stack_t fixed_x_stack;
    
    ui_fixed_y_node_t fixed_y_default_node;
    ui_fixed_y_stack_t fixed_y_stack;
    
    ui_fixed_width_node_t fixed_width_default_node;
    ui_fixed_width_stack_t fixed_width_stack;
    
    ui_fixed_height_node_t fixed_height_default_node;
    ui_fixed_height_stack_t fixed_height_stack;
    
    ui_width_node_t width_default_node;
    ui_width_stack_t width_stack;
    
    ui_height_node_t height_default_node;
    ui_height_stack_t height_stack;
    
    ui_padding_node_t padding_default_node;
    ui_padding_stack_t padding_stack;
    
    ui_layout_dir_node_t layout_dir_default_node;
    ui_layout_dir_stack_t layout_dir_stack;
    
    ui_text_alignment_node_t text_alignment_default_node;
    ui_text_alignment_stack_t text_alignment_stack;
    
    ui_rounding_00_node_t rounding_00_default_node;
    ui_rounding_00_stack_t rounding_00_stack;
    
    ui_rounding_01_node_t rounding_01_default_node;
    ui_rounding_01_stack_t rounding_01_stack;
    
    ui_rounding_10_node_t rounding_10_default_node;
    ui_rounding_10_stack_t rounding_10_stack;
    
    ui_rounding_11_node_t rounding_11_default_node;
    ui_rounding_11_stack_t rounding_11_stack;
    
    ui_border_size_node_t border_size_default_node;
    ui_border_size_stack_t border_size_stack;
    
    ui_shadow_size_node_t shadow_size_default_node;
    ui_shadow_size_stack_t shadow_size_stack;
    
    ui_texture_node_t texture_default_node;
    ui_texture_stack_t texture_stack;
    
    ui_font_node_t font_default_node;
    ui_font_stack_t font_stack;
    
    ui_font_size_node_t font_size_default_node;
    ui_font_size_stack_t font_size_stack;
    
    ui_color_background_node_t color_background_default_node;
    ui_color_background_stack_t color_background_stack;
    
    ui_color_text_node_t color_text_default_node;
    ui_color_text_stack_t color_text_stack;
    
    ui_color_border_node_t color_border_default_node;
    ui_color_border_stack_t color_border_stack;
    
    ui_color_shadow_node_t color_shadow_default_node;
    ui_color_shadow_stack_t color_shadow_stack;
    
    ui_color_hover_node_t color_hover_default_node;
    ui_color_hover_stack_t color_hover_stack;
    
    ui_color_active_node_t color_active_default_node;
    ui_color_active_stack_t color_active_stack;
    
    ui_color_accent_node_t color_accent_default_node;
    ui_color_accent_stack_t color_accent_stack;
    
    ui_hover_cursor_node_t hover_cursor_default_node;
    ui_hover_cursor_stack_t hover_cursor_stack;
    
};

// commands

struct ui_cmd_t {
    ui_cmd_t* next;
    ui_cmd_t* prev;
    
    ui_cmd_type type;
    ui_context_t* context;
    
    ui_panel_t* panel;
    ui_panel_t* split_panel;
    ui_dir dir;
};

// state
struct ui_state_t {
    
    arena_t* context_arena;
    arena_t* event_arena;
    arena_t* command_arena;
    
    // contexts
    ui_context_t* context_first;
    ui_context_t* context_last;
    ui_context_t* context_free;
    ui_context_t* context_active;
    
    // commands
    ui_cmd_t* command_first;
    ui_cmd_t* command_last;
    ui_cmd_t* command_free;
    
    // events
    ui_event_t* event_first;
    ui_event_t* event_last;
    u32 click_counter[3];
	u64 last_click_time[3];
};

//- globals

global ui_state_t ui_state;

//- functions

// state
function void ui_init();
function void ui_release();
function void ui_update();
function void ui_begin(ui_context_t* context);
function void ui_end(ui_context_t* context);
function ui_context_t* ui_active();
function arena_t* ui_build_arena();
function ui_frame_t* ui_last_frame();

// context
function ui_context_t* ui_context_create(os_handle_t window, gfx_handle_t renderer);
function void ui_context_release(ui_context_t* context);
function void ui_context_reset_stacks(ui_context_t* context);

// string
function u64 ui_string_hash(u64 seed, str_t string);
function str_t ui_string_display_format(str_t string);
function str_t ui_string_hash_format(str_t string);

// key
function ui_key_t ui_key_from_string(ui_key_t seed, str_t string);
function ui_key_t ui_key_from_stringf(ui_key_t seed, char* fmt, ...);
function b8 ui_key_equals(ui_key_t a, ui_key_t b);
function b8 ui_key_is_hovered(ui_key_t key);
function b8 ui_key_is_active(ui_key_t key);
function b8 ui_key_is_focused(ui_key_t key);

// size
function ui_size_t ui_size(ui_size_type type, f32 value, f32 strictness);
function ui_size_t ui_size_pixels(f32 pixels, f32 strictness = 1.0f);
function ui_size_t ui_size_percent(f32 percent);
function ui_size_t ui_size_by_children(f32 strictness);
function ui_size_t ui_size_text(f32 padding);

// axis
function ui_axis ui_axis_from_dir(ui_dir dir);

// side
function ui_side ui_side_from_dir(ui_dir dir);

// text alignment
function vec2_t ui_text_align(font_handle_t font, f32 font_size, str_t text, rect_t rect, ui_text_alignment alignment);
function vec2_t ui_text_size(font_handle_t font, f32 font_size, str_t text);
function f32 ui_text_offset_from_index(font_handle_t font, f32 font_size, str_t text, u32 index);
function u32 ui_text_index_from_offset(font_handle_t font, f32 font_size, str_t text, f32 offset);

// commands
function ui_cmd_t* ui_cmd_push(ui_context_t* context, ui_cmd_type type);
function void ui_cmd_pop(ui_cmd_t* command);

// events
function void ui_event_push(ui_event_t* event);
function void ui_event_pop(ui_event_t* event);

// drag state
function void ui_drag_store_data(void* data, u32 size);
function void* ui_drag_get_data();
function void ui_drag_clear_data();
function vec2_t ui_drag_delta();
function b8 ui_drag_is_active();
function void ui_drag_begin();
function b8 ui_drag_drop();
function void ui_drag_kill();



// animation
function ui_anim_params_t ui_anim_params_create(f32 initial, f32 target, f32 rate = ui_active()->anim_fast_rate);
function f32 ui_anim_ex(ui_key_t key, ui_anim_params_t params);
function f32 ui_anim(ui_key_t key, f32 initial, f32 target);

// data
function void* ui_data_ex(ui_key_t key, ui_data_type type, void* initial);
function b8* ui_data(ui_key_t key, b8 initial);
function f32* ui_data(ui_key_t key, f32 initial);


// frames
function ui_frame_t* ui_frame_find(ui_key_t key);
function ui_frame_t* ui_frame_from_key(ui_frame_flags flags, ui_key_t key);
function ui_frame_t* ui_frame_from_string(ui_frame_flags flags, str_t string);
function ui_frame_t* ui_frame_from_stringf(ui_frame_flags flags, char* fmt, ...);
function void ui_frame_set_display_string(ui_frame_t* frame, str_t string);
function void ui_frame_set_custom_draw(ui_frame_t* frame,  ui_frame_custom_draw_func* func, void* data);
function ui_frame_rec_t ui_frame_rec_depth_first(ui_frame_t* frame);
function ui_interaction ui_frame_interaction(ui_frame_t* frame);

// views
function ui_view_t* ui_view_create(ui_context_t* context, str_t label, ui_view_function* view_func);
function void ui_view_release(ui_context_t* context, ui_view_t* view);
function void ui_view_insert(ui_panel_t* panel, ui_view_t* view, ui_view_t* prev = nullptr);
function void ui_view_remove(ui_panel_t* panel, ui_view_t* view);

// panels
function ui_panel_t* ui_panel_create(ui_context_t* context, f32 percent = 0.5f, ui_axis split_axis = ui_axis_x);
function void ui_panel_release(ui_context_t* context, ui_panel_t* panel);
function void ui_panel_insert(ui_panel_t* parent, ui_panel_t* panel, ui_panel_t* prev = nullptr);
function void ui_panel_remove(ui_panel_t* panel);
function ui_panel_rec_t ui_panel_rec_depth_first(ui_panel_t* panel);
function rect_t ui_rect_from_panel_child(ui_panel_t* parent, ui_panel_t* panel, rect_t parent_rect);
function rect_t ui_rect_from_panel(ui_panel_t* panel, rect_t root_rect);

// layout
function void ui_layout_solve_independent(ui_frame_t* frame, ui_axis axis);
function void ui_layout_solve_upward_dependent(ui_frame_t* frame, ui_axis axis);
function void ui_layout_solve_downward_dependent(ui_frame_t* frame, ui_axis axis);
function void ui_layout_solve_violations(ui_frame_t* frame, ui_axis axis);
function void ui_layout_solve_set_positions(ui_frame_t* frame, ui_axis axis);

// layout widgets
function void ui_row_begin();
function void ui_row_end();
function void ui_column_begin();
function void ui_column_end();
function void ui_padding_begin(f32 size);
function void ui_padding_end();

// tooltip
function void ui_tooltip_begin();
function void ui_tooltip_end();

// popups
function b8 ui_popup_begin(ui_key_t key);
function void ui_popup_end();
function void ui_popup_open(ui_key_t key, vec2_t pos);
function void ui_popup_close();

// widgets
function void ui_spacer(ui_size_t size = ui_size_pixels(2.0f));
function ui_interaction ui_label(str_t label);
function ui_interaction ui_labelf(char* fmt, ...);
function ui_interaction ui_button(str_t label);
function ui_interaction ui_buttonf(char* fmt, ...);
function ui_interaction ui_slider(f32* value, f32 min, f32 max, str_t label);
function ui_interaction ui_sliderf(f32* value, f32 min, f32 max, char* fmt, ...);


function b8 ui_expander_begin(str_t label);
function b8 ui_expanderf_begin(char* fmt, ...);
function void ui_expander_end();


// custom draw functions

function void ui_drop_site_draw_function(ui_frame_t* frame);

function void ui_slider_draw_function(ui_frame_t* frame);

// stacks

function void ui_auto_pop_stacks();

function ui_frame_t* ui_top_parent();
function ui_frame_t* ui_push_parent(ui_frame_t* v);
function ui_frame_t* ui_pop_parent();
function ui_frame_t* ui_set_next_parent(ui_frame_t* v);

function ui_frame_flags ui_top_flags();
function ui_frame_flags ui_push_flags(ui_frame_flags v);
function ui_frame_flags ui_pop_flags();
function ui_frame_flags ui_set_next_flags(ui_frame_flags v);

function ui_key_t ui_top_seed_key();
function ui_key_t ui_push_seed_key(ui_key_t v);
function ui_key_t ui_pop_seed_key();
function ui_key_t ui_set_next_seed_key(ui_key_t v);

function f32 ui_top_fixed_x();
function f32 ui_push_fixed_x(f32 v);
function f32 ui_pop_fixed_x();
function f32 ui_set_next_fixed_x(f32 v);

function f32 ui_top_fixed_y();
function f32 ui_push_fixed_y(f32 v);
function f32 ui_pop_fixed_y();
function f32 ui_set_next_fixed_y(f32 v);

function f32 ui_top_fixed_width();
function f32 ui_push_fixed_width(f32 v);
function f32 ui_pop_fixed_width();
function f32 ui_set_next_fixed_width(f32 v);

function f32 ui_top_fixed_height();
function f32 ui_push_fixed_height(f32 v);
function f32 ui_pop_fixed_height();
function f32 ui_set_next_fixed_height(f32 v);

function ui_size_t ui_top_width();
function ui_size_t ui_push_width(ui_size_t v);
function ui_size_t ui_pop_width();
function ui_size_t ui_set_next_width(ui_size_t v);

function ui_size_t ui_top_height();
function ui_size_t ui_push_height(ui_size_t v);
function ui_size_t ui_pop_height();
function ui_size_t ui_set_next_height(ui_size_t v);

function f32 ui_top_padding();
function f32 ui_push_padding(f32 v);
function f32 ui_pop_padding();
function f32 ui_set_next_padding(f32 v);

function ui_dir ui_top_layout_dir();
function ui_dir ui_push_layout_dir(ui_dir v);
function ui_dir ui_pop_layout_dir();
function ui_dir ui_set_next_layout_dir(ui_dir v);

function ui_text_alignment ui_top_text_alignment();
function ui_text_alignment ui_push_text_alignment(ui_text_alignment v);
function ui_text_alignment ui_pop_text_alignment();
function ui_text_alignment ui_set_next_text_alignment(ui_text_alignment v);

function f32 ui_top_rounding_00();
function f32 ui_push_rounding_00(f32 v);
function f32 ui_pop_rounding_00();
function f32 ui_set_next_rounding_00(f32 v);

function f32 ui_top_rounding_01();
function f32 ui_push_rounding_01(f32 v);
function f32 ui_pop_rounding_01();
function f32 ui_set_next_rounding_01(f32 v);

function f32 ui_top_rounding_10();
function f32 ui_push_rounding_10(f32 v);
function f32 ui_pop_rounding_10();
function f32 ui_set_next_rounding_10(f32 v);

function f32 ui_top_rounding_11();
function f32 ui_push_rounding_11(f32 v);
function f32 ui_pop_rounding_11();
function f32 ui_set_next_rounding_11(f32 v);

function f32 ui_top_border_size();
function f32 ui_push_border_size(f32 v);
function f32 ui_pop_border_size();
function f32 ui_set_next_border_size(f32 v);

function f32 ui_top_shadow_size();
function f32 ui_push_shadow_size(f32 v);
function f32 ui_pop_shadow_size();
function f32 ui_set_next_shadow_size(f32 v);

function gfx_handle_t ui_top_texture();
function gfx_handle_t ui_push_texture(gfx_handle_t v);
function gfx_handle_t ui_pop_texture();
function gfx_handle_t ui_set_next_texture(gfx_handle_t v);

function font_handle_t ui_top_font();
function font_handle_t ui_push_font(font_handle_t v);
function font_handle_t ui_pop_font();
function font_handle_t ui_set_next_font(font_handle_t v);

function f32 ui_top_font_size();
function f32 ui_push_font_size(f32 v);
function f32 ui_pop_font_size();
function f32 ui_set_next_font_size(f32 v);

function color_t ui_top_color_background();
function color_t ui_push_color_background(color_t v);
function color_t ui_pop_color_background();
function color_t ui_set_next_color_background(color_t v);

function color_t ui_top_color_text();
function color_t ui_push_color_text(color_t v);
function color_t ui_pop_color_text();
function color_t ui_set_next_color_text(color_t v);

function color_t ui_top_color_border();
function color_t ui_push_color_border(color_t v);
function color_t ui_pop_color_border();
function color_t ui_set_next_color_border(color_t v);

function color_t ui_top_color_shadow();
function color_t ui_push_color_shadow(color_t v);
function color_t ui_pop_color_shadow();
function color_t ui_set_next_color_shadow(color_t v);

function color_t ui_top_color_hover();
function color_t ui_push_color_hover(color_t v);
function color_t ui_pop_color_hover();
function color_t ui_set_next_color_hover(color_t v);

function color_t ui_top_color_active();
function color_t ui_push_color_active(color_t v);
function color_t ui_pop_color_active();
function color_t ui_set_next_color_active(color_t v);

function color_t ui_top_color_accent();
function color_t ui_push_color_accent(color_t v);
function color_t ui_pop_color_accent();
function color_t ui_set_next_color_accent(color_t v);

function os_cursor ui_top_hover_cursor();
function os_cursor ui_push_hover_cursor(os_cursor v);
function os_cursor ui_pop_hover_cursor();
function os_cursor ui_set_next_hover_cursor(os_cursor v);

// group stacks

// size
function void ui_push_size(ui_size_t width, ui_size_t height);
function void ui_pop_size();
function void ui_set_next_size(ui_size_t width, ui_size_t height);

// rect
function void ui_push_rect(rect_t rect);
function void ui_pop_rect();
function void ui_set_next_rect(rect_t rect);

// rounding
function vec4_t ui_top_rounding();
function vec4_t ui_push_rounding(vec4_t rounding);
function vec4_t ui_pop_rounding();
function vec4_t ui_set_next_rounding(vec4_t rounding);

// color
function color_t ui_top_color(ui_color var);
function color_t ui_push_color(ui_color var, color_t color);
function color_t ui_pop_color(ui_color var);
function color_t ui_set_next_color(ui_color var, color_t color);

#endif // UI_H