#include "array.h"
#include "assert.h"
#include <memory.h>

static void alloc(array *a, size_t cap)
{
    a->_capacity = cap;
    a->_data = a->_data ? realloc(a->_data, cap) : malloc(cap);
}

void array_init(array *a, size_t element_size)
{
    memset(a, 0, sizeof(array));
    a->_element_size = element_size;
    alloc(a, 8 * element_size);
}

void array_deinit(array *a)
{
    if (a->_tmp)
    {
        free(a->_tmp);
        a->_tmp = NULL;
    }
    free(a->_data);
    a->_data = NULL;
}

void *array_get(array *a, int index)
{
    ASSERT(index < a->count);
    return (char*)a->_data + (a->_element_size * index);
}

void array_remove(array *a, int index)
{
    if (index < 0 || index >= a->count)
        return;
    // 往前挪一位
    for (int i = index + 1; i < a->count; i++)
    {
        memcpy(array_get(a, i - 1), array_get(a, i), a->_element_size);
    }
    a->count--;
}

void array_push(array *a, void *element)
{
    int req_size = (a->count + 1) * a->_element_size;
    if (a->_capacity < req_size)
        alloc(a, a->_element_size * (a->count + 8));
    ASSERT(a->_capacity >= req_size);
    void *next = (char *)a->_data + (a->_element_size * a->count);
    memcpy(next, element, a->_element_size);
    a->count++;
}

void array_clear(array *a)
{
    a->count = 0;
}

static void swap(array *arr, int i, int j) 
{
    void *a = array_get(arr, i);
    void *b = array_get(arr, j);
    memcpy(arr->_tmp, a, arr->_element_size);
    memcpy(a, b, arr->_element_size);
    memcpy(b, arr->_tmp, arr->_element_size);
}

static int partition(array *arr, int low, int high) 
{
    void *pivot = array_get(arr, high);
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) 
    {
        if ((*arr->cmp)(array_get(arr, j), pivot) <= 0) 
        {
            i++;
            swap(arr, i, j);
        }
    }
    swap(arr, i + 1, high);
    return i + 1;
}

void quick_sort(array *arr, int low, int high) 
{
    if (low < high) {
        int pi = partition(arr, low, high);
        quick_sort(arr, low, pi - 1);
        quick_sort(arr, pi + 1, high);
    }
}

// 快排
void array_sort(array *a, int (*cmp)(void *a, void *b))
{
    if (!a->_tmp)
        a->_tmp = malloc(a->_element_size);
    a->cmp = cmp;
    quick_sort(a, 0, a->count - 1);
}