/*
        upic_remote.c
         Created           : 17-NOV-1989 by Christian Arnault
*/

#ifdef REMOTE
    
//----------------------------------------------------------------------------
#include "UPI/upidef.h"
#include "UPI/upirem.h"
#include "WT/wt_facilities.h"
#include "WT/wtdef.h"
#include "SCR/lists.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define FRAME_SIZE 1500



typedef struct QUEUE   Queue;

struct QUEUE {
  Queue Link_items;

  Connect* connect;
  int code;
  union {
    struct {
      int menu_id;
      int item_id;
      int param_id;
      int list_index;
    } get_input;
    struct {
      int message_window_rows;
      int message_window_cols;
      int message_window_row;
      int message_window_col;
    } get_message_window;
    struct {
      int window_position_col;
      int window_position_row;
    } get_window_position;
    struct {
      int window_size_col;
      int window_size_row;
    } get_window_size;
    struct {
      int items_per_page;
    } get_items_per_page;
    struct {
      int mode;
    } get_mode;
    struct {
      int dldec_value;
    } dldec;
    struct {
      char* dltxt_value;
    } dltxt;
  } b;
};
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
static struct {
  struct {
    Connect Linked_list_items;
  } connect;
  struct {
    Queue Linked_list_items;
  } queue;
  int last_id;
} Header;


static int Updating = 0;
static char* Buffer = 0;
static int Frame = 0;
static int Pos = 0;
static int Pos_start = 0;
static char* Get_buffer = 0;
static int Get_pos = 0;

static int Hidden = 0;

static char My_name[80] = "";
static char Server_name[80] = "";
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
extern System Sys;

static void fill_int (int i);

//----------------------------------------------------------------------------
static void send_always (const char* source, int hidden)    {
  if (!Buffer || !Pos) return;

  int* p = (int*) (Buffer + Pos_start);
  *p = Pos - Pos_start;
  if (Updating)  {
    Pos_start = Pos;
    fill_int(0);
  }
  else  {
    if (!hidden)  {
      if (source) {
        upic_net_send_to_name (Buffer, Pos, source);
      }
      else   {
        for (Connect* c = Header.connect.first; c; c = c->next)  {
          upic_net_send_to_name (Buffer, Pos, c->source);
        }
      }
    }
    free (Buffer);
    Buffer = 0;
  }
}

//----------------------------------------------------------------------------
static void send (const char* source)   {
  send_always (source, Hidden);
}

//----------------------------------------------------------------------------
static Connect* find_connect (const char* source) {
  for( Connect* c = Header.connect.first; c; c = c->next)   {
    if (!strcmp(c->source, source)) return (c);
  }
  Connect* c1 = (Connect*) list_add_entry (&Header.connect, sizeof (Connect));
  c1->source = list_malloc (strlen(source)+1);
  strcpy (c1->source, source);
  
  return c1;
}

//----------------------------------------------------------------------------
static void check_buffer (int bytes)  {
  if (!Buffer)  {
    Buffer = (char*) list_malloc (FRAME_SIZE);
    Pos = 0;
    Frame = 1;
/*    Pos_start = Pos = net_message_init(Buffer);  */
    Pos_start = 0;
    fill_int (0);
  }
  if (Pos > (Frame*FRAME_SIZE) - bytes)  {
    char* b = Buffer;
    Frame++;
    Buffer = (char*) list_malloc(Frame*FRAME_SIZE);
    memcpy (Buffer, b, Pos+1);
    free (b);
  }
}

//----------------------------------------------------------------------------
static void fetch_int (int* i)  {
  int* p = (int*) (Get_buffer + Get_pos);
  *i = *p;
#ifdef OSK
  *i = (*p & 0xFF)<<24 | (*p & 0xFF00)<<8 | 
       (*p & 0xFF0000)>>8 | (*p & 0xFF000000)>>24;  
#endif
  Get_pos += sizeof(int);
}

//----------------------------------------------------------------------------
static void fetch_text (char** c)   {
  int len;
  fetch_int (&len);
  *c = Get_buffer + Get_pos;
  Get_pos += len + 1;
}

//----------------------------------------------------------------------------
static void fill_int (int i)    {
  check_buffer (sizeof(int));
  int* p = (int*) (Buffer + Pos);
#ifdef VAX
  *p = i;
#endif
#ifdef OSK
  *p = (i & 0xFF)<<24 | (i & 0xFF00)<<8 | 
       (i & 0xFF0000)>>8 | (i & 0xFF000000)>>24;  
#endif
  Pos += sizeof(int);
}

