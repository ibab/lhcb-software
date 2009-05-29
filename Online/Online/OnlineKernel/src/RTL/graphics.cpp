// $Id: graphics.cpp,v 1.3 2009-05-29 14:41:21 frankb Exp $
//====================================================================
//  ROLogger
//--------------------------------------------------------------------
//
//  Package    : OnlineKernel
//
//  Description: Simple screen display graphics
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineKernel/src/RTL/graphics.cpp,v 1.3 2009-05-29 14:41:21 frankb Exp $
#include "RTL/rtl.h"
#include "RTL/graphics.h"
#include <cstdlib>
#include <cerrno>

#ifdef _WIN32
#include "RTL/conioex.h"
#endif
namespace graphics {
#ifdef _WIN32
  void  consolesize(size_t* rows, size_t* cols) { int x,y; ::consolesize(&y,&x); *rows=y;*cols=x; }
  void  gotoxy(int x,int y) {    ::gotoxy(x,y);             }
  void  bold()              {    highvideo();               }
  void  nobold()            {    lowvideo();                }
  void  dimmed()            {    lowvideo();                }
  void  nodimmed()          {    normalvideo();             }
  void  inverse()           {    inversevideo();            }
  void  noinverse()         {    noinversevideo();          }
  void  underline()         {    underlinevideo();          }
  void  nounderline()       {    normalvideo();             }
  void  flash()             {    blinkvideo();              }
  void  noflash()           {    normalvideo();             }
  void  normal()            {    normalvideo();             }

  void  clear_screen()      {    clrscr();                  }
  void  plain()             {    lowvideo();normalvideo();  }
  void  narrow_screen()     {                               }
  void  wide_screen()       {                               }
  void  cursor_on()         {    _setcursortype(_NORMALCURSOR); }
  void  cursor_off()        {    _setcursortype(_NOCURSOR);}

  void  red()               {    textcolor(RED);           }
  void  yellow()            {    textcolor(YELLOW);        }
  void  white()             {    textcolor(WHITE);         }
  void  green()             {    textcolor(GREEN);         }
  void  blue()              {    textcolor(BLUE);          }
  void  cyan()              {    textcolor(CYAN);          }
  void  black()             {    textcolor(BLACK);         }
  void  magenta()           {    textcolor(MAGENTA);       }


  void  bg_red()            {    textbackground(RED);     }
  void  bg_yellow()         {    textbackground(YELLOW);  }
  void  bg_white()          {    textbackground(WHITE);   }
  void  bg_green()          {    textbackground(GREEN);   }
  void  bg_blue()           {    textbackground(BLUE);    }
  void  bg_cyan()           {    textbackground(CYAN);    }
  void  bg_black()          {    textbackground(BLACK);    }
  void  bg_magenta()        {    textbackground(MAGENTA);    }

#else

  void  consolesize(size_t* rows, size_t* cols);
  void  gotoxy(int x,int y) {    ::printf("\033[%d;%dH",y,x);    }
  void  scroll(int top,int bot)  { ::printf("\033[%d;%dr",top,bot); }

  void  plain()             {    ::printf("\033[0m");         }
  void  bold()              {    ::printf("\033[1m");         }
  void  nobold()            {    ::printf("\033[21m");        }
  void  dimmed()            {    ::printf("\033[2m");         }
  void  nodimmed()          {    ::printf("\033[22m");        }
  void  underline()         {    ::printf("\033[4m");         }
  void  nounderline()       {    ::printf("\033[24m");        }
  void  flash()             {    ::printf("\033[5m");         }
  void  noflash()           {    ::printf("\033[25m");        }
  void  hidden()            {    ::printf("\033[6m");         }
  void  inverse()           {    ::printf("\033[7m");         }
  void  noinverse()         {    ::printf("\033[27m");        }

  void  normal()            {    ::printf("\033[27m");        }
  void  clear_screen()      {    ::printf("\033[2J");         }
  void  narrow_screen()     {    ::printf("\033[?3l");        }
  void  wide_screen()       {    ::printf("\033[?3h");        }
  void  cursor_on()         {    ::printf("\033[?25h");       }
  void  cursor_off()        {    ::printf("\033[?25l");       }

