// task.cpp

#ifndef TASK_CPP
#define TASK_CPP

// implementation

//- state functions 

function void
task_init(u32 worker_thread_count, u32 fiber_count) {
    
    temp_t scratch = scratch_begin();
    
    // allocate arena
    task_state.arena = arena_create(megabytes(64));
    
    // create queues
    {
        task_state.fiber_free_indices = task_queue_create(task_state.arena, sizeof(u32), fiber_count + 1);
        task_state.fiber_sleeping = task_queue_create(task_state.arena, sizeof(task_sleeping_fiber_t), fiber_count + 1); 
        task_state.counter_free_indices = task_queue_create(task_state.arena, sizeof(u32), fiber_count + 1);
        for (i32 i = 0; i < task_priority_count; i++) {
            task_state.task_queues[i] = task_queue_create(task_state.arena, sizeof(task_t), TASK_MAX_TASK_COUNT);
        }
    }
    
    // create counters
    {
        task_state.counter_count = fiber_count;
        task_state.counters = (task_counter_t*)arena_alloc(task_state.arena, sizeof(task_counter_t) * task_state.counter_count);
        
        for (i32 i = 0; i < task_state.counter_count; i++) {
            task_state.counters[i] = 0;
            task_queue_push(task_state.counter_free_indices, &i);
        }
    }
    
    // create fibers
    {
        
        task_state.fiber_count = fiber_count;
        task_state.fibers = (os_handle_t*)arena_alloc(task_state.arena, sizeof(os_handle_t) * task_state.fiber_count);
        
        for (i32 i = 0; i < task_state.fiber_count; i++) {
            task_state.fibers[i] = os_fiber_create(megabytes(8), task_fiber_entry_point, nullptr);
            task_queue_push(task_state.fiber_free_indices, &i);
        }
    }
    
    // create worker threads
    {
        u32 os_core_count =  os_get_system_info().logical_processor_count - 1;
        task_state.worker_threads_count = clamp(worker_thread_count, 1, os_core_count);
        task_state.worker_threads = (os_handle_t*)arena_alloc(task_state.arena, sizeof(os_handle_t) * task_state.worker_threads_count);
        
        // create worker threads
        task_state.state_active = true;
        for (u32 i = 0; i < task_state.worker_threads_count; i++) {
            str_t worker_thread_name = str_format(scratch.arena, "worker_thread_%x", i);
            task_state.worker_threads[i] = os_thread_create(task_work_thread_entry_point); 
            os_thread_set_name(task_state.worker_threads[i], worker_thread_name);
        }
        
        scratch_end(scratch);
    }
}

function void
task_release() {
    
    // close threads
    atomic_u32_dec(&task_state.state_active);
    for (u32 i = 0; i < task_state.worker_threads_count; i++) {
        os_thread_join(task_state.worker_threads[i]);
    }
    
    // release fibers
    for (i32 i = 0; i < task_state.fiber_count; i++) {
        os_fiber_release(task_state.fibers[i]);
    }
    
    // release arena
    arena_release(task_state.arena);
    
} 

function task_counter_t*
task_run(task_desc_t* task_descs, u32 tasks_count) {
    
    // grab counter
    u32* counter_index;
    task_queue_pull(task_state.counter_free_indices, (void**)&counter_index);
    
    // initialize counter
    task_counter_t* counter = &task_state.counters[*counter_index];
    atomic_u32_assign(counter, tasks_count);
    
    // create tasks
    for (i32 i = 0; i < tasks_count; i++) {
        
        task_t task = { task_descs[i], counter };
        task_queue_t* queue = task_state.task_queues[task_descs[i].priority];
        
        // push to queue
        task_queue_push(queue, &task);
    }
    
    return counter;
}

function void
task_wait_for_counter(task_counter_t* counter, u32 value) {
    
    // if we are in a fiber
    if (task_current_fiber_index != -1) {
        
        // push this fiber to sleeping fiber queue
        task_sleeping_fiber_t sleeping_fiber = { task_current_fiber_index, counter, value };
        task_queue_push(task_state.fiber_sleeping, &sleeping_fiber);
        
        // switch to new 
        u32* free_fiber_index;
        task_queue_pull(task_state.fiber_free_indices, (void**)&free_fiber_index);
        
        os_handle_t fiber = task_state.fibers[*free_fiber_index];
        task_current_fiber_index = *free_fiber_index;
        os_fiber_switch(fiber);
        
    } else {
        // we aren't in a fiber, so we just spinlock for now.
        while (atomic_u32(counter) > value) { os_sleep(1); }
    }
    
    // once we are done, push counter index to free queue
    u32 counter_index = (u32)(counter - task_state.counters);
    task_queue_push(task_state.counter_free_indices, &counter_index);
    
}

