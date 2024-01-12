#if !defined(CAL_VIEW_H)
#define CAL_VIEW_H

#include "view.h"

typedef struct cal_view
{
    view v;
    int year;
    int month;
    int firstwd;
    int days;
    int select;
    time_t selected_date;
    int *calmap;
    void (*onselect)(struct cal_view *v, time_t date);
} cal_view;

void cal_view_init(cal_view *v, time_t date, int *calmap);
char *cal_view_selected_date(cal_view *v);

#endif // CAL_VIEW_H
