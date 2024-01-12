#include "./searchinput.h"
#include "console.h"

static void draw(view *v)
{
    searchinput_view *sv = (searchinput_view*)v;
    int x, y = 0;
    csl_print_center("Search Items by Name", v->frame.width, &y);
    x = 0; y++;
    csl_print_nowrap(sv->input, &x, &y);
    csl_setstyle(STY_CURSOR);
    csl_putchar(' ', x, y);
    csl_setstyle(STY_NONE);
}

static int handle(view *v, event *ev)
{
    searchinput_view *sv = (searchinput_view*)v;
    int len = strlen(sv->input);
    char *s = sv->input;
    if (ev->key == 8)
    {
        if (len > 0)
            s[len - 1] = '\0';
        return 1;
    }
    if (ev->achar >= 32 && ev->achar <= 126)
    {
        // 输入文本
        s[len++] = ev->achar;
        s[len] = '\0';
        return 1;
    }
    return 0;
}

void searchinput_view_init(searchinput_view *v)
{
    view_init((view*)v);
    v->v.draw_self = draw;
    v->v.handle = handle;
    v->input[0] = '\0';
}