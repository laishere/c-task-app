#if !defined(TIPS_VIEW_H)
#define TIPS_VIEW_H

#include "view.h"
#include "console.h"

typedef struct tips_view
{
    view v;
    char *text;
    csl_align align;
} tips_view;

void tips_view_init(tips_view *v, char *s, csl_align align);

#endif // TIPS_VIEW_H
