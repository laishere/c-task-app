#include "./search_screen.h"
#include "view/esc.h"
#include "view/tips.h"
#include "view/searchinput.h"
#include "view/searchresult.h"
#include "main.h"
#include "model/item.h"

typedef struct screen
{
    view v;
    esc_view esc;
    tips_view tips;
    searchinput_view input;
    searchresult_view result;
    char tipstext[50];
    char keyword[50];
    int keyword_delay;
    array results;
} screen;

static inline int filter_by_name(void *data, item *it)
{
    char *s = (char*)data;
    return strstr(it->name, s) != NULL;
}

static void search(screen *s)
{
    s->keyword_delay = ttl(500);
    if (strcmp(s->keyword, s->input.input) == 0)
        return;
    strcpy(s->keyword, s->input.input);
    array_clear(&s->results);
    if (*s->keyword == '\0')
        return;
    item_filter(&s->results, filter_by_name, s->keyword);
}

static void layout(view *v)
{
    screen *s = (screen*)v;
    int tipsw = 50;
    // int cx = v->frame.width / 2;
    int inputw = 20;
    int inputh = 3;
    int resultw = 30;
    int resulth = 20;
    view_setframe((view*)&s->esc, make_rect(0, v->frame.height - 1, 20, 1));
    view_setframe((view*)&s->tips, make_rect(v->frame.width - tipsw, v->frame.height - 1, tipsw, 1));
    view_setframe((view*)&s->input, make_rect((v->frame.width - inputw) / 2, 2, inputw, inputh));
    view_setframe((view*)&s->result, make_rect((v->frame.width - resultw) / 2, 4 + inputh, resultw, resulth));
}

static void draw(view *v)
{
    screen *s = (screen*)v;
    if (s->keyword_delay > 0)
        s->keyword_delay--;
    else
        search(s);
}

static void deinit(view *v)
{
    screen *s = (screen*)v;
    array_deinit(&s->results);
    free(v);
}

static void return_main()
{
    goto_main(0, NULL);
}

view *search_screen_new()
{
    screen *s = (screen*)malloc(sizeof(screen));
    view_init((view*)s);
    array_init(&s->results, sizeof(item));
    s->keyword[0] = '\0';
    s->keyword_delay = 0;
    s->v.layout_subviews = layout;
    s->v.deinit = deinit;
    s->v.draw_self = draw;
    esc_view_init(&s->esc, "Home", return_main);
    strcpy(s->tipstext, "[Arrow] Switch item  [Enter] Select item");
    tips_view_init(&s->tips, s->tipstext, CSL_ALIGN_RIGHT);
    searchinput_view_init(&s->input);
    searchresult_view_init(&s->result, &s->results);
    view_add_subview((view*)s, (view*)&s->esc);
    view_add_subview((view*)s, (view*)&s->tips);
    view_add_subview((view*)s, (view*)&s->input);
    view_add_subview((view*)s, (view*)&s->result);
    return (view*)s;
}