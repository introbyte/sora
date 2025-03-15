// console_program.cpp

#include "engine/base.h"
#include "engine/os.h"

#include "engine/base.cpp"
#include "engine/os.cpp"

//#include "engine/profile.h"
//#include "engine/profile.cpp"

#include "engine/task.h"
#include "engine/task.cpp"

function void
task_func2(void* params) {
    
    u32* value = (u32*)params;
    for (i32 i = 0; i < 1000; i++) {
        atomic_u32_dec(value);
    }
    
}

function void
task_func(void* params) {
    u32* value = (u32*)params;
    for (i32 i = 0; i < 10000; i++) {
        atomic_u32_inc(value);
    }
    
    task_desc_t desc = { task_priority_high, task_func2, value };
    task_counter_t* counter = task_run(&desc, 1);
    task_wait_for_counter(counter, 0);
    
}

function i32 
app_entry_point(i32 argc, char** argv) {
    
    os_init();
    task_init(8, 64);
    
    log_infof("Hello, World!");
    
    u32 value = 0;
    task_desc_t descs[5];
    for (i32 i = 0; i < array_count(descs); i++) {
        descs[i] = { task_priority_high, task_func, &value};
    }
    
    task_counter_t* counter = task_run(descs, array_count(descs));
    task_wait_for_counter(counter, 0);
    
    log_infof("value: %u", value);
    
    task_release();
    os_release();
    
    return 0;
}