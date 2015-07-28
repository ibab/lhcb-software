/*
  scrc_window.c
  Created           : 21-NOV-1989 by Christian Arnault
*/
 
#include <cstring>
#include "SCR/scr.h"
using namespace SCR;

enum {
  SEQUENTIAL,
  DETACHED,
  ICONIFIED,
  SCREENS
};
 
typedef struct WINDOW_SYSTEM Window_system;

struct WINDOW_SYSTEM   {
  Screen      scr[SCREENS];
  Screen*     cur;
  Window*     dragged;
  Pasteboard* pb;
  int         rows, cols;
};


static Window_system System;

//----------------------------------------------------------------------------
int scrc_memory_of_window (Window* w)  {
  int memory = sizeof (Window);
  for(Paste_entry *p = w->paste.first; p; p = p->next)
    memory += sizeof (Paste_entry);
  if (w->title) memory += scrc_memory_of_display (w->title);
  return memory;
}

//----------------------------------------------------------------------------
int scrc_memory_of_screen (Screen* s)    {
  int memory = sizeof(Screen);
  for(Window* w = s->wind.first; w; w = w->next)
    memory += ::scrc_memory_of_window (w);
  return memory;
}

//----------------------------------------------------------------------------
static void fill_title_display (Window *w, Display *d)    {
  char *t;
  ::scrc_put_chars(w->title, "", NORMAL, 1, 1, 1);
  if (d && (t=::scrc_get_title (d)))  {
    int pos = w->width - strlen(t);
    pos = (pos <= 0)? 1 : pos/2 + 1;
    ::scrc_put_chars (w->title, t, NORMAL, 1, pos, 0);
  }
}

//----------------------------------------------------------------------------
int scrc_memory_of_windows()    {
  int i;
  Screen *s;  
  int memory = sizeof (Window_system);
  for (i=0, s = &System.scr[0]; i<SCREENS; i++, s++)  {
    memory += ::scrc_memory_of_screen (s);
  }
  return memory;
}

//----------------------------------------------------------------------------
int scrc_configure_windows (int rows, int cols)  {
  System.rows = rows;
  System.cols = cols;
  return 1;
}

//----------------------------------------------------------------------------
int scrc_init_windows(Pasteboard* pb, int rows, int cols)   {
  Screen *s = &System.scr[0];  
  for (int i=0; i<SCREENS; i++, s++)  {  
    list_init (&s->wind);
    s->wind.cur = (Window *) 0;
    s->last_id    = 0;
  }
  System.cur = (Screen *) 0;
  System.pb = pb;
  System.rows = rows;
  System.cols = cols;
  ::scrc_set_configure_handler (pb, scrc_configure_windows);
  return 1;
}  

//----------------------------------------------------------------------------
Window* scrc_open_window (int type)   {
  int row, col;

  if (type < 0 || type > PULLDOWN_WINDOW) return 0;

  int scr = (type == DETACHED_WINDOW) ? DETACHED : SEQUENTIAL;
  Screen* s = &System.scr[scr];
  Window* w = (Window *) list_add_entry (&s->wind, sizeof(Window));
  w->id = ++s->last_id;
  w->width  = 2;
  w->height = 4;
  w->type = type;
  w->on_screen = (Display *) 0;
  w->moved = 0;
  w->iconified = OFF;
  list_init (&w->paste);

  w->title = (Display *) 0;

  if (type == DETACHED_WINDOW)  {
    row = (System.rows - 4) >> 1;
    col = (System.cols - 2) >> 1;
  }
  else  {  
    row    = 1;
    if (w->prev)    {
      col = w->prev->col + w->prev->width;
      if (type != PULLDOWN_WINDOW) col++;
    }
    else col = 1;
    if (type == PULLDOWN_WINDOW)  {
      ::scrc_create_display (&w->title, 1, w->width, INVERSE, OFF, 0);
      ::scrc_paste_display (w->title, System.pb, row, col+1);
      w->title->wind = w;
      w->on_screen = w->title;
    }
  }
  w->row    = row;
  w->col    = col;
  return w;
}

//----------------------------------------------------------------------------
int scrc_move_window_to (Window *w, int row, int col)   {
  return ::scrc_move_window (w, row - w->row, col - w->col);
}

//----------------------------------------------------------------------------
int scrc_move_window (Window *w, int drow, int dcol)    {
  w->row += drow;
  w->col += dcol;
  for (Paste_entry *p = w->paste.first; p; p = p->next)  {
    scrc_move_display (p->disp, System.pb, drow, dcol);
  }
  if (w->title) scrc_move_display (w->title, System.pb, drow, dcol);
  return 1;
}

