// base.h

#ifndef BASE_H
#define BASE_H

// todo:
// 
// [~] - add simd support.
//   [x] - vectors.
//   [x] - matrices.
//   [x] - quaternions.
//   [ ] - color.
// [ ] - add more math structs
//   [ ] - mat2.
//   [ ] - mat3.
//

// this project includes modified code from HandmadeMath.
// HandmadeMath was released under the CC0 1.0 universal 
// license, allowing unrestricted use.
// original source: https://github.com/HandmadeMath/HandmadeMath

// includes

#include <cstdio> // printf
#include <cmath> // math functions
#include <pmmintrin.h> // simd functions

// defines

#define BASE_USE_SIMD 1

#define function static
#define global static
#define persist static
#define inlnfunc inline static

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

// flags
#define flag_set(flags, mask) ((flags) |= (mask))
#define flag_remove(flags, mask) ((flags) &= ~(mask))
#define flag_has(flags, mask) ((flags) & (mask) != 0)

// stack
#define stack_push_n(f, n, next) ((n)->next = (f), (f) = (n))
#define stack_pop_n(f, next) (((f) == 0) ? 0 : ((f) = (f)->next))

#define stack_push(f, n) stack_push_n(f, n, next)
#define stack_pop(f) stack_pop_n(f, next)

// queue
#define queue_push_n(f, l, n, next) (((f) == 0) ? ((f)=(l)=(n), ((n)->next = 0)) : ((l)->next=(n),(l)=(n), ((n)->next = 0)))
#define queue_push_front_n(f, l, n, next) (((f)==0) ? ((f)=(l)=(n),((n)->next = 0)) : ((n)->next=(f),(f)=(n)))
#define queue_pop_n(f, l, next) ((f)==(l) ? (((f) = 0),((l) = 0)) : (f)=(f)->next))

#define queue_push(f, l, n) queue_push_n(f, l, n, next)
#define queue_push_front(f, l, n) queue_push_front_n(f, l, n, next)
#define queue_pop(f, l) queue_pop_n(f, l, next)

// doubly linked list
#define dll_insert_np(f,l,p,n,next,prev) (((f) == 0 || (f) == 0) ? ((f) = (l) = (n), (((n)->next) = 0), (((n)->prev) = 0)) : ((p) == 0 || (p) == 0) ? ((n)->next = (f), (f)->prev = (n), (f) = (n), (((n)->prev) = 0)) : ((p) == (l)) ? ((l)->next = (n), (n)->prev = (l), (l) = (n), (((n)->next) = 0)) : (((!((p) == 0 || (p) == 0) && (((p)->next) == 0 || ((p)->next) == 0)) ? (0) : ((p)->next->prev = (n))), ((n)->next = (p)->next), ((p)->next = (n)), ((n)->prev = (p))))
#define dll_push_back_np(f,l,n,next,prev) (((f) == 0 || (f) == 0) ? ((f) = (l) = (n), (((n)->next) = 0), (((n)->prev) = 0)) : ((l) == 0 || (l) == 0) ? ((n)->next = (f), (f)->prev = (n), (f) = (n), (((n)->prev) = 0)) : ((l) == (l)) ? ((l)->next = (n), (n)->prev = (l), (l) = (n), (((n)->next) = 0)) : (((!((l) == 0 || (l) == 0) && (((l)->next) == 0 || ((l)->next) == 0)) ? (0) : ((l)->next->prev = (n))), ((n)->next = (l)->next), ((l)->next = (n)), ((n)->prev = (l))))
#define dll_push_front_np(f,l,n,next,prev) (((l) == 0 || (l) == 0) ? ((l) = (f) = (n), (((n)->prev) = 0), (((n)->next) = 0)) : ((f) == 0 || (f) == 0) ? ((n)->prev = (l), (l)->next = (n), (l) = (n), (((n)->next) = 0)) : ((f) == (f)) ? ((f)->prev = (n), (n)->next = (f), (f) = (n), (((n)->prev) = 0)) : (((!((f) == 0 || (f) == 0) && (((f)->prev) == 0 || ((f)->prev) == 0)) ? (0) : ((f)->prev->next = (n))), ((n)->prev = (f)->prev), ((f)->prev = (n)), ((n)->next = (f))))
#define dll_remove_np(f,l,n,next,prev) (((n) == (f) ? (f) = (n)->next : (0)), ((n) == (l) ? (l) = (l)->prev : (0)), ((((n)->prev) == 0 || ((n)->prev) == 0) ? (0) : ((n)->prev->next = (n)->next)), ((((n)->next) == 0 || ((n)->next) == 0) ? (0) : ((n)->next->prev = (n)->prev)))

