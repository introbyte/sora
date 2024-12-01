// os_win32.h

#ifndef OS_WIN32_H
#define OS_WIN32_H

// includes
#include <windows.h>
#include <timeapi.h>
#include <dwmapi.h>

// defines

#define WM_NCUAHDRAWCAPTION (0x00AE)
#define WM_NCUAHDRAWFRAME (0x00AF)

// structs

struct os_window_t {

	// window list 
	os_window_t* next;
	os_window_t* prev;

	// flags
	os_window_flags flags;

	// win32
	HWND handle;
	//HDC hdc;

	// info
	str_t title;

	// sizing
	uvec2_t resolution;
	WINDOWPLACEMENT last_window_placement; // for fullscreen
	b8 borderless;
	b8 composition_enabled;
	b8 maximized;

	// custom title bar client area
	arena_t* title_bar_arena;
	os_title_bar_client_area_t* title_bar_client_area_first;
	os_title_bar_client_area_t* title_bar_client_area_last;

	// time
	LARGE_INTEGER tick_current;
	LARGE_INTEGER tick_previous;
	f64 delta_time;
	f64 elasped_time;

	// input state
	vec2_t mouse_pos;
	vec2_t mouse_pos_last;
	vec2_t mouse_delta;
};

struct os_file_t {
	HANDLE handle;
	os_file_attributes_t attributes;
};

struct os_thread_t {
	os_thread_t* next;
	HANDLE handle;
	DWORD thread_id;
	void* params;
	os_thread_function* func;
};

struct os_state_t {

	// arenas
	arena_t* window_arena;
	arena_t* event_list_arena;
	arena_t* thread_arena;
	arena_t* scratch_arena;

	// window list
	os_window_t* first_window;
	os_window_t* last_window;
	os_window_t* free_window;

	// event list
	os_event_list_t event_list;

	// time
	LARGE_INTEGER time_frequency;
	UINT blink_time;
	UINT double_click_time;

	// cursor
	HCURSOR cursors[os_cursor_count];

	// thread
	SRWLOCK thread_srw_lock;
	os_thread_t* thread_free;

	// log
	os_file_t log_file;

	// input state
	b8 keys[255];
	b8 mouse_buttons[os_mouse_button_count];

};

// global

global os_state_t os_state;
global b8 os_new_borderless_window = false;

// functions

function DWORD os_win32_thread_entry_point(void*);
LRESULT CALLBACK window_procedure(HWND, UINT, WPARAM, LPARAM);

#endif // OS_WIN32_H