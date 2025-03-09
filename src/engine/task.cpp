// task.cpp

#ifndef TASK_CPP
#define TASK_CPP

// implementation

function void
task_init(u32 worker_thread_count) {
    temp_t scratch = scratch_begin();
    
    // allocate arena
    task_state.arena = arena_create(megabytes(64));
    
    // allocate worker threads
    u32 os_core_count =  os_get_system_info().logical_processor_count - 1;
    task_state.worker_threads_count = clamp(worker_thread_count, 1, os_core_count);
    task_state.worker_threads = (os_handle_t*)arena_alloc(task_state.arena, sizeof(os_handle_t) * task_state.worker_threads_count);
    
    // create queue mutex
    task_state.task_queue_mutex = os_mutex_create();
    
    // create worker threads
    task_state.state_active = true;
    for (u32 i = 0; i < task_state.worker_threads_count; i++) {
        str_t worker_thread_name = str_format(scratch.arena, "worker_thread_%u", i);
        task_state.worker_threads[i] = os_thread_create(task_work_thread_entry_point); 
        os_thread_set_name(task_state.worker_threads[i], worker_thread_name);
    }
    
    scratch_end(scratch);
}

function void
task_release() {
    
    // close threads
    task_state.state_active = false;
    
    //os_mutex_release(task_state.task_queue_mutex);
    
    arena_release(task_state.arena);
    
} 

function task_counter_t*
task_run(task_desc_t* tasks, u32 tasks_count) {
    
    // grab from counter free list or allocate a counter
    task_counter_t* counter = task_state.counter_free;
    if (counter != nullptr) {
        stack_pop(task_state.counter_free);
    } else {
        counter = (task_counter_t*)arena_alloc(task_state.arena, sizeof(task_counter_t));
    }
    memset(counter, 0, sizeof(task_counter_t));
    
    // start counter
    atomic_u32_assign(&counter->count, tasks_count + 1);
    
    // create task
    os_mutex_lock(task_state.task_queue_mutex);
    for (i32 i = 0; i < tasks_count; i++) {
        
        task_queue_t* queue = task_state.task_queue_free;
        if (queue != nullptr) {
            stack_pop(task_state.task_queue_free);
        } else {
            queue = (task_queue_t*)arena_alloc(task_state.arena, sizeof(task_queue_t));
        }
        memset(queue, 0, sizeof(task_queue_t));
        
        queue->task.desc = tasks[i];
        queue->task.counter = counter;
        
        // push to queue
        queue_push(task_state.task_queue_first, task_state.task_queue_last, queue);
    }
    os_mutex_unlock(task_state.task_queue_mutex);
    
    return counter;
}

function void
task_wait_for_counter(task_counter_t* counter, u32 value) {
    
    // wait for counter to be complete 
    while (atomic_u32(&counter->count) > value) {
        os_sleep(16);
    }
    
    // once we are done with counter, add to free list for reuse
    stack_push(task_state.counter_free, counter);
}

function void 
task_work_thread_entry_point(void* params) {
    
    u32 tid = os_get_thread_id();
    log_infof("[worker thread  %u] starting...", tid);
    
    while (atomic_u32(&task_state.state_active)) {
        
        // pull job from the queue
        os_mutex_lock(task_state.task_queue_mutex);
        task_queue_t* queue = queue_pop(task_state.task_queue_first, task_state.task_queue_last);
        os_mutex_unlock(task_state.task_queue_mutex);
        
        if (queue != nullptr) {
            
            // execute task function
            task_t* task = &queue->task;
            task->desc.func(task->desc.data);
            
            // decrement counter
            atomic_u32_dec(&task->counter->count);
            
            // push queue to free list
            os_mutex_lock(task_state.task_queue_mutex);
            stack_push(task_state.task_queue_free, queue);
            os_mutex_unlock(task_state.task_queue_mutex);
        }
        
    }
    
    log_infof("[worker thread %u] ending.", tid);
    
}


#endif // TASK_CPP