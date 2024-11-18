// os_win32.cpp

#ifndef OS_WIN32_CPP
#define OS_WIN32_CPP

// include lib
#pragma comment(lib, "user32")
#pragma comment(lib, "gdi32")
#pragma comment(lib, "winmm")
#pragma comment(lib, "dwmapi")


// implementation

function void
os_init() {

	// arenas
	os_state.window_arena = arena_create(megabytes(8));
	os_state.event_list_arena = arena_create(megabytes(8));
	os_state.thread_arena = arena_create(megabytes(8));
	os_state.scratch_arena = arena_create(megabytes(8));

	// time
	timeBeginPeriod(1);
	QueryPerformanceFrequency(&os_state.time_frequency);
	os_state.blink_time = GetCaretBlinkTime();
	os_state.double_click_time = GetDoubleClickTime();

	// register window class
	WNDCLASS window_class = { 0 };
	window_class.lpfnWndProc = window_procedure;
	window_class.lpszClassName = "sora_window_class";
	window_class.hInstance = GetModuleHandle(NULL);
	window_class.hCursor = LoadCursorA(0, IDC_ARROW);
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassA(&window_class);

	// load cursors
	os_state.cursors[os_cursor_pointer] = LoadCursorA(NULL, IDC_ARROW);
	os_state.cursors[os_cursor_I_beam] = LoadCursorA(NULL, IDC_IBEAM);
	os_state.cursors[os_cursor_resize_EW] = LoadCursorA(NULL, IDC_SIZEWE);
	os_state.cursors[os_cursor_resize_NS] = LoadCursorA(NULL, IDC_SIZENS);
	os_state.cursors[os_cursor_resize_NWSE] = LoadCursorA(NULL, IDC_SIZENWSE);
	os_state.cursors[os_cursor_resize_NESW] = LoadCursorA(NULL, IDC_SIZENESW);
	os_state.cursors[os_cursor_resize_all] = LoadCursorA(NULL, IDC_SIZEALL);
	os_state.cursors[os_cursor_hand_point] = LoadCursorA(NULL, IDC_HAND);
	os_state.cursors[os_cursor_disabled] = LoadCursorA(NULL, IDC_NO);

	// init locks
	InitializeSRWLock(&os_state.thread_srw_lock);

	// clear input state
	for (i32 i = 0; i < 255; i++) {
		os_state.keys[i] = false;
	}

	for (i32 i = 0; i < os_mouse_button_count; i++) {
		os_state.mouse_buttons[i] = false;
	}

}

function void
os_release() {

	// release arenas
	arena_release(os_state.window_arena);
	arena_release(os_state.event_list_arena);
	arena_release(os_state.thread_arena);
	arena_release(os_state.scratch_arena);
}

function void
os_update() {

	// clear event list
	arena_clear(os_state.event_list_arena);
	os_state.event_list = { 0 };

	// dispatch win32 messages
	for (MSG message; PeekMessageA(&message, 0, 0, 0, PM_REMOVE);) {
		TranslateMessage(&message);
		DispatchMessageA(&message);
	}

	// update each window
	for (os_window_t* window = os_state.first_window; window != 0; window = window->next) {

		// window size
		RECT w32_rect = { 0 };
		GetClientRect(window->handle, &w32_rect);
		window->resolution = uvec2((w32_rect.right - w32_rect.left), (w32_rect.bottom - w32_rect.top));

		// mouse position
		POINT cursor_pos;
		GetCursorPos(&cursor_pos);
		ScreenToClient(window->handle, &cursor_pos);
		window->mouse_pos_last = window->mouse_pos;
		window->mouse_pos = { (f32)cursor_pos.x, (f32)cursor_pos.y };
		window->mouse_delta = vec2_sub(window->mouse_pos, window->mouse_pos_last);

		// time
		window->tick_previous = window->tick_current;
		QueryPerformanceCounter(&window->tick_current);
		window->delta_time = (f64)(window->tick_current.QuadPart - window->tick_previous.QuadPart) / (f64)os_state.time_frequency.QuadPart;
		window->elasped_time += window->delta_time;
	}

}

function b8
os_any_window_exist() {
	return os_state.first_window != nullptr;
}

function void
os_abort(u32 exit_code) {
	ExitProcess(exit_code);
}

