#ifndef GDDS_VECTOR_H
#define GDDS_VECTOR_H

#include "shared.h"

/* vector based stack opaque type*/

// no vector structure is used, any poiter to a vector instance is actually a pointer to the first element of the array data
// the vector meta-data (size, capacity, element size) is stored in a header structure that is allocated before the actual array data
// this keeps the arr[i] syntax for fast access to elements, but by doing so the user must manage the vector meta-data separately and manually
// this API will provide functions with the ability to read and write the vector meta-data (size, capacity, element size) as needed for automatic management

/* Creation and initialization ( write )*/

// creates a vector instance on the heap
// all fields are initialized to zero/NULL
void* vector_create();

// initializes a vector instance
// element_size: size of each element in bytes
// initial_capacity: initial number of elements the stack can hold (automatically rounded up to the next power of two)
int vector_init(void* vector, size_t element_size, size_t initial_capacity);

/* Adding and removing elements ( write )*/

// inserts an element at a specific index in a vector instance
// CopyFunc: function to copy the element, only needed if the element owns heap-allocated memory, otherwise memcpy is used
int vector_insert_at(void* vector, size_t index, const void* element, void (*CopyFunc)(void* dest, const void* src), bool PreserveOrder);

// removes the element at a specific index in a vector instance
// DestructorFunc: function to destroy the element, only needed if the element owns heap-allocated memory, otherwise no action is taken
int vector_remove_at(void* vector, size_t index, void (*DestructorFunc)(void* element), bool PreserveOrder);

// Accessing elements

//
void* vector_get_at(const void* vector, size_t index, void (*CopyFunc)(void* dest, const void* src));

//
int vector_set_at(void* vector, size_t index, const void* element, void (*CopyFunc)(void* dest, const void* src));

/* Clearing and destroying ( write )*/

// uninitializes a vector instance and frees all associated memory
// destructor_func: function to destroy each element, only needed if the elements own heap-allocated memory, otherwise no action is taken
// the vector structure itself is not freed and can be reused by calling vector_init again if desired
int vector_clear(void* vector, void (*DestructorFunc)(void* element));

// destroys a vector instance
// must clear the vector first using vector_clear if it is initialized to hold elements that own heap-allocated memory to avoid memory leaks
int vector_destroy(void** vector);

/* Utility functions ( read/check )*/

// returns true if a vector instance is created, false otherwise
bool vector_is_created(const void* vector);

// returns true if a vector instance is initialized, false otherwise
// assumes a vector instance is created (vector pointer is not NULL)
bool vector_is_initialized(const void* vector);

// returns true if a vector instance is empty, false otherwise
// assumes a vector instance is created (vector pointer is not NULL)
bool vector_is_empty(const void* vector);

// returns true if a vector instance is full, false otherwise
// assumes a vector instance is created (vector pointer is not NULL)
bool vector_is_full(const void* vector);

// returns the number of elements currently in a vector instance
// assumes a vector instance is created (vector pointer is not NULL)
size_t vector_size(const void* vector);

// returns the current capacity of a vector instance (i.e. how many elements it can hold before needing to resize)
// assumes a vector instance is created (vector pointer is not NULL)
size_t vector_capacity(const void* vector);

// returns the size of each element in a vector instance in bytes
// assumes a vector instance is created (vector pointer is not NULL)
size_t vector_element_size(const void* vector);

/* Debugging */

// prints the internal state of a vector instance for debugging purposes
// assumes a vector instance is created
void vector_print_info(const void* vector);

#endif