#define dll_insert(f,l,p,n) dll_insert_np(f,l,p,n,next,prev)
#define dll_push_back(f,l,n) dll_push_back_np(f,l,n,next,prev)
#define dll_push_front(f,l,n) dll_push_front_np(f,l,n,next,prev)
#define dll_remove(f,l,n) dll_remove_np(f,l,n,next,prev)

#define member_from_offset(type, ptr, off) (type)((((u8 *)ptr)+(off)))

// typedefs and enums

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

typedef bool b8;

typedef u32 str_match_flags;
enum : u32 {
	str_match_flag_case_insensitive = (1 << 0),
	str_match_flag_right_side_sloppy = (1 << 1),
	str_match_flag_slash_insensitive = (1 << 2),
	str_match_flag_find_last = (1 << 3),
	str_match_flag_keep_empties = (1 << 4),
};

// structs

// memory arena

struct arena_t {
	u32 pos;
	u32 commit_pos;
	u32 align;
	u32 size;
};

// strings

struct str_t {
	u8* data;
	u32 size;
};

struct str_node_t {
	str_node_t* next;
	str_node_t* prev;
	str_t string;
};

struct str_list_t {
	str_node_t* first;
	str_node_t* last;
	u32 count;
	u32 total_size;
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

// math

// vec2
union vec2_t {

	f32 data[2];

	struct {
		f32 x, y;
	};

	inline f32& operator[](i32 index) { return data[index]; }
	inline const f32& operator[](i32 index) const { return data[index]; }
};

// ivec2
union ivec2_t {

	i32 data[2];

	struct {
		i32 x, y;
	};

	inline i32& operator[](i32 index) { return data[index]; }
	inline const i32& operator[](i32 index) const { return data[index]; }
};

// uvec2
union uvec2_t {

	u32 data[2];

	struct {
		u32 x, y;
	};

	inline u32& operator[](i32 index) { return data[index]; }
	inline const u32& operator[](i32 index) const { return data[index]; }
};

// vec3
union vec3_t {

	f32 data[3];

	struct {
		f32 x, y, z;
	};

	struct {
		vec2_t xy;
		f32 z;
	};

	struct {
		f32 x;
		vec2_t yz;
	};

	inline f32& operator[](i32 index) { return data[index]; }
	inline const f32& operator[](i32 index) const { return data[index]; }
};

// vec4
union vec4_t {

	f32 data[4];
	
	struct {
		f32 x, y, z, w;
	};

	struct {
		vec2_t xy;
		f32 z;
		f32 w;
	};

	struct {
		f32 x;
		vec2_t yz;
		f32 w;
	};

	struct {
		f32 x;
		f32 y;
		vec2_t zw;
	};

	struct {
		vec3_t xyz;
		f32 w;
	};
	
	struct {
		f32 x;
		vec3_t yzw;
	};

#if BASE_USE_SIMD
	__m128 sse;
#endif

	inline f32& operator[](i32 index) { return data[index]; }
	inline const f32& operator[](i32 index) const { return data[index]; }
};

union quat_t {

	f32 data[4];

	struct {
		f32 x, y, z, w;
	};