//----------------------------------------------------------------------------
static void fill_double (double x)  {
  check_buffer (sizeof(int));
  double * p = (double*) (Buffer + Pos);
  *p = x;
  Pos += sizeof(double);
}

//----------------------------------------------------------------------------
static void fill_text (const char* c) {
  int len = strlen(c);
  check_buffer (4+len+1);
  fill_int (len);
  strcpy (Buffer + Pos, c);
  Pos += len + 1;
}

//----------------------------------------------------------------------------
static void fill_list (int type, const char** list, int size)   {
  int i;  
  switch (type)  {
    case ASC_FMT :
      for (i=0; i<size; i++) fill_text(list[i]);
      break;
    case DEC_FMT :
    case HEX_FMT :
    case OCT_FMT :
    case BIN_FMT :
    case LOG_FMT :
      {
        check_buffer (size * sizeof(int));
        int *p = (int*) (Buffer + Pos);
        for (i=0; i<size; i++,p++)        {
          int q = *(int*)list[i];
#ifdef VAX
          *p = q;
#endif
#ifdef OSK
          *p = (q & 0xFF)<<24    | (q & 0xFF00)<<8 | 
               (q & 0xFF0000)>>8 | (q & 0xFF000000)>>24;  
#endif
        }
        Pos += size * sizeof(int);
      }
      break;
    case REAL_FMT :   {
        double *d, *the_list = (double*)list;
        check_buffer (size * sizeof(double));
        d = (double*) (Buffer + Pos);
        for (i=0; i<size; i++,d++,the_list++) *d = *(double*) the_list;
        Pos += size * sizeof(double);
      }
      break;
  }
}

//----------------------------------------------------------------------------
static void fill_param (Param* p, int pos_i)    {
  check_buffer (sizeof(Param));
  int pos_pp = Pos;
  Param* pp = (Param*) (Buffer + pos_pp);
  *pp = *p;
  Pos += sizeof(Param);  
  fill_text (p->buf);
  if (p->list_size) fill_list (p->type, (const char**)p->list, p->list_size);
  pp = (Param*) (Buffer + pos_pp);
  if (pos_i)  {
    Item* i = (Item*) (Buffer + pos_i);
    listr_connect_entry (pp, &i->param);
  }
}

//----------------------------------------------------------------------------
static void fill_item (Item* i, int pos_d)    {
  check_buffer (sizeof(Item));
  int pos_ii = Pos;
  Item* ii = (Item*) (Buffer + pos_ii);
  *ii = *i;
  Pos += sizeof(Item);
  
  listr_init (&ii->param);
  fill_text (i->string);
  fill_text (i->help);
  for( Param* p = i->param.first; p; p = p->next)  {
    fill_param (p, pos_ii);
  }
  ii = (Item*) (Buffer + pos_ii);
  if (pos_d)  {
    Page* d = (Page*) (Buffer + pos_d);
    listr_connect_entry (ii, &d->item);
  }
}

//----------------------------------------------------------------------------
static void fill_menu (Menu* m)  {
  check_buffer (sizeof(Menu));
  int pos_mm = Pos;
  Menu* mm = (Menu*) (Buffer + pos_mm);

  *mm = *m;
  Pos += sizeof(Menu);
  listr_init (&mm->page);
  fill_text (m->mn_title);
  fill_text (m->up_title);
  fill_text (m->bt_title);
  
  check_buffer (sizeof(Page));
  int pos_dd = Pos;
  Page* dd = (Page*) (Buffer + pos_dd);
  *dd = *(m->page.first);
  Pos += sizeof(Page);
  listr_init (&dd->item);
  for(Page* d = m->page.first; d; d=d->next)  {
    for(Item* i = d->item.first; i; i=i->next)  {
      if (i->id != -1) fill_item(i, pos_dd);
    }
  }
  dd = (Page*) (Buffer + pos_dd);
  mm = (Menu*) (Buffer + pos_mm);
  listr_connect_entry (dd, &mm->page);
}

//----------------------------------------------------------------------------
static void fill_buffer (const void* buffer, int length)  {
  check_buffer (length);
  memcpy (Buffer + Pos, buffer, length);
  Pos += length;
}

