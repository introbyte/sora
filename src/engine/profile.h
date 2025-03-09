// profile.h

#ifndef PROFILE_H
#define PROFILE_H

//- defines 

// overwrite profile macros
#define prof_get_timestamp() os_time_microseconds()
#define prof_begin(name) pf_begin(name)
#define prof_end(name) pf_end(name)

//- structs 

struct pf_entry_t {
    pf_entry_t* next;
    pf_entry_t* prev;
    
    u32 pid;
    u32 tid;
    
    cstr name;
    u64 hash;
    
    u64 start_time;
    u64 end_time;
    u64 elapsed_time;
};

struct pf_state_t {
    
    arena_t* arena;
    
    pf_entry_t* entry_first;
    pf_entry_t* entry_last;
    pf_entry_t* entry_free;
    
};

//- globals 

global pf_state_t pf_state;

//- function  

// state
function void pf_init();
function void pf_release();

function void pf_begin(cstr name);
function void pf_end(cstr name);

#endif //PROFILE_H