//----------------------------------------------------------------------------
int scrc_change_window (Window *w, int height, int width)   {
  int dh, dw;

  if (height == -1) dh = 0;
  else dh = height - w->height;
  if (dh < 0) dh = 0;
  
  if (width == -1) dw = 0;
  else dw = width - w->width;
  if (dw < 0) dw = 0;
  
  if (!dh && !dw) return 0;
  
  w->height += dh;
  w->width  += dw;
  if (w->type == DETACHED_WINDOW)  {
    scrc_move_window (w, ((System.rows - w->height) >> 1) - w->row,
                      ((System.cols - w->width) >> 1) - w->col);
  }
  else  {
    for (Window *ww = w->next; ww; ww = ww->next)
      scrc_move_window (ww, 0, dw);
    if (w->title) scrc_change_display (w->title, 1, w->width);
  }
  return 1;
}

//----------------------------------------------------------------------------
int scrc_get_window_position (Window *w, int* row, int* col)    {
  *row = w->row;
  *col = w->col;
  return 1;
}

/*----------------------------------------------------------------------------*/
int scrc_get_window_size (Window* w, int* width, int* height)    {
  *width  = w->width;
  *height = w->height;
  return 1;
}

//----------------------------------------------------------------------------
static int scrc_drag_result (Display* id, int drow, int dcol)   {
  return scrc_move_window (id->wind, drow, dcol);
}

//----------------------------------------------------------------------------
static int scrc_scroll_result (int key)   {
  Window *w1;
  int dcol;
  
  Window *w = System.dragged;
  switch (key)  {
  case KPD_PREV :
    if ((w1 = w->prev))      {
      dcol = w1->width;
      if (w1->type != PULLDOWN_WINDOW) dcol++;
      scrc_move_window (w, 0, -dcol);
      dcol = w->width;
      if (w->type != PULLDOWN_WINDOW) dcol++;
      scrc_move_window (w1, 0, dcol);
      list_move_entry (w, 0, w1);
      scrc_show_window (w, w->on_screen);
    }
    break;
  case KPD_NEXT :
    if ((w1 = w->next))      {
      dcol = w1->width;
      if (w1->type != PULLDOWN_WINDOW) dcol++;
      scrc_move_window (w, 0, dcol);
      dcol = w->width;
      if (w->type != PULLDOWN_WINDOW) dcol++;
      scrc_move_window (w1, 0, -dcol);
      list_move_entry (w, w1, 0);
      scrc_show_window (w, w->on_screen);
    }
    break;
  }
  return 1;
}

//----------------------------------------------------------------------------
int scrc_window_moved (Window *w)   {
  return (w->moved);
}


//----------------------------------------------------------------------------
int scrc_put_display_on_window (Display *d, Window *w)    {
  Display *old;
  Paste_entry *p = (Paste_entry *) list_add_entry (&w->paste, sizeof (Paste_entry));
  p->disp = d;
  d->wind = w;
  scrc_enable_drag (d, scrc_drag_result);
  if (w->type != DETACHED_WINDOW)  {
    scrc_enable_scroll (d, scrc_scroll_result);
    if (!(old = w->on_screen))    {
      if (old && old != d) scrc_unpaste_display (old, System.pb);
      if (d != old) scrc_paste_display (d, System.pb, w->row+1, w->col+1);
      w->on_screen = d;
      return 1;
    }
    else if (old == w->title && (!p->prev))    {
      fill_title_display (w, d);
    }
  }
  return 0;
}

//----------------------------------------------------------------------------
int scrc_remove_display_from_window (Display *d, Window *w)   {
  Paste_entry *p = w->paste.first;
  for (; p; p = p->next )  {
    if (p->disp == d) break;
  }
  if (!p) return 0;
  if (d == w->on_screen)  {
    scrc_unpaste_display (d, System.pb);
    if (w->title)    {
      scrc_put_chars (w->title, "", NORMAL, 1, 1, 1);
      scrc_paste_display (w->title, System.pb, w->row, w->col+1);
      w->on_screen = w->title;
    }
    w->on_screen = (Display *) 0;
    scrc_enable_drag (d, 0);
    scrc_enable_scroll (d, 0);
  }
  list_remove_entry (p);
  if (!w->paste.first) scrc_delete_window (w);
  d->wind = 0;
  return 1;
}

//----------------------------------------------------------------------------
int scrc_delete_window (Window *w)    {
  if (w->type != DETACHED_WINDOW)  {
    if (w->title) scrc_delete_display (w->title);
    int shift = -(w->width) - 1;
    for(Window *ww = w->next; ww; ww = ww->next)
      scrc_move_window (ww, 0, shift);      
  }
  list_remove_entry (w);
  return 1;
}

//----------------------------------------------------------------------------
Display* scrc_get_window_display (Window *w)   {
  if (!w) return 0;
  if (w->on_screen == w->title) return 0;
  return w->on_screen;
}

//----------------------------------------------------------------------------
int scrc_window_has_display (Window *w, Display* d)   {
  if (!w) return 0;
  for (Paste_entry *p = w->paste.first; p; p = p->next)  {
    if (p->disp == d) return 1;
  }
  return 0;
}

