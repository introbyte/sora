// base.h

#ifndef BASE_H
#define BASE_H

// includes

#include <cstdio>

// defines

#define function static
#define global static
#define persist static

#define bytes(n)      (n)
#define kilobytes(n)  (n << 10)
#define megabytes(n)  (n << 20)
#define gigabytes(n)  (((u64)n) << 30)
#define terabytes(n)  (((u64)n) << 40)

#define arena_commit_size kilobytes(4)
#define arena_decommit_size megabytes(4)

#define stack_push(f, n) ((n)->next = (f), (f) = (n))
#define stack_pop(f) (((f) == 0) ? 0 : ((f) = (f)->next))


#define dll_push_back(f, l, n)\
((f)==0?\
((f)=(l)=(n),(n)->next=(n)->prev=0):\
((n)->prev=(l),(l)->next=(n),(l)=(n),(n)->next=0))

#define dll_push_front(f, l, n)\
((l)==0?\
((l)=(f)=(n),(n)->prev=(n)->next=0):\
((n)->next=(f),(f)->prev=(n),(f)=(n),(n)->prev=0))

#define dll_remove(f, l, n)\
((f)==(n)?\
((f)==(l)?\
((f)=(l)=(0)):\
((f)=(f)->next,(f)->prev=0)):\
(l)==(n)?\
((l)=(l)->prev,(l)->next=0):\
((n)->next->prev=(n)->prev,\
(n)->prev->next=(n)->next))

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
	f32 r, g, b, a;
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

function void* arena_malloc(arena_t*, u32);
function void* arena_calloc(arena_t*, u32);

function void arena_clear(arena_t*);

// cstr
function u32 cstr_length(cstr cstr);
function b8 cstr_equals(cstr cstr1, cstr cstr2);

// str
function str_t str(char*);
function str_t str(char*, u32);

// color
function color_t color(u32);
function color_t color(f32, f32, f32, f32);



#endif // BASE_H