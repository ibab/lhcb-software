#include <cstdio>
#include <cstring>
#include <unistd.h>
#include "RTL/rtl.h"
#include "RTL/screen.h"

extern "C" int rtl_testscr(int,char **) {
  char caption[255];
  initscreen();
  printxy(LINES/2, COLS/2-8,"Welcome to Curses!");
  sprintf(caption, "Screen is %d rows by %d cols", LINES, COLS);
  printxy(2+LINES/2, COLS/2-strlen(caption)/2,caption);
  refresh();
  lib_rtl_sleep(1000*100);
  return(0);
}
