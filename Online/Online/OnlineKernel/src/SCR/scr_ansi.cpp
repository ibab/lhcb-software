#include <cstdio>
#include "SCR/scr.h"

void scrc_resetANSI()   {
#define plain()            fputs("\033[0m",stdout)
#define toascii()          fputs("\033(B",stdout)
 fflush (stdout);
 toascii();
 plain ();				/* all attribytes off */
 fflush (stdout);
 fprintf (stdout, "\033>");
 fflush (stdout);
}

void scrc_setANSI()   {
/*
 * set terminal eof from <esc> to ^Z,
 * set pause off
 * set echo off 
 * set up intercept to restore terminal parameters
 */ 

 fflush (stdout);
 fprintf (stdout, "\033=");
 fflush (stdout);
}

