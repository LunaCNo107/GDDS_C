#include <stdlib.h>

#include "point.h"

point_t point_create_stack() {
    return (point_t){.x = rand()%100, .y = rand()%100};
}

point_t* point_create_heap() {
    point_t* point = malloc(sizeof(point_t));
    *point = point_create_stack();
    return point;
}

void CopyPoint_FromContainer(void* dest, const void* src) {
    point_t** dest_point_ptr = (point_t**)dest;
    const point_t* src_point = *((const point_t**)src);
    *dest_point_ptr = malloc(sizeof(point_t));
    **dest_point_ptr = *src_point;
}

void DestroyPoint(void* point) {
    free(point);
}
void DestroyPoint_FromContainer(void* mem_address) {
    point_t* point = *((point_t**)mem_address);
    free(point);
}