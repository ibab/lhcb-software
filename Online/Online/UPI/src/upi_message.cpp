/*
upic_message.c
Created           : 23-OCT-1989 by Christian Arnault
*/

//---------------------------------------------------------------------------
#include "UPI/upidef.h"
#include "SCR/scr.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#ifdef WIN32
#define vsnprintf _vsnprintf
#endif

#define MAX_MESSAGE_ENTRIES 200
//---------------------------------------------------------------------------
extern System Sys;
static char Separator[] = "--------------------------------------";
//---------------------------------------------------------------------------

#ifdef SCREEN
//---------------------------------------------------------------------------
int upic_scroll_message (int key)    {
  Async* a = &Sys.async;
  static Async_line *top, *bottom;
  scrc_begin_pasteboard_update (Sys.pb);
  switch (key)  {
  case 0 :
    scrc_paste_display (a->id, Sys.pb, a->row, a->col);
    bottom = a->last;
    top = bottom;
    for (int i = a->rows; i > 1 && top; i--)  {
      if (top->prev) top = top->prev;
      else break;
    }
    break;
  case SCR::KPD_PREV :
    if (top && a->rows < a->entries)    {
      for (int i=0; i<a->rows-1 && top; i++)      {
        if (top->prev)    {
          top = top->prev;
          bottom = bottom->prev;
          scrc_insert_line (a->id, top->text, top->render, 1, SCR::MOVE_DOWN);
        }
      }
    }
    break;
  case SCR::KPD_NEXT :
    if (bottom && a->rows < a->entries)  {
      for (int i=0; i<a->rows-1 && bottom; i++)  {
        if (bottom->next)   {
          top = top->next;
          bottom = bottom->next;
          scrc_insert_line (a->id, bottom->text, top->render, a->rows, SCR::MOVE_UP);
        }
      }
    }
    break;
  case SCR::KPD_PF4 :
    upic_paste_message();
    scrc_bring_display_to_back (a->id, Sys.pb);
  }
  scrc_end_pasteboard_update (Sys.pb);
  return 0;
}

//---------------------------------------------------------------------------
int  upic_resize_message (Display* /* id */, int rows, int cols)   {
  scrc_begin_pasteboard_update (Sys.pb);
  Async* a = &Sys.async;
  if (rows > 0 && cols > 0)  {
    a->rows = rows;
    a->cols = cols;
    scrc_change_display (a->id, a->rows, a->cols);
    upic_refresh_message (a);
  }
  return scrc_end_pasteboard_update (Sys.pb);
}

//---------------------------------------------------------------------------
int upic_drag_message (Display* /* id */, int drow, int dcol) {
  Async* a = &Sys.async;
  scrc_begin_pasteboard_update (Sys.pb);
  scrc_move_display (a->id, Sys.pb, drow, dcol);
  a->row += drow;
  a->col += dcol;
  return scrc_end_pasteboard_update (Sys.pb);
}

//---------------------------------------------------------------------------
void upic_draw_message ()   {
  int i;
  Async* a = &Sys.async;
  list_init (a);
  a->entries     = 1;
  Async_line* line = (Async_line*) list_add_entry (a, sizeof(Async_line));
  line->text = (char*) list_malloc (sizeof(Separator)+1);
  line->render = SCR::INVERSE;
  strcpy (line->text, Separator);

  a->max_entries = MAX_MESSAGE_ENTRIES;

  i = Sys.pb_rows - 30;
  if (i < 2) i = 2;
  a->rows = i;
  a->row  = Sys.pb_rows-i+1;

  a->col  = 2;
  a->cols = Sys.pb_cols-2;

  scrc_begin_pasteboard_update (Sys.pb);
  scrc_create_display (&a->id, a->rows, a->cols, SCR::NORMAL,ON, "Messages");
  scrc_enable_scroll (a->id, upic_scroll_message);
  scrc_enable_resize (a->id, upic_resize_message);
  scrc_enable_drag   (a->id, upic_drag_message);
  upic_paste_message();
  scrc_end_pasteboard_update (Sys.pb);
}

//---------------------------------------------------------------------------
void upic_paste_message ()  {
  Async* a = &Sys.async;
  upic_refresh_message (a);
  scrc_paste_display (a->id, Sys.pb, a->row, a->col);
  scrc_bring_display_to_back (a->id, Sys.pb);
}

//---------------------------------------------------------------------------
int upic_refresh_message (Async* a)  {
  Async_line* line = a->last;
  const char *c, null = '\0';
  for (int i = a->rows; i > 0; i--)  {
    int render = SCR::INVERSE;
    if (line)  {
      c = line->text;
      render = line->render;
      line = line->prev;
    }
    else {
      c = &null;
    }
    scrc_put_chars (a->id, c, render, i, 1, 1);
  }
  return UPI_NORMAL;
}

//---------------------------------------------------------------------------
static int upic_filter_controls (char* line)    {
  char c;
  int has_beep = 0;
  while ((c = *line))  {
    if (c < ' ')    {
      *line = ' ';
      if (c == 7) has_beep = 1;
    }
    line++;
  }
  return (has_beep);
}

