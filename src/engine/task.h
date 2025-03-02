// task.h

#ifndef TASK_H
#define TASK_H

// enums

// TODO: priority not implement yet.
enum task_priority {
    task_priority_low,
    task_priority_normal,
    task_priority_high,
    task_priority_count,
};

// typedefs

typedef void task_function(void*);

// structs

struct task_desc_t {
    task_function* func;
    void* data;
};

struct task_counter_t {
    task_counter_t* next;
    u32 count;
};

struct task_t {
    task_desc_t desc;
    task_counter_t* counter;
};

struct task_queue_t {
    task_queue_t* next;
    task_queue_t* prev;
    
    task_t task;
};

struct task_state_t {
    
    arena_t* arena;
    
    u32 state_active;
    
    // worker pool
    os_handle_t* worker_threads;
    u32 worker_threads_count;
    
    // counter free list
    task_counter_t* counter_free;
    
    // task queue
    task_queue_t* task_queue_first;
    task_queue_t* task_queue_last;
    task_queue_t* task_queue_free;
    
    os_handle_t task_queue_mutex;
    
};

global task_state_t task_state;

// functions

function void task_init(u32 worker_thread_count);
function void task_release();

function task_counter_t* task_run(task_desc_t* tasks, u32 tasks_count);
function void task_wait_for_counter(task_counter_t* counter, u32 value);

function void task_work_thread_entry_point(void* params);

#endif // TASK_H