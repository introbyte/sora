// profile.h

#ifndef PROFILE_H
#define PROFILE_H

//- defines 

#define prof_begin(name) pf_begin(name);
#define prof_end(name) pf_end();

//- structs 

struct pf_state_t {
    
    arena_t* arena;
    
};

//- globals 

global pf_state_t pf_state;

//- function  

// state
function void pf_init();
function void pf_release();

function void pf_begin(char* name);
function void pf_end();

#endif //PROFILE_H