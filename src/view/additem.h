#if !defined(ADD_ITEM_VIEW_H)
#define ADD_ITEM_VIEW_H

#include "view.h"

typedef struct additem_view
{
    view v;
    int focus;
    char name[50];
    char time[50];
    char notify[50];
    char msg[50];
    int msgttl;
    int msgtype;
    char *tips;
} additem_view;

void additem_view_init(additem_view *v, char *time, char *tips);

#endif // ADD_ITEM_VIEW_H
