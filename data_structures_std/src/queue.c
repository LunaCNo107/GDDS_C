#include "../include/queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* array based queue definition*/

struct GDDS_Queue_t {
    void* data;
    size_t element_size;
    size_t size;
    size_t capacity;
    size_t head;
    size_t tail;
};

/* Creation and initialization ( write )*/

GDDS_Queue_t* queue_create(){
    GDDS_Queue_t* queue = calloc(1, sizeof(GDDS_Queue_t));
    if (queue == NULL) {
        GDDS_SET_ERROR("OOM-Memory allocation failed for Queue");
        return NULL;
    }
    return queue;
}

static size_t queue_roundup_capacity(size_t capacity) {
    size_t rounded = 2;
    while (rounded < capacity) {
        rounded <<= 1;
    }
    return rounded;
}

int queue_init(GDDS_Queue_t* queue, size_t element_size, size_t initial_capacity){
    if (queue == NULL) {
        GDDS_SET_ERROR("INVARG-Queue is NULL");
        return -3;
    }
    if (element_size == 0) {
        GDDS_SET_ERROR("INVARG-Element size must be greater than zero");
        return -3;
    }

    if (queue->element_size > 0) {
        GDDS_SET_ERROR("INVOP-Queue is already initialized");
        return -2;
    }

    initial_capacity = queue_roundup_capacity(initial_capacity);
    queue->data = malloc(element_size * initial_capacity);
    if (queue->data == NULL) {
        GDDS_SET_ERROR("OOM-Memory allocation failed for queue data");
        return -1;
    }

    queue->element_size = element_size;
    queue->capacity = initial_capacity;
    return 0;
}

/* Adding and removing elements ( write )*/

static int queue_expand(GDDS_Queue_t* queue) {
    GDDS_ASSERT(queue != NULL);

    size_t new_capacity = queue->capacity * 2;
    void* new_data = realloc(queue->data, new_capacity * queue->element_size);
    if (new_data == NULL) {
        GDDS_SET_ERROR("OOM-Memory reallocation failed for queue data during expansion");
        return -1;
    }
    queue->data = new_data;

    if (queue->head > 1) {
        size_t elements_to_shift = queue->capacity - queue->head;
        
        char* src = (char*)queue->data + (queue->head * queue->element_size);
        char* dst = (char*)queue->data + (new_capacity - elements_to_shift) * queue->element_size;
        memmove(dst, src, elements_to_shift * queue->element_size);
        
        queue->head = new_capacity - elements_to_shift;
    }

    queue->capacity = new_capacity;
    return 0;
}

int queue_enqueue(GDDS_Queue_t* queue, const void* element, void (*CopyFunc)(void* dest, const void* src)) {
    if (queue == NULL) {
        GDDS_SET_ERROR("INVARG-Queue is NULL");
        return -3;
    }
    if (element == NULL) {
        GDDS_SET_ERROR("INVARG-Element is NULL");
        return -3;
    }
    if (queue->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Queue is not initialized");
        return -2;
    }

    if ((queue->tail + 1) % queue->capacity == queue->head) {
        if (queue_expand(queue) != 0) {
            GDDS_SET_ERROR("OOM-Failed to expand queue");
            return -1;
        }
    }

    void* target = (char*)queue->data + (queue->tail * queue->element_size);
    if (CopyFunc) CopyFunc(target, element);
    else memcpy(target, element, queue->element_size);

    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->size++;
    return 0;
}

static int queue_shrink(GDDS_Queue_t* queue) {
    GDDS_ASSERT(queue != NULL);

    size_t new_capacity = queue->capacity / 2;
    if (new_capacity < 2) {
        new_capacity = 2;
    }

    void* new_data = NULL;

    if (queue->tail >= queue->capacity/4 && queue->tail <= queue->capacity/2) {
        new_data = realloc(queue->data, new_capacity * queue->element_size);
        if (new_data == NULL) {
            GDDS_SET_ERROR("OOM-Memory reallocation failed for queue data during shrink");
            return -1;
        }

        queue->tail = queue->tail % new_capacity;
    } else if (queue->head < queue->capacity/2 && queue->tail > queue->capacity/2) {
        size_t elements_to_shift = queue->tail - queue->capacity/2;
        
        char* src = (char*)queue->data + ((queue->capacity/2) * queue->element_size);
        memmove(queue->data, src, elements_to_shift * queue->element_size);
        
        new_data = realloc(queue->data, new_capacity * queue->element_size);
        if (new_data == NULL) {
            GDDS_SET_ERROR("OOM-Memory reallocation failed for queue data during shrink");
            return -1;
        }
        
        queue->tail = queue->tail % new_capacity;
    } else if (queue->head >= queue->capacity/2 && queue->head <= queue->capacity/4*3) {
        char* src = (char*)queue->data + (queue->head * queue->element_size);
        memmove(queue->data, src, queue->size * queue->element_size);
        
        new_data = realloc(queue->data, new_capacity * queue->element_size);
        if (new_data == NULL) {
            GDDS_SET_ERROR("OOM-Memory reallocation failed for queue data during shrink");
            return -1;
        }
        
        queue->head = 0;
        queue->tail = queue->size;
    } else {
        size_t elements_to_shift = queue->capacity - queue->head;

        char* src = (char*)queue->data + (queue->head * queue->element_size);
        char* dst = (char*)queue->data + (new_capacity - elements_to_shift) * queue->element_size;
        memmove(dst, src, elements_to_shift * queue->element_size);
        
        new_data = realloc(queue->data, new_capacity * queue->element_size);
        if (new_data == NULL) {
            GDDS_SET_ERROR("OOM-Memory reallocation failed for queue data during shrink");
            return -1;
        }
        
        queue->head = new_capacity - elements_to_shift;
    }
    
    queue->data = new_data;
    queue->capacity = new_capacity;
    return 0;
}

