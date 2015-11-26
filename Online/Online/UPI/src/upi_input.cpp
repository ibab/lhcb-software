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
#define mini(a,b) ((a)<(b)?(a):(b))

//---------------------------------------------------------------------------
extern System Sys;
#ifdef SCREEN
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

static int Moving_window = 0;
static int Moving_display = 0;
static int Resizing_display = 0;
#endif
//---------------------------------------------------------------------------

void upic_spawn ();

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
  Routine action;
  int c;

  do  {
    *menu_id = *item_id = *param_id = *list_index = 0;
#ifdef SCREEN
    Param* p;
    if (!Last_menu)  {
      int input_type;
      upic_wait (&input_type);
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

    c = SCR::RETURN_KEY;

    if ((m = Sys.menu.cur))    {
      if ((d = m->page.cur))     {
        if ((i = d->item.cur))     {
          if ((p = i->param.cur))    {
            Sys.param.cur = p;
            if (m->type == UPI_PARAMETER_PAGE && i->id == -1)  {
              if (upic_go_backward (m) == UPI_SS_NORMAL)
                scrc_hide_window (m->window);
            }
          }
        }
      }
    }
#else
    c = upir_input (menu_id, item_id, param_id, list_index);
    if ( c != UPI_SS_NORMAL )  {
      return UPI_K_USER;
    }
    c = SCR::RETURN_KEY;
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
  } while (c != SCR::RETURN_KEY);
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_get_input (int* menu_id, int* item_id, int* param_id)  {
  Item* i;
  Routine action;
  int c;
  int list_index;

  do  {
    *menu_id = *item_id = *param_id = 0;

#ifdef SCREEN
    Param* p;
    if (!Last_menu)   {
      int input_type;
      upic_wait (&input_type);
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

    c = SCR::RETURN_KEY;
    if ((m = Sys.menu.cur))    {
      if ((d = m->page.cur))     {
        if ((i = d->item.cur))      {
          if ((p = i->param.cur))      {
            list_index = p->list_pos;
            Sys.param.cur = p;
            if (m->type == UPI_PARAMETER_PAGE && i->id == -1)   {
              if (upic_go_backward (m) == UPI_SS_NORMAL)
                scrc_hide_window (m->window);
            }
          }
        }
      }
    }
#else
    c = upir_input (menu_id, item_id, param_id, &list_index);
    if ( c != UPI_SS_NORMAL )  {
      return UPI_K_USER;
    }
    c = SCR::RETURN_KEY;
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
  } while (c != SCR::RETURN_KEY);
  return UPI_SS_NORMAL;
}

#ifdef SCREEN

//---------------------------------------------------------------------------
int upic_key_rearm (unsigned int /* event */ , void*)
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
  return WT_SUCCESS;
}

//---------------------------------------------------------------------------
int upic_key_action (unsigned int /* event */, void*)
//---------------------------------------------------------------------------
/*  We come to this point whenever a key stroke has been detected.            */
/*    It is a valid keystroke.                                                */
/*    It will eventually :                                                    */
/*       Generate a WT_FAC_UPI event if a <RETURN> event occured.             */
/*       Leave with SCR::NORMAL status.                                            */
/*                                                                            */
//---------------------------------------------------------------------------
{
  Menu* m = Sys.menu.cur;
  Page* d = 0;
  Item* i = 0;
  Routine action;

  if (!m || !(d = m->page.cur) || !(i = d->item.cur)) return UPI_SS_INVMENU;

  int key = scrc_read_keyboard (0, 0);
  if (!key) return WT_SUCCESS;

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

    param_page = (m->type == UPI_PARAMETER_PAGE);
    input = p->input;
    type  = p->type;
    list_elem = p->val;
    list_pos = p->list_size ? p->list_pos : -1;

    if (Sys.pop_up)    {
      upic_act_on_pop_up (&list_pos, key, 0);
      if (key == SCR::RETURN_KEY || key == SCR::ENTER)  {
        if (type == UPI_REAL_FMT) list_elem.d  = *((double*)p->list + list_pos);
        else if (type == UPI_ASC_FMT) list_elem.c  = *((char**)p->list + list_pos);
        else list_elem.i = *((int*)p->list + list_pos);
        upic_print_param (p, buf, list_elem);
        upic_draw_param (d, p, row, SCR::INVERSE, 0);
      }
      else if (key != SCR::BACK_SPACE && key != SCR::F12) key = SCR::INVALID;
    }

    switch (key)    {
      case SCR::CTRL_A:
        break;
      case SCR::CTRL_E:
        if (!p->flag)   {
          buf_pos = upic_non_blanks (p->buf);
          buf = p->buf + buf_pos;
          if (buf_pos > p->chars) pos = p->chars - 1;
          else pos = buf_pos;
          col = p->pos + pos;
          upic_draw_param (d, p, row, SCR::INVERSE, buf_pos - pos);
          input = 1;
        }
        break;
      case SCR::DELETE_KEY:
        if (input && buf_pos)    {
          ::strcpy(buf-1,buf);
	  --buf;
          p->buf[p->size - 1] = ' ';
          if (buf_pos > pos)   {        /* scroll to right */
            pos++;
            col++;
          }
          buf_pos--;
          pos--;
          col--;
          upic_draw_param (d, p, row, SCR::INVERSE, buf_pos - pos);
        }
        else scrc_ring_bell (d->id);
        break;
      case SCR::MOVE_LEFT:
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
            ::upic_draw_param (d, p, row, SCR::INVERSE, buf_pos - pos);
          }
          key = SCR::INVALID;
        }
        else   {
          if (!p->prev) ::upic_restore_params_in_line (i);
        }
        break;
      case SCR::MOVE_RIGHT :
        if (input)   {
          if ( buf_pos < ::strlen(p->buf) )   {
            if ( pos == p->chars )   {
              /* scroll to left */
              pos--;
              col--;
              ::upic_draw_param (d, p, row, SCR::INVERSE, buf_pos - pos);
            }
            if (pos < p->chars)   {
              buf++;
              buf_pos++;
              pos++;
              col++;
            }
          }
          key = SCR::INVALID;
        }
        else
        {
          if (!p->next) upic_restore_params_in_line (i);
        }
        break;
      case SCR::KPD_PERIOD :
        if (Sys.pop_up) upic_close_pop_up ();
        else if (p->list_size > 1) {
          upic_open_pop_up (m, p, row, col);
          pos = 0;
          input = 0;
          col = p->pos;
          buf = p->buf;
        }
        break;
      case SCR::PAGE_UP :
      case SCR::PAGE_DOWN :
      case SCR::MOVE_UP :
      case SCR::MOVE_DOWN :
        upic_restore_params_in_line (i);
        /*
        if (!param_page) upic_restore_params_in_line (i);
        */
        if (input) input = 0;
        break;
      case SCR::ENTER :
        if (param_page || p->next) key = SCR::MOVE_RIGHT;
        if ((m->condition & CALL_ON_ENTER))    {
          if (m->callback) (*m->callback)(m->id, i->id, CALL_ON_ENTER, m->arg);
        }
      case SCR::RETURN_KEY :
        if (type == UPI_ASC_FMT) 
          strcpy (p->val.c, p->buf);
        else    {
          if ( type == UPI_BIN_FMT ) val.i = upic_mtoi (p->buf, p->chars);
          else if ( type == UPI_LOG_FMT ) val.i = upic_ltoi (p->buf, p->chars);
          else sscanf (p->buf, p->convert, &val.d);
          if ( type == UPI_REAL_FMT )    {
            if (upic_check_double (val.d, p->min.d, p->max.d)) 
              p->val.d = val.d;
            else     {
              scrc_ring_bell (d->id);
              key = SCR::INVALID;
            }
          }
          else    {
            if (upic_check_int (val.i, p->min.i, p->max.i))
              p->val.i = val.i;
            else  {
              scrc_ring_bell (d->id);
              key = SCR::INVALID;
            }
          }
          upic_print_param (p, p->buf, p->val);
        }
        input = 0;
        break;
      default :
        if (key != SCR::TAB && (key < 32 || key > 127)) break;
        if (!p->flag)  {
          if (!input)    {
            buf = p->buf;
            if (type != UPI_ASC_FMT && type != UPI_BIN_FMT && type != UPI_LOG_FMT)    {
              for (size_t n = 0; n < p->size; n++) *buf++ = ' ';
              *buf = '\0';
            }
            upic_draw_param (d, p, row, SCR::INVERSE, 0);
            input = 1;
            pos = 0;
            buf_pos = 0;
            buf = p->buf;
          }
          if (type == UPI_BIN_FMT) {
            if (key != '.' && key != 'l')  {
              key = *buf;
              if (key == 'l') key = '.';
              else key = 'l';
            }
          }
          else if (type == UPI_LOG_FMT)
          {
            if (key != 'F' && key != 'T')
            {
              key = *buf;
              if (key == 'F') key = 'T';
              else key = 'F';
            }
          }
          if (type != UPI_ASC_FMT && !upic_valid_numeric(type,char(key)))
          {
            scrc_ring_bell (d->id);
            break;
          }

          if ( (pos == p->chars) && (buf_pos < p->size) )
          {
            /* scroll to left */
            pos--;
            col--;
            upic_draw_param (d, p, row, SCR::INVERSE, buf_pos - pos);
          }
          if (pos < p->chars)  {
            *buf = char(key);
            scrc_put_char (d->id, char(key), SCR::INVERSE + SCR::UNDERLINE, row, col);
            pos++;
            col++;
          }
          if (buf_pos < p->size)  {
            buf++;
            buf_pos++;
          }
          break;
        }
      case SCR::TAB :
        pos = 0;
        input = 0;
        col = p->pos;
        if (p->list_size)   {
          list_pos++;
          if (list_pos >= p->list_size) list_pos = 0;
          if (type == UPI_REAL_FMT) list_elem.d  = *((double*) p->list + list_pos);
          else if (type == UPI_ASC_FMT) list_elem.c  = *((char**) p->list + list_pos);
          else list_elem.i = *((int*)p->list + list_pos);
          buf = p->buf;
          upic_print_param (p, p->buf, list_elem);
          upic_draw_param (d, p, row, SCR::INVERSE, OFF);
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

  switch (key)  {
  case SCR::CTRL_Z :
    upic_spawn();
    break;
  case SCR::KPD_PF1 :
#ifdef SCREEN
        if (Sys.PF1CallBack != 0) {
          if (Sys.PF1CallBack) (*Sys.PF1CallBack)(m->id, i->id,UPI_CALL_ON_PF1, Sys.PF1Arg);
        }
#endif /* SCREEN */
  case SCR::KPD_0 :
  case SCR::KPD_1 :
  case SCR::KPD_2 :
  case SCR::KPD_3 :
  case SCR::KPD_4 :
  case SCR::KPD_5 :
  case SCR::KPD_6 :
  case SCR::KPD_7 :
  case SCR::KPD_8 :
  case SCR::KPD_9 :
    if (upic_valid_keypad (key)) upic_branch_on_keypad (key);
    break;
  case SCR::RETURN_KEY :
    if (m->type == UPI_PARAMETER_PAGE)    {
      if (i->id != -1)   {
        if ((action = i->action))     {
          int item_id = i ? i->id : 0;
          int param_id = p ? p->id : 0;
          int list_index = p ? p->list_pos : 0;
          upic_update_vars_of_page (m);
          (*action) (m->id, item_id, param_id, list_index);
        }
        upic_set_cursor (m->id, -1, UPI_ACCEPT_OPTION);
        key = SCR::INVALID;
      }
      else switch (p->id)
      {
  case UPI_ACCEPT_OPTION :
    upic_update_vars_of_page (m);
    break;
  case UPI_CANCEL_OPTION :
    upic_restore_params_in_page (m);
    break;
  case UPI_RESET_OPTION :
    key = SCR::INVALID;
    upic_restore_params_in_page (m);
    break;
      }
    }
    else if (i->param.first) 
      upic_update_vars_of_line (i);
    if (i->to)   {
      upic_set_cursor (i->to->id, 0, 0);
      key = SCR::INVALID;
    }
    break;
  case SCR::F12 :
  case SCR::BACK_SPACE :
    {
      int status;
      if (m->type == UPI_PARAMETER_PAGE) upic_restore_params_in_page (m);
      else if (i->param.first) upic_restore_params_in_line (i);
      status = upic_go_backward (m);
      if (m->type == UPI_PARAMETER_PAGE || m->type == UPI_DETACHED_MENU)
      {
        if (status == UPI_SS_NORMAL) scrc_hide_window (m->window);
      }
      if (Sys.GlobBSCallBack)    {
        (*Sys.GlobBSCallBack)(m->id, i->id,UPI_CALL_ON_ANY_BACKSPACE, Sys.GlobBSArg);
      }
      if ((m->condition & CALL_ON_BACK_SPACE))
      {
        if (m->callback) (*m->callback)(m->id, i->id,
          CALL_ON_BACK_SPACE, m->arg);
      }
    }
    break;
  case SCR::MOVE_UP :
    upic_move_up (m);
    break;
  case SCR::MOVE_DOWN :
    upic_move_down (m);
    break;
  case SCR::MOVE_LEFT :
    upic_move_left (m);
    break;
  case SCR::MOVE_RIGHT :
    upic_move_right (m);
    break;
  case SCR::PAGE_UP :
    upic_page_up (m);
    break;
  case SCR::PAGE_DOWN :
    upic_page_down (m);
    break;
  case SCR::UPDATE_KEY :
    scrc_repaint_screen (Sys.pb);
    break;
  case SCR::KPD_PF2 :
    upic_open_help (m);
    break;
  case SCR::KPD_PF3 :
    if (Moving_window)    {
      Moving_window = 0;
      if ((m->condition & CALL_ON_DRAG))  {
        if (m->callback) (*m->callback)(m->id,i->id,CALL_ON_DRAG,m->arg);
      }
    }
    else   {
      if (Resizing_display)   {
        if (Moving_display)
          Moving_display = 0;
        else     {
          Moving_display = 1;
          scrc_moving_display (Sys.async.id);
        }
      }
      else   {
        Moving_window = 1;
        scrc_moving_window (m->window);
      }
    }
    break;
  case SCR::KPD_PF4 :
    upic_scroll_message (0);
    if (Resizing_display)
      Resizing_display = 0;
    else  {
      Resizing_display = 1;
      scrc_resizing_display (Sys.async.id);
    }
    /*      upic_scroll_message (key);  */
    break;
  }
  /* {
    char txt[128];
    sprintf(txt,"You did hit key:%d 0x%02X",key,key);
    upic_write_message(txt,"");
    } */
  if (key == SCR::RETURN_KEY)  {
    int menu_id, item_id, param_id, list_index;
    menu_id = m->id;
    item_id = i ? i->id : 0;
    param_id = p ? p->id : 0;
    list_index = p ? p->list_pos : 0;
    if (i && (action = i->action))    {
      (*action) (menu_id, item_id, param_id, list_index);
      if (m->type == UPI_PARAMETER_PAGE && i->id == -1)      {
        if (upic_go_backward (m) == UPI_SS_NORMAL)
          scrc_hide_window (m->window);
      }
    }
    else    {
      Last_menu = menu_id;
      Last_item = item_id;
      Last_param = param_id;
      Last_index = list_index;
      wtc_insert_head (WT_FACILITY_UPI);
    }
  }
  return WT_SUCCESS;
}

#endif

//---------------------------------------------------------------------------
#ifdef SCREEN
void upic_start_recorder (int* list, int size)  {
  Recorder_list = list;
  Recorder_size = size;
  Recorder_pos  = 0;
  Recorder_on   = 1;
#else
void upic_start_recorder (int*,int)  {
#endif
}

//---------------------------------------------------------------------------
int upic_recorder_state() {
#ifdef SCREEN
  return Recorder_on;
#else
  return 0;
#endif
}

//---------------------------------------------------------------------------
int upic_replay_state() {
#ifdef SCREEN
  return Replay_on;
#else
  return 0;
#endif
}

//---------------------------------------------------------------------------
int upic_stop_recorder () {
  int pos = 0;
#ifdef SCREEN
  Recorder_on = 0;
  pos = Recorder_pos;
#else
#endif
  return (pos);
}

//---------------------------------------------------------------------------
#ifdef SCREEN
void upic_start_replay (int* list, int size)  {
  Replay_list = list;
  Replay_size = size;
  Replay_pos  = 0;
  Replay_on   = 1;
#else
void upic_start_replay (int*, int)  {
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
#ifdef SCREEN
void upic_record (int key)  {
  Recorder_list[Recorder_pos] = key;
  if (Recorder_pos == Recorder_size-1) Recorder_on = 0;
  else Recorder_pos++;
#else
void upic_record (int)  {
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
#else
  return 0;
#endif
}

#ifdef VMS
#include <descrip.h>
#include <jpidef.h>
#include <prvdef.h>

//---------------------------------------------------------------------------
void upic_spawn ()  {
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
#else
void upic_spawn () {
  int status = 0;
  upic_save_screen(0, 0);
  upic_restore_screen();
  if (!(status & 1)) upic_signal_error (status, "SPAWN");
}
#endif
