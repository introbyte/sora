// os.cpp

#ifndef OS_CPP
#define OS_CPP

// include libs
#pragma comment(lib, "user32")
#pragma comment(lib, "winmm")

// implementation

function void 
os_init() {

	// arenas
	os_state.window_arena = arena_create(megabytes(8));
	os_state.event_list_arena = arena_create(megabytes(8));
	os_state.scratch_arena = arena_create(megabytes(8));

	// time
	timeBeginPeriod(1);
	QueryPerformanceFrequency(&os_state.time_frequency);

	// register window class
	WNDCLASS window_class = { 0 };
	window_class.lpfnWndProc = window_procedure;
	window_class.lpszClassName = "sora_window_class";
	window_class.hInstance = GetModuleHandle(NULL);
	window_class.hCursor = LoadCursorA(0, IDC_ARROW);
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassA(&window_class);



}

function void 
os_release() {
	arena_release(os_state.window_arena);
	arena_release(os_state.event_list_arena);
	arena_release(os_state.scratch_arena);
}

function void 
os_update() {
	
	// clear event list
	arena_clear(os_state.event_list_arena);
	os_state.event_list = { 0 };

	for (MSG message; PeekMessage(&message, 0, 0, 0, PM_REMOVE);) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}


function void 
os_pop_event(os_event_t* event) {
	dll_remove(os_state.event_list.first, os_state.event_list.last, event);
	event->type = os_event_type_null;
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
			os_pop_event(e);
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
			os_pop_event(e);
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
			os_pop_event(e);
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
			os_pop_event(e);
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
			os_pop_event(e);
			result = e->scroll.y;
			break;
		}
	}
	return result;
}



function os_window_t* 
os_window_open(str_t title, u32 width, u32 height) {

	// find window
	os_window_t* window = nullptr;
	window = os_state.free_window;
	if (window != nullptr) {
		stack_pop(os_state.free_window);
	} else {
		window = (os_window_t*)arena_malloc(os_state.window_arena, sizeof(os_window_t));
	}
	memset(window, 0, sizeof(os_window_t));

	dll_push_back(os_state.first_window, os_state.last_window, window);

	// adjust window size
	RECT rect = { 0, 0, width, height };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	i32 adjusted_width = rect.right - rect.left;
	i32 adjusted_height = rect.bottom - rect.top;

	// open window
	window->handle = CreateWindowExA(0, "sora_window_class", (char*)title.data, 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, adjusted_width, adjusted_height, 
		nullptr, nullptr, GetModuleHandle(NULL), nullptr);
	window->hdc = GetDC(window->handle);
	SetWindowLongPtr(window->handle, GWLP_USERDATA, (LONG_PTR)window);
	ShowWindow(window->handle, SW_SHOW);

	window->width = width;
	window->height = height;

	// fullscreen
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
os_window_update(os_window_t* window) {

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

}

function void
os_window_set_title(os_window_t* window, str_t title) {
	SetWindowTextA(window->handle, (char*)title.data);
}


// memory 

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


// window procedure

LRESULT CALLBACK
window_procedure(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam) {

	os_window_t* window = (os_window_t*)GetWindowLongPtr(handle, GWLP_USERDATA);
	os_event_t* event = nullptr;
	LRESULT result = 0;

	switch (msg) {
		case WM_CLOSE: {
			os_window_close(window);
			break;
		}

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN: {
			u32 key = (u32)wparam;
			event = (os_event_t*)arena_malloc(os_state.event_list_arena, sizeof(os_event_t));
			event->window = window;
			event->type = os_event_type_key_press;
			event->key = (os_key)key;
			break;
		}

		case WM_SYSKEYUP:
		case WM_KEYUP: {
			u32 key = (u32)wparam;
			event = (os_event_t*)arena_malloc(os_state.event_list_arena, sizeof(os_event_t));
			event->window = window;
			event->type = os_event_type_key_release;
			event->key = (os_key)key;
			break;
		}

		case WM_MOUSEMOVE: {
			f32 mouse_x = (f32)LOWORD(lparam);
			f32 mouse_y = (f32)HIWORD(lparam);
			event = (os_event_t*)arena_malloc(os_state.event_list_arena, sizeof(os_event_t));
			event->window = window;
			event->type = os_event_type_mouse_move;
			event->position = { mouse_x, mouse_y };
			break;
		}

		case WM_SYSCHAR:
		case WM_CHAR: {
			u32 key = (u32)wparam;

			if (key == '\r') { key = '\n'; }

			if ((key >= 32 && key != 127) || key == '\t' || key == '\n') {
				event = (os_event_t*)arena_malloc(os_state.event_list_arena, sizeof(os_event_t));
				event->window = window;
				event->type = os_event_type_text;
				event->character = key;
			}

			break;
		}

		case WM_MOUSEWHEEL: {
			f32 delta = (f32)GET_WHEEL_DELTA_WPARAM(wparam) / WHEEL_DELTA;
			event = (os_event_t*)arena_malloc(os_state.event_list_arena, sizeof(os_event_t));
			event->window = window;
			event->type = os_event_type_mouse_scroll;
			event->scroll = { 0.0f, delta };
			break;
		}

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN: {
			SetCapture(handle);
			event = (os_event_t*)arena_malloc(os_state.event_list_arena, sizeof(os_event_t));
			event->window = window;
			event->type = os_event_type_mouse_press;
			event->mouse = os_mouse_button_left;
			switch (msg) {
				case WM_RBUTTONDOWN: event->mouse = os_mouse_button_right; break;
				case WM_MBUTTONDOWN: event->mouse = os_mouse_button_middle; break;
			}
			break;
		}

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP: {
			ReleaseCapture();
			event = (os_event_t*)arena_malloc(os_state.event_list_arena, sizeof(os_event_t));
			event->window = window;
			event->type = os_event_type_mouse_release;
			event->mouse = os_mouse_button_left;
			switch (msg) {
				case WM_RBUTTONUP: event->mouse = os_mouse_button_right; break;
				case WM_MBUTTONUP: event->mouse = os_mouse_button_middle; break;
			}
			break;
		}

		default: {
			result = DefWindowProcA(handle, msg, wparam, lparam);
			break;
		}

	}

	if (event) {
		event->modifiers = os_get_modifiers();
		dll_push_back(os_state.event_list.first, os_state.event_list.last, event);
	}

	return result;

}

#endif // OS_CPP