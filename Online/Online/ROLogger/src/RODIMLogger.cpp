// $Id: RODIMLogger.cpp,v 1.6 2008-04-30 21:06:53 frankb Exp $
//====================================================================
//  ROLogger
//--------------------------------------------------------------------
//
//  Package    : ROLogger
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/RODIMLogger.cpp,v 1.6 2008-04-30 21:06:53 frankb Exp $

// Framework include files
extern "C" {
  #include "dic.h"
  #include "dis.h"
}
#include <cerrno>
#include <iostream>
#include "RTL/rtl.h"
#include "ROLogger/RODIMLogger.h"

#ifdef _WIN32
#include "RTL/conioex.h"
#endif
namespace graphics {
#ifdef _WIN32
  inline void  consolesize(size_t* rows, size_t* cols) { int x,y; ::consolesize(&y,&x); *rows=y;*cols=x; }
  inline void  gotoxy(int x,int y) {    ::gotoxy(x,y);             }
  inline void  bold()              {    highvideo();               }
  inline void  nobold()            {    lowvideo();                }
  inline void  dimmed()            {    lowvideo();                }
  inline void  nodimmed()          {    normalvideo();             }
  inline void  inverse()           {    inversevideo();            }
  inline void  noinverse()         {    noinversevideo();          }
  inline void  underline()         {    underlinevideo();          }
  inline void  nounderline()       {    normalvideo();             }
  inline void  flash()             {    blinkvideo();              }
  inline void  noflash()           {    normalvideo();             }
  inline void  normal()            {    normalvideo();             }

  inline void  clear_screen()      {    clrscr();                  }
  inline void  plain()             {    lowvideo();normalvideo();  }
  inline void  narrow_screen()     {                               }
  inline void  wide_screen()       {                               }
  inline void  cursor_on()         {    _setcursortype(_NORMALCURSOR); }
  inline void  cursor_off()        {    _setcursortype(_NOCURSOR);}

  inline void  red()               {    textcolor(RED);           }
  inline void  yellow()            {    textcolor(YELLOW);        }
  inline void  white()             {    textcolor(WHITE);         }
  inline void  green()             {    textcolor(GREEN);         }
  inline void  blue()              {    textcolor(BLUE);          }
  inline void  cyan()              {    textcolor(CYAN);          }
  inline void  black()             {    textcolor(BLACK);         }
  inline void  magenta()           {    textcolor(MAGENTA);       }


  inline void  bg_red()            {    textbackground(RED);     }
  inline void  bg_yellow()         {    textbackground(YELLOW);  }
  inline void  bg_white()          {    textbackground(WHITE);   }
  inline void  bg_green()          {    textbackground(GREEN);   }
  inline void  bg_blue()           {    textbackground(BLUE);    }
  inline void  bg_cyan()           {    textbackground(CYAN);    }
  inline void  bg_black()          {    textbackground(BLACK);    }
  inline void  bg_magenta()        {    textbackground(MAGENTA);    }

  inline void  bold_yellow()       {    highvideo(); textattr(BUILD_TEXTATTR(BLACK,YELLOW));  }
  inline void  bold_magenta()      {    highvideo(); textattr(BUILD_TEXTATTR(BLACK,MAGENTA)); }
  inline void  bold_red()          {    highvideo(); textattr(BUILD_TEXTATTR(BLACK,RED));     }

#else

  inline void  consolesize(size_t* rows, size_t* cols);
  inline void  gotoxy(int x,int y) {    ::printf("\033[%d;%dH",y,x);    }
  inline void  scroll(int top,int bot)  { ::printf("\033[%d;%dr",top,bot); }

  inline void  plain()             {    ::printf("\033[0m");         }
  inline void  bold()              {    ::printf("\033[1m");         }
  inline void  nobold()            {    ::printf("\033[21m");        }
  inline void  dimmed()            {    ::printf("\033[2m");         }
  inline void  nodimmed()          {    ::printf("\033[22m");        }
  inline void  underline()         {    ::printf("\033[4m");         }
  inline void  nounderline()       {    ::printf("\033[24m");        }
  inline void  flash()             {    ::printf("\033[5m");         }
  inline void  noflash()           {    ::printf("\033[25m");        }
  inline void  hidden()            {    ::printf("\033[6m");         }
  inline void  inverse()           {    ::printf("\033[7m");         }
  inline void  noinverse()         {    ::printf("\033[27m");        }

