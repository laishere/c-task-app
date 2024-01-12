#if !defined(ITEMS_VIEW_H)
#define ITEMS_VIEW_H

#include "view.h"

typedef struct items_view
{
    view v;
    array *items;
    int select;
    int deletettl;
} items_view;

void items_view_init(items_view *v, array *items);

#endif // ITEMS_VIEW_H
