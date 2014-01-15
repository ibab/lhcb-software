//---------------------------------------------------------------------------
#include <cstdio>
#include <cstring>
#include <cstdlib>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#include "fcntl.h"
#endif
#include "SCR/scr.h"
#include "UPI/file.h"
#include "UPI/upidef.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#define mini(a,b) (a<b?a:b)
#define maxi(a,b) (a>b?a:b)

#define mini(a,b) (a<b?a:b)
#define maxi(a,b) (a>b?a:b)

#ifdef SCREEN
#define put_char(hh,rr,cc) scrc_put_chars (d0, hh, SCR::GRAPHIC, rr, cc, 0)
static char    Max_text[80];
#endif

#define ROWS 21
//---------------------------------------------------------------------------

/*---------------------- Global variables -----------------------------------*/
static struct {
  int initialized;
  Histo Linked_list_items;
} Header = {0, 0, 0};

static Pasteboard* Pb = 0;

static Histo* Current_histo = 0;
static Routine Drag_action = 0;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
static void upic_init_histos ()   {
  if (!Header.initialized)  {
    Header.initialized = 1;
    list_init (&Header.first);
    upic_get_info (&Pb);
  }
}

//---------------------------------------------------------------------------
Histo* upic_book_histo (const char* text, int _bins, int _rows, double _min, double _max) {
  Histo* h;
  double* p;

  upic_init_histos();
  if (!(h = upic_find_histo(text)))
  {
    h = (Histo*) list_add_entry (&Header.first, sizeof(Histo));
    h->text = (char*) list_malloc (strlen(text) + 1);
    strcpy (h->text, text);
    h->pasted = 0;
  }
  else
  {
    h->bins = 0;
    if (h->first) free (h->first);
#ifdef SCREEN
    if (h->disp) scrc_delete_display (h->disp);
#endif
  }

  h->disp = 0;
  h->first = (double*) list_malloc (_bins*sizeof(double));

  h->min     = _min;
  h->max     = _max;
  h->width   = (_max - _min) / _bins;
  h->underflow = 0;
  h->overflow  = 0;
  h->highest   = 0;
  h->scaled    = 0;
  h->bins    = _bins;

  p = h->first;
  for (int j=0; j<h->bins; j++, p++) *p = 0;

  if (_rows) h->rows = _rows;
  else h->rows = ROWS;
  h->cols = mini (76, h->bins) + 2;

#ifdef SCREEN
  if (h->pasted) upic_display_histo (h, h->row, h->col);
#else
  upir_book_histo (h);
#endif

  return (h);
}

//---------------------------------------------------------------------------
int upic_unbook_histo (Histo* h)  {
  if (!h) return (0);

  if (h->first) free (h->first);
  if (h->text) free (h->text);
#ifdef SCREEN
  if (h->disp)  {
    scrc_delete_display (h->disp);
    h->disp = 0;
  }
#else
  upir_unbook_histo (h);
#endif
  list_remove_entry (h);
  return (1);
}

//---------------------------------------------------------------------------
void upic_scale_histo (Histo* h, double maxw) {
  if (!h) return;
  h->highest = maxw;
  h->scaled = 1;
#ifdef SCREEN
  if (h->pasted) upic_display_histo (h, h->row, h->col);
#else
  upir_scale_histo (h, maxw);
#endif

}

//---------------------------------------------------------------------------
void upic_unscale_histo (Histo* h)   {
  if (!h) return;
  h->highest = 0;
  h->scaled = 0;
#ifdef SCREEN
  if (h->pasted) upic_display_histo (h, h->row, h->col);
#else
  upir_unscale_histo (h);
#endif

}

//---------------------------------------------------------------------------
int upic_fill_histo (Histo* _h, double _x, double _w)    {
  if (!_h) return 0;

  if      (_x <  _h->min) _h->underflow += _w;
  else if (_x >= _h->max) _h->overflow += _w;
  else  {
    int bin = int((_x - _h->min) / _h->width);
    double* p   = _h->first;
    if (p) p[bin] += _w;
  }
#ifdef SCREEN
  if (_h->pasted) upic_display_histo (_h, _h->row, _h->col);
#else
  upir_fill_histo (_h, _x, _w);
#endif
  return 1;
}

//---------------------------------------------------------------------------
Histo* upic_find_histo (const char* text)   {
  upic_init_histos();
  Histo* h = Header.first;
  while (h)  {
    if (!strcmp (h->text, text)) return (h);
    h = h->next;
  }
  return (0);
}

