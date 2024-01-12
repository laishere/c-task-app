#include "./item.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#define SAVE_PATH "./save.csv"

static array items;
static array notifylist;
static int version;
static time_t notifylist_updatetime;

static inline int time_cmp(time_t a, time_t b)
{
    if (a < b)
        return -1;
    if (a > b)
        return 1;
    return 0;
}

void item_load()
{
    array_init(&items, sizeof(item));
    array_init(&notifylist, sizeof(item));
    notifylist_updatetime = 0;
    FILE *f = fopen(SAVE_PATH, "rb");
    if (!f)
        return;
    item it;
    char buf[200];
    while (fgets(buf, 200, f))
    {
        char *p = buf;
        char *d = strchr(p, ',');
        *d = '\0';
        strcpy(it.name, p);
        p = d + 1;
        sscanf(p, "%lld,%d", &it.time, &it.notify);
        array_push(&items, &it);
    }
    fclose(f);
    item_sort_by_time(&items, 1);
}

void item_save()
{
    FILE *f = fopen(SAVE_PATH, "wb");
    if (!f)
        return;
    for (int i = 0; i < items.count; i++)
    {
        item *it = (item*)array_get(&items, i);
        fprintf(f, "%s,%lld,%d\n", it->name, it->time, it->notify);
    }
    fclose(f);
}

static int find_by_name(char *name)
{
    for (int i = 0; i < items.count; i++)
    {
        item *it = (item*)array_get(&items, i);
        if (strcmp(it->name, name) == 0)
            return i;
    }
    return -1;
}

item *item_find_by_name(char *name)
{
    int i = find_by_name(name);
    if (i != -1)
        return (item*)array_get(&items, i);
    return NULL;
}

static int notifytime_cmp(void *a, void *b)
{
    item *it1 = (item*)a;
    item *it2 = (item*)b;
    return time_cmp(it1->time - it1->notify, it2->time - it2->notify);
}

static inline int should_notify(time_t now, item *it)
{
    time_t notifytime = it->time - it->notify;
    // 通知时间30s
    return now >= notifytime && now <= notifytime + 30;
}

static void update_notifylist()
{
    time_t now;
    time(&now);
    array_clear(&notifylist);
    for (int i = 0; i < items.count; i++)
    {
        item *it = (item*)array_get(&items, i);
        if (should_notify(now, it))
            array_push(&notifylist, it);
    }
    array_sort(&notifylist, notifytime_cmp);
    notifylist_updatetime = now;
}

static void items_changed()
{
    item_sort_by_time(&items, 1);
    item_save();
    version++;
    update_notifylist();
}

void item_add(item it)
{
    array_push(&items, &it);
    items_changed();   
}

void item_delete_by_name(char *name)
{
    int i = find_by_name(name);
    if (i != -1)
    {
        array_remove(&items, i);
        items_changed();   
    }
}

array *item_getall()
{
    return &items;
}

void item_filter(array *result, int (*f)(void *, item *), void *data)
{
    for (int i = 0; i < items.count; i++)
    {
        item *it = (item*)array_get(&items, i);
        if ((*f)(data, it))
            array_push(result, it);
    }
}

inline int item_version()
{
    return version;
}

inline static int cmp_time_asc(void *a, void *b)
{
    item *it1 = (item*)a;
    item *it2 = (item*)b;
    return time_cmp(it1->time, it2->time);
}

inline static int cmp_time_dsc(void *a, void *b)
{
    return cmp_time_asc(b, a);
}

void item_sort_by_time(array *a, int asc)
{
    array_sort(a, asc ? cmp_time_asc : cmp_time_dsc);
}

array *item_notify_list()
{
    time_t now;
    time(&now);
    if (now > notifylist_updatetime + 1) // 每1s更新一次
        update_notifylist();
    return &notifylist;
}

int item_should_notify(item *it)
{
    time_t now;
    time(&now);
    return should_notify(now, it);
}