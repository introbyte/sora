// main.cpp

#include "base.h"
#include "os.h"
#include "gfx.h"

#include "base.cpp"
#include "os.cpp"
#include "gfx.cpp"


//#include "sui.h"


int main(int argc, char** argv) {

	os_init();
	gfx_init();

	os_window_t* window = os_window_open(str("title"), 1280, 960);
	gfx_renderer_t* renderer = gfx_renderer_create(window, color(0x07090bff), 1);

	while (os_state.first_window != nullptr) {
		os_update();
		gfx_renderer_begin_frame(renderer);



		gfx_renderer_end_frame(renderer);
	}

	gfx_renderer_release(renderer);
	os_release();

	return 0;
}