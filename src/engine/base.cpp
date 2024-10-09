// base.cpp

#ifndef BASE_CPP
#define BASE_CPP

// implementation

function arena_t*
arena_create(u32 size) {

	// roundup
	u32 size_roundup = megabytes(64);
	size += size_roundup - 1;
	size -= size % size_roundup;

	// reserve memory
	void* block = os_mem_reserve(size);

	// initial commit
	u64 initial_commit_size = arena_commit_size;
	os_mem_commit(block, initial_commit_size);

	// fill struct
	arena_t* arena = (arena_t*)block;
	arena->pos = sizeof(arena_t);
	arena->commit_pos = initial_commit_size;
	arena->align = 8;
	arena->size = size;

	return arena;
}

function void
arena_release(arena_t* arena) {
	os_mem_release(arena, arena->size);
}

function void*
arena_alloc(arena_t* arena, u32 size) {

	void* result = nullptr;

	if (arena->pos + size <= arena->size) {

		u8* base = (u8*)arena;

		// align
		u32 post_align_pos = (arena->pos + (arena->align - 1));
		post_align_pos -= post_align_pos % arena->align;
		u32 align = post_align_pos - arena->pos;
		result = base + arena->pos + align;
		arena->pos += size + align;

		// commit
		if (arena->commit_pos < arena->pos) {
			u32 size_to_commit = arena->pos - arena->commit_pos;
			size_to_commit += arena_commit_size - 1;
			size_to_commit -= size_to_commit % arena_commit_size;
			os_mem_commit(base + arena->commit_pos, size_to_commit);
			arena->commit_pos += size_to_commit;
		}

	} else {
		printf("[error] arena is full.\n");
	}

	return result;
}

function void*
arena_calloc(arena_t* arena, u32 size) {
	void* result = arena_alloc(arena, size);
	memset(result, 0, size);
	return result;
}

function void
arena_clear(arena_t* arena) {

	// set pos to min
	u32 min_pos = sizeof(arena_t);
	arena->pos = min_pos;

	// align pos
	u32 pos_aligned = arena->pos + arena_commit_size - 1;
	pos_aligned -= pos_aligned % arena_commit_size;

	// decommit
	if (pos_aligned + arena_decommit_size <= arena->commit_pos) {
		u8* base = (u8*)arena;
		u32 size_to_decommit = arena->commit_pos - pos_aligned;
		os_mem_decommit(base + pos_aligned, size_to_decommit);
		arena->commit_pos -= size_to_decommit;
	}

}


// cstr functions

function u32
cstr_length(cstr cstr) {
	u32 i;
	for (i = 0; cstr[i] != '\0'; i++);
	return i;
}

function b8
cstr_equals(cstr cstr1, cstr cstr2) {
	while (*cstr1 && (*cstr1 == *cstr2)) {
		cstr1++;
		cstr2++;
	}
	return (*(const unsigned char*)cstr1 - *(const unsigned char*)cstr2) == 0;
}

function b8
char_is_whitespace(char c) {
	return (c == ' ' || c == '\t' || c == '\v' || c == '\f');
}

function b8
char_is_alpha(char c) {
	return char_is_alpha_upper(c) || char_is_alpha_lower(c);
}

function b8
char_is_alpha_upper(char c) {
	return c >= 'A' && c <= 'Z';
}

function b8
char_is_alpha_lower(char c) {
	return c >= 'a' && c <= 'z';
}

function b8
char_is_digit(char c) {
	return (c >= '0' && c <= '9');
}

function b8
char_is_symbol(char c) {
	return (c == '~' || c == '!' || c == '$' || c == '%' || c == '^' ||
		c == '&' || c == '*' || c == '-' || c == '=' || c == '+' ||
		c == '<' || c == '.' || c == '>' || c == '/' || c == '?' ||
		c == '|' || c == '\\' || c == '{' || c == '}' || c == '(' ||
		c == ')' || c == '\\' || c == '[' || c == ']' || c == '#' ||
		c == ',' || c == ';' || c == ':' || c == '@');
}

function b8
char_is_space(char c) {
	return c == ' ' || c == '\r' || c == '\t' || c == '\f' || c == '\v' || c == '\n';
}

function char
char_to_upper(char c) {
	return (c >= 'a' && c <= 'z') ? ('A' + (c - 'a')) : c;
}

function char
char_to_lower(char c) {
	return (c >= 'A' && c <= 'Z') ? ('a' + (c - 'A')) : c;
}

function char
char_to_forward_slash(char c) {
	return (c == '\\' ? '/' : c);
}

// unicode

global u8 utf8_class[32] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,5,
};

function codepoint_t 
utf8_decode(u8* data, u32 max) {

	codepoint_t result = { ~((u32)0), 1 };
	u8 byte = data[0];
	u8 byte_class = utf8_class[byte >> 3];

	switch (byte_class) {
		case 1: {
			result.codepoint = byte;
			break;
		}

		case 2: {
			if (2 <= max) {
				u8 cont_byte = data[1];
				if (utf8_class[cont_byte >> 3] == 0) {
					result.codepoint = (byte & 0x1F) << 6;
					result.codepoint |= (cont_byte & 0x3F);
					result.advance = 2;
				}
			}
			break;
		}

		case 3: {
			if (3 <= max) {
				u8 cont_byte[2] = { data[1], data[2] };
				if (utf8_class[cont_byte[0] >> 3] == 0 &&
					utf8_class[cont_byte[1] >> 3] == 0) {
					result.codepoint = (byte & 0x0F) << 12;
					result.codepoint |= ((cont_byte[0] & 0x3F) << 6);
					result.codepoint |= (cont_byte[1] & 0x3F);
					result.advance = 3;
				}
			}
			break;
		}

		case 4: {
			if (4 <= max) {
				u8 cont_byte[3] = { data[1], data[2], data[3] };
				if (utf8_class[cont_byte[0] >> 3] == 0 &&
					utf8_class[cont_byte[1] >> 3] == 0 &&
					utf8_class[cont_byte[2] >> 3] == 0) {
					result.codepoint = (byte & 0x07) << 18;
					result.codepoint |= ((cont_byte[0] & 0x3F) << 12);
					result.codepoint |= ((cont_byte[1] & 0x3F) << 6);
					result.codepoint |= (cont_byte[2] & 0x3F);
					result.advance = 4;
				}
			}
			break;
		}
	}

	return result;
}