//----------------------------------------------------------------------------
Window *scrc_prev_window (Window *w)    {
  if ( w )  {
    while ((w = w->prev))
      if (w->on_screen) return w;
  }
  return 0;
}

//----------------------------------------------------------------------------
Window *scrc_next_window (Window *w)    {
  if ( w ) {
    while ((w = w->next))
      if (w->on_screen) return w;
  }
  return 0;
}

//----------------------------------------------------------------------------
int scrc_show_window (Window *w, Display *d)    {
  Display *dd;
  Window *ww;
  Screen *s = System.cur;
  int scr, shift, row, col;

  if ( s )  {
    ww =  s->wind.cur;
    if ( ww )  {
      switch (ww->type)    {
      case PULLDOWN_WINDOW :
        if (w != ww)        {
          if (ww->on_screen != ww->title && !ww->iconified)     {
            scrc_unpaste_display (ww->on_screen, System.pb);
            fill_title_display (ww, ww->on_screen);
            scrc_paste_display (ww->title, System.pb, ww->row, ww->col+1);
            ww->on_screen = ww->title;
          }
        }
        break;
      case DETACHED_WINDOW :
        if (w->type != DETACHED_WINDOW) scrc_hide_screen (s);
        break;
      }
    }
  }
    
  if (w->type != DETACHED_WINDOW)  {
    Linked_list* father = (Linked_list*)&System.scr[ICONIFIED].wind;
    if (w->father == father)  {
      s = &System.scr[SEQUENTIAL];
      ww = s->wind.last;
      if (ww)  {
        col = ww->col + ww->width;
        if (w->type != PULLDOWN_WINDOW) col++;
      }
      else col = 1;
      list_transfer_entry (w, &s->wind, ww, 0);
      scrc_move_window_to (w, w->row, col);
    }

    if (w->col + w->width - 1 > System.cols)  {
      shift = System.cols - (w->col + w->width - 1);
      ww = (Window*) w->father->first;
      while (ww)  {
        scrc_move_window (ww, 0, shift);
        ww = ww->next;
      }
    }
    else if (w->col < 1)  {
      shift = 1 - w->col;
      ww = (Window*) w->father->last;
      while (ww)   {
        scrc_move_window (ww, 0, shift);
        ww = ww->prev;
      }
    }
  }
  
  if (!(dd = w->on_screen) || dd != d || scrc_display_occluded (d) || w->iconified)  {
    if (dd && dd != d) scrc_unpaste_display(dd, System.pb);
    row = w->row + 1;
    if (d == w->title) row--;
    d->wind = 0;
    ::scrc_paste_display (d, System.pb, row, w->col+1);
    d->wind = w;
    w->on_screen = d;
    w->iconified = OFF;
  }
  scr = (w->type == DETACHED_WINDOW) ? DETACHED : SEQUENTIAL;
  s = System.cur = &System.scr[scr];
  s->wind.cur = w;
  return 1;
}

//----------------------------------------------------------------------------
int scrc_hide_screen (Screen *s)  {
  for (Window *w= s->wind.first; w; w = w->next)
    ::scrc_hide_window (w);
  return 1;
}

//----------------------------------------------------------------------------
int scrc_hide_window (Window *w)    {
  if (w->on_screen)  {
    ::scrc_unpaste_display (w->on_screen, System.pb);
    w->on_screen = 0;
  }
  return 1;
}


//----------------------------------------------------------------------------
int scrc_moving_window (Window *w)    {
  if (!w->on_screen) return 0;  
  System.dragged = w;
  return ::scrc_moving_display(w->on_screen);
}

//----------------------------------------------------------------------------
int scrc_iconify_window (Window* w, flag mode)    {
  Display *d = w->on_screen;
  if ( d ) {
    switch (mode)  {
    case ON :
      ::scrc_unpaste_display (d, System.pb);
      if ((d = w->title)) ::scrc_unpaste_display (d, System.pb);
      ::scrc_send_window_to_icon (w);
      break;
    case OFF :
      ::scrc_show_window (w, d);
      break;
    }
  }
  w->iconified = mode;
  return 1;
}

//----------------------------------------------------------------------------
void scrc_send_window_to_icon (Window* w)   {
  if (!w || w->type == DETACHED_WINDOW || w->iconified) return;

  /* We consider here the case 1 : [to] is after [w] */  
  Window* first = w->next ? w->next : w->prev;
  Screen* s = &System.scr[SEQUENTIAL];
  if (s->wind.cur == w) s->wind.cur = 0;
  
  s = &System.scr[ICONIFIED];
  list_transfer_entry (w, &s->wind, s->wind.last, 0);

  int dcol = w->width;
  if (w->type != PULLDOWN_WINDOW) dcol++;

  for(Window* next=w->next; next; next = next->next)
    ::scrc_move_window (next, 0, -dcol);

  if (first) ::scrc_show_window (first, first->on_screen);
}