//- queue functions 

function task_queue_t* 
task_queue_create(arena_t* arena, u32 data_size, u32 data_count) {
    
    task_queue_t* queue = (task_queue_t*)arena_alloc(arena, sizeof(task_queue_t));
    
    queue->data_size = data_size;
    queue->data_count = data_count;
    queue->data = arena_alloc(arena, queue->data_size * queue->data_count);
    
    queue->mutex = os_mutex_create();
    queue->cv = os_condition_variable_create();
    
    queue->head = 0;
    queue->tail = 0;
    
    return queue;
}

function void
task_queue_release(task_queue_t* queue) {
    os_condition_variable_release(queue->cv);
    os_mutex_release(queue->mutex);
}

function b8
task_queue_push(task_queue_t* queue, void* item) {
    b8 result = false;
    os_mutex_lock(queue->mutex);
    
    // if there is space
    if (queue->tail + 1!= queue->head) {
        // copy the item
        
        memcpy((u8*)queue->data + (queue->tail * queue->data_size), item, queue->data_size);
        queue->tail = (queue->tail + 1) % queue->data_count;
        result = true;
    }
    
    os_mutex_unlock(queue->mutex);
    
    return result;
}

function b8
task_queue_pull(task_queue_t* queue, void** item) {
    b8 result = false;
    os_mutex_lock(queue->mutex);
    
    // queue is not empty
    if (queue->head != queue->tail) {
        
        // copy data into item
        *item = ((u8*)queue->data + (queue->head * queue->data_size));
        queue->head = (queue->head + 1) % queue->data_count;
        
        result = true;
    }
    
    os_mutex_unlock(queue->mutex);
    return result;
}

//- entry points 

function void 
task_work_thread_entry_point(void* params) {
    
    // make current thread into a fiber
    u32 tid = os_get_thread_id();
    task_current_thread = os_fiber_from_thread();
    
    // pull initial fiber from free list
    u32 *free_fiber_index;
    task_queue_pull(task_state.fiber_free_indices, (void**)(&free_fiber_index));
    os_handle_t fiber = task_state.fibers[*free_fiber_index];
    task_current_fiber_index = *free_fiber_index;
    os_fiber_switch(fiber);
    
    log_infof("thread (%x) terminated successfully.", tid);
}

function void
task_fiber_entry_point(void* params) {
    
    while (atomic_u32(&task_state.state_active)) {
        
        // check if sleeping fibers can be swapped back in.
        task_sleeping_fiber_t* sleeping_fiber = nullptr;
        if (task_queue_pull(task_state.fiber_sleeping, (void**)&sleeping_fiber)) {
            
            // check wait condition
            if (atomic_u32(sleeping_fiber->counter) <= sleeping_fiber->wait_condition) {
                
                // put current fiber back to free fiber queue
                task_queue_push(task_state.fiber_free_indices, &task_current_fiber_index);
                
                // switch to fiber
                task_current_fiber_index = sleeping_fiber->fiber_index;
                os_fiber_switch(task_state.fibers[sleeping_fiber->fiber_index]);
                
            } else {
                
                // push sleeping fiber back to queue
                task_queue_push(task_state.fiber_sleeping, sleeping_fiber);
            }
            
        }
        
        // if not, pull task from queue
        task_t* task = nullptr;
        for (i32 i = 0; i < task_priority_count; i++) {
            task_queue_t* queue = task_state.task_queues[i];
            if (task_queue_pull(queue, (void**)(&task))) {
                break;
            }
        }
        
        //if task exist
        if (task != nullptr) {
            
            // execute task function
            if (task->desc.func != nullptr) {
                task->desc.func(task->desc.data);
            }
            
            // decrement counter
            atomic_u32_dec(task->counter);
        }
        
    }
    
    // once complete, switch back to original thread
    os_fiber_switch(task_current_thread);
    
}


#endif // TASK_CPP