//----------------------------------------------------------------------------
static int handler (unsigned int fac, void*)    {
  size_t bytes;
  int    length;
  char*  source;
  Connect* c;
  Menu* m;
  Page* d;
  Item* i;
  Routine action;
  int wakeup = 0;
  Queue* q = (Queue*) list_add_entry (&Header.queue.first, sizeof(Queue));
  int status = upic_net_read (&Get_buffer, &bytes, &source);
  if (source)  {
    c = find_connect (source);
    free (source);
  }
  else c = (Connect*) 0;
  
  q->connect = c;
  if (Get_buffer)  {
    if (bytes)  {
      Get_pos = 0;
      fetch_int (&length);
      fetch_int (&q->code);
      switch (q->code)    {
        case UPIF_RECONNECT :
          if ((!c->prev) && !(c->next)) upic_net_set_server_name (c->source);
          Hidden = 0;
          fill_int (UPIF_INIT_REMOTE);
          send (c->source);
          m = Sys.menu.first;
          while (m)   {
            if (m->id > 0) upir_close_menu (m, c);
            m = m->next;
          }
          list_remove_entry (q);
          q = (Queue*) 0;
          wakeup = 1;
          break;
        case UPIF_DISCONNECT_PROCESS :
          fill_int (UPIF_QUIT);
          send (c->source);
          if (c)    {
            if (c->source) free (c->source);
            list_remove_entry (c);
          }
          list_remove_entry (q);
          q = (Queue*) 0;
          wakeup = 1;
          break;
        case UPIF_INPUT :
          {
            int menu_id;
            int item_id;
            int param_id;
            int list_index;
            
            fetch_int (&q->b.get_input.menu_id);
            fetch_int (&q->b.get_input.item_id);
            fetch_int (&q->b.get_input.param_id);
            fetch_int (&q->b.get_input.list_index);

            menu_id    = q->b.get_input.menu_id;
            item_id    = q->b.get_input.item_id;
            param_id   = q->b.get_input.param_id;
            list_index = q->b.get_input.list_index;

            upic_retreive_vars (menu_id, item_id,
              Get_buffer + Get_pos);

            m = upic_find_menu (menu_id);
            if (!m)    {
              list_remove_entry (q);
              q = 0;
              break;
            }
            d = m->page.first;
            if (!d || !(i = upic_find_item (d->item.first, item_id)))  {
              list_remove_entry (q);
              q = 0;
              break;
            }

            if (i && (action = i->action))  {
              (*action) (menu_id, item_id, param_id, list_index);
              list_remove_entry (q);
              q = 0;
              break;
            }
          }
          break;
        case UPIF_GET_MESSAGE_WINDOW :
          fetch_int (&q->b.get_message_window.message_window_rows);
          fetch_int (&q->b.get_message_window.message_window_cols);
          fetch_int (&q->b.get_message_window.message_window_row);
          fetch_int (&q->b.get_message_window.message_window_col);
          wakeup = 1;
          q = 0;
          break;
        case UPIF_GET_WINDOW_POSITION :
          fetch_int (&q->b.get_window_position.window_position_col);
          fetch_int (&q->b.get_window_position.window_position_row);
          wakeup = 1;
          q = 0;
          break;
        case UPIF_GET_WINDOW_SIZE :
          fetch_int (&q->b.get_window_size.window_size_col);
          fetch_int (&q->b.get_window_size.window_size_row);
          wakeup = 1;
          q = 0;
          break;
        case UPIF_GET_ITEMS_PER_PAGE :
          fetch_int (&q->b.get_items_per_page.items_per_page);
          break;
        case UPIF_GET_MODE :
          fetch_int (&q->b.get_mode.mode);
          break;
        case UPIF_DLDEC :
          fetch_int (&q->b.dldec.dldec_value);
          break;
        case UPIF_DLTXT :
          {
            char* t;
            fetch_text (&t);
            q->b.dltxt.dltxt_value = (char*) list_malloc (strlen(t) + 1);
            strcpy (q->b.dltxt.dltxt_value, t);
          }
          break;
        case UPIF_SHOW_WARNING :
          break;
        case UPIF_MOVING_HISTO :
          {
            int row, col;
            fetch_int (&row);
            fetch_int (&col);
            upic_drag_histo (0, row, col);
            list_remove_entry (q);
            q = 0;
          }
          break;
        case UPIF_DECLARE_CALLBACK :
          {
            int condition, menu_id, item_id;
            fetch_int (&menu_id);
            fetch_int (&item_id);
            fetch_int (&condition);
            m = upic_find_menu (menu_id);
            if (m && (m->condition & condition))  {
              if (m->callback)(*m->callback)(menu_id, item_id, condition, m->arg);
            }
            list_remove_entry (q);
            q = 0;
            break;
          }
      }
      free (Get_buffer);
    }
  }
  if (wakeup) wtc_insert_head (WT_FACILITY_WAKEUP,0);
  if (q) wtc_insert_head (WT_FACILITY_UPI,0);

  return 1;
}

