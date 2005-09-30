#include <cstdio>
#include <cstring>
#include "RTL/rtl.h"
#include "RTL/screen.h"

extern "C" int rtl_testscr(int,char **) {
  char caption[255];
  initscreen();
  printxy(10, 20,"Welcome to Curses!");
  sprintf(caption, "This is just a message!");
  printxy(11, 20,caption);
  refresh();
  lib_rtl_sleep(1000*100);
  return(0);
}