  void  black()             {    ::printf("\033[30m");     }
  void  red()               {    ::printf("\033[31m");     }
  void  green()             {    ::printf("\033[32m");     }
  void  yellow()            {    ::printf("\033[33m");     }
  void  blue()              {    ::printf("\033[34m");     }
  void  magenta()           {    ::printf("\033[35m");     }
  void  cyan()              {    ::printf("\033[36m");     }
  void  white()             {    ::printf("\033[37m");     }

  void  bg_black()          {    ::printf("\033[40m");     }
  void  bg_red()            {    ::printf("\033[41m");     }
  void  bg_green()          {    ::printf("\033[42m");     }
  void  bg_yellow()         {    ::printf("\033[43m");     }
  void  bg_blue()           {    ::printf("\033[44m");     }
  void  bg_magenta()        {    ::printf("\033[45m");     }
  void  bg_cyan()           {    ::printf("\033[46m");     }
  void  bg_white()          {    ::printf("\033[47m");     }

  void  tographics()        {    ::printf("\033>"); }
  void  toascii()           {    ::printf("\033(B"); }
#endif
}
#ifndef _WIN32
#include <cstdlib>
#include <sys/ioctl.h>

void graphics::consolesize(size_t* rows, size_t* cols) {
  int fd = ::fileno(stdout);
  if ( ::isatty(fd) ) {
    struct winsize wns;
    do {
      if( ::ioctl(fd,TIOCGWINSZ,&wns) == 0 ) {
        *rows = wns.ws_row;
        *cols = wns.ws_col;
	return;
      }
    } while (errno == EINTR);   
  }
  *rows = 0;
  *cols = 0;
}
#endif

extern "C" int graphics_test(int,char**) {
  using namespace graphics;

#define DO(x,y) x(); y();                printf(" test ");           plain();                      printf("   ");  \
         x(); y();  dimmed();            printf(" Dimmed ");         nodimmed();                   printf("   ");  \
         x(); y();  bold();              printf(" Bold ");           nobold();                     printf("   ");  \
         x(); y();  underline();         printf(" Underline ");      nounderline();                printf("   ");  \
         x(); y();  underline(); bold(); printf(" Bold-Underline "); nobold();      nounderline(); printf("   ");  \
         x(); y();  inverse();           printf(" Inverse ");        noinverse();                  printf("   ");  \
         x(); y();  inverse(); bold();   printf(" Bold-Inverse ");   nobold();      noinverse();   printf("   ");  \
         x(); y();  flash();             printf(" Flash ");          noflash();                    plain();  printf("\n");
  DO(bg_black,black);
  DO(bg_black,white);
  DO(bg_black,yellow);
  DO(bg_black,green);
  DO(bg_black,red);
  DO(bg_black,magenta);
  DO(bg_black,blue);
  DO(bg_black,cyan);

  DO(bg_white,black);
  DO(bg_white,white);
  DO(bg_white,yellow);
  DO(bg_white,green);
  DO(bg_white,red);
  DO(bg_white,magenta);
  DO(bg_white,blue);
  DO(bg_white,cyan);

  DO(bg_green,black);
  DO(bg_green,white);
  DO(bg_green,yellow);
  DO(bg_green,green);
  DO(bg_green,red);
  DO(bg_green,magenta);
  DO(bg_green,blue);
  DO(bg_green,cyan);
#ifndef _WIN32
  fprintf(stdout,"\033#3");printf(" Hello\n");
  fprintf(stdout,"\033#4");printf(" Hello\n");
  fprintf(stdout,"\033#6");printf(" Hello\n");
  for(int i = 0; i < 3; ++i) {
    fprintf(stdout,"\033M"); fflush(stdout);
    ::lib_rtl_sleep(10);
  }
  for(int i = 0; i < 3; ++i) {
    fprintf(stdout,"\033D"); fflush(stdout);
    ::lib_rtl_sleep(100);
  }
#endif
  bg_black();
  green();
  normal();
  return 1;
}

