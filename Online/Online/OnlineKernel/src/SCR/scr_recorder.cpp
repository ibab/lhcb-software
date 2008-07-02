#include <ctime>
#include <cstdio>
#include <cstring>
#include "SCR/scr.h"
#include "WT/wtdef.h"
#include "sys/time.h"
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

struct MouseEvent {
  unsigned char button;
  unsigned char modifier;
  unsigned char x;
  unsigned char y;
  unsigned int  usec;
  MouseEvent(const char* data);
};

MouseEvent::MouseEvent(const char* data) {
  button = data[0]&0x3;
  modifier = data[0]>>2;
  x = data[1]-0x20;
  y = data[2]-0x20;
}

static void key_handler(char c1) {
  unsigned char c = c1;
  static char mouse_data[6] = {0,0,0,0,0,0};
  static bool pressed = false;
  static timeval click = {0,0};
  char txt[20];
  ::sprintf(txt," K'%X/%c' ",c,c);
  addText(txt);
  mouse_data[0] = mouse_data[1];
  mouse_data[1] = mouse_data[2];
  mouse_data[2] = mouse_data[3];
  mouse_data[3] = mouse_data[4];
  mouse_data[4] = mouse_data[5];
  mouse_data[5] = c;
  if ( mouse_data[0] == 0x1B && mouse_data[1] == '[' && mouse_data[2] == 'M' ) {
    // Button pressed
    timeval now, diff;
    ::gettimeofday(&now,0);
    timersub(&now,&click,&diff);
    if ( diff.tv_sec==0 && diff.tv_usec<300000 && pressed ) {
      MouseEvent* e = new MouseEvent(mouse_data+3);
      e->usec = diff.tv_usec;
      ::wtc_insert(WT_FACILITY_X,e);
      pressed = false;
      click.tv_sec = 0;
      click.tv_usec = 0;
    }
    else {
      click = now;
      pressed = true;
    }
  }
}

static int mouse_action(unsigned int, void* param) {
  MouseEvent* e = (MouseEvent*)param;
  char txt[132];
  sprintf(txt,"((Mouse event: %d  %d x:%d y:%d  %d))",
	  e->button,e->modifier,e->x,e->y,e->usec);
  addText(txt);
  delete e;
  return WT_SUCCESS;
}

static int mouse_rearm(unsigned int, void* param) {
  return WT_SUCCESS;
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

  ::wtc_subscribe(WT_FACILITY_X,mouse_rearm,mouse_action,0);

  //::scrc_putes("[?1000h",pb);
  ::scrc_putes("[?9h",pb);
  while(1) {
    key = ::scrc_wait(d0);
    ::sprintf(txt," B'%X/%c' ",(unsigned char)key,(unsigned char)key);
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

