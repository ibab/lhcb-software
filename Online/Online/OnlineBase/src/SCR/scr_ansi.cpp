#ifdef _WIN32
#include <io.h>
#include "RTL/conioex.h"
#else
#include <sys/ioctl.h>
#include <unistd.h>
#include <cerrno>
#include "termios.h"
static struct termios neu, alt; 
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
static int (*User_mouse_handler)(Pasteboard*,int,int,int) = 0;
static int (*User_configure_handler)(int,int) = 0;
typedef void (*Key_Handler)(char);
Key_Handler User_Basic_Key_Handler = 0;

extern int scr_ignore_input;
static const char* s_termType = ::getenv("TERM");

//----------------------------------------------------------------------------
#if 0
static int scrc_exit_handler (int* /* status */, Pasteboard *pb)    {
  ::scrc_cursor_on (pb);
  ::scrc_end_pasteboard_update (pb);
  ::wtc_insert_head (Fac_wt_wakeup);
  ::scrc_delete_pasteboard (pb);
  return 1;
}
#endif
static bool _p=false;


void scrc_resetANSI()   {
  int fd = fileno(stdin); 
  ::fflush (stdout);
  ::fputs("\033(B",stdout);
  ::fputs("\033[0m",stdout);    // all attributes off
  ::fputs("\033[?91",stdout);   // Mouse OFF
  ::fputs("\033[?10001",stdout);
  ::fflush (stdout);
  ::fprintf (stdout, "\033>");
  ::fprintf (stdout, "\033c");
  ::fflush (stdout);
#ifdef _WIN32
  ::clrscr();
#else
  ::tcsetattr(fd, TCSANOW, &alt); 
#endif
}

void scrc_setANSI()   {
  /*
   * set terminal eof from <esc> to ^Z,
   * set pause off
   * set echo off 
   * set up intercept to restore terminal parameters
   */ 
  int fd = fileno(stdin); 
  ::fflush (stdout);
  ::fprintf (stdout, "\033=");
  ::fflush (stdout);
#ifndef _WIN32
  ::tcgetattr(fd, &alt); 
  neu = alt; 
  neu.c_lflag &= ~(ICANON|ECHO); 
  neu.c_cc[VMIN] = 1;
  neu.c_cc[VTIME] = 0;
  ::tcsetattr(fd, TCSANOW, &neu); 
#endif
}


int scrc_handler_keyboard (unsigned int /* fac */, void* /* par */);
//----------------------------------------------------------------------------
int scrc_rearm_keyboard (unsigned int /* fac */, void* par)   {
  if (Armed) return 0;
  Armed = 1;  
#if 0 // _OSK
  if (!Insignal)  {
    Insignal = sig_book_signal();
    sig_declare_signal(Insignal, scrc_ast );
  }
  _ss_ssig(0,Insignal);
#else
  IOPortManager(0).addEx(0, ::fileno(stdin), scrc_ast_keyboard, par);
#endif
  return WT_SUCCESS;
}

//----------------------------------------------------------------------------
int scrc_ast_keyboard (void* par)   {
  if (scr_ignore_input == 0)  {
    ::wtc_insert (WT_FACILITY_KEYBOARD, par);
    if (User_ast) (* User_ast) ();
  }
  return WT_SUCCESS;
}

//----------------------------------------------------------------------------
int scrc_set_user_key_handler(Key_Handler handler) {
  User_Basic_Key_Handler = handler;
  return 1;
}

