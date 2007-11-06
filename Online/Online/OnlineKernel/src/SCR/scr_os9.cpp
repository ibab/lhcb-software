#ifdef _WIN32
#include <io.h>
#include "RTL/conioex.h"
#else
#include <sys/ioctl.h>
#include <unistd.h>
#include <cerrno>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "SCR/scr.h"
#include "WT/wtdef.h"
#include "NET/IOPortManager.h"

using namespace SCR;

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
static const char* s_termType = ::getenv("TERM");

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
char scrc_top_left_corner()  {
  // Double framed windows in DOS: 0xC9
  return s_termType ? 'l' : char(0xDA);
}
char scrc_top_right_corner()  {
  // Double framed windows in DOS: 0xBB
  return s_termType ? 'k' : char(0xBF);
}
char scrc_bottom_left_corner()  {
  // Double framed windows in DOS: 0xC8
  return s_termType ? 'm' : char(0xC0);
}
char scrc_bottom_right_corner()  {
  // Double framed windows in DOS: 0xBC
  return s_termType ? 'j' : char(0xD9);
}
char scrc_horizontal_bar()  {
  // Double framed windows in DOS: 0xCD
  return s_termType ? 'q' : char(0xC4);
}
char scrc_vertical_bar()  {
  // Double framed windows in DOS: 0xBA
  return s_termType ? 'x' : char(0xB3);
}

//----------------------------------------------------------------------------
int scrc_fputs (Pasteboard *pb)   {
#ifdef _WIN32
  if ( !s_termType )  {
    int row,col,r;
    char* s=pb->bufout;
    char* last = pb->bufout+strlen(pb->bufout);
    for(; s<last; ++s )  {
      if ( *s == ESCAPE )  {
        ++s;
        if ( (r=strncmp(s,"[7m",3))==0 )  {         // inverse
          textattr(BUILD_TEXTATTR(WHITE,BLACK));
          s += 2;
          continue;
        }
        else if ( (r=strncmp(s,"[1m",3))==0 )  {    // bold
          highvideo();
          s += 2;
          continue;
        }
        else if ( (r=strncmp(s,"[27m",4))==0 )  {   // normal
          textattr(BUILD_TEXTATTR(BLACK,WHITE));
          lowvideo();
          s += 3;
          continue;
        }
        else if ( (r=strncmp(s,"[0m",3))==0 )  {    // plain
          textattr(BUILD_TEXTATTR(BLACK,WHITE));
          lowvideo();
          s += 2;
          continue;
        }
        else if ( (r=strncmp(s,"[5m",3))==0 )  {    // flash
          s += 2;
          continue;
        }
        else if ( (r=strncmp(s,"[4;30m",6))==0 )  { // underline
          textattr(BUILD_TEXTATTR(BLACK,LIGHTRED));
          s += 5;
          continue;
        }
        else if ( (r=strncmp(s,"[?25h",5))==0 )  {  // cursor on
          _setcursortype(_SOLIDCURSOR);
          s += 4;
          continue;
        }
        else if ( (r=strncmp(s,"[?25l",5))==0 )  {  // cursor off
          _setcursortype(_NOCURSOR);
          s += 4;
          continue;
        }
        else if ( (r=strncmp(s,"[?3l",4))==0 )  {   // narrow_screen
          s += 3;
          continue;
        }
        else if ( (r=strncmp(s,"[?3h",4))==0 )  {   // wide_screen
          s += 3;
          continue;
        }
        else if ( (r=strncmp(s,"(B",2))==0 )  {     // toascii
          s += 1;
          continue;
        }
        else if ( (r=strncmp(s,"(0",2))==0 )  {     // tographic
          s += 1;
          continue;
        }
        else if ( (r=strncmp(s,")0",2))==0 )  {     // part 2 of setfonts
          s += 1;
          continue;
        }
        else if ( (r=strncmp(s,"*<",2))==0 )  {     // part 3 of setfonts
          s += 1;
          continue;
        }
        else if ( (r=strncmp(s,"+>",2))==0 )  {     // part 4 of setfonts
          s += 1;
          continue;
        }
        else if ( (r=strncmp(s,"=",1))==0 )  {      // setansi
          continue;
        }
        else if ( (r=strncmp(s,">",1))==0 )  {      // touser
          continue;
        }
        else if ( (r=strncmp(s,"<",1))==0 )  {      // tosupp
          continue;
        }
        else if ( (r=strncmp(s,"[2J",3))==0 )  {    // clear screen
          clrscr();
          s += 2;
          continue;
        }
        else if ( 2 == ::sscanf(s,"[%d;%dH",&row,&col) )  {
          gotoxy(col,row);
          s = strchr(s,'H');
          continue;
        }
        write(1,s,1);
        continue;
      }
      write(1,s,1);
    }
    return strlen(pb->bufout);
  }
#endif
  return write(1,pb->bufout, strlen(pb->bufout));
}

