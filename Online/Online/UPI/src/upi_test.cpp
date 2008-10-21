#ifdef SCREEN
#include "RTL/rtl.h"
#include "SCR/scr.h"
#include "UPI/upidef.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace  {
  const char ins_text[81] = "";
  const char ins_help[81] = "";
  const char ins_type[16] = "COMMENT";
  int render = 0;
  int  ins_id = 0;
  int  ins_menu = 0;
  int  ins_before = 0;
  const char* ins_type_list[] = {
    "COMMENT",
    "COMMAND",
    "PARAM  ",
  };

  //  Variables for creating a parameter
  const char param_type[] = "INTEGER";
  const char   param_string[10][81] = {"","","","","","","","","",""};

  int Replay[80];
  int Replay_size = 0;

  void build_insert_menu (int father, int father_c)  {
    upic_open_param (12, father, father_c, "Line definition", "", "");

    upic_set_param  (&ins_menu, 1, "I3", ins_menu, 0, 0, 0, 0, 0);
    upic_add_command (1,     "       | into menu   : ^^^", "");
    upic_add_command (2,     "insert |", "");
    upic_set_param  (&ins_before, 1, "I3", ins_before, 0, 0, 0, 0, 0);
    upic_add_command (3,     "       | before line : ^^^", "");
    upic_add_comment (4,     "", "");
    upic_set_param  (&ins_id, 1, "I3", ins_id, 0, 0, 0, 0, 0);
    upic_add_command (5,     "         | id       : ^^^", "");
    upic_set_param  (ins_type, 1, "A7", ins_type, 0, 0, ins_type_list, 3, 1);
    upic_add_command (6,     "new line | type     : ^^^^^^^", "");
    upic_set_param  (ins_text, 1, "A80", ins_text, 0, 0, 0, 0, 0);
    upic_add_command (7,     "         | text     : ^^^^^^^^^^^^^^^^^^^^", "");
    upic_set_param  (ins_help, 1, "A80", ins_help, 0, 0, 0, 0, 0);
    upic_add_command (8,     "         | help     : ^^^^^^^^^^^^^^^^^^^^", "");
    upic_close_menu();
  }

  void insert_action()  {
    static int flag=0;  
    if (!flag)  {
      flag=1;
      upic_set_cursor (12, 0, 0);
    }
    else  {
      flag = 0;
      if (!strcmp(ins_type, ins_type_list[0]))
        upic_insert_comment (ins_menu, ins_before, ins_id, ins_text, ins_help);
      else
        upic_insert_command (ins_menu, ins_before, ins_id, ins_text, ins_help);
    }
  }

  int loop() {
      static int menu = 0, item = 0, param = 0;
      int m = 0, i = 10, p = 0;
      float x = 20.;
      static char c[31] = "coucou";
      static int list[30] = {1,2,3,4,5,6,7,8,9,10,
                            11,12,13,14,15,16,17,18,19,20,
                            21,22,23,24,25,26,27,28,29,30};
      static const char* clist[5] = {"abc", "def", "ghi", "jkl", "mno"};
      char buffer[100];
      int mrows, mcols, mrow, mcol;
      int items;
      int mode = 0;
      static int mode_list[] = {0, 1};
      int wmenu = 0, wrow, wcol;
      int dmenu = 0;
      int emenu = 0;
      static int log;
      const char *fmt = "I3";

      upic_attach_terminal ();
      
      upic_restore_setup();

      upic_open_menu (1, 0, 0, "menu 1", "up title", "down title");
      upic_add_command (99, "quit.......................", "xxxx");
      upic_add_command (1, "disable 2/1", "xxxx");
      upic_add_command (2, "disable 1/2", "xxxx");
      upic_add_command (3, "enable  1/2", "xxxx");

      upic_set_param (&m, 1,  fmt, m, 0, 0, 0, 0, 0);
      upic_set_param (&i, 2, "I3", i, 0, 0, 0, 0, 0);
      upic_set_param (&p, 3, "I3", p, 0, 0, 0, 0, 0);
      upic_add_param_line (4, "set cursor to Menu ^^^ Item ^^^ Param ^^^", "");

      upic_add_command (5, "save_screen/restore_screen", "");
      upic_add_command (6, "refresh params", "");

      upic_set_param (&i, 1, "I5", i, -999, 999, list, 30, 0);
      strcpy (c, clist[0]);
      upic_set_param (c, 2, "A30", c, 0, 0, clist, 5, 0);
      upic_set_param (&x, 3, "F8.3", x, -100., 100., 0, 0, 0);
      upic_add_param_line (40, "i = ^^^^^ c = ^^^^^^^^^^ x = ^^^^^^^^", "");
      upic_add_command (7, "goto detached", "");
      upic_add_command (8, "goto son on same window", "");
      upic_set_param (&i, 2, "I3", 0, 0, 0, 0, 0, 0);
      upic_set_param (&m, 1, "I3", 0, 0, 0, 0, 0, 0);
      upic_add_param_line (9, "Delete item ^^^ of menu ^^^ ", "");
#ifdef SCREEN
      upic_get_message_window (&mrows, &mcols, &mrow, &mcol);

      upic_set_param (&mrows, 1, "I3", mrows, 0, 0, 0, 0, 0);
      upic_set_param (&mcols, 2, "I3", mcols, 0, 0, 0, 0, 0);
      upic_add_param_line (10, "Message size :  rows ^^^ cols ^^^", "");

      upic_set_param (&mrow, 1, "I3", mrow, 0, 0, 0, 0, 0);
      upic_set_param (&mcol, 2, "I3", mcol, 0, 0, 0, 0, 0);
      upic_add_param_line (11, "Move message to row  ^^^ col  ^^^", "");
#endif
      log = 0;
      upic_set_param (&log, 1, "L1", log, 0, 0, 0, 0, 0);
      upic_add_command (26, "Log = ^", "");
      
      items = upic_get_items_per_page();
      upic_set_param (&items, 1, "I3", items, 0, 0, 0, 0, 0);
      upic_add_param_line (12, "Items per page : ^^^", "");
#ifdef SCREEN
      mode = upic_get_mode();
      upic_set_param (&mode, 1, "I1", mode, 0, 0, mode_list, 2, 1);
      upic_add_param_line (13, "Mode : ^", "");

      upic_set_param (&wmenu, 1, "I3", 0, 0, 0, 0, 0, 0);
      upic_add_param_line (14, "Get window pos of menu ^^^ ", "");

      upic_set_param (&wrow, 1, "I3", 0, 0, 0, 0, 0, 0);
      upic_set_param (&wcol, 2, "I3", 0, 0, 0, 0, 0, 0);
      upic_add_param_line (15, "Window row  ^^^ col  ^^^", "");
#endif
      upic_set_param (&dmenu, 1, "I3", dmenu, 0, 0, 0, 0, 0);
      upic_add_param_line (16, "Delete menu ^^^ ", "");

      upic_add_command (17,    "Statistics", "");

      upic_add_command (18,    "Insert", "");

      upic_add_command (19,    "Start recording", "");
      upic_add_command (20,    "Stop  recording", "");
      upic_add_command (21,    "Start replay", "");

      upic_add_command (22,    "Save setup", "");

      upic_add_command (23,    "Quit - Attach", "");

      upic_add_command (24,    "Init AMS", "");
      upic_add_command (25,    "Send message", "");

      upic_set_param (&emenu, 1, "I3", emenu, 0, 0, 0, 0, 0);
      upic_add_param_line (27, "Erase menu ^^^ ", "");

      upic_add_command (28, "Modify param", "");
      
      for (i=29; i<35; i++)
        upic_add_command (i, "----", "");
      upic_close_menu();

      build_insert_menu (0, 0);

      upic_open_menu (10, 1, 8, "menu 6", "blabla", "brbrbrbrb");
      upic_add_comment (1000, "-----------", "");
      upic_add_command (1, "Go back", "");
      upic_add_comment (1001, "-----------", "");
      upic_close_menu();

      upic_open_pulldown_menu (2, 0, 0, "menu 2", "blabla", "brbrbrbrb");
      upic_add_comment (1000, "---------------------------", "");
      upic_add_command (1, "item 1", "xxxx");
      upic_add_command (3, "item 3", "xxxx");
      upic_set_param (&i, 1, "I5", 10, 0, 0, 0, 0, 0);
      upic_set_param (c, 2, "A30", "coucou", 0, 0, 0, 0, 0);
      upic_add_param_line (4, "i = ^^^^^ c = ^^^^^^^^^^", "");
      upic_add_command (2, "item 2 ...........", "xxxx");
      upic_add_comment (1001, "---------------------------", "");
      upic_close_menu();

      upic_open_pulldown_menu (3, 0, 0, "menu 3", "blabla", "brbrbrbrb");
      upic_add_comment (1000, "---------------------------", "");
      upic_add_command (1, "item 1", "xxxx");
      upic_add_command (3, "item 3", "xxxx");
      upic_add_command (2, "item 2 ...........", "xxxx");
      upic_add_comment (1001, "---------------------------", "");
      upic_close_menu();

      upic_open_pulldown_menu (4, 0, 0, "menu 4", "blabla", "brbrbrbrb");
      upic_add_comment (1000, "---------------------------", "");
      upic_add_command (1, "item 1", "xxxx");
      upic_add_command (3, "item 3", "xxxx");
      upic_add_command (2, "item 2 ...........", "xxxx");
      upic_add_comment (1001, "---------------------------", "");
      upic_close_menu();

      upic_open_pulldown_menu (5, 0, 0, "menu 5", "blabla", "brbrbrbrb");
      upic_add_comment (1000, "---------------------------", "");
      upic_add_command (1, "item 1", "xxxx");
      upic_add_command (3, "item 3", "xxxx");
      upic_add_command (2, "item 2 ...........", "xxxx");
      upic_add_comment (1001, "---------------------------", "");
      upic_close_menu();

      upic_open_pulldown_menu (6, 0, 0, "menu 6", "blabla", "brbrbrbrb");
      upic_add_comment (1000, "---------------------------", "");
      upic_add_command (1, "item 1", "xxxx");
      upic_add_command (3, "item 3", "xxxx");
      upic_add_command (2, "item 2 ...........", "xxxx");
      upic_add_comment (1001, "---------------------------", "");
      upic_close_menu();

      /*
      scrc_create_pasteboard (&Pb, 0, &pb_rows, &pb_cols);
      init_timer (show_memory);
      scrc_create_display (&Show_mem_display, 2,  10, BOLD,   1, " Memory ");
      scrc_paste_display (Show_mem_display, Pb, 15, 65);
      scrc_put_chars (Show_mem_display, "..........", SCR::NORMAL, 2, 1, 0);
      */
      static int _cnt=0;
      while (item != 99)   {
        upic_get_input (&menu, &item, &param);
        switch (menu)  {
          case 0 :
            item = 99;
            break;
          case 1 :
            switch (item)  {
              case 1: upic_disable_command (2,1);
                break;
              case 2: upic_disable_command (1,2);
                break;
              case 3: upic_enable_command (1,2);
                break;
              case 4: upic_set_cursor (m, i, p);
                break;
              case 5: upic_save_screen (0,0);
                printf ("Hit return");
                scanf  ("%s", c);
                upic_restore_screen();
                break;
              case 6: upic_refresh_param_page (2);
                break;
              case 7:
                upic_open_param (11, 0, 0, "menu 11", "blabla", "brbrbrbrb");
                upic_add_comment (1000, "---------------------------", "");
                upic_add_command (1, "item 1", "xxxx");
                upic_add_command (3, "item 3", "xxxx");
                upic_add_command (2, "item 2 ...........", "xxxx");
                upic_add_comment (1001, "---------------------------", "");
                for (i=4; i<25; i++)
                  upic_add_command (i, "----", "");
                upic_close_menu();
                upic_set_cursor (11, 0, 0);
                break;
              case 9:
                upic_delete_command (m, i);
                break;
              case 10:
                upic_set_message_window (mrows, mcols, -1, -1);
                upic_get_message_window (&mrows, &mcols, &mrow, &mcol);
                upic_refresh_param_page (1);
                break;
              case 11:
                upic_set_message_window (-1, -1, mrow, mcol);
                upic_get_message_window (&mrows, &mcols, &mrow, &mcol);
                upic_refresh_param_page (1);
                break;
              case 12:
                upic_set_items_per_page(items);
                break;
              case 13:
                upic_set_mode(mode);
                break;
              case 14:
                upic_get_window_position (wmenu, &wcol, &wrow);
                upic_refresh_param_page (1);
                break;
              case 15:
                upic_set_window_position (wmenu, wcol, wrow);
                break;
              case 16:
                upic_delete_menu (dmenu);
                break;
              case 18:
                insert_action();
                break;
              case 19:
                upic_start_recorder (Replay, 80);
                break;
              case 20:
                Replay_size = upic_stop_recorder ();
                break;
              case 21:
                upic_start_replay (Replay, Replay_size);
                break;
              case 22:
                upic_save_setup();
                break;
              case 23:
                upic_quit();
                /* stop_timer(); */
                return (1);
                break;
              case 24:
                break;
              case 25:
                break;
              case 27:
                upic_erase_menu (emenu);
                break;
              case 28:
                upic_modify_param (2, 4, 2, "blabla", 0, 0, 0, 0, 0);
                break;
            }
            break;
          case 5 :
          case 6 :
            upic_set_cursor (1, 0, 0);
            break;
          case 12 :
            break;
        }

        sprintf (buffer, "Unrendered:  [%d] Menu %d Item %d Param %d ", ++_cnt, menu, item, param);
        upic_write_message (buffer, "");
        render = ((render+1)%8);
        sprintf (buffer, "Rendered[%d]: [%d] Menu %d Item %d Param %d ", render, ++_cnt, menu, item, param);
        upic_write_rendered_message (buffer, "", render);
      }
      upic_quit();
      return (0);
  }
}

extern "C" int upi_test (int argc, char** argv )   {
  RTL::CLI cli(argc,argv,0);
  if ( cli.getopt("debug",1) != 0 ) ::lib_rtl_start_debugger();
  while (loop()) {}
  return 1;
}
#endif
