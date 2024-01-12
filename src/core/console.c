#include "console.h"

#define CSLBUF_ROW 50
#define CSLBUF_COL 160

typedef struct csl_glyph
{
    char c;
    csl_style style;
} csl_glyph;

static rect screen_clip;
static point screen_offset;
static point cursor;
static size screen_size;
static csl_glyph cslbuf[CSLBUF_ROW][CSLBUF_COL];
static csl_glyph cslbuf_new[CSLBUF_ROW][CSLBUF_COL];
static csl_style cslstyle_cur;
static char cslstyles[30][20];

static void set_encoding()
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
}

static void hide_cursor()
{
    CONSOLE_CURSOR_INFO cursorInfo;
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleCursorInfo(handle, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(handle, &cursorInfo);
}

static void set_async_input()
{
    DWORD mode;
    HANDLE hdl = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hdl, &mode);
    DWORD newmode = mode & (~ENABLE_LINE_INPUT);
    SetConsoleMode(hdl, newmode);
}

static void init_styles()
{
    strcpy(cslstyles[STY_NONE], "\033[0m");
    strcpy(cslstyles[STY_CAL_TODAY], "\033[44m\033[97m");
    strcpy(cslstyles[STY_CAL_SELECTED], "\033[42m\033[97m");
    strcpy(cslstyles[STY_CAL_HAS_ITEMS], "\033[47m\033[30m");
    strcpy(cslstyles[STY_CURSOR], "\033[47m");
    strcpy(cslstyles[STY_BTN_NORMAL], "\033[47m\033[30m");
    strcpy(cslstyles[STY_BTN_FOCUS], "\033[42m");
    strcpy(cslstyles[STY_MSG_ERROR], "\033[101m");
    strcpy(cslstyles[STY_MSG_SUCCESS], "\033[102m");
    strcpy(cslstyles[STY_TIPS], "\033[90m");
    strcpy(cslstyles[STY_ITEM_SELECT], "\033[44m\033[97m");
    strcpy(cslstyles[STY_ITEM_DELETE], "\033[41m\033[97m");
    strcpy(cslstyles[STY_DANGER], "\033[91m");
    strcpy(cslstyles[STY_ITEM_NOTIFY1], "\033[102m");
    strcpy(cslstyles[STY_ITEM_NOTIFY2], "\033[0m");
}

void csl_init()
{
    set_encoding();
    hide_cursor();
    set_async_input();
    init_styles();
    screen_clip = make_rect(0, 0, 0, 0);
    cursor = make_point(0, 0);
    screen_offset = make_point(0, 0);
    memset(cslbuf, ' ', sizeof(cslbuf));
}

void csl_moveto(int x, int y)
{
    COORD coord;
    coord.X = x; 
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    cursor.x = x;
    cursor.y = y;
}

void csl_updatesize(int *w, int *h)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *w = csbi.dwSize.X;
    *h = csbi.dwSize.Y;
    size newsize = {*w, *h};
    cursor.x = csbi.dwCursorPosition.X;
    cursor.y = csbi.dwCursorPosition.Y;
    if (screen_size.width != newsize.width || screen_size.height != newsize.height)
    {
        // 尺寸改变重绘整个屏幕
        system("cls");
        memset(cslbuf, 0, sizeof(cslbuf));
        screen_size = newsize;
    }
}

inline void csl_clip(rect region)
{
    screen_clip = region;
}

inline rect csl_getclip()
{
    return screen_clip;
}

inline void csl_offset(point p)
{
    screen_offset = p;
}

inline point csl_getoffset()
{
    return screen_offset;
}

static void csl_write(csl_glyph *g, int x, int y)
{
    if (x != cursor.x || y != cursor.y)
        csl_moveto(x, y);
    printf("%c", g->c == 0 ? ' ' : g->c);
    if (++x >= screen_size.width)
    {
        x = 0;
        y++;
    }
    cursor.x = x;
    cursor.y = y;
}

void csl_putchar(char c, int x, int y)
{
    x += screen_offset.x;
    y += screen_offset.y;
    if (!rect_inside(&screen_clip, x, y))
        return;
    cslbuf_new[y][x].c = c == ' ' ? 0 : c; // 统一用0表示
    cslbuf_new[y][x].style = cslstyle_cur;
}

void csl_print(char *str, int width, int *x, int *y)
{
    while (*str != '\0')
    {
        char c = *str;
        str++;
        if (c == '\n')
        {
            *x = 0;
            (*y)++;
            continue;
        }
        csl_putchar(c, *x, *y);
        if (++(*x) >= width)
        {
            *x = 0;
            (*y)++;
        }
    }
}

void csl_print_nowrap(char *str, int *x, int *y)
{
    while (*str != '\0' && *str != '\n')
    {
        char c = *str;
        str++;
        csl_putchar(c, *x, *y);
        (*x)++;
    }
}

void csl_print_align(char *str, int width, int *y, csl_align align)
{
    char buf[200];
    strcpy(buf, str);
    str = buf;
    char *linebegin = str;
    int linelen = 0;
    while (1)
    {
        char c = *str;
        if (c == '\0' || c == '\n')
        {
            *str = '\0';
            int x = 0;
            if (align == CSL_ALIGN_CENTER)
                x = (width - linelen) / 2;
            else if (align == CSL_ALIGN_RIGHT)
                x = width - linelen;
            csl_print_nowrap(linebegin, &x, y);
            (*y)++;
            linelen = 0;
            linebegin = str + 1;
            if (c == '\0')
                break;
        }
        linelen++;
        str++;
    }
}

void csl_print_center(char *str, int width, int *y)
{
    csl_print_align(str, width, y, CSL_ALIGN_CENTER);
}

void csl_fill_whitespace(int n, int x, int y)
{
    for (int i = 0; i < n; i++)
        csl_putchar(' ', x + i, y);
}

inline csl_style csl_getstyle()
{
    return cslstyle_cur;
}

inline void csl_setstyle(csl_style style)
{
    cslstyle_cur = style;
}

void csl_begin_draw()
{
    memset(cslbuf_new, 0, sizeof(cslbuf_new));
    csl_setstyle(STY_NONE);
}

static inline int glyph_eq(csl_glyph *a, csl_glyph *b)
{
    return a->c == b->c && a->style == b->style;
}

static inline void use_style(csl_style s)
{
    printf(cslstyles[s]);
}

void csl_sync()
{
    csl_style active = STY_NONE;
    use_style(active);
    for (int i = 0; i < screen_size.height; i++)
    {
        for (int j = 0; j < screen_size.width; j++)
        {
            csl_glyph *a = cslbuf[i] + j;
            csl_glyph *b = cslbuf_new[i] + j;
            if (!glyph_eq(a, b))
            {
                *a = *b;
                if (active != a->style)
                {
                    if (a->style != STY_NONE)
                        use_style(STY_NONE); // 先清除样式
                    use_style(a->style);
                    active = a->style;
                }
                csl_write(a, j, i);
            }
        }
    }
    use_style(STY_NONE);
}