/*
SERVER.C
Created           : 29-NOV-1989 by Christian Arnault

---------------------------------------------------------------------------*/
#ifdef SCREEN
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "RTL/rtl.h"
#include "UPI/file.h"
#include "UPI/upidef.h"
#include "UPI/upirem.h"
#include "UPI/upi_buffer.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"
#include "AMS/amsdef.h"

#include "SCR/scr.h"

int upic_net_close_mbx(int) { return 1; }
int upic_net_open_mbx(const char*) {  return 1; }

namespace {
enum {
  MAIN_MENU = 1,
  PARAMS_MENU,
  KEYBOARD_MENU,
  LOGFILE_MENU,
  LAST_MENU
};

enum {
  C_SEPARATOR = 1000000,
  C_DESTINATION,
  C_CONNECT,
  C_DISCONNECT,
  C_KILL,
  C_START,
  C_PARAMS,
  C_LOCK,
  C_LOG,
  C_SAVE_CONF,
  C_RESTORE_CONF,
  C_EXIT,

  C_P1,
  C_P2,
  C_P3,
  C_P4,

  C_INPUT,

  C_LOG_SEP1,
  C_LOG_ACTIVATE,
  C_LOG_DISACTIVATE,
  C_LOG_RESET,
  C_LOG_NAME,
  C_LOG_SHOW,
  C_LOG_DUMP_DTB
};

struct Menu_list;
struct Histo_list;
struct Var;

struct SrvConnect {
  SrvConnect Link_items;

  struct {
    Menu_list Linked_list_items;
  } menu_list;
  struct {
    Histo_list Linked_list_items;
  } histo_list;
  struct {
    Var Linked_list_items;
  } var;
  char* source;
  int disabled;
  int starting;
  int connected;
  int current_menu;
  int current_command;
  int current_param;
};

struct Menu_list {
  Menu_list Link_items;
  int remote_id;
  int local_id;
  Window* window;
};

struct Histo_list {
  Histo_list Link_items;

  int    remote_id;
  Histo* local_id;
};


struct Var {
  Var Link_items;
  void* reference;
  int  type;
  int  size;
  union {
    int     i;
    double  d;
    char* c;
  } value;
};

typedef struct KBD_REQUEST Kbd_request;
typedef struct KBD_CONNECT Kbd_connect;

struct KBD_REQUEST {
  Kbd_request Link_items;
  int lun;
};

struct KBD_CONNECT {
  Kbd_connect Link_items;
  char* name;
  int lun;
  int chan;
  FILE* f;
};
struct SrvConnection {
  int col;
  char s[64];
  void *data;
};

static char Error_message[256];

int conn_comp(const void *a1, const void*a2)  {
  int col1    = ((SrvConnection*)a1)->col;
  int col2    = ((SrvConnection*)a2)->col;
  if ( col1 < col2)      return -1;
  else if (col1 == col2) return 0;
  return 1;
}
}

/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
int exit_handler();
void screen_handler();
void upi_handler();
int to_be_started (char* name);
int message_handler (unsigned int, void*);
int broadcast (unsigned int, void*);
SrvConnect* find_connect (char* source);
SrvConnect* find_connect_with_name (char* source);
int find_remote_id (SrvConnect* connect, int id);
int find_menu_from_window (SrvConnect* connect, Window* w);
SrvConnect* find_connect_with_id (int local_id, int* remote_id);
int new_remote_id (SrvConnect* connect, int id, Window* w);
Histo* find_remote_histo (SrvConnect* connect, int histo);
void new_remote_histo (SrvConnect* connect, int histo, Histo* hptr);
int check_first_menu (SrvConnect* connect, Menu* menu);
Var* find_var (SrvConnect* connect, void* ref);
void input (SrvConnect* connect);
void delete_command (SrvConnect* connect);
void enable_command (SrvConnect* connect);
void disable_command (SrvConnect* connect);
void replace_item (SrvConnect* connect);
void insert_item (SrvConnect* connect);
void close_menu (SrvConnect* connect);
void delete_menu (SrvConnect* connect);
void erase_menu (SrvConnect* connect);
void write_message (SrvConnect* connect);
void set_message_window (SrvConnect* connect);
void get_message_window (SrvConnect* connect);
void change_titles (SrvConnect* connect);
void modify_param (SrvConnect* connect);
void refresh_param_page (SrvConnect* connect);
void refresh_param_line (SrvConnect* connect);
void set_cursor (SrvConnect* connect);
void save_screen (SrvConnect* connect);
void restore_screen (SrvConnect* connect);
void begin_update (SrvConnect* connect);
void end_update (SrvConnect* connect);
void init_remote (SrvConnect* connect);
void quit (SrvConnect* connect);
void open_window (SrvConnect* connect);
void open_old_window (SrvConnect* connect);
void set_window_position (SrvConnect* connect);
void get_window_position (SrvConnect* connect);
void get_window_size(SrvConnect* connect);
void attach_pf1 (SrvConnect* connect);
void cancel_notice (SrvConnect* connect);
void get_items_per_page (SrvConnect* connect);
void set_items_per_page (SrvConnect* connect);
void get_mode (SrvConnect* connect);
void set_mode (SrvConnect* connect);
void register_on_keypad (SrvConnect* connect);
void show_notice (SrvConnect* connect);
void show_warning (SrvConnect* connect);
void connect_process (SrvConnect* connect);
void disconnect_process (SrvConnect* connect);
void lock_cursor (SrvConnect* connect);
void save_conf();
int restore_conf(int fac,int param);
void start_restore_conf();
void end_restore_conf();
void unlock_cursor (SrvConnect* connect);
void dldec (SrvConnect* connect);
void dlend (SrvConnect* connect);
void dlhex (SrvConnect* connect);
void dlhead (SrvConnect* connect);
void dlkey (SrvConnect* connect);
void dlm32 (SrvConnect* connect);
void dlmask (SrvConnect* connect);
void dloct (SrvConnect* connect);
void dltxt (SrvConnect* connect);
void dlout (SrvConnect* connect);
void dlouto (SrvConnect* connect);
void dloutx (SrvConnect* connect);
void dlyeno (SrvConnect* connect);
void back_space (SrvConnect* connect);
void drag_histo (int row, int col);
void book_histo (SrvConnect* connect);
void unbook_histo (SrvConnect* connect);
void scale_histo (SrvConnect* connect);
void unscale_histo (SrvConnect* connect);
void fill_histo (SrvConnect* connect);
void display_histo (SrvConnect* connect);
void hide_histo (SrvConnect* connect);
void move_histo (SrvConnect* connect);
void moving_histo (SrvConnect* connect);
void copy_histo (SrvConnect* connect);
void callback_handler (int menu_id, int item_id, int condition,
                       SrvConnect* connect);
void callback_handler_wo_user (int menu_id, int item_id, int condition,
                               SrvConnect* connect);
void PF1_callback (int menu_id, int item_id, int condition,
                   SrvConnect* connect);
void PF3_callback (int menu_id, int item_id, int condition,
                   SrvConnect* connect);
void AnyBSCallback (int menu_id, int item_id, int condition,
                    SrvConnect* connect);
void declare_callback (SrvConnect* connect);
void hide_menu (SrvConnect* connect);
Histo* fetch_histo ();
void fetch_menu (Menu* menu, SrvConnect* connect);
void fetch_item (Item* item, SrvConnect* connect);
void fetch_param (Param* param, SrvConnect* connect);
int server_send_message (char* source);
void upper_case (char* name);
void start (const char* name, const char* node);
void kill_process (const char* name);
int exist (const char* name);
void scheinit (int sid, const char* name);
int get_pid ();
void get_pname (const char* name);
int cut_blanks (const char* buf);
void get_my_node ();
int kbd_handler (unsigned int event, void* param);
Kbd_connect* find_kbd_connect (const char* name);
Kbd_connect* find_old_kbd_connect (const char* name);
Kbd_connect* find_kbd_connect_with_lun (int lun);
void kill_kbd_connect (Kbd_connect* c);
void rearm_scr_mbx (int event);
void ast_scr (int arg);
void rearm_kbd_mbx (int event, int chan);
void ast_kbd (int arg);
void log_message (const char* t1, const char* t2);
void log_reset ();
void log_show ();
void log_get_name ();
void log_set_name (const char* name);
void database_dump ();
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//static int   Updating = 0;
static UpiBuffer GetBuffer = 0;
static UpiBuffer AckBuffer = 0;
//static int   Frame = 0;
static int restoring_configuration = 0;
static FILE *restore_file;
typedef void (*SrvFunc)(SrvConnect*);
static SrvFunc Actions[] = { 
  0,
    input,
    delete_command,
    enable_command,
    disable_command,
    replace_item,
    insert_item,
    close_menu,
    delete_menu,
    erase_menu,
    write_message,
    set_message_window,
    get_message_window,
    change_titles,
    modify_param,
    refresh_param_page,
    refresh_param_line,
    set_cursor,
    save_screen,
    restore_screen,
    begin_update,
    end_update,
    init_remote,
    quit,
    open_window,
    open_old_window,
    set_window_position,
    get_window_position,
    attach_pf1,
    cancel_notice,
    get_items_per_page,
    set_items_per_page,
    get_mode,
    set_mode,
    register_on_keypad,
    show_notice,
    show_warning,
    connect_process,
    disconnect_process,
    lock_cursor,
    unlock_cursor,
    dldec,
    dlend,
    dlhex,
    dlhead,
    dlkey,
    dlm32,
    dlmask,
    dloct,
    dltxt,
    dlout,
    dlouto,
    dloutx,
    dlyeno,
    back_space,

    book_histo,
    unbook_histo,
    scale_histo,
    unscale_histo,
    fill_histo,
    display_histo,
    hide_histo,
    move_histo,
    moving_histo,
    copy_histo,

    declare_callback,
    hide_menu,
    get_window_size,
};

