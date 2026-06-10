#include "../include/array.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* array definition*/

struct GDDS_Array_t {
    void* items;
    size_t element_size;
    size_t size;
    size_t capacity;
};

/* Creation and initialization ( write )*/

GDDS_Array_t* array_create() {
    GDDS_Array_t* array = (GDDS_Array_t*)calloc(1, sizeof(GDDS_Array_t));
    if (array == NULL) {
        GDDS_SET_ERROR("OOM-Memory allocation failed for array");
        return NULL;
    }
    return array;
}

static size_t array_roundup_capacity(size_t capacity) {
    size_t rounded = 2;
    while (rounded < capacity) {
        rounded <<= 1;
    }
    return rounded;
}

int array_init(GDDS_Array_t* array, size_t element_size, size_t initial_capacity) {
    if (array == NULL) {
        GDDS_SET_ERROR("INVARG-Array pointer is NULL");
        return -3;
    }
    if (element_size == 0) {
        GDDS_SET_ERROR("INVARG-Element size must be greater than zero");
        return -3;
    }

    initial_capacity = array_roundup_capacity(initial_capacity);
    array->items = calloc(initial_capacity, element_size);
    if (array->items == NULL) {
        GDDS_SET_ERROR("OOM-Memory allocation failed for array items");
        return -1;
    }

    array->element_size = element_size;
    array->size = 0;
    array->capacity = initial_capacity;
    return 0;
}

/* Adding and removing elements ( write )*/

static int array_expand(GDDS_Array_t* array) {
    GDDS_ASSERT(array != NULL);

    size_t new_capacity = array->capacity * 2;
    void* new_items = realloc(array->items, array->element_size * new_capacity);
    if (new_items == NULL) {
        GDDS_SET_ERROR("OOM-Memory allocation failed for array expansion");
        return -1;
    }

    array->items = new_items;
    array->capacity = new_capacity;
    return 0;
}

int array_insert_at(GDDS_Array_t* array, size_t index, const void* element, void (*CopyFunc)(void* dest, const void* src), bool PreserveOrder) {
    if (array == NULL) {
        GDDS_SET_ERROR("INVARG-Array pointer is NULL");
        return -3;
    }
    if (array->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Array is not initialized");
        return -2;
    }
    if (index > array->size || index < 0) {
        GDDS_SET_ERROR("INVARG-Index out of bounds");
        return -3;
    }
    if (element == NULL) {
        GDDS_SET_ERROR("INVARG-Element is NULL");
        return -3;
    }

    // Resize if necessary
    if (array->size >= array->capacity) {
        if (array_expand(array) != 0) {
            return -1;
        }
    }

    void* target = (char*)array->items + (index * array->element_size);
    if (PreserveOrder && index != array->size) {
        memmove((char*)target + array->element_size, target, (array->size - index) * array->element_size);
    }
    if (CopyFunc) CopyFunc(target, element);
    else memcpy(target, element, array->element_size);
    
    array->size++;
    return 0;
}

static int array_shrink(GDDS_Array_t* array) {
    GDDS_ASSERT(array != NULL);

    size_t new_capacity = array->capacity / 2;
    GDDS_ASSERT(new_capacity >= 2);
    void* new_items = realloc(array->items, array->element_size * new_capacity);
    if (new_items == NULL) {
        GDDS_SET_ERROR("OOM-Memory allocation failed for array shrinkage");
        return -1;
    }

    array->items = new_items;
    array->capacity = new_capacity;
    return 0;
}

int array_remove_at(GDDS_Array_t* array, size_t index, void (*DestructorFunc)(void* element), bool PreserveOrder) {
    if (array == NULL) {
        GDDS_SET_ERROR("INVARG-Array pointer is NULL");
        return -3;
    }
    if (array->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Array is not initialized");
        return -2;
    }
    if (index >= array->size || index < 0) {
        GDDS_SET_ERROR("INVARG-Index out of bounds");
        return -3;
    }

    void* target = (char*)array->items + (index * array->element_size);
    if (DestructorFunc) DestructorFunc(target);
    if (PreserveOrder && index != array->size - 1) {
        memmove(target, (char*)target + array->element_size, (array->size - index - 1) * array->element_size);
    }
    array->size--;

    if (array->size > 8 && array->size <= array->capacity / 4) {
        if (array_shrink(array) != 0) {
            GDDS_SET_ERROR("OOM-Failed to shrink array");
            return -1;
        }
    }

    return 0;
}

