#if !defined(GEOMETRY_H)
#define GEOMETRY_H


typedef struct rect
{
    int x, y, width, height;
} rect;

typedef struct size
{
    int width, height;
} size;

typedef struct point
{
    int x, y;
} point;

rect make_rect(int x, int y, int width, int height);
int rect_eq(rect *a, rect *b);
rect rect_intersect(rect *a, rect *b);
int rect_inside(rect *r, int x, int y);
point make_point(int x, int y);

#endif // GEOMETRY_H
