// $Id: RODIMLogger.cpp,v 1.2 2008-04-30 14:47:19 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/RODIMLogger.cpp,v 1.2 2008-04-30 14:47:19 frankb Exp $

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
#endif
namespace graphics {
#ifdef _WIN32
  inline void  consolesize(size_t* rows, size_t* cols) { int x,y; ::consolesize(&y,&x); *rows=y;*cols=x; }
  inline void  gotoxy(int x,int y) { ::gotoxy(x,y);            }
  inline void  inverse()       {    inversevideo();            }
  inline void  underline()     {    underlinevideo();          }
  inline void  normal()        {    normalvideo();             }
  inline void  clear_screen()  {    clrscr();                  }
  inline void  flash()         {    blinkvideo();              }
  inline void  plain()         {    lowvideo();normalvideo();  }
  inline void  narrow_screen() {                               }
  inline void  wide_screen()   {                               }
  inline void  cursor_on()     {    _setcursortype(); }
  inline void  cursor_off()    {    _setcursortype(_NOCURSOR);       }

  inline void  white()         {    textattr(BUILD_TEXTATTR(BLACK,WHITE));   }
  inline void  green()         {    textattr(BUILD_TEXTATTR(BLACK,GREEN));   }
  inline void  blue()          {    textattr(BUILD_TEXTATTR(BLACK,BLUE));    }

  inline void  bold()          {    highvideo();               }
  inline void  bold_yellow()   {    highvideo(); textattr(BUILD_TEXTATTR(BLACK,YELLOW));  }
  inline void  bold_magenta()  {    highvideo(); textattr(BUILD_TEXTATTR(BLACK,MAGENTA)); }
  inline void  bold_red()      {    highvideo(); textattr(BUILD_TEXTATTR(BLACK,RED));     }

#else

  inline void  consolesize(size_t* rows, size_t* cols);
  inline void  gotoxy(int x,int y) { ::printf("\033[%d;%dH",y,x);    }

  inline void  inverse()           {    ::printf("\033[7m");         }
  inline void  underline()         {    ::printf("\033[4;30m");      }
  inline void  normal()            {    ::printf("\033[27m");        }
  inline void  clear_screen()      {    ::printf("\033[2J");         }
  inline void  flash()             {    ::printf("\033[5m");         }
  inline void  plain()             {    ::printf("\033[0m");         }
  inline void  narrow_screen()     {    ::printf("\033[?3l");        }
  inline void  wide_screen()       {    ::printf("\033[?3h");        }
  inline void  cursor_on()         {    ::printf("\033[?25h");       }
  inline void  cursor_off()        {    ::printf("\033[?25l");       }

  inline void  white()             {    ::printf("\033[0;37;40m");   }
  inline void  green()             {    ::printf("\033[0;32;40m");   }
  inline void  blue()              {    ::printf("\033[0;34;40m");   }

  inline void  bold()              {    ::printf("\033[1m");         }
  inline void  bold_yellow()       {    ::printf("\033[0;1;33;40m"); }
  inline void  bold_magenta()      {    ::printf("\033[0;1;35;40m"); }
  inline void  bold_red()          {    ::printf("\033[0;1;31;40m"); }
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
      bold_yellow();
      break;
    case 5: /* ERROR - bold red */
      bold_red();
      break;
    case 6: /* FATAL - bold magenta */
      bold_magenta();
      break;
    } 
  }
  fprintf(fp,msg);
  if ( crlf ) fprintf(fp,"\n");
  if ( m_colors ) plain();
}

/// Print header information before starting output
void RODIMLogger::printHeader(FILE* fp, const std::string& title) {
  if ( m_colors ) {
    size_t rows=0, cols=0;
    consolesize(&rows,&cols);
    if ( rows>0 && cols>0 ) {
      char buffer[1024];
      cols  = cols<sizeof(buffer)?cols:sizeof(buffer);

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
