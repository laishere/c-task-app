#include "view.h"
#include "./main_screen.h"
#include "view/cal.h"
#include "view/items.h"
#include "view/caltips.h"
#include "view/itemstips.h"
#include "view/esc.h"
#include "view/tips.h"
#include "view/clock.h"
#include "model/item.h"
#include "main.h"

typedef struct screen
{
    view v;
    cal_view cal;
    items_view itemsv;
    caltips_view caltips;
    itemstips_view itemstips;
    esc_view esc;
    tips_view brtips;
    clock_view clock;
    array day_items;
    array month_items;
    char tipstext[50];
    int calmap[32];
    time_t calmap_timestart;
    time_t calmap_timeend;
    int items_version;
    char *init_itemname;
} screen;

static void onselect(cal_view *v, time_t date);
static void draw(view *v)
{
    screen *s = (screen*)v;
    s->itemstips.deleting = s->itemsv.deletettl > 0;
    if (s->items_version != item_version())
        onselect(&s->cal, s->cal.selected_date);
}

static void layout(view *v)
{
    screen *s = (screen*)v;
    int clockw = 20;
    int clockh = 4;
    int gap = 10;
    int calw = 27;
    int calh = 15;
    int itemsw = 20;
    int itemsh = 15;
    int cx = v->frame.width / 2;
    int caly = (v->bounds.height - calh) / 2;
    int itemsy = (v->bounds.height - itemsh) / 2;
    view_setframe((view*)&s->cal, make_rect(cx - gap - calw, caly, calw, calh));
    view_setframe((view*)&s->caltips, make_rect(cx - gap - calw, caly + calh + 2, calw, 2));
    view_setframe((view*)&s->itemsv, 
        make_rect(cx + gap, itemsy, itemsw, itemsh));
    view_setframe((view*)&s->itemstips, make_rect(cx + gap, itemsy + itemsh + 2, itemsw, 2));
    view_setframe((view*)&s->esc, make_rect(0, v->frame.height - 1, 30, 1));
    int brtipsw = 30;
    view_setframe((view*)&s->brtips, make_rect(v->frame.width - brtipsw, v->frame.height - 1, brtipsw, 1));
    view_setframe((view*)&s->clock, make_rect((v->frame.width - clockw) / 2, 2, clockw, clockh));
}

static void deinit(view *v)
{
    screen *s = (screen*)v;
    array_deinit(&s->day_items);
    array_deinit(&s->month_items);
    free(v);
}

static void update_calmap(screen *s)
{
    memset(s->calmap, 0, sizeof(s->calmap));
    for (int i = 0; i < s->month_items.count; i++)
    {
        item *it = (item*)array_get(&s->month_items, i);
        struct tm *t = localtime(&it->time);
        s->calmap[t->tm_mday] = 1;
    }
}

static void monthrange(time_t date, time_t *start, time_t *end)
{
    struct tm *t = localtime(&date);
    struct tm t2 = {0};
    t2.tm_year = t->tm_year;
    t2.tm_mon = t->tm_mon;
    t2.tm_mday = 1;
    *start = mktime(&t2);
    t2.tm_mon += 1;
    *end = mktime(&t2);
}

static int filter_items(void *data, item *it)
{
    screen *s = (screen*)data;
    return it->time >= s->calmap_timestart && it->time < s->calmap_timeend;
}

static void onselect(cal_view *v, time_t date)
{
    screen *s = (screen*)((view*)v)->super;
    time_t start, end;
    monthrange(date, &start, &end);
    if (start != s->calmap_timestart || s->items_version != item_version())
    {
        s->calmap_timestart = start;
        s->calmap_timeend = end;
        array_clear(&s->month_items);
        // 所有数据提前排序了，保证有序
        item_filter(&s->month_items, filter_items, s);
        update_calmap(s);
        s->items_version = item_version();
    }
    array_clear(&s->day_items);
    const time_t ONE_DAY = 86400;
    for (int i = 0; i < s->month_items.count; i++)
    {
        item *it = (item*)array_get(&s->month_items, i);
        if (it->time >= date && it->time < date + ONE_DAY)
            array_push(&s->day_items, it);
    }
    if (s->init_itemname)
    {
        if (*s->init_itemname && s->day_items.count > 0)
        {
            for (int i = 0; i < s->day_items.count; i++)
            {
                item *it = (item*)array_get(&s->day_items, i);
                if (strcmp(it->name, s->init_itemname) == 0)
                {
                    s->itemsv.select = i; // 通过事项名初始化选中
                    break;
                }
            }
        }
        s->init_itemname = NULL;
    }
}

static void esc_exit()
{
    exit_app();
}

static int handle(view *v, event *ev)
{
    screen *s = (screen*)v;
    if (ev->key == 'N')
    {
        goto_add(cal_view_selected_date(&s->cal));
        return 1;
    } 
    if (ev->key == 191)
    {
        goto_search();
        return 1;
    }
    return 0;
}

view *main_screen_new(main_data *data)
{
    screen *s = (screen*)malloc(sizeof(screen));
    view_init((view*)s);
    array_init(&s->day_items, sizeof(item));
    array_init(&s->month_items, sizeof(item));
    memset(s->calmap, 0, sizeof(s->calmap));
    s->init_itemname = data->item_name;
    s->calmap_timestart = 0;
    s->v.layout_subviews = layout;
    s->v.draw_self = draw;
    s->v.deinit = deinit;
    s->v.handle = handle;
    cal_view_init(&s->cal, data->date, s->calmap);
    s->cal.onselect = onselect;
    items_view_init(&s->itemsv, &s->day_items);
    caltips_view_init(&s->caltips);
    itemstips_view_init(&s->itemstips);
    esc_view_init(&s->esc, "Exit", esc_exit);
    clock_view_init(&s->clock);
    strcpy(s->tipstext, "[N] New item  [/] Search");
    tips_view_init(&s->brtips, s->tipstext, CSL_ALIGN_RIGHT);
    view_add_subview((view*)s, (view*)&s->cal);
    view_add_subview((view*)s, (view*)&s->itemsv);
    view_add_subview((view*)s, (view*)&s->caltips);
    view_add_subview((view*)s, (view*)&s->itemstips);
    view_add_subview((view*)s, (view*)&s->esc);
    view_add_subview((view*)s, (view*)&s->brtips);
    view_add_subview((view*)s, (view*)&s->clock);
    onselect(&s->cal, s->cal.selected_date);
    return (view*)s;
}

void main_screen_save(view *v, main_data *data)
{
    screen *s = (screen*)v;
    data->date = s->cal.selected_date;
    int sel = s->itemsv.select;
    if (sel >= 0 && sel < s->day_items.count)
    {
        item *it = (item*)array_get(&s->day_items, sel);
        strcpy(data->item_name, it->name);
    }
}