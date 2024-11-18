// template_console.cpp

#include "engine/base.h"
#include "engine/os.h"

#include "engine/base.cpp"
#include "engine/os.cpp"

function i32
app_entry_point(i32 argc, char** argv) {

	printf("hello, world!\n");


	return 0;
}

#if defined(BUILD_DEBUG)
int main(int argc, char** argv) {
	return app_entry_point(argc, argv);
}
#elif defined(BUILD_RELEASE)
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	return app_entry_point(__argc, __argv);
}
#endif 