#ifndef GDDS_ARRAY_H
#define GDDS_ARRAY_H

#include "shared.h"

/* array opaque type*/

// forward declaration of the array structure, the actual definition is hidden in the implementation file to enforce encapsulation
// users of the array API will only interact with pointers to this opaque type and will not have access to its internal fields
// ensuring that the array can only be manipulated through the provided API functions and preventing accidental misuse or corruption of the array's internal state
// this also allows the implementation to change the internal structure of the array without affecting code that uses the array, as long as the API functions remain consistent
// the array structure will typically contain a pointer to the data array, the size of each element, the current number of elements in the array, the total capacity of the array
typedef struct GDDS_Array_t GDDS_Array_t;

/* Creation and initialization ( write )*/

// creates an array instance on the heap
// all fields are initialized to zero/NULL
// return values: valid pointer = success, NULL = failure
GDDS_Array_t* array_create();

// initializes an array instance
// element_size: size of each element in bytes
// initial_capacity: initial number of elements the array can hold (automatically rounded up to the next power of two)
// return values: 0 = success, -1 = memory allocation failure, -2 = invalid operation, -3 = invalid argument
int array_init(GDDS_Array_t* array, size_t element_size, size_t initial_capacity);

/* Adding and removing elements ( write )*/

// inserts an element at a specific index in an array instance
// CopyFunc: function to copy the element, only needed if the element owns heap-allocated memory, otherwise memcpy is used
// return values: 0 = success, -1 = memory allocation failure, -2 = invalid operation, -3 = invalid argument
int array_insert_at(GDDS_Array_t* array, size_t index, const void* element, void (*CopyFunc)(void* dest, const void* src), bool PreserveOrder);

// removes the element at a specific index in an array instance
// DestructorFunc: function to destroy the element, only needed if the element owns heap-allocated memory, otherwise no action is taken
// return values: 0 = success, -1 = memory allocation failure, -2 = invalid operation, -3 = invalid argument
int array_remove_at(GDDS_Array_t* array, size_t index, void (*DestructorFunc)(void* element), bool PreserveOrder);

// Accessing elements

// shows the element at a specific index in an array instance without removing it
// copy_func: function to copy the element to out_element, only needed if the caller wants to operate on the element without overwritting it
// otherwise a reference to the internal element is returned (modifying it will modify the element in the array, so use with caution)
// return values: valid pointer = success, NULL = failure
void* array_get_at(const GDDS_Array_t* array, size_t index, void (*CopyFunc)(void* dest, const void* src));

// sets the element at a specific index in an array instance
// copy_func: function to copy the element, only needed if the element owns heap-allocated memory, otherwise memcpy is used
// return values: 0 = success, -1 = memory allocation failure, -2 = invalid operation, -3 = invalid argument
int array_set_at(GDDS_Array_t* array, size_t index, const void* element, void (*CopyFunc)(void* dest, const void* src));

/* Clearing and destroying ( write )*/

// uninitializes an array instance and frees all associated memory
// destructor_func: function to destroy each element, only needed if the elements own heap-allocated memory, otherwise no action is taken
// the array structure itself is not freed and can be reused by calling array_init again if desired
// return values: 0 = success, -2 = invalid operation, -3 = invalid argument
int array_clear(GDDS_Array_t* array, void (*DestructorFunc)(void* element));

// destroys an array instance
// must clear the array first using array_clear if it is initialized to hold elements that own heap-allocated memory to avoid memory leaks
// return values: 0 = success, -2 = invalid operation, -3 = invalid argument
int array_destroy(GDDS_Array_t** array);

/* Utility functions ( read/check )*/

// returns true if an array instance is created, false otherwise
bool array_is_created(const GDDS_Array_t* array);

// returns true if an array instance is initialized, false otherwise
// assumes an array instance is created (array pointer is not NULL)
bool array_is_initialized(const GDDS_Array_t* array);

// returns true if an array instance is empty, false otherwise
// assumes an array instance is created (array pointer is not NULL)
bool array_is_empty(const GDDS_Array_t* array);

// returns true if an array instance is full, false otherwise
// assumes an array instance is created (array pointer is not NULL)
bool array_is_full(const GDDS_Array_t* array);

// returns the number of elements currently in an array instance
// assumes an array instance is created (array pointer is not NULL)
size_t array_size(const GDDS_Array_t* array);

// returns the current capacity of an array instance (i.e. how many elements it can hold before needing to resize)
// assumes an array instance is created (array pointer is not NULL)
size_t array_capacity(const GDDS_Array_t* array);

// returns the size of each element in an array instance in bytes
// assumes an array instance is created (array pointer is not NULL)
size_t array_element_size(const GDDS_Array_t* array);

/* Debugging */

// prints the internal state of an array instance for debugging purposes
// assumes an array instance is created
void array_print_info(const GDDS_Array_t* array);

#endif