  inline void  normal()            {    ::printf("\033[27m");        }
  inline void  clear_screen()      {    ::printf("\033[2J");         }
  inline void  narrow_screen()     {    ::printf("\033[?3l");        }
  inline void  wide_screen()       {    ::printf("\033[?3h");        }
  inline void  cursor_on()         {    ::printf("\033[?25h");       }
  inline void  cursor_off()        {    ::printf("\033[?25l");       }

  inline void  black()             {    ::printf("\033[30m");     }
  inline void  red()               {    ::printf("\033[31m");     }
  inline void  green()             {    ::printf("\033[32m");     }
  inline void  yellow()            {    ::printf("\033[33m");     }
  inline void  blue()              {    ::printf("\033[34m");     }
  inline void  magenta()           {    ::printf("\033[35m");     }
  inline void  cyan()              {    ::printf("\033[36m");     }
  inline void  white()             {    ::printf("\033[37m");     }

  inline void  bg_black()          {    ::printf("\033[40m");     }
  inline void  bg_red()            {    ::printf("\033[41m");     }
  inline void  bg_green()          {    ::printf("\033[42m");     }
  inline void  bg_yellow()         {    ::printf("\033[43m");     }
  inline void  bg_blue()           {    ::printf("\033[44m");     }
  inline void  bg_magenta()        {    ::printf("\033[45m");     }
  inline void  bg_cyan()           {    ::printf("\033[46m");     }
  inline void  bg_white()          {    ::printf("\033[47m");     }

