/*
UPI_INPUT.C
Created           : 23-OCT-1989 by Christian Arnault
*/
//---------------------------------------------------------------------------
#include "UPI/upidef.h"
#include "SCR/scr.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"
#include <cstring>
#include <cstdio>

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#define UPI_K_MESSAGE  0
#define UPI_K_OPERATOR 1
#define UPI_K_USER     2

#define mini(a,b) ((a)<(b)?(a):(b))
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
extern System Sys;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
static int* Recorder_list = 0;
static int  Recorder_size = 0;
static int  Recorder_pos = 0;
static int  Recorder_on  = 0;
static int* Replay_list  = 0;
static int  Replay_size  = 0;
static int  Replay_pos   = 0;
static int  Replay_on    = 0;

static int Last_menu = 0;
static int Last_item = 0;
static int Last_param = 0;
static int Last_index = 0;

static int Fac_wt_upi = WT_FACILITY_UPI;

static int Moving_window = 0;
static int Moving_display = 0;
static int Resizing_display = 0;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
static void upic_spawn ();
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
int upic_wait (int* input_type)   {
  int sub_status;
  unsigned int event;
  void* dummy;  
  int status = wtc_wait (&event,&dummy,&sub_status);
  switch (event)  {
    case WT_FACILITY_UPI :
      *input_type = UPI_K_OPERATOR;
      break;
    case WT_FACILITY_AMS :
      *input_type = UPI_K_MESSAGE;
      break;
    default :
      *input_type = UPI_K_USER; 
      break;
  }
  return (status);
}

