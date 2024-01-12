#if !defined(ESC_VIEW_H)
#define ESC_VIEW_H

#include "view.h"

typedef struct esc_view
{
    view v;
    int escttl;
    char title[20];
    void (*action)();
} esc_view;

void esc_view_init(esc_view *v, char *title, void (*action)());

#endif // ESC_VIEW_H
