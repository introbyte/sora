// tree_test.cpp

// includes

#include "engine/base.h"
#include "engine/os.h"
#include "engine/gfx.h"
#include "engine/font.h"
#include "engine/draw.h"
#include "engine/ui.h"

#include "engine/base.cpp"
#include "engine/os.cpp"
#include "engine/gfx.cpp"
#include "engine/font.cpp"
#include "engine/draw.cpp"
#include "engine/ui.cpp"

// globals

global os_window_t* window;
global gfx_renderer_t* renderer;
global arena_t* resource_arena;
global arena_t* scratch_arena;

struct node_t {
	node_t* next;
	node_t* prev;
	node_t* first;
	node_t* last;
	node_t* parent;

	str_t label;
	vec2_t pos;
	f32 mod;
};

struct node_rec_t {
	node_t* next;
	i32 push_count;
	i32 pop_count;
};

global node_t* root;
global node_t* active;
global u32 count;

function node_t*
node_create(arena_t* arena, str_t label) {
	node_t* node = (node_t*)arena_calloc(arena, sizeof(node_t));
	node->label = label;
	return node;
}

function void
node_insert(node_t* parent, node_t* prev, node_t* node) {
	dll_insert(parent->first, parent->last, prev, node);
	node->parent = parent;
}

function void
node_remove(node_t* parent, node_t* node) {
	dll_remove(parent->first, parent->last, node);
	node->next = node->prev = node->parent = nullptr;
}

function node_rec_t
node_rec_depth_first_pre(node_t* node) {
	node_rec_t rec = { 0 };
	if (node->first != 0) {
		rec.next = node->first;
		rec.push_count = 1;
	} else for (node_t* n = node; n != 0; n = n->parent) {
		if (n->next != 0) {
			rec.next = n->next;
			break;
		}
		rec.pop_count++;
	}
	return rec;
}

function node_rec_t
node_rec_depth_first_post(node_t* node) {
	node_rec_t rec = { 0 };
	if (node->last != 0) {
		rec.next = node->last;
		rec.push_count = 1;
	} else for (node_t* n = node; n != 0; n = n->parent) {
		if (n->prev != 0) {
			rec.next = n->prev;
			break;
		}
		rec.pop_count++;
	}
	return rec;
}

// app functions

function void
app_init() {

	// allocate arenas
	resource_arena = arena_create(gigabytes(1));
	scratch_arena = arena_create(gigabytes(1));

	root = node_create(resource_arena, str("root"));
	active = root;
	count = 1;
}

function void
app_update() {

	// fullscreen
	if (os_key_release(window, os_key_F11)) {
		os_window_fullscreen(window);
	}






	//if (os_key_press(window, os_key_left)) {
	//	if (active->prev != 0) {
	//		active = active->prev;
	//	} else if (active->parent != 0 && active->parent->prev != 0 && active->parent->prev->last != 0) {
	//		active = active->parent->prev->last;
	//	}
	//}
	//if (os_key_press(window, os_key_right)) {
	//	if (active->next != 0) {
	//		active = active->next;
	//	} else if (active->parent != 0 && active->parent->next != 0 && active->parent->next->first != 0) {
	//		active = active->parent->next->first;
	//	}
	//}
	//if (os_key_press(window, os_key_up)) {
	//	if (active->parent != 0) {
	//		active = active->parent;
	//	}
	//}
	//if (os_key_press(window, os_key_down)) {
	//	if (active->first != 0) {
	//		active = active->first;
	//	}
	//}

	//if (os_key_press(window, os_key_A)) {
	//	str_t label = str_format(scratch_arena, "node%u", count);
	//	node_t* node = node_create(resource_arena, label);
	//	if (active->parent != 0) {
	//		node_insert(active->parent, active->prev, node);
	//	} else {
	//		node_insert(root, root->last, node);
	//	}
	//	count++;
	//}
	//if (os_key_press(window, os_key_D)) {
	//	str_t label = str_format(scratch_arena, "node%u", count);
	//	node_t* node = node_create(resource_arena, label);
	//	if (active->parent != 0) {
	//		node_insert(active->parent, active->parent->last, node);
	//	} else {
	//		node_insert(root, root->last, node);
	//	}
	//	count++;
	//}
	//if (os_key_press(window, os_key_S)) {
	//	str_t label = str_format(scratch_arena, "node%u", count);
	//	node_t* node = node_create(resource_arena, label);
	//	node_insert(active, active->last, node);

	//	count++;
	//}

}

function void
main_pass(gfx_render_target_t* current_render_target, gfx_render_target_t* prev_rander_target) {

	ui_begin_frame(renderer);

	ui_push_pref_width(ui_size_pixel(60.0f, 1.0f));
	ui_push_pref_height(ui_size_pixel(20.0f, 1.0f));
	ui_push_text_alignment(ui_text_alignment_center);
	
	// determine position
	i32 next_x = 0;
	i32 depth = 0;
	for (node_t* node = root; node != 0;) {
		node_rec_t rec = node_rec_depth_first_pre(node);

		// if no children
		if (node->first == 0) {
			node->pos.x = next_x * 60.0f;
			next_x++;
		} else {
			// center above children
			f32 first_child_x = node->first->pos.x;
			f32 last_child_x = node->last->pos.x;
			node->pos.x = (first_child_x + last_child_x) * 0.5f;
		}
		
		node->pos.y = 50.0f + (depth * 20.0f);
		depth += rec.push_count - rec.pop_count;
		node = rec.next;
	}

	for (node_t* node = root; node != 0;) {
		node_rec_t rec = node_rec_depth_first_pre(node);
		
		ui_frame_flags flags = ui_frame_flag_floating;
		if (active == node) {
			flags |= ui_frame_flag_draw_border_light;
		}
		ui_set_next_fixed_x(node->pos.x);
		ui_set_next_fixed_y(node->pos.y);
		ui_set_next_flags(flags);
		
		ui_label(node->label);
		
		node = rec.next;
	}

	ui_pop_pref_width();
	ui_pop_pref_height();

	ui_end_frame();

}

function void
app_release() {

	// release arenas
	arena_release(resource_arena);
	arena_release(scratch_arena);

}

// entry point

function i32
app_entry_point(i32 argc, char** argv) {

	// init layers
	os_init();
	gfx_init();
	font_init();
	draw_init();
	ui_init();

	// create contexts
	window = os_window_open(str("tree test"), 1280, 960);
	renderer = gfx_renderer_create(window, color(0x050505ff));

	// add ui pass
	gfx_render_target_desc_t render_target_desc = { 0 };
	render_target_desc.size = renderer->resolution;
	render_target_desc.sample_count = 1;
	render_target_desc.flags = gfx_render_target_flag_no_depth;
	render_target_desc.colorbuffer_format = gfx_texture_format_rgba8;
	gfx_renderer_add_pass(renderer, str("main_pass"), main_pass, render_target_desc);

	// init
	app_init();

	// main loop
	while (os_window_is_running(window)) {

		// update layers
		os_update();
		gfx_update();
		draw_update();

		// update app
		app_update();

		// submit to renderer
		gfx_renderer_submit(renderer);
	}

	// release
	app_release();

	// release context
	gfx_renderer_release(renderer);
	os_window_close(window);

	// release layers
	ui_release();
	draw_release();
	font_release();
	gfx_release();
	os_release();

	return 0;
}

// entry point

#if defined(BUILD_DEBUG)
int main(int argc, char** argv) {
	return app_entry_point(argc, argv);
}
#elif defined(BUILD_RELEASE)
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	return app_entry_point(__argc, __argv);
}
#endif 