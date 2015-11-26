/*
        upic_param.c
         Created           : 23-OCT-1989 by Christian Arnault
*/

//---------------------------------------------------------------------------
#include <cstdarg>
#include <ctype.h>
#include "UPI/upidef.h"
#include "SCR/scr.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <limits>

extern System Sys;
 
//---------------------------------------------------------------------------
static int upic_count_store_bytes(Item* i) {
  int n = 0;
  for (Param* p = i->param.first; p; p = p->next)  {
    n += sizeof(int);
    switch (p->type)    {
    case UPI_ASC_FMT :
      n += strlen(p->val.c) + 1;
      break;
    case UPI_REAL_FMT :
      n += sizeof(double);
      break;
    default :
      n += sizeof(int);
      break;
    }
  }
  return (n);
}

//---------------------------------------------------------------------------
static char* upic_store_params_of_line(Item* i, char* buffer) {
  double* d;
  for (const Param* p = i->param.first; p; p = p->next)  {
    int* n = (int*)buffer;
    *n = p->id;
    buffer += sizeof(int);
    switch (p->type)    {
    case UPI_ASC_FMT :
      strcpy (buffer, p->val.c);
      buffer += strlen(buffer) + 1;
      break;
    case UPI_REAL_FMT :
      d = (double*) buffer;
      *d = p->val.d;
      buffer += sizeof(double);
      break;
    default :
      n = (int*)buffer;
      *n = p->val.i;
      buffer += sizeof(int);
      break;
    }
  }
  return (buffer);
}

//---------------------------------------------------------------------------
static char* upic_retreive_params_of_line(Item* i, char* buffer)   {
  Param* p = i->param.first;
  while (p)  {
    int* n = (int*) buffer;
    if (*n != p->id) return (0);
    buffer += sizeof(int);
    
    switch (p->type)    {
    case UPI_ASC_FMT :
      strcpy (p->val.c, buffer);
      buffer += strlen(buffer) + 1;
      break;
    case UPI_REAL_FMT :  {
      double* d = (double*) buffer;
      p->val.d = *d;
      buffer += sizeof(double);
      break;
    }
    default:
      n = (int*) buffer;
      p->val.i = *n;
      buffer += sizeof(int);
      break;
    }
    p = p->next;
  }
  upic_update_vars_of_line (i);
  return (buffer);
}

