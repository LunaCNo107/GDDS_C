#include "../include/stack.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* array based stack definition*/

typedef struct GDDS_Stack_t {
    void* data;
    size_t element_size;
    size_t size;
    size_t capacity;
} GDDS_Stack_t;

/* Creation and initialization ( write )*/

GDDS_Stack_t* stack_create() {
    GDDS_Stack_t* stack = (GDDS_Stack_t*)calloc(1, sizeof(GDDS_Stack_t));
    if (stack == NULL) {
        GDDS_SET_ERROR("OOM-Memory allocation failed for Stack");
        return NULL;
    }
    return stack;
}

static size_t stack_roundup_capacity(size_t capacity) {
    size_t rounded = 2;
    while (rounded < capacity) {
        rounded <<= 1;
    }
    return rounded;
}

int stack_init(GDDS_Stack_t* stack, size_t element_size, size_t initial_capacity) {
    if (stack == NULL) {
        GDDS_SET_ERROR("INVARG-Stack is NULL");
        return -3;
    }
    if (element_size == 0) {
        GDDS_SET_ERROR("INVARG-Element size must be greater than zero");
        return -3;
    }

    if (stack->element_size > 0) {
        GDDS_SET_ERROR("INVOP-Stack is already initialized");
        return -2;
    }

    initial_capacity = stack_roundup_capacity(initial_capacity);
    stack->data = malloc(element_size * initial_capacity);
    if (stack->data == NULL) {
        GDDS_SET_ERROR("OOM-Memory allocation failed for stack data");
        return -1;
    }

    stack->element_size = element_size;
    stack->capacity = initial_capacity;
    return 0;
}

/* Adding and removing elements ( write )*/

static int stack_expand(GDDS_Stack_t* stack) {
    GDDS_ASSERT(stack != NULL);

    size_t new_capacity = stack->capacity * 2;
    void* new_data = realloc(stack->data, stack->element_size * new_capacity);
    if (new_data == NULL) {
        GDDS_SET_ERROR("OOM-Memory reallocation failed for stack data during expansion");
        return -1;
    }

    stack->data = new_data;
    stack->capacity = new_capacity;
    return 0;
}

int stack_push(GDDS_Stack_t* stack, const void* element, void (*CopyFunc)(void* dest, const void* src)) {
    if (stack == NULL) {
        GDDS_SET_ERROR("INVARG-Stack is NULL");
        return -3;
    }
    if (element == NULL) {
        GDDS_SET_ERROR("INVARG-Element is NULL");
        return -3;
    }

    if (stack->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Stack is not initialized");
        return -2;
    }

    GDDS_ASSERT(stack->size <= stack->capacity);
    if (stack->size >= stack->capacity) {
        if (stack_expand(stack) != 0) {
            GDDS_SET_ERROR("OOM-Failed to expand stack");
            return -1;
        }
    }

    void* target = (char*)stack->data + (stack->size * stack->element_size);
    if (CopyFunc) CopyFunc(target, element);
    else memcpy(target, element, stack->element_size);
    stack->size++;

    return 0;
}

static int stack_shrink(GDDS_Stack_t* stack) {
    GDDS_ASSERT(stack != NULL);

    size_t new_capacity = stack->capacity / 2;
    GDDS_ASSERT(new_capacity >= 2);

    void* new_data = realloc(stack->data, stack->element_size * new_capacity);
    if (new_data == NULL) {
        GDDS_SET_ERROR("OOM-Memory reallocation failed for stack data during shrink");
        return -1;
    }

    stack->data = new_data;
    stack->capacity = new_capacity;
    return 0;
}

int stack_pop(GDDS_Stack_t* stack, void (*DestructorFunc)(void* element)) {
    if (stack == NULL) {
        GDDS_SET_ERROR("INVARG-Stack is NULL");
        return -3;
    }

    if (stack->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Stack is not initialized");
        return -2;
    }
    if (stack->size == 0) {
        GDDS_SET_ERROR("INVOP-Stack is empty");
        return -2;
    }

    stack->size--;
    void* source = (char*)stack->data + (stack->size * stack->element_size);
    
    if (DestructorFunc) {
        DestructorFunc(source);
    }

    if (stack->size > 8 && stack->size <= stack->capacity / 4) {
        if (stack_shrink(stack) != 0) {
            GDDS_SET_ERROR("OOM-Failed to shrink stack");
            return -1;
        }
    }
    return 0;
}

void* stack_peek(const GDDS_Stack_t* stack, void (*CopyFunc)(void* dest, const void* src)) {
    if (stack == NULL) {
        GDDS_SET_ERROR("INVARG-Stack is NULL");
        return NULL;
    }

    if (stack->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Stack is not initialized");
        return NULL;
    }
    if (stack->size == 0) {
        GDDS_SET_ERROR("INVOP-Stack is empty");
        return NULL;
    }

    void* source = (char*)stack->data + ((stack->size - 1) * stack->element_size);
    if (CopyFunc) {
        void* out_element = malloc(stack->element_size);
        if (out_element == NULL) {
            GDDS_SET_ERROR("OOM-Memory allocation failed for peeked element");
            return NULL;
        }
        CopyFunc(out_element, source);
        return out_element;
    }
    return source;
}

/* Clearing and destroying ( write )*/

int stack_clear(GDDS_Stack_t* stack, void (*DestructorFunc)(void* element)) {
    if (stack == NULL) {
        GDDS_SET_ERROR("INVARG-Stack is NULL");
        return -3;
    }

    if (stack->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Stack is not initialized");
        return -2;
    }

    if (DestructorFunc) {
        for (size_t i = 0; i < stack->size; i++) {
            void* element = (char*)stack->data + (i * stack->element_size);
            DestructorFunc(element);
        }
    }

    free(stack->data);
    stack->data = NULL;
    stack->size = 0;
    stack->capacity = 0;
    stack->element_size = 0;
    return 0;
}

int stack_destroy(GDDS_Stack_t** stack) {
    if (stack == NULL) {
        GDDS_SET_ERROR("INVARG-Stack pointer is NULL");
        return -3;
    }

    if (*stack == NULL) {
        GDDS_SET_ERROR("INVOP-Stack is already destroyed");
        return -2;
    }
    if ((*stack)->element_size > 0 && (*stack)->data != NULL) {
        GDDS_SET_ERROR("INVOP-Stack is not cleared before destruction");
        return -2;
    }

    free(*stack);
    *stack = NULL;
    return 0;
}

/* Utility functions ( read/check )*/

bool stack_is_created(const GDDS_Stack_t* stack) {
    return stack != NULL;
}

bool stack_is_initialized(const GDDS_Stack_t* stack) {
    return stack->element_size > 0;
}

bool stack_is_empty(const GDDS_Stack_t* stack) {
    return stack->size == 0;
}

bool stack_is_full(const GDDS_Stack_t* stack) {
    return stack->size == stack->capacity;
}

size_t stack_size(const GDDS_Stack_t* stack) {
    return stack->size;
}

size_t stack_capacity(const GDDS_Stack_t* stack) {
    return stack->capacity;
}

size_t stack_element_size(const GDDS_Stack_t* stack) {
    return stack->element_size;
}

/* Debugging */

void stack_print_info(const GDDS_Stack_t* stack) {
    if (stack == NULL) {
        printf("Stack pointer is NULL.\n");
        return;
    }
    printf("Stack Info:\n");
    printf("\tElement Size: %zu bytes\n", stack->element_size);
    printf("\tSize: %zu elements\n", stack->size);
    printf("\tCapacity: %zu elements\n", stack->capacity);
}