//---------------------------------------------------------------------------
int upic_get_input_with_index (int* menu_id, int* item_id, int* param_id, int* list_index)  {
  Item* i;
  Param* p;
  Routine action;
  int status;
  int input_type;
  int c;

  do  {
    *menu_id = *item_id = *param_id = *list_index = 0;
#ifdef SCREEN
    if (!Last_menu)  {
      status = upic_wait (&input_type);
      if (input_type != UPI_K_OPERATOR) return UPI_SS_ABORTED;
    }
    *menu_id    = Last_menu;
    *item_id    = Last_item;
    *param_id   = Last_param;
    *list_index = Last_index;
    Last_menu = 0;
    Menu* m = upic_find_menu (*menu_id);
    if (!m) return 0;
    Page* d = m->page.first;

    if (!d || !(i = upic_find_item (d->item.first, *item_id)))
      return UPI_SS_INVMENU;

    c = RETURN;

    if ((m = Sys.menu.cur))    {
      if ((d = m->page.cur))     {
        if ((i = d->item.cur))     {
          if ((p = i->param.cur))    {
            Sys.param.cur = p;
            if (m->type == PARAMETER_PAGE && i->id == -1)  {
              if (upic_go_backward (m) == UPI_SS_NORMAL)
                scrc_hide_window (m->window);
            }
          }
        }
      }
    }
#else
    upir_input (menu_id, item_id, param_id, list_index);
    c = RETURN;
    Menu* m = upic_find_menu (*menu_id);
    if (!m) return UPI_SS_ABORTED;
    Page* d = m->page.first;
    if (!d || !(i = upic_find_item (d->item.first, *item_id)))
      return UPI_SS_INVMENU;
#endif
    if (i && (action = i->action))  {
      (*action) (*menu_id, *item_id, *param_id, *list_index);
      c = 0;
    }
  } while (c != RETURN);
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_get_input (int* menu_id, int* item_id, int* param_id)  {
  Item* i;
  Param* p;
  Routine action;
  int status;
  int input_type;
  int c;
  int list_index;

  do  {
    *menu_id = *item_id = *param_id = 0;

#ifdef SCREEN

    if (!Last_menu)   {
      status = upic_wait (&input_type);
      if (input_type != UPI_K_OPERATOR) return UPI_SS_ABORTED;
    }
    *menu_id   = Last_menu;
    *item_id   = Last_item;
    *param_id  = Last_param;
    list_index = 0;
    Last_menu = 0;
    Menu* m = upic_find_menu (*menu_id);
    if (!m) return 0;
    Page* d = m->page.first;
    if (!d || !(i = upic_find_item (d->item.first, *item_id)))
      return UPI_SS_INVMENU;

    c = RETURN;
    if ((m = Sys.menu.cur))    {
      if ((d = m->page.cur))     {
        if ((i = d->item.cur))      {
          if ((p = i->param.cur))      {
            list_index = p->list_pos;
            Sys.param.cur = p;
            if (m->type == PARAMETER_PAGE && i->id == -1)   {
              if (upic_go_backward (m) == UPI_SS_NORMAL)
                scrc_hide_window (m->window);
            }
          }
        }
      }
    }
#else
    upir_input (menu_id, item_id, param_id, &list_index);
    c = RETURN;
    Menu* m = upic_find_menu (*menu_id);
    if (!m) return UPI_SS_ABORTED;
    Page* d = m->page.first;
    if (!d || !(i = upic_find_item (d->item.first, *item_id)))
      return UPI_SS_INVMENU;
#endif
    if (i && (action = i->action))  {
      (*action) (*menu_id, *item_id, *param_id, list_index);
      c = 0;
    }
  } while (c != RETURN);
  return UPI_SS_NORMAL;
}

#ifdef SCREEN

//---------------------------------------------------------------------------
int upic_key_rearm (unsigned int event, void*)
//---------------------------------------------------------------------------
/*   This is called to reset the cursor properly and therefore to allow       */
/*  the operator to hit the next keystroke.                                   */
/*                                                                            */
/*      Problem :                                                             */
/*                moving window                                               */
/*                moving display... ???                                       */
/*                                                                            */
//---------------------------------------------------------------------------
{
  Menu* m;
  Page* d;
  Item* i;
  Param* p;
  int row;
  size_t col;
  size_t col_max;
  size_t pos;
  size_t buf_pos;
  int cursor = 0;

  if (Moving_window ||
    Moving_display ||
    Resizing_display ) return UPI_SS_NORMAL;

  if (Sys.pop_up)  {
    upic_end_update ();
    upic_set_pop_up_cursor ();
  }
  else  {
    upic_draw_cursor (ON);

    if (!(m = Sys.menu.cur) ||
      !(d = m->page.cur) ||
      !(i = d->item.cur)) return UPI_SS_INVMENU;

    p = i->param.cur;

    if (p)    {
      /*  Here : set the cursor to a parameter field.   */
      row = d->cur_line + 1;
      col = p->pos;
      buf_pos = p->buf_pos;
      pos = (buf_pos > p->chars) ? p->chars - 1 : buf_pos;
      col_max = p->pos + p->chars - 1;
      col = mini(p->pos + pos, col_max);
      cursor = p->input;
    }
    else   {
      /*  Here : set the cursor to a normal item        */
      row = d->cur_line + 1;
      col = 1;
    }
    upic_end_update ();
    scrc_set_cursor (d->id, row, col);
  }
  if (cursor) scrc_cursor_on (Sys.pb);
  else scrc_cursor_off (Sys.pb);
  scrc_fflush (Sys.pb);
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_key_action (unsigned int event, void*)
//---------------------------------------------------------------------------
/*  We come to this point whenever a key stroke has been detected.            */
/*    It is a valid keystroke.                                                */
/*    It will eventually :                                                    */
/*       Generate a WT_FAC_UPI event if a <RETURN> event occured.             */
/*       Leave with NORMAL status.                                            */
/*                                                                            */
//---------------------------------------------------------------------------
{
  Menu* m = Sys.menu.cur;
  Page* d;
  Item* i;
  Routine action;

  if (!m || !(d = m->page.cur) || !(i = d->item.cur)) return UPI_SS_INVMENU;

  int key = scrc_read_keyboard (0, 0);
  if (!key) return (UPI_SS_NORMAL);

  scrc_begin_pasteboard_update (Sys.pb);

  if (Sys.help) upic_close_help ();

  Param* p = i->param.cur;
  if (p)    {
    /*  Here : manage a key stroke related to a parameter field.   */
    int   param_page;
    Convert val;
    Convert list_elem;
    int   list_pos;

    int row = d->cur_line + 1;
    int col = p->pos;
    size_t buf_pos = p->buf_pos;
    char* buf = p->buf + buf_pos;
    size_t pos = (buf_pos > p->chars) ? p->chars - 1 : buf_pos;
    int type = p->type;
    int input = p->input;
    col = p->pos + pos;

    param_page = (m->type == PARAMETER_PAGE);
    input = p->input;
    type  = p->type;
    list_elem = p->val;
    list_pos = p->list_size?p->list_pos:-1;

    if (Sys.pop_up)    {
      upic_act_on_pop_up (&list_pos, key, 0);
      if (key == RETURN || key == ENTER)  {
        if (type == REAL_FMT) list_elem.d  = *((double*) p->list + list_pos);
        else list_elem.i = *(p->list + list_pos);
        upic_print_param (p, buf, list_elem);
        upic_draw_param (d, p, row, INVERSE, 0);
      }
      else if (key != BACK_SPACE && key != F12) key = INVALID;
    }

    switch (key)    {
      case CTRL_A :
        break;
      case CTRL_E :
        if (!p->flag)   {
          buf_pos = upic_non_blanks (p->buf);
          buf = p->buf + buf_pos;
          if (buf_pos > p->chars) pos = p->chars - 1;
          else pos = buf_pos;
          col = p->pos + pos;
          upic_draw_param (d, p, row, INVERSE, buf_pos - pos);
          input = 1;
        }
        break;
      case DELETE :
        if (input && buf_pos)    {
          strcpy (buf-1, buf);
          buf--;
          p->buf[p->size - 1] = ' ';

          if (buf_pos > pos)   {
            /* scroll to right */
            pos++;
            col++;
          }
          buf_pos--;
          pos--;
          col--;
          upic_draw_param (d, p, row, INVERSE, buf_pos - pos);
        }
        else scrc_ring_bell (d->id);
        break;
      case MOVE_LEFT :
        if (input)   {
          if (buf_pos)   {
            if (buf_pos > pos)    {
              /* scroll to right */
              pos++;
              col++;
            }
            buf_pos--;
            buf--;
            pos--;
            col--;
            upic_draw_param (d, p, row, INVERSE, buf_pos - pos);
          }
          key = INVALID;
        }
        else   {
          if (!p->prev) upic_restore_params_in_line (i);
        }
        break;
      case MOVE_RIGHT :
        if (input)
        {
          if (buf_pos < strlen(p->buf))
          {
            if ((pos == p->chars))
            {
              /* scroll to left */
              pos--;
              col--;
              upic_draw_param (d, p, row, INVERSE, buf_pos - pos);
            }
            if (pos < p->chars)
            {
              buf++;
              buf_pos++;
              pos++;
              col++;
            }
          }
          key = INVALID;
        }
        else
        {
          if (!p->next) upic_restore_params_in_line (i);
        }
        break;
      case KPD_PERIOD :
        if (Sys.pop_up) upic_close_pop_up ();
        else if (p->list_size > 1)
        {
          upic_open_pop_up (m, p, row, col);
          pos = 0;
          input = 0;
          col = p->pos;
          buf = p->buf;
        }
        break;
      case PAGE_UP :
      case PAGE_DOWN :
      case MOVE_UP :
      case MOVE_DOWN :
        upic_restore_params_in_line (i);
        /*
        if (!param_page) upic_restore_params_in_line (i);
        */
        if (input) input = 0;
        break;
      case ENTER :
        if (param_page || p->next) key = MOVE_RIGHT;
        if ((m->condition & CALL_ON_ENTER))
        {
          if (m->callback) (*m->callback)(m->id, i->id, CALL_ON_ENTER, m->arg);
        }
      case RETURN :
        if (type == ASC_FMT) strcpy (p->val.c, p->buf);
        else
        {
          if ( type == BIN_FMT ) val.i = upic_mtoi (p->buf, p->chars);
          else if ( type == LOG_FMT ) val.i = upic_ltoi (p->buf, p->chars);
          else sscanf (p->buf, p->convert, &val.d);
          if ( type == REAL_FMT )
          {
            if (upic_check_double (val.d, p->min.d, p->max.d)) p->val.d = val.d;
            else
            {
              scrc_ring_bell (d->id);
              key = INVALID;
            }
          }
          else
          {
            if (upic_check_int (val.i, p->min.i, p->max.i)) p->val.i = val.i;
            else
            {
              scrc_ring_bell (d->id);
              key = INVALID;
            }
          }
          upic_print_param (p, p->buf, p->val);
        }
        input = 0;
        break;
      default :
        if (key != TAB && (key < 32 || key > 127)) break;
        if (!p->flag)  {
          if (!input)    {
            buf = p->buf;
            if (type != ASC_FMT && type != BIN_FMT && type != LOG_FMT)
            {
              for (size_t n = 0; n < p->size; n++) *buf++ = ' ';
              *buf = '\0';
            }
            upic_draw_param (d, p, row, INVERSE, 0);
            input = 1;
            pos = 0;
            buf_pos = 0;
            buf = p->buf;
          }
          if (type == BIN_FMT)
          {
            if (key != '.' && key != 'l')
            {
              key = *buf;
              if (key == 'l') key = '.';
              else key = 'l';
            }
          }
          else if (type == LOG_FMT)
          {
            if (key != 'F' && key != 'T')
            {
              key = *buf;
              if (key == 'F') key = 'T';
              else key = 'F';
            }
          }
          if (type != ASC_FMT && !upic_valid_numeric (type, key))
          {
            scrc_ring_bell (d->id);
            break;
          }

          if ( (pos == p->chars) && (buf_pos < p->size) )
          {
            /* scroll to left */
            pos--;
            col--;
            upic_draw_param (d, p, row, INVERSE, buf_pos - pos);
          }
          if (pos < p->chars)  {
            *buf = key;
            scrc_put_char (d->id, key, INVERSE + UNDERLINE, row, col);
            pos++;
            col++;
          }
          if (buf_pos < p->size)  {
            buf++;
            buf_pos++;
          }

          break;
        }
      case TAB :
        pos = 0;
        input = 0;
        col = p->pos;
        if (p->list_size)
        {
          list_pos++;
          if (list_pos >= p->list_size) list_pos = 0;
          if (type == REAL_FMT)
            list_elem.d  = *((double*) p->list + list_pos);
          else list_elem.i = *(p->list + list_pos);
          buf = p->buf;
          upic_print_param (p, p->buf, list_elem);
          upic_draw_param (d, p, row, INVERSE, OFF);
        }
        break;
    }
    p->input = (FLAG)input;
    p->buf_pos = input ? buf_pos : 0;
    p->list_pos = list_pos;

  }
  else
  {
    /*  Here : manage a key stroke related to a normal item        */
  }

  /* here : execute actions common to all kinds of items  */

  upic_draw_cursor(OFF);

  switch (key)
  {
  case CTRL_Z :
    /*      upic_spawn(); */
    break;
  case KPD_PF1 :
  case KPD_0 :
  case KPD_1 :
  case KPD_2 :
  case KPD_3 :
  case KPD_4 :
  case KPD_5 :
  case KPD_6 :
  case KPD_7 :
  case KPD_8 :
  case KPD_9 :
    if (upic_valid_keypad (key)) upic_branch_on_keypad (key);
    break;
  case RETURN :
    if (m->type == PARAMETER_PAGE)
    {
      if (i->id != -1)
      {
        if ((action = i->action))
        {
          int item_id, param_id, list_index;

          item_id = i ? i->id : 0;
          param_id = p ? p->id : 0;
          list_index = p ? p->list_pos : 0;

          upic_update_vars_of_page (m);
          (*action) (m->id, item_id, param_id, list_index);
        }
        upic_set_cursor (m->id, -1, ACCEPT_OPTION);
        key = INVALID;
      }
      else switch (p->id)
      {
  case ACCEPT_OPTION :
    upic_update_vars_of_page (m);
    break;
  case CANCEL_OPTION :
    upic_restore_params_in_page (m);
    break;
  case RESET_OPTION :
    key = INVALID;
    upic_restore_params_in_page (m);
    break;
      }
    }
    else if (i->param.first) upic_update_vars_of_line (i);
    if (i->to)
    {
      Menu* m = i->to;

      upic_set_cursor (m->id, 0, 0);
      key = INVALID;
    }
    break;
  case F12 :
  case BACK_SPACE :
    {
      int status;
      if (m->type == PARAMETER_PAGE) upic_restore_params_in_page (m);
      else if (i->param.first) upic_restore_params_in_line (i);
      status = upic_go_backward (m);
      if (m->type == PARAMETER_PAGE || m->type == DETACHED_MENU)
      {
        if (status == UPI_SS_NORMAL) scrc_hide_window (m->window);
      }
      if ((m->condition & CALL_ON_BACK_SPACE))
      {
        if (m->callback) (*m->callback)(m->id, i->id,
          CALL_ON_BACK_SPACE, m->arg);
      }
    }
    break;
  case MOVE_UP :
    upic_move_up (m);
    break;
  case MOVE_DOWN :
    upic_move_down (m);
    break;
  case MOVE_LEFT :
    upic_move_left (m);
    break;
  case MOVE_RIGHT :
    upic_move_right (m);
    break;
  case PAGE_UP :
    upic_page_up (m);
    break;
  case PAGE_DOWN :
    upic_page_down (m);
    break;
  case UPDATE :
    scrc_repaint_screen (Sys.pb);
    break;
  case KPD_PF2 :
    upic_open_help (m);
    break;
  case KPD_PF3 :
    if (Moving_window)
    {
      Moving_window = 0;
      if ((m->condition & CALL_ON_DRAG))
      {
        if (m->callback) (*m->callback)(m->id, i->id,
          CALL_ON_DRAG, m->arg);
      }
    }
    else
    {
      if (Resizing_display)
      {
        if (Moving_display) Moving_display = 0;
        else
        {
          Moving_display = 1;
          scrc_moving_display (Sys.async.id);
        }
      }
      else
      {
        Moving_window = 1;
        scrc_moving_window (m->window);
      }
    }
    break;
  case KPD_PF4 :
    upic_scroll_message (0);
    if (Resizing_display) Resizing_display = 0;
    else
    {
      Resizing_display = 1;
      scrc_resizing_display (Sys.async.id);
    }
    /*      upic_scroll_message (key);  */
    break;
  }
  if (key == RETURN)
  {
    int menu_id, item_id, param_id, list_index;

    menu_id = m->id;
    item_id = i ? i->id : 0;
    param_id = p ? p->id : 0;
    list_index = p ? p->list_pos : 0;

    if (i && (action = i->action))
    {
      (*action) (menu_id, item_id, param_id, list_index);
      if (m->type == PARAMETER_PAGE && i->id == -1)
      {
        if (upic_go_backward (m) == UPI_SS_NORMAL)
          scrc_hide_window (m->window);
      }
    }
    else
    {
      Last_menu = menu_id;
      Last_item = item_id;
      Last_param = param_id;
      Last_index = list_index;
      wtc_insert_head (Fac_wt_upi);
    }
  }
  return UPI_SS_NORMAL;
}

#endif

//---------------------------------------------------------------------------
void upic_start_recorder (int* list, int size)  {
#ifdef SCREEN
  Recorder_list = list;
  Recorder_size = size;
  Recorder_pos  = 0;
  Recorder_on   = 1;
#else
#endif
}

//---------------------------------------------------------------------------
int upic_stop_recorder () {
  int pos;
#ifdef SCREEN
  Recorder_on = 0;
  pos = Recorder_pos;
#else
#endif
  return (pos);
}

//---------------------------------------------------------------------------
void upic_start_replay (int* list, int size)  {
#ifdef SCREEN
  Replay_list = list;
  Replay_size = size;
  Replay_pos  = 0;
  Replay_on   = 1;
#else
#endif
}

//---------------------------------------------------------------------------
void upic_stop_replay ()  {
#ifdef SCREEN
  Replay_on = 0;
#else
#endif
}

//---------------------------------------------------------------------------
void upic_record (int key)  {
#ifdef SCREEN
  Recorder_list[Recorder_pos] = key;
  if (Recorder_pos == Recorder_size-1) Recorder_on = 0;
  else Recorder_pos++;
#endif
}

//---------------------------------------------------------------------------
int upic_replay ()    {
#ifdef SCREEN
  if (Replay_pos >= Replay_size)  {
    Replay_on = 0;
    return 0;
  }
  return (Replay_list[Replay_pos++]);
#endif
}

#ifdef VMS
#include <descrip.h>
#include <jpidef.h>
#include <prvdef.h>

//---------------------------------------------------------------------------
static void upic_spawn ()
//---------------------------------------------------------------------------
{
  int status, s;
  $DESCRIPTOR (prompt, "upi> ");
  int current_privs[2];
  int def_privs[2];
  int check_privs[2];
  static int no_privs[] = {-1, -1};
  int info;

  upic_save_screen(0, 0);

  info = JPI$_PROCPRIV;
  status = lib$getjpi (&info, 0, 0, def_privs, 0, 0);
  if (!(status & 1))  {
    upic_signal_error (status, "SPAWN");
    return;
  }
  status = sys$setprv (0, no_privs, 0, current_privs);
  if (!(status & 1))  {
    upic_signal_error (status, "SPAWN");
    return;
  }
  def_privs[0] &= ~PRV$M_SETPRV;
  status = sys$setprv (1, def_privs, 0, check_privs);
  if (!(status & 1))  {
    upic_signal_error (status, "SPAWN");
    return;
  }
  status = lib$spawn (0,0,0,0,0,0,0,0,0,0,&prompt,0);
  status = sys$setprv (0, no_privs, 0, 0);
  if (!(status & 1))  {
    upic_signal_error (status, "SPAWN");
    return;
  }
  status = sys$setprv (1, current_privs, 0, 0);
  if (!(status & 1))  {
    upic_signal_error (status, "SPAWN");
    return;
  }
  upic_restore_screen();
  if (!(status & 1)) upic_signal_error (status, "SPAWN");
}
#endif