static struct {
  struct {
    SrvConnect Linked_list_items;
  } connect;
  int last_id;
} Sys;

static Pasteboard* Pb;

static char Ams_dest[81] = "";
static char Dest[81] = "";
static char Node[81] = "";
static char My_name[81] = "";
//static char Old_name[81] = "";
static char My_node[81] = "";

static int Cursor_locked = 0;

static char P[4][81];

static char Text[133];

#define EVENT_SCR 20
#define EVENT_KBD 21

static int  Event_scr = EVENT_SCR;
static int  Event_kbd = EVENT_KBD;

static FILE* F_scr = 0;
static int   Chan_scr = 0;

static char Input_text[133];
static char Output_text[133];
static int  End = 0;
//static int  Old_priority = 4;
static SrvConnect* SrvConnect_of_histo = 0;

static int LogFile_active = 1;
static char LogFile_name[133];
static System *UPIsystem;

static struct {
  struct {
    Kbd_connect Linked_list_items;
  } connects;
  struct {
    Kbd_request Linked_list_items;
  } requests;
} Mbx_header;

/*-------------------------------------------------------------------------*/
int exit_handler()  {
  int status = 1;
  Kbd_connect* c;
  if (Chan_scr)  {
    //upic_net_close_mbx (Chan_scr);
    if ( F_scr ) fclose (F_scr);
    F_scr = 0;
    while ((c = Mbx_header.connects.first))    {
      kill_kbd_connect (c);
    }
  }
  return (status);
}

/*-------------------------------------------------------------------------*/
extern "C" int upi_server (int argc, char** argv)  {
  int       status = 1;
  list_init ((Linked_list*) &Sys.connect);
  Sys.last_id = LAST_MENU;

  GetBuffer = UpiBufferNew ();
  AckBuffer = UpiBufferNew ();

  wtc_init();
  //wtc_subscribe (Event_scr, (wt_callback_t)rearm_scr_mbx, 0);
  upic_attach_terminal();
  UPIsystem = upic_get_system();
  upic_set_mode (WAKE_UP_ON_CHANGE);
  upic_get_info (&Pb);
  upic_set_drag_histo_action((Routine)drag_histo);
  upic_declare_exit_handler ((Routine)exit_handler);

  get_my_node();
  sprintf (My_name, "SW%08X", get_pid());
  status = upic_net_init (My_name, 0, message_handler, broadcast);
  if (!(status & 1)) exit(status);

  strcpy (Dest, My_node);
  strcat (Dest, My_name);
  scheinit (0x1234, Dest);

  upic_open_detached_menu (MAIN_MENU, 0, 0, "Server", "", "");
  upic_add_comment (C_SEPARATOR, "---------------", "");
  strcpy (Dest, "");
  strcpy (Node, "");
  upic_set_param (Dest, 1, "A30", Dest, 0, 0, 0, 0, 0);
  upic_set_param (Node, 2, "A8", Node, 0, 0, 0, 0, 0);
  upic_add_command (C_DESTINATION, "Name ^^^^^^^^^^^^^^ Node ^^^^^^^^", "");
  upic_add_comment (C_CONNECT,     "------> Connect", "");
  upic_add_comment (C_DISCONNECT,  "------> Disconnect", "");
  upic_add_comment (C_KILL,        "------> Kill", "");
  upic_add_comment (C_START,       "------> Start", "");
  upic_add_command (C_PARAMS,      "Params for start", "");
  upic_add_command (C_LOCK,        "Lock the cursor", "");
  upic_add_command (C_LOG,         "LogFile management", "");
  upic_add_command (C_SAVE_CONF,   "Save Configuration", "");
  upic_add_command (C_RESTORE_CONF,"Restore Configuration", "");
  upic_add_command (C_EXIT,    "Exit", "");
  upic_close_menu ();

  upic_attach_pf1 (MAIN_MENU);
  upic_declare_callback(0,CALL_ON_PF1, (Routine)PF1_callback, 0);
  upic_declare_callback(0,CALL_ON_DRAG, (Routine)PF1_callback, 0);
  upic_declare_callback(0,CALL_ON_ANY_BACKSPACE, (Routine)AnyBSCallback, 0);
  upic_declare_callback(0,CALL_ON_MOVE_LEFT, (Routine)AnyBSCallback, 0);
  upic_declare_callback(0,CALL_ON_MOVE_RIGHT, (Routine)AnyBSCallback, 0);

  upic_open_param (PARAMS_MENU, MAIN_MENU, C_PARAMS, "Start parameters", "", "");
  for (int i=0; i<4; i++)  {
    upic_set_param (P[i], 1, "A80", P[i], 0, 0, 0, 0, 0);
    sprintf (Text, "P%d = ^^^^^^^^^^^^^^^", i + 1);
    upic_add_command (C_P1 + i, Text, "");
  }
  upic_close_menu();
  list_init ((Linked_list*) &Mbx_header.connects.first);
  list_init ((Linked_list*) &Mbx_header.requests.first);

  sprintf (Dest, "MAILBOX_%s", My_name);
  Chan_scr = upic_net_open_mbx (Dest);
  // F_scr = fopen (Dest, "r");

  upic_open_detached_menu (KEYBOARD_MENU, 0, 0, "Input", "", "");
  upic_set_param (Input_text, 1, "A132", Input_text, 0, 0, 0, 0, 0);
  upic_add_command (C_INPUT, "Upi > ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^", "");
  upic_close_menu ();

  upic_open_detached_menu (LOGFILE_MENU, MAIN_MENU, C_LOG,
    "LogFile Management", "", "Hit <BackSpace> to return to Main Menu");

  upic_add_comment (C_LOG_SEP1,        "Actions on LogFile :", "");
  upic_add_comment (C_LOG_ACTIVATE,    "  | Activate", "");
  upic_add_command (C_LOG_DISACTIVATE, "  | DisActivate", "");
  upic_add_command (C_LOG_RESET,       "  | Reset", "");
  log_get_name ();
  upic_set_param (LogFile_name, 1, "A132", LogFile_name, 0, 0, 0, 0, 0);
  upic_add_command (C_LOG_NAME,        
    "  | Set current to  : ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^", "");
  upic_add_command (C_LOG_SHOW,        "  | Show current definition", "");
  upic_add_command (C_LOG_DUMP_DTB,    "  | Dump server database", "");

  upic_close_menu ();

  if (argc == 2)  {
    char saved;
    char* c = strchr (argv[1], ':');
    if (c && c[1] == ':')    {
      saved = *c;
      *c = 0;
      strcpy (Node, argv[1]);
      *c = saved;
      c += 2;
    }
    else c = argv[1];
    strcpy (Dest, c);

    start (Dest, Node);
  }

  log_show ();
  while (!End)  {
    int sc;
    void* par;
    unsigned int event;
    wtc_wait (&event,&par,&sc);
    switch (event)    {
    case EVENT_SCR:
      screen_handler ();
      break;
    case WT_FACILITY_UPI:
      upi_handler();
      break;
    case WT_FACILITY_AMS:
      message_handler(event,par);
      break;
    default:   /* Other events  */
      break;
    }
  }
  upic_quit();
  return 1;
}

//--------------------------------------------------------------------------
void screen_handler()  {
  Kbd_connect* c;
  strcpy (Output_text, "");
  fscanf (F_scr, "%[^\n]%*c", Output_text);
  upic_write_message (Output_text, "");
  if (!strncmp (Output_text, "save", 4))  {
    c = find_kbd_connect (&Output_text[5]);
    c->chan = upic_net_open_mbx (c->name);
    c->lun = open (c->name, O_RDWR, 7);
    c->f   = fopen (c->name, "w");
    Event_kbd = EVENT_KBD + c->lun;
    rearm_kbd_mbx (Event_kbd, c->chan);
    //wtc_subscribe (Event_kbd, 0, kbd_handler);
  }
  else if (!strncmp (Output_text, "restore", 7))  {
    c = find_old_kbd_connect (&Output_text[8]);
    if (c)    {
      upic_net_close_mbx (c->chan);
      close  (c->lun);
      fclose (c->f);
      kill_kbd_connect (c);
    }
  }
}

