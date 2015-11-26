/*
        upic_SYS.C
         Created           : 23-OCT-1989 by Christian Arnault
*/
 
//---------------------------------------------------------------------------
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "RTL/rtl.h"
#include "SCR/scr.h"
#include "UPI/upidef.h"
#ifdef VAX
#include <descrip.h>
#endif
//---------------------------------------------------------------------------
System Sys;
static Routine User_exit_handler = 0;

//---------------------------------------------------------------------------
static int upic_memory_of_param (Param* p)    {
  int n;
  int memory = sizeof (Param) + p->size + 1;  /* for p->buf  */
  if (p->type == UPI_ASC_FMT) memory += p->size + 1; /* for p->def */
  if ((n = p->list_size))  {
    switch (p->type)    {
      case UPI_ASC_FMT:
        {
          char** list = (char **)p->list;
          memory += n * sizeof (char*);
          for (;n;n--,list++)
            memory += strlen (*list) + 1;
        }
        break;
      case UPI_REAL_FMT :
        memory += n * sizeof (double);
        break;
      default :
        memory += n * sizeof (int);
        break;
    }
  }  
  return (memory);
}

//---------------------------------------------------------------------------
static int upic_memory_of_async (Async_line* a) {
  return sizeof (Async_line)+ strlen(a->text) + 1;
}

//---------------------------------------------------------------------------
static int upic_memory_of_item (Item* i)    {
  int memory = sizeof (Item);
  for(Param* p = i->param.first;p;p = p->next)
    memory += upic_memory_of_param (p);
  memory += strlen(i->string) + 1 +
            strlen(i->help) + 1;
  return memory;
}

//---------------------------------------------------------------------------
static int upic_memory_of_page (Page* d)  {
  int memory = sizeof (Page);
  for(Item* i = d->item.first;i;i = i->next)
    memory += upic_memory_of_item (i);
  return memory;
}

//---------------------------------------------------------------------------
static int upic_memory_of_menu (Menu* m)  {
  int memory = sizeof (Menu);
  for( Page* d = m->page.first; d; d=d->next)
    memory += upic_memory_of_page (d);
  memory += strlen (m->mn_title) + 1 +
            strlen (m->up_title) + 1 +
            strlen (m->bt_title) + 1;
  return (memory);
}
//---------------------------------------------------------------------------
static int upic_exit_handler (void* /* status */ )  {
  if (User_exit_handler) User_exit_handler(0,0);
#ifndef SCREEN
  upic_quit();
#endif
  return 1;
}