#endif

//---------------------------------------------------------------------------
int upic_write_message2(const char* fmt,...) {
  char buff[1024];
  va_list lst;
  va_start(lst, fmt);
  vsnprintf(buff,sizeof(buff),fmt,lst);
  va_end(lst);
  return ::upic_write_message(buff,"");
}

//---------------------------------------------------------------------------
int upic_write_message (const char* text1, const char* text2) {
#ifdef SCREEN
  const char* t[2] = {text1, text2};
  Async* a = &Sys.async;
  scrc_begin_pasteboard_update (Sys.pb);
  for (int i=0; i<2; i++)  {
    int len = strlen(t[i]);
    if ( len )    {
      if (a->entries == a->max_entries)    {
        free (a->first->next->text);
        list_remove_entry (a->first->next);
      }
      else a->entries++;

      Async_line* line = (Async_line*) list_add_entry (a, sizeof(Async_line));
      line->text = (char*) list_malloc (len+1);
      line->render = SCR::INVERSE;
      strcpy (line->text, t[i]);
      if (upic_filter_controls (line->text)) scrc_ring_bell (a->id);
      scrc_insert_line (a->id, line->text, line->render, a->rows, SCR::MOVE_UP);
    }
  }
  scrc_end_pasteboard_update (Sys.pb);
  upic_wakeup();
#else
  upir_write_message (text1, text2);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_write_rendered_message (const char* text1, const char* text2, int render) {
#ifdef SCREEN
  const char* t[2] = {text1, text2};
  Async* a = &Sys.async;
  scrc_begin_pasteboard_update (Sys.pb);
  for (int i=0; i<2; i++)  {
    int len = strlen(t[i]);
    if ( len )    {
      if (a->entries == a->max_entries)    {
        free (a->first->next->text);
        list_remove_entry (a->first->next);
      }
      else  {
        a->entries++;
      }
      Async_line* line = (Async_line*) list_add_entry (a, sizeof(Async_line));
      line->render = render;
      line->text = (char*) list_malloc (len+1);
      strcpy (line->text, t[i]);
      if (upic_filter_controls (line->text)) scrc_ring_bell (a->id);
      scrc_insert_line (a->id, line->text, render, a->rows, SCR::MOVE_UP);
    }
  }
  upic_wakeup();
  scrc_end_pasteboard_update (Sys.pb);
#else
  upir_write_rendered_message (text1, text2, render);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_write_rendered_message_sync (const char* text1, const char* text2, int render) {
#ifdef SCREEN
  const char* t[2] = {text1, text2};
  Async* a = &Sys.async;
  scrc_begin_pasteboard_update (Sys.pb);
  for (int i=0; i<2; i++)  {
    int len = strlen(t[i]);
    if ( len )    {
      if (a->entries == a->max_entries)    {
        free (a->first->next->text);
        list_remove_entry (a->first->next);
      }
      else  {
        a->entries++;
      }
      Async_line* line = (Async_line*) list_add_entry (a, sizeof(Async_line));
      line->render = render;
      line->text = (char*) list_malloc (len+1);
      strcpy (line->text, t[i]);
      if (upic_filter_controls (line->text)) scrc_ring_bell (a->id);
      scrc_insert_line (a->id, line->text, render, a->rows, SCR::MOVE_UP);
    }
  }
  upic_key_action(0,0);
  upic_key_rearm(0,0);
  //upic_wakeup();
  scrc_end_pasteboard_update (Sys.pb);
#else
  upir_write_rendered_message (text1, text2, render);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_set_message_window (int rows, int cols, int row, int col)    {
#ifdef SCREEN
  Async* a = &Sys.async;
  if (rows == -1) rows = a->rows;
  if (cols == -1) cols = a->cols;
  if (row == -1) row  = a->row ;
  if (col == -1) col  = a->col ;

  scrc_begin_pasteboard_update (Sys.pb);
  if (rows != a->rows || cols != a->cols)  {
    scrc_change_display (a->id, rows, cols);
    if (rows == 0 || cols == 0)
      scrc_unpaste_display (a->id, Sys.pb);
    else if (a->rows == 0 || a->cols == 0)
      upic_paste_message();

    a->rows = rows;
    a->cols = cols;
    upic_refresh_message (a);
  }

  if (row != a->row || col != a->col)  {
    scrc_move_display (a->id, Sys.pb, row - a->row, col - a->col);
    a->row = row;
    a->col = col;
  }
  scrc_end_pasteboard_update (Sys.pb);
#else
  upir_set_message_window (rows, cols, row, col);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_get_message_window (int* rows, int* cols, int* row, int* col)  {
#ifdef SCREEN
  Async* a = &Sys.async;
  *rows = a->rows;
  *cols = a->cols;
  *row  = a->row;
  *col  = a->col;
#else
  upir_get_message_window (rows, cols, row, col);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
void upic_drop_async (Async_line* line) {
  if (line->text) free (line->text);
  list_remove_entry (line);
}