//----------------------------------------------------------------------------
static int broadcast (unsigned int fac, void*)  {
  char* source;
  int status = upic_net_spy (&source);
  if (source)  {
    Connect* c = find_connect (source);
    free (source);
    if (c)  {
      if (c->source) free (c->source);
      list_remove_entry (c);
      for(Queue* q = Header.queue.first; q; q = q->next)  {
        if (q->connect == c) q->connect = (Connect*) 0;
      }
    }
  }
  return WT_SUCCESS;
}

//----------------------------------------------------------------------------
static int rearm (unsigned int fac, void*)   {
  if (Header.queue.first) return 1;
  upic_end_update();
  fill_int (UPIF_INPUT);
  send (0);
  return WT_SUCCESS;
}

//----------------------------------------------------------------------------
void upir_init_remote ()    {
  const char* source;
  wt_callback_t r, a;
  
  list_init (&Header.connect);
  wtc_init();
  if ( wtc_get_routines (WT_FACILITY_UPI, &r, &a) != WT_SUCCESS ) a = 0;
  wtc_subscribe (WT_FACILITY_UPI, rearm, (WtRoutine)a);
  upic_net_init (0, &source, handler, broadcast);
  if (source)  {
    find_connect (source);
    free ((void*)source);
  }
  fill_int (UPIF_INIT_REMOTE);
  send (0);
}

//----------------------------------------------------------------------------
static Queue* wait (int code)   {
  void* dummy;
  unsigned int event;
  int status, sub_status;
  for (Queue* q = Header.queue.first; ;q = Header.queue.first)  {
    for(; q; q=q->next )    {
      if (q->code == code) return q;
    }
    status = wtc_wait (&event, &dummy, &sub_status);
  }
}

//----------------------------------------------------------------------------
int upir_input (int* menu_id, int* item_id, int* param_id, int* list_index) {
  Queue* q = wait (UPIF_INPUT);
  if (q)  {
    *menu_id = q->b.get_input.menu_id;
    *item_id = q->b.get_input.item_id;
    *param_id = q->b.get_input.param_id;
    *list_index = q->b.get_input.list_index;
    list_remove_entry (q);
    return (1);
  }
  return (0);
}

//----------------------------------------------------------------------------
void upir_delete_command (int menu_id, int item_id)   {
  fill_int (UPIF_DELETE_COMMAND);
  fill_int (menu_id);
  fill_int (item_id);
  send (0);
}

//----------------------------------------------------------------------------
void upir_enable_command (int menu_id, int item_id) {
  fill_int (UPIF_ENABLE_COMMAND);
  fill_int (menu_id);
  fill_int (item_id);
  send (0);
}

//----------------------------------------------------------------------------
void upir_disable_command (int menu_id, int item_id)  {
  fill_int (UPIF_DISABLE_COMMAND);
  fill_int (menu_id);
  fill_int (item_id);
  send (0);
}

//----------------------------------------------------------------------------
void upir_replace_item (Menu* m, Item* i) {
  fill_int (UPIF_REPLACE_ITEM);
  fill_int (m->id);
  fill_item (i, 0);
  send (0);
}

//----------------------------------------------------------------------------
void upir_insert_item (Menu* m, Item* i)  {
  fill_int (UPIF_INSERT_ITEM);
  fill_int (m->id);
  Item* next = i->next;
  if (next) fill_int (next->id);
  else fill_int (-1);
  fill_item (i, 0);
  send (0);
}

//----------------------------------------------------------------------------
void upir_close_menu (Menu* m, Connect* c)  {
  fill_int (UPIF_CLOSE_MENU);
  fill_menu (m);
  if (c) send (c->source);
  else send (0);
}