//--------------------------------------------------------------------------
void upi_handler()
//--------------------------------------------------------------------------
{
  int      menu, command, param;
  int      status;
  SrvConnect* c;
  int      remote_id;
  char* buffer;
  int      length;
  int      list_index;

  upic_get_input_with_index (&menu, &command, &param, &list_index);
  switch (menu)  {
  case MAIN_MENU:
    switch (command)    {
    case C_DESTINATION:
      {
        char* c;

        length = cut_blanks (Dest);
        Dest[length] = '\0';
        upper_case (Dest);
        c = (char*) strchr (Node, ':');
        if (c) *c = '\0';
        c = (char*) strchr (Node, ' ');
        if (c) *c = '\0';
        upper_case (Node);
        if (strlen(Node))        {
          strcpy (Ams_dest, Node);
          strcat (Ams_dest, "::");
        }
        else
          strcpy (Ams_dest, My_node);

        strcat (Ams_dest, Dest);

        if (to_be_started (Dest))        {
          upic_disable_command (MAIN_MENU, C_CONNECT);
          upic_disable_command (MAIN_MENU, C_DISCONNECT);
          upic_disable_command (MAIN_MENU, C_KILL);
          upic_enable_command  (MAIN_MENU, C_START);
          upic_set_cursor (MAIN_MENU, C_START, 0);
        }
        else        {
          upic_enable_command  (MAIN_MENU, C_CONNECT);
          upic_enable_command  (MAIN_MENU, C_DISCONNECT);
          upic_enable_command  (MAIN_MENU, C_KILL);
          upic_disable_command (MAIN_MENU, C_START);
          upic_set_cursor (MAIN_MENU, C_CONNECT, 0);
        }
      }
      break;
    case C_CONNECT:
      if (!find_connect_with_name (Ams_dest))      {
        UpiBufferPutInt (AckBuffer, UPIF_RECONNECT);
        status = server_send_message (Ams_dest);
        if (!(status & 1)) upic_signal_error (status, "");
        else upic_write_message ("Connecting...", "");
      }
      else
        upic_write_message ("Process already connected", "");
      break;
    case C_DISCONNECT:
      UpiBufferPutInt (AckBuffer, UPIF_DISCONNECT_PROCESS);
      status = server_send_message (Ams_dest);
      if (!(status & 1)) upic_signal_error (status, "");
      else upic_write_message ("Disconnecting...", "");
      break;
    case C_KILL:
      kill_process (Dest);
      break;
    case C_START:
      upic_disable_command (MAIN_MENU, C_START);
      upic_set_cursor (MAIN_MENU, C_DESTINATION, 0);
      upic_end_update ();
      start (Dest, Node);
      break;
    case C_LOCK:
      if (Cursor_locked) unlock_cursor (0);
      else lock_cursor (0);
      break;
    case C_SAVE_CONF:
      save_conf();
      break;
    case C_RESTORE_CONF:
      start_restore_conf();
      break;
    case C_EXIT:
      End = 1;
      break;
    default:
      {
        int rid;
        SrvConnect* cc  = find_connect_with_id(menu, &rid);
        c = find_connect_with_id(command, &remote_id);
        if (c)    {
          if (cc == c)      {
            upic_set_cursor_and_mark
              (c->current_menu,c->current_command,c->current_param,1);
          }
          else      {
            upic_set_cursor_and_mark
              (c->current_menu,c->current_command,c->current_param,0);
          }
        }
      }
    }
    break;
  case PARAMS_MENU:
    break;
  case KEYBOARD_MENU:  {
      Kbd_request* r = Mbx_header.requests.first;
      if (r)   {
        Kbd_connect* c = find_kbd_connect_with_lun (r->lun);
        length = cut_blanks (Input_text);
        Input_text[length] = 0;
        fprintf (c->f, "%s\n", Input_text);
        Event_kbd = EVENT_KBD + r->lun;
        rearm_kbd_mbx (Event_kbd, c->chan);
        list_remove_entry ((Link*) r);
      }
      if (!(r = Mbx_header.requests.first))  {
	upic_back_space (KEYBOARD_MENU);
      }
      else  {
        Kbd_connect* c = find_kbd_connect_with_lun (r->lun);
        upic_change_titles (2, c->name, "", "");
      }
    }
    break;
  case LOGFILE_MENU:
    switch (command)  {
    case C_LOG_ACTIVATE:
      LogFile_active = 1;
      log_show ();
      upic_disable_command(LOGFILE_MENU, C_LOG_ACTIVATE);
      upic_enable_command (LOGFILE_MENU, C_LOG_DISACTIVATE);
      upic_enable_command (LOGFILE_MENU, C_LOG_RESET);
      upic_net_start_log ();
      break;
    case C_LOG_DISACTIVATE:
      LogFile_active = 0;
      upic_enable_command  (LOGFILE_MENU, C_LOG_ACTIVATE);
      upic_disable_command (LOGFILE_MENU, C_LOG_DISACTIVATE);
      upic_disable_command (LOGFILE_MENU, C_LOG_RESET);
      upic_net_stop_log ();
      break;
    case C_LOG_RESET:
      log_reset ();
      break;
    case C_LOG_NAME:
      log_set_name (LogFile_name);
      break;
    case C_LOG_SHOW:
      log_get_name ();
      upic_refresh_param_page (LOGFILE_MENU);
      log_show ();
      break;
    case C_LOG_DUMP_DTB:
      database_dump ();
      upic_net_flush_log ();
      break;
    }
    break;
  default:    
    if ((c = find_connect_with_id (menu, &remote_id)) && !c->disabled)  {
      UpiBufferPutInt (AckBuffer, UPIF_INPUT);
      UpiBufferPutInt (AckBuffer, remote_id);
      UpiBufferPutInt (AckBuffer, command);
      UpiBufferPutInt (AckBuffer, param);
      UpiBufferPutInt (AckBuffer, list_index);
      upic_store_vars (menu, command, &buffer, &length);
      UpiBufferPutBytes (AckBuffer, buffer, length);
      free (buffer);
      server_send_message (c->source);
      c->disabled = 0;
    }
    break;
  }
}


//--------------------------------------------------------------------------
int to_be_started (char* name)  {
  for (char* c = name;;)  {
    c = (char*) strchr (c, '_');
    if (!c) return (1);
    c++;
    if (c[1]) continue;
    if (*c >= '0' && *c <= '9') return (0);
    if (*c >= 'A' && *c <= 'F') return (0);
  }
}

//--------------------------------------------------------------------------
int message_handler (unsigned int, void*)  {
  size_t    message_length = 0;
  int       code, *cursor_context;
  char*     source = 0, *buffer = 0;
  SrvConnect*  c = 0;
  UpiConnect connect  = 0;
  UpiBufferInfo info;

  int status = upic_net_read (&buffer, &message_length, &source);
  if (!status)  {
    if ((status == AMS_TASKDIED) || (status == AMS_CONNCLOSED)) 
      broadcast (0,0);
    return (status);
  }

  UpiBufferInit (GetBuffer, buffer, message_length);
  if (source)  {
    connect = UpiConnectNew (source);
    c = find_connect (source);
    free (source);
  }
  upic_begin_update();
  scrc_save_cursor (Pb, &cursor_context);
  while ((info = UpiBufferCheckProtocol (GetBuffer)) == UpiBufferOk)  {
    UpiBufferGetInt (GetBuffer, &code);
    if ((code >= UPIF_FIRST_CODE) && (code < UPIF_LAST_CODE))    {
      (*Actions[code - UPIF_FIRST_CODE])(c);
    }
    else    {
      upic_write_message ("Server> wrong message code", "");
    }
  }
  if ((info != UpiBufferNoData) && (info != UpiBufferBadHeader))  {
    const char* infoText = UpiBufferGetInfoText (info);
    if (infoText) upic_write_message (infoText, "");
  }
  UpiBufferReset (GetBuffer);
  if (c) server_send_message (c->source);
  scrc_restore_cursor (Pb, cursor_context);
  upic_end_update();

  return WT_SUCCESS;
}

//--------------------------------------------------------------------------
int broadcast (unsigned int, void*)  {
  char* source;
  int status = upic_net_spy (&source);
  if ((status == AMS_TASKDIED) || (status == AMS_CONNCLOSED))  {
    if (source)    {
      upic_begin_update();
      SrvConnect* c = find_connect (source);
      free (source);
      quit (c);
      upic_end_update();
      if (c)  {
        if (c->source) free (c->source);
        /*        list_remove_entry ((Link*) c); BJ */
      }
      wtc_insert_head (WT_FACILITY_WAKEUP);
    }
    else  {
      upic_write_message ("AMS_TASKDIED received with no source.", "");
    }
  }
  return WT_SUCCESS;
}

//--------------------------------------------------------------------------
SrvConnect* find_connect (char* source)  {
  char* blank;
  if ((blank = strchr (source, ' '))) *blank = '\0';
  SrvConnect* c = Sys.connect.first;
  for( ;c;c=c->next) {
    if (!strcmp(c->source, source)) return (c);
  }
  c = (SrvConnect*) list_add_entry(&Sys.connect,sizeof (SrvConnect));
  c->source = list_malloc (strlen(source)+1);
  strcpy (c->source, source);
  c->disabled = 0;
  c->connected = 0;
  c->current_menu = 0;
  c->current_command = 0;
  c->current_param = 0;
  list_init ((Linked_list*) &c->menu_list);
  list_init ((Linked_list*) &c->histo_list);
  list_init ((Linked_list*) &c->var);
  return (c);
}

//--------------------------------------------------------------------------
SrvConnect* find_connect_with_name (char* source)  {
  char* blank;
  if ((blank = strchr (source, ' '))) *blank = '\0';
  for(SrvConnect* c = Sys.connect.first; c; c=c->next) {
    if (!strcmp(c->source, source))    {
      if (!c->connected) return 0;
      return c;
    }
  }
  return 0;
}

//--------------------------------------------------------------------------
int find_remote_id (SrvConnect* connect, int id)  {
  for(Menu_list* m=connect->menu_list.first; m; m=m->next)  {
    if (m->remote_id == id) return (m->local_id);
  }
  return 0;
}  

//--------------------------------------------------------------------------
int find_menu_from_window (SrvConnect* connect, Window* w)  {
  for(Menu_list* m=connect->menu_list.first; m; m=m->next)  {
    if (m->window == w) return (m->local_id);
  }
  return 0;
}  

//--------------------------------------------------------------------------
SrvConnect* find_connect_with_id (int local_id, int* remote_id)  {
  for(SrvConnect* c = Sys.connect.first;c;c=c->next) {
    for(Menu_list* m = c->menu_list.first;m;m=m->next) {
      if (m->local_id == local_id)  {
        *remote_id = m->remote_id;
        return (c);
      }
    }
  }
  return 0;
}  

//--------------------------------------------------------------------------
int new_remote_id (SrvConnect* connect, int id, Window* w)  {
  Menu_list*  m = connect->menu_list.first;
  for (; m; m=m->next )  {
    if (m->remote_id == id) break;
  }
  if (!m)  {
    m = (Menu_list*) list_add_entry ((Linked_list*) &connect->menu_list, 
      sizeof(Menu_list));
    m->remote_id = id;
    m->local_id = id = ++Sys.last_id;
  }
  else {
    id = m->local_id;
  }
  m->window = w;
  return id;
}  

