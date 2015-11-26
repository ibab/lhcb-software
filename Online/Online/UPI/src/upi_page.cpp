/*
        upic_page.c
         Created           : 23-OCT-1989 by Christian Arnault
*/
 
//---------------------------------------------------------------------------
#include "UPI/upidef.h"
#include "SCR/scr.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
extern System Sys;
//---------------------------------------------------------------------------
 
//---------------------------------------------------------------------------
int upic_set_items_per_page (int num) {
  Sys.items_per_page = num;
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_get_items_per_page ()  {
  return (Sys.items_per_page);
}


//---------------------------------------------------------------------------
int upic_change_titles (int menu_id, const char* title_1, const char* title_2, const char* title_0) {
  Menu* m = upic_find_menu (menu_id);
  size_t len0, len1, len2;
  
  if ( !m ) return UPI_SS_INVMENU;

  size_t max_len = m->width;

  if (!title_0) title_0 = m->mn_title;
  if ((len0 = upic_non_blanks (title_0)) > max_len)
    max_len = len0;
  if (!title_1) title_1 = m->up_title;
  if ((len1 = upic_non_blanks (title_1) + 2*UPI_PAGE_MARKER_SIZE) > max_len)
    max_len = len1;
  if (!title_2) title_2 = m->bt_title;
  if ((len2 = upic_non_blanks (title_2) + 2*UPI_PAGE_MARKER_SIZE) > max_len)
    max_len = len2;

  char* t = m->mn_title;

  m->mn_title = (char*) list_malloc (len0 + 1 + len1 + 1 + len2 + 1);
  strncpy (m->mn_title , title_0, len0);
  m->mn_title[len0] = '\0';

  m->up_title = m->mn_title + len0 + 1;
  strncpy (m->up_title , title_1, len1);
  m->up_title[len1] = '\0';

  m->bt_title = m->up_title + len1 + 1;
  strncpy (m->bt_title , title_2, len2);
  m->bt_title[len2] = '\0';

  free (t);

#ifdef SCREEN
  {
     Page* d;
    
    scrc_change_window (m->window, -1, max_len + 2);
    d = m->page.first;
    while (d)
    {
      upic_change_page (m, d, max_len);
      if (d->id) scrc_set_border (d->id, m->mn_title, SCR::NORMAL);
      d = d->next;
    }
    m->width = max_len;
  }
#else
  upir_change_titles (m);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
Page* upic_open_page (void* root) {
  Page* p = (Page*) list_add_entry (root, sizeof(Page));
  list_init (&p->item);
  p->id       = 0;
  p->item.cur = 0;
  p->lines    = 0;
  p->cur_line = 0;
  p->cols     = 0;
  p->row      = 0;
  p->col      = 0;
  return (p);
}


//---------------------------------------------------------------------------
void upic_drop_page (Page* d)   {
  for (Item* i = d->item.first;i; )  {
    Item* j = i->next;
    upic_drop_item(i);
    i = j;
  }
#ifdef SCREEN
  if (d->id) scrc_delete_display (d->id);
#endif
  list_remove_entry (d);
}

#ifdef SCREEN
//---------------------------------------------------------------------------
void upic_init_page (Menu* m, Page* d, int width) {
  scrc_create_display (&d->id, d->lines+2, width, SCR::NORMAL, ON, m->mn_title);
  
  Display* id = d->id;
  if (d->prev)
    upic_draw_title_bar (id, m->up_title, width, 1, UPI_PREV_PAGE);
  else
    upic_draw_title_bar (id, m->up_title, width, 1, UPI_ONLY_PAGE);
    
  if (d->next)
    upic_draw_title_bar (id, m->bt_title, width, d->lines+2, UPI_NEXT_PAGE);
  else
    upic_draw_title_bar (id, m->bt_title, width, d->lines+2, UPI_ONLY_PAGE);
    
  scrc_put_display_on_window (id, m->window);
}


//---------------------------------------------------------------------------
void upic_change_page (Menu* m, Page* d, int width)    {
  Display* id;
  if (!(id = d->id)) return;
  scrc_change_display (id, d->lines+2, width);
  if (d->prev)  {
    upic_draw_title_bar (id, m->up_title, width, 1, UPI_PREV_PAGE);
  }
  else  {
    upic_draw_title_bar (id, m->up_title, width, 1, UPI_ONLY_PAGE);
  }
    
  if (d->next)  {
    upic_draw_title_bar (id, m->bt_title, width, d->lines+2, UPI_NEXT_PAGE);
  }
  else  {
    upic_draw_title_bar (id, m->bt_title, width, d->lines+2, UPI_ONLY_PAGE);
  }
}

//---------------------------------------------------------------------------
void upic_draw_title_bar (Display* d, const char* title, int width, int row, const char* head)  {
  if (row == 1)  {
    scrc_put_chars (d, head, SCR::INVERSE, row, 1, 1);
    scrc_put_chars (d, UPI_ONLY_PAGE, SCR::INVERSE,
    	            row, width - UPI_PAGE_MARKER_SIZE + 1, 0);
  }
  else  {
    scrc_put_chars (d, UPI_ONLY_PAGE, SCR::INVERSE, row, 1, 1);
    scrc_put_chars (d, head, SCR::INVERSE, row, width - UPI_PAGE_MARKER_SIZE + 1, 0);
  }
  int ttl_len = strlen(title);
  if ( ttl_len )  {
    int margin = (width - 2*UPI_PAGE_MARKER_SIZE - ttl_len) >> 1;
    scrc_put_chars (d, title, SCR::INVERSE, row, margin + UPI_PAGE_MARKER_SIZE + 1, 0);
  }
}
int upic_change_pasteboard(int width,int height)  {
    Sys.pb_cols = width;
    Sys.pb_rows = height;
    scrc_change_pasteboard(Sys.pb,&Sys.pb_rows,&Sys.pb_cols);
    return UPI_SS_NORMAL;
}
#else
int upic_change_pasteboard(int,int)  {    return UPI_SS_NORMAL;   }
#endif
