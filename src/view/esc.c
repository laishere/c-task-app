#include "./esc.h"
#include "console.h"
#include "main.h"

static void draw(view *v)
{
    esc_view *ev = (esc_view*)v;
    int x = 0, y = 0;
    if (ev->escttl > 0)
    {
        ev->escttl--;
        csl_print_nowrap("[ESC] Press again", &x, &y);
    }
    else
    {
        csl_print_nowrap("[ESC] ", &x, &y);
        csl_print_nowrap(ev->title, &x, &y);
    }
}

static int handle(view *v, event *ev)
{
    esc_view *escv = (esc_view*)v;
    if (ev->key == 27)
    {
        // ESC
        if (escv->escttl > 0)
        {
            if (escv->action)
                (*escv->action)();
        }
        else
            escv->escttl = ttl(1000);
        return 1;
    }
    escv->escttl = 0;
    return 0;
}

void esc_view_init(esc_view *v, char *title, void (*action)())
{
    view_init((view*)v);
    v->v.draw_self = draw;
    v->v.handle = handle;
    v->escttl = 0;
    strcpy(v->title, title);
    v->action = action;
}