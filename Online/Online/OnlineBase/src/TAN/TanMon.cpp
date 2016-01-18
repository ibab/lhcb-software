#include <ctime>
#include <cstdio>
#include <iostream>
#include <sstream>
#include "RTL/rtl.h"
#include "TAN/TanDB.h"

#define dbl_hi_top      printf("\033#3")
#define dbl_hi_botm     printf("\033#4")
#define double_width    printf("\033#6")
#define single          printf("\033#5")
#define bold            printf("\033[1m")
#define flashing        printf("\033[5m")
#define underline       printf("\033[4m")
#define inverse         printf("\033[7m")
#define plain           printf("\033[0m")
#define clear_scr       printf("\033[2J")
#define cursor(x,y)     printf("\033[%d;%dH",x,y)
#define scroll(top,bot) printf("\033[%d;%dr",top,bot)
#define beep            printf("\007")
#define cr              printf("\n")
#define ascii           printf("\033(B")
#define graphics        printf("\033(0")
#define col80           printf("\033[?2h")

static int continuous = 0;
static int simple = 0;

void terminal_response(void) {
#ifdef _OSK
  void keyboard_handler(int);
  intercept( keyboard_handler );
}
void keyboard_handler( int signal ) {
  if( signal == 2 )
    continuous = 0;
  else if( signal == 3 ) {}
#endif
}

extern "C" int tanmon( int argc, char* argv[])   {
  char *c;
  time_t curr;
  while (--argc > 0)   {               /* process options */
    if (*(c = *++argv) == '-') {
      switch (*++c | 0x20)   {
      case 'c':
        continuous = 1;
        break;
      case 's':
        simple = 1;
        break;
      case '?':
      default:
        puts ("Syntax: tanmon [<-opt>]");
      puts ("Function: tan monitor");
      puts ("Options:");
      puts ("    -c                    Continuous mode");
      puts ("    -s                    Simple output");
      return lib_rtl_default_return();
      }
    }
  }
  terminal_response();
  do  {
    char text[132];
    std::stringstream s;
    time(&curr);
    if ( simple )  {
      s	<< std::endl << "\t\t\t\t  T A N   M O N I T O R \t\t\t\t" << std::endl << std::endl;
      TanDataBase::Instance().Dump(s);
      ::puts(s.str().c_str());
    }
    else  {
      clear_scr; bold; inverse; 
      cursor(1,25);  printf("                                                       ");
      cursor(2,25);  printf("                 T A N   M O N I T O R                 ");
      cursor(3,25);  printf("                                                       ");
      plain;      
      cursor(4,20);
      ::puts(ctime(&curr));          
      cursor(4,40);
      ::snprintf(text,sizeof(text),"%s  PID:%d  %s",RTL::nodeName().c_str(),::lib_rtl_pid(),RTL::processName().c_str());
      ::puts(text);
      cursor(5,1);
      TanDataBase::Instance().Dump(s);
      ::puts(s.str().c_str());
    }
    if ( continuous )  lib_rtl_sleep(2000);
  } while ( continuous );
  return lib_rtl_default_return();
}
