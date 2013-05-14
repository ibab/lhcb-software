#include <cstdio>
#include <cstring>
#include "SCR/scr.h"
#include "RTL/rtl.h"
using namespace SCR;
#ifdef _WIN32
#define popen _popen
#endif
static Pasteboard* pb;
typedef Display* PDisplay;
static PDisplay d0, d1, d2, d3, d4, d5;
#if 0 // not used
static void broadcast_handler (char* message)   {
  scrc_begin_pasteboard_update (pb);
  scrc_put_chars (d5, message, NORMAL, 1, 1, 1);
  scrc_end_pasteboard_update (pb);
}
#endif
static bool s_showMem = true;
static int show_memory()     {
  int memory;
  char text[64];
  int i;
  static int n = 0;
  if ( s_showMem ) {
    scrc_begin_pasteboard_update (pb);
    memory = scrc_memory_of_pasteboard (pb);
    sprintf (text, "Pb:%d", memory);
    scrc_put_chars (d4, text, NORMAL, 1, 1, 1);
    memory = scrc_memory_of_display (d0);
    sprintf (text, "D0:%d", memory);
    scrc_put_chars (d4, text, NORMAL|CYAN, 2, 1, 1);
    memory = scrc_memory_of_display (d1);
    sprintf (text, "D1:%d", memory);
    scrc_put_chars (d4, text, NORMAL|MAGENTA, 3, 1, 1);
    memory = scrc_memory_of_display (d2);
    sprintf (text, "D2:%d", memory);
    scrc_put_chars (d4, text, NORMAL, 4, 1, 1);
    memory = scrc_memory_of_display (d3);
    sprintf (text, "D3:%d", memory);
    scrc_put_chars (d4, text, 0x200|RED, 5, 1, 1);
    memory = scrc_memory_of_display (d4);
    sprintf (text, "D4:%d", memory);
    scrc_put_chars (d4, text, BOLD|INVERSE|RED, 6, 1, 1);
    memory = scrc_memory_of_display (d5);
    sprintf (text, "D5:%d", memory);
    scrc_put_chars (d5, text, NORMAL, 7, 1, 1);
    
    for (i=0; i<n; i++) scrc_put_char (d4, '.', INVERSE|BLUE, 8, i+1);
    for (; i<10; i++) scrc_put_char (d4, ' ', NORMAL, 8, i+1);
    n++;
    if (n >= 10) n = 0;    
    scrc_end_pasteboard_update (pb);
  }
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

static void help() {}

extern "C" int scr_test(int argc, char** argv)  {
  int i, pb_rows = 0, pb_cols = 80;
  char text[20], font[80];
  const char *device = 0;
  std::string arg;
  RTL::CLI cli(argc, argv, help);
  cli.getopt("font",1,arg="tcs.font");
  ::strcpy (font, arg.c_str());
  s_showMem = cli.getopt("nomemory",1) == 0;
  device = cli.getopt("device",1);
  
  scrc_create_pasteboard (&pb, device, &pb_rows, &pb_cols);
  
  // init_timer (show_memory);
  // scrc_set_broadcast_trapping (pb, broadcast_handler);

  scrc_create_display (&d0, 11, 20, INVERSE|BLUE, ON, " display 0 ");
  scrc_create_display (&d1, 15, 20, NORMAL, ON, " display 1 ");
  scrc_create_display (&d2, 15, 30, NORMAL|CYAN, ON, " display 2 ");
  scrc_create_display (&d3, 1,  50, BOLD|RED|INVERSE,   ON, " Doing : ");
  scrc_create_display (&d4, 8,  32, INVERSE,   ON, " Memory ");
  scrc_create_display (&d5, 1,  15, BOLD,   ON, " Messages ");

  scrc_paste_display (d3, pb, 2, 20);
  scrc_paste_display (d4, pb, 15, 65);
  scrc_paste_display (d5, pb, 20, 40);
  sprintf(text,"Create displays:%X",RED);
  scrc_put_chars (d3, text, NORMAL|MAGENTA, 1, 1, 1);
  end_update (pb);

  wait_next();

  scrc_begin_pasteboard_update (pb);
  scrc_paste_display (d0, pb, 2, 2);
  scrc_paste_display (d1, pb, 5, 5);
  scrc_paste_display (d2, pb, 8, 17);
  scrc_paste_display (d3, pb, 2, 20);
  end_update (pb);

  for (i=1;i<=10;i++)  {
    sprintf (text, "%d----------", i);
    scrc_put_chars (d0, text,BOLD|INVERSE|MAGENTA, i, 1, 1);
  }
  for (i=13;i<=15;i++)  {
    sprintf (text, "%d............", i);
    scrc_put_chars (d1, text, NORMAL, i, 1, 1);
  }
  for (i=1;i<=15;i++)  {
    sprintf (text, "%d", i);
    if ( (i%2) == 0 )
      scrc_put_chars (d2, text, BOLD|BLUE, i, 1, 1);
    else
      scrc_put_chars (d2, text, INVERSE|RED, i, 1, 1);
  }

  scrc_put_chars (d1, "normal", NORMAL, 1, 1, 1);
  scrc_put_chars (d1, "underline", UNDERLINE, 2, 1, 1);
  scrc_put_chars (d1, "inverse", INVERSE, 3, 1, 1);
  scrc_put_chars (d1, "bold", BOLD, 4, 1, 1);
  //  scrc_put_chars (d1, "graphic : ", NORMAL, 5, 1, 0);
  //  scrc_put_chars (d1, "abcdefghijkl", GRAPHIC, 5, 11, 0);
  scrc_put_chars (d1, "dec_sup : ", NORMAL, 6, 1, 0);
  scrc_put_chars (d1, "abcdefghijkl", FONT_SUPP, 6, 11, 0);
  scrc_load_font (pb, font);
  scrc_put_chars (d1, "user    : ", NORMAL, 7, 1, 0);
  scrc_put_chars (d1, "abcdefghijkl", FONT_USER, 7, 11, 0);
  scrc_put_chars (d1, "RED    : ", RED, 8, 1, 0);
  scrc_put_chars (d1, "abcdefghijkl", INVERSE|RED, 8, 11, 0);
  scrc_put_chars (d1, "GREEN  : ", GREEN, 9, 1, 0);
  scrc_put_chars (d1, "abcdefghijkl", INVERSE|GREEN, 9, 11, 0);
  scrc_put_chars (d1, "BLUE   : ", BLUE, 10, 1, 0);
  scrc_put_chars (d1, "abcdefghijkl", INVERSE|BLUE, 10, 11, 0);
  scrc_put_chars (d1, "MAGENTA: ", MAGENTA, 11, 1, 0);
  scrc_put_chars (d1, "abcdefghijkl", INVERSE|MAGENTA, 11, 11, 0);
  scrc_put_chars (d1, "RED    : ", BOLD|RED, 12, 1, 0);
  scrc_put_chars (d1, "abcdefghijkl", BOLD|INVERSE|RED, 12, 11, 0);

  wait_next();

  scrc_put_chars (d3, "Move display 1", NORMAL|RED, 1, 1, 1);
  end_update (pb);

  for (i=6; i<20; i+=4) scrc_move_display (d1, pb, 4, 4);
  for (i=20; i<40; i+=4) scrc_move_display (d1, pb, -4, 4);
  wait_next();

  scrc_put_chars (d3, "Repaste display 0", NORMAL|BLUE, 1, 1, 1);
  end_update (pb);

  scrc_begin_pasteboard_update (pb);
  scrc_paste_display (d0, pb, 2, 2);
  scrc_paste_display (d3, pb, 2, 20);
  wait_next();

  scrc_put_chars (d3, "Bring it to back", NORMAL|CYAN|INVERSE, 1, 1, 1);
  end_update (pb);

  scrc_begin_pasteboard_update (pb);
  scrc_bring_display_to_back (d0, pb);
  scrc_paste_display (d3, pb, 2, 20);
  wait_next();

  scrc_put_chars (d3, "And drag it", NORMAL|YELLOW|INVERSE, 1, 1, 1);
  end_update (pb);

  for (i=3; i<20; i+=4) scrc_move_display (d0, pb, 4, 4);
  for (i=20; i<50; i+=4) scrc_move_display (d0, pb, -4, 4);
  scrc_begin_pasteboard_update (pb);
  for (i=50; i>1; i-=4) scrc_move_display (d0, pb, 2, -4);
  wait_next();

  scrc_put_chars (d3, "Repaste display 2", NORMAL|GREEN, 1, 1, 1);
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

  scrc_insert_line (d0, "new line 1 ", NORMAL, 1, MOVE_DOWN);
  scrc_insert_line (d0, "new line 5 ", CYAN, 5, MOVE_DOWN);
  scrc_insert_line (d0, "new line 10 ",CYAN, 10, MOVE_DOWN);
  scrc_insert_line (d0, "new line 11 ",GREEN, 11, MOVE_DOWN);
  wait_next();

  scrc_put_chars(d3,"Insert lines in display 0 with scroll up",NORMAL,1,1,1);
  end_update (pb);

  scrc_insert_line(d0,"new line 1 ", NORMAL, 1, MOVE_DOWN);
  scrc_insert_line(d0,"new line 5 ", NORMAL|YELLOW|INVERSE, 5, MOVE_UP);
  scrc_insert_line(d0,"new line 10 ",NORMAL|RED,10, MOVE_UP);
  wait_next();

  scrc_put_chars (d3, "Exit", NORMAL|BOLD|BLUE, 1, 1, 1);
  wait_next();
  scrc_clear_screen(pb);
  scrc_delete_pasteboard(pb);
  scrc_resetANSI();
  ::printf("\n\n\n  +---------------------------------------+\n");
  ::printf("  |    It looks like SCR seems to work    |\n");
  ::printf("  +---------------------------------------+\n\n\n");
  return 0;
}

extern "C" int scr_qmtest(int /* argc */, char** /* argv */)  {
  FILE* input = ::popen("test.exe scr_test -nomemory","w");
  for(size_t i=0; i<13; ++i) {
    ::fwrite("\n",2,1,input);
    ::fflush(input);
    ::lib_rtl_sleep(500);
  }
  ::pclose(input);
  return 0;
}
