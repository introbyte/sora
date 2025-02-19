
//#include "engine/base.h"

#include <cmath>
#include <pmmintrin.h>

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

#define min(a, b) (((a)<(b)) ? (a) : (b))
#define max(a, b) (((a)>(b)) ? (a) : (b))


#include <windows.h>
#include <timeapi.h>
#include <dwmapi.h>

#include <vulkan/vulkan.h>
#pragma comment(lib, "vulkan-1.lib")

int main() {

	return 0;
}