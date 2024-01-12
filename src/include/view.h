#if !defined(VIEW_H)
#define VIEW_H

#include "array.h"
#include "geometry.h"
#include "event.h"
#define SIZE_NO_LIMIT INT_MAX

typedef struct view
{
    struct view *super;
    rect frame;
    rect bounds;
    array subviews;
    void (*layout_subviews)(struct view*);
    void (*draw_self)(struct view*);
    void (*deinit)(struct view*);
    int (*handle)(struct view *, event *ev);
} view;

void view_init(view *v);
void view_deinit(view *v);
void view_add_subview(view *v, view *subview);
void view_layout(view *v);
void view_draw(view *v);
void view_setframe(view *v, rect frame);
void view_setbounds(view *v, rect bounds);
int view_handle(view *v, event *ev);
int view_childcount(view *v);
view *view_child_at(view *v, int index);

#endif // VIEW_H
