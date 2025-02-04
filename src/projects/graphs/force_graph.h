// force_graph.h

#ifndef FORCE_GRAPH_H
#define FORCE_GRAPH_H

// structs

struct fg_node_t {
	fg_node_t* next;
	fg_node_t* prev;
    
	// sim
	vec2_t pos;
	vec2_t vel;
    
	// info
	f32 size;
	color_t color;
    void* data;
    
	// ui
	ui_frame_t* frame;
};

struct fg_link_t {
	fg_link_t* next;
	fg_link_t* prev;
	
	fg_node_t* from;
	fg_node_t* to;
    
	// sim
	f32 length;
    
};

struct fg_link_draw_data_t {
	vec2_t from;
	vec2_t to;
};

struct fg_state_t {
    
	arena_t* arena;
    
	fg_node_t* node_first;
	fg_node_t* node_last;
	fg_node_t* node_free;
	fg_node_t* node_active;
    
	fg_link_t* link_first;
	fg_link_t* link_last;
	fg_link_t* link_free;
    
	// simulation params
	f32 link_size;
	f32 link_strength;
	f32 repulsive_strength;
	f32 damping;
	u32 iterations;
    
};


// functons

// state
function fg_state_t* fg_create();
function void fg_release(fg_state_t* state);
function void fg_update(fg_state_t* state, f32 dt);

// simulation
function void fg_apply_spring_forces(fg_state_t* state, f32 dt);
function void fg_apply_repulsive_forces(fg_state_t* state, f32 dt);
function void fg_update_position(fg_state_t* state, f32 dt);

// nodes
function fg_node_t* fg_node_create(fg_state_t* state, vec2_t pos, f32 size = 8.0f, void* data = nullptr);
function void fg_node_release(fg_state_t* state, fg_node_t* node);

// links
function fg_link_t* fg_link_create(fg_state_t* state, fg_node_t* from, fg_node_t* to, f32 length = 75.0f);
function void fg_link_release(fg_state_t* state, fg_link_t* link);

// ui custom draw functions
function void fg_node_custom_draw(ui_frame_t* frame);
function void fg_link_custom_draw(ui_frame_t* frame);

#endif // FORCE_GRAPH_H