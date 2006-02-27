#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <cstdio>
#include <cstring>
#include "SCR/scr.h"
#include "WT/wtdef.h"
#include "NET/IOPortManager.h"
static Pasteboard *Kbd = 0;

static char Last_char = 0;
static char Armed = 0;
#define KEY_BUF_SIZE 80
static char Key_buffer[KEY_BUF_SIZE+2] = "";
static int  Key_ptr = 0;
static int  Last_key_stroke = -1;
static int (*User_ast) () = 0;
static int (*User_mouse_handler)(int,int) = 0;
static int (*User_configure_handler)(int,int) = 0;

extern int scr_ignore_input;

//----------------------------------------------------------------------------
#if 0
static int scrc_exit_handler (int* /* status */, Pasteboard *pb)    {
  scrc_cursor_on (pb);
  scrc_end_pasteboard_update (pb);
  wtc_insert_head (Fac_wt_wakeup);
  scrc_delete_pasteboard (pb);
  return 1;
}
#endif
static bool _p=false;
int scrc_handler_keyboard (unsigned int /* fac */, void* /* par */);
//----------------------------------------------------------------------------
int scrc_rearm_keyboard (unsigned int /* fac */, void* /* par */)   {
  if (Armed) return 0;
  Armed = 1;  
#if _OSK
  if (!Insignal)  {
    Insignal = sig_book_signal();
    sig_declare_signal(Insignal, scrc_ast );
  }
  _ss_ssig(0,Insignal);
#else 
  typedef int (*_F)(void*);
  IOPortManager(0).add(0, fileno(stdin), scrc_ast_keyboard, 0);
  //IOPortManager(0).add(0, fileno(stdin), (_F)scrc_handler_keyboard, 0);
#endif
  return WT_SUCCESS;
}

//----------------------------------------------------------------------------
int scrc_ast_keyboard (void*)   {
  if (scr_ignore_input == 0)  {
    wtc_insert (WT_FACILITY_KEYBOARD);
    if (User_ast) (* User_ast) ();
  }
  return WT_SUCCESS;
}

//----------------------------------------------------------------------------
int scrc_handler_keyboard (unsigned int fac, void* /* par */)  {
  int status = 0;
  do  {
    int fd = fileno(stdin);
    status = IOPortManager::getAvailBytes(fd);
    if ( status>0 )  {
      IOPortManager::getChar(fd, &Last_char);
      if (_p)printf("scrc_handler_keyboard[%d, %d]: Got char: %d %02X\n",status,fac,Last_char,Last_char);
      if (Key_ptr >= KEY_BUF_SIZE) status = 0;
      else if (Last_char)      {
        Key_buffer[Key_ptr] = Last_char;
        Key_ptr++;
        Key_buffer[Key_ptr] = 0;
        Last_key_stroke = scrc_check_key_buffer (Key_buffer);
        if (Last_key_stroke > 0)        {
          Key_ptr = 0;
          Key_buffer[Key_ptr] = 0;          
          if (Kbd->moving)          {
            if (scrc_action_moving_display (Kbd, Last_key_stroke)) {
              wtc_insert_head (WT_FACILITY_SCR);  
            }
            else
              Last_key_stroke = -1;
          }
          else if (Kbd->resizing)          {
            if (scrc_action_resizing_display (Kbd, Last_key_stroke)) {
              wtc_insert_head (WT_FACILITY_SCR);
            }
            else
              Last_key_stroke = -1;
          }
          else {
            wtc_insert_head (WT_FACILITY_SCR);
          }
        }
      }
    }
  } while( status > 0);
  Armed = 0;
  return WT_SUCCESS;
}

//----------------------------------------------------------------------------
static int scrc_rearm (unsigned int /* fac */, void* /* param */)  {
  return scrc_fflush(Kbd);
}

//----------------------------------------------------------------------------
int scrc_init_screen (Pasteboard *pb, int rows, int cols)   {
  if (cols) pb->cols = cols;
  if (rows) pb->rows = rows;
  scrc_setANSI();
  if (!Kbd)  {
    Kbd = pb;
    wtc_init();
    wtc_subscribe (WT_FACILITY_KEYBOARD, scrc_rearm_keyboard, scrc_handler_keyboard);
    wtc_subscribe (WT_FACILITY_SCR, scrc_rearm, (wt_callback_t)0);
  }
  return 1;
}

//----------------------------------------------------------------------------
int scrc_get_last_key ()    {
  int key = Last_key_stroke;
  Last_key_stroke = -1;
  if (key < 0) key = 0;
  return (key);
}

//----------------------------------------------------------------------------
int scrc_save_screen_rearm()  {
  return scrc_rearm_keyboard(WT_FACILITY_KEYBOARD, 0);
}

//----------------------------------------------------------------------------
int scrc_fputs (Pasteboard *pb)   {
  return write(1,pb->bufout, strlen(pb->bufout));
}

/*---------------------------------------------------------------------------*/
int scrc_wait (Display *disp)   {
  return scrc_read_keyboard(disp, 1);
}

/*---------------------------------------------------------------------------*/
int scrc_read_keyboard (Display * /*disp */, int wait)  {
  int status, sub_status;
  unsigned int event;
  void* dummy;  
  if (wait) status = wtc_wait (&event,&dummy,&sub_status);
  int key = scrc_get_last_key ();
  if(_p)printf("scrc_read_keyboard: Got char: %d %02X\n",key,key);
  return key;
}

//----------------------------------------------------------------------------
int scrc_read (Display *disp, unsigned char *buffer, int wait)      {
  int sub_status;
  unsigned int c;
  void* dummy;
  
  if (!Last_char)  {
    scrc_fflush(disp->pb);
    if (wait) wtc_wait (&c,&dummy,&sub_status);
  }
  *buffer = Last_char;
  if (Last_char)  {
    Armed = 0;
    Last_char = 0;
  }
  return 1;
}

/*---------------------------------------------------------------------------*/
int scrc_test_input ()    {
  scrc_rearm (0,0);
  if(_p)printf("scrc_test_input\n");
  return (Last_char);
}

//----------------------------------------------------------------------------
int scrc_get_smgid (Pasteboard *pb)   {
  return pb->smgid;
}

//----------------------------------------------------------------------------
int scrc_enable_unsolicited_input (Pasteboard* /* pb */, int (* ast)())   {
  User_ast = ast;
  wtc_subscribe (WT_FACILITY_KEYBOARD, scrc_rearm_keyboard, scrc_handler_keyboard);
  scrc_rearm_keyboard (WT_FACILITY_KEYBOARD,0);
  return 1;
}

//----------------------------------------------------------------------------
int scrc_disable_unsolicited_input (Pasteboard* /* pb */)   {
  wtc_remove (WT_FACILITY_KEYBOARD);
  return 1;
}

//----------------------------------------------------------------------------
int scrc_change_pbd_characteristics (Pasteboard* /* pb */)  {
  return 1;
}

//----------------------------------------------------------------------------
int scrc_delete_physical_pasteboard (Pasteboard* /* pb */)    {
  return 1;
}

/*---------------------------------------------------------------------------*/
void scrc_set_mouse_handler (Pasteboard* /* pb */, int (*handler)(int,int))  {
  User_mouse_handler = handler;
}

/*---------------------------------------------------------------------------*/
void scrc_set_configure_handler (Pasteboard* /* pb */, int (*handler)(int,int))  {
  User_configure_handler = handler;
}

/*---------------------------------------------------------------------------*/
void scrc_iconify_display (Display* /* d */)    {
}

/*---------------------------------------------------------------------------*/
void scrc_uniconify_display (Display* /* d */)    {
}