//--------------------------------------------------------------------------
Histo* find_remote_histo (SrvConnect* connect, int histo)  {
  for( Histo_list* h = connect->histo_list.first;h;h=h->next) {
    if (h->remote_id == histo) return (h->local_id);
  }
  return 0;
}  

//--------------------------------------------------------------------------
void new_remote_histo (SrvConnect* connect, int histo, Histo* hptr)  {
  Histo_list* h = connect->histo_list.first;
  for( ;h;h=h->next) {
    if (h->remote_id == histo) break;
  }
  if (h && h->local_id) {
    upic_unbook_histo (h->local_id);
  }
  else  {
    h = (Histo_list*)list_add_entry(&connect->histo_list,sizeof(Histo_list));
    h->remote_id = histo;
  }
  h->local_id = hptr;
}  

//--------------------------------------------------------------------------
int check_first_menu (SrvConnect* connect, Menu* menu) {
  Menu_list* m = connect->menu_list.first;
  if (!m->next)  {
    char* c = connect->source;
    upic_insert_command (
      MAIN_MENU,
      0,
      m->local_id,
      c, "");
    menu->from.menu = 0;
    menu->from.item = 0;
    if (menu->type == NORMAL_MENU) upic_open_window();
    return 1;
  }
  return 0;
}

//--------------------------------------------------------------------------
Var* find_var (SrvConnect* connect, void* ref) {
  Var* v = connect->var.first;
  for( ; v; v=v->next )  {
    if (v->reference == ref) return v;
  }
  v = (Var*) list_add_entry(&connect->var, sizeof(Var));
  v->reference = ref;
  return v;
}  

//--------------------------------------------------------------------------
void input (SrvConnect* connect)  {
  connect->disabled = 0;
  wtc_insert_head (WT_FACILITY_SCR);
}

//--------------------------------------------------------------------------
void delete_command (SrvConnect* connect) {
  int menu_id, item_id;
  UpiBufferGetInt (GetBuffer, &menu_id);
  UpiBufferGetInt (GetBuffer, &item_id);
  if ((menu_id = find_remote_id (connect, menu_id)))  {
    upic_delete_command (menu_id, item_id);
  }
}

//--------------------------------------------------------------------------
void enable_command (SrvConnect* connect) {
  int menu_id, item_id;
  UpiBufferGetInt (GetBuffer, &menu_id);
  UpiBufferGetInt (GetBuffer, &item_id);
  if ((menu_id = find_remote_id (connect, menu_id)))  {
    upic_enable_command (menu_id, item_id);
  }
}

//--------------------------------------------------------------------------
void disable_command (SrvConnect* connect)  {
  int menu_id, item_id;
  UpiBufferGetInt (GetBuffer, &menu_id);
  UpiBufferGetInt (GetBuffer, &item_id);
  if ((menu_id = find_remote_id (connect, menu_id)))  {
    upic_disable_command (menu_id, item_id);
  }
}

//--------------------------------------------------------------------------
void replace_item (SrvConnect* connect)   {
  int menu_id;
  Item* item;
  UpiBufferGetInt (GetBuffer, &menu_id);
  UpiBufferGetItem (GetBuffer, &item);
  if (item == 0)  {
    upic_write_message("For Expert:....   In replace_item: Item pointer is Zero","");
    return;
  }
  fetch_item (item, connect);

  if ((menu_id = find_remote_id (connect, menu_id)))  {
    if (item->enabled)   
      upic_replace_command (menu_id, item->id, item->string, item->help);
    else   
      upic_replace_comment (menu_id, item->id, item->string, item->help);
  }
}

//--------------------------------------------------------------------------
void insert_item (SrvConnect* connect)  {
  int menu_id;
  int before;
  Item* item;

  UpiBufferGetInt (GetBuffer, &menu_id);
  UpiBufferGetInt (GetBuffer, &before);
  UpiBufferGetItem (GetBuffer, &item);
  fetch_item (item, connect);
  if ((menu_id = find_remote_id (connect, menu_id)))  {
    if (item->enabled)    {
      upic_insert_command (menu_id, before, item->id, item->string, item->help);
    }
    else    {
      upic_insert_comment (menu_id, before, item->id, item->string, item->help);
    }
  }
}

//--------------------------------------------------------------------------
void close_menu (SrvConnect* connect)  {
  Menu* menu;
  UpiBufferGetMenu (GetBuffer, &menu);
  fetch_menu (menu, connect);
}

//--------------------------------------------------------------------------
void delete_menu (SrvConnect* connect)  {
  int menu_id;
  UpiBufferGetInt (GetBuffer, &menu_id);
  if ((menu_id = find_remote_id (connect, menu_id)))  {
    upic_delete_menu (menu_id);
    if (menu_id != connect->current_menu)    {
      upic_set_cursor(connect->current_menu,connect->current_command,
        connect->current_param);
    }
    else    {
      Menu_list *m    = connect->menu_list.first;
      if (m != 0)  {
        connect->current_menu	= m->local_id;
        connect->current_command    = 0;
        connect->current_param	= 0;
        upic_set_cursor(connect->current_menu,connect->current_command,
          connect->current_param);
      }
    }
  }
  upic_begin_update();
  int maxcol=1;
  int i, nconn=0, row, col, w, h;
  SrvConnect* c   = Sys.connect.first;
  for( ; c; c=c->next) nconn++;
  if (nconn == 0)    {
    return;
  }
  SrvConnection* carr    = (SrvConnection*)malloc(nconn*sizeof(SrvConnection));
  if (carr == 0)  {
    return;
  }
  for(c = Sys.connect.first, i=0; c; c=c->next, i++) {
    Menu_list *m    = c->menu_list.first;
    if (m != 0)        {
      upic_get_window_position(m->local_id,&carr[i].col,&row);
    }
    else
      {
	carr[i].col = 9999999;
      }
    carr[i].data    = c;
    strcpy(carr[i].s,c->source);
  }
  nconn   = i;
  qsort (carr, nconn, sizeof(SrvConnection), conn_comp);
  for (i=0;i<nconn;i++)  {
    c	= (SrvConnect*)carr[i].data;
    Menu_list *m = c->menu_list.first;
    if (m != 0)  {
      upic_set_window_position(m->local_id,maxcol,1);
      upic_get_window_position(m->local_id,&col,&row);
      upic_get_window_size(m->local_id,&w,&h);
      maxcol  = w+col+1;
    }
  }
  upic_end_update();
}

//--------------------------------------------------------------------------
void erase_menu (SrvConnect* connect)  {
  int menu_id;
  UpiBufferGetInt (GetBuffer, &menu_id);
  if ((menu_id = find_remote_id (connect, menu_id)))  {
    upic_erase_menu (menu_id);
  }
}

//--------------------------------------------------------------------------
void write_message (SrvConnect* connect)  {
  char *text1, *text2, *t1, *t2, *timestr;
  static char null[] = "";
  time_t t;
  static const char format1[] = "[";
  static const char format2[] = " - ";
  static const char format3[] = "] ";
  int size_format;

  UpiBufferGetText (GetBuffer, &text1);
  UpiBufferGetText (GetBuffer, &text2);
  t = time (0);
  timestr = ctime (&t);
  timestr[19] = 0;
  timestr += 4;

  size_format = sizeof(format1) + strlen(timestr) + sizeof(format2) +
    strlen(connect->source) + sizeof(format3);

  if (strlen(text1))  {
    t1 = list_malloc (size_format + strlen(text1) + 1);
    strcpy (t1, format1);
    strcat (t1, timestr);
    strcat (t1, format2);
    strcat (t1, connect->source);
    strcat (t1, format3);
    strcat (t1, text1);
  }
  else t1 = null;

  if (strlen(text2))  {
    t2 = list_malloc (size_format + strlen(text2) + 1);
    strcpy (t2, format1);
    strcat (t2, timestr);
    strcat (t2, format2);
    strcat (t2, connect->source);
    strcat (t2, format3);
    strcat (t2, text2);
  }
  else t2 = null;

  upic_write_message (t1, t2);
  upic_end_update ();

  if (LogFile_active) log_message (t1, t2);

  if (strlen(text1)) free (t1);
  if (strlen(text2)) free (t2);
}

//--------------------------------------------------------------------------
void set_message_window (SrvConnect* /* connect */ )  {
  int rows, cols, row, col;
  UpiBufferGetInt (GetBuffer, &rows);
  UpiBufferGetInt (GetBuffer, &cols);
  UpiBufferGetInt (GetBuffer, &row);
  UpiBufferGetInt (GetBuffer, &col);
  upic_set_message_window (rows, cols, row, col);
}

//--------------------------------------------------------------------------
void get_message_window (SrvConnect* /* connect */ )  {
  int rows, cols, row, col;
  upic_get_message_window (&rows, &cols, &row, &col);
  UpiBufferPutInt (AckBuffer, UPIF_GET_MESSAGE_WINDOW);
  UpiBufferPutInt (AckBuffer, rows);
  UpiBufferPutInt (AckBuffer, cols);
  UpiBufferPutInt (AckBuffer, row);
  UpiBufferPutInt (AckBuffer, col);
}

//--------------------------------------------------------------------------
void change_titles (SrvConnect* connect )  {
  int menu_id;
  char* text1, *text2, *text3;
  UpiBufferGetInt (GetBuffer, &menu_id);
  UpiBufferGetText (GetBuffer, &text1);
  UpiBufferGetText (GetBuffer, &text2);
  UpiBufferGetText (GetBuffer, &text3);
  if ((menu_id = find_remote_id (connect, menu_id)))  {
    upic_change_titles (menu_id, text1, text2, text3);
  }
}