  inline void  tographics()          {    ::printf("\033>"); }
  inline void  toascii()          {    ::printf("\033(B"); }
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

using namespace graphics;

extern "C" int graphics_test(int,char**) {
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


using namespace ROLogger;
using namespace graphics;

/// Standard constructor
RODIMLogger::RODIMLogger(int argc, char** argv) 
: m_colors(true)
{
  std::string name;
  RTL::CLI cli(argc, argv, help_fun);
  cli.getopt("service",1,name);
  m_colors = cli.getopt("colors",1) != 0;
  if ( name.empty() ) {
    std::cout << "You have to supply a service name to display its data." << std::endl;
    ::lib_rtl_sleep(10000);
    ::exit(2);
  }
  m_service = ::dis_add_cmnd((char*)name.c_str(),(char*)"C",requestHandler,(long)this);
  bg_black();
}

/// Standard destructor
RODIMLogger::~RODIMLogger()  {
  if ( m_service ) {
    ::dis_remove_service(m_service);
    m_service = 0;
  }
  removeAllServices();
}

/// Remove unconditionally all connected services
void RODIMLogger::removeAllServices() {
  for ( Services::iterator i=m_infos.begin(); i!=m_infos.end(); ++i )
    cleanupService((*i).second);
  m_infos.clear();
}

void RODIMLogger::help_fun() {
}

/// Retrieve message severity from DIM logger message
int RODIMLogger::msgSeverity(const char* msg) {
  if ( msg ) {
    int len = ::strlen(msg);  
    if      ( len>18 && !strncmp(msg+12,"[VERB]",6)  ) return 1;
    else if ( len>19 && !strncmp(msg+12,"[DEBUG]",7) ) return 2;
    else if ( len>18 && !strncmp(msg+12,"[INFO]",6)  ) return 3;
    else if ( len>18 && !strncmp(msg+12,"[WARN]",6)  ) return 4;
    else if ( len>19 && !strncmp(msg+12,"[ERROR]",7) ) return 5;
    else if ( len>19 && !strncmp(msg+12,"[FATAL]",7) ) return 6;
  }
  return 3;
}

/// Print single message retrieved from error logger
void RODIMLogger::printMessage(FILE* fp, const char* msg, bool crlf)  {
  if ( m_colors ) {
    bg_black();
    switch(msgSeverity(msg))      {
    case 0: /* No label - white */
      white();
      break;
    case 1: /* VERBOSE - blue */
      blue();
      break;
    case 2: /* DEBUG - green */
      green();
      break;
    case 3: /* INFO - white */
      white();
      break;
    case 4: /* WARN - bold yellow */
      bold(); yellow();
      break;
    case 5: /* ERROR - bold red */
      bold(); red();
      break;
    case 6: /* FATAL - bold magenta */
      bold(); magenta();
      break;
    } 
  }
  fprintf(fp,msg);
  if ( crlf ) fprintf(fp,"\n");
  if ( m_colors ) ::plain();
}

/// Print header information before starting output
void RODIMLogger::printHeader(FILE* fp, const std::string& title) {
  if ( m_colors ) {
    size_t rows=0, cols=0;
    consolesize(&rows,&cols);
    if ( rows>0 && cols>0 ) {
      char buffer[1024];
      cols  = cols<sizeof(buffer)?cols:sizeof(buffer);

      ::white();
      ::bg_black();
      ::inverse();
      ::bold();

      ::memset(buffer,' ',cols);
      buffer[cols-1] = 0;	
      ::printf(buffer);
      ::printf("\n");
      ::memcpy(&buffer[20],title.c_str(),title.length());
      ::printf(buffer);
      ::printf("\n");
      ::memset(&buffer[20],' ',title.length());
      buffer[cols-1] = 0;
      ::printf(buffer);
      ::printf("\n");
      ::normal();
      ::plain();
      ::printf("\n");
      return;
    }
  }
  ::fprintf(fp,"\n                        Logger history of %s\n\n",title.c_str());
}

/// Print multi-line header information before starting output
void RODIMLogger::printHeader(FILE* fp, const std::vector<std::string>& titles) {
  if ( m_colors ) {
    size_t rows=0, cols=0;
    consolesize(&rows,&cols);
    if ( rows>0 && cols>0 ) {
      char buffer[1024];
      cols  = cols<sizeof(buffer)?cols:sizeof(buffer);
      ::white();
      ::bg_black();
      ::inverse();
      ::bold();
      ::memset(buffer,' ',cols);
      buffer[cols-1] = 0;	
      ::printf(buffer);
      ::printf("\n");
      for(std::vector<std::string>::const_iterator i=titles.begin();i!=titles.end();++i) {
	const std::string& title = *i;
	::memcpy(&buffer[20],title.c_str(),title.length());
	::printf(buffer);
	::printf("\n");
	::memset(&buffer[20],' ',title.length());
	buffer[cols-1] = 0;
      }
      
      ::printf(buffer);
      ::printf("\n");
      ::normal();
      ::plain();
      ::printf("\n");
      return;
    }
  } 
  for(std::vector<std::string>::const_iterator i=titles.begin();i!=titles.end();++i)
    ::fprintf(fp,"                   -> %s\n",(*i).c_str());
}

/// DIM command service callback
void RODIMLogger::requestHandler(void* tag, void* address, int* size) {
  RODIMLogger* h = *(RODIMLogger**)tag;
  std::string nam, n = (char*)address;
  size_t idx;
  switch(::toupper(n[0])) {
  case 'H':  // History mode
    idx = n.find(":");
    if ( idx != std::string::npos ) {
      nam = n.substr(idx+1);
      h->cleanupServices(nam);
      h->handleHistory(nam);
      return;
    }
    break;
  case 'M': // Messages mode
    idx = n.find(":");
    if ( idx != std::string::npos ) {
      const char* p = (const char*)address;
      h->removeAllServices();
      h->handleMessages(p+idx+1,p + (*size));
      return;
    }
    idx = n.find("+");
    if ( idx != std::string::npos ) {
      const char* p = (const char*)address;
      h->handleMessages(p+idx+1,p + (*size));
      return;
    }
    idx = n.find("-");
    if ( idx != std::string::npos ) {
      const char* p = (const char*)address;
      h->handleRemoveMessages(p+idx+1,p + (*size));
      return;
    }
    break;
  default:
    break;
  }
  std::cout << "Received invalid request." << std::endl;
}

/// DIM command service callback
void RODIMLogger::handleMessages(const char* items, const char* end) {
  if ( items ) {
    std::vector<std::string> titles;
    time_t now = ::time(0);
    for(const char* p=items; p < end; p += ::strlen(p)+1 ) {
      if ( strlen(p)>0 ) {
	Services::iterator i=m_infos.find(p);
	if ( i == m_infos.end() ) {
	  Entry* e = m_infos[p] = new Entry;
	  std::cout << "Adding client:" << p << std::endl;
	  e->id      = ::dic_info_service((char*)p,MONITORED,0,0,0,messageInfoHandler,(long)e,0,0);
	  e->created = now;
	  e->self    = this;
	  e->name    = p;
	}
      }
    }
    for(Services::iterator i=m_infos.begin();i!=m_infos.end();++i) 
      titles.push_back((*i).first);
    printHeader(stdout,titles);
  }
}

/// DIM command service callback
void RODIMLogger::handleRemoveMessages(const char* items, const char* end) {
  if ( items ) {
    std::vector<std::string> titles;
    for(const char* p=items; p < end; p += ::strlen(p)+1 ) {
      if ( strlen(p)>0 ) {
	Services::iterator i=m_infos.find(p);
	if ( i != m_infos.end() ) {
	  cleanupService((*i).second);
	  m_infos.erase(i);
	}
      }
    }
    for(Services::iterator i=m_infos.begin();i!=m_infos.end();++i) 
      titles.push_back((*i).first);
    printHeader(stdout,titles);
  }
}

/// Cleanup service stack
void RODIMLogger::cleanupServices(const std::string& match) {
  time_t now = ::time(0);
  for ( Services::iterator i=m_infos.begin(); i != m_infos.end(); ) {
    Entry* e = (*i).second;
    if ( now-e->created>2 || (*i).first == match ) {
      cleanupService(e);
      m_infos.erase(i);
      i = m_infos.begin();
      continue;
    }
    ++i;
  }
}

/// Cleanup service entry
void RODIMLogger::cleanupService(Entry* e) {
  if ( e ) {
    //std::cout << "Removing client:" << e->name << std::endl;
    ::dic_release_service(e->id);
    delete e;
  }
}

/// DIM command service callback
void RODIMLogger::handleHistory(const std::string& nam) {
  time_t now = ::time(0);
  Entry* e = m_infos[nam] = new Entry;
  std::cout << "Adding client:" << nam << std::endl;
  e->id      = ::dic_info_service((char*)nam.c_str(),ONCE_ONLY,0,0,0,historyInfoHandler,(long)e,0,0);
  e->created = now;
  e->self    = this;
  e->name    = nam;
}

/// Dim Info callback overload to process messages
void RODIMLogger::messageInfoHandler(void* tag, void* address, int* size)  {
  if ( address && size && *size>0 ) {
    Entry* e = *(Entry**)tag;
    e->self->printMessage(stdout,(char*)address,false);
  }
}

/// Dim Info callback overload to process messages
void RODIMLogger::historyInfoHandler(void* tag, void* address, int* size)  {
  if ( address && size && *size>0 ) {
    Entry* e = *(Entry**)tag;
    Services& s = e->self->m_infos;
    FILE* fp = stdout;
    char *msg = (char*)address, *end = msg + *size, *ptr = msg;
    std::string title = "Logger history of:";
    title += e->name;
    e->self->printHeader(fp, title);
    while (ptr<=end) {
      char* p = strchr(ptr,'\n');
      if ( p ) {
	*p = 0;
	e->self->printMessage(fp,ptr);
	ptr = p;
      }
      ++ptr;
    }
    for ( Services::iterator i=s.begin(); i != s.end(); ++i ) {
      if ( (*i).second == e ) {
	std::cout << "Release client:" << e->name << std::endl;
	e->self->cleanupService((*i).second);
	s.erase(i);
	break;
      }
    }
  }
}

extern "C" int romon_logger(int argc, char** argv) {
  RODIMLogger mon(argc, argv);
  ::dis_start_serving((char*)RTL::processName().c_str());
  while(1) ::lib_rtl_sleep(1000);
  return 1;
}