//---------------------------------------------------------------------------
int upic_attach_terminal ()   {
  static bool first = true;
  if ( first ) {
    first = false;
    memset(&Sys,0,sizeof(Sys));
  }
  Sys.id++;
  if (Sys.id > 1) return UPI_SS_NORMAL;    
  list_init ((Linked_list*)&Sys.menu);
  Sys.menu.cur    = (Menu*) 0;
  
  list_init ((Linked_list*)&Sys.param);
  Sys.param.cur   = (Param*) 0;
  
  list_init ((Linked_list*)&Sys.item);
  Sys.item.cur    = (Item*) 0;
  Sys.window      = 0;
  Sys.detached_window = 0;
  
  lib_rtl_declare_exit(upic_exit_handler,0);
#ifdef SCREEN
  upic_init_screen();
  Sys.items_per_page = Sys.pb_rows - 6;
  Sys.pf1.menu = 0;
  Sys.pf1.item = 0;  
  for (int i=0; i<10; i++)    {
    Sys.keypad[i].menu = 0;
    Sys.keypad[i].item = 1;
  }
  upic_draw_message();
#else
  Sys.items_per_page = 0x7fffffff;
  upir_init_remote ();
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_declare_exit_handler (Routine handler) {
  User_exit_handler = handler;
  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
int upic_declare_ast_handler (Routine /* handler */ )  {
  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
int upic_quit ()    {
  if (Sys.id) Sys.id--;
  if (Sys.id) return UPI_SS_NORMAL;
  
  for(Menu* m=Sys.menu.first; m; m=Sys.menu.first) upic_drop_menu (m);
  upic_drop_params (Sys.param.first);
  for(Item* i=Sys.item.first; i; i=Sys.item.first) upic_drop_item (i);
  for(Async_line* a=Sys.async.first; a; a=Sys.async.first) upic_drop_async (a);
  
  Sys.id = 0;
#ifdef SCREEN
  scrc_cursor_on(Sys.pb);
  if (Sys.async.id) scrc_delete_display (Sys.async.id);
  if (Sys.pop_up) scrc_delete_display (Sys.pop_up->id);
  if (Sys.help) scrc_delete_display (Sys.help->id);
  upic_erase_screen();
  scrc_resetANSI();
#else
  upir_quit ();
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_set_mode (int mode)  {
#ifdef SCREEN
  Sys.mode = mode;
  if ((mode & UPI_WAKE_UP_ON_CHANGE)) upic_set_wakeup();
  else upic_reset_wakeup();
#else
  upir_set_mode (mode);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_get_mode ()    {
#ifdef SCREEN
  return (Sys.mode);
#else
  int mode;
  upir_get_mode (&mode);
  return mode;
#endif
}

//---------------------------------------------------------------------------
System* upic_get_system ()    {
  return (&Sys);
}

//---------------------------------------------------------------------------
int upic_get_info ( Pasteboard** pb)  {
  if (pb) *pb = Sys.pb;
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
void upic_set_message_callback (Routine callback)   {
  Sys.message_callback = callback;
}

//---------------------------------------------------------------------------
unsigned long upic_non_blanks (const char* buf) {
  unsigned long len = strlen(buf);
  buf += len - 1;
  while (len && *buf == ' ')  {
    len--;
    buf--;
  }
  return len;
}

//---------------------------------------------------------------------------
int upic_save_setup ()    {
  FILE* f;
  if (!(f = fopen ("upi.setup", "w+"))) return UPI_SS_NORMAL;
  fprintf (f, "%d Items_per_page\n", Sys.items_per_page);
  fprintf (f, "%d Mode\n", Sys.mode);
  Async* a = &Sys.async;
  fprintf (f, "%d Message_window_rows\n", a->rows);
  fprintf (f, "%d Message_window_cols\n", a->cols);
  fprintf (f, "%d Message_window_row\n", a->row);
  fprintf (f, "%d Message_window_col\n", a->col);
  fclose (f);
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_restore_setup ()   {
  FILE* f;
  static char format[] = {'%','d','%','*','s','\n','\0'};
  if (!(f = fopen ("upi.setup", "r"))) return UPI_SS_NORMAL;
  fscanf (f, format, &Sys.items_per_page);
  fscanf (f, format, &Sys.mode);
  Async* a = &Sys.async;
  fscanf (f, format, &a->rows);
  fscanf (f, format, &a->cols);
  fscanf (f, format, &a->row);
  fscanf (f, format, &a->col);
  fclose (f);
  return UPI_SS_NORMAL;
}

/*--------------------------------------------------*/
int upic_signal_error (int code,const char* text) {
  const char* errmsg = ::lib_rtl_error_message(code);
  return upic_write_message (errmsg, text);
}

/*--------------------------------------------------*/
#ifdef REMOTE
int upic_connect_process (const char* name) {
  upir_connect_process(name);
#else
int upic_connect_process (const char* ) {
#endif
  return UPI_SS_NORMAL;
}


/*--------------------------------------------------*/
#ifdef REMOTE
int upic_disconnect_process (const char* name)  {
  upir_disconnect_process (name);
#else
int upic_disconnect_process (const char*)  {
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_show_memory ()   {
  int memory = sizeof(System);
  for(Menu* m = Sys.menu.first;m;m = m->next) 
    memory += upic_memory_of_menu (m);
  for(Param* p = Sys.param.first;p;p = p->next)
    memory += upic_memory_of_param (p);
  for(Item* i = Sys.item.first;i;i = i->next)
    memory += upic_memory_of_item (i);
  for(Async_line* a = Sys.async.first;a;a = a->next)
    memory += upic_memory_of_async (a);

#ifdef SCREEN  
  if (Sys.pop_up) memory += scrc_memory_of_display (Sys.pop_up->id);
  if (Sys.help) memory += scrc_memory_of_display (Sys.help->id);
  if (Sys.async.id) memory += scrc_memory_of_display (Sys.async.id);
  memory += scrc_memory_of_pasteboard (Sys.pb);
  memory += scrc_memory_of_windows();
#else
#endif
  return (memory);
}