//--------------------------------------------------------------------------
void modify_param (SrvConnect* connect)   {
  int menu_id, item_id;
  Param* p;

  UpiBufferGetInt (GetBuffer, &menu_id);
  if (!(menu_id = find_remote_id (connect, menu_id))) return;

  UpiBufferGetInt (GetBuffer, &item_id);
  UpiBufferGetParam (GetBuffer, &p);

  switch (p->type)  {
  case REAL_FMT:
    upic_modify_param (menu_id, item_id, p->id,
      p->val.d, p->min.d, p->max.d,
      p->list, p->list_size, p->flag);
    break;
  default:
    if (p->type == ASC_FMT) p->val.c = p->buf;
    upic_modify_param (menu_id, item_id, p->id,
      p->val.i, p->min.i, p->max.i,
      p->list, p->list_size, p->flag);
    break;
  }
}

//--------------------------------------------------------------------------
void refresh_param_page (SrvConnect* connect)  {
  int menu_id;
  char* buffer;
  UpiBufferGetInt (GetBuffer, &menu_id);
  UpiBufferGetBytes (GetBuffer, &buffer, 0);
  if ((menu_id = find_remote_id (connect, menu_id)))  {
    if (buffer) upic_retreive_vars (menu_id, 0, buffer);
    upic_refresh_param_page (menu_id);
  }
}

//--------------------------------------------------------------------------
void refresh_param_line (SrvConnect* connect)  {
  int menu_id, item_id;
  char* buffer;
  UpiBufferGetInt (GetBuffer, &menu_id);
  UpiBufferGetInt (GetBuffer, &item_id);
  UpiBufferGetBytes (GetBuffer, &buffer, 0);
  if ((menu_id = find_remote_id (connect, menu_id)))  {
    if (buffer) upic_retreive_vars (menu_id, item_id, buffer);
    upic_refresh_param_line (menu_id, item_id);
  }
}

//--------------------------------------------------------------------------
void set_cursor (SrvConnect* connect)  {
  SrvConnect* c;
  int menu_id, item_id, param_id, rid;
  UpiBufferGetInt (GetBuffer, &menu_id);
  UpiBufferGetInt (GetBuffer, &item_id);
  UpiBufferGetInt (GetBuffer, &param_id);

  if (Cursor_locked) return;  
  if (UPIsystem->menu.cur != 0)  {
    if ( (c = find_connect_with_id (UPIsystem->menu.cur->id, &rid)) )    {
      c->current_menu   = UPIsystem->menu.cur->id;
      if (UPIsystem->item.cur != 0)      {
        c->current_command = UPIsystem->item.cur->id;
      }
      if (UPIsystem->param.cur != 0)      {
        c->current_param = UPIsystem->param.cur->id;
      }
    }
  }
  if ((menu_id = find_remote_id (connect, menu_id)))  {
    if (c == connect)  {
      upic_set_cursor (menu_id, item_id, param_id);
    }
    else    {
      upic_set_cursor_and_mark (menu_id, item_id, param_id, 0);
    }
    connect->current_menu   = menu_id;
    connect->current_command = item_id;
    connect->current_param = param_id;
  }
}

//--------------------------------------------------------------------------
void save_screen (SrvConnect* /* connect */)    {
  char* t;
  UpiBufferGetText (GetBuffer, &t);
  Kbd_connect* c = find_kbd_connect (t);
  c->chan = upic_net_open_mbx (c->name);
  c->lun = open (c->name, O_RDWR, 7);
  c->f   = fopen (c->name, "w");
  Event_kbd = EVENT_KBD + c->lun;
  rearm_kbd_mbx (Event_kbd, c->chan);
  //wtc_subscribe (Event_kbd, 0, kbd_handler);
}

//--------------------------------------------------------------------------
void restore_screen (SrvConnect* /* connect */)   {
  char* t;
  UpiBufferGetText (GetBuffer, &t);
  upic_back_space (KEYBOARD_MENU);
  Kbd_connect* c = find_old_kbd_connect (t);
  if (c)  {
    upic_net_close_mbx (c->chan);
    close  (c->lun);
    fclose (c->f);
    kill_kbd_connect (c);
  }
}

//--------------------------------------------------------------------------
void begin_update (SrvConnect* /* connect */ )  {
}

//--------------------------------------------------------------------------
void end_update (SrvConnect* /* connect */ )  {
}

//--------------------------------------------------------------------------
void init_remote (SrvConnect* connect )  {
  connect->connected = 1;
}

//--------------------------------------------------------------------------
void quit (SrvConnect* connect)  {
  Var* v;
  Menu_list* m;
  Histo_list* h;
  /*
  sprintf (temp, "SERVER> Disconnecting source [%s]", connect->source);
  upic_write_message (temp, "");
  */

  while ((v = connect->var.first))  {
    if (v->type == ASC_FMT) free (v->value.c);
    list_remove_entry ((Link*) v);
  }

  upic_begin_update();  

  if ((m = connect->menu_list.first))
    upic_delete_command (MAIN_MENU, m->local_id);

  while ((m = connect->menu_list.first))  {
    upic_delete_menu (m->local_id);
    list_remove_entry ((Link*) m);
  }

  while ((h = connect->histo_list.first))
  {
    upic_unbook_histo (h->local_id);
    list_remove_entry ((Link*) h);
  }

  list_remove_entry ((Link*) connect);
  upic_end_update();
}

//--------------------------------------------------------------------------
void open_window (SrvConnect* /* connect */)  {
  /*  upic_open_window ();  */
}

//--------------------------------------------------------------------------
void open_old_window (SrvConnect* connect)  {
  int menu_id;
  UpiBufferGetInt (GetBuffer, &menu_id);
  if ((menu_id = find_remote_id (connect, menu_id)))  {
    upic_open_old_window (menu_id);
  }
}

//--------------------------------------------------------------------------
void set_window_position (SrvConnect* connect)  {
  int menu_id, row, col;
  UpiBufferGetInt (GetBuffer, &menu_id);
  UpiBufferGetInt (GetBuffer, &col);
  UpiBufferGetInt (GetBuffer, &row);
  if ((menu_id = find_remote_id (connect, menu_id)))  {
    upic_set_window_position (menu_id, col, row);
  }
}

//--------------------------------------------------------------------------
void get_window_position (SrvConnect* connect)  {
  int menu_id, row, col;
  UpiBufferGetInt (GetBuffer, &menu_id);
  if ((menu_id = find_remote_id (connect, menu_id)))  {
    upic_get_window_position (menu_id, &col, &row);
    UpiBufferPutInt (AckBuffer, UPIF_GET_WINDOW_POSITION);
    UpiBufferPutInt (AckBuffer, col);
    UpiBufferPutInt (AckBuffer, row);
  }
}

//--------------------------------------------------------------------------
void get_window_size (SrvConnect* connect)  {
  int menu_id, row, col;
  UpiBufferGetInt (GetBuffer, &menu_id);
  if ((menu_id = find_remote_id (connect, menu_id)))  {
    upic_get_window_size(menu_id, &col, &row);
    UpiBufferPutInt (AckBuffer, UPIF_GET_WINDOW_SIZE);
    UpiBufferPutInt (AckBuffer, col);
    UpiBufferPutInt (AckBuffer, row);
  }
}


//--------------------------------------------------------------------------
void attach_pf1 (SrvConnect* connect)  {
  int menu_id;
  UpiBufferGetInt (GetBuffer, &menu_id);
  if ((menu_id = find_remote_id (connect, menu_id)))  {
    upic_attach_pf1 (menu_id);
  }
}

//--------------------------------------------------------------------------
void cancel_notice (SrvConnect* /* connect */ )  {
  upic_cancel_notice();
}

//--------------------------------------------------------------------------
void get_items_per_page (SrvConnect* /* connect */ )   {
  int num = upic_get_items_per_page();
  UpiBufferPutInt (AckBuffer, num);
}

//--------------------------------------------------------------------------
void set_items_per_page (SrvConnect* /* connect */ )  {
  int num;
  UpiBufferGetInt (GetBuffer, &num);
  upic_set_items_per_page (num);
}

//--------------------------------------------------------------------------
void get_mode (SrvConnect* /* connect */ ) {
  int mode = upic_get_mode ();
  UpiBufferPutInt (AckBuffer, mode);
}

//--------------------------------------------------------------------------
void set_mode (SrvConnect* /* connect */ )   {
  int mode;
  UpiBufferGetInt (GetBuffer, &mode);
  upic_set_mode (mode);
}

//--------------------------------------------------------------------------
void register_on_keypad (SrvConnect* /* connect */ )  {
  int menu, item, key;
  UpiBufferGetInt (GetBuffer, &menu);
  UpiBufferGetInt (GetBuffer, &item);
  UpiBufferGetInt (GetBuffer, &key);
  upic_register_on_keypad (menu, item, key);
}

//--------------------------------------------------------------------------
void show_notice (SrvConnect* /* connect */ )  {
  char* title;
  int lines;
  const char** text;
  UpiBufferGetText (GetBuffer, &title);
  UpiBufferGetInt (GetBuffer, &lines);
  UpiBufferGetList (GetBuffer, ASC_FMT, (void**) &text, lines);
  upic_show_notice (title, lines, text);
}

//--------------------------------------------------------------------------
void show_warning (SrvConnect* /* connect */ )   {
  int lines;
  const char** text;
  UpiBufferGetInt (GetBuffer, &lines);
  UpiBufferGetList (GetBuffer, ASC_FMT, (void**) &text, lines);
  upic_show_warning (lines, text);
  UpiBufferPutInt (AckBuffer, UPIF_SHOW_WARNING);
}

