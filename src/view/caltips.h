#if !defined(CALTIPS_VIEW_H)
#define CALTIPS_VIEW_H

#include "view.h"

typedef struct caltips_view
{
    view v;
} caltips_view;

void caltips_view_init(caltips_view *v);

#endif // CALTIPS_VIEW_H
