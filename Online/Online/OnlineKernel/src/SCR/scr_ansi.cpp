#include <cstdio>
#include "SCR/scr.h"

#ifdef _WIN32
#elif __linux
#include "termios.h"
static struct termios neu, alt; 
#endif 

void scrc_resetANSI()   {
#define plain()            fputs("\033[0m",stdout)
#define changetoascii()    fputs("\033(B",stdout)
  int fd = fileno(stdin); 
  fflush (stdout);
  changetoascii();
  plain ();				/* all attribytes off */
  fflush (stdout);
  fprintf (stdout, "\033>");
  fflush (stdout);
#ifndef _WIN32
  tcsetattr(fd, TCSANOW, &alt); 
#endif
}

void scrc_setANSI()   {
/*
 * set terminal eof from <esc> to ^Z,
 * set pause off
 * set echo off 
 * set up intercept to restore terminal parameters
 */ 
  int fd = fileno(stdin); 
  fflush (stdout);
  fprintf (stdout, "\033=");
  fflush (stdout);
#ifndef _WIN32
  tcgetattr(fd, &alt); 
  neu = alt; 
  neu.c_lflag &= ~(ICANON|ECHO); 
  neu.c_cc[VMIN] = 1;
  neu.c_cc[VTIME] = 0;
  tcsetattr(fd, TCSANOW, &neu); 
#endif
}