//--------------------------------------------------------------------------
void connect_process (SrvConnect* /* connect */ )    {
  char* name;
  UpiBufferGetText (GetBuffer, &name);
  UpiBufferPutInt (AckBuffer, UPIF_RECONNECT);
  server_send_message (name);
}


//--------------------------------------------------------------------------
void disconnect_process (SrvConnect* /* connect */ )  {
  char* name;
  UpiBufferGetText (GetBuffer, &name);
  UpiBufferPutInt (AckBuffer, UPIF_DISCONNECT_PROCESS);
  server_send_message (name);
}

//--------------------------------------------------------------------------
void lock_cursor (SrvConnect* /* connect */ )  {
  if (!Cursor_locked)  {
    Cursor_locked = 1;
    upic_replace_command (MAIN_MENU, C_LOCK, "Unlock the cursor", "");
  }
}

//--------------------------------------------------------------------------
void unlock_cursor (SrvConnect* /* connect */ )  {
  if (Cursor_locked)  {
    Cursor_locked = 0;
    upic_replace_command (MAIN_MENU, C_LOCK, "Lock the cursor", "");
  }
}

//--------------------------------------------------------------------------
void dldec (SrvConnect* /* connect */ )  {
  char* prompt;
  int def, value, min, max;
  UpiBufferGetText (GetBuffer, &prompt);
  UpiBufferGetInt (GetBuffer, &def);
  UpiBufferGetInt (GetBuffer, &min);
  UpiBufferGetInt (GetBuffer, &max);
  if (upic_dldec (prompt, def, &value, min, max) != UPI_SS_NORMAL)
    upic_dlend();

  UpiBufferPutInt (AckBuffer, UPIF_DLDEC);
  UpiBufferPutInt (AckBuffer, value);
}

//--------------------------------------------------------------------------
void dlend (SrvConnect* /* connect */ )  {
  upic_dlend ();
}

//--------------------------------------------------------------------------
void dlhex (SrvConnect* /* connect */ )  {
  char* prompt;
  int def;
  int value;
  int min, max;

  UpiBufferGetText (GetBuffer, &prompt);
  UpiBufferGetInt (GetBuffer, &def);
  UpiBufferGetInt (GetBuffer, &min);
  UpiBufferGetInt (GetBuffer, &max);

  if (upic_dlhex (prompt, def, &value, min, max) != UPI_SS_NORMAL)
    upic_dlend();

  UpiBufferPutInt (AckBuffer, UPIF_DLDEC);
  UpiBufferPutInt (AckBuffer, value);
}

//--------------------------------------------------------------------------
void dlhead (SrvConnect* /* connect */ )  {
  char* title;
  int lines, cols;

  UpiBufferGetText (GetBuffer, &title);
  UpiBufferGetInt (GetBuffer, &lines);
  UpiBufferGetInt (GetBuffer, &cols);
  upic_dlhead (title, lines, cols);
}

//--------------------------------------------------------------------------
void dlkey (SrvConnect* /* connect */ )  {
  if (upic_dlkey () != UPI_SS_NORMAL)
    upic_dlend();

  UpiBufferPutInt (AckBuffer, UPIF_DLDEC);
  UpiBufferPutInt (AckBuffer, 0);
}

//--------------------------------------------------------------------------
void dlm32 (SrvConnect* /* connect */ )  {
  char* prompt;
  int def, value;

  UpiBufferGetText (GetBuffer, &prompt);
  UpiBufferGetInt (GetBuffer, &def);

  if (upic_dlm32 (prompt, def, &value) != UPI_SS_NORMAL)
    upic_dlend();

  UpiBufferPutInt (AckBuffer, UPIF_DLDEC);
  UpiBufferPutInt (AckBuffer, value);
}

//--------------------------------------------------------------------------
void dlmask (SrvConnect* /* connect */ )  {
  char* prompt;
  int def, value;

  UpiBufferGetText (GetBuffer, &prompt);
  UpiBufferGetInt (GetBuffer, &def);

  if (upic_dlmask (prompt, def, &value) != UPI_SS_NORMAL)
    upic_dlend();

  UpiBufferPutInt (AckBuffer, UPIF_DLDEC);
  UpiBufferPutInt (AckBuffer, value);
}

//--------------------------------------------------------------------------
void dloct (SrvConnect* /* connect */ )  {
  char* prompt;
  int def, value, min, max;

  UpiBufferGetText (GetBuffer, &prompt);
  UpiBufferGetInt (GetBuffer, &def);
  UpiBufferGetInt (GetBuffer, &min);
  UpiBufferGetInt (GetBuffer, &max);

  if (upic_dloct (prompt, def, &value, min, max) != UPI_SS_NORMAL)
    upic_dlend();

  UpiBufferPutInt (AckBuffer, UPIF_DLDEC);
  UpiBufferPutInt (AckBuffer, value);
}

//--------------------------------------------------------------------------
void dltxt (SrvConnect* /* connect */ )  {
  char *prompt, *def, *value;
  int length, ret_len;
  UpiBufferGetText (GetBuffer, &prompt);
  UpiBufferGetText (GetBuffer, &def);
  UpiBufferGetInt (GetBuffer, &length);
  value = (char*) list_malloc (length + 1);

  if (upic_dltxt (prompt, def, value, length, &ret_len) != UPI_SS_NORMAL)
    upic_dlend();

  UpiBufferPutInt (AckBuffer, UPIF_DLTXT);
  UpiBufferPutText (AckBuffer, value);
  UpiBufferPutInt (AckBuffer, ret_len);
  free (value);
}

//--------------------------------------------------------------------------
void dlout (SrvConnect* /* connect */ )  {
  char* text;
  int var1, var2, nvar;
  UpiBufferGetText (GetBuffer, &text);
  UpiBufferGetInt (GetBuffer, &var1);
  UpiBufferGetInt (GetBuffer, &var2);
  UpiBufferGetInt (GetBuffer, &nvar);
  upic_dlout (text, var1, var2, nvar);
}

//--------------------------------------------------------------------------
void dlouto (SrvConnect* /* connect */ )  {
  char* text;
  int var1, var2, nvar;
  UpiBufferGetText (GetBuffer, &text);
  UpiBufferGetInt (GetBuffer, &var1);
  UpiBufferGetInt (GetBuffer, &var2);
  UpiBufferGetInt (GetBuffer, &nvar);
  upic_dlouto (text, var1, var2, nvar);
}

//--------------------------------------------------------------------------
void dloutx (SrvConnect* /* connect */ )  {
  char* text;
  int var1, var2, nvar;
  UpiBufferGetText (GetBuffer, &text);
  UpiBufferGetInt (GetBuffer, &var1);
  UpiBufferGetInt (GetBuffer, &var2);
  UpiBufferGetInt (GetBuffer, &nvar);
  upic_dlouto (text, var1, var2, nvar);
}

//--------------------------------------------------------------------------
void dlyeno (SrvConnect* /* connect */ )  {
  char* prompt;
  int def, value;
  UpiBufferGetText (GetBuffer, &prompt);
  UpiBufferGetInt (GetBuffer, &def);
  if (upic_dlyeno (prompt, def, &value) != UPI_SS_NORMAL)
    upic_dlend();
  UpiBufferPutInt (AckBuffer, UPIF_DLDEC);
  UpiBufferPutInt (AckBuffer, value);
}

//--------------------------------------------------------------------------
void back_space (SrvConnect* /* connect */ )  {
  int menu_id;
  UpiBufferGetInt (GetBuffer, &menu_id);
  upic_back_space (menu_id);
}

//--------------------------------------------------------------------------
void drag_histo (int row, int col) {
  if (SrvConnect_of_histo)  {
    UpiBufferPutInt (AckBuffer, UPIF_MOVING_HISTO);
    UpiBufferPutInt (AckBuffer, row);
    UpiBufferPutInt (AckBuffer, col);
    server_send_message (SrvConnect_of_histo->source);
  }
}

//--------------------------------------------------------------------------
void book_histo (SrvConnect* connect)  {
  char* text;
  double min, max;
  int histo, bins, rows;
  UpiBufferGetInt    (GetBuffer, &histo);
  UpiBufferGetText   (GetBuffer, &text);
  UpiBufferGetInt    (GetBuffer, &bins);
  UpiBufferGetInt    (GetBuffer, &rows);
  UpiBufferGetDouble (GetBuffer, &min);
  UpiBufferGetDouble (GetBuffer, &max);
  Histo *h = (Histo*) upic_book_histo (text, bins, rows, min, max);
  new_remote_histo (connect, histo, h);
}

//--------------------------------------------------------------------------
void unbook_histo (SrvConnect* connect)  {
  int histo;
  Histo* h;
  UpiBufferGetInt (GetBuffer, &histo);
  if ((h = find_remote_histo (connect, histo)))  {
    upic_unbook_histo (h);
  }
}

//--------------------------------------------------------------------------
void scale_histo (SrvConnect* connect)  {
  int histo;
  double maxw;
  Histo* h;
  UpiBufferGetInt (GetBuffer, &histo);
  UpiBufferGetDouble (GetBuffer, &maxw);
  if ((h = find_remote_histo (connect, histo)))  {
    upic_scale_histo (h, maxw);
  }
}

//--------------------------------------------------------------------------
void unscale_histo (SrvConnect* connect)  {
  int histo;
  Histo* h;
  UpiBufferGetInt (GetBuffer, &histo);
  if ((h = find_remote_histo (connect, histo)))  {
    upic_unscale_histo (h);
  }
}

