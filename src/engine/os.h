// os.h

#ifndef OS_H
#define OS_H

// includes
#include <windows.h>
#include <timeapi.h>

// typedefs

typedef void os_window_resize_func();
typedef void os_window_close_func();
typedef void os_thread_function();

// enums

enum os_key {
	os_key_null = 0,
	os_key_backspace = 8,
	os_key_tab = 9,
	os_key_enter = 13,
	os_key_shift = 16,
	os_key_ctrl = 17,
	os_key_alt = 18,
	os_key_caps = 20,
	os_key_esc = 27,
	os_key_space = 32,
	os_key_page_up = 33,
	os_key_page_down = 34,
	os_key_end = 35,
	os_key_home = 36,
	os_key_left = 37,
	os_key_up = 38,
	os_key_right = 39,
	os_key_down = 40,
	os_key_delete = 46,
	os_key_0 = 48,
	os_key_1 = 49,
	os_key_2 = 50,
	os_key_3 = 51,
	os_key_4 = 52,
	os_key_5 = 53,
	os_key_6 = 54,
	os_key_7 = 55,
	os_key_8 = 56,
	os_key_9 = 57,
	os_key_A = 65,
	os_key_B = 66,
	os_key_C = 67,
	os_key_D = 68,
	os_key_E = 69,
	os_key_F = 70,
	os_key_G = 71,
	os_key_H = 72,
	os_key_I = 73,
	os_key_J = 74,
	os_key_K = 75,
	os_key_L = 76,
	os_key_M = 77,
	os_key_N = 78,
	os_key_O = 79,
	os_key_P = 80,
	os_key_Q = 81,
	os_key_R = 82,
	os_key_S = 83,
	os_key_T = 84,
	os_key_U = 85,
	os_key_V = 86,
	os_key_W = 87,
	os_key_X = 88,
	os_key_Y = 89,
	os_key_Z = 90,
	os_key_equal = 187,
	os_key_comma = 188,
	os_key_minus = 189,
	os_key_period = 190,
	os_key_forward_slash = 191,
	os_key_left_bracket = 219,
	os_key_backslash = 220,
	os_key_right_bracket = 221,
	os_key_quote = 222,
	os_key_insert = 45,
	os_key_semicolon = 186,
	os_key_F1 = 112,
	os_key_F2 = 113,
	os_key_F3 = 114,
	os_key_F4 = 115,
	os_key_F5 = 116,
	os_key_F6 = 117,
	os_key_F7 = 118,
	os_key_F8 = 119,
	os_key_F9 = 120,
	os_key_F10 = 121,
	os_key_F11 = 122,
	os_key_F12 = 123,
	os_key_F13 = 124,
	os_key_F14 = 125,
	os_key_F15 = 126,
	os_key_F16 = 127,
	os_key_F17 = 128,
	os_key_F18 = 129,
	os_key_F19 = 130,
	os_key_F20 = 131,
	os_key_F21 = 132,
	os_key_F22 = 133,
	os_key_F23 = 134,
	os_key_F24 = 135,
	os_key_grave_accent = 192,
};

enum os_mouse_button {
	os_mouse_button_left,
	os_mouse_button_middle,
	os_mouse_button_right,
	os_mouse_button_count,
};

typedef u32 os_modifiers;
enum {
	os_modifier_shift = (1 << 0),
	os_modifier_ctrl = (1 << 1),
	os_modifier_alt = (1 << 2),
};

enum os_event_type {
	os_event_type_null,
	os_event_type_window_close,
	os_event_type_window_resize,
	os_event_type_key_press,
	os_event_type_key_release,
	os_event_type_text,
	os_event_type_mouse_press,
	os_event_type_mouse_release,
	os_event_type_mouse_scroll,
	os_event_type_mouse_move,
};

typedef u32 os_file_access_flag;
enum {
	os_file_access_flag_none = (0),
	os_file_access_flag_read = (1 << 0),
	os_file_access_flag_write = (1 << 1),
	os_file_access_flag_execute = (1 << 2),
	os_file_access_flag_append = (1 << 3),
	os_file_access_flag_share_read = (1 << 4),
	os_file_access_flag_share_write = (1 << 5),
	os_file_access_flag_attribute = (1 << 6),
};

