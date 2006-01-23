#include <cstdio>
#include "SCR/scr.h"
#include <termios.h> 

static  struct termios neu, alt; 

void scrc_resetANSI()   {
#define plain()            fputs("\033[0m",stdout)
#define toascii()          fputs("\033(B",stdout)
  int fd = fileno(stdin); 
  fflush (stdout);
  toascii();
  plain ();				/* all attribytes off */
  fflush (stdout);
  fprintf (stdout, "\033>");
  fflush (stdout);
  tcsetattr(fd, TCSANOW, &alt); 
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
  tcgetattr(fd, &alt); 
  neu = alt; 
  neu.c_lflag &= ~(ICANON|ECHO); 
  neu.c_cc[VMIN] = 1;
  neu.c_cc[VTIME] = 0;
  tcsetattr(fd, TCSANOW, &neu); 
}

