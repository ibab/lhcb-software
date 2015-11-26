/*
upic_item.c
Created           : 23-OCT-1989 by Christian Arnault
*/

//---------------------------------------------------------------------------
#include "UPI/upidef.h"
#include "SCR/scr.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
extern System Sys;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
int upic_add_command (int id,  const char* text_0, const char* text_1)  {
  return upic_add_item (id, text_0, text_1, UPI_COMMAND);
}

//---------------------------------------------------------------------------
int upic_add_comment ( int id, const char* text_0, const char* text_1)  {
  return upic_add_item (id, text_0, text_1, UPI_COMMENT);
}

//---------------------------------------------------------------------------
int upic_add_param_line ( int id, const char* text_0, const char* text_1) {
  return upic_add_item (id, text_0, text_1, UPI_COMMAND);
}

//---------------------------------------------------------------------------
int upic_replace_command ( int menu_id, int id, const char* text_0, const char* text_1)  {
  return upic_replace_item (menu_id, id, text_0, text_1, UPI_COMMAND);
}

//---------------------------------------------------------------------------
int upic_replace_comment (int menu_id, int id, const char* text_0, const char* text_1) {
  return upic_replace_item (menu_id, id, text_0, text_1, UPI_COMMENT);
}

//---------------------------------------------------------------------------
int upic_replace_param_line ( int menu_id, int id, const char* text_0, const char* text_1)  {
  return upic_replace_item (menu_id, id, text_0, text_1, UPI_COMMAND);
}

//---------------------------------------------------------------------------
int upic_delete_comment (int menu_id, int item_id)    {
  return (upic_delete_command (menu_id, item_id));
}

//---------------------------------------------------------------------------
int upic_delete_command (int menu_id, int item_id)  {
  int row;
  Menu *m;
  Item *i;
  if (!(m = upic_find_menu(menu_id))) return UPI_SS_INVMENU;
  Page* d = m->page.first;
  if (!d || !(i = (Item*) upic_find_item_row(d->item.first, item_id, &row)))
    return UPI_SS_INVCOMMAND;

#ifdef SCREEN
  scrc_begin_pasteboard_update (Sys.pb);
  Item* dropped = i;
#endif

  d = Page_address(i->father);

  /*  First delete underlaying structure  */
  if (i->to) upic_delete_menu (i->to->id);

  upic_drop_item(i);
  m->items--;
  if (Sys.menu.cur == m)  {
    Sys.item.cur = 0;
    upic_wakeup();
  }

#ifdef SCREEN
  while(0 != d )  {  //  Rearrange the pages
    if (d->item.cur == dropped) d->item.cur=0;
    if (d->cur_line > row) d->cur_line--;
    else if (d->cur_line == row && d->item.cur)
      d->item.cur = d->item.cur->next;

    scrc_delete_line (d->id, row + 1);
    i = d->item.last;
    if (d->next)    {
      /* There is another Page */
      list_transfer_entry (d->next->item.first, &d->item, i, 0);
      if (!d->item.cur) d->item.cur = d->item.last;
      upic_draw_item (d->item.last, d->lines);
      upic_change_page (m, d, m->width);
      d = d->next;
      row = 1;
    }
    else    {
      /* This is the last Page */
      d->lines--;
      upic_change_page (m, d, m->width);
      if (!d->lines)      {
	Page* dprev = d->prev;
        if ( dprev )  {
          // This is the last line of the display.
          scrc_remove_display_from_window (d->id, m->window);
          upic_drop_page (d);
        }
        else  {
	  d->item.cur = 0;
	}
        if (m->page.cur == d) m->page.cur = dprev;
      }
      else if (!d->item.cur)  {
	d->item.cur = d->item.first;
      }
      d = 0;
    }
  }
  scrc_end_pasteboard_update (Sys.pb);
#else
  d->lines--;
  if (!d->lines) upic_drop_page (d);
  upir_delete_command (menu_id, item_id);
#endif

  return UPI_SS_NORMAL;
}


//---------------------------------------------------------------------------
int upic_insert_command (int menu_id, int position, int id, const char* text_0, const char* text_1) {
  return upic_insert_item (menu_id, position, id, text_0, text_1, UPI_COMMAND);
}

//---------------------------------------------------------------------------
int upic_insert_comment (int menu_id, int position, int id, const char* text_0, const char* text_1) {
  return upic_insert_item (menu_id, position, id, text_0, text_1, UPI_COMMENT);
}

