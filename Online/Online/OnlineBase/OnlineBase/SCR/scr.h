#ifndef __SCR_H
#define __SCR_H

#include "lists.h"

#ifdef IBM
#define BUFFER_SIZE 200
#else
#define BUFFER_SIZE 1000
#endif
#ifdef __cplusplus
namespace SCR {
#endif

#if !defined(ON_OFF_DEFINED)
#define ON_OFF_DEFINED 1
  typedef enum FLAG {OFF, ON} flag;
#endif
  typedef unsigned char byte;

  struct Window;
  struct Screen;
  struct Pasteboard;
  struct Display;
  struct Paste_entry;
  struct Update;

  struct Paste_entry    {
    Paste_entry Link_items;  
    Display *disp;
  };

  struct Window   {
    Window Link_items;
    int   id;
    struct    {
      Paste_entry Linked_list_items;
    }       paste;
    Pasteboard *pb;
    Display    *on_screen;
    Display    *title;
    int         row;
    int         col;
    int         height;
    int         width;
  
    int         type;
    int         moved;
    int         iconified;
  };

  struct Screen  {
    Screen  Link_items;
    int     id;
    struct {  
      Window  Linked_list_items;
      Window *cur;
    }       wind;
    int     last_id;
  };

  struct Pasteboard   {
    char   *bufout;
    int     bufptr;
    int     bufsize;
    int     outchan;
    char   *device;

    int     rows,
      cols;
    struct  {
      int   row,
        col;
    }       curs;
    struct  {
      Paste_entry Linked_list_items;
    }       paste;
    struct  {
      Screen      Linked_list_items;
    }       scr;
    Update  *upd, *old;
    int          updating;
    unsigned int old_attr;
    int          smgid;
  
    Display *moving;
    Display *resizing;
  };

  struct Display    {
    Pasteboard *pb;
    Paste_entry *paste;
    Window  *wind;

    flag    pasted;           /* Obsolete  */
    int     rows,
      cols,
      row,
      col;
    int     row0,
      col0,
      row1,
      col1,
      height,
      width;
    char    *map;
    unsigned int *attr;
    unsigned int def_attr;
    unsigned int brd_attr;
    flag    border;
    char   *title;

    int    (*scroll)(int);
    int    (*resize)(Display*, int, int);
    int    (*drag)(Display*, int, int);
    void*  param;
  };

  struct Update    {
    char         *map;
    unsigned int *attr;
    char         *mod_rows;
    int          *mod_col0,
      *mod_col1;
  };

  struct PasteboardContext {
    Pasteboard* pb;
    int         rows;
    int         cols;
    /*
      PasteboardContext() : pb(0), rows(0), cols(0) {}
      PasteboardContext(const PasteboardContext& c) : pb(c.pb), rows(c.rows), cols(c.cols) {}
    */
  };
#ifdef __cplusplus
}
#endif

#include "scrdef.h"

#define level_up(pb)     if (pb) pb->updating++
#define level_down(pb)   if (pb)                            \
  {                                                         \
    pb->updating--;                                         \
    if (pb->updating <= 0) scrc_end_pasteboard_update (pb); \
  }
