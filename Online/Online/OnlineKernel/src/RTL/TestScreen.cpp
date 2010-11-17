#include <cstdio>
#include <cstring>
#include "RTL/rtl.h"
#include "RTL/screen.h"

extern "C" int rtl_testscreen(int argc,char **) {
  char caption[255];
  initscreen();
  printxy(10, 20,"Welcome to Curses!");
  snprintf(caption, sizeof(caption), "This is just a message!");
  printxy(10, 22,caption);
  printxy(1,  33,".....");
  refresh();
  if ( argc == 0 ) lib_rtl_sleep(1000*20);
  return 0;
}
