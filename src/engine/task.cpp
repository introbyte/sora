// task.cpp

#ifndef TASK_CPP
#define TASK_CPP

// implementation

function void
task_init(u32 worker_thread_count) {
    temp_t scratch = scratch_begin();
    
    // allocate arena
    task_state.arena = arena_create(megabytes(64));
    
    // clamp worker threads count
    u32 os_core_count =  os_get_system_info().logical_processor_count - 1;
    task_state.worker_threads_count = clamp(worker_thread_count, 1, os_core_count);
    
    // allocate worker threads
    task_state.worker_threads = (os_handle_t*)arena_alloc(task_state.arena, sizeof(os_handle_t) * task_state.worker_threads_count);
    
    // create threads
    for (u32 i = 0; i < task_state.worker_threads_count; i++) {
        str_t worker_thread_name = str_format(scratch.arena, "worker_thread_%u", i);
        task_state.worker_threads[i] = os_thread_create(task_work_thread_entry_point, worker_thread_name); 
    }
    
    scratch_end(scratch);
}

function void
task_release() {
    
    for (u32 i = 0; i < task_state.worker_threads_count; i++) {
        os_thread_detach(task_state.worker_threads[i]);
    }
    
} 

function task_counter_t*
task_start(task_t* task_list, u32 task_count) {
    
    // allocate counter
    task_counter_t* counter = (task_counter_t*)arena_alloc(task_state.arena, sizeof(task_counter_t));
    
    
    
    
    
    return counter;
}




function void 
task_work_thread_entry_point() {
    
    while (true) {
        
    }
    
}


#endif // TASK_CPP