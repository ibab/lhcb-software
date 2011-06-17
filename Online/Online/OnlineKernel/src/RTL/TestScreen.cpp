#include <cstdio>
#include <cstring>
#include "RTL/rtl.h"
#include "RTL/strdef.h"
#include "SCR/scr.h"

extern "C" int rtl_testscreen(int argc,char **) {
  char caption[255];
  ::snprintf(caption, sizeof(caption), "This is just a message!");
#if 0
  initscreen();
  printxy(10, 20,"Welcome to Curses!");
  printxy(10, 22,caption);
  printxy(1,  33,".....");
  refresh();
#endif
  using namespace SCR;
  Pasteboard* pasteboard;
  Display*    display;
  int height, width;
  ::scrc_create_pasteboard (&pasteboard, 0, &height, &width);
  ::scrc_create_display (&display, height-2, width-2, NORMAL, ON, "SCR test program");
  ::scrc_paste_display  (display, pasteboard, 2, 2);
  ::scrc_put_chars(display,caption, NORMAL, 2, 2, 0);
  ::scrc_end_pasteboard_update(pasteboard);
  ::scrc_fflush(pasteboard);
  ::scrc_set_cursor(display, 2, 10);
  ::scrc_cursor_off(pasteboard);

  if ( argc == 0 ) lib_rtl_sleep(1000*20);
  return 0;
}
