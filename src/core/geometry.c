#include "geometry.h"
#include <memory.h>

static inline int max(int a, int b)
{
    return a > b ? a : b;
}

static inline int min(int a, int b)
{
    return a < b ? a : b;
}

inline rect make_rect(int x, int y, int width, int height)
{
    rect r = {x, y, width, height};
    return r;
}

inline int rect_eq(rect *a, rect *b)
{
    return a && b && memcmp(a, b, sizeof(rect)) == 0;
}

inline int rect_inside(rect *r, int x, int y)
{
    return  x >= r->x && 
            x < r->x + r->width &&
            y >= r->y &&
            y < r->y + r->height;
}

rect rect_intersect(rect *a, rect *b)
{
    int left = max(a->x, b->x);
    int top = max(a->y, b->y);
    int right = min(a->x + a->width, b->x + b->width);
    int bottom = min(a->y + a->height, b->y + b->height);
    return make_rect(left, top, right - left, bottom - top);
}

inline point make_point(int x, int y)
{
    point p = {x, y};
    return p;
}