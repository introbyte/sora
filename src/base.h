// base.h

#ifndef BASE_H
#define BASE_H

// includes

#include <cstdio>
#include <cmath>

// defines

#define function static
#define global static
#define persist static

#define bytes(n)      (n)
#define kilobytes(n)  (n << 10)
#define megabytes(n)  (n << 20)
#define gigabytes(n)  (((u64)n) << 30)
#define terabytes(n)  (((u64)n) << 40)

#define u8_max  (0xFF)
#define u8_min  (0)
#define u16_max (0xFFFF)
#define u16_min (0)
#define u32_max (0xFFFFFFFF)
#define u32_min (0)
#define u64_max (0xFFFFFFFFFFFFFFFF)
#define u64_min (0)
#define i8_max (0x7F)
#define i8_min (-1 - 0x7F)
#define i16_max (0x7FFF)
#define i16_min (-1 - 0x7FFF)
#define i32_max (0x7FFFFFFF)
#define i32_min (-1 - 0x7FFFFFFF)
#define i64_max (0x7FFFFFFFFFFFFFFF)
#define i64_min (-1 - 0x7FFFFFFFFFFFFFFF)

#define f32_sign (0x80000000)
#define f32_exp (0x7F800000)
#define f32_mantissa (0x7FFFFF)
#define f32_max (3.402823e+38f)
#define f32_min (-3.402823e+38f)
#define f32_smallest_positive (1.1754943508e-38)
#define f32_epsilon (5.96046448e-8)

#define f32_pi (3.141592653597f)
#define f64_pi (3.141592653597)

#define min(a, b) (((a)<(b)) ? (a) : (b))
#define max(a, b) (((a)>(b)) ? (a) : (b))
#define clamp(x, a, b) (((a)>(x))?(a):((b)<(x))?(b):(x))
#define clamp_01(x) (((0)>(x))?(0):((1)<(x))?(1):(x))

#define array_count(a) (sizeof(a) / sizeof((a)[0]))

#define arena_commit_size kilobytes(4)
#define arena_decommit_size megabytes(4)

#define stack_push_n(f, n, next) ((n)->next = (f), (f) = (n))
#define stack_pop_n(f, next) (((f) == 0) ? 0 : ((f) = (f)->next))

#define stack_push(f, n) stack_push_n(f, n, next)
#define stack_pop(f) stack_pop_n(f, next)

#define dll_push_back_np(f, l, n, next, prev)\
((f)==0?\
((f)=(l)=(n),(n)->next=(n)->prev=0):\
((n)->prev=(l),(l)->next=(n),(l)=(n),(n)->next=0))

#define dll_push_front_np(f, l, n, next, prev)\
((l)==0?\
((l)=(f)=(n),(n)->prev=(n)->next=0):\
((n)->next=(f),(f)->prev=(n),(f)=(n),(n)->prev=0))

#define dll_remove_np(f, l, n, next, prev)\
((f)==(n)?\
((f)==(l)?\
((f)=(l)=(0)):\
((f)=(f)->next,(f)->prev=0)):\
(l)==(n)?\
((l)=(l)->prev,(l)->next=0):\
((n)->next->prev=(n)->prev,\
(n)->prev->next=(n)->next))

#define dll_push_back(f, l, n) dll_push_back_np(f, l, n, next, prev)
#define dll_push_front(f, l, n) dll_push_front_np(f, l, n, next, prev)
#define dll_remove(f, l, n) dll_remove_np(f, l, n, next, prev)

#define member_from_offset(type, ptr, off) (type)((((u8 *)ptr)+(off)))

// typedefs

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef float f32;
typedef double f64;

typedef const char* cstr;

typedef i8 b8;

// enums

typedef u32 str_match_flags;
enum : u32 {
	str_match_flag_case_insensitive = (1 << 0),
	str_match_flag_right_side_sloppy = (1 << 1),
	str_match_flag_slash_insensitive = (1 << 2),
	str_match_flag_find_last = (1 << 3),
	str_match_flag_keep_empties = (1 << 4),
};

enum color_format {
	color_format_rgb,
	color_format_hsv,
};


// structs

struct str_t {
	u8* data;
	u32 size;
};

struct str16_t {
	u16* data;
	u32 size;
};

struct str32_t {
	u32* data;
	u32 size;
};

struct codepoint_t {
	u32 codepoint;
	u32 advance;
};

struct vec2_t {
	f32 x, y;
};

struct vec3_t {
	f32 x, y, z;
};

struct vec4_t {
	f32 x, y, z, w;
};

struct rect_t {
	f32 x0, y0;
	f32 x1, y1;
};

struct color_t {
	union {
		struct {
			union {
				struct { f32 r, g, b; };
				struct { f32 h, s, v; };
			};
			f32 a;
		};
		vec4_t vec;
	};
	color_format format;
};

