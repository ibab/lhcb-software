// $Id: RODIMLogger.cpp,v 1.1 2008-04-28 15:16:40 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/RODIMLogger.cpp,v 1.1 2008-04-28 15:16:40 frankb Exp $

// Framework include files
extern "C" {
#include "dic.h"
}
#include "dis.hxx"
#include <cerrno>
#include <sys/ioctl.h>
#include "RTL/rtl.h"
#include "ROMonDefs.h"
#include "ROMon/RODIMLogger.h"

#define inverse(fp)          fprintf(fp,"\033[7m")
#define underline(fp)        fprintf(fp,"\033[4;30m")
#define bold(fp)             fprintf(fp,"\033[1m")
#define normal(fp)           fprintf(fp,"\033[27m")
#define clear_screen(fp)     fprintf(fp,"\033[2J")
#define flash(fp)            fprintf(fp,"\033[5m")
#define plain(fp)            fprintf(fp,"\033[0m")
#define narrow_screen(fp)    fprintf(fp,"\033[?3l")
#define wide_screen(fp)      fprintf(fp,"\033[?3h")
#define cursor_on(fp)        fprintf(fp,"\033[?25h")
#define cursor_off(fp)       fprintf(fp,"\033[?25l")

using namespace ROMon;

/// Standard constructor
RODIMLogger::RODIMLogger(int argc, char** argv) 
: m_colors(true)
{
  std::string name;
  RTL::CLI cli(argc, argv, help_fun);
  cli.getopt("service",1,name);
  m_colors = cli.getopt("colors",1) != 0;
  if ( name.empty() ) {
    log() << "You have to supply a service name to display its data." << std::endl;
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
      fprintf(fp,"\033[0;37;40m");
      break;
    case 1: /* VERBOSE - blue */
      fprintf(fp,"\033[0;34;40m");
      break;
    case 2: /* DEBUG - green */
      fprintf(fp,"\033[0;32;40m");
      break;
    case 3: /* INFO - white */
      fprintf(fp,"\033[0;37;40m");
      break;
    case 4: /* WARN - bold yellow */
      fprintf(fp,"\033[0;1;33;40m");
      break;
    case 5: /* ERROR - bold red */
      fprintf(fp,"\033[0;1;31;40m");
      break;
    case 6: /* FATAL - bold magenta */
      fprintf(fp,"\033[0;1;35;40m");
      break;
    } 
  }
  fprintf(fp,msg);
  if ( crlf ) fprintf(fp,"\n");
  if ( m_colors ) fprintf(fp,"\033[0m");
}

/// Print header information before starting output
void RODIMLogger::printHeader(FILE* fp, const std::string& title) {
#ifdef __linux
  size_t rows, cols;
  int fd = ::fileno(fp);
  if ( m_colors && ::isatty(fd) ) {
    struct winsize wns;
    do {
      if(::ioctl(fd,TIOCGWINSZ,&wns) == 0) {
	char buffer[1024];
        rows = wns.ws_row;
        cols = wns.ws_col;
	cols  = cols<sizeof(buffer)?cols:sizeof(buffer);

	//::clear_screen(fp);
	::inverse(fp);
	::bold(fp);

	::memset(buffer,' ',cols);
	buffer[cols-1] = 0;	
	::fprintf(fp,buffer);
	::fprintf(fp,"\n");
	::memcpy(&buffer[20],title.c_str(),title.length());
	::fprintf(fp,buffer);
	::fprintf(fp,"\n");
	::memset(&buffer[20],' ',title.length());
	buffer[cols-1] = 0;
	::fprintf(fp,buffer);
	::fprintf(fp,"\n");
	::normal(fp);
	::plain(fp);
	::fprintf(fp,"\n");
        return;
      }
    } while (errno == EINTR);   
  } 
#endif
  ::fprintf(fp,"\n                        Logger history of %s\n\n",title.c_str());
}

/// Print multi-line header information before starting output
void RODIMLogger::printHeader(FILE* fp, const std::vector<std::string>& titles) {
#ifdef __linux
  size_t rows, cols;
  int fd = ::fileno(fp);
  if ( m_colors && ::isatty(fd) ) {
    struct winsize wns;
    do {
      if(::ioctl(fd,TIOCGWINSZ,&wns) == 0) {
	char buffer[1024];
        rows = wns.ws_row;
        cols = wns.ws_col;
	cols  = cols<sizeof(buffer)?cols:sizeof(buffer);

	//::clear_screen(fp);
	::inverse(fp);
	::bold(fp);

	::memset(buffer,' ',cols);
	buffer[cols-1] = 0;	
	::fprintf(fp,buffer);
	::fprintf(fp,"\n");
	for(std::vector<std::string>::const_iterator i=titles.begin();i!=titles.end();++i) {
	  const std::string& title = *i;
	  ::memcpy(&buffer[20],title.c_str(),title.length());
	  ::fprintf(fp,buffer);
	  ::fprintf(fp,"\n");
	  ::memset(&buffer[20],' ',title.length());
	  buffer[cols-1] = 0;
	}

	::fprintf(fp,buffer);
	::fprintf(fp,"\n");
	::normal(fp);
	::plain(fp);
	::fprintf(fp,"\n");
        return;
      }
    } while (errno == EINTR);   
  } 
#endif
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
  log () << "Received invalid request." << std::endl;
}

/// DIM command service callback
void RODIMLogger::handleMessages(const char* items, const char* end) {
  if ( items ) {
    std::vector<std::string> titles;
    time_t now = ::time(0);
    for(const char* p=items; p < end; p += ::strlen(p)+1 ) {
      if ( strlen(p)>0 ) {
	Services::iterator i=m_infos.find(p);
	if ( i != m_infos.end() ) {
	  Entry* e = m_infos[p] = new Entry;
	  log() << "Adding client:" << p << std::endl;
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
    time_t now = ::time(0);
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
    //log() << "Removing client:" << e->name << std::endl;
    ::dic_release_service(e->id);
    delete e;
  }
}

/// DIM command service callback
void RODIMLogger::handleHistory(const std::string& nam) {
  time_t now = ::time(0);
  Entry* e = m_infos[nam] = new Entry;
  log() << "Adding client:" << nam << std::endl;
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
	log() << "Release client:" << e->name << std::endl;
	e->self->cleanupService((*i).second);
	s.erase(i);
	break;
      }
    }
  }
}

extern "C" int romon_logger(int argc, char** argv) {
  RODIMLogger mon(argc, argv);
  DimServer::start(RTL::processName().c_str());
  log() << "Going asleep" << std::endl;
  while(1) ::lib_rtl_sleep(1000);
  return 1;
}