function u64
os_time_microseconds() {
	LARGE_INTEGER current_time;
	QueryPerformanceCounter(&current_time);
	f64 time_in_seconds = ((f64)current_time.QuadPart) / ((f64)os_state.time_frequency.QuadPart);
	u64 time = (u64)(time_in_seconds * 1000000);
	return time;
}

function void
os_set_cursor(os_cursor cursor) {
	if (cursor == os_cursor_null) {
		ShowCursor(false);
	} else {
		ShowCursor(true);
		HCURSOR hcursor = os_state.cursors[cursor];
		SetCursor(hcursor);
	}
}

function vec2_t
os_get_cursor_pos(os_window_t* window) {
	POINT cursor_pos;
	GetCursorPos(&cursor_pos);
	ScreenToClient(window->handle, &cursor_pos);
	return vec2(cursor_pos.x, cursor_pos.y);
}

function void
os_set_cursor_pos(os_window_t* window, vec2_t pos) {
	POINT p = { pos.x, pos.y };
	ClientToScreen(window->handle, &p);
	SetCursorPos(p.x, p.y);
}

// event functions

function void
os_event_push(os_event_t* event) {
	os_event_t* new_event = (os_event_t*)arena_alloc(os_state.event_list_arena, sizeof(os_event_t));
	memcpy(new_event, event, sizeof(os_event_t));
	dll_push_back(os_state.event_list.first, os_state.event_list.last, new_event);
	os_state.event_list.count++;
}

function void
os_event_pop(os_event_t* event) {
	dll_remove(os_state.event_list.first, os_state.event_list.last, event);
	os_state.event_list.count--;
}

function os_event_t* 
os_event_get(os_event_type type) {
	os_event_t* result = nullptr;
	for (os_event_t* e = os_state.event_list.first; e != 0; e = e->next) {
		if (e->type == type) {
			result = e;
			break;
		}
	}
	return result;
}

function os_modifiers
os_get_modifiers() {
	os_modifiers modifiers = 0;
	if (GetKeyState(VK_CONTROL) & 0x8000) {
		modifiers |= os_modifier_ctrl;
	}
	if (GetKeyState(VK_SHIFT) & 0x8000) {
		modifiers |= os_modifier_shift;
	}
	if (GetKeyState(VK_MENU) & 0x8000) {
		modifiers |= os_modifier_alt;
	}
	return modifiers;
}

function b8
os_key_press(os_window_t* window, os_key key, os_modifiers modifiers = 0) {
	b8 result = 0;
	for (os_event_t* e = os_state.event_list.first; e != 0; e = e->next) {
		if (e->type == os_event_type_key_press && (window == e->window) &&
			e->key == key &&
			((e->modifiers & modifiers) != 0 || (e->modifiers == 0 && modifiers == 0))) {
			os_event_pop(e);
			result = 1;
			break;
		}
	}
	return result;
}

function b8
os_key_release(os_window_t* window, os_key key, os_modifiers modifiers = 0) {
	b8 result = 0;
	for (os_event_t* e = os_state.event_list.first; e != 0; e = e->next) {
		if (e->type == os_event_type_key_release && (window == e->window) &&
			e->key == key &&
			((e->modifiers & modifiers) || (e->modifiers == 0 && modifiers == 0))) {
			os_event_pop(e);
			result = 1;
			break;
		}
	}
	return result;
}

function b8
os_mouse_press(os_window_t* window, os_mouse_button button, os_modifiers modifiers = 0) {
	b8 result = 0;
	for (os_event_t* e = os_state.event_list.first; e != 0; e = e->next) {
		if (e->type == os_event_type_mouse_press && (window == e->window) &&
			e->mouse == button &&
			((e->modifiers & modifiers) || (e->modifiers == 0 && modifiers == 0))) {
			os_event_pop(e);
			result = 1;
			break;
		}
	}
	return result;
}

function b8
os_mouse_release(os_window_t* window, os_mouse_button button, os_modifiers modifiers = 0) {
	b8 result = 0;
	for (os_event_t* e = os_state.event_list.first; e != 0; e = e->next) {
		if (e->type == os_event_type_mouse_release && (window == e->window) &&
			e->mouse == button &&
			((e->modifiers & modifiers) || (e->modifiers == 0 && modifiers == 0))) {
			os_event_pop(e);
			result = 1;
			break;
		}
	}
	return result;
}

