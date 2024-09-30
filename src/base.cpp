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

// vec3

function vec3_t
vec3(f32 a = 0.0f) {
	return {a, a, a };
}

function vec3_t
vec3(f32 x, f32 y, f32 z) {
	return { x, y, z };
}

function vec3_t 
vec3_clamp(vec3_t v, f32 a, f32 b) {
	return { clamp(v.x, a, b), clamp(v.y, a, b), clamp(v.z, a, b) };
}

// vec4

function vec4_t 
vec4(f32 v = 0.0f) {
	return { v, v, v, v };
}

function vec4_t 
vec4(f32 x, f32 y, f32 z, f32 w) {
	return { x, y, z, w };
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