function codepoint_t 
utf16_decode(u16* data, u32 max) {
	codepoint_t result = { ~((u32)0), 1 };
	result.codepoint = data[0];
	result.advance = 1;
	if (1 < max && 0xD800 <= data[0] && data[0] < 0xDC00 && 0xDC00 <= data[1] && data[1] < 0xE000) {
		result.codepoint = ((data[0] - 0xD800) << 10) | (data[1] - 0xDC00) + 0x10000;
		result.advance = 2;
	}
	return result;
}

function u32 
utf8_encode(u8* out, codepoint_t codepoint) {
	u32 advance = 0;
	if (codepoint.codepoint <= 0x7F) {
		out[0] = (u8)codepoint.codepoint;
		advance = 1;
	} else if (codepoint.codepoint <= 0x7FF) {
		out[0] = (0x03 << 6) | ((codepoint.codepoint >> 6) & 0x1F);
		out[1] = 0x80 | (codepoint.codepoint & 0x3F);
		advance = 2;
	} else if (codepoint.codepoint <= 0xFFFF) {
		out[0] = (0x07 << 5) | ((codepoint.codepoint >> 12) & 0x0F);
		out[1] = 0x80 | ((codepoint.codepoint >> 6) & 0x3F);
		out[2] = 0x80 | (codepoint.codepoint & 0x3F);
		advance = 3;
	} else if (codepoint.codepoint <= 0x10FFFF) {
		out[0] = (0x0F << 4) | ((codepoint.codepoint >> 18) & 0x07);
		out[1] = 0x80 | ((codepoint.codepoint >> 12) & 0x3F);
		out[2] = 0x80 | ((codepoint.codepoint >> 6) & 0x3F);
		out[3] = 0x80 | (codepoint.codepoint & 0x3F);
		advance = 4;
	} else {
		out[0] = '?';
		advance = 1;
	}
	return advance;
}

function u32 
utf16_encode(u16* out, codepoint_t codepoint) {
	u32 advance = 1;
	if (codepoint.codepoint == ~((u32)0)) {
		out[0] = (u16)'?';
	} else if (codepoint.codepoint < 0x10000) {
		out[0] = (u16)codepoint.codepoint;
	} else {
		u32 v = codepoint.codepoint - 0x10000;
		out[0] = 0xD800 + (v >> 10);
		out[1] = 0xDC00 + (v & 0x03FF);
		advance = 2;
	}
	return advance;
}

// str functions

function str_t 
str(char* cstr) {
	str_t string;
	string.data = (u8*)cstr;
	string.size = cstr_length(cstr);
	return string;
}

function str_t
str(char* cstr, u32 size) {
	str_t string;
	string.data = (u8*)cstr;
	string.size = size;
	return string;
}

function str_t 
str_copy(arena_t* arena, str_t string) {
	char* data = (char*)arena_alloc(arena, sizeof(char) * string.size);
	memcpy(data, string.data, sizeof(char) * string.size);
	str_t result = str(data, string.size);
	return result;
}

function str_t
str_substr(str_t string, u32 min_pos, u32 max_pos) {
	u32 min = min_pos;
	u32 max = max_pos;
	if (max > string.size) {
		max = string.size;
	}
	if (min > string.size) {
		min = string.size;
	}
	if (min > max) {
		u32 swap = min;
		min = max;
		max = swap;
	}
	string.size = max - min;
	string.data += min;
	return string;
}

function str_t
str_range(u8* first, u8* last) {
	str_t result = { 0 };
	result.data = first;
	result.size = (u32)(last - first);
	return result;
}

function str_t
str_skip(str_t string, u32 min) {
	return str_substr(string, min, string.size);
}

function str_t
str_chop(str_t string, u32 nmax) {
	return str_substr(string, 0, string.size - nmax);
}

function str_t
str_prefix(str_t string, u32 size) {
	return str_substr(string, 0, size);
}

function str_t
str_suffix(str_t string, u32 size) {
	return str_substr(string, string.size - size, string.size);
}

function b8
str_match(str_t a, str_t b, str_match_flags flags = 0) {
	b8 result = 0;

	if (a.size == b.size || flags & str_match_flag_right_side_sloppy) {
		result = 1;
		for (u32 i = 0; i < a.size; i++) {
			b8 match = (a.data[i] == b.data[i]);

			if (flags & str_match_flag_case_insensitive) {
				match |= (char_to_lower(a.data[i]) == char_to_lower(b.data[i]));
			}

			if (flags & str_match_flag_slash_insensitive) {
				match |= (char_to_forward_slash(a.data[i]) == char_to_forward_slash(b.data[i]));
			}

			if (match == 0) {
				result = 0;
				break;
			}
		}
	}
	return result;
}

function u32
str_find_substr(str_t haystack, str_t needle, u32 start_pos = 0, str_match_flags flags = 0) {
	b8 found = 0;
	u32 found_idx = haystack.size;
	for (u32 i = start_pos; i < haystack.size; i++) {
		if (i + needle.size <= haystack.size) {
			str_t substr = str_substr(haystack, i, i + needle.size);
			if (str_match(substr, needle, flags)) {
				found_idx = i;
				found = 1;
				if (!(flags & str_match_flag_find_last)) {
					break;
				}
			}
		}
	}
	return found_idx;
}

function str_t 
str_get_file_name(str_t string) {

	u32 slash_pos = str_find_substr(string, str("/"), 0, str_match_flag_case_insensitive | str_match_flag_find_last);
	if (slash_pos < string.size) {
		string.data += slash_pos + 1;
		string.size -= slash_pos + 1;
	}
	
	u32 period_pos = str_find_substr(string, str("."), 0, str_match_flag_find_last);
	if (period_pos < string.size) {
		string.size = period_pos;
	}

	return string;
}

function str_t 
str_get_file_extension(str_t string) {

	u32 period_pos = str_find_substr(string, str("."), 0, str_match_flag_find_last);
	if (period_pos < string.size) {
		string.data += period_pos + 1;
		string.size -= period_pos + 1;
	}

	return string;
}

function str_t
str_formatv(arena_t* arena, char* fmt, va_list args) {
	str_t result = { 0 };
	va_list args2;
	va_copy(args2, args);
	u32 needed_bytes = vsnprintf(0, 0, fmt, args) + 1;
	result.data = (u8*)arena_alloc(arena, sizeof(u8) * needed_bytes);
	result.size = needed_bytes - 1;
	vsnprintf((char*)result.data, needed_bytes, fmt, args2);
	return result;
}

