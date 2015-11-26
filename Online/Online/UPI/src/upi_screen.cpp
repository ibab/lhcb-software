/*
         Created           : 23-OCT-1989 by Christian Arnault
*/

//---------------------------------------------------------------------------
#include <cstdio>
#include "UPI/upidef.h"
#include "SCR/scr.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
//---------------------------------------------------------------------------

extern System Sys;

#ifndef SCREEN
static char My_name[80];
static char Server_name[80];
#endif

static FILE* Lun_scr = 0;
static FILE* Lun_kbd = 0;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifdef SCREEN
static inline Page* getPage(Menu* m) {  return m ? m->page.cur : 0;  }
int upic_mouse_handler (int window, void* display, size_t row, size_t col);
#endif
//---------------------------------------------------------------------------
 
//---------------------------------------------------------------------------
int upic_set_cursor (int menu_id, int item_id, int param_id)  {
  return (upic_set_cursor_and_mark (menu_id, item_id, param_id, 1));
}
//---------------------------------------------------------------------------
#ifdef SCREEN
int upic_set_cursor_and_mark (int menu_id, int item_id, int param_id, int mark) {
#else
int upic_set_cursor_and_mark (int menu_id, int item_id, int param_id, int /* mark */) {
#endif
  Menu* m;
  Item* i;
  int row;
  Page* d;
  
  if (!(m = upic_find_menu(menu_id))) return UPI_SS_INVMENU;
  if (item_id)  {
    if (!(d = m->page.first)) return UPI_SS_INVMENU;
    if (!(i = upic_find_item_row(d->item.first, item_id, &row)))
      return UPI_SS_INVCOMMAND;
    d = Page_address (i->father);
  }
  else  {
    d = m->page.cur;
    if (!d)    {
      d = m->page.first;
      m->page.cur = d;
    }
    if (!d) return UPI_SS_INVCOMMAND;
    i = d->item.cur;
    if (!i)    {
      i = d->item.first;
      d->item.cur = i;
      d->cur_line = 1;
    }
    if (!i) return UPI_SS_INVCOMMAND;
    row = d->cur_line;
  }

  Param* p = 0;
  if (i->type == UPI_PARAM)  {
    if (param_id)    {
      if (!(p = (Param*)upic_find_param(i->param.first, param_id)))
	return UPI_SS_INVPARAM;
    }
    else    {
#ifdef SCREEN
      p = i->param.cur;
#else
      p = 0;
#endif
    }
  }

#ifdef SCREEN
  Menu* cur_menu;
  if ((cur_menu = Sys.menu.cur) && !m->from.menu && cur_menu != m && mark)  {
    m->from.last = cur_menu->id;
  }
  if (Sys.menu.cur != m || Sys.item.cur != i || Sys.param.cur != p)  {
    upic_draw_cursor (OFF);
    upic_wakeup();
  }
  if (i->type == UPI_PARAM) i->param.cur = p;
  upic_move_cursor (m, d, i, row);
  upic_draw_cursor (ON);
#else
    upir_set_cursor (menu_id, item_id, p ? param_id : param_id);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_save_screen (FILE **kbd,FILE **scr)    {
#ifdef SCREEN
  upic_end_update();
  upic_disable_ast();
  scrc_cursor_on (Sys.pb);
  scrc_save_screen(Sys.pb);
  Lun_kbd = stdin;
  Lun_scr = stdout;
#else
  upir_save_screen ();
/*  upic_net_get_mbx_names (Server_name, My_name); */
  Lun_kbd = ::fopen (My_name, "r");
  Lun_scr = ::fopen (Server_name, "w");
#endif
  if (kbd) *kbd = Lun_kbd;
  if (scr) *scr = Lun_scr;
  
  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
int upic_restore_screen () {
#ifdef SCREEN
  scrc_cursor_off (Sys.pb);
  scrc_restore_screen(Sys.pb);
  upic_enable_ast();
#else
  if (Lun_kbd) ::fclose(Lun_kbd);
  if (Lun_scr) ::fclose(Lun_scr);
  Lun_kbd = 0;
  Lun_scr = 0;
  upir_restore_screen ();
#endif
  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
int upic_begin_update ()  {
#ifdef SCREEN
  scrc_begin_pasteboard_update(Sys.pb);
#else
  upir_begin_update();
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_end_update ()  {
#ifdef SCREEN
  while (scrc_end_pasteboard_update(Sys.pb)) {}
#else
  upir_end_update();
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_hide_menu (int menu) {
#ifdef SCREEN
  Menu* m = upic_find_menu (menu);
  if (m)  {
    upic_go_backward (m);
    scrc_hide_window (m->window);
    return (UPI_SS_NORMAL);
  }
  else
    return (UPI_SS_INVMENU);
#else
  upir_hide_menu (menu);
  return (UPI_SS_NORMAL);
#endif
}

//---------------------------------------------------------------------------
int upic_has_screen() {
#ifdef SCREEN
  return 1;
#else
  return 0;
#endif
}

//---------------------------------------------------------------------------
int upic_get_screen_size(int* rows, int* cols)  {
#ifdef SCREEN
  *rows = Sys.pb_rows;
  *cols = Sys.pb_cols;
#else
  *rows = 66;
  *cols = 256;
#endif
  return UPI_NORMAL;
}

#ifdef SCREEN
//---------------------------------------------------------------------------
int upic_refresh_screen ()   {
  scrc_repaint_screen(Sys.pb);
  return UPI_NORMAL;
}

//---------------------------------------------------------------------------
void upic_broadcast_handler (const char* message)   {
  upic_write_message (message, "");
}

//---------------------------------------------------------------------------
int upic_mouse_handler (Window* window, void* disp, size_t row, size_t col)
//---------------------------------------------------------------------------
/*  window is the window where the mouse click has been detected.             */
/*         (may be 0).                                                        */
/*                                                                            */
/*  display is the display id when window is null.                            */
/*                                                                            */
/*  row    is the row within the window.                                      */
/*  col    is the col within the window.                                      */
/*                                                                            */
//---------------------------------------------------------------------------
{
  Display* display = (Display*)disp;
  if (window && !Sys.pop_up)  {
    /*  A menu is concerned  */
    Menu* m= upic_find_menu_on_window (window);
    Page* d;
    Item* i;
    Param* param;
    int p_id;

    if (!m) return (SCR::ON_SCREEN);

    if (!(d = m->page.cur)) return 0;
    if (row == 1)    {
      return (SCR::ON_PAGE_UP);
    }
    else if ( row == size_t(d->lines+2) )    {
      return (SCR::ON_PAGE_DOWN);
    }
    else    {
      if (row > 0)  {        /* Cursor action ... */
        i = d->item.first;
        row--;
        while (i && row)   {
          row--;
          if (!row) break;
          i = i->next;
        }
        if (!i) return (SCR::ON_SCREEN);

        p_id = 0;
        param = i->param.first;
        while (param)   {
          if (col < param->pos + param->chars || (param->next == 0))   {
            p_id = param->id;
            break;
          }
          param = param->next;
        }
        if (upic_set_cursor (m->id, i->id, p_id) == UPI_SS_NORMAL)
          return (SCR::ON_CONTENTS);
      }
      else      {
        if (upic_set_cursor (m->id, 0, 0) == UPI_SS_NORMAL)
          return (SCR::ON_CONTENTS);
      }
    }
  }
  else  {
    /*-----------------------------------*/
    /*  Click in a non menu Scr display. */
    /*  it can be                        */
    /*     the message window            */
    /*     the list popup display        */
    /*     an upi histo                  */
    /*     any user display.             */
    /*-----------------------------------*/
    if (Sys.pop_up)    {
      int num;
      if (display == Sys.pop_up->id)      {
        upic_act_on_pop_up (&num, 0, row);
        return (SCR::ON_POP_UP);
      }
      else   {
        upic_act_on_pop_up (&num, 0, 0);
      }
    }
  }
  return (SCR::ON_SCREEN);
}

//---------------------------------------------------------------------------
void upic_init_screen ()    {
  Sys.pb_rows = 0;           /* use default settings  */
  Sys.pb_cols = 0;
  scrc_create_pasteboard (&Sys.pb, 0, &Sys.pb_rows, &Sys.pb_cols);
  scrc_cursor_off (Sys.pb);
  wtc_subscribe (WT_FACILITY_SCR, upic_key_rearm, upic_key_action);
  upic_set_wakeup();
/*  wtc_subscribe (WT_FACILITY_UPI, upic_key_rearm, 0);  */
  scrc_init_windows (Sys.pb, Sys.pb_rows, Sys.pb_cols);
//  scrc_set_mouse_handler (Sys.pb, upic_mouse_handler);
}

//---------------------------------------------------------------------------
void upic_erase_screen () {
  scrc_delete_pasteboard (Sys.pb);
}
//---------------------------------------------------------------------------
int upic_move_cursor (Menu* m, Page* d, Item* i, int line)  {
  Sys.menu.cur = m;
  if ( i != 0 ) {
    if (!i->enabled)  {
      Item* j;
      if (!((j = upic_find_next_item (i, &line)) ||
	    (j = upic_find_prev_item (i, &line)))) return (0);
      i = j;
      d = Page_address (i->father);    
    }
    if (m->page.cur != d) m->page.cur = d;
    Sys.item.cur = i;
    Sys.param.cur = i->param.cur;
    d->item.cur  = i;
    d->cur_line  = line;
    return UPI_NORMAL;
  }
  return UPI_ERROR;
}

//---------------------------------------------------------------------------
int upic_draw_cursor (FLAG mode)    {
  Menu* m = 0;
  Page* d = 0;
  Item* i = 0;
  int row = 0, col = 0, attr = 0;

  scrc_begin_pasteboard_update (Sys.pb);
  do  {
    if (!(m = Sys.menu.cur))    {
      m = Sys.menu.first;
      while (m && !(d = m->page.cur)) m = m->next;
      if (!m) break;
      upic_move_cursor (m, d, d->item.cur, d->cur_line);
    }
    if (!(i = Sys.item.cur))   {
      d = m->page.cur;
      if (!d) break;
      i = d->item.cur;
      upic_move_cursor (m, d, i, d->cur_line);
      i = 0;
    }
    else    {
      d = Page_address (i->father);
      if (d != m->page.cur)  {
        upic_move_cursor (m, d, i, d->cur_line);
        i = 0;
      }
    }
    row = d->cur_line;  
    if (mode == ON)  {
      attr = SCR::INVERSE | SCR::BOLD;
      if (i && !i->enabled)  {
        if (!upic_move_cursor (m, d, i, row)) upic_go_backward(m);
        i = 0;
      }
    }
    else   {
      attr = SCR::NORMAL;
      if (i && i->enabled) attr = SCR::BOLD;
    }
  } while (!i);

  if (i)  {
    row++;
    if (i->type == UPI_PARAM)
      upic_draw_param (d, i->param.cur, row, attr, 0);
    else if ( d )   {
      col = 1;
      scrc_put_chars (d->id, i->string, attr, row, col, 0);
    }
    if (d && mode == ON) scrc_show_window (m->window, d->id);
  }
  scrc_end_pasteboard_update (Sys.pb);
  return UPI_SS_NORMAL;
}
//---------------------------------------------------------------------------
int upic_move_up (Menu* m)    {
  Item* i;
  if (!m) return (0);
  Page* d = m->page.cur;
  int line = d->cur_line;
  if (!(i = upic_find_prev_item (d->item.cur, &line))) return 0;
  d = Page_address(i->father);
  return (upic_move_cursor (m, d, i, line));
}
//---------------------------------------------------------------------------
int upic_move_down (Menu* m)    {
  Item* i;
  if (!m) return (0);
  Page* d = m->page.cur;
  int line = d->cur_line;
  if (!(i = upic_find_next_item (d->item.cur, &line))) return 0;
  d = Page_address(i->father);  
  return (upic_move_cursor (m, d, i, line));
}
//---------------------------------------------------------------------------
int upic_move_left (Menu* m)    {
  Page* d = getPage(m);
  if (!d) return 0;
  Item* i = d->item.cur;
  
  if (i && (i->type == UPI_PARAM))  {
    Param* p = i->param.cur;
    if ( (p = p->prev) )    {
      i->param.cur = p;
      Sys.param.cur = p;
      return 1;
    }
    else if (m->type == UPI_PARAMETER_PAGE)    {
      return (upic_move_up (m));
    }
  }
  
  if (Sys.MLCallback) {
    (*Sys.MLCallback)(m->id, i->id, UPI_CALL_ON_MOVE_LEFT, Sys.MLArg);
  }
  Window* w = m->window;
  while ((w = scrc_prev_window(w)))  {
    m = upic_find_menu_on_window (w);
    if (!m) return 0;
    d = m->page.cur;
    if (!d)    {
      m->page.cur = m->page.first;
      d = m->page.cur;
    }
    if (!d) return 0;
    if (!d->item.cur) return 0;
    i = d->item.cur;
    if (!i)    {
      d->item.cur = d->item.first;
      i = d->item.cur;
    }
    if (!i) return 0;
    if (i->enabled)    {
      upic_move_cursor (m, d, i, d->cur_line);
      return 1;
    }
    else
      return (upic_move_down (m) || upic_move_up (m));
  }
  return 0;
}
//---------------------------------------------------------------------------
int upic_move_right (Menu* m)   {
  Page* d = getPage(m);
  if (!d) return 0;
  
  Item* i = d->item.cur;  
  if (i && (i->type == UPI_PARAM))  {
    Param* p = i->param.cur;
    if ( (p = p->next) )    {
      i->param.cur = p;
      Sys.param.cur = p;
      return 1;
    }
    else if (m->type == UPI_PARAMETER_PAGE)    {
      return (upic_move_down (m));
    }
  }
  if (Sys.MRCallback)  {
    (*Sys.MRCallback)(m->id, i->id, UPI_CALL_ON_MOVE_LEFT, Sys.MRArg);
  }  
  Window* w = m->window;
  while ( (w = scrc_next_window(w)) )  {
    m = upic_find_menu_on_window (w);
    if (!m) return 0;
    d = m->page.cur;
    if (!d)    {
      m->page.cur = m->page.first;
      d = m->page.cur;
    }
    if (!d) return 0;
    if (!d->item.cur) return 0;
    i = d->item.cur;
    if (!i)    {
      d->item.cur = d->item.first;
      i = d->item.cur;
    }
    if (!i) return 0;
    if (i->enabled)    {
      upic_move_cursor (m, d, i, d->cur_line);
      return 1;
    }
    else
      return (upic_move_down (m) || upic_move_up (m));
  }
  return UPI_SS_NORMAL;
}
//---------------------------------------------------------------------------
int upic_page_up (Menu* m)    {
  Page* d = getPage(m);
  if (d)  {
    upic_move_cursor (m, d, d->item.first, 1);
    return upic_move_up (m);
  }
  return 0;
}
//---------------------------------------------------------------------------
int upic_page_down (Menu* m)    {
  Page* d = getPage(m);
  if (!d) return 0;
  upic_move_cursor (m, d, d->item.last, d->lines);
  return (upic_move_down (m));
}
//---------------------------------------------------------------------------
int upic_go_backward (Menu* m)  {
  if (m->from.menu && m->from.item)  {
    return upic_set_cursor_and_mark (m->from.menu, m->from.item, 0, 0);
  }
  else if (m->from.last && upic_find_menu(m->from.last))  {
    int status = upic_set_cursor_and_mark (m->from.last, 0, 0, 0);
    m->from.last = 0;
    return status;
  }
  return upic_set_cursor_and_mark (Sys.menu.first->id, 0, 0, 0);
}
#endif
