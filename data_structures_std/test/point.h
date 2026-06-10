#ifndef POINT_H
#define POINT_H

typedef struct point_t {
    int x;
    int y;
} point_t;

point_t point_create_stack();

point_t* point_create_heap();

void CopyPoint_FromContainer(void* dest, const void* src);

void DestroyPoint(void* point);
void DestroyPoint_FromContainer(void* mem_address);

#endif