//----------------------------------------------------------------------------
void upir_delete_menu (int menu_id) {
  fill_int (UPIF_DELETE_MENU);
  fill_int (menu_id);
  send (0);
}

//----------------------------------------------------------------------------
void upir_erase_menu (int menu_id)  {
  fill_int (UPIF_ERASE_MENU);
  fill_int (menu_id);
  send (0);
}

//----------------------------------------------------------------------------
void upir_write_message (const char* text1, const char* text2)  {
  fill_int (UPIF_WRITE_MESSAGE);
  fill_text (text1);
  fill_text (text2);
  send_always (0, 0);
}

//----------------------------------------------------------------------------
void upir_set_message_window (int rows, int cols, int row, int col) {
  fill_int (UPIF_SET_MESSAGE_WINDOW);
  fill_int (rows);
  fill_int (cols);
  fill_int (row);
  fill_int (col);
  send (0);
}

//----------------------------------------------------------------------------
int upir_get_message_window (int* rows, int* cols, int* row, int* col)  {
  fill_int (UPIF_GET_MESSAGE_WINDOW);
  send (0);
  Queue* q = wait (UPIF_GET_MESSAGE_WINDOW);
  if (q)  {
    *rows = q->b.get_message_window.message_window_rows;
    *cols = q->b.get_message_window.message_window_cols;
    *row  = q->b.get_message_window.message_window_row;
    *col  = q->b.get_message_window.message_window_col;
    list_remove_entry (q);
    return (1);
  }
  return (0);
}

//----------------------------------------------------------------------------
void upir_change_titles (Menu* m) {
  fill_int (UPIF_CHANGE_TITLES);
  fill_int (m->id);
  fill_text (m->up_title);
  fill_text (m->bt_title);
  fill_text (m->mn_title);
  send (0);
}

//----------------------------------------------------------------------------
void upir_modify_param (Menu* m, Item* i, Param* p) {
  fill_int (UPIF_MODIFY_PARAM);
  fill_int (m->id);
  fill_int (i->id);
  fill_param (p, 0);
  send (0);
}

//----------------------------------------------------------------------------
void upir_refresh_param_page (Menu* m)  {
  char* buffer;
  int length;
  
  fill_int (UPIF_REFRESH_PARAM_PAGE);
  fill_int (m->id);
  upic_store_vars (m->id, 0, &buffer, &length);
  fill_buffer (buffer, length);
  free (buffer);
  send (0);
}

//----------------------------------------------------------------------------
void upir_refresh_param_line (Menu* m, Item* i) {
  char* buffer;
  int length;
  
  fill_int (UPIF_REFRESH_PARAM_LINE);
  fill_int (m->id);
  fill_int (i->id);
  upic_store_vars (m->id, i->id, &buffer, &length);
  fill_buffer (buffer, length);
  free (buffer);
  send (0);
}

//----------------------------------------------------------------------------
void upir_set_cursor (int menu_id, int item_id, int param_id) {
  fill_int (UPIF_SET_CURSOR);
  fill_int (menu_id);
  fill_int (item_id);
  fill_int (param_id);
  send (0);
}

//----------------------------------------------------------------------------
void upir_save_screen ()  {
/*  upic_net_open_mailboxes ();
  upic_net_get_mbx_names (Server_name, My_name); */
  fill_int (UPIF_SAVE_SCREEN);
  fill_text (My_name); 
  send (0);
}

//----------------------------------------------------------------------------
void upir_restore_screen ()    {
/*  upic_net_close_mailboxes (); */
  fill_int (UPIF_RESTORE_SCREEN);
  fill_text (My_name);
  send (0);
}

//----------------------------------------------------------------------------
void upir_begin_update () {
  Updating = 1;
}

//----------------------------------------------------------------------------
void upir_end_update () {
  Updating = 0;
  fill_int (UPIF_END_UPDATE);
  send (0);
}

//----------------------------------------------------------------------------
void upir_hide_menu (int menu)  {
  fill_int (UPIF_HIDE_MENU);
  fill_int (menu);
  send (0);
}

//----------------------------------------------------------------------------
void upir_quit () {
  fill_int (UPIF_QUIT);
  send (0);
  upic_net_close();
}

//----------------------------------------------------------------------------
void upir_open_window ()    {
  fill_int (UPIF_OPEN_WINDOW);
  send (0);
}

