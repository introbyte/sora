// profile.cpp

#ifndef PROFILE_CPP
#define PROFILE_CPP

//- state functions 

function void
pf_init() {
    
    u32 start_pid = 0;
    u64 start_timestamp = __rdtscp(&start_pid);
    
    pf_state.arena = arena_create(megabytes(64));
    
    u32 end_pid = 0;
    u64 end_timestamp = __rdtscp(&end_pid);
    
    u64 elapsed = end_timestamp - start_timestamp;
    
    log_infof("start: %u on pid: %u", start_timestamp, start_pid);
    log_infof("end: %u on pid: %u", end_timestamp, end_pid);
    log_infof("elapsed: %u", elapsed);
    
}

function void
pf_release() {
    
}

//- 

function void
pf_begin(char* name) {
    
}

function void
pf_end() {
    
}


#endif // PROFILE_CPP