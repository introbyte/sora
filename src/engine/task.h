// task.h

#ifndef TASK_H
#define TASK_H

// enums

enum task_priority {
    task_priority_low,
    task_priority_normal,
    task_priority_high,
    task_priority_count,
};

// typedefs

typedef void task_function(void*);

// structs

struct task_t {
    task_priority priority;
    task_function* func;
    void* data;
};

struct task_counter_t {
    u32 id;
};

struct task_fiber_t {

};

struct task_queue_t {
    task_queue_t* next;
    task_queue_t* prev;
    
    task_t task;
}

struct task_state_t {
    
    arena_t* arena;
    
    os_handle_t* worker_threads;
    u32 worker_threads_count;
    u32 worker_threads_active_count;
    
};

global task_state_t task_state;

// functions

function void task_init(u32 worker_thread_count);
function void task_release();

function task_counter_t* task_start(task_t* task_list, u32 task_count);
function void task_wait_for_counter(task_counter_t* counter, u32 value);


function void task_work_thread_entry_point();

#endif // TASK_H