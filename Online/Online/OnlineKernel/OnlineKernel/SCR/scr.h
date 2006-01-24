#ifndef __SCR_H
#define __SCR_H

#include "lists.h"

#ifdef IBM
#define BUFFER_SIZE 200
#else
#define BUFFER_SIZE 1000
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
    Display *on_screen;
    Display *title;
    int   row;
    int   col;
    int   height;
    int   width;

    int   type;
    int   moved;
    int   iconified;
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
  int     updating;
  byte    old_attr;
  int     smgid;
  
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
  char    *map,
          *attr;
  int     def_attr;
  flag    border;
  char   *title;
  
  int    (* scroll)(int);
  int    (* resize)(Display*, int, int);
  int    (* drag)(Display*, int, int);
};

struct Update
{
  char    *map,
          *attr;
  char    *mod_rows;
  int     *mod_col0,
          *mod_col1;
};

#include "scrdef.h"

#define level_up(pb)     if (pb) pb->updating++
#define level_down(pb)   if (pb) \
{\
  pb->updating--;\
  if (pb->updating <= 0) scrc_end_pasteboard_update (pb);\
}
#ifdef __cplusplus
extern "C"  {
#define __CXX_CONST const
#else
#define __CXX_CONST 
#endif

//scr_edit.cpp
int scrc_change_rendition (Display *disp, int r1, int c1, int rows, int cols, char attr);
int scrc_insert_line (Display *disp, __CXX_CONST char *str, byte attr, int row, int scroll);
int scrc_insert_char (Display *disp, char ch, byte attr, int row, int col);
int scrc_delete_line (Display *disp, int row);
int scrc_delete_char (Display *disp, int row, int col);

// scr_ansi.cpp
void scrc_resetANSI();
void scrc_setANSI();
void scrc_set_configure_handler (Pasteboard* pb, int (*handler)(int,int));
void scrc_set_mouse_handler (Pasteboard* pb, int (*handler)(int,int));

int scrc_init_windows(Pasteboard* pb, int rows, int cols);
Window* scrc_open_window (int type);
int scrc_move_window_to (Window *w, int row, int col);
int scrc_move_window (Window *w, int drow, int dcol);
int scrc_change_window (Window *w, int height, int width);
int scrc_get_window_size (Window *w, int* row, int* col);
int scrc_get_window_position (Window *w, int* row, int* col);
int scrc_window_moved (Window *w);
int scrc_put_display_on_window (Display *d, Window *w);
int scrc_remove_display_from_window (Display *d, Window *w);
int scrc_delete_window (Window *w);
Display* scrc_get_window_display (Window *w);
int scrc_show_window (Window *w, Display *d);
Window *scrc_next_window (Window *w);
Window *scrc_prev_window (Window *w);
int scrc_window_has_display (Window *w, Display* d);
int scrc_hide_screen (Screen *s);
int scrc_hide_window (Window *w);
int scrc_moving_window (Window *w);
int scrc_iconify_window (Window* w, flag mode);
void scrc_send_window_to_icon (Window* w);

int scrc_memory_of_windows();
int scrc_memory_of_window (Window* w);
int scrc_memory_of_screen (Screen* s);
int scrc_configure_windows (int rows, int cols);

int scrc_change_pbd_characteristics (Pasteboard *pb);
int scrc_disable_unsolicited_input (Pasteboard *pb);
int scrc_enable_unsolicited_input (Pasteboard *pb, int (* ast)());
int scrc_get_smgid (Pasteboard *pb);
int scrc_save_screen_rearm();
int scrc_test_input ();
int scrc_handler_keyboard (unsigned int fac,void* param);
int scrc_get_last_key ();
int scrc_read (Display *disp, unsigned char *buffer, int wait);
int scrc_fputs (Pasteboard *pb);
int scrc_wait (Display *disp);
int scrc_save_screen_rearm();
int scrc_rearm_keyboard (unsigned int fac, void* par);
int scrc_ast_keyboard (void*);
void scrc_uniconify_display (Display* d);
void scrc_iconify_display (Display* d);


int scrc_delete_physical_pasteboard (Pasteboard *pb);
int scrc_read_keyboard (Display *disp, int wait);

int scrc_init_screen (Pasteboard *pb, int rows, int cols);

int scrc_create_pasteboard (Pasteboard** paste, char* device, int* rows, int* cols);
int scrc_memory_of_pasteboard (Pasteboard *pb);
int scrc_get_pasteboard_infos (Pasteboard *pb, int* rows, int *cols);
int scrc_change_pasteboard (Pasteboard *pb, int *prows, int *pcols);
int scrc_delete_pasteboard (Pasteboard *pb);
int scrc_delete_display (Display *d);
int scrc_clear_screen (Pasteboard *pb);
int scrc_save_screen (Pasteboard *pb);
int scrc_restore_screen (Pasteboard *pb);
int scrc_repaint_screen (Pasteboard *pb);
int scrc_create_display (Display** disp, int rows, int cols, int attr, flag border, __CXX_CONST char* title);
int scrc_memory_of_display (Display *disp);
int scrc_enable_scroll (Display *disp, int (*scroll)(int));
int scrc_enable_resize (Display *disp, int (*resize)(Display*,int,int));
int scrc_enable_drag (Display *disp, int (*drag)(Display*,int,int));
int scrc_get_display_attr (Display *disp, int* rows, int* cols);
int scrc_read_from_display (Display *disp, char *string, int maxlen, int row);
int scrc_set_border (Display *disp, __CXX_CONST char *title, char attr);
int scrc_paste_display (Display *disp, Pasteboard *pb, int row, int col);
int scrc_unpaste_display (Display *disp, Pasteboard *pb);
int scrc_bring_display_to_back (Display *disp, Pasteboard *pb);
int scrc_move_display (Display *disp, Pasteboard *pb, int drow, int dcol);
int scrc_change_display (Display *disp, int rows, int cols);
char *scrc_get_title (Display *disp);
int scrc_draw_box (Display *disp, char attr);
int scrc_draw_block (Display *disp, int r1, int r2, int c1, int c2);
int scrc_undraw_block (Display *disp, int r1, int r2, int c1, int c2);
Display* scrc_display_at (Pasteboard* pb, int row, int col);
int scrc_display_occluded (Display *disp);
int scrc_occluded (Display *disp, int row, int col);
int scrc_put_chars (Display *disp, __CXX_CONST char *str, byte attr, int row, int col, int erase);
int scrc_erase_line (Display *disp, int row);
int scrc_put_char_all (Display *disp, int offset, char c, unsigned char attr, int row, int col);
int scrc_restore_cursor(Pasteboard *pb, int context[]);
int scrc_save_cursor(Pasteboard *pb, int *(context[]));
int scrc_fflush(Pasteboard *pb);
int scrc_action_moving_display (Pasteboard *pb, int key);
int scrc_moving_display (Display *d);
int scrc_action_resizing_display (Pasteboard *pb, int key);
int scrc_resizing_display (Display *d);
int scrc_load_font (Pasteboard *pb, __CXX_CONST char *name);
int scrc_ring_bell (Display *disp);
int scrc_check_key_buffer (char *buffer);
int scrc_set_cursor (Display *disp, int row, int col);
int scrc_set_cursor_abs (Pasteboard *pb, int row, int col);
int scrc_draw_char_on_pb (Pasteboard *pb, char c, int attr, int row, int col);
int scrc_draw_char (Display *disp, Pasteboard *pb, int offset, 
                    char c, unsigned char attr, Display *ddef, int row, int col);
int scrc_get_char (Display *disp, int row, int col, char* c, unsigned int* attr);
int scrc_put_char (Display *disp, char c, unsigned char attr, int row, int col);



int scrc_putc (char c, Pasteboard *pb);
int scrc_putes (__CXX_CONST char* s, Pasteboard *pb);
int scrc_puts (__CXX_CONST char* s, Pasteboard *pb);
int scrc_puti (int i, Pasteboard *pb);
int scrc_begin_pasteboard_update (Pasteboard *pb);
int scrc_end_pasteboard_update (Pasteboard *pb);
int scrc_count_unmodified (char* attr, char* last);
int cursor (Pasteboard *pb, int row, int col);
int scrc_set_scroll (Pasteboard *pb, int row1, int row2);
int scrc_cursor_on (Pasteboard *pb);
int scrc_cursor_off (Pasteboard *pb);

#ifdef __cplusplus
}
#endif
#undef __CXX_CONST

#endif /* __SCR_H */
