#include "./add_screen.h"
#include "view/additem.h"
#include "view/esc.h"
#include "view/tips.h"
#include "main.h"

typedef struct screen
{
    view v;
    additem_view additems;
    esc_view esc;
    tips_view tips;
    char tipstext[50];
} screen;

static void layout(view *v)
{
    screen *s = (screen*)v;
    int addw = 30;
    int addh = 16;
    int tipsw = 30;
    view_setframe((view*)&s->additems, make_rect((v->frame.width - addw) / 2, (v->frame.height - addh) / 2, addw, addh));
    view_setframe((view*)&s->esc, make_rect(0, v->frame.height - 1, 20, 1));
    view_setframe((view*)&s->tips, make_rect(v->frame.width - tipsw, v->frame.height - 1, tipsw, 1));
}

static void deinit(view *v)
{
    free(v);
}

static void return_main()
{
    goto_main(0, NULL);
}

view *add_screen_new(char *time)
{
    screen *s = (screen*)malloc(sizeof(screen));
    view_init((view*)s);
    s->v.layout_subviews = layout;
    s->v.deinit = deinit;
    additem_view_init(&s->additems, time, s->tipstext);
    esc_view_init(&s->esc, "Home", return_main);
    tips_view_init(&s->tips, s->tipstext, CSL_ALIGN_RIGHT);
    view_add_subview((view*)s, (view*)&s->additems);
    view_add_subview((view*)s, (view*)&s->esc);
    view_add_subview((view*)s, (view*)&s->tips);
    return (view*)s;
}