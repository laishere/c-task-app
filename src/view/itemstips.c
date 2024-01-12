#include "./itemstips.h"
#include "console.h"

static void draw(view *v)
{
    itemstips_view *iv = (itemstips_view*)v;
    int x = 0, y = 0;
    csl_setstyle(STY_TIPS);
    csl_print_nowrap(" [ARROW] Switch item ", &x, &y);
    x = 0; y++;
    if (iv->deleting) 
    {
        csl_setstyle(STY_DANGER);
        csl_print_nowrap("[DELETE] Press again", &x, &y);
    }
    else
        csl_print_nowrap("[DELETE] Delete item", &x, &y);
    csl_setstyle(STY_NONE);
}

void itemstips_view_init(itemstips_view *v)
{
    view_init((view*)v);
    v->v.draw_self = draw;
}