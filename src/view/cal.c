#include "./cal.h"
#include "console.h"
#include <time.h>

#define ONE_DAY 86400

const static char *months[] = {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};

static time_t year_month(int year, int month) {
    struct tm date = {0};
    date.tm_year = year - 1900;
    date.tm_mon = month - 1;
    date.tm_mday = 1;
    return mktime(&date);
}

static void draw(view *v)
{
    cal_view *cv = (cal_view*)v;
    int w = v->bounds.width;
    int x, y;
    char buf[30];
    sprintf(buf, "%s %d", months[cv->month - 1], cv->year);
    y = 0;
    csl_print_center(buf, w, &y);
    x = 0; y += 2;
    csl_print_nowrap("Su  Mo  Tu  We  Th  Fr  Sa", &x, &y);
    int md = 1 - cv->firstwd;
    time_t now;
    time(&now);
    struct tm *t = localtime(&now);
    int thismonth = t->tm_year + 1900 == cv->year && t->tm_mon + 1 == cv->month;
    int today = t->tm_mday;
    y += 2;
    while (md <= cv->days)
    {
        x = 0;
        for (int wd = 0; wd < 7; wd++)
        {
            if (md >= 1 && md <= cv->days)
            {
                csl_style s = STY_NONE;
                if (cv->select == md)
                    s = STY_CAL_SELECTED;
                else if (thismonth && md == today)
                    s = STY_CAL_TODAY;
                else if (cv->calmap[md])
                    s = STY_CAL_HAS_ITEMS;
                csl_setstyle(s);
                sprintf(buf, "%02d", md);
                csl_print_nowrap(buf, &x, &y);
                csl_setstyle(STY_NONE);
                x += 2;
            } 
            else x += 4;
            md++;
        }
        y += 2;
    }
}

static void update_select(cal_view *v, int sel)
{
    v->select = sel;
    struct tm t = {0};
    t.tm_year = v->year - 1900;
    t.tm_mon = v->month - 1;
    t.tm_mday = sel;
    v->selected_date = mktime(&t);
    if (v->onselect)
        (*v->onselect)(v, v->selected_date);
}

static int set_time(cal_view *v, int year, int month)
{
    struct tm *t;
    time_t firstday = year_month(year, month);
    t = localtime(&firstday);
    year = t->tm_year + 1900;
    month = t->tm_mon + 1;
    time_t dt = year_month(year, month + 1) - firstday; // 即使12月 + 1也会是正常的
    int days = (int)(dt / ONE_DAY);
    v->year = year;
    v->month = month;
    v->firstwd = t->tm_wday;
    v->days = days;
    if (v->select > days)
        v->select = days;
    update_select(v, v->select);
}

static int handle(view *v, event *ev)
{
    cal_view *cv = (cal_view*)v;
    switch (ev->key)
    {
    case 'W':
        if (cv->select - 7 > 0)
            update_select(cv, cv->select - 7);
        break; 
    case 'S':
        if (cv->select + 7 <= cv->days)
            update_select(cv, cv->select + 7);
        break;
    case 'A':
        if (cv->select - 1 > 0)
            update_select(cv, cv->select - 1);
        break;
    case 'D':
        if (cv->select + 1 <= cv->days)
            update_select(cv, cv->select + 1);
        break;
    case 'Q':
        set_time(cv, cv->year, cv->month - 1);
        break;
    case 'E':
        set_time(cv, cv->year, cv->month + 1);
        break;
    default:
        return 0;
    }
    return 1;
}

void cal_view_init(cal_view *v, time_t date, int *calmap)
{
    view_init((view*)v);
    v->onselect = NULL;
    v->calmap = calmap;
    v->v.draw_self = draw;
    v->v.handle = handle;
    if (date == 0) {
        time(&date);
    }
    struct tm *t = localtime(&date);
    int year = t->tm_year + 1900;
    int month = t->tm_mon + 1;
    v->select = t->tm_mday;
    set_time(v, year, month);
}

inline char *cal_view_selected_date(cal_view *v)
{
    static char buf[20];
    struct tm *t = localtime(&v->selected_date);
    sprintf(buf, "%d-%02d-%02d ", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    return buf;
}