#ifdef __cplusplus
extern "C"  {
#define __CXX_CONST const
#else
#define __CXX_CONST 
#endif

  // scr_edit.cpp
  int scrc_change_rendition (SCR::Display *disp, int r1, int c1, int rows, int cols, unsigned int attr);
  int scrc_insert_line (SCR::Display *disp, __CXX_CONST char *str, unsigned int attr, int row, int scroll);
  int scrc_insert_char (SCR::Display *disp, char ch, unsigned int attr, int row, int col);
  int scrc_delete_line (SCR::Display *disp, int row);
  int scrc_delete_char (SCR::Display *disp, int row, int col);

  // scr_ansi.cpp
  void scrc_resetANSI();
  void scrc_setANSI();
  void scrc_set_configure_handler (SCR::Pasteboard* pb, int (*handler)(int,int));
  void scrc_set_mouse_handler (SCR::Pasteboard* pb, int (*handler)(SCR::Pasteboard* pb, int,int,int));
  void scrc_handle_mouse(SCR::Pasteboard* pb, char key);
  void scrc_enable_mouse(SCR::Pasteboard* pb);
  void scrc_disable_mouse(SCR::Pasteboard* pb);

  int scrc_init_windows(SCR::Pasteboard* pb, int rows, int cols);
  SCR::Window* scrc_open_window (int type);
  int scrc_move_window_to (SCR::Window *w, int row, int col);
  int scrc_move_window (SCR::Window *w, int drow, int dcol);
  int scrc_change_window (SCR::Window *w, int height, int width);
  int scrc_get_window_size (SCR::Window *w, int* row, int* col);
  int scrc_get_window_position (SCR::Window *w, int* row, int* col);
  int scrc_window_moved (SCR::Window *w);
  int scrc_put_display_on_window (SCR::Display *d, SCR::Window *w);
  int scrc_remove_display_from_window (SCR::Display *d, SCR::Window *w);
  int scrc_delete_window (SCR::Window *w);
  SCR::Display* scrc_get_window_display (SCR::Window *w);
  int scrc_show_window (SCR::Window *w, SCR::Display *d);
  SCR::Window *scrc_next_window (SCR::Window *w);
  SCR::Window *scrc_prev_window (SCR::Window *w);
  int scrc_window_has_display (SCR::Window *w, SCR::Display* d);
  int scrc_hide_screen (SCR::Screen *s);
  int scrc_hide_window (SCR::Window *w);
  int scrc_moving_window (SCR::Window *w);
  int scrc_iconify_window (SCR::Window* w, SCR::flag mode);
  void scrc_send_window_to_icon (SCR::Window* w);

  int scrc_memory_of_windows();
  int scrc_memory_of_window (SCR::Window* w);
  int scrc_memory_of_screen (SCR::Screen* s);
  int scrc_configure_windows (int rows, int cols);

  int scrc_change_pbd_characteristics (SCR::Pasteboard *pb);
  int scrc_disable_unsolicited_input (SCR::Pasteboard *pb);
  int scrc_enable_unsolicited_input (SCR::Pasteboard *pb, int (* ast)());
  int scrc_get_smgid (SCR::Pasteboard *pb);
  int scrc_save_screen_rearm();
  int scrc_test_input ();
  int scrc_handler_keyboard (unsigned int fac,void* param);
  int scrc_get_last_key ();
  int scrc_read (SCR::Display *disp, unsigned char *buffer, int wait);
  int scrc_fputs (SCR::Pasteboard *pb);
  int scrc_wait (SCR::Display *disp);
  int scrc_save_screen_rearm();
  int scrc_rearm_keyboard (unsigned int fac, void* par);
  int scrc_ast_keyboard (void*);
  void scrc_uniconify_display (SCR::Display* d);
  void scrc_iconify_display (SCR::Display* d);


  int scrc_delete_physical_pasteboard (SCR::Pasteboard *pb);
  int scrc_read_keyboard (SCR::Display *disp, int wait);

  int scrc_init_screen (SCR::Pasteboard *pb, int rows, int cols);

  int scrc_create_pasteboard (SCR::Pasteboard** paste, __CXX_CONST char* device, int* rows, int* cols);
  int scrc_memory_of_pasteboard (SCR::Pasteboard *pb);
  int scrc_get_pasteboard_infos (SCR::Pasteboard *pb, int* rows, int *cols);
  int scrc_change_pasteboard (SCR::Pasteboard *pb, int *prows, int *pcols);
  int scrc_delete_pasteboard (SCR::Pasteboard *pb);
  int scrc_delete_display (SCR::Display *d);
  int scrc_clear_screen (SCR::Pasteboard *pb);
  int scrc_save_screen (SCR::Pasteboard *pb);
  int scrc_restore_screen (SCR::Pasteboard *pb);
  int scrc_repaint_screen (SCR::Pasteboard *pb);
  int scrc_create_display (SCR::Display** disp, int rows, int cols, unsigned int attr, SCR::flag border, __CXX_CONST char* title);
  int scrc_memory_of_display (SCR::Display *disp);
  int scrc_enable_scroll (SCR::Display *disp, int (*scroll)(int));
  int scrc_enable_resize (SCR::Display *disp, int (*resize)(SCR::Display*,int,int));
  int scrc_enable_drag (SCR::Display *disp, int (*drag)(SCR::Display*,int,int));
  int scrc_get_display_attr (SCR::Display *disp, int* rows, int* cols);
  int scrc_read_from_display (SCR::Display *disp, char *string, int maxlen, int row);
  int scrc_set_border (SCR::Display *disp, __CXX_CONST char *title, unsigned int attr);
  int scrc_paste_display (SCR::Display *disp, SCR::Pasteboard *pb, int row, int col);
  int scrc_unpaste_display (SCR::Display *disp, SCR::Pasteboard *pb);
  int scrc_bring_display_to_back (SCR::Display *disp, SCR::Pasteboard *pb);
  int scrc_move_display (SCR::Display *disp, SCR::Pasteboard *pb, int drow, int dcol);
  int scrc_change_display (SCR::Display *disp, int rows, int cols);
  char *scrc_get_title (SCR::Display *disp);
  int scrc_draw_box (SCR::Display *disp, unsigned int attr);
  int scrc_draw_block (SCR::Display *disp, int r1, int r2, int c1, int c2);
  int scrc_undraw_block (SCR::Display *disp, int r1, int r2, int c1, int c2);
  SCR::Display* scrc_display_at (SCR::Pasteboard* pb, int row, int col);
  int scrc_display_occluded (SCR::Display *disp);
  int scrc_occluded (SCR::Display *disp, int row, int col);
  int scrc_put_chars (SCR::Display *disp, __CXX_CONST char *str, unsigned int attr, int row, int col, int erase);
  int scrc_erase_line (SCR::Display *disp, int row);
  int scrc_put_char_all (SCR::Display *disp, int offset, char c, unsigned int attr, int row, int col);
  int scrc_restore_cursor(SCR::Pasteboard *pb, int context[]);
  int scrc_save_cursor(SCR::Pasteboard *pb, int *(context[]));
  int scrc_fflush(SCR::Pasteboard *pb);
  int scrc_action_moving_display (SCR::Pasteboard *pb, int key);
  int scrc_moving_display (SCR::Display *d);
  int scrc_action_resizing_display (SCR::Pasteboard *pb, int key);
  int scrc_resizing_display (SCR::Display *d);
  int scrc_load_font (SCR::Pasteboard *pb, __CXX_CONST char *name);
  int scrc_ring_bell (SCR::Display *disp);
  int scrc_check_key_buffer (char *buffer);
  int scrc_set_cursor (SCR::Display *disp, int row, int col);
  int scrc_set_cursor_abs (SCR::Pasteboard *pb, int row, int col);
  int scrc_draw_char_on_pb (SCR::Pasteboard *pb, char c, unsigned int attr, int row, int col);
  int scrc_draw_char (SCR::Display *disp, SCR::Pasteboard *pb, int offset, 
                      char c, unsigned int attr, SCR::Display *ddef, int row, int col);
  int scrc_get_char (SCR::Display *disp, int row, int col, char* c, unsigned int* attr);
  int scrc_put_char (SCR::Display *disp, char c, unsigned int attr, int row, int col);



  int scrc_putc (char c, SCR::Pasteboard *pb);
  int scrc_putes (__CXX_CONST char* s, SCR::Pasteboard *pb);
  int scrc_puts (__CXX_CONST char* s, SCR::Pasteboard *pb);
  int scrc_puti (int i, SCR::Pasteboard *pb);
  int scrc_begin_pasteboard_update (SCR::Pasteboard *pb);
  int scrc_end_pasteboard_update (SCR::Pasteboard *pb);
  int scrc_count_unmodified (unsigned int* attr, unsigned int* last);
  int cursor (SCR::Pasteboard *pb, int row, int col);
  int scrc_set_scroll (SCR::Pasteboard *pb, int row1, int row2);
  int scrc_cursor_on (SCR::Pasteboard *pb);
  int scrc_cursor_off (SCR::Pasteboard *pb);
  int scrc_get_console_dimensions(int* rows, int* cols);
  char scrc_top_left_corner();
  char scrc_top_right_corner();
  char scrc_bottom_left_corner();
  char scrc_bottom_right_corner();
  char scrc_horizontal_bar();
  char scrc_vertical_bar();

#define fg_black(pb)      {   scrc_putes("[30m",pb); }
#define fg_red(pb)        {   scrc_putes("[31m",pb); }
#define fg_green(pb)      {   scrc_putes("[32m",pb); }
#define fg_yellow(pb)     {   scrc_putes("[33m",pb); }
#define fg_blue(pb)       {   scrc_putes("[34m",pb); }
#define fg_magenta(pb)    {   scrc_putes("[35m",pb); }
#define fg_cyan(pb)       {   scrc_putes("[36m",pb); }
#define fg_white(pb)      {   scrc_putes("[37m",pb); }

#define bg_black(pb)      {   scrc_putes("[40m",pb); }
#define bg_red(pb)        {   scrc_putes("[41m",pb); }
#define bg_green(pb)      {   scrc_putes("[42m",pb); }
#define bg_yellow(pb)     {   scrc_putes("[43m",pb); }
#define bg_blue(pb)       {   scrc_putes("[44m",pb); }
#define bg_magenta(pb)    {   scrc_putes("[45m",pb); }
#define bg_cyan(pb)       {   scrc_putes("[46m",pb); }
#define bg_grey(pb)       {   scrc_putes("[47m",pb); }
#define bg_white(pb)      {   scrc_putes("[47m",pb); }

#ifdef __cplusplus
}
#endif
#undef __CXX_CONST

#endif /* __SCR_H */
