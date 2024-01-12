#include "console.h"
#include "screen/main_screen.h"
#include "screen/add_screen.h"
#include "screen/search_screen.h"
#include <Windows.h>
#include <stdio.h>
#include "model/item.h"

#define DELAY 20

static int alive;
static int current_screen;
static int switch_screen;
static view *root;
static view *switch_root;
static main_data mdata;

enum
{
    SCREEN_MAIN,
    SCREEN_ADD,
    SCREEN_SEARCH
};

static int handle_event(view *root, INPUT_RECORD *r)
{
    if (r->EventType != KEY_EVENT || !r->Event.KeyEvent.bKeyDown)
        return 0;
    event ev;
    ev.key = r->Event.KeyEvent.wVirtualKeyCode;
    ev.achar = r->Event.KeyEvent.uChar.AsciiChar;
    ev.uchar = r->Event.KeyEvent.uChar.UnicodeChar;
    return view_handle(root, &ev);
}

static view *process_switch_screen()
{
    if (root)
    {
        if (current_screen == SCREEN_MAIN)
            main_screen_save(root, &mdata); // 保存主页数据
        view_deinit(root);
    }
    root = switch_root;
    current_screen = switch_screen;
    switch_screen = -1;
    switch_root = NULL;
}

void goto_main(time_t date, char *item_name)
{
    if (date)
        mdata.date = date;
    if (item_name)
        strcpy(mdata.item_name, item_name);
    switch_root = main_screen_new(&mdata);
    switch_screen = SCREEN_MAIN;
}

void goto_add(char *date)
{
    switch_root = add_screen_new(date);
    switch_screen = SCREEN_ADD;
}

void goto_search()
{
    switch_root = search_screen_new();
    switch_screen = SCREEN_SEARCH;
}

void exit_app()
{
    alive = 0;
}

int ttl(int t)
{
    return t / DELAY;
}

static void initsound()
{
    mciSendString(L"open Clock-sound-effect.mp3 alias clock", NULL, 0, 0);
}

static int isplaying = 0;
static void playsound()
{
    if (isplaying)
        return;
    mciSendString(L"play clock repeat", NULL, 0, 0);
    isplaying = 1;
}

static void stopsound()
{
    if (!isplaying)
        return;
    mciSendString(L"stop clock", NULL, 0, 0);
    isplaying = 0;
}

static void checknotify()
{
    if (item_notify_list()->count > 0)
        playsound();
    else
        stopsound();
}

int main()
{
    alive = 1;
    csl_init();
    item_load();
    char buf[100];
    rect screen_frame = {0, 0, 0, 0};
    DWORD read;
    INPUT_RECORD rec;
    HANDLE hin = GetStdHandle(STD_INPUT_HANDLE);
    int bufttl = 0;
    root = NULL;
    current_screen = -1;
    mdata.date = 0;
    mdata.item_name[0] = '\0';
    goto_main(0, NULL);
    initsound();
    while (alive)
    {
        checknotify();
        if (switch_root)
        {
            process_switch_screen();
        }
        PeekConsoleInput(hin, &rec, 1, &read);
        csl_begin_draw();
        if (read)
        {
            int c = -1;
            ReadConsoleInput(hin, &rec, 1, &read);
            int handled = handle_event(root, &rec);
            csl_moveto(0, 0);
            sprintf(buf, "key %d c %d handled %d\n", rec.Event.KeyEvent.wVirtualKeyCode, rec.Event.KeyEvent.uChar.AsciiChar, handled);
            bufttl = ttl(1000);
        }
        if (bufttl)
        {
            int x, y;
            x = y = 0;
            csl_print_nowrap(buf, &x, &y);
            bufttl--;
        }
        csl_updatesize(&screen_frame.width, &screen_frame.height);
        csl_clip(screen_frame);
        view_setframe(root, screen_frame);
        view_draw(root);
        csl_sync();
        Sleep(DELAY);
    }
    return 0;
}