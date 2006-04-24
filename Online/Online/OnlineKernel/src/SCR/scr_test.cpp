#include <cstdio>
#include <cstring>
#include "SCR/scr.h"
using namespace SCR;

static Pasteboard* pb;
typedef Display* PDisplay;
static PDisplay d0, d1, d2, d3, d4, d5;

static void broadcast_handler (char* message)   {
  scrc_begin_pasteboard_update (pb);
  scrc_put_chars (d5, message, NORMAL, 1, 1, 1);
  scrc_end_pasteboard_update (pb);
}
static int show_memory()     {
  int memory;
  char text[20];
  int i;
  static int n = 0;
  scrc_begin_pasteboard_update (pb);
  memory = scrc_memory_of_pasteboard (pb);
  sprintf (text, "%d", memory);
  scrc_put_chars (d4, text, NORMAL, 1, 1, 1);
  memory = scrc_memory_of_display (d0);
  sprintf (text, "%d", memory);
  scrc_put_chars (d4, text, NORMAL, 2, 1, 1);
  memory = scrc_memory_of_display (d1);
  sprintf (text, "%d", memory);
  scrc_put_chars (d4, text, NORMAL, 3, 1, 1);
  memory = scrc_memory_of_display (d2);
  sprintf (text, "%d", memory);
  scrc_put_chars (d4, text, NORMAL, 4, 1, 1);
  memory = scrc_memory_of_display (d3);
  sprintf (text, "%d", memory);
  scrc_put_chars (d4, text, NORMAL, 5, 1, 1);
  memory = scrc_memory_of_display (d4);
  sprintf (text, "%d", memory);
  scrc_put_chars (d4, text, NORMAL, 6, 1, 1);
  memory = scrc_memory_of_display (d5);
  sprintf (text, "%d", memory);
  scrc_put_chars (d5, text, NORMAL, 7, 1, 1);
  
  for (i=0; i<n; i++) scrc_put_char (d4, '.', NORMAL, 8, i+1);
  for (; i<10; i++) scrc_put_char (d4, ' ', NORMAL, 8, i+1);
  n++;
  if (n >= 10) n = 0;
  
  scrc_end_pasteboard_update (pb);
  return 1;
}

static void end_update (Pasteboard* pb) {
  scrc_end_pasteboard_update (pb);
  scrc_fflush (pb);
}

static void wait_next()   {
  end_update (pb);
  show_memory();
  scrc_set_cursor (d3, 1, 1);
  scrc_wait(d3);
}

