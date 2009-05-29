#ifndef _RTL_SCREEN_H
#define _RTL_SCREEN_H

#include <cstdio>

#if defined(_WIN32)

#include "RTL/conioex.h"
typedef void* WINDOW;
static inline void ascii()          {  }
static inline void graphics()       {  }
static inline void refresh()        {  }
static inline WINDOW* initscreen()  { clrscr(); return 0; }
static inline void printxy(int x, int y, const char* buff) {  ::gotoxy(x,y);  ::printf(buff); }
static inline void print_char(int x, int y, int c) {  ::gotoxy(x,y); ::putchar(c); }
static inline size_t term_height() {  return   24; }
static inline size_t term_width()  {   return 132; }
#define TERM_WIDTH  (size_t)132
#define TERM_HEIGHT  (size_t)24

#define NORMAL          0
#define BOLD            0
#define REVERSE         0

#define VERT_BAR       (0xba)
#define HORZ_BAR       (0xcd)

#define FAT_VERT_BAR   (0xdb)
#define DIM_VERT_BAR   (0xb0)

#define LEFT_UP_EDGE     (0xc9)
#define RIGHT_UP_EDGE    (0xbb)
#define LEFT_LOW_EDGE    (0xc8)
#define RIGHT_LOW_EDGE   (0xbc)
#define TEE_LEFT         (0xcc)
#define TEE_RIGHT        (0xb9)
#define TEE_LEFT_SIMPLE  (0xc3)
#define TEE_RIGHT_SIMPLE (0xb4)

#else

#include <curses.h>

#define clrscr    clear
#define RED     COLOR_RED
#define BLUE    COLOR_BLUE
#define YELLOW  COLOR_YELLOW
#define BLACK   COLOR_BLACK
#define LIGHTGREEN COLOR_GREEN
static inline WINDOW* initscreen()  {  WINDOW* w=initscr(); clrscr(); return w; }
static inline void gotoxy(int x, int y) { ::move(x-1,y-1); }
static inline void printxy(int x, int y, const char* buff) { ::mvprintw(y-1,x-1,buff); }
static inline void printxyw(WINDOW* w,int x, int y, const char* buff) {  ::mvwprintw(w,x-1,y-1,buff); }

#define _NOCURSOR 0
#define _NORMALCURSOR 1

static inline void _setcursortype(int) {}
static inline void textcolor(int) {}
static inline void textbackground(int) {}
static inline void ascii()          {  }
static inline void graphics()       {  }
static inline void print_char(int x, int y, int c) {  mvaddch(y, x-1, c); }
static inline size_t term_height() {  return LINES; }
static inline size_t term_width()  {   return COLS; }
#define TERM_WIDTH  (size_t)COLS
#define TERM_HEIGHT (size_t)LINES

#define NORMAL          A_NORMAL
#define BOLD            A_BOLD
#define REVERSE         A_REVERSE

#define VERT_BAR        ACS_VLINE
#define HORZ_BAR        ACS_HLINE
#define FAT_VERT_BAR   (A_REVERSE|' ')
#define DIM_VERT_BAR   (A_UNDERLINE|'.')
#define LEFT_UP_EDGE    ACS_ULCORNER
#define RIGHT_UP_EDGE   ACS_URCORNER
#define LEFT_LOW_EDGE   ACS_LLCORNER
#define RIGHT_LOW_EDGE  ACS_LRCORNER
#define TEE_LEFT        ACS_LTEE
#define TEE_RIGHT       ACS_RTEE

#endif

#endif // _RTL_SCREEN_H