function f32
os_mouse_scroll(os_window_t* window) {
	f32 result = 0.0f;
	for (os_event_t* e = os_state.event_list.first; e != 0; e = e->next) {
		if (e->type == os_event_type_mouse_scroll && (window == e->window)) {
			os_event_pop(e);
			result = e->scroll.y;
			break;
		}
	}
	return result;
}

function vec2_t
os_mouse_move(os_window_t* window) {

	vec2_t result = vec2(0.0f, 0.0f);

	for (os_event_t* e = os_state.event_list.first; e != 0; e = e->next) {
		if (e->type == os_event_type_mouse_move && (window == e->window)) {
			os_event_pop(e);
			result = e->position;
			break;
		}
	}

	return result;
}

function b8
os_mouse_button_is_down(os_mouse_button mouse_button) {
	return os_state.mouse_buttons[mouse_button];
}

function b8
os_key_is_down(os_key key) {
	return os_state.keys[key];
}

// window functions

function os_window_t*
os_window_open(str_t title, u32 width, u32 height, os_window_flags flags) {

	// find window
	os_window_t* window = nullptr;
	window = os_state.free_window;
	if (window != nullptr) {
		stack_pop(os_state.free_window);
	} else {
		window = (os_window_t*)arena_alloc(os_state.window_arena, sizeof(os_window_t));
	}
	memset(window, 0, sizeof(os_window_t));
	dll_push_back(os_state.first_window, os_state.last_window, window);

	// adjust window size
	DWORD style = WS_OVERLAPPEDWINDOW;
	RECT rect = { 0, 0, width, height };
	AdjustWindowRect(&rect, style, FALSE);
	i32 adjusted_width = rect.right - rect.left;
	i32 adjusted_height = rect.bottom - rect.top;

	// open window
	window->handle = CreateWindowExA(0, "sora_window_class", (char*)title.data,
		style, CW_USEDEFAULT, CW_USEDEFAULT, adjusted_width, adjusted_height,
		nullptr, nullptr, GetModuleHandle(NULL), nullptr);
	window->hdc = GetDC(window->handle);
	SetWindowLongPtr(window->handle, GWLP_USERDATA, (LONG_PTR)window);
	ShowWindow(window->handle, SW_SHOW);

	// fill stuct
	window->flags = flags;
	window->title = title;
	window->resolution = uvec2(width, height);
	QueryPerformanceCounter(&window->tick_current);
	window->tick_previous = window->tick_current;
	window->delta_time = 0.0;
	window->elasped_time = 0.0;

	// for fullscreen
	window->last_window_placement.length = sizeof(WINDOWPLACEMENT);

	return window;
}

function void
os_window_close(os_window_t* window) {
	dll_remove(os_state.first_window, os_state.last_window, window);
	stack_push(os_state.free_window, window);
	if (window->hdc) { ReleaseDC(window->handle, window->hdc); }
	if (window->handle) { DestroyWindow(window->handle); }
}

function void
os_window_minimize(os_window_t* window) {
	ShowWindow(window->handle, SW_MINIMIZE);
}

function void
os_window_maximize(os_window_t* window) {
	ShowWindow(window->handle, SW_MAXIMIZE);
}

function void
os_window_restore(os_window_t* window) {
	ShowWindow(window->handle, SW_RESTORE);
}