	struct {
		vec3_t xyz;
		f32 w;
	};

#if BASE_USE_SIMD
	__m128 sse;
#endif
};

// matrices:
//
//             c0 c1 c2
//             |  |  | 
//             v  v  v
//	   r0 -> [ 1  2  3 ]
// A = r1 -> [ 4  5  6 ]
//     r2 -> [ 7  8  9 ]
// 
//   column-major:
// -----------------------------------------  
//   - accessed like:
//       A[row][col];
//	  
//   - store as:
//       A = [ 1, 4, 7, 2, 5, 8, 3, 6, 9 ];
//   
//   row major:
// -----------------------------------------  
//   - accessed like:
//       A[col][row];
//   
//   - stored as:
//       A = [ 1, 2, 3, 4, 5, 6, 7, 8, 9 ];
//   

// TODO: implement functions
union mat2_t {
	f32 data[4];
	vec2_t columns[2];

	inline vec2_t& operator[](i32 index) { return columns[index]; }
	inline const vec2_t& operator[](i32 index) const { return columns[index]; }
};

// TODO: implement functions
union mat3_t {
	f32 data[9];
	vec3_t columns[3];

	inline vec3_t& operator[](i32 index) { return columns[index]; }
	inline const vec3_t& operator[](i32 index) const { return columns[index]; }
};

union mat4_t {
	f32 data[16];
	vec4_t columns[4];

	inline vec4_t& operator[](i32 index) { return columns[index]; }
	inline const vec4_t& operator[](i32 index) const { return columns[index]; }
};

// misc

union rect_t {

	struct {
		f32 x0, y0;
		f32 x1, y1;
	};

	struct {
		vec2_t v0;
		vec2_t v1;
	};
	
};

union color_t {

	struct { 
		f32 r, g, b, a; 
	};

	struct {
		f32 h, s, v, a;
	};

