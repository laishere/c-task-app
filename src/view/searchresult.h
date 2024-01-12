#if !defined(SEARCHRESULT_VIEW_H)
#define SEARCHRESULT_VIEW_H

#include "view.h"

typedef struct
{
    view v;
    array *items;
    int select;
} searchresult_view;

void searchresult_view_init(searchresult_view *v, array *items);

#endif // SEARCHRESULT_VIEW_H
