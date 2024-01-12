#if !defined(CLOCK_VIEW_H)
#define CLOCK_VIEW_H

#include "view.h"
#include "console.h"

typedef struct clock_view
{
    view v;
} clock_view;

void clock_view_init(clock_view *v);

#endif // CLOCK_VIEW_H
