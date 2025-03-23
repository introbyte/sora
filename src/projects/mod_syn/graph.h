// graph.h

#ifndef GRAPH_H
#define GRAPH_H

//- defines 

#define graph_buffer_size (128)
#define graph_audio_sample_rate (1.0f / 44100.0f)

//- enums 

enum graph_data_type {
    graph_data_type_none,
    graph_data_type_f32,
    graph_data_type_i32,
    graph_data_type_u32,
    graph_data_type_range,
    graph_data_type_buffer,
    graph_data_type_count,
};

enum graph_port_type {
    graph_port_type_input,
    graph_port_type_output,
};

typedef u32 graph_port_flags;
enum {
    graph_port_flag_none = (0),
    graph_port_flag_edit = (1 << 0),
    graph_port_flag_display = (1 << 1),
};

//- typedefs 

struct graph_t;
struct graph_node_t;
struct graph_port_t;

typedef void graph_node_process_func(graph_node_t*);

//- structs 

struct graph_key_t {
    u64 data[1];
};

struct graph_data_t {
    
    graph_key_t key;
    
    graph_data_t* next;
    graph_data_t* prev;
    
    graph_data_type type;
    union {
        
        f32 f32_value;
        
        i32 i32_value;
        
        i32 u32_value;
        
        struct {
            f32 range_value;
            f32 range_min;
            f32 range_max;
        };
        
        f32 buffer[graph_buffer_size];
    };
};

// port
struct graph_port_t {
    graph_key_t key;
    
    graph_port_t* next;
    graph_port_t* prev;
    
    str_t label;
    graph_port_type port_type;
    graph_port_flags flags;
    
    graph_data_t* data;
    graph_node_t* node;
    graph_port_t* connected_port;
    
    // ui 
    ui_frame_t* frame;
};

struct graph_port_list_node_t {
    graph_port_list_node_t* next;
    graph_port_list_node_t* prev;
    graph_port_t* port;
};

struct graph_port_list_t {
    graph_port_list_node_t* first;
    graph_port_list_node_t* last;
};

// node
struct graph_node_t {
    
    graph_key_t key;
    graph_t* graph;
    
    graph_node_t* next;
    graph_node_t* prev;
    
    // ports
    graph_port_t* port_first;
    graph_port_t* port_last;
    
    // funcs
    graph_node_process_func* process_func;
    
    // data
    str_t label;
    
    // ui
    ui_frame_t* frame;
    vec2_t pos;
    
};

struct graph_node_list_node_t {
    graph_node_list_node_t* next;
    graph_node_list_node_t* prev;
    graph_node_t* node;
};

struct graph_node_list_t {
    graph_node_list_node_t* first;
    graph_node_list_node_t* last;
};

// topological sort

struct graph_ts_node_t {
    graph_ts_node_t* next;
    graph_ts_node_t* prev;
    
    graph_node_t* node;
    u32 in_degree;
};

struct graph_ts_queue_t {
    graph_ts_queue_t* next;
    graph_ts_queue_t* prev;
    
    graph_ts_node_t* node;
};

// graph

struct graph_t {
    
    arena_t* arena;
    os_handle_t window;
    
    // data list
    graph_data_t* data_first;
    graph_data_t* data_last;
    graph_data_t* data_free;
    
    // free port list
    graph_port_t* port_free;
    
    // node list
    graph_node_t* node_first;
    graph_node_t* node_last;
    graph_node_t* node_free;
    
    // process list
    arena_t* process_list_arena;
    graph_node_list_t process_list;
    
};

//- globals 

global color_t graph_data_type_colors[] = {
    color(0x505050ff),
    color(0xB0EB93ff),
    color(0xFFF7A0ff),
    color(0xFFC384ff),
    color(0xF98284ff),
    color(0xB0A9E4ff),
    color(0xFEAAE4ff),
    color(0xB3E3DAff),
};

//- functions 

// graph
function graph_t* graph_create(os_handle_t window);
function void graph_release(graph_t* graph);
function void graph_build(graph_t* graph);
function void graph_process(graph_t* graph);
function void graph_render(graph_t* graph);

// key
function graph_key_t graph_key_from_string(graph_key_t seed, str_t string);
function graph_key_t graph_key_from_stringf(graph_key_t seed, char* fmt, ...);
function b8 graph_key_equals(graph_key_t a, graph_key_t b);

// data
function graph_data_t* graph_data_from_key(graph_t* graph, graph_data_type type, graph_key_t key);
function graph_data_t* graph_data_from_string(graph_t* graph, graph_data_type type, str_t string);
function graph_data_t* graph_data_from_stringf(graph_t* graph, graph_data_type type, char* fmt, ...);
function void graph_data_release(graph_t* graph, graph_data_t* data);

// port
function graph_port_t* graph_port_create(graph_t* graph);
function void graph_port_release(graph_t* graph, graph_port_t* port);
function graph_port_t* graph_port_find(graph_node_t* node, str_t label);

// node
function graph_node_t* graph_node_create(graph_t* graph, str_t label, graph_node_process_func* process_func);
function void graph_node_release(graph_node_t* node);
function graph_port_t* graph_node_add_input(graph_node_t* node, str_t label, graph_data_type data_type, graph_port_flags flags = 0);
function graph_port_t* graph_node_add_output(graph_node_t* node, str_t label, graph_data_type data_type, graph_port_flags flags = 0);
function void graph_node_bring_to_front(graph_node_t* node);

// data display 

function void graph_data_display_buffer(ui_frame_t* frame);

#endif // GRAPH_H