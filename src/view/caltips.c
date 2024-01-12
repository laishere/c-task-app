#include "./caltips.h"
#include "console.h"

static void draw(view *v)
{
    int x = 0, y = 0;
    csl_setstyle(STY_TIPS);
    csl_print_nowrap("[W/A/S/D] Switch date", &x, &y);
    x = 0; y++;
    csl_print_nowrap("    [Q/E] Switch month", &x, &y);
    csl_setstyle(STY_NONE);
}

void caltips_view_init(caltips_view *v)
{
    view_init((view*)v);
    v->v.draw_self = draw;
}