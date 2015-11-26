/*
        upic_popup.c
         Created           : 23-OCT-1989 by Christian Arnault
*/

#ifdef SCREEN 
//---------------------------------------------------------------------------
#include "UPI/upidef.h"
#include "SCR/scr.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

extern System Sys;
 
//---------------------------------------------------------------------------
void upic_open_pop_up (Menu* m, Param* p, int row, int col)   {
  Convert val;
  char* s = (char*)list_malloc (p->size + 1);
  Page* d = (Page*)list_malloc (sizeof(Page));
  d->lines = p->list_size;
  d->cols  = p->size;
  scrc_get_window_position (m->window, &d->row, &d->col);
  d->row   += row;
  d->col   += col;
  d->cur_line = 1;

  scrc_create_display (&d->id, d->lines, d->cols, SCR::NORMAL, ON, "List");
  for (int r=0; r<d->lines; )  {
    if (p->type == UPI_REAL_FMT) val.d = *((double*) p->list + r);
    else if (p->type == UPI_ASC_FMT) val.c = *((char**) p->list + r);
    else val.i = *((int*)p->list + r);
    upic_print_param (p, s, val);
    scrc_put_chars (d->id, s, SCR::NORMAL, ++r, 1, 0);
  }
  scrc_change_rendition (d->id, d->cur_line, 1, 1, d->cols, SCR::INVERSE);
  scrc_paste_display (d->id, Sys.pb, d->row, d->col);
  if (s) free (s);
  Sys.pop_up = d;
}

//---------------------------------------------------------------------------
void upic_close_pop_up () {
  scrc_delete_display (Sys.pop_up->id);
  ::free(Sys.pop_up);
  Sys.pop_up = (Page*)0;
}

//---------------------------------------------------------------------------
void upic_set_pop_up_cursor ()    {
  return;
}

//---------------------------------------------------------------------------
void upic_act_on_pop_up (int *num, int key, int row)    {
  Page* d = Sys.pop_up;
  bool kill_popup = false;
  int delta = 0, c = key, cur = d->cur_line;

  if (cur) scrc_change_rendition (d->id, cur, 1, 1, d->cols, SCR::NORMAL);
  scrc_begin_pasteboard_update (Sys.pb);
  switch (c)  {
    case SCR::PAGE_UP :
      delta = -12;
      if (delta < cur) delta = 1 - cur;
      break;
    case SCR::MOVE_UP :
      delta = -1;
      break;
    case SCR::PAGE_DOWN :
      delta = 12;
      if (delta >= d->lines - cur) delta = d->lines - cur - 1;
      break;
    case SCR::MOVE_DOWN :
      delta = 1;
      break;
    case 0 :
      delta = row - cur;
      break;
    default :
      if (cur) *num = cur - 1;
      kill_popup = true;
      break;
  }

  cur += delta;
  if (cur)  {
    if (cur > 0 && cur <= d->lines) d->cur_line = cur;
    else cur = d->cur_line;
    if (d->row + cur - 1 < 3 || d->row + cur - 1 > Sys.pb_rows - 2)  {
      d->row -= delta;
      scrc_move_display (d->id, Sys.pb, -delta, 0);
    }
    scrc_change_rendition (d->id, cur, 1, 1, d->cols, SCR::INVERSE);
    if ( kill_popup ) upic_close_pop_up();
  }
  else   {
     d->cur_line = cur;
  }
  /*  scrc_set_cursor (d->id, cur, 1);  */
  scrc_end_pasteboard_update (Sys.pb);
}
#endif
