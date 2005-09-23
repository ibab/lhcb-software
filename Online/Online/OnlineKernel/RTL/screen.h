#ifndef _RTL_SCREEN_H
#define _RTL_SCREEN_H

#include <cstdio>

#if defined(_WIN32)

#include "RTL/conioex.h"

//#elif defined(VT100)
#else

inline void dbl_hi_top()     { ::printf("\033#3"); }
inline void dbl_hi_botm()    { ::printf("\033#4"); }
inline void double_height()         { ::printf("\033#6"); }
inline void single_heihgt()         { ::printf("\033#5"); }
inline void bold()           { ::printf("\033[1m"); }

inline void flashing()       { ::printf("\033[5m"); }
inline void underline()      { ::printf("\033[4m"); }
inline void inverse()        { ::printf("\033[7m"); }
inline void plain()          { ::printf("\033[0m"); }
inline void clrscr()              { ::printf("\033[2J"); }
inline void gotoxy(int x,int y)   { ::printf("\033[%d;%dH",x,y); }
inline void scroll(int top,int bot)  { ::printf("\033[%d;%dr",top,bot); }
inline void beep()           { ::printf("\007"); }
inline void cr()             { ::printf("\n"); }
inline void ascii()          { ::printf("\033(B"); }
inline void graphics()       { ::printf("\033(0"); }
inline void col80()          { ::printf("\033[?2h"); }

#define YELLOW 1
#define RED 2
#define BLUE 3
#define _NOCURSOR 0
#define _NORMALCURSOR 1
inline void _setcursortype(int) {}
inline void textcolor(int) {}
inline void textbackground(int) {}

#define g_blank      95
#define g_diam       96
#define g_check      97
#define g_ht         98
#define g_ff         99
#define g_cr         100
#define g_lf         101
#define g_degree     102
#define g_plusminus  103
#define g_nl         104
#define g_vt         105
#define g_brc        106
#define g_trc        107
#define g_tlc        108
#define g_blc        109
#define g_cross      110
#define g_sc1        111
#define g_sc3        112
#define g_sc5        113
#define g_sc7        114
#define g_sc9        115
#define g_lj         116
#define g_rj         117
#define g_bj         118
#define g_tj         119
#define g_bar        120
#define g_leq        121
#define g_geq        122
#define g_pi         123
#define g_neq        124
#define g_pound      125
#define g_tit        126
#define g_del        127
                                                
#endif

#endif // _RTL_SCREEN_H
