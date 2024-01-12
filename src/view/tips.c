#include "./tips.h"

static void draw(view *v)
{
    tips_view *tv = (tips_view*)v;
    int y = 0;
    csl_print_align(tv->text, v->frame.width, &y, tv->align);
}

void tips_view_init(tips_view *v, char *s, csl_align align)
{
    view_init((view*)v);
    v->v.draw_self = draw;
    v->align = align;
    v->text = s;
}