	vec4_t vec;
};


// globals

global u32 random_state = 0;

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
function str_t str_copy(arena_t*, str_t);
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
function void str_scan(str_t, char*, ...);

// str list
function void str_list_push_node(str_list_t*, str_node_t*);
function void str_list_push(arena_t*, str_list_t*, str_t);
function str_list_t str_split(arena_t*, str_t, u8*, u32);

// str16
function str16_t str16(u16*);
function str16_t str16(u16*, u32);
function str16_t str_to_st16(str_t);

// random

function void random_seed(u32);
function u32 random_u32();
function u32 random_u32_range(u32, u32);
function f32 random_f32();
function f32 random_f32_range(f32, f32);

// math
inlnfunc f32 radians(f32);
inlnfunc f32 degrees(f32);
inlnfunc f32 remap(f32, f32, f32, f32, f32);
inlnfunc f32 lerp(f32, f32, f32);
inlnfunc f32 s_sqrt(f32);

// vec2 
inlnfunc vec2_t vec2(f32);
inlnfunc vec2_t vec2(f32, f32);
inlnfunc vec2_t vec2_add(vec2_t, f32);
inlnfunc vec2_t vec2_add(vec2_t, vec2_t);
inlnfunc vec2_t vec2_sub(vec2_t, f32);
inlnfunc vec2_t vec2_sub(vec2_t, vec2_t);
inlnfunc vec2_t vec2_mul(vec2_t, f32);
inlnfunc vec2_t vec2_mul(vec2_t, vec2_t);
inlnfunc vec2_t vec2_div(vec2_t, f32);
inlnfunc vec2_t vec2_div(vec2_t, vec2_t);
inlnfunc b8     vec2_equals(vec2_t, vec2_t);
inlnfunc f32    vec2_dot(vec2_t, vec2_t);
inlnfunc f32    vec2_cross(vec2_t, vec2_t);
inlnfunc f32    vec2_length(vec2_t);
inlnfunc vec2_t vec2_normalize(vec2_t);
inlnfunc vec2_t vec2_direction(vec2_t, vec2_t);
inlnfunc f32    vec2_to_angle(vec2_t);
inlnfunc vec2_t vec2_from_angle(f32, f32 = 1);
inlnfunc vec2_t vec2_rotate(vec2_t, f32);
inlnfunc vec2_t vec2_lerp(vec2_t, vec2_t, f32);

// ivec2
inlnfunc ivec2_t ivec2(i32);
inlnfunc ivec2_t ivec2(i32, i32);
inlnfunc b8 ivec2_equals(i32, i32);

// uvec2
inlnfunc uvec2_t uvec2(u32);
inlnfunc uvec2_t uvec2(u32, u32);
inlnfunc b8 uvec2_equals(u32, u32);

// vec3
inlnfunc vec3_t vec3(f32);
inlnfunc vec3_t vec3(f32, f32, f32);
inlnfunc vec3_t vec3(vec2_t, f32);
inlnfunc vec3_t vec3_add(vec3_t, vec3_t);
inlnfunc vec3_t vec3_add(vec3_t, f32);
inlnfunc vec3_t vec3_sub(vec3_t, vec3_t);
inlnfunc vec3_t vec3_sub(vec3_t, f32);
inlnfunc vec3_t vec3_mul(vec3_t, vec3_t);
inlnfunc vec3_t vec3_mul(vec3_t, f32);
inlnfunc vec3_t vec3_div(vec3_t, vec3_t);
inlnfunc vec3_t vec3_div(vec3_t, f32);
inlnfunc b8     vec3_equals(vec3_t, vec3_t);
inlnfunc f32    vec3_dot(vec3_t, vec3_t);
inlnfunc vec3_t vec3_cross(vec3_t, vec3_t);
inlnfunc f32    vec3_length(vec3_t);
inlnfunc vec3_t vec3_normalize(vec3_t);
inlnfunc vec3_t vec3_negate(vec3_t);
inlnfunc vec3_t vec3_lerp(vec3_t, vec3_t, f32);
inlnfunc f32    vec3_angle_between(vec3_t, vec3_t);
inlnfunc vec3_t vec3_project(vec3_t, vec3_t);
inlnfunc vec3_t vec3_rotate(vec3_t, quat_t);
inlnfunc vec3_t vec3_clamp(vec3_t, f32, f32);

// vec4
inlnfunc vec4_t vec4(f32);
inlnfunc vec4_t vec4(f32, f32, f32, f32);
inlnfunc vec4_t vec4_add(vec4_t, vec4_t);
inlnfunc vec4_t vec4_add(vec4_t, f32);
inlnfunc vec4_t vec4_sub(vec4_t, vec4_t);
inlnfunc vec4_t vec4_sub(vec4_t, f32);
inlnfunc vec4_t vec4_mul(vec4_t, vec4_t);
inlnfunc vec4_t vec4_mul(vec4_t, f32);
inlnfunc vec4_t vec4_mul(vec4_t, mat4_t);
inlnfunc vec4_t vec4_div(vec4_t, vec4_t);
inlnfunc vec4_t vec4_div(vec4_t, f32);
inlnfunc b8     vec4_equals(vec4_t, vec4_t);
inlnfunc f32    vec4_dot(vec4_t, vec4_t);
inlnfunc f32    vec4_length(vec4_t);
inlnfunc vec4_t vec4_normalize(vec4_t);
inlnfunc vec4_t vec4_lerp(vec4_t, vec4_t, f32);
inlnfunc f32    vec4_angle_between(vec4_t, vec4_t);
inlnfunc vec4_t vec4_project(vec4_t, vec4_t);

// rect
inlnfunc rect_t rect(f32, f32, f32, f32);
inlnfunc rect_t rect(vec2_t, vec2_t);
inlnfunc b8     rect_equals(rect_t a, rect_t b);
inlnfunc void   rect_validate(rect_t&);
inlnfunc b8     rect_contains(rect_t, vec2_t);
inlnfunc b8     rect_contains(rect_t, rect_t);
inlnfunc rect_t rect_intersection(rect_t, rect_t);
inlnfunc f32    rect_width(rect_t);
inlnfunc f32    rect_height(rect_t);
inlnfunc vec2_t rect_size(rect_t);
inlnfunc vec2_t rect_center(rect_t);
inlnfunc rect_t rect_grow(rect_t, f32);
inlnfunc rect_t rect_grow(rect_t, vec2_t);
inlnfunc rect_t rect_shrink(rect_t, f32);
inlnfunc rect_t rect_shrink(rect_t, vec2_t);
inlnfunc rect_t rect_translate(rect_t, f32);
inlnfunc rect_t rect_translate(rect_t, vec2_t);
inlnfunc rect_t rect_bbox(vec2_t*, u32);
inlnfunc rect_t rect_round(rect_t);

// quat
inlnfunc quat_t quat(f32, f32, f32, f32);
inlnfunc quat_t quat(vec4_t);
inlnfunc quat_t quat_from_axis_angle(vec3_t, f32);
inlnfunc quat_t quat_from_euler_angle(vec3_t);
inlnfunc vec3_t quat_to_euler_angle(quat_t);
inlnfunc vec3_t quat_to_dir(quat_t);
inlnfunc quat_t quat_add(quat_t, quat_t);
inlnfunc quat_t quat_sub(quat_t, quat_t);
inlnfunc quat_t quat_mul(quat_t, quat_t);
inlnfunc quat_t quat_mul(quat_t, f32);
inlnfunc quat_t quat_div(quat_t, f32);
inlnfunc f32    quat_dot(quat_t, quat_t);
inlnfunc quat_t quat_inverse(quat_t);
inlnfunc f32    quat_length(quat_t);
inlnfunc quat_t quat_normalize(quat_t);
inlnfunc quat_t quat_negate(quat_t);
inlnfunc quat_t quat_lerp(quat_t, quat_t, f32);
inlnfunc quat_t quat_slerp(quat_t, quat_t, f32);

// mat4
inlnfunc mat4_t mat4(f32);
inlnfunc b8     mat4_equals(mat4_t, mat4_t);
inlnfunc mat4_t mat4_transpose(mat4_t);
inlnfunc mat4_t mat4_from_quat(quat_t);

inlnfunc mat4_t mat4_translate(vec3_t);
inlnfunc mat4_t mat4_translate(mat4_t, vec3_t);
inlnfunc mat4_t mat4_scale(vec3_t);

inlnfunc mat4_t mat4_mul(mat4_t, mat4_t);
inlnfunc vec4_t mat4_mul(mat4_t, vec4_t);
inlnfunc mat4_t mat4_inverse(mat4_t);
inlnfunc mat4_t mat4_inv_perspective(mat4_t);

inlnfunc mat4_t mat4_orthographic(f32, f32, f32, f32, f32, f32);
inlnfunc mat4_t mat4_perspective(f32, f32, f32, f32);
inlnfunc mat4_t mat4_lookat(vec3_t, vec3_t, vec3_t);
function void   mat4_print(mat4_t);

// color
inlnfunc color_t color(u32 hex);
inlnfunc color_t color(f32 r, f32 g, f32 b, f32 a = 1.0f);
inlnfunc color_t color_add(color_t a, f32 b);
inlnfunc color_t color_add(color_t a, color_t b);
inlnfunc color_t color_lerp(color_t a, color_t b, f32 t);
inlnfunc color_t color_rgb_to_hsv(color_t rgb);
inlnfunc color_t color_hsv_to_rgb(color_t hsv);

// misc
function vec3_t barycentric(vec2_t, vec2_t, vec2_t, vec2_t);
function b8 tri_contains(vec2_t, vec2_t, vec2_t, vec2_t);

#endif // BASE_H