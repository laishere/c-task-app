#if !defined(ITEMSTIPS_VIEW_H)
#define ITEMSTIPS_VIEW_H

#include "view.h"

typedef struct itemstips_view
{
    view v;
    int deleting;
} itemstips_view;

void itemstips_view_init(itemstips_view *v);

#endif // ITEMSTIPS_VIEW_H
