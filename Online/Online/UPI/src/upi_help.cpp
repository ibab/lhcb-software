/*
        upic_help.c
         Created           : 23-OCT-1989 by Christian Arnault
*/
#ifdef SCREEN
#include "UPI/upidef.h"
#include "SCR/scr.h"
#include <cstring>
#include <cstdlib>

//---------------------------------------------------------------------------
extern System Sys;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void upic_open_help  (Menu* m)  {
  Page* d = (Page*) list_malloc (sizeof(Page));
  d->lines    = m->page.cur->lines;
  scrc_get_window_position (m->window, &d->row, &d->col);
  d->row      += 2;
  d->col      += m->width;
  d->cur_line = 1;

  int cols  = 0;
  Item* i     = m->page.cur->item.first;
  for (int r=1, len=0; r<=d->lines; (r++, i = i->next))
  {
    if ((len = strlen(i->help)) > cols) cols = len;
  }
  d->cols = cols + 1;
  scrc_create_display (&d->id, d->lines, d->cols + 1, SCR::INVERSE, ON, "");

  i = m->page.cur->item.first;
  for (int r=1; r<=d->lines; (r++, i = i->next))
    scrc_put_chars (d->id, i->help, SCR::INVERSE, r, 2, 0);
  scrc_paste_display (d->id, Sys.pb, d->row, d->col);
  Sys.help = d;
}

//---------------------------------------------------------------------------
void upic_close_help () {
  scrc_delete_display (Sys.help->id);
  free (Sys.help);
  Sys.help = (Page*) 0;
}
#endif
