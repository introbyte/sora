// editor.cpp

#ifndef EDITOR_CPP
#define EDITOR_CPP

// implementation

function void 
editor_init() {

	// allocate arenas
	editor_state.space_arena = arena_create(megabytes(8));

	editor_state.space_count = 0;
}

function void 
editor_release() {

	// release arenas
	arena_release(editor_state.space_arena);

}

function void 
editor_update() {

}

// space

function space_t*
space_create() {

	space_t* space = editor_state.space_free;
	if (space != nullptr) {
		stack_pop(editor_state.space_free);
	} else {
		space = (space_t*)arena_alloc(editor_state.space_arena, sizeof(space_t));
	}
	memset(space, 0, sizeof(space_t));

	// allocate node arena
	space->node_arena = arena_create(gigabytes(1));

	// add to space list
	dll_push_back(editor_state.space_first, editor_state.space_last, space);
	editor_state.space_count++;

	// create root node
	space->node_root = node_create(space, str("program"), node_type_program);

	return space;
}

function void
space_release(space_t* space) {

	// remove from space list
	dll_remove(editor_state.space_first, editor_state.space_last, space);
	stack_push(editor_state.space_free, space);
	editor_state.space_count--;

	// release node arena
	arena_release(space->node_arena);
}

function void
space_render(space_t* space) {

	space->layout_pos = vec2(50.0f, 50.0f);


	for (node_t* node = space->node_root; node != 0;) {
		node_rec_t rec = node_rec_depth_first(node);
		switch (node->type) {

			case node_type_program: {

				break;
			}

			case node_type_identifier: {
				space_push_text(space, node->label);
				break;
			}

			case node_type_keyword: {
				space_push_text(space, node->label);
				break;
			}

			case node_type_function_declaration: {
				space_push_text(space, node->label);
				space_push_text(space, str("{"));
				space_push_newline(space);
				space_push_text(space, str("}"));
				break;
			} 

		}
		node = rec.next;
	}

}

function void
space_push_text(space_t* space, str_t text) {
	draw_text(text, space->layout_pos);
	space->layout_pos.x += font_text_get_width(draw_state.font, 12.0f, text) + 12.0f;
}

function void
space_push_newline(space_t* space) {
	space->layout_pos.x = 50.0f;
	space->layout_pos.y += 20.0f;
}



// nodes

function node_t* 
node_create(space_t* space, str_t label, node_type type, node_flags flags) {

	node_t* node = space->node_free;
	if (node != nullptr) {
		stack_pop(space->node_free);
	} else {
		node = (node_t*)arena_alloc(space->node_arena, sizeof(node_t));
	}
	memset(node, 0, sizeof(node_t));

	node->type = type;
	node->flags = flags;
	node->label = label;

	return node;
}

function void 
node_release(space_t* space, node_t* node) {

	// remove from parent if needed
	if (node->parent != nullptr) {
		dll_remove(node->parent->first, node->parent->last, node);
	}

	stack_push(space->node_free, node);
}

function void 
node_insert(node_t* parent, node_t* node, node_t* prev) {
	if (prev == nullptr) {
		dll_push_back(parent->first, parent->last, node);
	} else {
		dll_insert(parent->first, parent->last, prev, node);
	}
	node->parent = parent;
}

function void
node_remove(node_t* parent, node_t* node) {
	dll_remove(parent->first, parent->last, node);
	node->parent = nullptr;
}

function node_rec_t 
node_rec_depth_first(node_t* node) {
	node_rec_t rec = { 0 };
	if (node->first != 0) {
		rec.next = node->first;
		rec.push_count++;
	} else for (node_t* n = node; n != 0; n = n->parent) {
		if (n->next != 0) {
			rec.next = n->next;
			break;
		}
		rec.pop_count++;
	}
	return rec;
}

#endif // EDITOR_CPP