//----------------------------------------------------------------------------
void upir_open_old_window (int menu_id) {
  fill_int (UPIF_OPEN_OLD_WINDOW);
  send (0);
}

//----------------------------------------------------------------------------
void upir_set_window_position (int menu_id, int col, int row) {
  fill_int (UPIF_SET_WINDOW_POSITION);
  fill_int (menu_id);
  fill_int (col);
  fill_int (row);
  send (0);
}

//----------------------------------------------------------------------------
int upir_get_window_position (int menu_id, int* col, int* row)  {
  fill_int (UPIF_GET_WINDOW_POSITION);
  fill_int (menu_id);
  send (0);
  Queue* q = wait (UPIF_GET_WINDOW_POSITION);
  if (q)  {
    *col  = q->b.get_window_position.window_position_col;
    *row  = q->b.get_window_position.window_position_row;
    list_remove_entry (q);
    return (1);
  }
  return (0);
}

/*--------------------------------------------------------------------------*/
int upir_get_window_size (int menu_id, int* col, int* row)    {
  fill_int (UPIF_GET_WINDOW_SIZE);
  fill_int (menu_id);
  send (0);
  Queue* q = wait (UPIF_GET_WINDOW_SIZE);
  if (q)  {
    *col  = q->b.get_window_size.window_size_col;
    *row  = q->b.get_window_size.window_size_row;
    list_remove_entry (q);
    return (1);
  }
  return (0);
}

//----------------------------------------------------------------------------
void upir_attach_pf1 (int menu_id)    {
  fill_int (UPIF_ATTACH_PF1);  
  fill_int (menu_id);
  send (0);
}

//----------------------------------------------------------------------------
void upir_cancel_notice ()  {
  fill_int (UPIF_CANCEL_NOTICE);  
  send (0);
}

//----------------------------------------------------------------------------
int upir_get_items_per_page (int* num)  {
  fill_int (UPIF_GET_ITEMS_PER_PAGE);  
  send (0);
  Queue* q = wait (UPIF_GET_ITEMS_PER_PAGE);
  if (q)  {
    *num  = q->b.get_items_per_page.items_per_page;
    list_remove_entry (q);
    return (1);
  }
  return (0);
}

//----------------------------------------------------------------------------
void upir_set_items_per_page (int num)    {
  fill_int (UPIF_SET_ITEMS_PER_PAGE);  
  fill_int (num);
  send (0);
}

//----------------------------------------------------------------------------
int upir_get_mode (int* mode) {
  fill_int (UPIF_GET_MODE);  
  send (0);
  Queue* q = wait (UPIF_GET_MODE);
  if (q)  {
    *mode  = q->b.get_mode.mode;
    list_remove_entry (q);
    return (1);
  }
  return (0);
}

//----------------------------------------------------------------------------
void upir_set_mode (int mode)   {
  fill_int (UPIF_SET_MODE);  
  fill_int (mode);
  send (0);
}

//----------------------------------------------------------------------------
void upir_register_on_keypad (int menu, int item, int key)  {
  fill_int (UPIF_REGISTER_ON_KEYPAD);  
  fill_int (menu);
  fill_int (item);
  fill_int (key);
  send (0);
}

//----------------------------------------------------------------------------
void upir_show_notice (const char* title, int lines, const char**text)    {
  fill_int (UPIF_SHOW_NOTICE);  
  fill_text (title);
  fill_int (lines);
  fill_list (ASC_FMT, text, lines);
  send (0);
}

//----------------------------------------------------------------------------
int upir_show_warning (int lines, const char** text)    {
  fill_int (UPIF_SHOW_WARNING);
  fill_int (lines);
  fill_list (ASC_FMT, text, lines);
  send (0);
  Queue* q = wait (UPIF_SHOW_WARNING);
  if (q)  {
    list_remove_entry (q);
    return (1);
  }
  return (0);
}

//----------------------------------------------------------------------------
void upir_connect_process (const char* name)  {
  fill_int (UPIF_CONNECT_PROCESS);
  fill_text (name);
  send (0);
}


//----------------------------------------------------------------------------
void upir_disconnect_process (const char* name) {
  if (name)  {
    fill_int (UPIF_DISCONNECT_PROCESS);
    fill_text (name);
    send (0);
  }
  else  {
    fill_int (UPIF_QUIT);
    send (0);
    Hidden = 1;
/*
    Connect* c = Header.connect.first;
    if (!c) upic_net_discard_server ();
    while (c)
    {
      if (c->source) free (c->source);
      list_remove_entry (c);
      c = c->next;
    }
*/
  }
}