enum os_cursor {
	os_cursor_null,
	os_cursor_pointer,
	os_cursor_I_beam,
	os_cursor_resize_EW,
	os_cursor_resize_NS,
	os_cursor_resize_NWSE,
	os_cursor_resize_NESW,
	os_cursor_resize_all,
	os_cursor_hand_point,
	os_cursor_disabled,
	os_cursor_count,
};

// structs

struct os_window_t; // defined in backends.

struct os_event_t {

	// event list
	os_event_t* next;
	os_event_t* prev;

	os_event_type type;
	os_window_t* window;
	os_key key;
	os_mouse_button mouse;
	os_modifiers modifiers;
	u32 character;
	vec2_t position;
	vec2_t scroll;
};

struct os_event_list_t {
	os_event_t* first;
	os_event_t* last;
	u32 count;
};

struct os_file_attributes_t {
	u32 size;
	u32 last_modified;
};

struct os_file_t; // defined in backends.

struct os_thread_t; // defined in backends.

struct os_state_t; // defined in backends.

// functions

// state
function void os_init();
function void os_release();
function void os_update();
function b8 os_any_window_exist();
function void os_abort(u32);
function u64 os_time_microseconds();

function void os_set_cursor(os_cursor);
function vec2_t os_get_cursor_pos(os_window_t*);
function void os_set_cursor_pos(os_window_t*, vec2_t);

// events
function void os_event_push(os_event_t*);
function void os_event_pop(os_event_t*);
function os_modifiers os_get_modifiers();
function b8 os_key_press(os_window_t*, os_key, os_modifiers);
function b8 os_key_release(os_window_t*, os_key, os_modifiers);
function b8 os_mouse_press(os_window_t*, os_mouse_button, os_modifiers);
function b8 os_mouse_release(os_window_t*, os_mouse_button, os_modifiers);
function f32 os_mouse_scroll(os_window_t*);
function vec2_t os_mouse_move(os_window_t*);
function b8 os_mouse_button_is_down(os_mouse_button);
function b8 os_key_is_down(os_key);

// window

function os_window_t* os_window_open(str_t, u32, u32);
function void os_window_close(os_window_t*);
function b8 os_window_is_running(os_window_t*);
function void os_window_minimize(os_window_t*);
function void os_window_maximize(os_window_t*);
function void os_window_restore(os_window_t*);
function void os_window_fullscreen(os_window_t*);
function void os_window_set_title(os_window_t*, str_t);
function void os_window_set_resize_function(os_window_t*, os_window_resize_func*);
function void os_window_set_close_function(os_window_t*, os_window_close_func*);

// memory

function u32 os_page_size();
function void* os_mem_reserve(u32);
function void os_mem_release(void*, u32);
function void os_mem_commit(void*, u32);
function void os_mem_decommit(void*, u32);

// file 

function os_file_t os_file_open(str_t, os_file_access_flag = os_file_access_flag_read | os_file_access_flag_share_read | os_file_access_flag_share_write);
function void os_file_close(os_file_t);
function os_file_attributes_t os_file_get_attributes(os_file_t);
function os_file_attributes_t os_file_get_attributes(str_t);
function str_t os_file_read_range(arena_t*, os_file_t, u32, u32);
function str_t os_file_read_all(arena_t*, str_t);
function str_t os_file_read_all(arena_t*, os_file_t);

function void os_file_delete(str_t);
function void os_file_move(str_t, str_t);
function void os_file_copy(str_t, str_t);

// thread
function os_thread_t* os_thread_create(os_thread_function*, str_t);
function void os_thread_release(os_thread_t*);
function void os_thread_set_name(os_thread_t*, str_t);
function void os_thread_join(os_thread_t*);
function void os_thread_detach(os_thread_t*);

// backend includes

#define OS_BACKEND_WIN32

#if defined(OS_BACKEND_WIN32)
#include "backends/os/os_win32.h"
#elif defined*OS_BACKEND_LINUX)
#include "backends/os/os_linux.h"
#elif defined(OS_BACKEND_MACOS)
#include "backends/os/os_macos.h"
#else 
#error undefined os backend.
#endif 

#endif // OS_H