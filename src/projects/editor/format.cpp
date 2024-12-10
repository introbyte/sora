// format.cpp

#ifndef FORMAT_CPP
#define FORMAT_CPP

// implementation

function void
space_init() {

	// allocate arenas
	space.entity_arena = arena_create(gigabytes(1));
	space.text_arena = arena_create(gigabytes(1));
	space.event_arena = arena_create(megabytes(1));
	
	// null free list
	space.line_free = nullptr;
	space.token_free = nullptr;

	// load assets
	space.font = font_open(str("res/fonts/consola.ttf"));


	// add first line and token
	space.line_first = line_create();
	space.line_first->token_first = token_create();

	space.cursor = { 0, 1, 1 };
	space.mark = { 0, 1, 1 };

}

function void
space_release() {

	// release arenas
	arena_release(space.entity_arena);
	arena_release(space.text_arena);
	arena_release(space.event_arena);

	// release assets
	font_close(space.font);
}

function void 
space_update() {

	// clear events arena
	arena_clear(space.event_arena);
	space.event_first = nullptr;
	space.event_last = nullptr;

	// retrieve events from os
	for (os_event_t* os_event = os_state.event_list.first; os_event != 0; os_event = os_event->next) {
		space_event_t space_event = { 0 };

		space_event.os_event = os_event;
		space_event.window = os_event->window;
		space_event.type = space_event_type_null;
		space_event.flags = 0;
		space_event.key = os_event->key;
		space_event.mouse_button = os_event->mouse;
		space_event.modifiers = os_event->modifiers;
		space_event.character = os_event->character;
		space_event.position = os_event->position;
		space_event.scroll = os_event->scroll;
		space_event.delta = ivec2(0);

		if (os_event->type != 0) {
			switch (os_event->type) {
				case os_event_type_key_press: { space_event.type = space_event_type_key_press; break; }
				case os_event_type_key_release: { space_event.type = space_event_type_key_release; break;}
				case os_event_type_mouse_press: { space_event.type = space_event_type_mouse_press; break;}
				case os_event_type_mouse_release: { space_event.type = space_event_type_mouse_release; break;}
				case os_event_type_mouse_move: { space_event.type = space_event_type_mouse_move; break;}
				case os_event_type_mouse_scroll: { space_event.type = space_event_type_mouse_scroll;break; }
				case os_event_type_text: { space_event.type = space_event_type_text; break; }
			}
		}

		space_event_push(&space_event);
	}
	
	// get cursor line and token
	space.line_active = line_from_point(space.cursor);
	space.token_active = token_from_point(space.cursor);

	// convert events into text ops
	for (space_event_t* event = space.event_first; event != nullptr; event = event->next) {

		space_op_t op = space_op_from_event(event, space.cursor, space.mark);
		
		if (!point_equals(op.range.min, op.range.max) || op.replace.size != 0) {

		}

		// update cursor and mark
		space.cursor = op.cursor;
		space.mark = op.mark;

		if (event->type == space_event_type_text) {
			b8 taken = false;
			// all cases of text ops:
			//
			// [ ] insert char in token.
			// [ ] remove char in token.
			// [ ] add new token.
			//    [ ] space at end of token.
			//    [ ] symbols (+, -, ;, etc.)
			//    [ ] copy, paste (worry about later though).
			// [ ] split token in two.
			//    [ ] space in middle of token.
			//    [ ] symbol (also need to insert new symbol token).
			// [ ] combine tokens.
			//    [ ] backspace between two tokens.
			// [ ] split lines in two.
			//    [ ] enter in middle of token.
			// [ ] combine lines. 
			//    [ ] backspace between two lines.
			//

			//switch (event->character) {

			//	case os_key_space: {
			//		// create new token
			//		token_t* token = token_create();
			//		dll_insert(space.line_active->token_first, space.line_active->token_last, space.token_active, token);
			//		space.token_active = token;
			//		space.cursor.char_index = 0;
			//		space.cursor.token_index++;
			//		taken = true;
			//		break;
			//	}

			//	default: {
			//		space.token_active->data[space.cursor.char_index] = event->character;
			//		space.token_active->size++;
			//		space.cursor.char_index++;
			//		taken = true;
			//		break;
			//	}

			//}

			//if (taken) {
			//	space_event_pop(event);
			//}
		}
	}

}

function void
space_render(f32 dt) {

	vec2_t layout = vec2(50.0f, 50.0f);

	draw_push_font(space.font);
	draw_push_font_size(12.0f);
	draw_push_color(color(color_text));

	vec2_t cursor_pos = vec2(50.0f, 50.0f);

	i32 token_index = 1;
	i32 line_index = 1;
	f32 text_height = roundf(font_text_get_height(space.font, 12.0f));

	for (line_t* line = space.line_first; line != nullptr; line = line->next) {
		for (token_t* token = line->token_first; token != nullptr; token = token->next) {

			str_t token_string = str(token->data, token->size);
			f32 text_width = font_text_get_width(space.font, 12.0f, token_string);

			draw_text(token_string, layout);
			
			if (token == space.token_active) {
				draw_set_next_color0(color(0xc8c8c800));
				draw_set_next_color1(color(0xc8c8c8dd));
				draw_set_next_color2(color(0xc8c8c800));
				draw_set_next_color3(color(0xc8c8c8dd));
				draw_rect(rect(layout.x-1.0f, layout.y + text_height - 4.0f, layout.x + text_width+1.0f, layout.y + text_height));
			}

			// determine cursor pos
			if (token == space.token_active) {
				str_t partial_string = str(token->data, space.cursor.char_index);
				f32 partial_width = font_text_get_width(space.font, 12.0f, partial_string);
				cursor_pos.x += partial_width;
			} else {
				if (token_index < space.cursor.token_index) {
					cursor_pos.x += text_width + 12.0f;
				}
			}

			token_index++;
			layout.x += text_width + 12.0f;
		}
		line_index++;
		if (line_index < space.cursor.line_index) {
			cursor_pos.y += 25.0f;
		}
	}

	f32 fast_rate = 1.0f - powf(2.0f, -50.0f * dt);
	f32 slow_rate = 1.0f - powf(2.0f, -30.0f * dt);

	space.cursor_pos_t = vec2_add(space.cursor_pos_t, vec2_mul(vec2_sub(cursor_pos, space.cursor_pos_t), slow_rate));

	// draw cursor
	draw_set_next_color(color(0xcececeff));
	draw_rect(rect(space.cursor_pos_t.x, space.cursor_pos_t.y, cursor_pos.x + 2.0f, cursor_pos.y + text_height));


	// draw mark
	
	draw_pop_color();
	draw_pop_font_size();
	draw_pop_font();

}

