/**
  *
  * upic_remote.c
  * Created           : 17-NOV-1989 by Christian Arnault
  */

#ifdef REMOTE

//----------------------------------------------------------------------------
#include "UPI/upidef.h"
#include "UPI/upirem.h"
#include "UPI/upi_buffer.h"
#include "AMS/amsdef.h"
#include "WT/wt_facilities.h"
#include "WT/wtdef.h"
#include "SCR/lists.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

// #define UpiBufferSetCommand(x,y) { printf("Setting command: " #y "\n"); UpiBufferPutInt(x,y); }
#define UpiBufferSetCommand UpiBufferPutInt

/*-------------------------------------------------------------------------*/
typedef struct QUEUE   Queue;

struct QUEUE {
  Queue Link_items;

  UpiConnect connect;

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
//--------------------------------------------------------------------------

static struct {
  struct {
    Queue Linked_list_items;
  } queue;
  int last_id;
} Header;


static UpiBuffer SendBuffer = 0;
static UpiBuffer GetBuffer = 0;

static char My_name[AMS_NAME_LENGTH] = "";

extern System Sys;

//--------------------------------------------------------------------------
static Queue* upi_int_wait (int code)   {
  void* dummy;
  unsigned int event;
  int status, sub_status;
  for (Queue* q = Header.queue.first; ;q = Header.queue.first)  {
    for(; q; q=q->next )    {
      if (q->code == code) return q;
    }
    status = wtc_wait (&event, &dummy, &sub_status);
    if ( status != WT_SUCCESS )  {
      return 0;
    }
  }
}

int upir_remove_entry(Queue* q)  {
  if (q)  {
    list_remove_entry ((Link*) q);
    return UPI_SS_NORMAL;
  }
  return UPI_SS_ABORTED;
}

//--------------------------------------------------------------------------
int upir_input (int* menu_id, int* item_id, int* param_id, int* list_index) {
  upir_end_update ();
  Queue* q = upi_int_wait (UPIF_INPUT);
  if (q)  {
    *menu_id    = q->b.get_input.menu_id;
    *item_id    = q->b.get_input.item_id;
    *param_id   = q->b.get_input.param_id;
    *list_index = q->b.get_input.list_index;
  }
  return upir_remove_entry(q);
}

//--------------------------------------------------------------------------
void upir_delete_command (int menu_id, int item_id)   {
  UpiBufferSetCommand (SendBuffer, UPIF_DELETE_COMMAND);
  UpiBufferPutInt (SendBuffer, menu_id);
  UpiBufferPutInt (SendBuffer, item_id);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_enable_command (int menu_id, int item_id)   {
  UpiBufferSetCommand (SendBuffer, UPIF_ENABLE_COMMAND);
  UpiBufferPutInt (SendBuffer, menu_id);
  UpiBufferPutInt (SendBuffer, item_id);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_disable_command (int menu_id, int item_id)    {
  UpiBufferSetCommand (SendBuffer, UPIF_DISABLE_COMMAND);
  UpiBufferPutInt (SendBuffer, menu_id);
  UpiBufferPutInt (SendBuffer, item_id);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_replace_item (Menu* m, Item* i) {
  UpiBufferSetCommand (SendBuffer, UPIF_REPLACE_ITEM);
  UpiBufferPutInt (SendBuffer, m->id);
  UpiBufferPutItem (SendBuffer, i);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_insert_item (Menu* m, Item* i)  {
  UpiBufferSetCommand (SendBuffer, UPIF_INSERT_ITEM);
  UpiBufferPutInt (SendBuffer, m->id);
  if (i->next) UpiBufferPutInt (SendBuffer, i->next->id);
  else UpiBufferPutInt (SendBuffer, -1);
  UpiBufferPutItem (SendBuffer, i);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_close_menu (Menu* m, UpiConnect connect)  {
  UpiBufferSetCommand (SendBuffer, UPIF_CLOSE_MENU);
  UpiBufferPutMenu (SendBuffer, m);
  if (connect) UpiBufferSendToName (SendBuffer, UpiConnectName(connect));
  else UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_delete_menu (int menu_id) {
  UpiBufferSetCommand (SendBuffer, UPIF_DELETE_MENU);
  UpiBufferPutInt (SendBuffer, menu_id);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_erase_menu (int menu_id)  {
  UpiBufferSetCommand (SendBuffer, UPIF_ERASE_MENU);
  UpiBufferPutInt (SendBuffer, menu_id);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_write_message (const char* text1, const char* text2)  {
  UpiBufferSetCommand (SendBuffer, UPIF_WRITE_MESSAGE);
  UpiBufferPutText (SendBuffer, text1);
  UpiBufferPutText (SendBuffer, text2);
  UpiBufferSendAlways (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_write_rendered_message (const char* text1, const char* text2, int render)  {
  UpiBufferSetCommand (SendBuffer, UPIF_WRITE_MESSAGE_RENDERED);
  UpiBufferPutText (SendBuffer, text1);
  UpiBufferPutText (SendBuffer, text2);
  UpiBufferPutInt  (SendBuffer, render);
  UpiBufferSendAlways (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_set_message_window (int rows, int cols, int row, int col) {
  UpiBufferSetCommand (SendBuffer, UPIF_SET_MESSAGE_WINDOW);
  UpiBufferPutInt (SendBuffer, rows);
  UpiBufferPutInt (SendBuffer, cols);
  UpiBufferPutInt (SendBuffer, row);
  UpiBufferPutInt (SendBuffer, col);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
int upir_get_message_window (int* rows, int* cols, int* row, int* col)  {
  UpiBufferSetCommand (SendBuffer, UPIF_GET_MESSAGE_WINDOW);
  UpiBufferSend (SendBuffer);
  Queue* q = upi_int_wait (UPIF_GET_MESSAGE_WINDOW);
  if (q)  {
    *rows = q->b.get_message_window.message_window_rows;
    *cols = q->b.get_message_window.message_window_cols;
    *row  = q->b.get_message_window.message_window_row;
    *col  = q->b.get_message_window.message_window_col;
  }
  return upir_remove_entry(q);
}

//--------------------------------------------------------------------------
void upir_change_titles (Menu* m) {
  UpiBufferSetCommand (SendBuffer, UPIF_CHANGE_TITLES);
  UpiBufferPutInt (SendBuffer, m->id);
  UpiBufferPutText (SendBuffer, m->up_title);
  UpiBufferPutText (SendBuffer, m->bt_title);
  UpiBufferPutText (SendBuffer, m->mn_title);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_modify_param (Menu* m, Item* i, Param* p) {
  UpiBufferSetCommand (SendBuffer, UPIF_MODIFY_PARAM);
  UpiBufferPutInt (SendBuffer, m->id);
  UpiBufferPutInt (SendBuffer, i->id);
  UpiBufferPutParam (SendBuffer, p);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_refresh_param_page (Menu* m)  {
  char* buffer;
  int length;
  UpiBufferSetCommand (SendBuffer, UPIF_REFRESH_PARAM_PAGE);
  UpiBufferPutInt (SendBuffer, m->id);
  upic_store_vars (m->id, 0, &buffer, &length);
  UpiBufferPutBytes (SendBuffer, buffer, length);
  free (buffer);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_refresh_param_line (Menu* m, Item* i) {
  char* buffer;
  int length;
  UpiBufferSetCommand (SendBuffer, UPIF_REFRESH_PARAM_LINE);
  UpiBufferPutInt (SendBuffer, m->id);
  UpiBufferPutInt (SendBuffer, i->id);
  upic_store_vars (m->id, i->id, &buffer, &length);
  UpiBufferPutBytes (SendBuffer, buffer, length);
  free (buffer);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_set_cursor (int menu_id, int item_id, int param_id)   {
  UpiBufferSetCommand (SendBuffer, UPIF_SET_CURSOR);
  UpiBufferPutInt (SendBuffer, menu_id);
  UpiBufferPutInt (SendBuffer, item_id);
  UpiBufferPutInt (SendBuffer, param_id);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_save_screen ()    {
  UpiBufferSetCommand (SendBuffer, UPIF_SAVE_SCREEN);
  UpiBufferPutText (SendBuffer, My_name);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_restore_screen ()   {
  UpiBufferSetCommand (SendBuffer, UPIF_RESTORE_SCREEN);
  UpiBufferPutText (SendBuffer, My_name);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_begin_update () {
  UpiBufferSetUpdating (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_end_update () {
  UpiBufferResetUpdating (SendBuffer);
  UpiBufferSetCommand (SendBuffer, UPIF_END_UPDATE);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_hide_menu (int menu)  {
  UpiBufferSetCommand (SendBuffer, UPIF_HIDE_MENU);
  UpiBufferPutInt (SendBuffer, menu);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_quit ()   {
  UpiBufferSetCommand (SendBuffer, UPIF_QUIT);
  UpiBufferSend (SendBuffer);
  upic_net_close();
}

//--------------------------------------------------------------------------
void upir_open_window ()    {
  UpiBufferSetCommand (SendBuffer, UPIF_OPEN_WINDOW);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_open_old_window (int /* menu_id */)   {
  UpiBufferSetCommand (SendBuffer, UPIF_OPEN_OLD_WINDOW);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_set_window_position (int menu_id, int col, int row)   {
  UpiBufferSetCommand (SendBuffer, UPIF_SET_WINDOW_POSITION);
  UpiBufferPutInt (SendBuffer, menu_id);
  UpiBufferPutInt (SendBuffer, col);
  UpiBufferPutInt (SendBuffer, row);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
int upir_get_window_position (int menu_id, int* col, int* row)    {
  UpiBufferSetCommand (SendBuffer, UPIF_GET_WINDOW_POSITION);
  UpiBufferPutInt (SendBuffer, menu_id);
  UpiBufferSend (SendBuffer);
  Queue* q = upi_int_wait (UPIF_GET_WINDOW_POSITION);
  if (q)  {
    *col  = q->b.get_window_position.window_position_col;
    *row  = q->b.get_window_position.window_position_row;
  }
  return upir_remove_entry(q);
}

//--------------------------------------------------------------------------
int upir_get_window_size (int menu_id, int* col, int* row)    {
  UpiBufferSetCommand (SendBuffer, UPIF_GET_WINDOW_SIZE);
  UpiBufferPutInt (SendBuffer, menu_id);
  UpiBufferSend (SendBuffer);
  Queue* q = upi_int_wait (UPIF_GET_WINDOW_SIZE);
  if (q)  {
    *col  = q->b.get_window_size.window_size_col;
    *row  = q->b.get_window_size.window_size_row;
  }
  return upir_remove_entry(q);
}

//--------------------------------------------------------------------------
void upir_attach_pf1 (int menu_id)  {
  UpiBufferSetCommand (SendBuffer, UPIF_ATTACH_PF1);  
  UpiBufferPutInt (SendBuffer, menu_id);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_cancel_notice ()    {
  UpiBufferSetCommand (SendBuffer, UPIF_CANCEL_NOTICE);  
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
int upir_get_items_per_page (int* num)    {
  UpiBufferSetCommand (SendBuffer, UPIF_GET_ITEMS_PER_PAGE);  
  UpiBufferSend (SendBuffer);
  Queue* q = upi_int_wait (UPIF_GET_ITEMS_PER_PAGE);
  if (q) *num  = q->b.get_items_per_page.items_per_page;
  return upir_remove_entry(q);
}

//--------------------------------------------------------------------------
void upir_set_items_per_page (int num)  {
  UpiBufferSetCommand (SendBuffer, UPIF_SET_ITEMS_PER_PAGE);  
  UpiBufferPutInt (SendBuffer, num);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
int upir_get_mode (int* mode) {
  UpiBufferSetCommand (SendBuffer, UPIF_GET_MODE);  
  UpiBufferSend (SendBuffer);
  Queue* q = upi_int_wait (UPIF_GET_MODE);
  if (q) *mode  = q->b.get_mode.mode;
  return upir_remove_entry(q);
}

//--------------------------------------------------------------------------
void upir_set_mode (int mode) {
  UpiBufferSetCommand (SendBuffer, UPIF_SET_MODE);  
  UpiBufferPutInt (SendBuffer, mode);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_register_on_keypad (int menu, int item, int key)  {
  UpiBufferSetCommand (SendBuffer, UPIF_REGISTER_ON_KEYPAD);  
  UpiBufferPutInt (SendBuffer, menu);
  UpiBufferPutInt (SendBuffer, item);
  UpiBufferPutInt (SendBuffer, key);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_show_notice (const char* title, int lines, const char** text)   {
  UpiBufferSetCommand (SendBuffer, UPIF_SHOW_NOTICE);  
  UpiBufferPutText (SendBuffer, title);
  UpiBufferPutInt (SendBuffer, lines);
  UpiBufferPutList (SendBuffer, UPI_ASC_FMT, text, lines);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
int upir_show_warning (int lines, const char** text)    {
  UpiBufferSetCommand (SendBuffer, UPIF_SHOW_WARNING);
  UpiBufferPutInt (SendBuffer, lines);
  UpiBufferPutList (SendBuffer, UPI_ASC_FMT, text, lines);
  UpiBufferSend (SendBuffer);
  Queue* q = upi_int_wait (UPIF_SHOW_WARNING);
  return upir_remove_entry(q);
}

//--------------------------------------------------------------------------
void upir_connect_process (const char* name)    {
  UpiBufferSetCommand (SendBuffer, UPIF_CONNECT_PROCESS);
  UpiBufferPutText(SendBuffer, name);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_disconnect_process (const char* name)   {
  if (name)  {
    UpiBufferSetCommand (SendBuffer, UPIF_DISCONNECT_PROCESS);
    UpiBufferPutText (SendBuffer, name);
    UpiBufferSend (SendBuffer);
  }
  else  {
    UpiBufferSetCommand (SendBuffer, UPIF_QUIT);
    UpiBufferSend (SendBuffer);
    UpiBufferSetNoSend (SendBuffer);
  }
}

//--------------------------------------------------------------------------
int upir_dldec (const char* prompt, int def, int* value, int min, int max)    {
  UpiBufferSetCommand (SendBuffer, UPIF_DLDEC);
  UpiBufferPutText (SendBuffer, prompt);
  UpiBufferPutInt (SendBuffer, def);
  UpiBufferPutInt (SendBuffer, min);
  UpiBufferPutInt (SendBuffer, max);
  UpiBufferSend (SendBuffer);
  Queue* q = upi_int_wait (UPIF_DLDEC);
  if (q) *value = q->b.dldec.dldec_value;
  return upir_remove_entry(q);
}

//--------------------------------------------------------------------------
void upir_dlend ()  {
  UpiBufferSetCommand (SendBuffer, UPIF_DLEND);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
int upir_dlhex (const char* prompt, int def, int* value, int min, int max)  {
  UpiBufferSetCommand (SendBuffer, UPIF_DLHEX);
  UpiBufferPutText (SendBuffer, prompt);
  UpiBufferPutInt (SendBuffer, def);
  UpiBufferPutInt (SendBuffer, min);
  UpiBufferPutInt (SendBuffer, max);
  UpiBufferSend (SendBuffer);
  Queue* q = upi_int_wait (UPIF_DLDEC);
  if (q) *value = q->b.dldec.dldec_value;
  return upir_remove_entry(q);
}

//--------------------------------------------------------------------------
void upir_dlhead (const char* title, int lines, int cols)  {
  UpiBufferSetCommand (SendBuffer, UPIF_DLHEAD);
  UpiBufferPutText (SendBuffer, title);
  UpiBufferPutInt (SendBuffer, lines);
  UpiBufferPutInt (SendBuffer, cols);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
int upir_dlkey ()   {
  UpiBufferSetCommand (SendBuffer, UPIF_DLKEY);
  UpiBufferSend (SendBuffer);
  Queue* q = upi_int_wait (UPIF_DLDEC);
  return upir_remove_entry(q);
}

//--------------------------------------------------------------------------
int upir_dlm32 (const char* prompt, int def, int* value)  {
  UpiBufferSetCommand (SendBuffer, UPIF_DLM32);
  UpiBufferPutText (SendBuffer, prompt);
  UpiBufferPutInt (SendBuffer, def);
  UpiBufferSend (SendBuffer);
  Queue* q = upi_int_wait (UPIF_DLDEC);
  if (q) *value = q->b.dldec.dldec_value;
  return upir_remove_entry(q);
}

//--------------------------------------------------------------------------
int upir_dlmask (const char* prompt, int def, int* value)   {
  UpiBufferSetCommand (SendBuffer, UPIF_DLMASK);
  UpiBufferPutText (SendBuffer, prompt);
  UpiBufferPutInt (SendBuffer, def);
  UpiBufferSend (SendBuffer);
  Queue* q = upi_int_wait (UPIF_DLDEC);
  if (q) *value = q->b.dldec.dldec_value;
  return upir_remove_entry(q);
}

//--------------------------------------------------------------------------
int upir_dloct (const char* prompt, int def, int* value, int min, int max)  {
  UpiBufferSetCommand (SendBuffer, UPIF_DLOCT);
  UpiBufferPutText (SendBuffer, prompt);
  UpiBufferPutInt (SendBuffer, def);
  UpiBufferPutInt (SendBuffer, min);
  UpiBufferPutInt (SendBuffer, max);
  UpiBufferSend (SendBuffer);
  Queue* q = upi_int_wait (UPIF_DLDEC);
  if (q) *value = q->b.dldec.dldec_value;
  return upir_remove_entry(q);
}

//--------------------------------------------------------------------------
int upir_dltxt (const char* prompt, const char* def, char* value, int length, int* /* ret_len */) {
  UpiBufferSetCommand (SendBuffer, UPIF_DLTXT);
  UpiBufferPutText (SendBuffer, prompt);
  UpiBufferPutText (SendBuffer, def);
  UpiBufferPutInt (SendBuffer, length);
  UpiBufferSend (SendBuffer);
  Queue* q = upi_int_wait (UPIF_DLTXT);
  if (q)  {
    if (q->b.dltxt.dltxt_value)    {
      strcpy (value, q->b.dltxt.dltxt_value);
      free (q->b.dltxt.dltxt_value);
    }
  }
  return upir_remove_entry(q);
}

//--------------------------------------------------------------------------
void upir_dlout (const char* text, int var1, int var2, int nvar)  {
  UpiBufferSetCommand (SendBuffer, UPIF_DLOUT);
  UpiBufferPutText (SendBuffer, text);
  UpiBufferPutInt (SendBuffer, var1);
  UpiBufferPutInt (SendBuffer, var2);
  UpiBufferPutInt (SendBuffer, nvar);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_dlouto (const char* text, int var1, int var2, int nvar) {
  UpiBufferSetCommand (SendBuffer, UPIF_DLOUTO);
  UpiBufferPutText (SendBuffer, text);
  UpiBufferPutInt (SendBuffer, var1);
  UpiBufferPutInt (SendBuffer, var2);
  UpiBufferPutInt (SendBuffer, nvar);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_dloutx (const char* text, int var1, int var2, int nvar) {
  UpiBufferSetCommand (SendBuffer, UPIF_DLOUTX);
  UpiBufferPutText (SendBuffer, text);
  UpiBufferPutInt (SendBuffer, var1);
  UpiBufferPutInt (SendBuffer, var2);
  UpiBufferPutInt (SendBuffer, nvar);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
int upir_dlyeno (const char* prompt, int def, int* value) {
  UpiBufferSetCommand (SendBuffer, UPIF_DLYENO);
  UpiBufferPutText (SendBuffer, prompt);
  UpiBufferPutInt (SendBuffer, def);
  UpiBufferSend (SendBuffer);
  Queue* q = upi_int_wait (UPIF_DLDEC);
  if (q) *value = q->b.dldec.dldec_value;
  return upir_remove_entry(q);
}

//--------------------------------------------------------------------------
void upir_lock_cursor ()  {
  UpiBufferSetCommand (SendBuffer, UPIF_LOCK_CURSOR);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_unlock_cursor ()  {
  UpiBufferSetCommand (SendBuffer, UPIF_UNLOCK_CURSOR);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_back_space(int menu_id)  {
  UpiBufferSetCommand (SendBuffer, UPIF_BACK_SPACE);
  UpiBufferPutInt (SendBuffer, menu_id);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_book_histo(Histo* h) {
  UpiBufferSetCommand(SendBuffer, UPIF_BOOK_HISTO);
  UpiBufferPutLong   (SendBuffer, (long)h);
  UpiBufferPutText   (SendBuffer, h->text);
  UpiBufferPutInt    (SendBuffer, h->bins);
  UpiBufferPutInt    (SendBuffer, h->rows);
  UpiBufferPutDouble (SendBuffer, h->min);
  UpiBufferPutDouble (SendBuffer, h->max);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_unbook_histo (Histo* h) {
  UpiBufferSetCommand (SendBuffer, UPIF_UNBOOK_HISTO);
  UpiBufferPutLong(SendBuffer, (long)h);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_scale_histo (Histo* h, double maxw)   {
  UpiBufferSetCommand (SendBuffer, UPIF_SCALE_HISTO);
  UpiBufferPutLong   (SendBuffer,  (long)h);
  UpiBufferPutDouble (SendBuffer,  maxw);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_unscale_histo (Histo* h)  {
  UpiBufferSetCommand (SendBuffer, UPIF_UNSCALE_HISTO);
  UpiBufferPutLong(SendBuffer, (long) h);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_fill_histo (Histo* h, double x, double w)   {
  UpiBufferSetCommand(SendBuffer, UPIF_FILL_HISTO);
  UpiBufferPutLong   (SendBuffer, (long) h);
  UpiBufferPutDouble (SendBuffer, x);
  UpiBufferPutDouble (SendBuffer, w);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_display_histo (Histo* h, int row, int col)  {
  UpiBufferSetCommand (SendBuffer, UPIF_DISPLAY_HISTO);
  UpiBufferPutLong(SendBuffer, (long) h);
  UpiBufferPutInt (SendBuffer, row);
  UpiBufferPutInt (SendBuffer, col);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_hide_histo (Histo* h) {
  UpiBufferSetCommand (SendBuffer, UPIF_HIDE_HISTO);
  UpiBufferPutLong(SendBuffer, (long) h);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_move_histo (Histo* h, int row, int col) {
  UpiBufferSetCommand (SendBuffer, UPIF_MOVE_HISTO);
  UpiBufferPutLong(SendBuffer, (long) h);
  UpiBufferPutInt (SendBuffer, row);
  UpiBufferPutInt (SendBuffer, col);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_moving_histo (Histo* h) {
  UpiBufferSetCommand (SendBuffer, UPIF_MOVING_HISTO);
  UpiBufferPutLong(SendBuffer, (long) h);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_copy_histo (Histo* h) {
  UpiBufferSetCommand (SendBuffer, UPIF_COPY_HISTO);
  UpiBufferPutLong(SendBuffer, (long) h);
  UpiBufferPutHisto (SendBuffer, h);
  UpiBufferSend (SendBuffer);
}

//--------------------------------------------------------------------------
void upir_declare_callback (int menu_id, int condition)  {
  UpiBufferSetCommand (SendBuffer, UPIF_DECLARE_CALLBACK);
  UpiBufferPutInt (SendBuffer, menu_id);
  UpiBufferPutInt (SendBuffer, condition);
  UpiBufferSend (SendBuffer);
}

int upir_build_format()       {  return UPI_SS_INVCOMMAND;    }
int upir_find_menu()          {  return UPI_SS_INVCOMMAND;    }
int upir_drop_param()         {  return UPI_SS_INVCOMMAND;    }
int upir_find_list_elem()     {  return UPI_SS_INVCOMMAND;    }
int upir_set_value_from_list(){  return UPI_SS_INVCOMMAND;    }
int upir_draw_cursor()        {  return UPI_SS_INVCOMMAND;    }
int upir_open_menus()         {  return UPI_SS_INVCOMMAND;    }
int upir_refresh_screen()     {  return UPI_SS_INVCOMMAND;    }
int upir_change_page()        {  return UPI_SS_INVCOMMAND;    }
int upir_init_item_strings()  {  return UPI_SS_INVCOMMAND;    }
int upir_find_menu_on_window(){  return UPI_SS_INVCOMMAND;    }
int upir_connect_menu()       {  return UPI_SS_INVCOMMAND;    }
int upir_draw_item()          {  return UPI_SS_INVCOMMAND;    }
int upic_change_page()        {  return UPI_SS_INVCOMMAND;    }
int upic_draw_cursor()        {  return UPI_SS_INVCOMMAND;    }
int upic_draw_item()          {  return UPI_SS_INVCOMMAND;    }
int upic_find_menu_on_window(){  return UPI_SS_INVCOMMAND;    }
int upic_refresh_screen()     {  return UPI_SS_INVCOMMAND;    }

//--------------------------------------------------------------------------
static int handler (unsigned int /* fac */ )  {
  size_t bytes = 0;
  char* source = 0;
  UpiConnect connect = 0;
  Menu* m;
  Page* d;
  Item* i;
  Routine action;
  int wakeup = 0;
  char* buffer = 0;
  int row, col, condition, menu_id, item_id, param_id, list_index;

  Queue* q = (Queue*) list_add_entry ((Linked_list*) &Header.queue.first, sizeof(Queue));
  /* int status = */ upic_net_read (&buffer, &bytes, &source);
  if (source)    {
    connect = UpiConnectNew (source);
    free (source);
  }
  q->connect = connect;
  UpiBufferInit (GetBuffer, buffer, bytes);

  if (UpiBufferCheckProtocol (GetBuffer) == UpiBufferOk)  {
    UpiBufferGetInt (GetBuffer, &q->code);
    switch (q->code)    {
    case UPIF_RECONNECT :
      if (UpiConnectNumber () == 1)
        upic_net_set_server_name (UpiConnectName(connect));
      UpiBufferResetNoSend (SendBuffer);
      UpiBufferPutInt (SendBuffer, UPIF_INIT_REMOTE);
      UpiBufferSendToName (SendBuffer, UpiConnectName(connect));
      m = Sys.menu.first;
      while (m)  {
        if (m->id > 0) upir_close_menu (m, connect);
        m = m->next;
      }
      list_remove_entry ((Link*) q);
      q = 0;
      wakeup = 1;
      break;
    case UPIF_DISCONNECT_PROCESS :
      UpiBufferPutInt (SendBuffer, UPIF_QUIT);
      UpiBufferSendToName (SendBuffer, UpiConnectName(connect));
      UpiConnectDelete (connect);
      list_remove_entry ((Link*) q);
      q = 0;
      wakeup = 1;
      break;
    case UPIF_INPUT :
      UpiBufferGetInt (GetBuffer, &q->b.get_input.menu_id);
      UpiBufferGetInt (GetBuffer, &q->b.get_input.item_id);
      UpiBufferGetInt (GetBuffer, &q->b.get_input.param_id);
      UpiBufferGetInt (GetBuffer, &q->b.get_input.list_index);
      UpiBufferGetBytes (GetBuffer, &buffer, 0);

      menu_id    = q->b.get_input.menu_id;
      item_id    = q->b.get_input.item_id;
      param_id   = q->b.get_input.param_id;
      list_index = q->b.get_input.list_index;
      upic_retreive_vars (menu_id, item_id, buffer);
      m = upic_find_menu (menu_id);
      if (!m)     {
        list_remove_entry ((Link*) q);
        q = 0;
        break;
      }
      d = m->page.first;
      if (!d || !(i = upic_find_item (d->item.first, item_id)))   {
        list_remove_entry ((Link*) q);
        q = 0;
        break;
      }
      if (i && (action = i->action))        {
        (*action) (menu_id, item_id, param_id, list_index);
        list_remove_entry ((Link*) q);
        q = 0;
        break;
      }
      break;
    case UPIF_GET_MESSAGE_WINDOW :
      UpiBufferGetInt (GetBuffer, &q->b.get_message_window.message_window_rows);
      UpiBufferGetInt (GetBuffer, &q->b.get_message_window.message_window_cols);
      UpiBufferGetInt (GetBuffer, &q->b.get_message_window.message_window_row);
      UpiBufferGetInt (GetBuffer, &q->b.get_message_window.message_window_col);
      q = 0;
      wakeup = 1;
      break;
    case UPIF_GET_WINDOW_POSITION :
      UpiBufferGetInt (GetBuffer, &q->b.get_window_position.window_position_col);
      UpiBufferGetInt (GetBuffer, &q->b.get_window_position.window_position_row);
      q = 0;
      wakeup = 1;
      break;
    case UPIF_GET_WINDOW_SIZE :
      UpiBufferGetInt (GetBuffer, &q->b.get_window_size.window_size_col);
      UpiBufferGetInt (GetBuffer, &q->b.get_window_size.window_size_row);
      q = 0;
      wakeup = 1;
      break;
    case UPIF_GET_ITEMS_PER_PAGE :
      UpiBufferGetInt (GetBuffer, &q->b.get_items_per_page.items_per_page);
      break;
    case UPIF_GET_MODE :
      UpiBufferGetInt (GetBuffer, &q->b.get_mode.mode);
      break;
    case UPIF_DLDEC :
      UpiBufferGetInt (GetBuffer, &q->b.dldec.dldec_value);
      break;
    case UPIF_DLTXT :
      UpiBufferGetText (GetBuffer, &buffer);
      q->b.dltxt.dltxt_value = (char*) malloc (strlen(buffer) + 1);
      strcpy (q->b.dltxt.dltxt_value, buffer);
      break;
    case UPIF_SHOW_WARNING :
      break;
    case UPIF_MOVING_HISTO :
      UpiBufferGetInt (GetBuffer, &row);
      UpiBufferGetInt (GetBuffer, &col);
      upic_drag_histo (0, row, col);
      list_remove_entry ((Link*) q);
      q = 0;
      break;
    case UPIF_DECLARE_CALLBACK :
      UpiBufferGetInt (GetBuffer, &menu_id);
      UpiBufferGetInt (GetBuffer, &item_id);
      UpiBufferGetInt (GetBuffer, &condition);
      m = upic_find_menu (menu_id);
      if (m && (m->condition & condition))        {
        if (m->callback) (*m->callback)(menu_id, item_id,
          condition, m->arg);
      }
      list_remove_entry ((Link*) q);
      q = 0;
      break;
    }
  }
  UpiBufferReset (GetBuffer);
  if (wakeup) wtc_insert_head (WT_FACILITY_WAKEUP);
  if (q) wtc_insert_head (WT_FACILITY_UPI);
  return 1;
}

//--------------------------------------------------------------------------
static int broadcast (int /* fac */)    {
  char* source = 0;
  /* int status = */ upic_net_spy (&source);
  if (source)  {
    UpiConnect connect = UpiConnectFind (source);
    free (source);
    if (connect)    {
      UpiConnectDelete (connect);
    }
  }
  return 1;
}

//--------------------------------------------------------------------------
static int rearm (int /* fac */)    {
  if (Header.queue.first) return (1);
  upic_end_update();
  UpiBufferPutInt (SendBuffer, UPIF_INPUT);
  UpiBufferSend (SendBuffer);
  return (1);
}

//--------------------------------------------------------------------------
void upir_init_remote ()    {
  //_asm int 3
  char* source;
  Routine a = 0, r = 0;
  memset(&Header,0,sizeof(Header));
  SendBuffer = UpiBufferNew ();
  GetBuffer = UpiBufferNew ();
  wtc_init();
  int i = wtc_get_routines (WT_FACILITY_UPI, (wt_callback_t*)&r, (wt_callback_t*)&a);
  if ( i != WT_SUCCESS ) a = 0;
  wtc_subscribe (WT_FACILITY_UPI, (wt_callback_t) rearm, (wt_callback_t) a);
  upic_net_init (0, &source, (WtRoutine) handler, (WtRoutine) broadcast);
  if (source)  {
    UpiConnectNew (source);
    free (source);
  }
  UpiBufferPutInt (SendBuffer, UPIF_INIT_REMOTE);
  UpiBufferSend (SendBuffer);
}
#endif
