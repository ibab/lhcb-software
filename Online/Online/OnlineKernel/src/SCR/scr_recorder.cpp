#include <cstdio>
#include <cstring>
#include "SCR/scr.h"
using namespace SCR;

static Pasteboard* pb;
static Display* d0;

typedef void (*Key_Handler)(char);
Key_Handler User_Key_Handler = 0;
int scrc_set_user_key_handler(Key_Handler handler);

static int l, key, pb_rows = 0, pb_cols = 80, posx=1, posy=1;

static void addText(const char* txt) {
  l = ::strlen(txt);
  if ( posx+l > pb_cols-2 ) { 
    posx = 1;
    ++posy;
    if ( posy >= pb_rows-2) {
      posy = 1;
    }
  }
  ::scrc_begin_pasteboard_update (pb);
  ::scrc_put_chars (d0, txt, NORMAL, posy, posx, 1);
  ::scrc_end_pasteboard_update (pb);
  ::scrc_fflush (pb);
  posx += l;
}

static void key_handler(char c) {
  char txt[20];
  ::sprintf(txt," K'%X/%c' ",int(c),c);
  addText(txt);
}

extern "C" int scr_recorder(int, char**)  {
  char txt[20];

  ::scrc_create_pasteboard (&pb, 0, &pb_rows, &pb_cols);
  ::scrc_create_display (&d0, pb_rows-2, pb_cols-2, BOLD, ON, " Key stroke recorder: CTRL-E to exit");
  ::scrc_set_user_key_handler(key_handler);
  ::scrc_begin_pasteboard_update (pb);
  ::scrc_paste_display (d0, pb, 2, 2);
  ::scrc_end_pasteboard_update (pb);
  ::scrc_fflush (pb);

  while(1) {
    key = ::scrc_wait(d0);
    ::sprintf(txt," B'%X/%c' ",key,key);
    addText(txt);
    if (key==CTRL_E) break;
  }
  ::scrc_unpaste_display(d0,pb);
  ::scrc_delete_display(d0);
  ::scrc_clear_screen(pb);
  ::scrc_delete_pasteboard(pb);
  ::scrc_resetANSI();
  return 0;
}

