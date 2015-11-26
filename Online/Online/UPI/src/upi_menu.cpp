/*
upic_menu.c
Created           : 23-OCT-1989 by Christian Arnault
*/

//---------------------------------------------------------------------------
#include "UPI/upidef.h"
#include "SCR/scr.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
//---------------------------------------------------------------------------

extern System Sys;
static const char* Options[] = {
  "ACCEPT ",
    "CANCEL ",
    "RESET  ",
};

//---------------------------------------------------------------------------
int upic_open_menu (int id, int f, int fc, const char* title_0, const char* title_1, const char* title_2) {
  return upic_open_menus (id, f, fc, title_0, title_1, title_2, UPI_NORMAL_MENU);
}

//---------------------------------------------------------------------------
int upic_open_pulldown_menu (int id, int f, int fc, const char* title_0, const char* title_1, const char* title_2) {
  return upic_open_menus (id, f, fc, title_0, title_1, title_2, UPI_PULLDOWN_MENU);
}

//---------------------------------------------------------------------------
int upic_open_detached_menu (int id, int f, int fc, const char* title_0, const char* title_1, const char* title_2) {
  return upic_open_menus (id, f, fc, title_0, title_1, title_2, UPI_DETACHED_MENU);
}

//---------------------------------------------------------------------------
int upic_open_param (int id, int f, int fc, const char* title_0, const char* title_1, const char* title_2) {
  return upic_open_menus (id, f, fc, title_0, title_1, title_2, UPI_PARAMETER_PAGE);
}

//---------------------------------------------------------------------------
int upic_close_param () {
  return (upic_close_menu());
}