int queue_dequeue(GDDS_Queue_t* queue, void (*DestructorFunc)(void* element)) {
    if (queue == NULL) {
        GDDS_SET_ERROR("INVARG-Queue is NULL");
        return -3;
    }
    if (queue->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Queue is not initialized");
        return -2;
    }
    if (queue->size == 0) {
        GDDS_SET_ERROR("INVOP-Queue is empty");
        return -2;
    }

    void* source = (char*)queue->data + (queue->head * queue->element_size);
    if (DestructorFunc) {
        DestructorFunc(source);
    }

    queue->head = (queue->head + 1) % queue->capacity;
    queue->size--;

    if (queue->size > 0 && queue->size <= queue->capacity / 4) {
        if (queue_shrink(queue) != 0) {
            GDDS_SET_ERROR("OOM-Failed to shrink queue");
            return -1;
        }
    }

    return 0;
}

void* queue_head(const GDDS_Queue_t* queue, void (*CopyFunc)(void* dest, const void* src)) {
    if (queue == NULL) {
        GDDS_SET_ERROR("INVARG-Queue is NULL");
        return NULL;
    }
    if (queue->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Queue is not initialized");
        return NULL;
    }
    if (queue->size == 0) {
        GDDS_SET_ERROR("INVOP-Queue is empty");
        return NULL;
    }

    void* source = (char*)queue->data + (queue->head * queue->element_size);
    if (CopyFunc) {
        void* out_element = malloc(queue->element_size);
        if (out_element == NULL) {
            GDDS_SET_ERROR("OOM-Memory allocation failed for queue head copy");
            return NULL;
        }
        CopyFunc(out_element, source);
        return out_element;
    }
    return source;
}

void* queue_tail(const GDDS_Queue_t* queue, void (*CopyFunc)(void* dest, const void* src)) {
    if (queue == NULL) {
        GDDS_SET_ERROR("INVARG-Queue is NULL");
        return NULL;
    }
    if (queue->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Queue is not initialized");
        return NULL;
    }
    if (queue->size == 0) {
        GDDS_SET_ERROR("INVOP-Queue is empty");
        return NULL;
    }

    size_t tail_index = (queue->tail + queue->capacity - 1) % queue->capacity;
    void* source = (char*)queue->data + (tail_index * queue->element_size);
    if (CopyFunc) {
        void* out_element = malloc(queue->element_size);
        if (out_element == NULL) {
            GDDS_SET_ERROR("OOM-Memory allocation failed for queue tail copy");
            return NULL;
        }
        CopyFunc(out_element, source);
        return out_element;
    }
    return source;
}

/* Clearing and destroying ( write )*/

int queue_clear(GDDS_Queue_t* queue, void (*DestructorFunc)(void* element)) {
    if (queue == NULL) {
        GDDS_SET_ERROR("INVARG-Queue is NULL");
        return -3;
    }
    if (queue->element_size == 0) {
        GDDS_SET_ERROR("INVOP-Queue is not initialized");
        return -2;
    }

    if (DestructorFunc) {
        for (size_t i = 0; i < queue->size; i++) {
            size_t index = (queue->head + i) % queue->capacity;
            void* element = (char*)queue->data + (index * queue->element_size);
            DestructorFunc(element);
        }
    }

    free(queue->data);
    queue->data = NULL;
    queue->size = 0;
    queue->capacity = 0;
    queue->element_size = 0;
    queue->head = 0;
    queue->tail = 0;
    return 0;
}

int queue_destroy(GDDS_Queue_t** queue) {
    if (queue == NULL) {
        GDDS_SET_ERROR("INVARG-Queue pointer is NULL");
        return -3;
    }
    if (*queue == NULL) {
        GDDS_SET_ERROR("INVOP-Queue is already destroyed");
        return -2;
    }
    if ((*queue)->element_size > 0 && (*queue)->data != NULL) {
        GDDS_SET_ERROR("INVOP-Queue is not cleared before destruction");
        return -2;
    }

    free(*queue);
    *queue = NULL;
    return 0;
}

/* Utility functions ( read/check )*/

bool queue_is_created(const GDDS_Queue_t* queue) {
    return queue != NULL;
}

bool queue_is_initialized(const GDDS_Queue_t* queue) {
    return queue->element_size > 0;
}

bool queue_is_empty(const GDDS_Queue_t* queue) {
    return queue->size == 0;
}

bool queue_is_full(const GDDS_Queue_t* queue) {
    return (queue->tail + 1) % queue->capacity == queue->head;
}

size_t queue_size(const GDDS_Queue_t* queue) {
    return queue->size;
}

size_t queue_capacity(const GDDS_Queue_t* queue) {
    return queue->capacity;
}

size_t queue_element_size(const GDDS_Queue_t* queue) {
    return queue->element_size;
}

size_t queue_head_cursor(const GDDS_Queue_t* queue) {
    return queue->head;
}

size_t queue_tail_cursor(const GDDS_Queue_t* queue) {
    return queue->tail;
}

/* Debugging */

void queue_print_info(const GDDS_Queue_t* queue) {
    if (queue == NULL) {
        printf("Queue is not created.\n");
        return;
    }
    printf("Queue info:\n");
    printf("\tElement size: %zu bytes\n", queue->element_size);
    printf("\tSize: %zu\n", queue->size);
    printf("\tCapacity: %zu\n", queue->capacity);
    printf("\tHead cursor: %zu\n", queue->head);
    printf("\tTail cursor: %zu\n", queue->tail);
}