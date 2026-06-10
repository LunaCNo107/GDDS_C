#include "../include/stack.h"
#include "point.h"

#include <stdio.h>
#include <stdlib.h>

int main() {

    gdds_enable_errors();
    gdds_enable_debug();

    GDDS_Stack_t* stack = stack_create();
    if (!stack) {
        printf("Failed to create stack: %s\n", gdds_get_error());
        return EXIT_FAILURE;
    }
    stack_print_info(stack);

    if (stack_init(stack, sizeof(point_t*), 1) != 0) {
        printf("Failed to initialize stack: %s\n", gdds_get_error());
        return EXIT_FAILURE;
    }
    stack_print_info(stack);

    for (size_t i = 0; i < 10; i++)
    {
        point_t* point = point_create_heap();
        if (stack_push(stack, &point, CopyPoint_FromContainer) != 0)
        {
            printf("Failed to push element: %s\n", gdds_get_error());
            return EXIT_FAILURE;
        }
        DestroyPoint(point);
    }
    stack_print_info(stack);

    stack_clear(stack, DestroyPoint_FromContainer);
    stack_print_info(stack);

    stack_destroy(&stack);
    stack_print_info(stack);
 
    return EXIT_SUCCESS;
}