//---------------------------------------------------------------------------
int upic_close_menu ()  {
  Menu* m = Sys.menu.last;
  if (!m) return UPI_SS_NOOPENMENU;

  if (m->type == UPI_PARAMETER_PAGE)  {
    upic_set_param((int*)Sys.result,UPI_ACCEPT_OPTION,"A7","ACCEPT ",0,0,Options,  1,1);
    upic_set_param((int*)Sys.result,UPI_CANCEL_OPTION,"A7","CANCEL ",0,0,Options+1,1,1);
    upic_set_param((int*)Sys.result,UPI_RESET_OPTION, "A7","RESET  ",0,0,Options+2,1,1);
    upic_add_param_line (-1, "[^^^^^^^]   [^^^^^^^]   [^^^^^^^]", "");
  }
#ifdef SCREEN
  upic_draw_menu(m);
#else
  upir_close_menu(m, 0);
#endif
  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
int upic_delete_menu (int menu_id)  {
  Menu* m = upic_find_menu(menu_id);
  if ( !m ) return UPI_SS_INVMENU;
#ifdef SCREEN
  scrc_begin_pasteboard_update (Sys.pb);
#endif
  Page* d = m->page.first;
  while (d)  {
    Item* i = d->item.first;
    while (i)    {
      if (i->to) upic_delete_menu (i->to->id);
      i = i->next;
    }
    d = d->next;
  }

#ifdef SCREEN
  if (m == Sys.menu.cur)  {
    upic_go_backward (m);
    upic_wakeup();
    scrc_end_pasteboard_update (Sys.pb);
  }
#else
  upir_delete_menu (menu_id);
#endif
  upic_drop_menu (m);
  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
int upic_erase_menu (int menu_id) {
  Menu* m = upic_find_menu(menu_id);
  Page* d;
  Item* i;

  if (!m) return UPI_SS_INVMENU;

#ifdef SCREEN
  scrc_begin_pasteboard_update (Sys.pb);
  if (m == Sys.menu.cur)   {
    upic_go_backward (m);
    upic_wakeup();
  }
#endif

  while ((d = m->page.last))  {
    while ((i = d->item.first))  {
      if (i->to) upic_delete_menu (i->to->id);
      upic_drop_item(i);
    }
    if (d->prev)     {
#ifdef SCREEN
      d->lines--;
      upic_change_page (m, d, m->width);
      if (d->id) scrc_remove_display_from_window (d->id, m->window);
#endif
      upic_drop_page(d);
    }
    else    {
#ifdef SCREEN
      d->lines = 0;
      d->item.cur = 0;
      d->cur_line = 0;
      upic_change_page (m, d, m->width);
#endif
      break;
    }
  }

#ifdef SCREEN
  scrc_end_pasteboard_update (Sys.pb);
#else
  upir_erase_menu (menu_id);
#endif
  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
Menu* upic_find_menu (int id)   {
  for (Menu* m = Sys.menu.first; m; m = m->next)  {
    if (m->id == id) return m;
  }
  return 0;
}

//---------------------------------------------------------------------------
void upic_drop_menu (Menu* m)   {
  Page* d = m->page.first;
  while (d)  {
    Page* e = d->next;
#ifdef SCREEN
    if (d->id) scrc_remove_display_from_window (d->id, m->window);
#endif
    upic_drop_page(d);
    d = e;
  }
  free (m->mn_title);
  list_remove_entry (m);
}

//---------------------------------------------------------------------------
int upic_open_menus (int id, int f, int fc, const char* title_0, const char* title_1, const char* title_2, int type) {
  int max_len, len0, len1, len2;
  Window* w = 0;
  if (upic_find_menu(id)) return UPI_SS_INVMENU;
  Menu* m = (Menu*) list_add_entry (&Sys.menu, sizeof(Menu));
  m->id = id;
  if (type == UPI_NORMAL_MENU)  {
    if (!(w = Sys.window))    {
#ifdef SCREEN
      upic_open_window();
      w = Sys.window;
#endif
    }
  }
  else if (type == UPI_PULLDOWN_MENU)  {
    upic_open_pulldown_window();
    w = Sys.window;
  }
  else  {
    upic_open_detached_window();
    w = Sys.detached_window;
  }

  m->window = (Window*)w;  
  m->type = type;
  list_init (&m->page);
  m->page.cur = (Page*) 0;
  upic_open_page (&m->page);

  len0 = max_len = upic_non_blanks (title_0);
  if ((len1 = upic_non_blanks (title_1) + 2*UPI_PAGE_MARKER_SIZE) > max_len)
    max_len = len1;
  if ((len2 = upic_non_blanks (title_2) + 2*UPI_PAGE_MARKER_SIZE) > max_len)
    max_len = len2;
  m->width = max_len;
  m->items = 0;

  m->mn_title = (char*) list_malloc (len0 + 1 + len1 + 1 + len2 + 1);
  strncpy (m->mn_title , title_0, len0);
  m->mn_title[len0] = '\0';

  m->up_title = m->mn_title + len0 + 1;
  strncpy (m->up_title , title_1, len1);
  m->up_title[len1] = '\0';

  m->bt_title = m->up_title + len1 + 1;
  strncpy (m->bt_title , title_2, len2);
  m->bt_title[len2] = '\0';

  m->condition = 0;
  m->callback  = 0;
  m->arg       = 0;

  if (f && fc && upic_connect_menu (m, f, fc))  {
    m->from.menu = f;
    m->from.item = fc;
  }
  else  {
    m->from.menu = 0;
    m->from.item = 0;
  }
  m->from.last = 0;
  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
int upic_connect_menu (Menu* m, int f_menu, int f_item) {
  Menu* f;
  Item* i;
  Page* d;
  int row;

  if (!(f = upic_find_menu(f_menu))) return 0;
  d = f->page.first;
  if (!(i = upic_find_item_row(d->item.first, f_item, &row))) return 0;
  if (i->type == UPI_COMMENT) return 0;

#ifdef SCREEN
  scrc_begin_pasteboard_update (Sys.pb);
  if (!i->to)  {
    int len = strlen(i->string) + 1;
    if (len > f->width)    {
      scrc_change_window (f->window, -1, len+2);
      f->width = len;
      d = f->page.first;
      while (d)    {
        upic_change_page (f, d, len);
        d = d->next;
      }
    }
  }
#endif

  i->to = m;

#ifdef SCREEN
  upic_draw_item (i, row);
  scrc_end_pasteboard_update (Sys.pb);
#endif
  return 1;
}

#ifdef SCREEN
//---------------------------------------------------------------------------
Menu* upic_find_menu_on_window (Window* w)  {
  Display* id = scrc_get_window_display (w);  
  for (Menu* m = Sys.menu.first; m; m = m->next)  {
    if (m->window == w)  {
      if (!id) return m;
      Page* d = m->page.cur;
      if (d && (d->id == id)) return m;
    }
  }
  return 0;
}


//---------------------------------------------------------------------------
void upic_draw_menu (Menu* m) {
  Page* d = m->page.first;
  if (!d) return;

  scrc_begin_pasteboard_update (Sys.pb);
  int rows = d->lines+4;
  int cols = m->width + 2;
  scrc_change_window (m->window, rows, cols);
  for (;d; d = d->next)  {
    upic_init_page (m, d, m->width);
    Item* i = d->item.first;
    for (int n=1; n<=d->lines; n++, i = i->next)    {
      upic_draw_item (i, n);
    }
  }
  scrc_end_pasteboard_update (Sys.pb);
}

#endif
