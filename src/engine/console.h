// console.h

#ifndef CONSOLE_H
#define CONSOLE_H

//- types 

typedef void console_command_func(str_list_t);

//- structs 

struct console_command_t {
    console_command_t* next;
    console_command_t* prev;
    
    str_t string;
    u32 argument_count;
    console_command_func* func;
};

struct console_state_t {
    arena_t* arena;
    
    console_command_t* command_first;
    console_command_t* command_last;
    console_command_t* command_free;
    
};

//- functions 

function console_state_t* console_init();
function void console_release(console_state_t* state);
function void console_execute(console_state_t* state, str_t command_string);

function void console_command_register(console_state_t* state, str_t command_string, u32 argument_count, console_command_func* func);
function void console_command_unregister(console_state_t* state, str_t command_string);


#endif // CONSOLE_H