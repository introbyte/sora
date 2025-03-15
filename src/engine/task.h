// task.h

#ifndef TASK_H
#define TASK_H

// TODO:
// 
// [ ] - add fibers.
// [ ] - 
//
//

//- defines

#define TASK_MAX_TASK_COUNT 1000

//- enums

enum task_priority {
    task_priority_high,
    task_priority_normal,
    task_priority_low,
    task_priority_count,
};

//- typedefs

typedef void task_function(void*);
typedef u32 task_counter_t;

//- structs

struct task_queue_t {
    void* data;
    u32 data_size;
    u32 data_count;
    
    os_handle_t mutex;
    os_handle_t cv;
    
    u32 head;
    u32 tail;
};

struct task_desc_t {
    task_priority priority;
    task_function* func;
    void* data;
};

struct task_t {
    task_desc_t desc;
    task_counter_t* counter;
};

struct task_sleeping_fiber_t {
    u32 fiber_index;
    task_counter_t* counter;
    u32 wait_condition;
};

struct task_state_t {
    
    arena_t* arena;
    
    u32 state_active;
    
    // task queue
    task_queue_t* task_queues[task_priority_count];
    
    // counters
    task_counter_t* counters;
    u32 counter_count;
    task_queue_t* counter_free_indices;
    
    // fibers
    os_handle_t* fibers;
    u32 fiber_count;
    task_queue_t* fiber_free_indices;
    task_queue_t* fiber_sleeping;
    
    // worker threads
    os_handle_t* worker_threads;
    u32 worker_threads_count;
    
};

//- globals 

global task_state_t task_state;
thread_global os_handle_t task_current_thread = { 0 };
thread_global i32 task_current_fiber_index = -1;

//- functions

// state
function void task_init(u32 worker_thread_count, u32 fiber_count);
function void task_release();

function task_counter_t* task_run(task_desc_t* tasks, u32 tasks_count);
function void task_wait_for_counter(task_counter_t* counter, u32 value);

// queues

function task_queue_t* task_queue_create(arena_t* arena, u32 data_size, u32 data_count);
function void task_queue_release(task_queue_t* queue);
function b8 task_queue_push(task_queue_t* queue, void* item);
function b8 task_queue_pull(task_queue_t* queue, void** item);

// entry points
function void task_work_thread_entry_point(void* params);
function void task_fiber_entry_point(void* return_fiber);

#endif // TASK_H