//---------------------------------------------------------------------------
void upic_save_histo (Histo* _h, const char* _file_name)    {
  char* t;
  int path;
  if (!_h) return;

  if (_file_name)  {
    t = (char*) list_malloc (strlen(_file_name) + 1);
    strcpy (t, _file_name);
  }
  else  {
    t = (char*) list_malloc (strlen(_h->text) + 5);
    strcpy (t, _h->text);
    strcat (t, ".HIS");
  }
  path = open (t, O_RDWR+O_CREAT);
  if (path != -1)  {
    write (path, _h, sizeof(Histo));
    write (path, _h->first, _h->bins * sizeof(double));
    close (path);
  }
  free(t);
}

//---------------------------------------------------------------------------
Histo* upic_restore_histo (const char* text, const char* file_name) {
  Histo* h;
  static Histo htemp;
  char* t;
  int path;

  upic_init_histos();
  h = (Histo*) 0;
  if (file_name)  {
    t = (char*) list_malloc (strlen(file_name) + 1);
    strcpy (t, file_name);
  }
  else  {
    t = (char*) list_malloc (strlen(text) + 5);
    ::strcpy (t, text);
    ::strcat (t, ".HIS");
  }
  path = ::open(t, O_RDONLY);
  if (path != -1)  {
    ::read (path, &htemp, sizeof(Histo));
    htemp.text  = (char*)text;
    htemp.first = (double*) list_malloc (htemp.bins*sizeof(double));
    ::read (path, htemp.first, htemp.bins*sizeof(double));
    h = (Histo*) upic_copy_histo (&htemp);
    ::free (htemp.first);
    ::close(path);
  }
  free(t);
  return (h);  
}

//---------------------------------------------------------------------------
Histo* upic_copy_histo (Histo* h_source)    {
  Histo* h;

  upic_init_histos();
  if (!(h = upic_find_histo(h_source->text)))  {
    h = (Histo*) list_add_entry (&Header.first, sizeof(Histo));
    Histo* p = h->prev;
    Histo* n = h->next;
    int*   f = (int *)h->father;
    size_t l = ::strlen(h_source->text)+1;
    *h = *h_source;
    h->prev = p;
    h->next = n;
    h->father = (Linked_list *)f;

    h->text = (char*)list_malloc(l);
    ::strncpy (h->text, h_source->text,l);
    h->disp = 0;
  }
  else    {
    Histo* p;
    Histo* n;
    int* f;

    p = h->prev;
    n = h->next;
    f = (int *)h->father;

    if (h->first) free (h->first);
#ifdef SCREEN
    if (h->disp)
    {
      scrc_delete_display (h->disp);
      h->disp = 0;
    }
#endif
    *h = *h_source;
    h->prev = p;
    h->next = n;
    h->father = (Linked_list *)f;
  }

  h->first = (double*) list_malloc (h->bins*sizeof(double));

  ::memcpy (h->first, h_source->first, h->bins*sizeof(double));
#ifdef SCREEN
  if (h->pasted) upic_display_histo (h, h->row, h->col);
#else
  upir_copy_histo (h);
#endif
  return (h);  
}

//---------------------------------------------------------------------------
int upic_bins_of_histo (Histo* _h)    {
  if (!_h) return 0;
  else return _h->bins;
}

//---------------------------------------------------------------------------
double upic_min_of_histo (Histo* _h)    {
  if (!_h) return 0;
  else return _h->min;
}

//---------------------------------------------------------------------------
double upic_max_of_histo (Histo* _h)    {
  if (!_h) return 0;
  else return _h->max;
}

