#if !defined(SEARCHINPUT_VIEW_H)
#define BRTIPS_VIEW_H

#include "view.h"
#include "console.h"

typedef struct
{
    view v;
    char input[50];
} searchinput_view;

void searchinput_view_init(searchinput_view *v);

#endif // BRTIPS_VIEW_H
