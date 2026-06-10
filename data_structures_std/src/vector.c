#include "../include/vector.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* vector meta-data definition*/

typedef struct vector_header_t {
    size_t element_size;
    size_t size;
    size_t capacity;
} vector_header_t;

/* Creation and initialization ( write )*/

void* vector_create() {
    vector_header_t* header = (vector_header_t*)calloc(1, sizeof(vector_header_t));
    if (header == NULL) {
        GDDS_SET_ERROR("OOM-Memory allocation failed for vector header");
        return NULL;
    }

    return (void*)(header + 1);
}

static size_t vector_roundup_capacity(size_t capacity) {
    size_t rounded = 2;
    while (rounded < capacity) {
        rounded <<= 1;
    }
    return rounded;
}

int vector_init(void* vector, size_t element_size, size_t initial_capacity) {
    if (vector == NULL) {
        GDDS_SET_ERROR("INVARG-Vector pointer is NULL");
        return -3;
    }
    if (element_size == 0) {
        GDDS_SET_ERROR("INVARG-Element size must be greater than zero");
        return -3;
    }
    
    vector_header_t* header = (vector_header_t*)vector - 1; // Get pointer to header
    if (header->element_size > 0) {
        GDDS_SET_ERROR("INVOP-Vector is already initialized");
        return -2;
    }
    
    initial_capacity = vector_roundup_capacity(initial_capacity);
    header = (vector_header_t*)realloc(header, sizeof(vector_header_t) + (element_size * initial_capacity));
    if (header == NULL) {
        GDDS_SET_ERROR("OOM-Memory allocation failed for vector data");
        return -1;
    }

    header->element_size = element_size;
    header->capacity = initial_capacity;
    return 0;
}

/* Adding and removing elements ( write )*/

static int vector_expand(void** vector) {
    GDDS_ASSERT(vector != NULL);

    vector_header_t* header = (vector_header_t*)(*vector) - 1; // Get pointer to header
    size_t new_capacity = header->capacity * 2;
    header = (vector_header_t*)realloc(header, sizeof(vector_header_t) + (header->element_size * new_capacity));
    if (header == NULL) {
        GDDS_SET_ERROR("OOM-Memory allocation failed for vector data during expansion");
        return -1;
    }

    header->capacity = new_capacity;
    *vector = (void*)(header + 1); // Update vector pointer to the data array
    return 0;
}

int vector_insert_at(void* vector, size_t index, const void* element, void (*CopyFunc)(void* dest, const void* src), bool PreserveOrder) {
    if (vector == NULL) {
        GDDS_SET_ERROR("INVARG-Vector pointer is NULL");
        return -3;
    }
    vector_header_t* header = (vector_header_t*)vector - 1; // Get pointer to header
    if (header->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Vector is not initialized");
        return -2;
    }
    if (index > header->size || index < 0) {
        GDDS_SET_ERROR("INVARG-Index out of bounds");
        return -3;
    }
    if (element == NULL) {
        GDDS_SET_ERROR("INVARG-Element is NULL");
        return -3;
    }

    // Resize if necessary
    if (header->size >= header->capacity) {
        if (vector_expand(&vector) != 0) {
            GDDS_SET_ERROR("OOM-Failed to expand vector");
            return -1;
        }
    }

    void* target = (char*)vector + (index * header->element_size);
    if (PreserveOrder && index != header->size) {
        memmove((char*)target + header->element_size, target, (header->size - index) * header->element_size);
    }
    if (CopyFunc) CopyFunc(target, element);
    else memcpy(target, element, header->element_size);
    header->size++;

    return 0;
}

static int vector_shrink(void** vector) {
    GDDS_ASSERT(vector != NULL);

    vector_header_t* header = (vector_header_t*)(*vector) - 1; // Get pointer to header
    size_t new_capacity = header->capacity / 2;
    GDDS_ASSERT(new_capacity >= 2);
    header = (vector_header_t*)realloc(header, sizeof(vector_header_t) + (header->element_size * new_capacity));
    if (header == NULL) {
        GDDS_SET_ERROR("OOM-Memory allocation failed for vector data during shrinkage");
        return -1;
    }

    header->capacity = new_capacity;
    *vector = (void*)(header + 1); // Update vector pointer to the data array
    return 0;
}

int vector_remove_at(void* vector, size_t index, void (*DestructorFunc)(void* element), bool PreserveOrder) {
    if (vector == NULL) {
        GDDS_SET_ERROR("INVARG-Vector pointer is NULL");
        return -3;
    }
    vector_header_t* header = (vector_header_t*)vector - 1; // Get pointer to header
    if (header->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Vector is not initialized");
        return -2;
    }
    if (index >= header->size) {
        GDDS_SET_ERROR("INVARG-Index out of bounds");
        return -3;
    }

    void* target = (char*)vector + (index * header->element_size);
    if (DestructorFunc) DestructorFunc(target);
    if (PreserveOrder && index != header->size - 1) {
        memmove(target, (char*)target + header->element_size, (header->size - index - 1) * header->element_size);
    }
    header->size--;

    if (header->size > 8 && header->size <= header->capacity / 4) {
        if (vector_shrink(&vector) != 0) {
            GDDS_SET_ERROR("OOM-Failed to shrink vector");
            return -1;
        }
    }

    return 0;
}

