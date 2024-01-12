#if !defined(ARRAY_H)
#define ARRAY_H

#include "stdlib.h"

typedef struct array
{
    size_t count;
    size_t _element_size;
    size_t _capacity;
    void *_data;
    void *_tmp;
    int (*cmp)(void*, void*);
} array;

void array_init(array *a, size_t element_size);
void array_deinit(array *a);
void *array_get(array *a, int index);
void array_remove(array *a, int index);
void array_push(array *a, void *element);
void array_clear(array *a);
void array_sort(array *a, int (*cmp)(void *a, void *b));

#endif // ARRAY_H