//---------------------------------------------------------------------------
void upic_display_histo (Histo* h, int row, int col)    {
#ifdef SCREEN
  int bin[78];
  char str_mark[30];
  int* q, i, j, l, row0, max_bin, dh;
  double *p, bmax, scale, mark;
  char* c;
  Display* d0;
  int scaled;

  if (!h) return;
  if (!(d0 = h->disp))  {
    scrc_create_display (&d0, h->rows, h->cols, SCR::NORMAL, ON, h->text);
    scrc_enable_drag (d0, upic_drag_histo);
    h->disp = d0;
    h->pasted = 0;
  }
  scrc_begin_pasteboard_update (Pb);
  if (h->pasted)  {
    row = h->row;
    col = h->col;
  }
  else
  {
    scrc_paste_display (h->disp, Pb, row, col);
    h->pasted = 1;
    h->row = row;
    h->col = col;
  }

  scaled = h->scaled;
  if (!h->rows) h->rows = ROWS;
  h->cols = mini (76, h->bins) + 2;

  bmax = 0;
  row0    = h->rows - 2;
  max_bin = h->rows - 4;
  bin[0] = int(h->underflow);
  for (i=1, p = h->first; i<h->cols-1; i++,p++)  {
    bin[i] = int(*p);
    if (*p > bmax && !scaled) bmax = *p;
  }

  bin[i] = int(h->overflow);
  if (scaled) bmax = h->highest;
  if (bmax >= max_bin || h->scaled)  {
    scale = (double) max_bin / bmax;
    q = &bin[0];
    for (i=0; i<h->cols; i++,q++)  {
      j = int(scale * (*q));
      if (j < 0) j = 0;
      if (j > max_bin) j = max_bin;
      *q = j;
    }
  }

  for (i=1; i<h->rows; i++) scrc_put_chars (d0, " ", SCR::NORMAL, i, 1, 1);

  sprintf (Max_text, " %f", bmax);
  scrc_put_chars (d0, Max_text, SCR::NORMAL, 1, 1, 0);

  /* Bottom scale */
  scrc_put_chars (d0, "<", SCR::NORMAL, h->rows-1, 1, 0);
  for (i=2; i<h->cols; i++)  {
    if ( !((i-2) % 5) )  {
      scrc_put_chars (d0, "n", SCR::GRAPHIC, h->rows-1, i, 0);
      if ( !((i-2) % 10) )   {
        mark = h->min + ((h->max - h->min) * (double)(i-2) / h->bins);
        sprintf(str_mark,"%f",mark);
        l = strlen(str_mark) - 1;
        j = l;
        /* suppress trailing 0's and center on dot */
        for(c=str_mark+l; j>0; c--,j--)  {
          if (str_mark[l] == '0') l--;
          if (*c == '.') break;
        }
        str_mark[l+1] = 0;
        scrc_put_chars (d0, str_mark, SCR::NORMAL, h->rows, i-j, 0);
      }
    }
    else scrc_put_chars (d0, "v", SCR::GRAPHIC, h->rows-1, i, 0);
  }
  scrc_put_chars (d0, ">", SCR::NORMAL, h->rows-1, h->cols, 0);


  for (i=row0-bin[0]; i<row0; i++) scrc_put_chars (d0, " ", SCR::INVERSE, i, 1, 0);

  row = row0 - bin[1];
  col = 2;
  for (i=2; i<=row0; i++) put_char ("x", i, col);
  put_char ("t", row, col);
  for (i=2,col=3; col<h->cols; i++,col++)  {
    dh = bin[i] - bin[i-1];
    if (dh == 0)    {
      put_char ("q", row, col);
    }
    else if (dh > 0)    {
      put_char ("j", row--, col);
      for (j=0; j<(dh-1); j++, row--)
        put_char ("x", row, col);
      put_char ("l", row, col);
    }
    else if (dh < 0)    {
      dh = -dh;
      put_char ("k", row++, col);
      for (j=0; j<(dh-1); j++, row++)
        put_char ("x", row, col);
      put_char ("m", row, col);
    }
  }

  for (i=row0-bin[h->cols-1]; i<row0; i++)
    scrc_put_chars (d0, " ", SCR::INVERSE, i, h->cols, 0);

  scrc_end_pasteboard_update (Pb);
#else
  if (h->pasted)  {
    row = h->row;
    col = h->col;
  }
  else  {
    h->pasted = 1;
    h->row = row;
    h->col = col;
  }

  upir_display_histo (h, row, col);
#endif
}

//---------------------------------------------------------------------------
void upic_hide_histo (Histo* h)   {
  if ( h )  {
    if (h->pasted)  {
#ifdef SCREEN
      scrc_unpaste_display (h->disp, Pb);
#else
      upir_hide_histo (h);
#endif
      h->pasted = 0;
    }
  }
}

//---------------------------------------------------------------------------
void upic_set_drag_histo_action (Routine action)  {
  Drag_action = action;
}

//---------------------------------------------------------------------------
int upic_drag_histo (Display* /* id */, int row, int col)   {
#ifdef SCREEN
  upic_move_histo (Current_histo, row, col);
#else
  if (Current_histo)  {
    Current_histo->row = row;
    Current_histo->col = col;
  }
#endif
  if (Drag_action) (*Drag_action)(row, col);
  Current_histo = 0;
  return 1;
}

/*--------------------------------------------------*/
void upic_move_histo (Histo* h, int row, int col) {
  if (h)  {
#ifdef SCREEN
    scrc_move_display (h->disp, Pb, row - h->row, col - h->col);
#else
    upir_move_histo (h, row, col);
#endif
    h->row = row;
    h->col = col;
  }
}

/*--------------------------------------------------*/
void upic_pos_of_histo (Histo* h, int* row, int* col) {
  *row = h->row;
  *col = h->col;
}

/*--------------------------------------------------*/
void upic_moving_histo (Histo* h) {
  Current_histo = h;
#ifdef SCREEN
  scrc_moving_display (h->disp);
#else
  upir_moving_histo (h);
#endif
}