/*---------------------------------------------------------------------------*/
int scr_get_console_dimensions(int* rows, int* cols)  {
#ifdef __linux
  int fd = ::fileno(stdin);
  if ( ::isatty(fd) ) {
    struct winsize wns;
    do {
      if(::ioctl(fd,TIOCGWINSZ,&wns) == 0) {
        *rows = wns.ws_row;
        *cols = wns.ws_col;
        break;
      }
    } while (errno == EINTR);   
  } 
#else
  if ( 0 == ::getenv("TERM") )  {
    consolesize(rows, cols);
    return 1;
  }
  if ( ::getenv("COLUMNS") ) {
    if ( 1 != ::sscanf(::getenv("COLUMNS"),"%d",cols) ) *cols = 80;
  }
  if ( ::getenv("LINES") ) {
    if ( 1 != ::sscanf(::getenv("LINES"),"%d",rows) ) *rows = 24;
  }
#endif
  return 1;
}
//---------------------------------------------------------------------------
int scrc_check_key_buffer (char *buffer)
//---------------------------------------------------------------------------
/* This function checks the buffer for a valid escape sequence or a normal   */
/* key stroke.                                                               */
/*  If a valid key sequence is found, the corresponding code is returned.    */
/*  INVALID may be returned.                                                 */
/*  (-1)    is returned if the escape sequence is incomplete.                */
//---------------------------------------------------------------------------
{
  int b;
  char c;

  b = *buffer & 0xff;
  switch (b)  {
  case 0x9b:
    buffer++;
    switch (*buffer)  {
    case 'D': return MOVE_LEFT;
    case 'B': return MOVE_DOWN;
    case 'A': return MOVE_UP;
    case 'C': return MOVE_RIGHT;
    case '1':
      buffer++;
      switch (c = *buffer)  {
      case '~' : return KPD_FIND;
      case '7' :
      case '8' :
      case '9' :
        buffer++;
        switch (*buffer)  {
        case '~' :
          switch (c) {
          case '7' : return F6;
          case '8' : return F7;
          case '9' : return F8;
          }
          break;
        case 0 : return (-1);
        }
        break;
      case 0 : return (-1);
      }
      break;
    case '2':
      buffer++;
      switch (c = *buffer)  {
      case '~' : return KPD_INSERT;
      case '0' :
      case '1' :
      case '3' :
      case '4' :
      case '5' :
      case '6' :
      case '8' :
      case '9' :
        buffer++;
        switch (*buffer) {
        case '~' :
          switch (c) {
          case '0' : return F9;
          case '1' : return F10;
          case '3' : return F11;
          case '4' : return F12;
          case '5' : return F13;
          case '6' : return F14;
          case '8' : return F15;
          case '9' : return F16;
          }
          break;
        case 0 : return (-1);
        }
        break;
      case 0 : return (-1);
      }
      break;
    case '3':
      buffer++;
      switch (c = *buffer)  {
      case '~' : return KPD_REMOVE;
      case '1' :
      case '2' :
      case '3' :
      case '4' :
        buffer++;
        switch (*buffer)  {
        case '~' :
          switch (c) {
          case '1' : return F17;
          case '2' : return F18;
          case '3' : return F19;
          case '4' : return F20;
          }
          break;
        case 0 : return (-1);
        }
        break;
      case 0 : return (-1);
      }
      break;
    case '4':
      buffer++;
      switch (c = *buffer) {
      case '~' : return KPD_SELECT;
      case 0 : return (-1);
      }
      break;
    case '5':
      buffer++;
      switch (c = *buffer) {
      case '~' : return KPD_PREV;
      case 0 : return (-1);
      }
      break;
    case '6':
      buffer++;
      switch (c = *buffer) {
      case '~' : return KPD_NEXT;
      case 0 : return (-1);
      }
      break;
    case 0:
      return (-1);
      break;
    }
    break;
  case 0x8f :
    buffer++;
    switch (*buffer) {
    case 'l': return PAGE_DOWN;
    case 'm': return PAGE_UP;
    case 'n': return KPD_PERIOD;
    case 'p': return KPD_0;
    case 'q': return KPD_1;
    case 'r': return KPD_2;
    case 's': return KPD_3;
    case 't': return KPD_4;
    case 'u': return KPD_5;
    case 'v': return KPD_6;
    case 'w': return KPD_7;
    case 'x': return KPD_8;
    case 'y': return KPD_9;
    case 'M': return KPD_ENTER;
    case 'P': return KPD_PF1;
    case 'Q': return KPD_PF2;
    case 'R': return KPD_PF3;
    case 'S': return KPD_PF4;
    case 0: return (-1);
    }
    break;
  case 0x1b :
    buffer++;
    switch (*buffer)
    {
    case '[':
      buffer++;
      switch (*buffer)
      {
      case 'D': return MOVE_LEFT;
      case 'B': return MOVE_DOWN;
      case 'A': return MOVE_UP;
      case 'C': return MOVE_RIGHT;
      case '1':
        buffer++;
        switch (c = *buffer)
        {
        case '~' : return KPD_FIND;
        case '7' :
        case '8' :
        case '9' :
          buffer++;
          switch (*buffer)
          {
          case '~' :
            switch (c)
            {
            case '7' : return F6;
            case '8' : return F7;
            case '9' : return F8;
            }
          case 0 : return (-1);
          }
          break;
        case 0 : return (-1);
        }
        break;
      case '2':
        buffer++;
        switch (c = *buffer)
        {
        case '~' : return KPD_INSERT;
        case '0' :
        case '1' :
        case '3' :
        case '4' :
        case '5' :
        case '6' :
        case '8' :
        case '9' :
          buffer++;
          switch (*buffer)
          {
          case '~' :
            switch (c)
            {
            case '0' : return F9;
            case '1' : return F10;
            case '3' : return F11;
            case '4' : return F12;
            case '5' : return F13;
            case '6' : return F14;
            case '8' : return F15;
            case '9' : return F16;
            }
            break;
          case 0 : return (-1);
          }
          break;
        case 0 : return (-1);
        }
        break;
      case '3':
        buffer++;
        switch (c = *buffer)
        {
        case '~' : return KPD_REMOVE;
        case '1' :
        case '2' :
        case '3' :
        case '4' :
          buffer++;
          switch (*buffer)
          {
          case '~' :
            switch (c)
            {
            case '1' : return F17;
            case '2' : return F18;
            case '3' : return F19;
            case '4' : return F20;
            }
            break;
          case 0 : return (-1);
          }
          break;
        case 0 : return (-1);
        }
        break;
      case '4':
        buffer++;
        switch (c = *buffer)
        {
        case '~' : return KPD_SELECT;
        case 0 : return (-1);
        }
        break;
      case '5':
        buffer++;
        switch (c = *buffer)
        {
        case '~' : return KPD_PREV;
        case 0 : return (-1);
        }
        break;
      case '6':
        buffer++;
        switch (c = *buffer)
        {
        case '~' : return KPD_NEXT;
        case 0 : return (-1);
        }
        break;
      case 0 :
        return (-1);
        break;
      }
      break;
    case 'O':
      buffer++;
      switch (*buffer)
      {
      case 'l': return PAGE_DOWN;
      case 'm': return PAGE_UP;
      case 'n': return KPD_PERIOD;
      case 'p': return KPD_0;
      case 'q': return KPD_1;
      case 'r': return KPD_2;
      case 's': return KPD_3;
      case 't': return KPD_4;
      case 'u': return KPD_5;
      case 'v': return KPD_6;
      case 'w': return KPD_7;
      case 'x': return KPD_8;
      case 'y': return KPD_9;
      case 'M': return KPD_ENTER;
      case 'P': return KPD_PF1;
      case 'Q': return KPD_PF2;
      case 'R': return KPD_PF3;
      case 'S': return KPD_PF4;
      case 0 : return (-1);
      }
      break;
    case 0:
      return (-1);
      break;
    }
    break;
  case 0x7f :
    return DELETE;

  default:
#ifdef _WIN32
    switch(b)  {
      case 13:   return RETURN_KEY;
      case 16:   return KEY_SHIFT;
      case 17:   return KEY_CTRL;

      case 33:   return KPD_PREV;  // PAGE_UP;
      case 34:   return KPD_NEXT;  // PAGE_DOWN;
      case 35:   return PAGE_DOWN; // END
      case 36:   return PAGE_UP;   // HOME;
      case 37:   return MOVE_LEFT;
      case 38:   return MOVE_UP;
      case 39:   return MOVE_RIGHT;
      case 40:   return MOVE_DOWN;
      //case 45:   return INSERT;
      case 123:  return BACK_SPACE;
      // ? case 115:  return KPD_PF4;
      case 46:   return DELETE;
      case 112:  return DELETE;
      case 144:  return KPD_PF1;
      case 111:  return KPD_PF2;
      case 106:  return KPD_PF3;
      case 109:  return KPD_PF4;
      case 110:  return KPD_PERIOD;
      case 96:   return KPD_0;
      case 97:   return KPD_1;
      case 98:   return KPD_2;
      case 99:   return KPD_3;
      case 100:   return KPD_4;
      case 101:   return KPD_5;
      case 102:   return KPD_6;
      case 103:   return KPD_7;
      case 104:   return KPD_8;
      case 105:   return KPD_9;
      default:   break;
    }
#endif
    if (b < 0x20) return (INVALID + b);
    else if (b <= '~') return b;
  }
  return INVALID;
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
