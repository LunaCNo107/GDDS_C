#include "../include/queue.h"
#include "point.h"

#include <stdio.h>
#include <stdlib.h>

void shift(GDDS_Queue_t* queue, void* sack_point) {
    if (queue_enqueue(queue, sack_point, NULL) != 0 || queue_dequeue(queue, NULL) != 0) {
        printf("Failed to shift elements: %s\n", gdds_get_error());
    }
}

int main() {

    gdds_enable_errors();
    gdds_enable_debug();

    GDDS_Queue_t* queue = queue_create();
    if (!queue) {
        printf("Failed to create queue: %s\n", gdds_get_error());
        return EXIT_FAILURE;
    }
    // queue_print_info(queue);

    size_t initial_capacity = 16;
    if (queue_init(queue, sizeof(point_t), initial_capacity) != 0) {
        printf("Failed to initialize queue: %s\n", gdds_get_error());
        return EXIT_FAILURE;
    }
    // queue_print_info(queue);

    size_t min_size = 5;
    for (size_t i = 0; i < min_size; i++) {
        point_t point = point_create_stack();
        if (queue_enqueue(queue, &point, NULL) != 0) {
            printf("Failed to enqueue element: %s\n", gdds_get_error());
            return EXIT_FAILURE;
        }
    }
    // queue_print_info(queue);

    size_t test = 31%initial_capacity;
    for (size_t i = 0; i < test; i++) {
        point_t point =point_create_stack();
        shift(queue, &point);
    }
    queue_print_info(queue);

    for (size_t i = 0; i < 1; i++) {
        if (queue_dequeue(queue, NULL) != 0) {
            printf("Failed to dequeue element: %s\n", gdds_get_error());
            return EXIT_FAILURE;
        }
    }
    queue_print_info(queue);

    queue_clear(queue, NULL);
    // queue_print_info(queue);

    queue_destroy(&queue);
    // queue_print_info(queue);
 
    return EXIT_SUCCESS;
}