function str_t
str_format(arena_t* arena, char* fmt, ...) {
	str_t result = { 0 };
	va_list args;
	va_start(args, fmt);
	result = str_formatv(arena, fmt, args);
	va_end(args);
	return result;
}

function void
str_scan(str_t string, char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vsscanf((char*)string.data, fmt, args);
	va_end(args);
}

// str list

function void
str_list_push_node(str_list_t* list, str_node_t* node) {
	dll_push_back(list->first, list->last, node);
	list->count++;
	list->total_size += node->string.size;
}

function void
str_list_push(arena_t* arena, str_list_t* list, str_t string) {
	str_node_t* n = (str_node_t*)arena_alloc(arena, sizeof(str_node_t));
	n->string = string;
	str_list_push_node(list, n);
}

function str_list_t
str_split(arena_t* arena, str_t string, u8* splits, u32 split_count) {
	str_list_t list = { 0 };

	u8* ptr = (u8*)string.data;
	u8* opl = (u8*)string.data + string.size;
	for (; ptr < opl;) {
		u8* first = ptr;
		for (; ptr < opl; ptr += 1) {
			u8 c = *ptr;
			b8 is_split = 0;
			for (u64 i = 0; i < split_count; i += 1) {
				if (splits[i] == c) {
					is_split = 1;
					break;
				}
			}
			if (is_split) {
				break;
			}
		}

		str_t string = str_range(first, ptr);
		if (string.size > 0) {
			str_list_push(arena, &list, string);
		}
		ptr += 1;
	}

	return(list);
}


// str16 functions

function str16_t
str16(u16* data) {
	str16_t result;
	result.data = data;
	u16* i = data;
	for (; *i; i += 1);
	result.size = i - data;
	return result;
}

function str16_t
str16(u16* data, u32 size) {
	str16_t result;
	result.data = data;
	result.size = size;
	return result;
}

function str16_t
str_to_str16(arena_t* arena, str_t string) {

	u32 capacity = string.size * 2;

	u16* string16 = (u16*)arena_alloc(arena, sizeof(u16) * capacity + 1);

	u8* ptr = (u8*)string.data;
	u8* opl = ptr + string.size;

	u32 size = 0;
	codepoint_t codepoint;

	for (; ptr < opl;) {
		codepoint = utf8_decode(ptr, opl - ptr);
		ptr += codepoint.advance;
		size += utf16_encode(string16 + size, codepoint);
	}

	string16[size] = 0;
	//arena_pop(arena, 2 * (cap - size));

	str16_t result;
	result.data = string16;
	result.size = size;
	return result;

}


// math

function f32
radians(f32 degrees) {
	return degrees * 0.0174533f;
}

function f32
degrees(f32 radians) {
	return radians * 57.2958f;
}

function f32
remap(f32 value, f32 from_min, f32 from_max, f32 to_min, f32 to_max) {
	return to_min + (value - from_min) * (to_max - to_min) / (from_max - from_min);
}

function f32
lerp(f32 a, f32 b, f32 t) {
	return (a * (1.0f - t)) + (b * t);
}


// color functions

function color_t 
color(u32 hex, color_format format) {
	color_t result = { 0 };
	result.r = (f32)((hex & 0xff000000) >> 24) / 255.0f;
	result.g = (f32)((hex & 0x00ff0000) >> 16) / 255.0f;
	result.b = (f32)((hex & 0x0000ff00) >> 8) / 255.0f;
	result.a = (f32)((hex & 0x000000ff) >> 0) / 255.0f;
	result.format = format;
	return result;
}

function color_t
color(f32 r, f32 g, f32 b, f32 a, color_format format) {
	color_t col;
	col.r = r;
	col.g = g;
	col.b = b;
	col.a = a;
	col.format = format;
	return col;
}

function color_t 
color_add(color_t a, f32 b) {
	return { clamp_01(a.r + b), clamp_01(a.g + b), clamp_01(a.b + b), clamp_01(a.a + b) };
}

function color_t 
color_add(color_t a, color_t b) {
	return { clamp_01(a.r + b.r), clamp_01(a.g + b.g), clamp_01(a.b + b.b), clamp_01(a.a + b.a)};
}

function color_t 
color_lerp(color_t a, color_t b, f32 t) {
	return { 
		lerp(a.r, b.r, t),
		lerp(a.g, b.g, t),
		lerp(a.b, b.b, t),
		lerp(a.a, b.a, t) 
	};
}

function color_t
color_rgb_to_hsv(color_t rgb) {
	if (rgb.format == color_format_hsv) { return rgb; }

	f32 c_max = max(rgb.r, max(rgb.g, rgb.b));
	f32 c_min = min(rgb.r, min(rgb.g, rgb.b));
	f32 delta = c_max - c_min;
	f32 h = ((delta == 0.0f) ? 0.0f :
		(c_max == rgb.r) ? fmodf((rgb.g - rgb.b) / delta + 6.0f, 6.0f) :
		(c_max == rgb.g) ? (rgb.b - rgb.r) / delta + 2.0f :
		(c_max == rgb.b) ? (rgb.r - rgb.g) / delta + 4.0f :
		0.0f);
	f32 s = (c_max == 0.0f) ? 0.0f : (delta / c_max);
	f32 v = c_max;

	color_t hsv_color;
	hsv_color.h = h / 6.0f;
	hsv_color.s = s;
	hsv_color.v = v;
	hsv_color.a = rgb.a;
	hsv_color.format = color_format_hsv;
	return hsv_color;

}

function color_t
color_hsv_to_rgb(color_t hsv) {

	if (hsv.format == color_format_rgb) { 
		return hsv; 
	}

	f32 h = fmodf(hsv.h * 360.0f, 360.0f);
	f32 s = hsv.s;
	f32 v = hsv.v;

	f32 c = v * s;
	f32 x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
	f32 m = v - c;

	f32 r = 0.0f;
	f32 g = 0.0f;
	f32 b = 0.0f;

	if ((h >= 0.0f && h < 60.0f) || (h >= 360.0f && h < 420.0f)) {
		r = c;
		g = x;
		b = 0.0f;
	} else if (h >= 60.0f && h < 120.0f) {
		r = x;
		g = c;
		b = 0.0f;
	} else if (h >= 120.0f && h < 180.0f) {
		r = 0.0f;
		g = c;
		b = x;
	} else if (h >= 180.0f && h < 240.0f) {
		r = 0.0f;
		g = x;
		b = c;
	} else if (h >= 240.0f && h < 300.0f) {
		r = x;
		g = 0.0f;
		b = c;
	} else if ((h >= 300.0f && h <= 360.0f) || (h >= -60.0f && h <= 0.0f)) {
		r = c;
		g = 0.0f;
		b = x;
	}
	
	color_t rgb_color;
	rgb_color.r = clamp_01(r + m);
	rgb_color.g = clamp_01(g + m);
	rgb_color.b = clamp_01(b + m);
	rgb_color.a = hsv.a;
	rgb_color.format = color_format_rgb;

	return rgb_color;
}