struct arena_t {
	u32 pos;
	u32 commit_pos;
	u32 align;
	u32 size;
};

// functions

// arenas
function arena_t* arena_create(u32);
function void arena_release(arena_t*);
function void* arena_alloc(arena_t*, u32);
function void* arena_calloc(arena_t*, u32);
function void arena_clear(arena_t*);

// cstr
function u32 cstr_length(cstr cstr);
function b8 cstr_equals(cstr cstr1, cstr cstr2);
function b8 char_is_whitespace(char);
function b8 char_is_alpha(char);
function b8 char_is_alpha_upper(char);
function b8 char_is_alpha_lower(char);
function b8 char_is_digit(char);
function b8 char_is_symbol(char);
function b8 char_is_space(char);
function char char_to_upper(char);
function char char_to_lower(char);
function char char_to_forward_slash(char);

// unicode
function codepoint_t utf8_decode(u8*, u32);
function codepoint_t utf16_decode(u16*, u32);
function u8 utf8_encode(codepoint_t);
function u16 utf16_encode(codepoint_t);

// str
function str_t str(char*);
function str_t str(char*, u32);
function str_t str_substr(str_t, u32, u32);
function str_t str_range(u8*, u8*);
function str_t str_skip(str_t, u32);
function str_t str_chop(str_t, u32);
function str_t str_prefix(str_t, u32);
function str_t str_suffix(str_t, u32);
function b8 str_match(str_t, str_t, str_match_flags);
function u32 str_find_substr(str_t, str_t, u32, str_match_flags);
function str_t str_get_file_name(str_t);
function str_t str_get_file_extension(str_t);
function str_t str_formatv(arena_t*, char*, va_list);
function str_t str_format(arena_t*, char*, ...);

// str16
function str16_t str16(u16*);
function str16_t str16(u16*, u32);
function str16_t str_to_st16(str_t);

// math
function f32 radians(f32);
function f32 degrees(f32);
function f32 remap(f32, f32, f32, f32, f32);
function f32 lerp(f32, f32, f32);

// color
function color_t color(u32, color_format = color_format_rgb);
function color_t color(f32, f32, f32, f32, color_format = color_format_rgb);
function color_t color_add(color_t, f32);
function color_t color_add(color_t, color_t);
function color_t color_lerp(color_t, color_t, f32);
function color_t color_rgb_to_hsv(color_t);
function color_t color_hsv_to_rgb(color_t);

// vec2 
function vec2_t vec2(f32);
function vec2_t vec2(f32, f32);
function vec2_t vec2_add(vec2_t, f32);
function vec2_t vec2_add(vec2_t, vec2_t);
function vec2_t vec2_sub(vec2_t, f32);
function vec2_t vec2_sub(vec2_t, vec2_t);
function vec2_t vec2_mul(vec2_t, f32);
function vec2_t vec2_mul(vec2_t, vec2_t);
function vec2_t vec2_div(vec2_t, f32);
function vec2_t vec2_div(vec2_t, vec2_t);
function f32 vec2_dot(vec2_t, vec2_t);
function f32 vec2_cross(vec2_t, vec2_t);
function f32 vec2_length(vec2_t);
function vec2_t vec2_normalize(vec2_t);
function vec2_t vec2_direction(vec2_t, vec2_t);
function f32 vec2_to_angle(vec2_t);
function vec2_t vec2_from_angle(f32, f32 = 1);
function vec2_t vec2_rotate(vec2_t, f32);
function vec2_t vec2_lerp(vec2_t, vec2_t, f32);

// vec3

function vec3_t vec3(f32);
function vec3_t vec3(f32, f32, f32);
function vec3_t vec3_clamp(vec3_t, f32, f32);

// vec4

function vec4_t vec4(f32);
function vec4_t vec4(f32, f32, f32, f32);

// rect
function rect_t rect(f32, f32, f32, f32);
function rect_t rect(vec2_t, vec2_t);
function void rect_validate(rect_t&);
function b8 rect_contains(rect_t, vec2_t);
function b8 rect_contains(rect_t, rect_t);
function rect_t rect_intersection(rect_t, rect_t);
function f32 rect_width(rect_t);
function f32 rect_height(rect_t);
function vec2_t rect_center(rect_t);
function rect_t rect_grow(rect_t, f32);
function rect_t rect_grow(rect_t, vec2_t);
function rect_t rect_shrink(rect_t, f32);
function rect_t rect_shrink(rect_t, vec2_t);
function rect_t rect_translate(rect_t, f32);
function rect_t rect_translate(rect_t, vec2_t);
function rect_t rect_bbox(vec2_t*, u32);

// misc
function vec3_t barycentric(vec2_t, vec2_t, vec2_t, vec2_t);
function b8 tri_contains(vec2_t, vec2_t, vec2_t, vec2_t);

#endif // BASE_H