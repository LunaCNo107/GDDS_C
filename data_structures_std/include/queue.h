#ifndef GDDS_QUEUE_H
#define GDDS_QUEUE_H

#include "shared.h"

/* array based queue opaque type*/

// forward declaration of the queue structure, the actual definition is hidden in the implementation file to enforce encapsulation
// users of the queue API will only interact with pointers to this opaque type and will not have access to its internal fields
// ensuring that the queue can only be manipulated through the provided API functions and preventing accidental misuse or corruption of the queue's internal state
// this also allows the implementation to change the internal structure of the queue without affecting code that uses the queue, as long as the API functions remain consistent
// the queue structure will typically contain a pointer to the data array, the size of each element, the current number of elements in the queue, the total capacity of the queue
// and cursors for the head and tail positions in the circular buffer
typedef struct GDDS_Queue_t GDDS_Queue_t;

/* Creation and initialization ( write )*/

// creates a queue instance on the heap
// all fields are initialized to zero/NULL
// return values: valid pointer = success, NULL = failure
GDDS_Queue_t* queue_create();

// initializes a queue instance
// element_size: size of each element in bytes
// initial_capacity: initial number of elements the stack can hold (automatically rounded up to the next power of two)
// return values: 0 = success, -1 = memory allocation failure, -2 = invalid operation, -3 = invalid argument
int queue_init(GDDS_Queue_t* queue, size_t element_size, size_t initial_capacity);

/* Adding and removing elements ( write )*/

// enqueues an element onto a queue instance
// copy_func: function to copy the element, only needed if the element owns heap-allocated memory, otherwise memcpy is used
// return values: 0 = success, -1 = memory allocation failure, -2 = invalid operation, -3 = invalid argument
int queue_enqueue(GDDS_Queue_t* queue, const void* element, void (*CopyFunc)(void* dest, const void* src));

// dequeue an element from a queue instance
// destructor_func: function to destroy the element, only needed if the element owns heap-allocated memory, otherwise no action is taken
// return values: 0 = success, -1 = memory allocation failure, -2 = invalid operation, -3 = invalid argument
int queue_dequeue(GDDS_Queue_t* queue, void (*DestructorFunc)(void* element));

// peeks at the first(front) element of a queue instance without removing it
// out_element: pointer to store the peeked element
// copy_func: function to copy the element to out_element, only needed if the element owns heap-allocated memory, otherwise memcpy is used
void* queue_head(const GDDS_Queue_t* queue, void (*CopyFunc)(void* dest, const void* src));

// peeks at the last(rear) element of a queue instance without removing it
// out_element: pointer to store the peeked element
// copy_func: function to copy the element to out_element, only needed if the element owns heap-allocated memory, otherwise memcpy is used
void* queue_tail(const GDDS_Queue_t* queue, void (*CopyFunc)(void* dest, const void* src));

/* Clearing and destroying ( write )*/

// uninitializes a queue instance and frees all associated memory
// destructor_func: function to destroy each element, only needed if the elements own heap-allocated memory, otherwise no action is taken
// the queue structure itself is not freed and can be reused by calling queue_init again if desired
int queue_clear(GDDS_Queue_t* queue, void (*DestructorFunc)(void* element));

// destroys a queue instance
// must clear the queue first using queue_clear if it is initialized to hold elements that own heap-allocated memory to avoid memory leaks
int queue_destroy(GDDS_Queue_t** queue);

/* Utility functions ( read/check )*/

// returns true if a queue instance is created, false otherwise
bool queue_is_created(const GDDS_Queue_t* queue);

// returns true if a queue instance is initialized, false otherwise
// assumes a queue instance is created (queue pointer is not NULL)
bool queue_is_initialized(const GDDS_Queue_t* queue);

// returns true if a queue instance is empty, false otherwise
// assumes a queue instance is created (queue pointer is not NULL)
bool queue_is_empty(const GDDS_Queue_t* queue);

// returns true if a queue instance is full, false otherwise
// assumes a queue instance is created (queue pointer is not NULL)
// a queue is never really full since it always resizes up when tail cursor catches up to head cursor(still has one empty slot)
// in that case this function will return true
bool queue_is_full(const GDDS_Queue_t* queue);

// returns the number of elements currently in a queue instance
// assumes a queue instance is created (queue pointer is not NULL)
size_t queue_size(const GDDS_Queue_t* queue);

// returns the current capacity of a queue instance (i.e. how many elements it can hold before needing to resize)
// assumes a queue instance is created (queue pointer is not NULL)
size_t queue_capacity(const GDDS_Queue_t* queue);

// returns the size of each element in a queue instance in bytes
// assumes a queue instance is created (queue pointer is not NULL)
size_t queue_element_size(const GDDS_Queue_t* queue);

// returns the current head cursor position of a queue instance (i.e. the index where the next element will be dequeued from)
// assumes a queue instance is created (queue pointer is not NULL)
size_t queue_head_cursor(const GDDS_Queue_t* queue);

// returns the current head index of a queue instance (i.e. the index of the first element in the queue)
// assumes a queue instance is created (queue pointer is not NULL) and has at least one element (queue is not empty)
size_t queue_head_index(const GDDS_Queue_t* queue);

// returns the current tail cursor position of a queue instance (i.e. the index where the next element will be enqueued to)
// assumes a queue instance is created (queue pointer is not NULL)
size_t queue_tail_cursor(const GDDS_Queue_t* queue);

// returns the current tail index of a queue instance (i.e. the index of the last element in the queue)
// assumes a queue instance is created (queue pointer is not NULL) and has at least one element (queue is not empty)
size_t queue_tail_index(const GDDS_Queue_t* queue);

/* Debugging */

// prints the internal state of a queue instance for debugging purposes
// assumes a queue instance is created (queue pointer is not NULL)
void queue_print_info(const GDDS_Queue_t* queue);

#endif