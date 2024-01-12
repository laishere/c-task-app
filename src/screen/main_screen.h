#if !defined(MAIN_SCREEN_H)
#define MAIN_SCREEN_H

#include "view.h"

typedef struct
{
    time_t date;
    char item_name[50];
} main_data;

view *main_screen_new(main_data *data);
void main_screen_save(view *v, main_data *data);

#endif // MAIN_SCREEN_H
