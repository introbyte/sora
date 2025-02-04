// file_iter_queue.h

#ifndef FILE_ITER_QUEUE_H
#define FILE_ITER_QUEUE_H

// implements a simple queue system to
// iterate over all sub-directories in a 
// directory.

// structs

struct file_iter_node_t {
    file_iter_node_t* next;
    file_iter_node_t* prev;
    
    str_t path;
};

struct file_iter_queue_t {
    file_iter_node_t* first;
    file_iter_node_t* last;
};

struct file_iter_queue_t {
    file_iter_node_t* first;
    file_iter_node_t* last;
};

// functions

function file_iter_node_t* 
file_iter_queue_push(arena_t* arena, file_iter_queue_t* queue, str_t path, fg_node_t* node) {
    
    file_iter_node_t* file_iter_node = (file_iter_node_t*)arena_alloc(arena, sizeof(file_iter_node_t));
    queue_push(queue->first, queue->last, file_iter_node);
    
    file_iter_node->path = path;
    file_iter_node->node = node;
    
    return file_iter_node;
}

function file_iter_node_t* 
file_iter_queue_pop(file_iter_queue_t* queue) {
    file_iter_node_t* first_node = queue->first;
    queue_pop(queue->first, queue->last);
    return first_node;
}

function b8
file_iter_queue_is_empty(file_iter_queue_t* queue) {
    return (queue->first == nullptr);
}

#endif // FILE_ITER_H