function void
os_window_fullscreen(os_window_t* window) {

	DWORD window_style = GetWindowLong(window->handle, GWL_STYLE);
	if (window_style & WS_OVERLAPPEDWINDOW) {
		MONITORINFO monitor_info = { sizeof(monitor_info) };
		if (GetWindowPlacement(window->handle, &window->last_window_placement) &&
			GetMonitorInfo(MonitorFromWindow(window->handle, MONITOR_DEFAULTTOPRIMARY), &monitor_info)) {
			SetWindowLong(window->handle, GWL_STYLE, window_style & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(window->handle, HWND_TOP,
				monitor_info.rcMonitor.left,
				monitor_info.rcMonitor.top,
				monitor_info.rcMonitor.right -
				monitor_info.rcMonitor.left,
				monitor_info.rcMonitor.bottom -
				monitor_info.rcMonitor.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
	} else {
		SetWindowLong(window->handle, GWL_STYLE, window_style | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(window->handle, &window->last_window_placement);
		SetWindowPos(window->handle, 0, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}

}

function void
os_window_set_title(os_window_t* window, str_t title) {
	SetWindowTextA(window->handle, (char*)title.data);
}

function u32
os_window_get_dpi(os_window_t* window) {
	return GetDpiForWindow(window->handle);
}

// memory functions

function u32
os_page_size() {
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return info.dwPageSize;
}

function void*
os_mem_reserve(u32 size) {
	u32 size_snapped = size;
	size_snapped += gigabytes(1) - 1;
	size_snapped -= size_snapped % gigabytes(1);
	void* ptr = VirtualAlloc(0, size_snapped, MEM_RESERVE, PAGE_NOACCESS);
	return ptr;
}

function void
os_mem_release(void* ptr, u32 size) {
	VirtualFree(ptr, 0, MEM_RELEASE);
}

function void
os_mem_commit(void* ptr, u32 size) {
	u32 page_snapped = size;
	page_snapped += os_page_size() - 1;
	page_snapped -= page_snapped % os_page_size();
	VirtualAlloc(ptr, page_snapped, MEM_COMMIT, PAGE_READWRITE);
}

function void
os_mem_decommit(void* ptr, u32 size) {
	VirtualFree(ptr, size, MEM_DECOMMIT);
}

// files

function os_file_t
os_file_open(str_t filepath, os_file_access_flag flags) {

	os_file_t file = { 0 };

	DWORD access_flags = 0;
	DWORD share_mode = 0;
	DWORD creation_disposition = OPEN_EXISTING;

	if (flags & os_file_access_flag_read) { access_flags |= GENERIC_READ; }
	if (flags & os_file_access_flag_write) { access_flags |= GENERIC_WRITE; }
	if (flags & os_file_access_flag_execute) { access_flags |= GENERIC_EXECUTE; }
	if (flags & os_file_access_flag_share_read) { share_mode |= FILE_SHARE_READ; }
	if (flags & os_file_access_flag_share_write) { share_mode |= FILE_SHARE_WRITE; }
	if (flags & os_file_access_flag_write) { creation_disposition = CREATE_ALWAYS; }
	if (flags & os_file_access_flag_append) { creation_disposition = OPEN_ALWAYS; }
	if (flags & os_file_access_flag_attribute) { access_flags = READ_CONTROL | FILE_READ_ATTRIBUTES;  share_mode = FILE_SHARE_READ; }

	file.handle = CreateFileA((char*)filepath.data, access_flags, share_mode, NULL, creation_disposition, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file.handle == INVALID_HANDLE_VALUE) {
		printf("[error] failed to open file '%.*s'\n", filepath.size, filepath.data);
		file.handle = nullptr;
	} else {
		os_file_attributes_t attributes = os_file_get_attributes(file);
		file.attributes = attributes;
	}

	return file;
}

function void
os_file_close(os_file_t file) {
	CloseHandle(file.handle);
}

function os_file_attributes_t
os_file_get_attributes(os_file_t file) {
	os_file_attributes_t attributes = { 0 };
	u32 high_bits = 0;
	u32 low_bits = GetFileSize(file.handle, (DWORD*)&high_bits);
	FILETIME last_write_time = { 0 };
	GetFileTime(file.handle, 0, 0, &last_write_time);
	attributes.size = (u64)low_bits | (((u64)high_bits) << 32);
	attributes.last_modified = ((u64)last_write_time.dwLowDateTime) | (((u64)last_write_time.dwHighDateTime) << 32);
	return attributes;
}

function os_file_attributes_t
os_file_get_attributes(str_t filepath) {
	WIN32_FILE_ATTRIBUTE_DATA file_info;
	GetFileAttributesExA((char*)filepath.data, GetFileExInfoStandard, &file_info);
	os_file_attributes_t attributes = { 0 };
	attributes.last_modified = ((u64)file_info.ftLastWriteTime.dwLowDateTime) | (((u64)file_info.ftLastWriteTime.dwHighDateTime) << 32);
	attributes.size = (u64)file_info.nFileSizeLow | (((u64)file_info.nFileSizeHigh) << 32);
	return attributes;
}


function str_t
os_file_read_range(arena_t* arena, os_file_t file, u32 start, u32 length) {

	str_t result;

	LARGE_INTEGER off_li = { 0 };
	off_li.QuadPart = start;

	if (SetFilePointerEx(file.handle, off_li, 0, FILE_BEGIN)) {
		u32 bytes_to_read = length;
		u32 bytes_actually_read = 0;
		result.data = (u8*)arena_alloc(arena, sizeof(u8) * bytes_to_read);
		result.size = 0;

		u8* ptr = (u8*)result.data;
		u8* opl = (u8*)result.data + bytes_to_read;

		for (;;) {
			u32 unread = (u32)(opl - ptr);
			DWORD to_read = (DWORD)(min(unread, u32_max));
			DWORD did_read = 0;
			if (!ReadFile(file.handle, ptr, to_read, &did_read, 0)) {
				break;
			}
			ptr += did_read;
			result.size += did_read;
			if (ptr >= opl) {
				break;
			}
		}
	}
	return result;

}

function str_t
os_file_read_all(arena_t* arena, str_t filepath) {
	str_t data = str("");
	os_file_t file = os_file_open(filepath);
	if (file.handle != nullptr) {
		os_file_attributes_t attributes = os_file_get_attributes(file);
		data = os_file_read_range(arena, file, 0, attributes.size);
		os_file_close(file);
	}
	return data;
}

function str_t
os_file_read_all(arena_t* arena, os_file_t file) {
	os_file_attributes_t attributes = os_file_get_attributes(file);
	str_t data = os_file_read_range(arena, file, 0, attributes.size);
	return data;
}

function void
os_file_delete(str_t filepath) {
	DeleteFileA((char*)filepath.data);
}

function void
os_file_move(str_t src_path, str_t dst_path) {
	MoveFileA((char*)src_path.data, (char*)dst_path.data);
}

function void
os_file_copy(str_t src_path, str_t dst_path) {
	CopyFileA((char*)src_path.data, (char*)dst_path.data, true);
}

// thread functions

function os_thread_t*
os_thread_create(os_thread_function* thread_function, str_t name = str("")) {

	os_thread_t* thread = { 0 };

	// find or create thread
	AcquireSRWLockExclusive(&os_state.thread_srw_lock);
	thread = os_state.thread_free;
	if (thread != nullptr) {
		stack_pop(os_state.thread_free);
	} else {
		thread = (os_thread_t*)arena_alloc(os_state.thread_arena, sizeof(os_thread_t));
	}
	memset(thread, 0, sizeof(os_thread_t));
	ReleaseSRWLockExclusive(&os_state.thread_srw_lock);

	if (thread != nullptr) {
		thread->func = thread_function;
		thread->handle = CreateThread(NULL, 0, os_win32_thread_entry_point, thread, 0, &thread->thread_id);
	}

	os_thread_set_name(thread, name);

	return thread;
}

function void
os_thread_release(os_thread_t* thread) {
	AcquireSRWLockExclusive(&os_state.thread_srw_lock);
	stack_push(os_state.thread_free, thread);
	ReleaseSRWLockExclusive(&os_state.thread_srw_lock);
}

function void
os_thread_set_name(os_thread_t* thread, str_t name) {
	str16_t thread_wide = str_to_str16(os_state.scratch_arena, name);
	SetThreadDescription(thread->handle, (WCHAR*)thread_wide.data);
}

function void
os_thread_join(os_thread_t* thread) {
	if (thread != nullptr) {
		if (thread->handle != nullptr) {
			WaitForSingleObject(thread->handle, INFINITE);
			CloseHandle(thread->handle);
		}

		os_thread_release(thread);
	}
}

function void
os_thread_detach(os_thread_t* thread) {
	if (thread != nullptr) {
		if (thread->handle != nullptr) {
			CloseHandle(thread->handle);
		}
		os_thread_release(thread);
	}
}

function DWORD
os_win32_thread_entry_point(void* params) {
	os_thread_t* thread = (os_thread_t*)params;
	thread->func();
	return 0;
}

// window procedure

LRESULT CALLBACK
window_procedure(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam) {

	os_window_t* window = (os_window_t*)GetWindowLongPtr(handle, GWLP_USERDATA);
	os_event_t* event = nullptr;
	LRESULT result = 0;

	switch (msg) {

		case WM_CLOSE: {

			event = (os_event_t*)arena_calloc(os_state.event_list_arena, sizeof(os_event_t));
			event->window = window;
			event->type = os_event_type_window_close;
			
			break;
		}

		case WM_SIZE: {
			if (window != nullptr) {
				UINT width = LOWORD(lparam);
				UINT height = HIWORD(lparam);
				window->resolution = uvec2(width, height);

				event = (os_event_t*)arena_calloc(os_state.event_list_arena, sizeof(os_event_t));
				event->window = window;
				event->type = os_event_type_window_resize;

			}
			result = DefWindowProcA(handle, msg, wparam, lparam);
			break;
		}

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN: {
			u32 key = (u32)wparam;

			event = (os_event_t*)arena_calloc(os_state.event_list_arena, sizeof(os_event_t));
			event->window = window;
			event->type = os_event_type_key_press;
			event->key = (os_key)key;
			os_state.keys[key] = true;
			break;
		}

		case WM_SYSKEYUP:
		case WM_KEYUP: {
			u32 key = (u32)wparam;
			event = (os_event_t*)arena_calloc(os_state.event_list_arena, sizeof(os_event_t));
			event->window = window;
			event->type = os_event_type_key_release;
			event->key = (os_key)key;
			os_state.keys[key] = false;
			break;
		}

		case WM_MOUSEMOVE: {
			//f32 mouse_x = (f32)LOWORD(lparam);
			//f32 mouse_y = (f32)HIWORD(lparam);
			event = (os_event_t*)arena_calloc(os_state.event_list_arena, sizeof(os_event_t));
			event->window = window;
			event->type = os_event_type_mouse_move;
			//event->position = { mouse_x, mouse_y };
			break;
		}

		case WM_SYSCHAR:
		case WM_CHAR: {
			u32 key = (u32)wparam;

			if (key == '\r') { key = '\n'; }

			if ((key >= 32 && key != 127) || key == '\t' || key == '\n') {
				event = (os_event_t*)arena_calloc(os_state.event_list_arena, sizeof(os_event_t));
				event->window = window;
				event->type = os_event_type_text;
				event->character = key;
			}

			break;
		}

		case WM_MOUSEWHEEL: {
			f32 delta = (f32)GET_WHEEL_DELTA_WPARAM(wparam) / WHEEL_DELTA;
			event = (os_event_t*)arena_calloc(os_state.event_list_arena, sizeof(os_event_t));
			event->window = window;
			event->type = os_event_type_mouse_scroll;
			event->scroll = { 0.0f, delta };
			break;
		}

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN: {
			SetCapture(handle);
			event = (os_event_t*)arena_calloc(os_state.event_list_arena, sizeof(os_event_t));
			event->window = window;
			event->type = os_event_type_mouse_press;
			event->mouse = os_mouse_button_left;
			switch (msg) {
				case WM_RBUTTONDOWN: event->mouse = os_mouse_button_right; break;
				case WM_MBUTTONDOWN: event->mouse = os_mouse_button_middle; break;
			}
			os_state.mouse_buttons[event->mouse] = true;
			break;
		}

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP: {
			ReleaseCapture();
			event = (os_event_t*)arena_calloc(os_state.event_list_arena, sizeof(os_event_t));
			event->window = window;
			event->type = os_event_type_mouse_release;
			event->mouse = os_mouse_button_left;
			switch (msg) {
				case WM_RBUTTONUP: event->mouse = os_mouse_button_right; break;
				case WM_MBUTTONUP: event->mouse = os_mouse_button_middle; break;
			}
			os_state.mouse_buttons[event->mouse] = false;
			break;
		}
		
		default: {
			result = DefWindowProcA(handle, msg, wparam, lparam);
			break;
		}

	}

	// add event to event list
	if (event) {
		event->modifiers = os_get_modifiers();
		event->position = window->mouse_pos;
		dll_push_back(os_state.event_list.first, os_state.event_list.last, event);
		os_state.event_list.count++;
	}

	return result;

}


#endif // OS_WIN32_CPP