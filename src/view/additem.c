#include "./additem.h"
#include "console.h"
#include "model/item.h"
#include <time.h>
#include "main.h"

enum {
    MSG_ERROR,
    MSG_SUCCESS
};

static void show_cursor(int *x, int *y, char *tips)
{
    csl_setstyle(STY_CURSOR);
    csl_print_nowrap(" ", x, y);
    if (tips) {
        int x2 = 8, y2 = *y + 1;
        csl_setstyle(STY_TIPS);
        csl_print_nowrap(tips, &x2, &y2);
    }
    csl_setstyle(STY_NONE);
}

static void draw(view *v)
{
    additem_view *av = (additem_view*)v;
    int x, y;
    x = 0; y = 0;
    csl_print_center("Add an Item", v->frame.width, &y);
    x = 0; y += 2;
    csl_print_nowrap("  Item: ", &x, &y);
    csl_print_nowrap(av->name, &x, &y);
    if (av->focus == 0) 
        show_cursor(&x, &y, "name of the item");
    x = 0; y += 2;
    csl_print_nowrap("  Time: ", &x, &y);
    csl_print_nowrap(av->time, &x, &y);
    if (av->focus == 1)
        show_cursor(&x, &y, "e.g. 2024-01-10 10:30");
    x = 0; y += 2;
    csl_print_nowrap("Notfiy: ", &x, &y);
    csl_print_nowrap(av->notify, &x, &y);
    if (av->focus == 2)
        show_cursor(&x, &y, "e.g. 5m(5 minutes)");
    x = 8; y += 3;
    csl_setstyle(av->focus == 3 ? STY_BTN_FOCUS : STY_BTN_NORMAL);
    csl_print_nowrap("  OK  ", &x, &y);
    csl_setstyle(STY_NONE);
    x = 0; y += 2;
    if (av->msgttl > 0)
    {
        csl_setstyle(av->msgtype == MSG_ERROR ? STY_MSG_ERROR : STY_MSG_SUCCESS);
        csl_print_center(av->msg, v->frame.width, &y);
        csl_setstyle(STY_NONE);
        av->msgttl--;
    }
}

static void update_focus(additem_view *v, int focus)
{
    v->focus = focus;
    if (focus < 3)
        sprintf(v->tips, "[Enter/Tab] Next");
    else
        sprintf(v->tips, "[Enter] Submit");
}

static char *active_buf(additem_view *av)
{
    switch (av->focus)
    {
    case 0: return av->name;
    case 1: return av->time;
    case 2: return av->notify;
    default: return NULL;
    }
}

static time_t parse_time(char *s)
{
    int y, mon, d, h, m;
    if (sscanf(s, "%d-%d-%d%d:%d", &y, &mon, &d, &h, &m) != 5)
        return 0;
    struct tm t = {0};
    t.tm_year = y - 1900;
    t.tm_mon = mon - 1;
    t.tm_mday = d;
    t.tm_hour = h;
    t.tm_min = m;
    return mktime(&t);
}

static int parse_notify(char *s)
{
    int n;
    if (sscanf(s, "%d", &n) != 1)
        return -1;
    int len = strlen(s);
    if (s[len - 1] == 'm') // 分钟
        n *= 60;
    return n;
}

static void set_time(char *s, time_t t)
{
    struct tm *tm = localtime(&t);
    sprintf(s, "%d-%02d-%02d ", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
}

static void submit(additem_view *av)
{
    item it;
    if (strlen(av->name) == 0)
    {
        sprintf(av->msg, "Name can't be empty!");
        av->focus = 0;
        goto err;
    }
    if (item_find_by_name(av->name))
    {
        sprintf(av->msg, "Item already exists!");
        update_focus(av, 0);
        goto err;
    }
    strcpy(it.name, av->name);
    it.time = parse_time(av->time);
    if (it.time == 0)
    {
        sprintf(av->msg, "Time format is incorrect!");
        update_focus(av, 1);
        goto err;
    }
    it.notify = parse_notify(av->notify);
    if (it.notify == -1)
    {
        sprintf(av->msg, "Notfiy format is incorrect!");
        update_focus(av, 2);
        goto err;
    }
    item_add(it);
    sprintf(av->msg, "Add item successfully!");
    av->msgtype = MSG_SUCCESS;
    av->msgttl = ttl(2000);
    av->name[0] = '\0';
    set_time(av->time, it.time); // 复用年月日
    update_focus(av, 0);
    return;
err:
    av->msgtype = MSG_ERROR;
    av->msgttl = ttl(2000);
}

int handle(view *v, event *ev)
{
    additem_view *av = (additem_view*)v;
    char *s = active_buf(av);
    int len = s ? strlen(s) : 0;
    switch (ev->key)
    {
    case 8: // 删除
        if (len > 0)
            s[len - 1] = '\0';
        return 1;
    case 38: // 箭头上
        update_focus(av, (av->focus - 1 + 4) % 4);
        return 1;
    case 13: // 回车
        if (av->focus == 3)
        {
            // 提交
            submit(av);
            return 1;
        }
    case 40: // 箭头下
    case 9: // tab
        update_focus(av, (av->focus + 1 + 4) % 4);
        return 1;
    }
    if (!s || len >= 49)
        return 0;
    if (ev->achar >= 32 && ev->achar <= 126)
    {
        // 输入文本
        s[len++] = ev->achar;
        s[len] = '\0';
        return 1;
    }
    return 0;
}

void additem_view_init(additem_view *v, char *time, char *tips)
{
    view_init((view*)v);
    v->v.draw_self = draw;
    v->v.handle = handle;
    v->name[0] = '\0';
    strcpy(v->time, time);
    strcpy(v->notify, "5m");
    v->tips = tips;
    update_focus(v, 0);
}