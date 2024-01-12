#if !defined(CONSOLE_H)
#define CONSOLE_H

#include "geometry.h"
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

typedef enum
{
    STY_NONE,
    STY_CAL_TODAY,
    STY_CAL_SELECTED,
    STY_CAL_HAS_ITEMS,
    STY_CURSOR,
    STY_BTN_NORMAL,
    STY_BTN_FOCUS,
    STY_MSG_ERROR,
    STY_MSG_SUCCESS,
    STY_TIPS,
    STY_ITEM_SELECT,
    STY_ITEM_DELETE,
    STY_DANGER,
    STY_ITEM_NOTIFY1,
    STY_ITEM_NOTIFY2
} csl_style;

typedef enum
{
    CSL_ALIGN_LEFT,
    CSL_ALIGN_CENTER,
    CSL_ALIGN_RIGHT,
} csl_align;

void csl_init();
void csl_moveto(int x, int y);
void csl_updatesize(int *w, int *h);
void csl_clip(rect region);
rect csl_getclip();
void csl_offset(point p);
point csl_getoffset();
void csl_putchar(char c, int x, int y);
void csl_print(char *str, int width, int *x, int *y);
void csl_print_nowrap(char *str, int *x, int *y);
void csl_print_align(char *str, int width, int *y, csl_align align);
void csl_print_center(char *str, int width, int *y);
void csl_fill_whitespace(int n, int x, int y);
void csl_begin_draw();
void csl_sync();
csl_style csl_getstyle();
void csl_setstyle(csl_style style);

#endif // CONSOLE_H