void* array_get_at(const GDDS_Array_t* array, size_t index, void (*CopyFunc)(void* dest, const void* src)) {
    if (array == NULL) {
        GDDS_SET_ERROR("INVARG-Array pointer is NULL");
        return NULL;
    }
    if (index >= array->size || index < 0) {
        GDDS_SET_ERROR("INVARG-Index out of bounds");
        return NULL;
    }
    if (array->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Array is not initialized");
        return NULL;
    }

    void* target = (char*)array->items + (index * array->element_size);
    if (CopyFunc) {
        void* result = malloc(array->element_size);
        if (result == NULL) {
            GDDS_SET_ERROR("OOM-Memory allocation failed for array element copy");
            return NULL;
        }
        CopyFunc(result, target);
        return result;
    }

    return target;
}

int array_set_at(GDDS_Array_t* array, size_t index, const void* element, void (*CopyFunc)(void* dest, const void* src)) {
    if (array == NULL) {
        GDDS_SET_ERROR("INVARG-Array pointer is NULL");
        return -3;
    }
    if (array->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Array is not initialized");
        return -2;
    }
    if (index >= array->size || index < 0) {
        GDDS_SET_ERROR("INVARG-Index out of bounds");
        return -3;
    }
    if (element == NULL) {
        GDDS_SET_ERROR("INVARG-Element is NULL");
        return -3;
    }

    void* target = (char*)array->items + (index * array->element_size);
    if (CopyFunc) CopyFunc(target, element);
    else memcpy(target, element, array->element_size);

    return 0;
}

/* Clearing and destroying ( write )*/

int array_clear(GDDS_Array_t* array, void (*DestructorFunc)(void* element)) {
    if (array == NULL) {
        GDDS_SET_ERROR("INVARG-Array pointer is NULL");
        return -3;
    }
    if (array->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Array is not initialized");
        return -2;
    }

    if (DestructorFunc) {
        for (size_t i = 0; i < array->size; i++) {
            void* element = (char*)array->items + (i * array->element_size);
            DestructorFunc(element);
        }
    }
    
    free(array->items);
    array->items = NULL;
    array->element_size = 0;
    array->size = 0;
    array->capacity = 0;
    return 0;
}

int array_destroy(GDDS_Array_t** array) {
    if (array == NULL || *array == NULL) {
        GDDS_SET_ERROR("INVARG-Array pointer is NULL");
        return -3;
    }

    free(*array);
    *array = NULL;
    return 0;
}

/* Utility functions ( read/check )*/

bool array_is_created(const GDDS_Array_t* array) {
    return array != NULL;
}

bool array_is_initialized(const GDDS_Array_t* array) {
    return array->element_size > 0;
}

bool array_is_empty(const GDDS_Array_t* array) {
    return array->size == 0;
}

bool array_is_full(const GDDS_Array_t* array) {
    return array->size == array->capacity;
}

size_t array_size(const GDDS_Array_t* array) {
    return array->size;
}

size_t array_capacity(const GDDS_Array_t* array) {
    return array->capacity;
}

size_t array_element_size(const GDDS_Array_t* array) {
    return array->element_size;
}

/* Debugging */

void array_print_info(const GDDS_Array_t* array) {
    if (array == NULL) {
        printf("Array pointer is NULL\n");
        return;
    }
    printf("Array info:\n");
    printf("\tElement size: %zu bytes\n", array->element_size);
    printf("\tSize: %zu\n", array->size);
    printf("\tCapacity: %zu\n", array->capacity);
}