//--------------------------------------------------------------------------
void fill_histo (SrvConnect* connect)  {
  int histo;
  double x, w;
  Histo* h;
  UpiBufferGetInt (GetBuffer, &histo);
  UpiBufferGetDouble (GetBuffer, &x);
  UpiBufferGetDouble (GetBuffer, &w);
  if ((h = find_remote_histo (connect, histo)))  {
    upic_fill_histo (h, x, w);
  }
}

//--------------------------------------------------------------------------
void display_histo (SrvConnect* connect)  {
  int histo;
  int row, col;
  Histo* h;
  UpiBufferGetInt (GetBuffer, &histo);
  UpiBufferGetInt (GetBuffer, &row);
  UpiBufferGetInt (GetBuffer, &col);
  if ((h = find_remote_histo (connect, histo)))  {
    upic_display_histo (h, row, col);
  }
}

//--------------------------------------------------------------------------
void hide_histo (SrvConnect* connect)  {
  int histo;
  Histo* h;
  UpiBufferGetInt (GetBuffer, &histo);
  if ((h = find_remote_histo (connect, histo)))  {
    upic_hide_histo (h);
  }
}

//--------------------------------------------------------------------------
void move_histo (SrvConnect* connect)  {
  int histo, row, col;
  Histo* h;
  UpiBufferGetInt (GetBuffer, &histo);
  UpiBufferGetInt (GetBuffer, &row);
  UpiBufferGetInt (GetBuffer, &col);
  if ((h = find_remote_histo (connect, histo)))  {
    upic_move_histo (h, row, col);
  }
}

//--------------------------------------------------------------------------
void moving_histo (SrvConnect* connect)  {
  int histo;
  Histo* h;
  UpiBufferGetInt (GetBuffer, &histo);
  if ((h = find_remote_histo (connect, histo)))  {
    SrvConnect_of_histo = connect;
    upic_moving_histo (h);
  }
}

//--------------------------------------------------------------------------
void copy_histo (SrvConnect* connect)  {
  int histoId;
  Histo* histo;
  UpiBufferGetInt (GetBuffer, &histoId);
  UpiBufferGetHisto (GetBuffer, &histo);
  new_remote_histo (connect, histoId, histo);
}

//--------------------------------------------------------------------------
void callback_handler (int menu_id, int item_id, int condition,
                       SrvConnect* connect)
{
  int remote_id;
  if (find_connect_with_id (menu_id, &remote_id) != connect) return;
  if (condition & CALL_ON_BACK_SPACE)  {
    connect->current_menu   = menu_id;
    connect->current_command = item_id;
    connect->current_param = 0;
  }
  UpiBufferPutInt (AckBuffer, UPIF_DECLARE_CALLBACK);
  UpiBufferPutInt (AckBuffer, remote_id);
  UpiBufferPutInt (AckBuffer, item_id);
  UpiBufferPutInt (AckBuffer, condition);
  server_send_message (connect->source);
}

void AnyBSCallback (int menu_id, int item_id, int /* condition */, SrvConnect* /* c */) {
  int remote_id;
  SrvConnect* c = find_connect_with_id (menu_id, &remote_id);
  if (c == (SrvConnect*)0 ) return;  {
    c->current_menu   = menu_id;
    c->current_command = item_id;
    c->current_param = 0;
  }
}

//--------------------------------------------------------------------------
void PF1_callback(int menu_id, int item_id, int /* condition */, SrvConnect* /* c */) {
  int remote_id;
  SrvConnect* c = find_connect_with_id (menu_id, &remote_id);
  if (c)  {
    c->current_menu   = menu_id;
    c->current_command = item_id;
    c->current_param = 0;
  }
  return;
}

//--------------------------------------------------------------------------
void PF3_callback(int menu_id, int item_id, int /* condition */, SrvConnect* /* c */) {
  int remote_id;
  SrvConnect* c = find_connect_with_id (menu_id, &remote_id);
  if (c)  {
    c->current_menu   = menu_id;
    c->current_command = item_id;
    c->current_param = 0;
  }
  return;
}

//--------------------------------------------------------------------------
void declare_callback (SrvConnect* connect)  {
  int menu_id, condition;
  UpiBufferGetInt (GetBuffer, &menu_id);
  UpiBufferGetInt (GetBuffer, &condition);
  if ((menu_id = find_remote_id (connect, menu_id)))  {
    upic_declare_callback (menu_id, condition, 
      (Routine) callback_handler, (int) connect);
  }
}

//--------------------------------------------------------------------------
void hide_menu (SrvConnect* connect) {
  int menu_id;
  UpiBufferGetInt (GetBuffer, &menu_id);
  if ((menu_id = find_remote_id (connect, menu_id)))  {
    upic_hide_menu (menu_id);
  }
}

//--------------------------------------------------------------------------
Histo* fetch_histo () {
  Histo* histo;
  UpiBufferGetHisto (GetBuffer, &histo);
  return (upic_copy_histo (histo));
}

//--------------------------------------------------------------------------
void fetch_menu (Menu* menu, SrvConnect* connect) {
  Page* page;
  Item* item;
  int father, first, old;
  if (menu->type == NORMAL_MENU) 
    old = find_menu_from_window (connect, menu->window);

  if (!(menu->id = new_remote_id (connect, menu->id, menu->window))) return;

  father = menu->from.menu;  
  if (father && !(father = find_remote_id (connect, father))) return;
  menu->from.menu = father;

  first = check_first_menu (connect, menu);
  switch (menu->type)  {
  case NORMAL_MENU:
    if (!first)    {
      if (old) upic_open_old_window(old);
      else upic_open_window();
    }
    upic_open_menu (menu->id, menu->from.menu, menu->from.item,
      menu->mn_title, menu->up_title, menu->bt_title);
    break;
  case DETACHED_MENU:
    upic_open_detached_menu (menu->id, menu->from.menu, menu->from.item,
      menu->mn_title, menu->up_title, menu->bt_title);
    break;
  case PARAMETER_PAGE:
    upic_open_param (menu->id, menu->from.menu, menu->from.item,
      menu->mn_title, menu->up_title, menu->bt_title);
    break;
  case PULLDOWN_MENU:
    upic_open_pulldown_menu (menu->id, menu->from.menu, menu->from.item,
      menu->mn_title, menu->up_title, menu->bt_title);
    break;
  }

  page = menu->page.first;
  item = page->item.first;
  while (item)  {
    fetch_item (item, connect);
    if (item->enabled) 
      upic_add_command (item->id, item->string, item->help);
    else 
      upic_add_comment (item->id, item->string, item->help);
    item = item->next;
  }
  upic_close_menu();

  if (menu->condition)  {
    upic_declare_callback(menu->id,menu->condition,(Routine)callback_handler,(int)connect);
  }
  if (first)  {
    int maxcol=1;
    connect->current_menu   = menu->id;
    connect->current_command = 0;
    connect->current_param = 0;
    for(SrvConnect* c = Sys.connect.first; c; c=c->next) {
      if (c != connect)      {
        Menu_list *m    = c->menu_list.first;
        if (m != 0)    {
          int row,col,w,h,mc;
          upic_get_window_position(m->local_id,&col,&row);
          upic_get_window_size(m->local_id,&w,&h);
          mc	= w+col+1;
          if (mc > maxcol)  {
            maxcol	= mc;
          }
        }
      }
      upic_set_window_position(menu->id,maxcol,1);
    }
    if (restoring_configuration == 1)    {
      wtc_insert(WT_FACILITY_USER1,0);
    }
  }
  if (first && !connect->prev && !connect->next)  {
    upic_set_cursor (menu->id, 0, 0);
  }
}

/*----------------------------------------------------------------------------*/
void fetch_item (Item* item, SrvConnect* connect)  {
  for( Param* p = item->param.first; p; p=p->next ) {
    fetch_param (p, connect);
  }
}

//--------------------------------------------------------------------------
void fetch_param (Param* param, SrvConnect* connect)  {
  void* varAddress;
  Var* v = find_var (connect, param->var);
  v->type = param->type;
  switch (param->type)
  {
  case ASC_FMT:
    v->size = param->size;
    v->value.c = (char*) list_malloc (param->size + 1);
    strcpy (v->value.c, param->buf);
    varAddress = v->value.c;
    param->val.c = (char*)varAddress;
    break;
  case REAL_FMT:
    v->size = sizeof (double);
    v->value.d = param->val.d;
    varAddress = &v->value.d;
    break;
  default:
    v->size = sizeof (int);
    v->value.i = param->val.i;
    varAddress = &v->value.i;
    break;
  }

  switch (param->type)  {
  case REAL_FMT:
    upic_set_param (varAddress, param->id, param->format,
      param->val.d, param->min.d, param->max.d,
      param->list, param->list_size, param->flag);
    break;
  default:
    upic_set_param (varAddress, param->id, param->format,
      param->val.i, param->min.i, param->max.i,
      param->list, param->list_size, param->flag);
    break;
  }
}

//--------------------------------------------------------------------------
int server_send_message (char* source)    {
  return UpiBufferSendToName (AckBuffer, source);
}

//--------------------------------------------------------------------------
void upper_case (char* name)  {
  if (name)  {
    for (;*name;name++) *name = toupper(*name);
  }
}

//--------------------------------------------------------------------------
void start (const char* /* name */, const char* /* node */ )   {
  int status = 0;
  //status = s_create_process (&name, &vms, Error_message, p_node, 0, p0, p1, p2, p3);
  if (!(status & 1)) upic_signal_error (status, Error_message);
  else upic_write_message (Error_message, "");
}

//--------------------------------------------------------------------------
void kill_process (const char* /* name */ )  {
  int status = 0;
  //status = s_kill_process (&dname);
  if (!(status & 1)) upic_signal_error (status, "");
  else upic_write_message ("Process killed", "");
}

