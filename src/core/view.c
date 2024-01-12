#include "view.h"
#include "console.h"
#include "assert.h"
#include <memory.h>

static inline int childcount(view *v)
{
    return v->subviews.count;
}

static view *child_at(view *v, int index)
{
    ASSERT(index < childcount(v));
    return *(view**)array_get(&v->subviews, index);
}

static void layout_subviews(view *v)
{
    for (int i = 0; i < childcount(v); i++)
    {
        view *child = child_at(v, i);
        view_layout(child);
    }
}

static void draw_subviews(view *v)
{
    for (int i = 0; i < v->subviews.count; i++)
    {
        view *child = *(view**)array_get(&v->subviews, i);
        view_draw(child);
    }
}

static point screen_pos(view *v)
{
    int x = 0, y = 0;
    view *p = v;
    while (p)
    {
        x += p->frame.x - p->bounds.x;
        y += p->frame.y - p->bounds.y;
        p = p->super;
    }
    // 多减了自身的bounds，恢复回来
    x += v->bounds.x;
    y += v->bounds.y;
    return make_point(x, y);
}

void view_init(view *v)
{
    memset(v, 0, sizeof(view));
    array_init(&v->subviews, sizeof(view*));
    v->layout_subviews = layout_subviews;
}

void view_add_subview(view *v, view *subview)
{
    ASSERT(subview->super == NULL);
    subview->super = v;
    array_push(&v->subviews, &subview);
}

void view_deinit(view *v)
{
    for (int i = 0; i < childcount(v); i++)
    {
        view *child = child_at(v, i);
        view_deinit(child);
    }
    array_deinit(&v->subviews);
    if (v->deinit)
        (*v->deinit)(v);
}

void view_layout(view *v)
{
    (*v->layout_subviews)(v);
}

void view_draw(view *v)
{
    rect prevclip = csl_getclip();
    point prevoff = csl_getoffset();
    point spos = screen_pos(v);
    point soff = spos;
    soff.x -= v->bounds.x;
    soff.y -= v->bounds.y;
    rect sframe = make_rect(spos.x, spos.y, v->frame.width, v->frame.height);
    rect clip = rect_intersect(&prevclip, &sframe);
    csl_clip(clip);
    csl_offset(soff);
    if (v->draw_self)
        (*v->draw_self)(v);
    draw_subviews(v);
    csl_clip(prevclip);
    csl_offset(prevoff);
}

int view_childcount(view *v)
{
    return childcount(v);
}

view *view_child_at(view *v, int index)
{
    return child_at(v, index);
}

void view_setframe(view *v, rect frame)
{
    if (rect_eq(&v->frame, &frame))
        return;
    v->frame = frame;
    v->bounds = make_rect(0, 0, frame.width, frame.height);
    view_layout(v);
}

void view_setbounds(view *v, rect bounds)
{
    if (bounds.width < v->frame.width)
        bounds.width = v->frame.width;
    if (bounds.height < v->frame.height)
        bounds.height = v->frame.height;
    v->bounds = bounds;
}

/**
 * 处理事件
 * 子优先
*/
int view_handle(view *v, event *ev)
{
    for (int i = childcount(v) - 1; i >= 0 ; i--)
    {
        view *ch = child_at(v, i);
        if (view_handle(ch, ev))
            return 1;
    }
    if (v->handle)
        return (*v->handle)(v, ev);
    return 0;
}