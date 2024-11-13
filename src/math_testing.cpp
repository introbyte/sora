// math_testing.cpp

#include "engine/base.h"
#include "engine/os.h"

#include "engine/base.cpp"
#include "engine/os.cpp"

void mat4_print(mat4_t m) {
	printf("         c0:     c1:     c2:     c3:\n");
	for (i32 row = 0; row < 4; row++) {
		printf("r%i: [ %7.2f %7.2f %7.2f %7.2f ]\n", row, m[0][row], m[1][row], m[2][row], m[3][row]);
	}
}

int main() {

	// define matrix A
	mat4_t A = mat4();
	A.data[0] = 16;
	A.data[1] = 5;
	A.data[2] = 9;
	A.data[3] = 4;
	A.data[4] = 2;
	A.data[5] = 11;
	A.data[6] = 7;
	A.data[7] = 14;
	A.data[8] = 3;
	A.data[9] = 10;
	A.data[10] = 6;
	A.data[11] = 15;
	A.data[12] = 13;
	A.data[13] = 8;
	A.data[14] = 12;
	A.data[15] = 1;

	printf("A:\n");
	mat4_print(A);
	printf("\n");

	printf("A.data[13] = %5.2f\n", A.data[13]);
	printf("A[3][1] = %5.2f\n", A[3][1]); // col, row
	

	mat4_t proj = mat4_perspective(80.0f, 1.5f, 0.01f, 100.0f);
	mat4_t view = mat4_translate(vec3(1.0f, 2.0f, 3.0f));

	printf("proj:\n");
	mat4_print(proj);

	printf("view:\n");
	mat4_print(view);

	mat4_t view_proj = mat4_mul(proj, view);
	printf("view_proj:\n");
	mat4_print(view_proj);

	return 0;
}