//----------------------------------------------------------------------------
int scrc_handler_keyboard (unsigned int fac, void* par)  {
  int status = 0;
  do  {
    int fd = ::fileno(stdin);
    status = IOPortManager::getAvailBytes(fd);
    if ( status>0 )  {
      IOPortManager::getChar(fd, &Last_char);
      if (_p)printf("scrc_handler_keyboard[%d, %d]: Got char: %d %02X\n",status,int(fac),Last_char,Last_char);
      if ( User_Basic_Key_Handler ) (*User_Basic_Key_Handler)(Last_char);
      if ( User_mouse_handler ) ::scrc_handle_mouse(Kbd,Last_char);
      if (Key_ptr >= KEY_BUF_SIZE) status = 0;
      else if (Last_char)      {
        Key_buffer[Key_ptr] = Last_char;
        Key_ptr++;
        Key_buffer[Key_ptr] = 0;
        Last_key_stroke = ::scrc_check_key_buffer (Key_buffer);
        if (Last_key_stroke > 0)        {
          Key_ptr = 0;
          Key_buffer[Key_ptr] = 0;          
          if (Kbd->moving)          {
            if (::scrc_action_moving_display (Kbd, Last_key_stroke)) {
              ::wtc_insert_head (WT_FACILITY_SCR, par);  
            }
            else
              Last_key_stroke = -1;
          }
          else if (Kbd->resizing)          {
            if (::scrc_action_resizing_display (Kbd, Last_key_stroke)) {
              ::wtc_insert_head (WT_FACILITY_SCR, par);
            }
            else
              Last_key_stroke = -1;
          }
          else {
            //printf("%08X\n",Last_key_stroke);
            ::wtc_insert_head (WT_FACILITY_SCR, par);
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
  return ::scrc_fflush(Kbd);
}

//----------------------------------------------------------------------------
int scrc_init_screen (Pasteboard *pb, int rows, int cols)   {
  if (cols) pb->cols = cols;
  if (rows) pb->rows = rows;
  scrc_setANSI();
  if (!Kbd)  {
    Kbd = pb;
    ::wtc_init();
    ::wtc_subscribe (WT_FACILITY_KEYBOARD, scrc_rearm_keyboard, scrc_handler_keyboard, pb);
    ::wtc_subscribe (WT_FACILITY_SCR, scrc_rearm, (wt_callback_t)0, pb);
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
    static bool s_bold = false;
    static bool s_blink = false;
    static bool s_inverse = false;
    static bool s_underline = false;
    int row,col,r;
    char* s=pb->bufout;
    char* last = pb->bufout+strlen(pb->bufout);
    for(; s<last; ++s )  {
      if ( *s == ESCAPE )  {
        ++s;
        if ( *s == '[' ) {
          ++s;
          if      ( (r=::strncmp(s,"0m",2))==0 )  {    // plain
            if ( s_bold ) { lowvideo(); s_bold = false; }
            if ( s_blink ) { noblinkvideo(); s_blink = false; }
            if ( s_inverse ) { /* noinversevideo(); */ s_inverse = false; }
            if ( s_underline ) { nounderlinevideo(); noblinkvideo(); s_underline = false; }
            ::textattr(BUILD_TEXTATTR(::BLACK,::WHITE));
            ++s;
            continue;
          }
          else if ( (r=::strncmp(s,"0;49m",5))==0 )  { // plain
            if ( s_bold ) { ::lowvideo(); s_bold = false; }
            if ( s_blink ) { ::noblinkvideo(); s_blink = false; }
            if ( s_inverse ) { /* noinversevideo(); */ s_inverse = false; }
            if ( s_underline ) { ::nounderlinevideo(); ::noblinkvideo(); s_underline = false; }
            ::textattr(BUILD_TEXTATTR(::BLACK,::WHITE));
            s += 4;
            continue;
          }
          else if ( (r=::strncmp(s,"27m",3))==0 )  {   // normal
            if ( s_bold ) { ::lowvideo(); s_bold = false; }
            if ( s_blink ) { ::noblinkvideo(); s_blink = false; }
            if ( s_inverse   ) { /* noinversevideo(); */ s_inverse = false; }
            if ( s_underline ) { ::nounderlinevideo(); ::noblinkvideo(); s_underline = false; }
            ::textattr(BUILD_TEXTATTR(::BLACK,::WHITE));
            s += 2;
            continue;
          }
          else if ( (r=::strncmp(s,"7m",2))==0 )  {    // inverse
            //::textattr(BUILD_TEXTATTR(::BLACK,WHITE));
            if ( !s_inverse ) {
              s_inverse = true;
              ::inversevideo();
            }
            ++s;
            continue;
          }
          else if ( (r=::strncmp(s,"1m",2))==0 )  {    // bold
            if ( !s_bold ) {
              s_bold = true;
              ::highvideo();
            }
            ++s;
            continue;
          }
          else if ( (r=::strncmp(s,"5m",2))==0 )  {    // flash
            if ( !s_blink ) {
              s_blink = true;
              ::blinkvideo();
            }
            ++s;
            continue;
          }
          else if ( (r=::strncmp(s,"4;30m",5))==0 )  { // underline
            if ( !s_underline ) {
              // Underline does not work properly on windows.....use Bold/Blink
              ::textattr(BUILD_TEXTATTR(::BLACK,::DARKGRAY));
              ::underlinevideo();
              ::blinkvideo();
              s_underline = true;
              s_blink = true;
            }
            s += 4;
            continue;
          }
          else if ( (r=::strncmp(s,"?25h",4))==0 )  {  // cursor on
            _setcursortype(_SOLIDCURSOR);
            s += 3;
            continue;
          }
          else if ( (r=::strncmp(s,"?25l",4))==0 )  {  // cursor off
            _setcursortype(_NOCURSOR);
            s += 3;
            continue;
          }
          else if ( (r=::strncmp(s,"?3l",3))==0 )  {   // narrow_screen
            s += 2;
            continue;
          }
          else if ( (r=::strncmp(s,"?3h",3))==0 )  {   // wide_screen
            s += 2;
            continue;
          }
          else if ( (r=::strncmp(s,"31m",3))==0 )  {   // fg_red
            ::textcolor(::RED);
            s += 2;
            continue;
          }
          else if ( (r=::strncmp(s,"32m",3))==0 )  {   // fg_green
            ::textcolor(::GREEN);
            s += 2;
            continue;
          }
          else if ( (r=::strncmp(s,"33m",3))==0 )  {   // fg_yellow
            ::textcolor(::YELLOW);
            s += 2;
            continue;
          }
          else if ( (r=::strncmp(s,"34m",3))==0 )  {   // fg_blue
            ::textcolor(::BLUE);
            s += 2;
            continue;
          }
          else if ( (r=::strncmp(s,"35m",3))==0 )  {   // fg_magenta
            ::textcolor(::MAGENTA);
            s += 2;
            continue;
          }
          else if ( (r=::strncmp(s,"36m",3))==0 )  {   // fg_cyan
            ::textcolor(::CYAN);
            s += 2;
            continue;
          }
          else if ( (r=::strncmp(s,"2J",2))==0 )  {    // clear screen
            ::clrscr();
            s += 2;
            continue;
          }
          else if ( 2 == ::sscanf(s,"%d;%dH",&row,&col) )  {
            ::gotoxy(col,row);
            s = ::strchr(s,'H');
            continue;
          }
        }
        else if ( (r=::strncmp(s,"(B",2))==0 )  {     // toascii
          ++s;
          continue;
        }
        else if ( (r=::strncmp(s,"(0",2))==0 )  {     // tographic
          ++s;
          continue;
        }
        else if ( (r=::strncmp(s,"(>",2))==0 )  {     // touser
          ++s;
          continue;
        }
        else if ( (r=::strncmp(s,"(<",2))==0 )  {     // tosupp
          ++s;
          continue;
        }
        else if ( (r=::strncmp(s,")0",2))==0 )  {     // part 2 of setfonts
          ++s;
          continue;
        }
        else if ( (r=::strncmp(s,"*<",2))==0 )  {     // part 3 of setfonts
          ++s;
          continue;
        }
        else if ( (r=::strncmp(s,"+>",2))==0 )  {     // part 4 of setfonts
          ++s;
          continue;
        }
        else if ( (r=::strncmp(s,">",1))==0 )  {      // resetansi
          if ( s_bold ) { ::lowvideo(); s_bold = false; }
          if ( s_blink ) { ::noblinkvideo(); s_blink = false; }
          if ( s_inverse ) { /* noinversevideo(); */ s_inverse = false; }
          if ( s_underline ) { ::nounderlinevideo(); ::noblinkvideo(); s_underline = false; }
          ::textattr(BUILD_TEXTATTR(::BLACK,::WHITE));
          continue;
        }
        else if ( (r=::strncmp(s,"=",1))==0 )  {      // setansi
          continue;
        }
        ::write(1,s,1);
        continue;
      }
      ::write(1,s,1);
    }
    return ::strlen(pb->bufout);
  }
#endif
  return ::write(1,pb->bufout, ::strlen(pb->bufout));
}

//---------------------------------------------------------------------------
int scrc_get_console_dimensions(int* rows, int* cols)  {
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
    ::consolesize(rows, cols);
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
// This function checks the buffer for a valid escape sequence or a normal   
// key stroke.                                                               
//  If a valid key sequence is found, the corresponding code is returned.    
//  INVALID may be returned.                                                 
//  (-1)    is returned if the escape sequence is incomplete.                
//---------------------------------------------------------------------------
{
  int b;
  char c;
  b = *buffer & 0xff;
#define UNKNOWN -1
#define _RET(x,y,z) return x;
#define _RET2(x,y) return x;
  //#define _RET(x,y,z) printf("%s -- [%d,%d]  %s  -- [%d,%d] \n",#x,y,#x,y); return x;
  //#define _RET2(x,y) printf("%s -- [%d]  %s [%d]\n",#x,y,#x,y); return x;
  switch (b)  {
  case 0x9b:
    buffer++;
    switch (*buffer)  {
    case 'D': _RET(MOVE_LEFT,b,*buffer);
    case 'B': _RET(MOVE_DOWN,b,*buffer);
    case 'A': _RET(MOVE_UP,b,*buffer);
    case 'C': _RET(MOVE_RIGHT,b,*buffer);
    case '1':
      buffer++;
      switch (c = *buffer)  {
      case '~' : _RET(KPD_FIND,b,*buffer);
      case '7' :
      case '8' :
      case '9' :
        buffer++;
      switch (*buffer)  {
      case '~' :
        switch (c) {
        case '7' : _RET(F6,b,*buffer);
        case '8' : _RET(F7,b,*buffer);
        case '9' : _RET(F8,b,*buffer);
        }
        break;
      case 0 : _RET(UNKNOWN,b,*buffer);
      }
      break;
      case 0 : _RET(UNKNOWN,b,*buffer);
      }
      break;
    case '2':
      buffer++;
      switch (c = *buffer)  {
      case '~' : _RET(KPD_INSERT,b,*buffer);
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
        case '0' : _RET(F9,b,*buffer);
        case '1' : _RET(F10,b,*buffer);
        case '3' : _RET(F11,b,*buffer);
        case '4' : _RET(F12,b,*buffer);
        case '5' : _RET(F13,b,*buffer);
        case '6' : _RET(F14,b,*buffer);
        case '8' : _RET(F15,b,*buffer);
        case '9' : _RET(F16,b,*buffer);
        }
        break;
      case 0 : _RET(UNKNOWN,b,*buffer);
      }
      break;
      case 0 : _RET(UNKNOWN,b,*buffer);
      }
      break;
    case '3':
      buffer++;
      switch (c = *buffer)  {
      case '~': _RET(KPD_REMOVE,b,*buffer);
      case '1':
      case '2':
      case '3':
      case '4':
        buffer++;
      switch (*buffer)  {
      case '~':
        switch (c) {
        case '1': _RET(F17,b,*buffer);
        case '2': _RET(F18,b,*buffer);
        case '3': _RET(F19,b,*buffer);
        case '4': _RET(F20,b,*buffer);
        }
        break;
      case 0 : _RET(UNKNOWN,b,*buffer);
      }
      break;
      case 0 : _RET(UNKNOWN,b,*buffer);
      }
      break;
    case '4':
      buffer++;
      switch (c = *buffer) {
      case '~' : _RET(KPD_SELECT,b,*buffer);
      case 0 : _RET(UNKNOWN,b,*buffer);
      }
      break;
    case '5':
      buffer++;
      switch (c = *buffer) {
      case '~' : _RET(KPD_PREV,b,*buffer);
      case 0 : _RET(UNKNOWN,b,*buffer);
      }
      break;
    case '6':
      buffer++;
      switch (c = *buffer) {
      case '~' : _RET(KPD_NEXT,b,*buffer);
      case 0 : _RET(UNKNOWN,b,*buffer);
      }
      break;
    case 0:
      _RET(UNKNOWN,b,*buffer);
      break;
    }
    break;
  case 0x8f :
    buffer++;
    switch (*buffer) {
    case 'l': _RET(PAGE_DOWN,b,*buffer);
    case 'm': _RET(PAGE_UP,b,*buffer);
    case 'n': _RET(KPD_PERIOD,b,*buffer);
    case 'p': _RET(KPD_0,b,*buffer);
    case 'q': _RET(KPD_1,b,*buffer);
    case 'r': _RET(KPD_2,b,*buffer);
    case 's': _RET(KPD_3,b,*buffer);
    case 't': _RET(KPD_4,b,*buffer);
    case 'u': _RET(KPD_5,b,*buffer);
    case 'v': _RET(KPD_6,b,*buffer);
    case 'w': _RET(KPD_7,b,*buffer);
    case 'x': _RET(KPD_8,b,*buffer);
    case 'y': _RET(KPD_9,b,*buffer);
    case 'M': _RET(KPD_ENTER,b,*buffer);
    case 'P': _RET(KPD_PF1,b,*buffer);
    case 'Q': _RET(KPD_PF2,b,*buffer);
    case 'R': _RET(KPD_PF3,b,*buffer);
    case 'S': _RET(KPD_PF4,b,*buffer);
    case 0: _RET(UNKNOWN,b,*buffer);
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
      case 'M': // ignore mouse events here!
        if ( *++buffer && *++buffer && *++buffer ) return INVALID;
        return UNKNOWN;
      case 'A': _RET(MOVE_UP,b,*buffer);       // Arrow key UP
      case 'B': _RET(MOVE_DOWN,b,*buffer);     // Arrow key DOWN
      case 'C': _RET(MOVE_RIGHT,b,*buffer);    // Arrow key RIGHT
      case 'D': _RET(MOVE_LEFT,b,*buffer);     // Arrow key LEFT
      case '1':
        buffer++;
        switch (c = *buffer)
	      {
	      case '~' : _RET(KPD_FIND,b,*buffer);
	      case '7' :
	      case '8' :
	      case '9' :
          buffer++;
        switch (*buffer)  {
        case '~' :
          switch (c)  {
          case '7' : _RET(F6,b,*buffer);
          case '8' : _RET(F7,b,*buffer);
          case '9' : _RET(F8,b,*buffer);
          }
        case 0 : _RET(UNKNOWN,b,*buffer);
        }
        break;
	      case 0 : _RET(UNKNOWN,b,*buffer);
	      }
        break;
      case '2':
        buffer++;
        switch (c = *buffer)  {
        case '~': _RET(KPD_INSERT,b,*buffer);
        case '0':
        case '1':
        case '3':
        case '4':
        case '5':
        case '6':
        case '8':
        case '9':
          buffer++;
	      switch (*buffer)  {
	      case '~':
          switch (c)   {
          case '0' : _RET(F9,b,*buffer);
          case '1' : _RET(F10,b,*buffer);
          case '3' : _RET(F11,b,*buffer);
          case '4' : _RET(F12,b,*buffer);
          case '5' : _RET(F13,b,*buffer);
          case '6' : _RET(F14,b,*buffer);
          case '8' : _RET(F15,b,*buffer);
          case '9' : _RET(F16,b,*buffer);
          }
          break;
	      case 0: _RET(UNKNOWN,b,*buffer);
	      }
	      break;
        case 0: _RET(UNKNOWN,b,*buffer);
        }
        break;
      case '3':
        buffer++;
        switch (c = *buffer)  {
        case '~': _RET(KPD_PERIOD,b,*buffer);
          //case '~': _RET(KPD_REMOVE,b,*buffer);
        case '1':
        case '2':
        case '3':
        case '4':
          buffer++;
	      switch (*buffer) {
	      case '~':
          switch (c)  {
          case '1': _RET(F17,b,*buffer);
          case '2': _RET(F18,b,*buffer);
          case '3': _RET(F19,b,*buffer);
          case '4': _RET(F20,b,*buffer);
          }
          break;
	      case 0: _RET(UNKNOWN,b,*buffer);
	      }
	      break;
        case 0: _RET(UNKNOWN,b,*buffer);
        }
        break;
      case '4':
        buffer++;
        switch(c = *buffer) {
        case '~': _RET(KPD_SELECT,b,*buffer);
        case 0:   _RET(UNKNOWN,b,*buffer);
        }
        break;
      case '5':
        buffer++;
        switch (c = *buffer)  {
          //case '~': _RET(KPD_PREV,b,*buffer);
        case '~': _RET(PAGE_UP,b,*buffer);
        case 0:   _RET(UNKNOWN,b,*buffer);
        }
        break;
      case '6':
        buffer++;
        switch (c = *buffer)
	      {
          //case '~' : _RET(KPD_NEXT,b,*buffer);
	      case '~' : _RET(PAGE_DOWN,b,*buffer);
	      case 0   : _RET(UNKNOWN,b,*buffer);
	      }
        break;
      case 0:
        _RET(UNKNOWN,b,*buffer);
        break;
      }
      break;
    case 'O':  // Typically KPD Numlock ON
      buffer++;
      switch (*buffer)
      {
      case 'l': _RET(PAGE_DOWN,b,*buffer);
      case 'n': _RET(KPD_PERIOD,b,*buffer);
      case 'o': _RET(KPD_PF2,b,*buffer);
      case 'j': _RET(KPD_PF3,b,*buffer);
      case 'm': _RET(KPD_PF4,b,*buffer);
      case 'p': _RET(KPD_0,b,*buffer);
      case 'q': _RET(KPD_1,b,*buffer);
      case 'r': _RET(KPD_2,b,*buffer);
      case 's': _RET(KPD_3,b,*buffer);
      case 't': _RET(KPD_4,b,*buffer);
      case 'u': _RET(KPD_5,b,*buffer);
      case 'v': _RET(KPD_6,b,*buffer);
      case 'w': _RET(KPD_7,b,*buffer);
      case 'x': _RET(KPD_8,b,*buffer);
      case 'y': _RET(KPD_9,b,*buffer);
      case 'M': _RET(KPD_ENTER,b,*buffer);
      case 'P': _RET(KPD_PF1,b,*buffer);
      case 'Q': _RET(KPD_PF2,b,*buffer);
      case 'R': _RET(KPD_PF3,b,*buffer);
      case 'S': _RET(KPD_PF4,b,*buffer);
      case 0 :  _RET(UNKNOWN,b,*buffer);
      }
      break;
    case 0:
      _RET(UNKNOWN,b,*buffer);
      break;
    }
    break;
  case 0x7f :
    _RET(DELETE_KEY,b,*buffer);

  default:
#ifdef _WIN32
    switch(b)  {
    case 13:    _RET2(RETURN_KEY,b);
    case 16:    _RET2(KEY_SHIFT,b);
    case 17:
      buffer++;
      if ( *buffer >= 'A' && *buffer <= 26 )
        return *buffer - 'A' + CTRL_A;
      _RET2(KEY_CTRL,b);
    case 33:    _RET2(KPD_PREV,b);  // PAGE_UP
    case 34:    _RET2(KPD_NEXT,b);  // PAGE_DOWN
    case 35:    _RET2(PAGE_DOWN,b); // END
    case 36:    _RET2(PAGE_UP,b);   // HOME
    case 37:    _RET2(MOVE_LEFT,b);
    case 38:    _RET2(MOVE_UP,b);
    case 39:    _RET2(MOVE_RIGHT,b);
    case 40:    _RET2(MOVE_DOWN,b);
      //case 45:   _RET2(INSERT,b);
    case 123:  _RET2(BACK_SPACE,b);
      // ? case 115:  _RET2(KPD_PF4,b);

    case 46:    _RET2(KPD_PERIOD,b); //_RET22(DELETE_KEY,b); // UPI Needs period!
    case 112:   _RET2(DELETE_KEY,b);
    case 144:   _RET2(KPD_PF1,b);
    case 111:   _RET2(KPD_PF2,b);
    case 106:   _RET2(KPD_PF3,b);
    case 109:   _RET2(KPD_PF4,b);
    case 110:   _RET2(KPD_PERIOD,b);
    case 96:    _RET2(KPD_0,b);
    case 97:    _RET2(KPD_1,b);
    case 98:    _RET2(KPD_2,b);
    case 99:    _RET2(KPD_3,b);
    case 100:   _RET2(KPD_4,b);
    case 101:   _RET2(KPD_5,b);
    case 102:   _RET2(KPD_6,b);
    case 103:   _RET2(KPD_7,b);
    case 104:   _RET2(KPD_8,b);
    case 105:   _RET2(KPD_9,b);
    default:
      break;
    }
#endif
    if (b < 0x20) { _RET2(INVALID+b,b); }
    else if (b <= '~') { _RET2(b,b); }
  }
  _RET2(INVALID,INVALID);
}
//----------------------------------------------------------------------------
int scrc_wait (Display *disp)   {
  return ::scrc_read_keyboard(disp, 1);
}

//----------------------------------------------------------------------------
int scrc_read_keyboard (Display * /*disp */, int wait)  {
  int status = -1, sub_status;
  unsigned int event;
  void* dummy;  
  if (wait) status = ::wtc_wait(&event,&dummy,&sub_status);
  int key = ::scrc_get_last_key ();
  if(_p)printf("scrc_read_keyboard: Got char: %d %02X status=%d\n",key,key,status);
  return key;
}

//----------------------------------------------------------------------------
int scrc_read (Display *disp, unsigned char *buffer, int wait)      {
  int sub_status;
  unsigned int c;
  void* dummy;

  if (!Last_char)  {
    ::scrc_fflush(disp->pb);
    if (wait) ::wtc_wait (&c,&dummy,&sub_status);
  }
  *buffer = Last_char;
  if (Last_char)  {
    Armed = 0;
    Last_char = 0;
  }
  return 1;
}

//----------------------------------------------------------------------------
int scrc_test_input ()    {
  ::scrc_rearm (0,0);
  if(_p)::printf("scrc_test_input\n");
  return (Last_char);
}

//----------------------------------------------------------------------------
int scrc_get_smgid (Pasteboard *pb)   {
  return pb->smgid;
}

//----------------------------------------------------------------------------
int scrc_enable_unsolicited_input (Pasteboard* pb, int (* ast)())   {
  User_ast = ast;
  ::wtc_subscribe (WT_FACILITY_KEYBOARD, scrc_rearm_keyboard, scrc_handler_keyboard, pb);
  ::scrc_rearm_keyboard (WT_FACILITY_KEYBOARD,0);
  return 1;
}
//----------------------------------------------------------------------------
int scrc_disable_unsolicited_input (Pasteboard* /* pb */)   {
  ::wtc_remove (WT_FACILITY_KEYBOARD);
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
//---------------------------------------------------------------------------
void scrc_set_mouse_handler (Pasteboard* /* pb */, int (*handler)(Pasteboard*,int,int,int))  {
  User_mouse_handler = handler;
}
//---------------------------------------------------------------------------
void scrc_enable_mouse(Pasteboard* pb) {
  //::scrc_putes("[?1000h",pb); // Sends event on press & release
  ::scrc_putes("[?9h",pb);
}
//---------------------------------------------------------------------------
void scrc_disable_mouse(Pasteboard* pb) {
  ::scrc_putes("[?91",pb);
  ::scrc_putes("[?10001",pb); // Stop sending event on press & release
}
//---------------------------------------------------------------------------
void scrc_handle_mouse(Pasteboard* pb, char key)  {
  if ( User_mouse_handler )   {
    static char mouse_data[6] = {0,0,0,0,0,0};
    mouse_data[0] = mouse_data[1];
    mouse_data[1] = mouse_data[2];
    mouse_data[2] = mouse_data[3];
    mouse_data[3] = mouse_data[4];
    mouse_data[4] = mouse_data[5];
    mouse_data[5] = key;
    if ( mouse_data[0] == 0x1B && mouse_data[1] == '[' && mouse_data[2] == 'M' ) {
      (*User_mouse_handler)(pb,mouse_data[3],mouse_data[4],mouse_data[5]);
    }
  }
}
//---------------------------------------------------------------------------
void scrc_set_configure_handler (Pasteboard* /* pb */, int (*handler)(int,int))  {
  User_configure_handler = handler;
}
//---------------------------------------------------------------------------
int scrc_execute_configure_handler(Pasteboard* /* pb */)  {
  if ( User_configure_handler ) return (*User_configure_handler)(0,0);
  return 0;
}
//---------------------------------------------------------------------------
void scrc_iconify_display (Display* /* d */)    {
}
//---------------------------------------------------------------------------
void scrc_uniconify_display (Display* /* d */)    {
}