//----------------------------------------------------------------------------
int upir_dldec (const char* prompt, int def, int* value, int min, int max)  {
  fill_int (UPIF_DLDEC);
  fill_text (prompt);
  fill_int (def);
  fill_int (min);
  fill_int (max);
  send (0);
  Queue* q = wait (UPIF_DLDEC);
  if (q)  {
    *value = q->b.dldec.dldec_value;
    list_remove_entry (q);
    return (UPI_SS_NORMAL);
  }
  return (UPI_SS_ABORTED);
}

//----------------------------------------------------------------------------
void upir_dlend ()  {
  fill_int (UPIF_DLEND);
  send (0);
}

//----------------------------------------------------------------------------
int upir_dlhex (const char* prompt, int def, int* value, int min, int max)  {
  fill_int (UPIF_DLHEX);
  fill_text (prompt);
  fill_int (def);
  fill_int (min);
  fill_int (max);
  send (0);
  Queue* q = wait (UPIF_DLDEC);
  if (q)  {
    *value = q->b.dldec.dldec_value;
    list_remove_entry (q);
    return (UPI_SS_NORMAL);
  }
  return (UPI_SS_ABORTED);
}

//----------------------------------------------------------------------------
void upir_dlhead (const char* title, int lines, int cols)   {
  fill_int (UPIF_DLHEAD);
  fill_text (title);
  fill_int (lines);
  fill_int (cols);
  send (0);
}

//----------------------------------------------------------------------------
int upir_dlkey ()   {
  fill_int (UPIF_DLKEY);
  send (0);
  Queue* q = wait (UPIF_DLDEC);
  if (q)  {
    list_remove_entry (q);
    return (UPI_SS_NORMAL);
  }
  return (UPI_SS_ABORTED);
}

//----------------------------------------------------------------------------
int upir_dlm32 (const char* prompt, int def, int* value)  {
  fill_int (UPIF_DLM32);
  fill_text (prompt);
  fill_int (def);
  send (0);
  Queue* q = wait (UPIF_DLDEC);
  if (q)  {
    *value = q->b.dldec.dldec_value;
    list_remove_entry (q);
    return (UPI_SS_NORMAL);
  }
  return (UPI_SS_ABORTED);
}

//----------------------------------------------------------------------------
int upir_dlmask (const char* prompt, int def, int* value) {
  fill_int (UPIF_DLMASK);
  fill_text (prompt);
  fill_int (def);
  send (0);
  Queue* q = wait (UPIF_DLDEC);
  if (q)  {
    *value = q->b.dldec.dldec_value;
    list_remove_entry (q);
    return (UPI_SS_NORMAL);
  }
  return (UPI_SS_ABORTED);
}

//----------------------------------------------------------------------------
int upir_dloct (const char* prompt, int def, int* value, int min, int max)  {
  fill_int (UPIF_DLOCT);
  fill_text (prompt);
  fill_int (def);
  fill_int (min);
  fill_int (max);
  send (0);
  Queue* q = wait (UPIF_DLDEC);
  if (q)  {
    *value = q->b.dldec.dldec_value;
    list_remove_entry (q);
    return (UPI_SS_NORMAL);
  }
  return (UPI_SS_ABORTED);
}

//----------------------------------------------------------------------------
int upir_dltxt (const char* prompt, const char* def, char* value, 
                int length, int* ret_len) {
  fill_int (UPIF_DLTXT);
  fill_text (prompt);
  fill_text (def);
  fill_int (length);
  send (0);

  Queue* q = wait (UPIF_DLTXT);
  if (q)  {
    if (q->b.dltxt.dltxt_value)    {
      strcpy (value, q->b.dltxt.dltxt_value);
      free (q->b.dltxt.dltxt_value);
    }
    list_remove_entry (q);
    return (UPI_SS_NORMAL);
  }
  return (UPI_SS_ABORTED);
}

//----------------------------------------------------------------------------
void upir_dlout (const char* text, int var1, int var2, int nvar)  {
  fill_int (UPIF_DLOUT);
  fill_text (text);
  fill_int (var1);
  fill_int (var2);
  fill_int (nvar);
  send (0);
}