function void 
space_event_push(space_event_t* event) {
	space_event_t* new_event = (space_event_t*)arena_calloc(space.event_arena, sizeof(space_event_t));
	memcpy(new_event, event, sizeof(space_event_t));
	dll_push_back(space.event_first, space.event_last, new_event);
}

function void 
space_event_pop(space_event_t* event) {
	if (event->os_event != nullptr) {
		os_event_pop(event->os_event); // take os event if ui event was taken.
	}
	dll_remove(space.event_first, space.event_last, event);
}


function space_op_t 
space_op_from_event(space_event_t* event, point_t cursor, point_t mark) {

	point_range_t range = { 0 };
	str_t replace = { 0 };
	str_t copy = { 0 };
	point_t next_cursor = cursor;
	point_t next_mark = mark;

	// insert character
	if (event->character != 0) {
		range = point_range(cursor, mark);
		replace = str((char*)(&event->character), 1);
		next_cursor = next_mark = { range.min.char_index + 1, range.min.token_index, range.min.line_index };
	}

	space_op_t space_op = { 0 };
	space_op.flags = 0;
	space_op.replace = replace;
	space_op.copy = copy;
	space_op.cursor = next_cursor;
	space_op.mark = next_mark;
	return space_op;

}


// point

function b8 
point_equals(point_t a, point_t b) {
	return (a.char_index == b.char_index && a.token_index == b.token_index && a.line_index == b.line_index);
}

function b8 
point_less_than(point_t a, point_t b) {
	b8 result = 
		(a.line_index < b.line_index) ||
		(a.line_index == b.line_index && a.token_index < b.token_index) ||
		(a.line_index == b.line_index && a.token_index == b.token_index && a.char_index < b.char_index);
	return result;
}

function point_t
point_min(point_t a, point_t b) {
	point_t result = b;
	if (point_less_than(a, b)) {
		result = a;
	}
	return result;
}

function point_t 
point_max(point_t a, point_t b) {
	point_t result = a;
	if (point_less_than(a, b)) {
		result = b;
	}
	return result;
}

// point range

function point_range_t 
point_range(point_t min, point_t max) {
	point_range_t result = { 0 };
	if (point_less_than(min, max)) {
		result.min = min;
		result.max = max;
	} else {
		result.min = max;
		result.max = min;
	}
	return result;
}

function point_range_t
point_range_intersects(point_range_t a, point_range_t b) {
	point_range_t result = { 0 };
	result.min = point_min(a.min, b.min);
	result.max = point_max(a.max, b.max);
	if (point_less_than(result.max, result.min)) {
		memset(&result, 0, sizeof(point_range_t));
	}
	return result;
}

function point_range_t 
point_range_union(point_range_t a, point_range_t b) {
	point_range_t result = { 0 };
	result.min = point_min(a.min, b.min);
	result.max = point_max(a.max, b.max);
	return result;
}

function b8 
point_range_contains(point_range_t r, point_t p) {
	b8 result = (point_less_than(r.min, p) || point_equals(r.min, p)) && point_less_than(p, r.max);
	return result;
}


// token

function token_t* 
token_create() {
	token_t* token = space.token_free;
	if (token != nullptr) {
		stack_pop(space.token_free);
	} else {
		token = (token_t*)arena_alloc(space.entity_arena, sizeof(token_t));
		token->data = (char*)arena_alloc(space.text_arena, sizeof(char) * token_data_max);
	}
	token->size = 0;
	return token;
}

function void
token_release(token_t* token) {
	stack_push(space.token_free, token);
}

function token_t*
token_from_point(point_t point) {
	token_t* result = nullptr;
	line_t* line = line_from_point(point);
	i32 index = 1;

	for (token_t* token = line->token_first; token != nullptr; token = token->next) {
		if (index == point.token_index) {
			result = token;
			break;
		}
		index++;
	}
	return result;
}

// line
function line_t*
line_create() {
	line_t* line = space.line_free;
	if (line != nullptr) {
		stack_pop(space.line_free);
	} else {
		line = (line_t*)arena_alloc(space.entity_arena, sizeof(line_t));
	}
	memset(line, 0, sizeof(line_t));
	return line;
}

function void
line_release(line_t* line) {
	// free all tokens in the line
	for (token_t* token = line->token_first; token != nullptr; token = token->next) {
		dll_remove(line->token_first, line->token_last, token);
		token_release(token);
	}
	stack_push(space.line_free, line);
}

function line_t* 
line_from_point(point_t point) {
	line_t* result = nullptr;
	i32 index = 1;
	for (line_t* line = space.line_first; line != nullptr; line = line->next) {
		if (index == point.line_index) {
			result = line;
			break;
		}
		index++;
	}
	return result;
}




#endif // FORMAT_CPP