void* vector_get_at(const void* vector, size_t index, void (*CopyFunc)(void* dest, const void* src)) {
    if (vector == NULL) {
        GDDS_SET_ERROR("INVARG-Vector pointer is NULL");
        return NULL;
    }
    vector_header_t* header = (vector_header_t*)vector - 1; // Get pointer to header
    if (index >= header->size || index < 0) {
        GDDS_SET_ERROR("INVARG-Index out of bounds");
        return NULL;
    }
    if (header->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Vector is not initialized");
        return NULL;
    }

    void* target = (char*)vector + (index * header->element_size);
    if (CopyFunc) {
        void* result = malloc(header->element_size);
        if (result == NULL) {
            GDDS_SET_ERROR("OOM-Memory allocation failed for vector element copy");
            return NULL;
        }
        CopyFunc(result, target);
        return result;
    }
    return target;
}

int vector_set_at(void* vector, size_t index, const void* element, void (*CopyFunc)(void* dest, const void* src)) {
    if (vector == NULL) {
        GDDS_SET_ERROR("INVARG-Vector pointer is NULL");
        return -3;
    }
    vector_header_t* header = (vector_header_t*)vector - 1; // Get pointer to header
    if (header->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Vector is not initialized");
        return -2;
    }
    if (index >= header->size || index < 0) {
        GDDS_SET_ERROR("INVARG-Index out of bounds");
        return -3;
    }
    if (element == NULL) {
        GDDS_SET_ERROR("INVARG-Element is NULL");
        return -3;
    }

    void* target = (char*)vector + (index * header->element_size);
    if (CopyFunc) CopyFunc(target, element);
    else memcpy(target, element, header->element_size);
    return 0;
}

/* Clearing and destroying ( write )*/

int vector_clear(void* vector, void (*DestructorFunc)(void* element)) {
    if (vector == NULL) {
        GDDS_SET_ERROR("INVARG-Vector pointer is NULL");
        return -3;
    }
    vector_header_t* header = (vector_header_t*)vector - 1; // Get pointer to header
    if (header->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Vector is not initialized");
        return -2;
    }

    if (DestructorFunc) {
        for (size_t i = 0; i < header->size; i++) {
            void* element = (char*)vector + (i * header->element_size);
            DestructorFunc(element);
        }
    }
    header = realloc(header, sizeof(vector_header_t));
    if (header == NULL) {
        GDDS_SET_ERROR("OOM-Memory allocation failed for vector header during clear");
        return -1;
    }

    header->element_size = 0;
    header->size = 0;
    header->capacity = 0;
    return 0;
}

int vector_destroy(void** vector) {
    if (vector == NULL || *vector == NULL) {
        GDDS_SET_ERROR("INVARG-Vector pointer is NULL");
        return -3;
    }

    vector_header_t* header = (vector_header_t*)(*vector) - 1;
    free(header);
    *vector = NULL;
    return 0;
}

/* Utility functions ( read/check )*/

bool vector_is_created(const void* vector) {
    return vector != NULL;
}

bool vector_is_initialized(const void* vector) {
    if (vector == NULL) return false;
    vector_header_t* header = (vector_header_t*)vector - 1;
    return header->element_size > 0;
}

bool vector_is_empty(const void* vector) {
    if (vector == NULL) return true;
    vector_header_t* header = (vector_header_t*)vector - 1;
    return header->size == 0;
}

bool vector_is_full(const void* vector) {
    if (vector == NULL) return false;
    vector_header_t* header = (vector_header_t*)vector - 1;
    return header->size == header->capacity;
}

size_t vector_size(const void* vector) {
    if (vector == NULL) return 0;
    vector_header_t* header = (vector_header_t*)vector - 1;
    return header->size;
}

size_t vector_capacity(const void* vector) {
    if (vector == NULL) return 0;
    vector_header_t* header = (vector_header_t*)vector - 1;
    return header->capacity;
}

size_t vector_element_size(const void* vector) {
    if (vector == NULL) return 0;
    vector_header_t* header = (vector_header_t*)vector - 1;
    return header->element_size;
}

/* Debugging */

void vector_print_info(const void* vector) {
    if (vector == NULL) {
        printf("Vector is not created.\n");
        return;
    }
    vector_header_t* header = (vector_header_t*)vector - 1; // Get pointer to header
    printf("Vector Info:\n");
    printf("\tElement Size: %lu bytes\n", header->element_size);
    printf("\tSize: %lu elements\n", header->size);
    printf("\tCapacity: %lu elements\n", header->capacity);
}