#if !defined(ITEM_H)
#define ITEM_H

#include <time.h>
#include "array.h"

typedef struct item
{
    char name[50];
    time_t time;
    int notify;
} item;

void item_load();
item *item_find_by_name(char *name);
void item_add(item it);
void item_delete_by_name(char *name);
void item_save();
array *item_getall();
void item_filter(array *result, int (*f)(void *, item *), void *data);
int item_version();
void item_sort_by_time(array *a, int asc);
array *item_notify_list();
int item_should_notify(item *it);

#endif // ITEM_H
