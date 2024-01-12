#include "./searchresult.h"
#include "console.h"
#include "model/item.h"
#include "main.h"

static void draw(view *v)
{
    int x, y;
    y = 0;
    searchresult_view *sv = (searchresult_view*)v;
    if (sv->items->count > 0 && sv->select >= sv->items->count)
        sv->select = sv->items->count - 1;
    char buf[20];
    for (int i = 0; i < sv->items->count; i++)
    {
        item *it = (item*)array_get(sv->items, i);
        if (sv->select == i)
        {
            csl_setstyle(STY_ITEM_SELECT);
            csl_fill_whitespace(v->frame.width, 0, y);
        }
        else csl_setstyle(STY_NONE);
        x = 0;
        csl_print_nowrap(it->name, &x, &y);
        struct tm *t = localtime(&it->time);
        sprintf(buf, "%d-%02d-%-02d %02d:%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
        csl_print_align(buf, v->frame.width, &y, CSL_ALIGN_RIGHT);
    }
    if (sv->items->count == 0)
    {
        csl_print_center("[EMPTY]", v->frame.width, &y);
    }
    csl_setstyle(STY_NONE);
}

static void delete_item(searchresult_view *v)
{
    if (v->select < 0 || v->select >= v->items->count)
        return;
    item *it = (item*)array_get(v->items, v->select);
    item_delete_by_name(it->name);
    array_remove(v->items, v->select);
}

static void select_item(searchresult_view *v)
{
    if (v->select < 0 || v->select >= v->items->count)
        return;
    item *it = (item*)array_get(v->items, v->select);
    goto_main(it->time, it->name);
}

static int handle(view *v, event *ev)
{
    searchresult_view *sv = (searchresult_view*)v;
    int n = max(1, sv->items->count);
    switch (ev->key)
    {
    case 38: // 上
        sv->select = (sv->select - 1 + n) % n;
        return 1;
    case 40: // 下
        sv->select = (sv->select + 1) % n;
        return 1;
    case 13: // 回车
        select_item(sv);
        return 1;
    }
    return 0;
}

void searchresult_view_init(searchresult_view *v, array *items)
{
    view_init((view*)v);
    v->v.draw_self = draw;
    v->v.handle = handle;
    v->items = items;
    v->select = 0;
}