// vec2 

function vec2_t
vec2(f32 v = 0.0f) {
	return { v, v };
}

function vec2_t
vec2(f32 x, f32 y) {
	return { x, y };
}

function vec2_t
vec2_add(vec2_t a, vec2_t b) {
	return { a.x + b.x, a.y + b.y };
}

function vec2_t
vec2_add(vec2_t a, f32 b) {
	return { a.x + b, a.y + b };
}

function vec2_t
vec2_sub(vec2_t a, vec2_t b) {
	return { a.x - b.x, a.y - b.y };
}

function vec2_t
vec2_sub(vec2_t a, f32 b) {
	return { a.x - b, a.y - b };
}

function vec2_t
vec2_mul(vec2_t a, vec2_t b) {
	return { a.x * b.x, a.y * b.y };
}

function vec2_t
vec2_mul(vec2_t a, f32 b) {
	return { a.x * b, a.y * b };
}

function vec2_t
vec2_div(vec2_t a, vec2_t b) {
	return { a.x / b.x, a.y / b.y };
}

function vec2_t
vec2_div(vec2_t a, f32 b) {
	return { a.x / b, a.y / b };
}

function f32
vec2_dot(vec2_t a, vec2_t b) {
	return a.x * b.x + a.y * b.y;
}

function f32
vec2_cross(vec2_t a, vec2_t b) {
	return a.x * b.y - a.y * b.x;
}

function f32
vec2_length(vec2_t a) {
	return sqrtf(a.x * a.x + a.y * a.y);
}

function vec2_t
vec2_normalize(vec2_t a) {
	f32 inv_dem = 1.0f / vec2_length(a);
	return { a.x * inv_dem , a.y * inv_dem };
}

function vec2_t 
vec2_direction(vec2_t a, vec2_t b) {
	return { b.x - a.x, b.y - a.y };
}

function f32
vec2_to_angle(vec2_t a) {
	return atan2f(a.y, a.x);
}

function vec2_t 
vec2_from_angle(f32 a, f32 m) {
	return { m * cosf(a), m * sinf(a) };
}

function vec2_t
vec2_rotate(vec2_t v, f32 a) {
	return { v.x * cosf(a) - v.y * sinf(a), v.x * sinf(a) - v.y * cosf(a) };
}

function vec2_t
vec2_lerp(vec2_t a, vec2_t b, f32 t) {
	return { lerp(a.x, b.x, t), lerp(a.y, b.y, t) };
}

// ivec2

function ivec2_t 
ivec2(i32 v = 0) {
	return { v, v };
}

function ivec2_t 
ivec2(i32 x, i32 y) {
	return { x, y };
}

function b8
ivec2_equals(ivec2_t a, ivec2_t b) {
	return ((a.x == b.x) && (a.y == b.y));
}

// uvec2

function uvec2_t 
uvec2(u32 v = 0) {
	return { v, v };
}

function uvec2_t 
uvec2(u32 x, u32 y) {
	return { x, y };
}

function b8 
uvec2_equals(uvec2_t a, uvec2_t b) {
	return ((a.x == b.x) && (a.y == b.y));
}

// vec3

function vec3_t
vec3(f32 v = 0.0f) {
	return { v, v, v };
}

function vec3_t
vec3(f32 x, f32 y, f32 z) {
	return { x, y, z };
}

function vec3_t
vec3_add(vec3_t a, vec3_t b) {
	return { a.x + b.x, a.y + b.y, a.z + b.z };
}

function vec3_t
vec3_add(vec3_t a, f32 b) {
	return { a.x + b, a.y + b, a.z + b };
}

function vec3_t
vec3_sub(vec3_t a, vec3_t b) {
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}

function vec3_t
vec3_sub(vec3_t a, f32 b) {
	return { a.x - b, a.y - b, a.z - b };
}

function vec3_t
vec3_mul(vec3_t a, vec3_t b) {
	return { a.x * b.x, a.y * b.y, a.z * b.z };
}

function vec3_t
vec3_mul(vec3_t a, f32 b) {
	return { a.x * b, a.y * b, a.z * b };
}

function vec3_t
vec3_div(vec3_t a, vec3_t b) {
	return { a.x / b.x, a.y / b.y, a.z / b.z };
}

function vec3_t
vec3_div(vec3_t a, f32 b) {
	return { a.x / b, a.y / b, a.z / b };
}

