#include "./clock.h"
#include <time.h>
#include "console.h"
#include "model/item.h"

static void draw(view *v)
{
    clock_view *tv = (clock_view*)v;
    time_t now;
    time(&now);
    struct tm *t = localtime(&now);
    char buf[100];
    sprintf(buf, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
    int y = 0;
    csl_print_center(buf, v->frame.width, &y);
    array *list = item_notify_list();
    if (list->count > 0)
    {
        int period = 800;
        clock_t t = clock() % period;
        csl_setstyle(t < period / 2 ? STY_ITEM_NOTIFY1 : STY_ITEM_NOTIFY2);
        sprintf(buf, "% 2d item(s) ", list->count);
        csl_print_center(buf, v->frame.width, &y);
        csl_setstyle(STY_NONE);
    }
}

void clock_view_init(clock_view *v)
{
    view_init((view*)v);
    v->v.draw_self = draw;
}