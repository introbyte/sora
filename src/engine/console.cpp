// console.cpp

#ifndef CONSOLE_CPP
#define CONSOLE_CPP

//- implementation 

function console_state_t* 
console_init() {
    
    arena_t* arena = arena_create(megabytes(128));
    
    console_state_t* state = (console_state_t*)arena_alloc(arena, sizeof(console_state_t));
    state->arena = arena;
    
    return state;
}

function void
console_release(console_state_t* state) {
    arena_release(state->arena);
}

function void
console_execute(console_state_t* state, str_t command_string) {
    
    temp_t scratch = scratch_begin();
    
    str_list_t str_list = str_split(scratch.arena, command_string, (u8*)" ", 1);
    
    str_t command_name = str_list.first->string;
    console_command_t* command = nullptr;
    
    for (console_command_t* c = state->command_first; c != nullptr; c = c->next) {
        if (str_match(command_name, c->string)) {
            command = c;
            break;
        }
    }
    
    if (command != nullptr) {
        if (str_list.count-1 == command->argument_count) {
            command->func(state, str_list);
        } else {
            // command did not match number of arguments
            log_errorf("incorrect number of arguments for '%s' command! (expected: %u, received: %u)", command->string.data, command->argument_count, str_list.count-1);
        }
    } else {
        log_errorf("could not find command '%.*s' to execute!", command_name.size, command_name.data);
    }
    
    scratch_end(scratch);
}


function void
console_command_register(console_state_t* state, str_t command_string, str_t desc, u32 argument_count, console_command_func* func) {
    
    console_command_t* command = state->command_free;
    if (command != nullptr) {
        stack_pop(state->command_free);
    } else {
        command = (console_command_t*)arena_alloc(state->arena, sizeof(console_command_t));
    }
    memset(command, 0, sizeof(console_command_t));
    
    command->string = command_string;
    command->desc = desc;
    command->argument_count = argument_count;
    command->func = func;
    
    dll_push_back(state->command_first, state->command_last, command);
}

function void
console_command_unregister(console_state_t* state, str_t command_string) {
    console_command_t* command = nullptr;
    
    for (console_command_t* c = state->command_first; c != nullptr; c = c->next) {
        if (str_match(c->string, command_string)) {
            command = c;
            break;
        }
    }
    
    if (command != nullptr) {
        dll_remove(state->command_first, state->command_last, command);
        stack_push(state->command_free, command);
    }
}

// default commands

function void 
console_command_help_func(console_state_t* state, str_list_t str_list) {
    for (console_command_t* command = state->command_first; command != nullptr; command = command->next) {
        log_infof("%s : %s", command->string, command->desc);
    }
}

#endif // CONSOLE_CPP