extern "C" int scr_test(int argc, char** argv)  {
  int pb_rows = 0, pb_cols = 80;
  int i;
  char text[20], font[80], *device;

  if (argc == 2) strcpy (font, argv[1]);
  else strcpy (font, "tcs.font");

  if (argc == 3) device = argv[2];
  else device = 0;
  
  scrc_create_pasteboard (&pb, device, &pb_rows, &pb_cols);
  
  // init_timer (show_memory);
  // scrc_set_broadcast_trapping (pb, broadcast_handler);

  scrc_create_display (&d0, 10, 20, INVERSE, ON, " display 0 ");
  scrc_create_display (&d1, 10, 20, NORMAL, ON, " display 1 ");
  scrc_create_display (&d2, 15, 30, NORMAL, ON, " display 2 ");
  scrc_create_display (&d3, 1,  50, BOLD,   ON, " Doing : ");
  scrc_create_display (&d4, 8,  10, BOLD,   ON, " Memory ");
  scrc_create_display (&d5, 1,  15, BOLD,   ON, " Messages ");

  scrc_paste_display (d3, pb, 2, 20);
  scrc_paste_display (d4, pb, 15, 65);
  scrc_paste_display (d5, pb, 20, 40);

  scrc_put_chars (d3, "Create displays", NORMAL, 1, 1, 1);
  end_update (pb);

  scrc_begin_pasteboard_update (pb);
  scrc_paste_display (d0, pb, 2, 2);
  scrc_paste_display (d1, pb, 5, 5);
  scrc_paste_display (d2, pb, 8, 17);
  scrc_paste_display (d3, pb, 2, 20);
  end_update (pb);

  scrc_put_chars (d1, "normal", NORMAL, 1, 1, 1);
  scrc_put_chars (d1, "underline", UNDERLINE, 2, 1, 1);
  scrc_put_chars (d1, "inverse", INVERSE, 3, 1, 1);
  scrc_put_chars (d1, "bold", BOLD, 4, 1, 1);
  scrc_put_chars (d1, "graphic : ", NORMAL, 5, 1, 0);
  scrc_put_chars (d1, "abcdefghijkl", GRAPHIC, 5, 11, 0);
  scrc_put_chars (d1, "dec_sup : ", NORMAL, 6, 1, 0);
  scrc_put_chars (d1, "abcdefghijkl", FONT_SUPP, 6, 11, 0);
  scrc_load_font (pb, font);
  scrc_put_chars (d1, "user    : ", NORMAL, 7, 1, 0);
  scrc_put_chars (d1, "abcdefghijkl", FONT_USER, 7, 11, 0);

  for (i=1;i<=10;i++)  {
    sprintf (text, "%d----------", i);
    scrc_put_chars (d0, text, BOLD, i, 1, 1);
  }
  for (i=8;i<=10;i++)  {
    sprintf (text, "%d............", i);
    scrc_put_chars (d1, text, NORMAL, i, 1, 1);
  }
  for (i=1;i<=15;i++)  {
    sprintf (text, "%d", i);
    scrc_put_chars (d2, text, NORMAL, i, 1, 1);
  }
  wait_next();

  scrc_put_chars (d3, "Move display 1", NORMAL, 1, 1, 1);
  end_update (pb);

  for (i=6; i<20; i+=4) scrc_move_display (d1, pb, 4, 4);
  for (i=20; i<40; i+=4) scrc_move_display (d1, pb, -4, 4);
  wait_next();

  scrc_put_chars (d3, "Repaste display 0", NORMAL, 1, 1, 1);
  end_update (pb);

  scrc_begin_pasteboard_update (pb);
  scrc_paste_display (d0, pb, 2, 2);
  scrc_paste_display (d3, pb, 2, 20);
  wait_next();

  scrc_put_chars (d3, "Bring it to back", NORMAL, 1, 1, 1);
  end_update (pb);

  scrc_begin_pasteboard_update (pb);
  scrc_bring_display_to_back (d0, pb);
  scrc_paste_display (d3, pb, 2, 20);
  wait_next();

  scrc_put_chars (d3, "And drag it", NORMAL, 1, 1, 1);
  end_update (pb);

  for (i=3; i<20; i+=4) scrc_move_display (d0, pb, 4, 4);
  for (i=20; i<50; i+=4) scrc_move_display (d0, pb, -4, 4);
  scrc_begin_pasteboard_update (pb);
  for (i=50; i>1; i-=4) scrc_move_display (d0, pb, 2, -4);
  wait_next();

  scrc_put_chars (d3, "Repaste display 2", NORMAL, 1, 1, 1);
  end_update (pb);

  scrc_begin_pasteboard_update (pb);
  scrc_paste_display (d2, pb, 15, 15);
  scrc_paste_display (d3, pb, 2, 20);
  wait_next();

  scrc_put_chars (d3, "And change it", NORMAL, 1, 1, 1);
  end_update (pb);
  
  for (i=29; i>5; i -= 4)
    scrc_change_display (d2, i, i);
  wait_next();

  scrc_begin_pasteboard_update (pb);
  scrc_paste_display (d0, pb, 2, 2);
  scrc_paste_display (d3, pb, 2, 20);
  end_update (pb);

  scrc_put_chars (d3, "Delete lines in display 0", NORMAL, 1, 1, 1);
  end_update (pb);

  scrc_delete_line (d0, 10);
  scrc_delete_line (d0, 5);
  scrc_delete_line (d0, 1);
  wait_next();

  scrc_put_chars(d3,"Insert lines in display 0 with scroll down",NORMAL,1,1,1);
  end_update (pb);

  scrc_insert_line (d0, "new line 1 ", INVERSE, 1, MOVE_DOWN);
  scrc_insert_line (d0, "new line 5 ", INVERSE, 5, MOVE_DOWN);
  scrc_insert_line (d0, "new line 10 ", INVERSE, 10, MOVE_DOWN);
  wait_next();

  scrc_put_chars(d3,"Insert lines in display 0 with scroll up",NORMAL,1,1,1);
  end_update (pb);

  scrc_insert_line(d0,"new line 1 ", NORMAL, 1, MOVE_UP);
  scrc_insert_line(d0,"new line 5 ", NORMAL, 5, MOVE_UP);
  scrc_insert_line(d0,"new line 10 ",NORMAL,10, MOVE_UP);
  wait_next();

  scrc_put_chars (d3, "Change pasteboard to 132", NORMAL, 1, 1, 1);
  end_update (pb);

  pb_cols = 132;
  scrc_change_pasteboard (pb, &pb_rows, &pb_cols);
  end_update (pb);
  wait_next();

  scrc_put_chars (d3, "Exit", NORMAL, 1, 1, 1);
  wait_next();
  scrc_clear_screen(pb);
  scrc_delete_pasteboard(pb);
  return 0;
}

