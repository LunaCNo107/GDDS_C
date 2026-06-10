#include "../include/array.h"
#include "point.h"

#include <stdio.h>
#include <stdlib.h>

int main() {

    gdds_enable_errors();
    gdds_enable_debug();

    GDDS_Array_t* array = array_create();
    if (!array) {
        printf("Failed to create array: %s\n", gdds_get_error());
        return EXIT_FAILURE;
    }
    array_print_info(array);

    if (array_init(array, sizeof(point_t), 1) != 0) {
        printf("Failed to initialize array: %s\n", gdds_get_error());
        return EXIT_FAILURE;
    }
    array_print_info(array);

    for (size_t i = 0; i < 10; i++)
    {
        point_t point = point_create_stack();
        if (array_insert_at(array, 0, &point, NULL, false) != 0)
        {
            printf("Failed to insert element at index %zu: %s\n", i, gdds_get_error());
            return EXIT_FAILURE;
        }
    }
    array_print_info(array);

    array_clear(array, NULL);
    array_print_info(array);

    array_destroy(&array);
    array_print_info(array);
 
    return EXIT_SUCCESS;
}