//--------------------------------------------------------------------------
int exist (const char* /* name */ )  {
  int status = 0;
  //status = s_proc_exist(&name, &Dvms);
  return (status & 1);
}

//--------------------------------------------------------------------------
void scheinit (int /* sid */, const char* /* name */)  {
  //int lsid = sid;
  int status = 1;
  //status = s_init ();
  //s_set_termserver (&dname);
  if (!(status & 1)) exit(status);
}

//--------------------------------------------------------------------------
int get_pid ()  {
  return lib_rtl_pid();
}

//--------------------------------------------------------------------------
void get_pname (char* name) {
  lib_rtl_get_process_name(name,32);
}

//--------------------------------------------------------------------------
int cut_blanks (const char* buf)  {
  int len = strlen(buf);
  buf += len - 1;
  while (len && *buf == ' ')  {
    len--;
    buf--;
  }
  return len;
}

//--------------------------------------------------------------------------
void get_my_node () {
  lib_rtl_get_node_name(My_node,sizeof(My_node));
}

//--------------------------------------------------------------------------
int kbd_handler (unsigned int event, void* )  {
  Kbd_request* r;

  r = (Kbd_request*) list_add_entry ((Linked_list*) &Mbx_header.requests.first,
    sizeof(Kbd_request));
  r->lun = event - EVENT_KBD;

  if (!r->prev)   {
    Kbd_connect* c = find_kbd_connect_with_lun (r->lun);
    upic_change_titles (2, c->name, "", "");
  }
  upic_set_cursor (KEYBOARD_MENU, C_INPUT, 0);
  return (1);
}

//--------------------------------------------------------------------------
Kbd_connect* find_old_kbd_connect (const char* name)    {
  for(Kbd_connect* c = Mbx_header.connects.first;c;c=c->next)  {
    if (!strcmp (c->name, name)) return (c);
  }
  return 0;
}

//--------------------------------------------------------------------------
Kbd_connect* find_kbd_connect (const char* name)  {
  Kbd_connect* c = find_old_kbd_connect (name);
  if (!c)  {
    c = (Kbd_connect*)list_add_entry(&Mbx_header.connects.first,sizeof(Kbd_connect));
    c->name = (char*) list_malloc (strlen(name) + 1);
    strcpy (c->name, name);
    c->lun = -1;
  }
  return (c);
}

//--------------------------------------------------------------------------
Kbd_connect* find_kbd_connect_with_lun (int lun)  {
  for(Kbd_connect* c = Mbx_header.connects.first;c;c=c->next)  {
    if (c->lun == lun) return (c);
  }
  return 0;
}

//--------------------------------------------------------------------------
void kill_kbd_connect (Kbd_connect* c)  {
  if (c->name) free (c->name);
  int lun = c->lun;
  Kbd_request* r = Mbx_header.requests.first;
  while (r)  {
    Kbd_request* rr = r->next;
    if (r->lun == lun) list_remove_entry ((Link*) r);
    r = rr;
  }

  list_remove_entry ((Link*) c);
}

//---------------------------------------------------------------------------
void rearm_scr_mbx (int /* event */ )    {
  int status = 1;
  // rearm terminal screen 
  if (!(status & 1)) exit (status);
}

//--------------------------------------------------------------------------
void ast_scr (int arg)  {
  int event = arg;
  wtc_insert (event);
}

//--------------------------------------------------------------------------
void rearm_kbd_mbx (int /* event */, int /* chan */)    {
  // rearm keyboard
  int status = 1;
  if (!(status & 1)) exit (status);
}

//--------------------------------------------------------------------------
void ast_kbd (int arg)    {
  int event = arg;
  wtc_insert (event);
}

//--------------------------------------------------------------------------
void log_message (const char* /* t1 */, const char* /* t2 */ ) {
  //server_log( t1, t2 );
}

//--------------------------------------------------------------------------
void log_reset () {
  const char* log_file = ::getenv ("SERVER_LOG_FILE");
  if (!log_file) return;
  FILE* f = fopen (log_file, "w");
  if (!f) return;
  fclose (f);
}

//--------------------------------------------------------------------------
void log_show ()    {
  int len;
  char msg[1024];
  const char* log_file = (char*) getenv ("SERVER_LOG_FILE");
  if (log_file)  {
    strcpy(msg,log_file);
    len = cut_blanks (msg);
    msg[len] = 0;
    sprintf (Text, "Current active LogFile : %s", msg);
    upic_write_message (Text, "");
  }
}

//--------------------------------------------------------------------------
void log_get_name ()    {
  const char* log_file = (char*) getenv ("SERVER_LOG_FILE");
  if (log_file) strcpy (LogFile_name, log_file);
  else strcpy (LogFile_name, "");
}

//--------------------------------------------------------------------------
void log_set_name (const char* /* name */ )    {
  log_show ();
}

//--------------------------------------------------------------------------
void database_dump ()   {
  const char* log_file = getenv ("SERVER_LOG_FILE");
  if (!log_file)   {
    log_file	= "SERVER_LOG_FILE";
  }
  char str[255];
  sprintf(str,"Dumping to file %s",log_file);
  upic_write_message(str,"");
  FILE* f = fopen (log_file, "a");
  if (!f)  {
    upic_write_message("cannot open file...","");
    return;
  }

  fprintf (f, "-----------------------------------------\n");
  fprintf (f, "Connections :\n");
  for(SrvConnect* connect = Sys.connect.first; connect;connect=connect->next) {
    fprintf (f, "  Source : %s\n", connect->source);
    fprintf (f, "  State  : ");
    if (connect->disabled) fprintf (f, "Disabled ");
    if (connect->starting) fprintf (f, "Starting ");
    if (connect->connected) fprintf (f, "Connected    ");
    else fprintf (f, "Disconnected ");
    fprintf (f, "\n");
    fprintf (f, "  Menus : \n");
    for(Menu_list* menu = connect->menu_list.first; menu; menu=menu->next) {
      fprintf (f, "    Remote : %6d Local : %6.6d Window : %p\n",
        menu->remote_id, menu->local_id, (void*)menu->window);
    }
    fprintf (f, "  Variables : \n");
    for (Var* var = connect->var.first;var;var=var->next)    {
      fprintf (f, "    Address : %p Type : %d Size : %6.6d ",
        var->reference, var->type, var->size);
      fprintf (f, "Value : ");
      if (var->type == ASC_FMT) fprintf (f, "\"%s\"", var->value.c);
      else if (var->type == REAL_FMT) fprintf (f, "%g", var->value.d);
      else fprintf (f, "%d", var->value.i);
      fprintf (f, "\n");
    }
  }
  fprintf (f, "-----------------------------------------\n");
  fclose (f);
}

void save_conf()  {
  char str[255];
  int i, nconn=0;
  const char *cfile = getenv("SERVER_CONFIG");
  SrvConnect* c   = Sys.connect.first;
  for(; c; c=c->next )  {
    nconn++;
  }
  if (nconn == 0)  {
    return;
  }
  SrvConnection *carr = (SrvConnection*)malloc(nconn*sizeof(SrvConnection));
  if (carr == 0)  {
    return;
  }

  if (cfile == 0)  {
    cfile	= "SERVER_CONFIG";
  }
  FILE* f = fopen (cfile, "w+");
  if (f == 0)  {
    upic_write_message ("Cannot open new configuration file", "");
  }
  c   = Sys.connect.first;
  for(i=0; c; c=c->next )  {
    int row;
    Menu_list *m    = c->menu_list.first;
    if (m)   {
      upic_get_window_position(m->local_id,&carr[i].col,&row);
    }
    else    {
      carr[i].col	= 99999999 ;
    }
    strcpy(carr[i].s,c->source);
    i++;
  }
  nconn = i;
  qsort (carr, nconn, sizeof(SrvConnection), conn_comp);
  for (i=0;i<nconn;i++)  {
    if (carr[i].col == 99999999) {
      fprintf(f,"%s %d\n",carr[i].s,0);
    }
    else {
      fprintf(f,"%s %d\n",carr[i].s,1);
    }
  }
  free (carr);
  fclose (f);
  sprintf(str,"Configuration saved on file %s",cfile);
  upic_write_message(str,"");
  return;
}

int restore_conf(int /* fac */, int /* par */)
{
  char s[64];
  int mflag;
again:
  int status = fscanf(restore_file,"%s %d",s, &mflag);
  if (status != EOF)
  {
    UpiBufferPutInt (AckBuffer, UPIF_RECONNECT);
    status = server_send_message (s);
    if (!(status & 1))  {
      upic_signal_error (status, s);
      goto again;
    }
    else {
      char str[255];
      sprintf(str,"Connecting to %s",s);
      upic_write_message (str,"");
    }
  }
  else  {
    end_restore_conf();
  }
  return 1;
}

void start_restore_conf()   {
  SrvConnect* c = Sys.connect.first;
  if (c)  {
    upic_write_message("There are already connections present",
      "Please EXIT the server, restart and hit 'Restore Configuration' again");
    return;
  }
  const char* cfile = getenv("SERVER_CONFIG");
  if (cfile == 0)  {
    cfile = "sys$login:SERVER_CONFIG";
  }
  restore_file = fopen (cfile, "r");
  if (restore_file == 0)  {
    upic_write_message ("Cannot open configuration file", "");
    return;
  }
  //wtc_subscribe(WT_FACILITY_USER1, 0, (wt_callback_t)restore_conf);
  restoring_configuration = 1;
  //wtc_insert(WT_FACILITY_USER1,0);
}
void end_restore_conf()
{
  wtc_remove(WT_FACILITY_USER1);
  restoring_configuration = 0;
  fclose(restore_file);
  restore_file    = 0;
  return;
}
#endif