//---------------------------------------------------------------------------
int upic_insert_param_line (int menu_id, int position, int id, const char* text_0, const char* text_1) {
  return upic_insert_item (menu_id, position, id, text_0, text_1, UPI_COMMAND);
}

//---------------------------------------------------------------------------
int upic_enable_command (int menu_id, int item_id)  {
  Menu* m;
  Item* i;
  Page* d;
  int row;

  if (!(m = upic_find_menu(menu_id))) return UPI_SS_INVMENU;
  d = m->page.first;
  if (!d || !(i = (Item*) upic_find_item_row (d->item.first, item_id, &row)))
    return UPI_SS_INVCOMMAND;

  i->enabled = UPI_ENABLED;

#ifdef SCREEN
  upic_draw_item (i, row);
#else
  upir_enable_command (menu_id, item_id);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_disable_command (int menu_id, int item_id)  {
  Menu* m;
  Item* i;
  Page* d;
  int row;

  if (!(m = upic_find_menu(menu_id))) return UPI_SS_INVMENU;
  d = m->page.first;
  if (!d || !(i = (Item*) upic_find_item_row(d->item.first, item_id, &row)))
    return UPI_SS_INVCOMMAND;

  i->enabled = UPI_DISABLED;
  if (Sys.item.cur == i) upic_wakeup();

#ifdef SCREEN
  upic_draw_item (i, row);
#else
  upir_disable_command (menu_id, item_id);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_enable_commands(int m, int n, ...)  {
  int ret, sc = UPI_SS_NORMAL;
  va_list ap;
  va_start(ap,n);
  for(int i=0; i<n; ++i) {
    ret = ::upic_enable_command(m,va_arg(ap,int));
    if ( ret != UPI_SS_NORMAL ) sc = ret;
  }
  va_end(ap);
  return sc;
}

//---------------------------------------------------------------------------
int upic_disable_commands(int m, int n, ...)   {
  int ret, sc = UPI_SS_NORMAL;
  va_list ap;
  va_start(ap,n);
  for(int i=0; i<n; ++i)  {
    ret = ::upic_disable_command(m,va_arg(ap,int));
    if ( ret != UPI_SS_NORMAL ) sc = ret;
  }
  va_end(ap);
  return sc;
}

//---------------------------------------------------------------------------
int upic_enable_action_routine (int menu_id, int item_id, Routine action) {
  Menu* m;
  Page* d;
  Item* i;

  if (!(m = upic_find_menu(menu_id))) return UPI_SS_INVMENU;
  d = m->page.first;
  if (!d || !(i = (Item*) upic_find_item(d->item.first, item_id)))
    return UPI_SS_INVCOMMAND;
  i->action = action;

  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_disable_action_routine (int menu_id, int item_id)  {
  Menu* m;
  Page* d;
  Item* i;

  if (!(m = upic_find_menu(menu_id))) return UPI_SS_INVMENU;
  d = m->page.first;
  if (!d || !(i = (Item*) upic_find_item(d->item.first, item_id)))
    return UPI_SS_INVCOMMAND;
  i->action = 0;

  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_add_item (int id, const char* text_0, const char* text_1, int type)  {
  Menu* m = Sys.menu.last;
  if (!m) return UPI_SS_INVMENU;
  Page* d = m->page.last;
  if (d->lines == Sys.items_per_page) d = upic_open_page (&m->page);
  if (upic_find_item(d->item.first, id)) return UPI_SS_INVCOMMAND;

  Item* i = (Item*) list_add_entry (&d->item, sizeof(Item));
  i->id = id;

  d->lines++;
  if (!d->item.cur)  {
    if (type != UPI_COMMENT)  {
      d->cur_line = d->lines;
      d->item.cur = i;
      if (!m->page.cur) m->page.cur = d;
    }
  }
  m->items++;
  list_init (&i->param);
  i->param.cur = 0;
  i->string    = 0;
  upic_init_item_strings (i, text_0, text_1);
  i->type      = char(type);
  i->enabled   = char((type == UPI_COMMENT)?UPI_DISABLED:UPI_ENABLED);
  i->to        = 0;

  int len = strlen(i->string);
  if (len > m->width) m->width = len;

  if (Sys.param.first)  {
    list_transfer (&Sys.param, &i->param);
    upic_install_params (i->param.first, i->string);
    i->param.cur = i->param.first;
    i->type      = UPI_PARAM;
  }
  i->action = 0;
  return UPI_SS_NORMAL;
}


//---------------------------------------------------------------------------
int upic_replace_item (int menu_id, int id, const char* text_0, const char* text_1, int type)  {
  Menu* m;
  Item* i;
  int len, row;

  if (!(m = upic_find_menu(menu_id))) return UPI_SS_INVMENU;

  Page* d = m->page.first;
  if (!d || !(i = (Item*) upic_find_item_row(d->item.first, id, &row)))
    return UPI_SS_INVCOMMAND;

  if (Sys.item.cur == i) upic_wakeup();
  upic_init_item_strings (i, text_0, text_1);
  i->type    = char(type);
  if (Sys.param.first || i->param.first)  {  
    Param* p = Sys.param.first;
    if ( p )  {
      upic_drop_params (i->param.first);
      list_transfer (&Sys.param, &i->param);
    }
    else p = i->param.first;
    upic_install_params (p, i->string);    
    i->param.cur = p;
    i->type      = UPI_PARAM;
  }

#ifdef SCREEN
  scrc_begin_pasteboard_update (Sys.pb);
  if ((len = strlen(i->string)) > m->width)  {
    scrc_change_window (m->window, -1, len+2);
    m->width = len;
    d = m->page.first;
    while (d)  {
      upic_change_page (m, d, len);
      d = d->next;
    }
  }
  upic_draw_item (i, row);
  scrc_end_pasteboard_update (Sys.pb);
#else
  if ((len = strlen(i->string)) > m->width)
    m->width = len;
  upir_replace_item (m, i);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_insert_item (int menu_id, int position, int id, const char* text_0, const char* text_1, int type)  {
  int row, width, len, to_the_end = 0;
  Menu* m = upic_find_menu(menu_id);
  Item* i = 0;

  if ( !m ) return UPI_SS_INVMENU;
  if (position == -1 && m->type != UPI_PARAMETER_PAGE) to_the_end = 1;
  Page* d = m->page.first;
  if (!position)  {
    i = d->item.first;
    row = 1;
  }
  else if (to_the_end)  {
    d = m->page.last;
    i = d->item.last;
    row = d->lines;
  }
  else   {
    if (!d) return UPI_SS_INVCOMMAND;
    if (!(i = (Item*) upic_find_item_row(d->item.first, position, &row)))    {
      d = m->page.last;
      i = d->item.last;
      row = d->lines;
      to_the_end = 1;
    }
  }
  if (upic_find_item (d->item.first, id)) return UPI_SS_INVCOMMAND;
  if (i)  {
    d = Page_address(i->father);
    if (to_the_end)
      i = (Item*) list_insert_entry (i, 0, sizeof(Item));
    else
      i = (Item*) list_insert_entry (0, i, sizeof(Item));
  }
  else
    i = (Item*) list_add_entry (&d->item.first, sizeof(Item));

  i->id = id;

  m->items++;

  list_init (&i->param);
  i->param.cur = 0;
  i->string = 0;
  upic_init_item_strings (i, text_0, text_1);

  i->type    = char(type);
  i->enabled = char((type == UPI_COMMENT)?UPI_DISABLED:UPI_ENABLED);
  i->to      = 0;

  if (Sys.param.first)  {
    list_transfer (&Sys.param, &i->param);
    upic_install_params (i->param.first, i->string);
    i->param.cur = i->param.first;
    i->type      = UPI_PARAM;
  }
  i->action = 0;
  if (Sys.menu.cur == m)  {
    Sys.item.cur = 0;
    upic_wakeup();
  }

#ifdef SCREEN
  int last;
  scrc_begin_pasteboard_update (Sys.pb);
  width = m->width;
  if ((len = strlen(i->string)) > m->width)  {
    scrc_change_window (m->window, -1, len+2);
    width = len;
  }
  if (m->items - 1 < Sys.items_per_page)  {
    scrc_change_window (m->window, m->items + 4, -1);
  }

  last = 0;  
  while (d)  {
    if (!d->next && (d->lines != Sys.items_per_page))  {
      /* This is the last Page */
      d->lines++;
      last = 1;
    }
    if (to_the_end) row++;

    scrc_insert_line (d->id, " ", SCR::NORMAL, row+1, SCR::MOVE_DOWN);

    if (!last)  {      /* There is another Page */
      if (!d->next)    {
        upic_open_page (&m->page);
        upic_init_page (m, m->page.last, width);
      }
      list_transfer_entry (d->item.last, &d->next->item, 0,
        d->next->item.first);
    }

    upic_change_page (m, d, width);
    upic_draw_item (i, row);

    if (d->cur_line >= row || d->cur_line == 0)   {
      if (d->cur_line < d->lines) d->cur_line++;
      else d->item.cur = d->item.last;
    }
    if ((d=d->next))  {
      i = d->item.first;
      row = 1;
    }
    scrc_end_pasteboard_update (Sys.pb);
  }
#else
  width = m->width;
  if ((len = strlen(i->string)) > m->width)
    width = len;
  upir_insert_item (m, i);
  d->lines++;
#endif
  m->width = width;

  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
Item* upic_find_item (Item* first,int id) {
  if (!first) return 0;
  Page* d = Page_address(first->father);
  while (d)  {  
    first = d->item.first;
    while (first)    {
      if (first->id == id) return (first);
      first = first->next;
    }
    d = d->next;
  }
  return 0;
}

//---------------------------------------------------------------------------
Item* upic_find_item_row (Item* first, int id, int* row)  {
  if (!first) return 0;
  Page* d = Page_address(first->father);
  while (d)  {  
    first = d->item.first;
    int r = 1;
    while (first)    {
      if (first->id == id)  {
        *row = r;
        return (first);
      }
      first = first->next;
      r++;
    }
    d = d->next;
    if (d) first = d->item.first;
  }
  return 0;
}

//---------------------------------------------------------------------------
Item* upic_find_next_item (Item* first, int* row) {
  if (!first) return 0;
  Page* d = Page_address(first->father);
  int r = *row + 1;
  first = first->next;
  while (d)  {  
    while (first)    {
      if (first->enabled)    {
        *row = r;
        return (first);
      }
      first = first->next;
      r++;
    }
    d = d->next;
    if (d)  {
      first = d->item.first;
      r = 1;
    }
  }
  return 0;
}


//---------------------------------------------------------------------------
Item* upic_find_prev_item (Item* first, int *row)  {
  if (!first) return 0;
  Page* d = Page_address(first->father);
  int r = *row - 1;

  first = first->prev;
  while (d)  {  
    while (first)   {
      if (first->enabled)  {
        *row = r;
        return (first);
      }
      first = first->prev;
      r--;
    }
    d = d->prev;
    if (d)  {
      first = d->item.last;
      r = d->lines;
    }
  }
  return 0;
}


//---------------------------------------------------------------------------
void upic_drop_item (Item* i)   {
  upic_drop_params (i->param.first);
  if (i->string) free (i->string);
  list_remove_entry (i);
}

//---------------------------------------------------------------------------
void upic_init_item_strings (Item* i, const char* text_0, const char* text_1) {
  if (i->string) free (i->string);  
  int len_0 = upic_non_blanks (text_0);
  int len_1 = upic_non_blanks (text_1);
  i->string  = (char*) list_malloc (len_0 + 3 + len_1 + 1);
  i->help    = i->string + len_0 + 3;
  strncpy (i->string, text_0, len_0);
  i->string[len_0] = '\0';
  strncpy (i->help, text_1, len_1);
  i->help[len_1] = '\0';
}


#ifdef SCREEN
//---------------------------------------------------------------------------
void upic_draw_item (Item* i, int row)  {
  Page* d = Page_address (i->father);
  Display* disp = d->id;
  if (!disp) return;

  scrc_begin_pasteboard_update (Sys.pb);
  int attr = (i->enabled) ? SCR::BOLD : SCR::NORMAL;
  row++;
  scrc_put_chars (disp, i->string, attr, row, 1, 1);
  if (i->to)
    scrc_put_char (disp, '>', attr, row, strlen(i->string)+1);
  if (i->type == UPI_PARAM)  {
    attr |= SCR::UNDERLINE;
    Param* p = i->param.first;
    while (p)      {
      upic_draw_param (d, p, row, attr, 0);
      p = p->next;
    }
  }
  scrc_end_pasteboard_update (Sys.pb);
}
#endif

