// task_testing.cpp

#include "engine/core/base.h"
#include "engine/core/os.h"
#include "engine/core/task.h"

#include "engine/core/base.cpp"
#include "engine/core/os.cpp"
#include "engine/coretask.cpp"

function void
test_function(void* params) {
    i32* value = (i32*)params;
    atomic_u32_inc(value);
}

function i32 
app_entry_point(i32 argc, char** argv) {
    
    os_init();
    task_init(4);
    
    i32 value = 5;
    
    task_desc_t tasks[] = {
        {test_function, &value},
        {test_function, &value},
        {test_function, &value},
        {test_function, &value},
        {test_function, &value},
    };
    
    task_counter_t* counter = task_run(tasks, 6);
    //task_wait_for_counter(counter, 0);
    os_sleep(1000);
    printf("value: %d\n", value);
    
    task_release();
    os_release();
    
    return 0;
}