function f32
vec3_length(vec3_t a) {
	return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

function vec3_t
vec3_normalize(vec3_t a) {
	f32 d = vec3_length(a);
	if (d < f32_epsilon) {
		return a;
	}
	f32 inv_dem = 1.0f / vec3_length(a);
	return { a.x * inv_dem , a.y * inv_dem, a.z * inv_dem };
}

function f32
vec3_dot(vec3_t a, vec3_t b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

function vec3_t
vec3_cross(vec3_t a, vec3_t b) {
	return { a.y * b.z - a.z * b.y,  a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}

function vec3_t
vec3_lerp(vec3_t a, vec3_t b, f32 t) {
	return { lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t) };
}

function f32
vec3_angle_between(vec3_t a, vec3_t b) {
	return acosf(vec3_dot(a, b) / (vec3_length(a) * vec3_length(b)));
}

function vec3_t
vec3_project(vec3_t a, vec3_t b) {
	return vec3_mul(b, vec3_dot(a, b) / vec3_dot(b, b));
}

function vec3_t
vec3_rotate(vec3_t v, quat_t q) {
	const quat_t tmp0 = { -q.x, -q.y, -q.z, q.w };
	const quat_t qv = { v.x, v.y, v.z, 0.0f };
	const quat_t tmp1 = quat_mul(qv, tmp0);
	const quat_t result = quat_mul(q, tmp1);
	return { result.x, result.y, result.z };
}

function vec3_t
vec3_negate(vec3_t v) {
	return vec3(-v.x, -v.y, -v.z);
}

function vec3_t 
vec3_clamp(vec3_t v, f32 a, f32 b) {
	return { clamp(v.x, a, b), clamp(v.y, a, b), clamp(v.z, a, b) };
}

// vec4

function vec4_t
vec4(f32 v) {
	return { v, v, v, v };
}

function vec4_t
vec4(f32 x, f32 y, f32 z, f32 w) {
	return { x, y, z, w };
}

function vec4_t
vec4_add(vec4_t a, vec4_t b) {
	return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}

function vec4_t
vec4_add(vec4_t a, f32 b) {
	return { a.x + b, a.y + b, a.z + b, a.w + b };
}

function vec4_t
vec4_sub(vec4_t a, vec4_t b) {
	return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}

function vec4_t
vec4_sub(vec4_t a, f32 b) {
	return { a.x - b, a.y - b, a.z - b, a.w - b };
}

function vec4_t
vec4_mul(vec4_t a, vec4_t b) {
	return { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
}

function vec4_t
vec4_mul(vec4_t a, f32 b) {
	return { a.x * b, a.y * b, a.z * b, a.w * b };
}

function vec4_t
vec4_mul(vec4_t a, mat4_t b) {
	return { 0.0f, 0.0f, 0.0f, 0.0f };
}

function vec4_t
vec4_div(vec4_t a, vec4_t b) {
	return { a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w };
}

function vec4_t
vec4_div(vec4_t a, f32 b) {
	return { a.x / b, a.y / b, a.z / b, a.w / b };
}

function f32
vec4_dot(vec4_t a, vec4_t b) {
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

function f32
vec4_cross(vec4_t a, vec4_t b) {
	return 0.0f;
}

function f32
vec4_length(vec4_t a) {
	return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w);
}

function vec4_t
vec4_normalize(vec4_t a) {
	f32 inv_dem = 1.0f / vec4_length(a);
	return { a.x * inv_dem , a.y * inv_dem, a.z * inv_dem, a.w * inv_dem };
}

function vec4_t
vec4_lerp(vec4_t a, vec4_t b, f32 t) {
	return { lerp(a.x, b.y, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t), lerp(a.w, b.w, t) };
}

function f32
vec4_angle_between(vec4_t a, vec4_t b) {
	return acosf(vec4_dot(a, b) / (vec4_length(a) * vec4_length(b)));
}

function vec4_t
vec4_project(vec4_t a, vec4_t b) {
	return vec4_mul(b, vec4_dot(a, b) / vec4_dot(b, b));
}

// rect

function rect_t 
rect(f32 x0, f32 y0, f32 x1, f32 y1) {
	return { x0, y0, x1, y1 };
}

function rect_t 
rect(vec2_t p0, vec2_t p1) {
	return { p0.x, p0.y, p1.x, p1.y };
}

function void 
rect_validate(rect_t& r) {
	if (r.x0 > r.x1) {
		f32 temp = r.x0;
		r.x0 = r.x1;
		r.x1 = temp;
	}
	if (r.y0 > r.y1) {
		f32 temp = r.y0;
		r.y0 = r.y1;
		r.y1 = temp;
	}
}

function b8 
rect_contains(rect_t r, vec2_t p) {
	return (p.x > r.x0 && p.x < r.x1 && p.y > r.y0 && p.y < r.y1);
}

function b8
rect_contains(rect_t a, rect_t b) {
	return (a.x0 <= b.x0 && a.x1 >= b.x1 && a.y0 <= b.y0 && a.y1 >= b.y1);
}

function rect_t 
rect_intersection(rect_t a, rect_t b) {
	return { max(a.x0, b.x0), max(a.y0, b.y0), min(a.x1, b.x1), min(a.y1, b.y1) };
}

function f32 
rect_width(rect_t r) {
	return fabsf(r.x1 - r.x0);
}

function f32 
rect_height(rect_t r) {
	return fabsf(r.y1 - r.y0);
}

function vec2_t 
rect_center(rect_t r) {
	return { (r.x0 + r.x1) * 0.5f, (r.y0 + r.y1) * 0.5f };
}

function rect_t
rect_grow(rect_t r, f32 a) {
	return { r.x0 - a, r.y0 - a, r.x1 + a, r.y1 + a };
}

function rect_t 
rect_grow(rect_t r, vec2_t a) {
	return { r.x0 - a.x, r.y0 - a.y, r.x1 + a.x, r.y1 + a.y };
}

function rect_t 
rect_shrink(rect_t r, f32 a) {
	return { r.x0 + a, r.y0 + a, r.x1 - a, r.y1 - a };
}

function rect_t 
rect_shrink(rect_t r, vec2_t a) {
	return { r.x0 + a.x, r.y0 + a.y, r.x1 - a.x, r.y1 - a.y };
}

function rect_t 
rect_translate(rect_t r, f32 a) {
	return { r.x0 + a, r.y0 + a, r.x1 + a, r.y1 + a };
}

function rect_t 
rect_translate(rect_t r, vec2_t a) {
	return { r.x0 + a.x, r.y0 + a.y, r.x1 + a.x, r.y1 + a.y };
}

function rect_t 
rect_bbox(vec2_t* points, u32 count) {
	rect_t result = {f32_max, f32_max, f32_min, f32_min};

	for (i32 i = 0; i < count; i++) {
		vec2_t p = points[i];

		if (p.x < result.x0) { result.x0 = p.x; }
		if (p.y < result.y0) { result.y0 = p.y; }
		if (p.x > result.x1) { result.x1 = p.x; }
		if (p.y > result.y1) { result.y1 = p.y; }
	}
	
	return result;
}


// quat

function quat_t
quat_create(f32 x, f32 y, f32 z, f32 w) {
	return { x, y, z, w };
}

function quat_t
quat_axis_angle(vec3_t axis, f32 angle) {
	quat_t result = { 0.0f };
	f32 ha = angle * 0.5f;
	f32 sa = sinf(ha);
	result.x = axis.x * sa;
	result.y = axis.y * sa;
	result.z = axis.z * sa;
	result.w = cosf(ha);
	return result;
}

function quat_t
quat_from_euler_angle(vec3_t euler) {
	quat_t result = { 0.0f };

	f32 cy = cos(euler.z * 0.5f);
	f32 sy = sin(euler.z * 0.5f);
	f32 cp = cos(euler.y * 0.5f);
	f32 sp = sin(euler.y * 0.5f);
	f32 cr = cos(euler.x * 0.5f);
	f32 sr = sin(euler.x * 0.5f);

	result.w = cr * cp * cy + sr * sp * sy;
	result.x = sr * cp * cy - cr * sp * sy;
	result.y = cr * sp * cy + sr * cp * sy;
	result.z = cr * cp * sy - sr * sp * cy;

	return result;
}

function vec3_t
quat_to_euler_angle(quat_t quat) {

	vec3_t result;

	const f32 xx = quat.x;
	const f32 yy = quat.y;
	const f32 zz = quat.z;
	const f32 ww = quat.w;
	const f32 xsq = xx * xx;
	const f32 ysq = yy * yy;
	const f32 zsq = zz * zz;

	return vec3(
		atan2f(2.0f * (xx * ww - yy * zz), 1.0f - 2.0f * (xsq + zsq)),
		atan2f(2.0f * (yy * ww + xx * zz), 1.0f - 2.0f * (ysq + zsq)),
		asinf(2.0f * (xx * yy + zz * ww))
	);
}

function vec3_t
quat_to_dir(quat_t q) {
	vec3_t dir;
	dir.x = -(2.0f * (q.x * q.z + q.w * q.y));
	dir.y = -(2.0f * (q.y * q.z - q.w * q.x));
	dir.z = -(1.0f - 2.0f * (q.x * q.x + q.y * q.y));
	return dir;
}

function quat_t
quat_add(quat_t a, quat_t b) {
	return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}

function quat_t
quat_sub(quat_t a, quat_t b) {
	return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}

function quat_t
quat_mul(quat_t a, quat_t b) {
	quat_t r;

	const f32 ax = a.x;
	const f32 ay = a.y;
	const f32 az = a.z;
	const f32 aw = a.w;

	const f32 bx = b.x;
	const f32 by = b.y;
	const f32 bz = b.z;
	const f32 bw = b.w;

	r.x = aw * bx + ax * bw + ay * bz - az * by;
	r.y = aw * by - ax * bz + ay * bw + az * bx;
	r.z = aw * bz + ax * by - ay * bx + az * bw;
	r.w = aw * bw - ax * bx - ay * by - az * bz;

	return r;
}

function quat_t
quat_mul(quat_t a, f32 b) {
	return{ a.x * b, a.y * b, a.z * b, a.w * b };
}

function quat_t
quat_div(quat_t a, f32 b) {
	return { a.x / b, a.y / b, a.z / b, a.w / b };
}

function f32
quat_dot(quat_t a, quat_t b) {
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

function f32
quat_length(quat_t q) {
	return sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

function quat_t
quat_normalize(quat_t q) {
	const f32 norm = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	const f32 inv_norm = powf(norm, -0.5f);
	return { q.x * inv_norm , q.y * inv_norm, q.z * inv_norm, q.w * inv_norm };
}

function quat_t
quat_negate(quat_t a) {
	return { -a.x, -a.y, -a.z, -a.w };
}

function quat_t
quat_lerp(quat_t a, quat_t b, f32 t) {
	return { lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t), lerp(a.w, b.w, t) };
}

function quat_t
quat_slerp(quat_t a, quat_t b, f32 t) {

	quat_t c = b;

	f32 cos_theta = quat_dot(a, b);

	if (cos_theta < 0.0f) {
		c = quat_negate(b);
		cos_theta = -cos_theta;
	}

	if (cos_theta > (1.0f - 1.19209e-07)) {
		return quat_lerp(a, c, t);
	} else {
		f32 angle = acosf(cos_theta);
		return quat_div(quat_add(quat_mul(a, sinf((1.0f - t) * angle)), quat_mul(b, sinf(t * angle))), sinf(angle));
	}


}


// mat4

function mat4_t
mat4_identity() {
	mat4_t r = { {
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	} };
	return r;
}

function b8
mat4_equals(mat4_t mat1, mat4_t mat2, f32 tolerance = 0.1f) {

	for (i32 i = 0; i < 4; i++) {
		for (i32 j = 0; j < 4; j++) {
			if (fabs(mat1.data[i][j] - mat2.data[i][j]) > tolerance) {
				return false;
			}
		}
	}

	return true;
}

function mat4_t
mat4_transpose(mat4_t m) {

	mat4_t r = { {
		{m.data[0][0], m.data[0][1], m.data[0][2], m.data[0][3]},
		{m.data[1][0], m.data[1][1], m.data[1][2], m.data[1][3]},
		{m.data[2][0], m.data[2][1], m.data[2][2], m.data[2][3]},
		{m.data[3][0], m.data[3][1], m.data[3][2], m.data[3][3]}
	} };

	return r;

}

function mat4_t
mat4_from_quat(quat_t q) {
	mat4_t result = mat4_identity();

	const f32 qx = q.x;
	const f32 qy = q.y;
	const f32 qz = q.z;
	const f32 qw = q.w;

	const f32 x2 = qx + qx;
	const f32 y2 = qy + qy;
	const f32 z2 = qz + qz;
	const f32 x2x = x2 * qx;
	const f32 x2y = x2 * qy;
	const f32 x2z = x2 * qz;
	const f32 x2w = x2 * qw;
	const f32 y2y = y2 * qy;
	const f32 y2z = y2 * qz;
	const f32 y2w = y2 * qw;
	const f32 z2z = z2 * qz;
	const f32 z2w = z2 * qw;

	result.data[0][0] = 1.0f - (y2y + z2z);
	result.data[0][1] = x2y + z2w;
	result.data[0][2] = x2z - y2w;
	result.data[0][3] = 0.0f;

	result.data[1][0] = x2y - z2w;
	result.data[1][1] = 1.0f - (x2x + z2z);
	result.data[1][2] = y2z + x2w;
	result.data[1][3] = 0.0f;

	result.data[2][0] = x2z + y2w;
	result.data[2][1] = y2z - x2w;
	result.data[2][2] = 1.0f - (x2x + y2y);
	result.data[2][3] = 0.0f;

	result.data[3][0] = 0.0f;
	result.data[3][1] = 0.0f;
	result.data[3][2] = 0.0f;
	result.data[3][3] = 1.0f;

	return result;
}

function mat4_t
mat4_translate(vec3_t translate) {

	mat4_t r = { {
		{ 1.0f, 0.0f, 0.0f, translate.x },
		{ 0.0f, 1.0f, 0.0f, translate.y },
		{ 0.0f, 0.0f, 1.0f, translate.z },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	} };

	return r;
}

function mat4_t
mat4_translate(mat4_t m, vec3_t t) {
	mat4_t result = m;
	result.data[0][3] = t.x;
	result.data[1][3] = t.y;
	result.data[2][3] = t.z;
	return result;
}

function mat4_t
mat4_scale(vec3_t scale) {
	mat4_t r = { {
		{ scale.x, 0.0f, 0.0f, 0.0f },
		{ 0.0f, scale.y, 0.0f, 0.0f },
		{ 0.0f, 0.0f, scale.z, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	} };
	return r;
}

function mat4_t
mat4_mul(mat4_t a, mat4_t b) {

	mat4_t r = { 0 };

	for (i32 j = 0; j < 4; j++) {
		for (i32 i = 0; i < 4; i++) {
			r.data[j][i] = (a.data[j][0] * b.data[0][i] +
				a.data[j][1] * b.data[1][i] +
				a.data[j][2] * b.data[2][i] +
				a.data[j][3] * b.data[3][i]);
		}
	}

	return r;
}

function mat4_t
mat4_mul(mat4_t a, vec4_t b) {

}

function mat4_t
mat4_inverse(mat4_t m) {

	mat4_t r;

	r.data[0][0] = m.data[1][1] * m.data[2][2] * m.data[3][3] -
		m.data[1][1] * m.data[2][3] * m.data[3][2] -
		m.data[2][1] * m.data[1][2] * m.data[3][3] +
		m.data[2][1] * m.data[1][3] * m.data[3][2] +
		m.data[3][1] * m.data[1][2] * m.data[2][3] -
		m.data[3][1] * m.data[1][3] * m.data[2][2];

	r.data[1][0] = -m.data[1][0] * m.data[2][2] * m.data[3][3] +
		m.data[1][0] * m.data[2][3] * m.data[3][2] +
		m.data[2][0] * m.data[1][2] * m.data[3][3] -
		m.data[2][0] * m.data[1][3] * m.data[3][2] -
		m.data[3][0] * m.data[1][2] * m.data[2][3] +
		m.data[3][0] * m.data[1][3] * m.data[2][2];

	r.data[2][0] = m.data[1][0] * m.data[2][1] * m.data[3][3] -
		m.data[1][0] * m.data[2][3] * m.data[3][1] -
		m.data[2][0] * m.data[1][1] * m.data[3][3] +
		m.data[2][0] * m.data[1][3] * m.data[3][1] +
		m.data[3][0] * m.data[1][1] * m.data[2][3] -
		m.data[3][0] * m.data[1][3] * m.data[2][1];

	r.data[3][0] = -m.data[1][0] * m.data[2][1] * m.data[3][2] +
		m.data[1][0] * m.data[2][2] * m.data[3][1] +
		m.data[2][0] * m.data[1][1] * m.data[3][2] -
		m.data[2][0] * m.data[1][2] * m.data[3][1] -
		m.data[3][0] * m.data[1][1] * m.data[2][2] +
		m.data[3][0] * m.data[1][2] * m.data[2][1];

	r.data[0][1] = -m.data[0][1] * m.data[2][2] * m.data[3][3] +
		m.data[0][1] * m.data[2][3] * m.data[3][2] +
		m.data[2][1] * m.data[0][2] * m.data[3][3] -
		m.data[2][1] * m.data[0][3] * m.data[3][2] -
		m.data[3][1] * m.data[0][2] * m.data[2][3] +
		m.data[3][1] * m.data[0][3] * m.data[2][2];

	r.data[1][1] = m.data[0][0] * m.data[2][2] * m.data[3][3] -
		m.data[0][0] * m.data[2][3] * m.data[3][2] -
		m.data[2][0] * m.data[0][2] * m.data[3][3] +
		m.data[2][0] * m.data[0][3] * m.data[3][2] +
		m.data[3][0] * m.data[0][2] * m.data[2][3] -
		m.data[3][0] * m.data[0][3] * m.data[2][2];

	r.data[2][1] = -m.data[0][0] * m.data[2][1] * m.data[3][3] +
		m.data[0][0] * m.data[2][3] * m.data[3][1] +
		m.data[2][0] * m.data[0][1] * m.data[3][3] -
		m.data[2][0] * m.data[0][3] * m.data[3][1] -
		m.data[3][0] * m.data[0][1] * m.data[2][3] +
		m.data[3][0] * m.data[0][3] * m.data[2][1];

	r.data[3][1] = m.data[0][0] * m.data[2][1] * m.data[3][2] -
		m.data[0][0] * m.data[2][2] * m.data[3][1] -
		m.data[2][0] * m.data[0][1] * m.data[3][2] +
		m.data[2][0] * m.data[0][2] * m.data[3][1] +
		m.data[3][0] * m.data[0][1] * m.data[2][2] -
		m.data[3][0] * m.data[0][2] * m.data[2][1];

	r.data[0][2] = m.data[0][1] * m.data[1][2] * m.data[3][3] -
		m.data[0][1] * m.data[1][3] * m.data[3][2] -
		m.data[1][1] * m.data[0][2] * m.data[3][3] +
		m.data[1][1] * m.data[0][3] * m.data[3][2] +
		m.data[3][1] * m.data[0][2] * m.data[1][3] -
		m.data[3][1] * m.data[0][3] * m.data[1][2];

	r.data[1][2] = -m.data[0][0] * m.data[1][2] * m.data[3][3] +
		m.data[0][0] * m.data[1][3] * m.data[3][2] +
		m.data[1][0] * m.data[0][2] * m.data[3][3] -
		m.data[1][0] * m.data[0][3] * m.data[3][2] -
		m.data[3][0] * m.data[0][2] * m.data[1][3] +
		m.data[3][0] * m.data[0][3] * m.data[1][2];

	r.data[2][2] = m.data[0][0] * m.data[1][1] * m.data[3][3] -
		m.data[0][0] * m.data[1][3] * m.data[3][1] -
		m.data[1][0] * m.data[0][1] * m.data[3][3] +
		m.data[1][0] * m.data[0][3] * m.data[3][1] +
		m.data[3][0] * m.data[0][1] * m.data[1][3] -
		m.data[3][0] * m.data[0][3] * m.data[1][1];

	r.data[3][2] = -m.data[0][0] * m.data[1][1] * m.data[3][2] +
		m.data[0][0] * m.data[1][2] * m.data[3][1] +
		m.data[1][0] * m.data[0][1] * m.data[3][2] -
		m.data[1][0] * m.data[0][2] * m.data[3][1] -
		m.data[3][0] * m.data[0][1] * m.data[1][2] +
		m.data[3][0] * m.data[0][2] * m.data[1][1];

	r.data[0][3] = -m.data[0][1] * m.data[1][2] * m.data[2][3] +
		m.data[0][1] * m.data[1][3] * m.data[2][2] +
		m.data[1][1] * m.data[0][2] * m.data[2][3] -
		m.data[1][1] * m.data[0][3] * m.data[2][2] -
		m.data[2][1] * m.data[0][2] * m.data[1][3] +
		m.data[2][1] * m.data[0][3] * m.data[1][2];

	r.data[1][3] = m.data[0][0] * m.data[1][2] * m.data[2][3] -
		m.data[0][0] * m.data[1][3] * m.data[2][2] -
		m.data[1][0] * m.data[0][2] * m.data[2][3] +
		m.data[1][0] * m.data[0][3] * m.data[2][2] +
		m.data[2][0] * m.data[0][2] * m.data[1][3] -
		m.data[2][0] * m.data[0][3] * m.data[1][2];

	r.data[2][3] = -m.data[0][0] * m.data[1][1] * m.data[2][3] +
		m.data[0][0] * m.data[1][3] * m.data[2][1] +
		m.data[1][0] * m.data[0][1] * m.data[2][3] -
		m.data[1][0] * m.data[0][3] * m.data[2][1] -
		m.data[2][0] * m.data[0][1] * m.data[1][3] +
		m.data[2][0] * m.data[0][3] * m.data[1][1];

	r.data[3][3] = m.data[0][0] * m.data[1][1] * m.data[2][2] -
		m.data[0][0] * m.data[1][2] * m.data[2][1] -
		m.data[1][0] * m.data[0][1] * m.data[2][2] +
		m.data[1][0] * m.data[0][2] * m.data[2][1] +
		m.data[2][0] * m.data[0][1] * m.data[1][2] -
		m.data[2][0] * m.data[0][2] * m.data[1][1];

	f32 det = m.data[0][0] * r.data[0][0] +
		m.data[0][1] * r.data[1][0] +
		m.data[0][2] * r.data[2][0] +
		m.data[0][3] * r.data[3][0];

	det = 1.0f / det;

	for (i32 i = 0; i < 4; i++) {
		for (i32 j = 0; j < 4; j++) {
			r.data[i][j] = r.data[i][j] * det;
		}
	}

	return r;
}

function mat4_t
mat4_orthographic(f32 left, f32 right, f32 top, f32 bottom, f32 z_near, f32 z_far) {
	mat4_t r = { {
		{2.0f / (right - left), 0.0f                 , 0.0f                   , (left + right) / (left - right)},
		{0.0f                 , 2.0f / (top - bottom), 0.0f                   , (bottom + top) / (bottom - top)},
		{0.0f                 , 0.0f                 , 1.0f / (z_near - z_far), (z_near) / (z_near - z_far)},
		{0.0f                 , 0.0f                 , 0.0f                   , 1.0f}
	} };
	return r;
}

function mat4_t
mat4_perspective(f32 fov, f32 ar, f32 n, f32 f) {

	f32 cotangent = 1.0f / tanf(radians(fov) / 2.0f);

	mat4_t r = { {
		{cotangent / ar, 0.0f,      0.0f,                     0.0f},
		{0.0f,           cotangent, 0.0f,                     0.0f},
		{0.0f,           0.0f,      (n + f) / (n - f),       (2.0f * n * f) / (n - f)},
		{0.0f,           0.0f,      -1.0f,                    0.0f}
	} };

	return r;
}

function mat4_t
mat4_lookat(vec3_t from, vec3_t to, vec3_t up) {

	vec3_t f = vec3_normalize(vec3_sub(to, from));
	vec3_t r = vec3_normalize(vec3_cross(f, up));
	vec3_t u = vec3_cross(r, f);

	mat4_t v = { {
		{ r.x,  r.y,  r.z, -vec3_dot(from, r)},
		{ u.x,  u.y,  u.z, -vec3_dot(from, u)},
		{-f.x, -f.y, -f.z,  vec3_dot(from, f)},
		{ 0.0f,       0.0,        0.0f,      1.0f}
	} };

	return v;
}


// misc functions

function vec3_t
barycentric(vec2_t p, vec2_t a, vec2_t b, vec2_t c) {

	vec2_t v0 = vec2_sub(b, a);
	vec2_t v1 = vec2_sub(c, a);
	vec2_t v2 = vec2_sub(p, a);

	//f32 d00 = vec2_dot(v0, v0);
	//f32 d01 = vec2_dot(v0, v1);
	//f32 d11 = vec2_dot(v1, v1);
	//f32 d20 = vec2_dot(v2, v0);
	//f32 d21 = vec2_dot(v2, v1);

	//f32 denom = d00 * d11 - d01 * d01;

	//f32 v = (d11 * d20 - d01 * d21) / denom;
	//f32 w = (d00 * d21 - d01 * d20) / denom;
	//f32 u = 1.0f - v - w;

	f32 denom = v0.x * v1.y - v1.x * v0.y;

	f32 v = (v2.x * v1.y - v1.x * v2.y) / denom;
	f32 w = (v0.x * v2.y - v2.x * v0.y) / denom;
	f32 u = 1.0f - v - w;

	return vec3(u, v, w);

}

function b8
tri_contains(vec2_t a, vec2_t b, vec2_t c, vec2_t p) {

	vec2_t v0 = vec2_sub(b, a);
	vec2_t v1 = vec2_sub(c, a);
	vec2_t v2 = vec2_sub(p, a);

	f32 d00 = vec2_dot(v0, v0);
	f32 d01 = vec2_dot(v0, v1);
	f32 d11 = vec2_dot(v1, v1);
	f32 d20 = vec2_dot(v2, v0);
	f32 d21 = vec2_dot(v2, v1);

	f32 denom = d00 * d11 - d01 * d01;

	f32 u = (d11 * d20 - d01 * d21) / denom;
	f32 v = (d00 * d21 - d01 * d20) / denom;

	return (u >= 0) && (v >= 0) && (u + v <= 1);
}


#endif // BASE_CPP