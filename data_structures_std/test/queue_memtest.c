#include "../include/queue.h"
#include "point.h"

#include <stdio.h>
#include <stdlib.h>

int main() {

    gdds_enable_errors();
    gdds_enable_debug();

    GDDS_Queue_t* queue = queue_create();
    if (!queue) {
        printf("Failed to create queue: %s\n", gdds_get_error());
        return EXIT_FAILURE;
    }
    queue_print_info(queue);

    if (queue_init(queue, sizeof(point_t*), 1) != 0) {
        printf("Failed to initialize queue: %s\n", gdds_get_error());
        return EXIT_FAILURE;
    }
    queue_print_info(queue);

    for (size_t i = 0; i < 10; i++)
    {
        point_t* point = point_create_heap();
        if (queue_enqueue(queue, &point, CopyPoint_FromContainer) != 0)
        {
            printf("Failed to enqueue element: %s\n", gdds_get_error());
            return EXIT_FAILURE;
        }
        DestroyPoint(point);
    }
    queue_print_info(queue);

    queue_clear(queue, DestroyPoint_FromContainer);
    queue_print_info(queue);

    queue_destroy(&queue);
    queue_print_info(queue);
 
    return EXIT_SUCCESS;
}