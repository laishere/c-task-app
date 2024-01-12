#include "./items.h"
#include "console.h"
#include "model/item.h"
#include "main.h"

static void draw(view *v)
{
    int x, y;
    y = 0;
    csl_print_center("Item List", v->frame.width, &y);
    y += 2;
    items_view *iv = (items_view*)v;
    if (iv->items->count > 0 && iv->select >= iv->items->count)
        iv->select = iv->items->count - 1;
    char buf[20];
    for (int i = 0; i < iv->items->count; i++)
    {
        item *it = (item*)array_get(iv->items, i);
        if (item_should_notify(it))
        {
            int period = 800;
            clock_t t = clock() % period;
            csl_setstyle(t < period / 2 ? STY_ITEM_NOTIFY1 : STY_ITEM_NOTIFY2);
        }
        else
        {
            if (iv->select == i)
                csl_setstyle(iv->deletettl ? STY_ITEM_DELETE : STY_ITEM_SELECT);
            else csl_setstyle(STY_NONE);
        }
        if (csl_getstyle() != STY_NONE)
            csl_fill_whitespace(v->frame.width, 0, y);
        x = 0;
        csl_print_nowrap(it->name, &x, &y);
        struct tm *t = localtime(&it->time);
        sprintf(buf, "%02d:%02d", t->tm_hour, t->tm_min);
        csl_print_align(buf, v->frame.width, &y, CSL_ALIGN_RIGHT);
    }
    if (iv->items->count == 0)
    {
        csl_print_center("[EMPTY]", v->frame.width, &y);
    }
    csl_setstyle(STY_NONE);
    if (iv->deletettl)
        iv->deletettl--;
}

static void delete_item(items_view *v)
{
    if (v->select < 0 || v->select >= v->items->count)
        return;
    item *it = (item*)array_get(v->items, v->select);
    item_delete_by_name(it->name);
    array_remove(v->items, v->select);
}

static int handle(view *v, event *ev)
{
    items_view *iv = (items_view*)v;
    int n = max(1, iv->items->count);
    int deleting = iv->deletettl;
    iv->deletettl = 0;
    switch (ev->key)
    {
    case 38: // 上
        iv->select = (iv->select - 1 + n) % n;
        return 1;
    case 40: // 下
        iv->select = (iv->select + 1) % n;
        return 1;
    case 46: // 删除
        if (deleting)
            delete_item(iv);
        else if (iv->items->count > 0)
            iv->deletettl = ttl(1000);
        return 1;
    }
    return 0;
}

void items_view_init(items_view *v, array *items)
{
    view_init((view*)v);
    v->v.draw_self = draw;
    v->v.handle = handle;
    v->items = items;
    v->select = 0;
    v->deletettl = 0;
}