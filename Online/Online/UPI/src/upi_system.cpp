/*
        upic_SYS.C
         Created           : 23-OCT-1989 by Christian Arnault
*/
 
//---------------------------------------------------------------------------
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "UPI/upidef.h"
#include "SCR/scr.h"
#ifdef VAX
#include <descrip.h>
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
System Sys;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifdef _VMS
static struct {
  int link;
  Routine handler;
  int args;
  int arg[7];
} Exit_block;
  
static int Exit_status;
#endif
static Routine User_exit_handler = 0;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static int upic_memory_of_param (Param* p)    {
  int n;
  int memory = sizeof (Param) + p->size + 1;  /* for p->buf  */
  if (p->type == ASC_FMT) memory += p->size + 1; /* for p->def */
  
  if ((n = p->list_size))  {
    switch (p->type)    {
      case ASC_FMT:
        {
          char** list = (char **)p->list;
          memory += n * sizeof (char*);
          for (;n;n--,list++)
            memory += strlen (*list) + 1;
        }
        break;
      case REAL_FMT :
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
  Param* p = i->param.first;
  while (p)  {
    memory += upic_memory_of_param (p);
    p = p->next;
  }
  memory += strlen(i->string) + 1 +
            strlen(i->help) + 1;

  return (memory);
}

//---------------------------------------------------------------------------
static int upic_memory_of_page (Page* d)  {
  int memory = sizeof (Page);
  Item* i = d->item.first;
  while (i)  {
    memory += upic_memory_of_item (i);
    i = i->next;
  }
  
  return (memory);
}

//---------------------------------------------------------------------------
static int upic_memory_of_menu (Menu* m)  {
  int memory = sizeof (Menu);
  Page* d = m->page.first;
  while (d)  {
    memory += upic_memory_of_page (d);
    d = d->next;
  }
  memory += strlen (m->mn_title) + 1 +
            strlen (m->up_title) + 1 +
            strlen (m->bt_title) + 1;
  return (memory);
}
//---------------------------------------------------------------------------
static void upic_exit_handler (int* /* status */ )  {
  if (User_exit_handler) User_exit_handler(0,0);
#ifndef SCREEN
  upic_quit();
#endif
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
  
#ifdef VAX
  Exit_block.link = 0;
  Exit_block.handler = upic_exit_handler;
  Exit_block.args = 1;
  Exit_block.arg[0] = &Exit_status;
  SYS$DCLEXH (&Exit_block);
#endif
  
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
  Menu* m;
  Item* i;
  Async_line* a;

  if (Sys.id) Sys.id--;
  if (Sys.id) return UPI_SS_NORMAL;
  
  while ((m = Sys.menu.first)) upic_drop_menu (m);
  upic_drop_params (Sys.param.first);
  while ((i = Sys.item.first)) upic_drop_item (i);
  while ((a = Sys.async.first)) upic_drop_async (a);
  
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
  if ((mode & WAKE_UP_ON_CHANGE)) upic_set_wakeup();
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
#ifdef VAX
int upic_signal_error (int code,const char* text) {
  short len;
  int flag;
  int status;
  static char Buffer[133];
  static $DESCRIPTOR (Dbuffer, Buffer);
  int p_code = code;

  flag = 15;
  status = LIB$SYS_GETMSG (&p_code, &len, &Dbuffer, &flag, 0);
  if (len > 132) len = 132;
  Buffer[len] = '\0';
  if (len + strlen(text) <= 132) strcat (Buffer, text);
  return (upic_write_message (Buffer, text));
#else
int upic_signal_error (int, const char* ) {
#endif
  return UPI_SS_NORMAL;
}


/*--------------------------------------------------*/
#ifdef REMOTE
int upic_connect_process (const char* name) {
  upir_connect_process (name);
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
  Menu* m = Sys.menu.first;
  while (m)  {
    memory += upic_memory_of_menu (m);
    m = m->next;
  }  
  Param* p = Sys.param.first;
  while (p)  {
    memory += upic_memory_of_param (p);
    p = p->next;
  }
  Item* i = Sys.item.first;
  while (i)  {
    memory += upic_memory_of_item (i);
    i = i->next;
  }
  Async_line* a = Sys.async.first;
  while (a)  {
    memory += upic_memory_of_async (a);
    a = a->next;
  }

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