//----------------------------------------------------------------------------
void upir_dlouto (const char* text, int var1, int var2, int nvar)  {
  fill_int (UPIF_DLOUTO);
  fill_text (text);
  fill_int (var1);
  fill_int (var2);
  fill_int (nvar);
  send (0);
}

//----------------------------------------------------------------------------
void upir_dloutx (const char* text, int var1, int var2, int nvar) {
  fill_int (UPIF_DLOUTX);
  fill_text (text);
  fill_int (var1);
  fill_int (var2);
  fill_int (nvar);
  send (0);
}

//----------------------------------------------------------------------------
int upir_dlyeno (const char* prompt, int def, int* value)   {
  fill_int (UPIF_DLYENO);
  fill_text (prompt);
  fill_int (def);
  send (0);
  Queue* q = wait (UPIF_DLDEC);
  if (q)  {
    *value = q->b.dldec.dldec_value;
    list_remove_entry (q);
    return (UPI_SS_NORMAL);
  }
  return (UPI_SS_ABORTED);
}

//----------------------------------------------------------------------------
void upir_lock_cursor ()  {
  fill_int (UPIF_LOCK_CURSOR);
  send (0);
}

//----------------------------------------------------------------------------
void upir_unlock_cursor ()    {
  fill_int (UPIF_UNLOCK_CURSOR);
  send (0);
}

//----------------------------------------------------------------------------
void upir_back_space (int menu_id)  {
  fill_int (UPIF_BACK_SPACE);
  fill_int (menu_id);
  send (0);
}

//----------------------------------------------------------------------------
void upir_book_histo (Histo* h)   {
  fill_int (UPIF_BOOK_HISTO);
  fill_int    (int(h));
  fill_text   (h->text);
  fill_int    (h->bins);
  fill_int    (h->rows);
  fill_double (h->min);
  fill_double (h->max);
  send (0);
}

//----------------------------------------------------------------------------
void upir_unbook_histo (Histo* h) {
  fill_int (UPIF_UNBOOK_HISTO);
  fill_int    (int(h));
  send (0);
}

//----------------------------------------------------------------------------
void upir_scale_histo (Histo* h, double maxw) {
  fill_int (UPIF_SCALE_HISTO);
  fill_int (int(h));
  fill_double (maxw);
  send (0);
}

//----------------------------------------------------------------------------
void upir_unscale_histo (Histo* h)    {
  fill_int (UPIF_UNSCALE_HISTO);
  fill_int (int(h));
  send (0);
}

//----------------------------------------------------------------------------
void upir_fill_histo (Histo* h, double x, double w) {
  fill_int (UPIF_FILL_HISTO);
  fill_int (int(h));
  fill_double (x);
  fill_double (w);
  send (0);
}

//----------------------------------------------------------------------------
void upir_display_histo (Histo* h, int row, int col)  {
  fill_int (UPIF_DISPLAY_HISTO);
  fill_int (int(h));
  fill_int (row);
  fill_int (col);
  send (0);
}

//----------------------------------------------------------------------------
void upir_hide_histo (Histo* h)   {
  fill_int (UPIF_HIDE_HISTO);
  fill_int (int(h));
  send (0);
}

//----------------------------------------------------------------------------
void upir_move_histo (Histo* h, int row, int col)  {
  fill_int (UPIF_MOVE_HISTO);
  fill_int (int(h));
  fill_int (row);
  fill_int (col);
  send (0);
}

//----------------------------------------------------------------------------
void upir_moving_histo (Histo* h) {
  fill_int (UPIF_MOVING_HISTO);
  fill_int (int(h));
  send (0);
}

//----------------------------------------------------------------------------
void fill_histo (Histo* h)    {
  check_buffer (sizeof(Histo));
  Histo* hh = (Histo*) (Buffer + Pos);
  *hh = *h;
  Pos += sizeof(Histo);
  fill_text (h->text);
  fill_buffer (h->first, h->bins * sizeof(double));
}
  
//----------------------------------------------------------------------------
void upir_copy_histo (Histo* h) {
  fill_int (UPIF_COPY_HISTO);
  fill_int (int(h));
  fill_histo (h);
  send (0);
}

//----------------------------------------------------------------------------
void upir_declare_callback (int menu_id, int condition)   {
  fill_int (UPIF_DECLARE_CALLBACK);
  fill_int (menu_id);
  fill_int (condition);
  send (0);
}
#endif