//---------------------------------------------------------------------------
int upic_set_param(const void *var, int id, const char *format, ...) {
  va_list args;
  Convert def, min, max, *list;
  int list_size, flag, width, fortran;

  if (upic_find_param(Sys.param.first, id)) return UPI_SS_INVPARAM;
  
  Param* p = (Param*) list_add_entry (&Sys.param, sizeof(Param));
  p->id = id;

  p->fortran = fortran = (*format == '#');
  if (fortran) format++;
  
  upic_build_format (format, p);
  width = p->size;
  p->offset = 0;
  p->buf = (char*)list_malloc (width + 1);

  va_start (args, format);
  switch (p->type)  {
  case UPI_ASC_FMT :
    def.c = va_arg (args, char*);
    min.c = va_arg (args, char*);
    max.c = va_arg (args, char*);
    break;
  case UPI_REAL_FMT :
    def.d = va_arg (args, double);
    min.d = va_arg (args, double);
    max.d = va_arg (args, double);
    break;
  default :
    def.i = va_arg (args, int);
    min.i = va_arg (args, int);
    max.i = va_arg (args, int);
    break;
  }
  list = va_arg(args, Convert*);
  list_size = va_arg(args,int);
  flag = va_arg (args, int);
  va_end (args);

  upic_print_param (p, p->buf, def);

  p->var = (int*)var;
  p->input = OFF;
  p->buf_pos = 0;
  p->list_pos = 0;

  switch (p->type)    {
  case UPI_ASC_FMT:    {
    char **q, **add = (char**)list;
    p->def.c = (char*) list_malloc (width + 1);
    p->val.c = (char*) list_malloc (width + 1);
    ::sprintf (p->def.c, p->format, def.c);
    ::sprintf (p->val.c, p->format, def.c);
    p->list = list ? list_malloc (list_size * sizeof(char*)) : 0;
    q = (char**)p->list;
    for (int i=0; i<list_size; ++i, ++add, ++q)
      *q = ::strcpy((char*)list_malloc(strlen(*add)+1), *add);
    break;
  }
  case UPI_REAL_FMT:    {
    p->def.d = def.d;
    p->val.d = def.d;
    p->min.d = min.d;
    p->max.d = max.d;
    p->list = list ? ::memcpy(list_malloc(list_size*sizeof(double)),list,list_size*sizeof(double)) : 0;
    break;
  }
  default:    {
    p->def.i = def.i;
    p->val.i = def.i;
    p->min.i = min.i;
    p->max.i = max.i;
    p->list = list ? ::memcpy(list_malloc(list_size*sizeof(int)),list,list_size*sizeof(int)) : 0;
    break;
  }
  }
  p->list_size   = list_size;
  p->flag        = flag;
  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
int upic_modify_param (int menu_id, int item_id, int param_id, ...) {
  Menu* m;
  Page* d;
  Item* i;
  Param* p;

  va_list args;
  
  int list_size, flag;
  Convert def, min, max;
  Convert* list;
  
  int status = upic_get_param (menu_id, item_id, param_id, &m, &d, &i, &p);
  if (status != UPI_SS_NORMAL) return (status);
  
  if (Sys.item.cur == i) upic_wakeup();

  va_start (args, param_id);
  switch (p->type)  {
  case UPI_ASC_FMT :
    def.c = va_arg (args, char*);
    min.c = va_arg (args, char*);
    max.c = va_arg (args, char*);
    break;
  case UPI_REAL_FMT :
    def.d = va_arg (args, double);
    min.d = va_arg (args, double);
    max.d = va_arg (args, double);
    break;
  default :
    def.i = va_arg (args, int);
    min.i = va_arg (args, int);
    max.i = va_arg (args, int);
    break;
  }
  list = va_arg (args, Convert*);
  list_size = va_arg (args, int);
  flag = va_arg (args, int);
  va_end (args);

  switch (p->type)  {
  case UPI_ASC_FMT:    {
    char **q, **add;
    if (def.c)  {
      ::sprintf (p->def.c, p->format, def.c);
      ::sprintf (p->val.c, p->format, def.c);
    }
    if (list)   {
      q = (char**)p->list;
      for (int j=0; j<p->list_size; j++, q++)  {
	if (*q) free (*q);
      }
      if (p->list) ::free(p->list);
      q = (char**) list_malloc (list_size * sizeof(char*));
      p->list = (int*)q;
      add = (char**)list;
      for (int j=0; j<list_size; j++, q++, add++)
	*q = ::strcpy ((char*)list_malloc(strlen(*add)+1), *add);
    }
    break;
  }
  case UPI_REAL_FMT :    {
    if (def.i) p->val.d = p->def.d = def.d;
    if (min.i) p->min.d = min.d;
    if (max.i) p->max.d = max.d;
    if (list)   {
      if (p->list) ::free(p->list);
      p->list = ::memcpy(list_malloc(list_size*sizeof(double)),list,list_size*sizeof(double));
    }
    break;
  }
  default:    {
    if (def.i) p->val.i = p->def.i = def.i;
    if (min.i) p->min.i = min.i;
    if (max.i) p->max.i = max.i;    
    if (list)  {
      if (p->list) free (p->list);
      p->list = ::memcpy(list_malloc(list_size*sizeof(int)),list,list_size*sizeof(int));
    }
    break;
  }
  }
  p->list_size   = list_size;
  p->flag        = flag;
  upic_print_param (p, p->buf, def);
#ifdef SCREEN
  {
    int row;
    upic_find_item_row(d->item.first, item_id, &row);
    upic_draw_item (i, row);
  }
#else
  upir_modify_param (m, i, p);
#endif
  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
int upic_get_param (int menu_id, int item_id, int param_id, Menu** m, Page** d, Item** i, Param** p)  {
  if (!(*m = upic_find_menu(menu_id))) return UPI_SS_INVMENU;
  
  *d = (*m)->page.first;
  if (!*d || !(*i = upic_find_item ((*d)->item.first, item_id)))
    return UPI_SS_INVCOMMAND;
  else if (!(*p = upic_find_param ((*i)->param.first, param_id)))
    return UPI_SS_INVPARAM;
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_refresh_param_page (int page)  {
  Menu* m = upic_find_menu(page);

  if ( !m ) return UPI_SS_INVMENU;
  Page* d = m->page.first;
  while (d)  {
    Item* i = d->item.first;
    int row = 1;
    while (i) {
      if (i->type == UPI_PARAM && i->id != -1)    {
        if (Sys.menu.cur == m) upic_wakeup();
        upic_refresh_params_in_line (i->param.first);
#ifdef SCREEN
        upic_draw_item (i, row);
#endif
      }
      row++;
      i = i->next;
    }
    d = d->next;
  }
#ifdef SCREEN
#else
  upir_refresh_param_page (m);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_refresh_param_line (int page, int item_id) {
  Menu* m;
  Item* i;
  int row;

  if (!(m = upic_find_menu(page))) return UPI_SS_INVMENU;
  Page* d = m->page.first;
  if (!d || !(i = upic_find_item_row (d->item.first, item_id, &row)))
    return UPI_SS_INVCOMMAND;
  if (i->type == UPI_PARAM && i->id != -1)  {
    if (Sys.item.cur == i) upic_wakeup();
    upic_refresh_params_in_line (i->param.first);
#ifdef SCREEN
    upic_draw_item (i, row);
#endif
  }
#ifdef SCREEN
#else
  upir_refresh_param_line (m, i);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
Param* upic_find_param (Param* first, int id)   {
  for (; first; first = first->next)
    if (first->id == id) break;
  return first;
}

//---------------------------------------------------------------------------
void upic_drop_params (Param* p)  {
  Param* p1;  
  while (p)  {
    p1 = p->next;
    upic_drop_param (p);
    p = p1;
  }
}

//---------------------------------------------------------------------------
void upic_drop_param (Param* p)   {
  ::free (p->buf);
  if (p->type == UPI_ASC_FMT)  {
    char** qc = (char**)p->list;
    free (p->def.c);
    free (p->val.c);
    for (int i=0; i<p->list_size; ++i, ++qc)  {
      if (*qc) ::free(*qc);
    }
  }
  if (p->list) ::free(p->list);
  list_remove_entry (p);
}

//---------------------------------------------------------------------------
void upic_refresh_params_in_line (Param* first) {
  for(Param* p = first; p; p = p->next)
    upic_refresh_param (p);
}

//---------------------------------------------------------------------------
void upic_refresh_param (Param* p)    {
  switch (p->type)  {
  case UPI_ASC_FMT :
    if (p->fortran)  {
      char* c = p->val.c;
      strncpy (c, (char *)p->var, p->size);
      c[p->size] = 0;
      c[upic_non_blanks(c)] = 0;
    }
    else strcpy (p->val.c, (char *)p->var);
    strcpy (p->def.c, p->val.c);
    break;
  case UPI_REAL_FMT :
    p->def.d = *((double*) p->var);
    p->val.d = p->def.d;
    break;
  default :
    p->def.i = *(p->var);
    p->val.i = p->def.i;
    break;
  }
  upic_print_param (p, p->buf, p->val);
}

//---------------------------------------------------------------------------
int upic_build_format (const char* format, Param* p)    {
  int width = 0, w2 = 0;
  char *fmt, *conv, c = char(::toupper(*format++));
  if (c == '%')  {
    if (*format == '-') format++;
    c = format[upic_non_blanks(format) - 1];
    c = char(::toupper(c));
  }
  ::sscanf (format, "%d.%d", &width, &w2);
  if (!width) return UPI_SS_INVFORM;
  if (!w2 )  {
    if ( c=='A' || c=='S' || c=='F' || c=='E' || c=='G' ) w2 = width;
    else w2 = 1;
  }

  fmt = p->format;
  conv = p->convert;
  *fmt++ = '%';
  *conv++ = '%';
  if (c == 'A' || c == 'S') *fmt++ = '-';
  sprintf (fmt, "%d.%d", width, w2);
  fmt += strlen(fmt);
  p->chars = width;
  switch (c)  {
    case 'A' :
    case 'S' :
      p->type = UPI_ASC_FMT;
      *fmt++ = 's';
      *conv++ = 's';
      break;
    case 'I' :
    case 'D' :
      p->type = UPI_DEC_FMT;
      *fmt++ = 'd';
      *conv++ = 'd';
      if (width < 14) width = 14;
      break;
    case 'M' :
      p->type = UPI_BIN_FMT;
      *fmt++ = 'd';
      *conv++ = 'd';
      break;
    case 'L' :
      p->type = UPI_LOG_FMT;
      *fmt++ = 'l';
      *conv++ = 'l';
      break;
    case 'Z' :
    case 'X' :
      p->type = UPI_HEX_FMT;
      *fmt++ = 'x';
      *conv++ = 'x';
      if (width < 9) width = 9;
      break;
    case 'O' :
      p->type = UPI_OCT_FMT;
      *fmt++ = 'o';
      *conv++ = 'o';
      if (width < 11) width = 11;
      break;
    case 'F' :
      p->type = UPI_REAL_FMT;
      *fmt++ = 'l';
      *fmt++ = 'f';
      *conv++ = 'l';
      *conv++ = 'f';
      if (width < 14) width = 14;
      break;
    case 'E' :
    case 'G' :
      p->type = UPI_REAL_FMT;
      *fmt++ = 'l';
      *fmt++ = 'e';
      *conv++ = 'l';
      *conv++ = 'e';
      if (width < 14) width = 14;
      break;
  }
  *fmt++ = '\0';
  *conv++ = '\0';
  p->size = width;
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
void upic_install_params (Param* param, char* text)   {
  char* c = text;
  while (param)  {
    size_t n;
    if ( (c=strstr(c,"^")) == 0) return;
    param->pos = c - text + 1;
    for (n=0; (n < param->chars) && (*c == '^'); n++) c++;
    param->chars = n;
    param = param->next;
  }
}

//---------------------------------------------------------------------------
int upic_check_int (int val, int min, int max)  {
  return ( min == max || ( val >= min && val <= max ) );
}

//---------------------------------------------------------------------------
int upic_check_double (double val, double min, double max)  {
  return ( fabs(min-max)<std::numeric_limits<double>::epsilon() || (val >= min && val <= max) );
}

//---------------------------------------------------------------------------
int upic_valid_numeric (int type, char c)   {
  int result = 0;
  c = char(toupper(c));
  switch (type)  {
    case UPI_REAL_FMT :
      if ((c >= '0' && c <= '9') ||
           c == '.' ||
           c == '-' ||
           c == 'E' ||
           c == '+') result = 1;
      break;
    case UPI_DEC_FMT :
      if ((c >= '0' && c <= '9') || c == '-' ) result = 1;
      break;
    case UPI_OCT_FMT :
      if (c >= '0' && c <= '7') result = 1;
      break;
    case UPI_HEX_FMT :
      if ((c >= '0' && c <= '9') ||
          (c >= 'A' && c <= 'F')) result = 1;
      break;
    case UPI_BIN_FMT :
      if (c == '.' || c == 'L') result = 1;
      break;
    case UPI_LOG_FMT :
      if (c == 'T' || c == 'F') result = 1;
      break;
  }
  return result;
}

//---------------------------------------------------------------------------
void upic_update_var_of_param (Param* p)    {
  int fortran = p->fortran;
  switch (p->type)  {
  	case UPI_ASC_FMT :
      if (fortran)    {
        memset (p->var, ' ', p->size);
        strncpy ((char *)p->var, p->val.c, strlen(p->val.c));
      }
      else strcpy ((char*)p->var, p->val.c);
      break;
    case UPI_REAL_FMT :
      if (fortran) *((float*) p->var) = float(p->val.d);
      else *((double*) p->var) = p->val.d;
      break;
    default :
      *p->var = p->val.i;
      break;
  }
}

//---------------------------------------------------------------------------
void upic_update_vars_of_line (Item* i) {
  for (Param* p = i->param.first; p;  p = p->next )
    upic_update_var_of_param (p);
}

//---------------------------------------------------------------------------
void upic_update_vars_of_page (Menu* m) {
  for (Page* d = m->page.first; d; d = d->next)  {
    Item* i = d->item.first;
    while (i)    {
      if (i->param.first && i->id != -1) upic_update_vars_of_line (i);
      i = i->next;
    }
  }
}

//---------------------------------------------------------------------------
int upic_store_vars (int menu_id, int item_id, char** buffer, int *length)
//---------------------------------------------------------------------------
/*  This function stores the current values of the parameters associated      */
/* either with a menu item, or with the complete menu, in the case the        */
/* referenced menu is A parameter page.                                       */
/*                                                                            */
/*  The operation is performed in three phases :                              */
/*   1) the total number of bytes needed is counted first,                    */
/*   2) a buffer is allocated with that number of bytes,                      */
/*   3) the values are stored in the buffer.                                  */
/*                                                                            */
/*  The format for storing the values is the following:                       */
/*                                                                            */
/*  1) for a parameter page (or if item_id is null)                           */
/*   for each item in the parameter page :                                    */
/*   {                                                                        */
/*     store id of the item;                                                  */
/*     store all the values for this item;                                    */
/*   }                                                                        */
/*                                                                            */
/*  2) for a single menu item :                                               */
/*   store all the values for this item;                                      */
/*                                                                            */
/*  The internal format for each item is :                                    */
/*                                                                            */
/*   for each parameter of this item :                                        */
/*   {                                                                        */
/*     store id of the param;                                                 */
/*     store value :                                                          */
/*       either an integer;                                                   */
/*              a double;                                                     */
/*              a text_string :                                               */
/*                the length,                                                 */
/*                the null-terminated string;                                 */
/*   }                                                                        */
/*                                                                            */
//---------------------------------------------------------------------------
{
  Menu* m;
  Item* i;
  Page* d;
  char* buf;
  int bytes;
  int* n;

  *length = 0;
  *buffer = 0;
  if (!(m = upic_find_menu(menu_id))) return UPI_SS_INVMENU;
  bytes = 0;
  if (m->type == UPI_PARAMETER_PAGE || !item_id)  {
    d = m->page.first;
    while (d)  {
      i = d->item.first;
      while (i)   {
        if (i->type == UPI_PARAM && i->id != -1)      {
          bytes += sizeof(int);  /* for item_id */
          bytes += upic_count_store_bytes (i);
        }
        i = i->next;
      }
      d = d->next;
    }
    buf = (char*) list_malloc (bytes);
    *buffer = buf;
    *length = bytes;
    d = m->page.first;
    while (d)  {
      i = d->item.first;
      while (i)   {
        if (i->type == UPI_PARAM && i->id != -1)  {
          n = (int*) buf;
          *n = i->id;
          buf += sizeof(int);
          buf = (char*) upic_store_params_of_line (i, buf);
        }
        i = i->next;
      }
      d = d->next;
    }
  }
  else   {
    d = m->page.first;
    if (!d || !(i = upic_find_item (d->item.first, item_id)))
      return UPI_SS_INVCOMMAND;
    if (i->type == UPI_PARAM && i->id != -1)    {
      bytes = upic_count_store_bytes (i);
      buf = (char*) list_malloc (bytes);
      *buffer = buf;
      *length = bytes;
      upic_store_params_of_line (i, buf);
    }
  }
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_retreive_vars (int menu_id, int item_id, char* buffer)
//---------------------------------------------------------------------------
/*  This function retreives the values of the parameter, according to the     */
/* conventions used in the function upic_store_vars.                          */
/*                                                                            */
//---------------------------------------------------------------------------
{
  Menu* m;
  Item* i;
  Page* d;
  int* n;

  if (!(m = upic_find_menu(menu_id))) return UPI_SS_INVMENU;
  if (m->type == UPI_PARAMETER_PAGE || !item_id)  {
    d = m->page.first;
    while (d)    {
      i = d->item.first;
      while (i)      {
        if (i->type == UPI_PARAM && i->id != -1)   {
          n = (int*) buffer;
          if (*n != i->id) return (UPI_SS_INVCOMMAND);
          buffer += sizeof(int);
          buffer = (char*) upic_retreive_params_of_line (i, buffer);
          if (!buffer) return (UPI_SS_INVPARAM);
        }
        i = i->next;
      }
      d = d->next;
    }
  }
  else  {
    d = m->page.first;
    if (!d || !(i = upic_find_item (d->item.first, item_id)))
      return UPI_SS_INVCOMMAND;
    if (i->type == UPI_PARAM && i->id != -1)    {
      buffer = upic_retreive_params_of_line (i, buffer);
      if (!buffer) return (UPI_SS_INVPARAM);
    }
  }
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
void upic_restore_params_in_line (Item* i)  {
  Param* p = i->param.first;
  while (p)  {
    upic_print_param (p, p->buf, p->val);
    p = p->next;
  }
}

//---------------------------------------------------------------------------
void upic_restore_params_in_page (Menu* m)    {
  Page* d = m->page.first;
  while (d)  {
    Item *i = d->item.first;
    while (i)    {
      if (i->param.first) upic_restore_params_in_line (i);
      i = i->next;
    }
    d = d->next;
  }
}

//---------------------------------------------------------------------------
int upic_itom (char* buffer, int digits, Unsigned value)  {
  if (digits < 1 || digits > 32) return 0;
  digits--;
  for (int i = 1 << digits; i; i >>= 1, buffer++)  {
    *buffer = (value & i) ? 'l' : '.';
  }
  *buffer = '\0';
  return digits;
}

//---------------------------------------------------------------------------
Unsigned upic_mtoi (char* buffer, int digits)   {
  if (digits < 1 || digits > 32) return 0;
  int i = strlen(buffer);
  if (digits > i) digits = i;
  Unsigned value = 0;
  digits--;
  i = 1 << digits;
  for (; i; i >>= 1, buffer++)  {
    if (*buffer != '.') value |= i;
  }
  return value;
}

//---------------------------------------------------------------------------
int upic_itol (char* buffer, int digits, Unsigned value)    {
  int i;
  if ((value & 1)) *buffer = 'T';
  else *buffer = 'F';
  for (buffer++, i=1; i<digits; i++, buffer++) *buffer = ' ';
  *buffer = '\0';
  return digits;
}

//---------------------------------------------------------------------------
int upic_ltoi ( char* buffer, int /* digits */ )   {
  char c = char(toupper(*buffer));
  return (c == 'T') ? 1 : 0;
}

//---------------------------------------------------------------------------
void upic_print_param (Param* p, char* buf, Convert source) {
  switch (p->type)  {
    case UPI_BIN_FMT :
      upic_itom (buf, p->chars, source.i);
      break;
    case UPI_LOG_FMT:
      upic_itol (buf, p->chars, source.i);
      break;
    case UPI_REAL_FMT:
      ::sprintf (buf, p->format, source.d);
      break;
    case UPI_ASC_FMT:
      ::sprintf (buf, p->format, source.c);
      break;
    case UPI_OCT_FMT:
    case UPI_DEC_FMT:
    case UPI_HEX_FMT:
      ::sprintf (buf, p->format, source.i);
      break;
  }
}

//---------------------------------------------------------------------------
int upic_find_list_elem (int menu_id, int item_id, int param_id)  {
  Menu* m;
  Page* d;
  Item* i;
  Param* p;
  int status = upic_get_param (menu_id, item_id, param_id, &m, &d, &i, &p);
  if (status != UPI_SS_NORMAL) return (-1);
  
  int list_size = p->list_size;
  if (!list_size) return (-1);
  if (p->type == UPI_ASC_FMT)  {
    int len1 = upic_non_blanks (p->val.c);
    char** q = (char **)p->list;
    for (int j=0; j<p->list_size; j++, q++)    {
      if (*q)  {
        int len2 = upic_non_blanks(*q);
        if ((len1 == len2) && !strncmp (*q, p->val.c, len1)) return (j);
      }
    }
  }
  else if (p->type == UPI_REAL_FMT)  {
    double* q = (double*)p->list;
    for (int j=0; j<list_size; j++, q++)    {
      if ( fabs(*q-p->val.d) < std::numeric_limits<double>::epsilon() ) return (j);
    }
  }
  else  {
    int* q = (int*)p->list;
    for (int j=0; j<list_size; j++, q++)    {
      if (*q == p->val.i) return (j);
    }
  }
  return (-1);
}

//---------------------------------------------------------------------------
int upic_set_value_from_list (int menu_id, int item_id, int param_id, int elem) {
  Menu* m;
  Page* d;
  Item* i;
  Param* p;
  int status = upic_get_param (menu_id, item_id, param_id, &m, &d, &i, &p);
  if (status != UPI_SS_NORMAL) return (status);
  
  int list_size = p->list_size;
  if (!list_size && elem >= list_size) return (UPI_SS_INVPARAM);

  if (p->type == UPI_ASC_FMT)  {
    char** q = (char **)p->list;
    q += elem;
    strcpy (p->val.c, *q);
  }
  else if (p->type == UPI_REAL_FMT)  {
    double* q = (double *)p->list;
    q += elem;
    p->val.d = *q;
  }
  else  {
    int* q = (int*)p->list;
    q += elem;
    p->val.i = *q;
  }
  
  upic_update_var_of_param (p);
  upic_refresh_param (p);
  return (-1);
}

#ifdef SCREEN
//---------------------------------------------------------------------------
void upic_draw_param (Page*  page, Param* param, int row, int attrib, int /* offs */) {
  int attr = attrib | SCR::UNDERLINE;
  size_t col = param->pos;
  size_t pos = (param->buf_pos > param->chars) ? param->chars - 1 : param->buf_pos;
  size_t offset = (param->input) ? (param->buf_pos - pos) : 0;
  if (param->size > param->chars)  {
    char* buf    = param->buf + offset;
    char* last   = buf + param->chars;
     char c = *last;
    *last = '\0';
    scrc_put_chars (page->id, buf, attr, row, col, 0);
    *last = c;    
    if (offset)    {
      scrc_put_char (page->id, '<', attr, row, col);
    }
    if (upic_non_blanks(buf) > param->chars)    {
      scrc_put_char (page->id, '>', attr, row, col + param->chars - 1);
    }
  }
  else
    scrc_put_chars (page->id, param->buf, attr, row, col, 0);
}
#endif
