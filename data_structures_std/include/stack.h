#ifndef GDDS_STACK_H
#define GDDS_STACK_H

#include "shared.h"

/* array based stack opaque type*/

// forward declaration of the stack structure, the actual definition is hidden in the implementation file to enforce encapsulation
// users of the stack API will only interact with pointers to this opaque type and will not have access to its internal fields
// ensuring that the stack can only be manipulated through the provided API functions and preventing accidental misuse or corruption of the stack's internal state
// this also allows the implementation to change the internal structure of the stack without affecting code that uses the stack, as long as the API functions remain consistent
// the stack structure will typically contain a pointer to the data array, the size of each element, the current number of elements in the stack, the total capacity of the stack
typedef struct GDDS_Stack_t GDDS_Stack_t;

/* Creation and initialization ( write )*/

// creates a stack instance on the heap
// all fields are initialized to zero/NULL
// return values: valid pointer = success, NULL = failure
GDDS_Stack_t* stack_create();

// initializes a stack instance
// element_size: size of each element in bytes
// initial_capacity: initial number of elements the stack can hold (automatically rounded up to the next power of two)
// return values: 0 = success, -1 = memory allocation failure, -2 = invalid operation, -3 = invalid argument
int stack_init(GDDS_Stack_t* stack, size_t element_size, size_t initial_capacity);

/* Adding and removing elements ( write )*/

// pushes an element onto a stack instance
// copy_func: function to copy the element, only needed if the element owns heap-allocated memory, otherwise memcpy is used
// return values: 0 = success, -1 = memory allocation failure, -2 = invalid operation, -3 = invalid argument
int stack_push(GDDS_Stack_t* stack, const void* element, void (*CopyFunc)(void* dest, const void* src));

// pops an element from a stack instance
// destructor_func: function to destroy the element, only needed if the element owns heap-allocated memory, otherwise no action is taken
// return values: 0 = success, -1 = memory allocation failure, -2 = invalid operation, -3 = invalid argument
int stack_pop(GDDS_Stack_t* stack, void (*DestructorFunc)(void* element));

// peeks at the top element of a stack instance without removing it
// copy_func: function to copy the element to out_element, only needed if the caller wants to operate on the element without overwritting it
// otherwise a reference to the internal element is returned (modifying it will modify the element in the array, so use with caution)
// return values: valid pointer = success, NULL = failure
void* stack_peek(const GDDS_Stack_t* stack, void (*CopyFunc)(void* dest, const void* src));

/* Clearing and destroying ( write )*/

// uninitializes a stack instance and frees all associated memory
// destructor_func: function to destroy each element, only needed if the elements own heap-allocated memory, otherwise not necessary
// the stack structure itself is not freed and can be reused by calling stack_init again if desired
// return values: 0 = success, -2 = invalid operation, -3 = invalid argument
int stack_clear(GDDS_Stack_t* stack, void (*DestructorFunc)(void* element));

// destroys a stack instance
// must clear the stack first using stack_clear if it is initialized to hold elements that own heap-allocated memory to avoid memory leaks
// return values: 0 = success, -2 = invalid operation, -3 = invalid argument
int stack_destroy(GDDS_Stack_t** stack);

/* Utility functions ( read/check )*/

// returns true if a stack instance is created, false otherwise
bool stack_is_created(const GDDS_Stack_t* stack);

// returns true if a stack instance is initialized, false otherwise
// assumes a stack instance is created (stack pointer is not NULL)
bool stack_is_initialized(const GDDS_Stack_t* stack);

// returns true if a stack instance is empty, false otherwise
// assumes a stack instance is created (stack pointer is not NULL)
bool stack_is_empty(const GDDS_Stack_t* stack);

// returns true if a stack instance is full, false otherwise
// assumes a stack instance is created (stack pointer is not NULL)
bool stack_is_full(const GDDS_Stack_t* stack);

// returns the number of elements currently in a stack instance
// assumes a stack instance is created (stack pointer is not NULL)
size_t stack_size(const GDDS_Stack_t* stack);

// returns the current capacity of a stack instance (i.e. how many elements it can hold before needing to resize)
// assumes a stack instance is created (stack pointer is not NULL)
size_t stack_capacity(const GDDS_Stack_t* stack);

// returns the size of each element in a stack instance in bytes
// assumes a stack instance is created (stack pointer is not NULL)
size_t stack_element_size(const GDDS_Stack_t* stack);

/* Debugging */

// prints the internal state of a stack instance for debugging purposes
// assumes a stack instance is created (stack pointer is not NULL)
void stack_print_info(const GDDS_Stack_t* stack);

#endif