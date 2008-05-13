// $Id: MessageLogger.cpp,v 1.3 2008-05-13 07:55:40 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/MessageLogger.cpp,v 1.3 2008-05-13 07:55:40 frankb Exp $

// Framework include files
#include <cerrno>
#include <cstdarg>
#include <sstream>
#include <iostream>
#include "RTL/rtl.h"
#include "RTL/strdef.h"
#include "RTL/graphics.h"
#include "CPP/Event.h"

#include "ROLogger/MessageLogger.h"
#include "ROLogger/Filter.h"
#include "ROLoggerDefs.h"

extern "C" {
  #include "dic.h"
  #include "dis.h"
}

using namespace ROLogger;
using namespace graphics;

/// Standard constructor
MessageLogger::MessageLogger(int argc, char** argv) 
  : m_colors(true), m_historySize(0)
{
  std::string name, outfile = "logger.dat";
  RTL::CLI cli(argc, argv, help_fun);
  cli.getopt("service",1,name);
  cli.getopt("buffer",1,m_historySize);
  m_colors  = cli.getopt("colors",1) != 0;
  m_display = cli.getopt("display",1) != 0;
  m_output  = stdout;
  if ( name.empty() ) {
    ::lib_rtl_output(LIB_RTL_INFO,"You have to supply a service name to display its data.");
    ::lib_rtl_sleep(10000);
    ::exit(2);
  }
  ::lib_rtl_output(LIB_RTL_INFO,"Service name:         %s", name.c_str());
  ::lib_rtl_output(LIB_RTL_INFO,"Message buffer size:  %d",m_historySize);
  m_service = ::dis_add_cmnd((char*)name.c_str(),(char*)"C",requestHandler,(long)this);
  if ( m_colors ) bg_black();
}

/// Standard destructor
MessageLogger::~MessageLogger()  {
  if ( m_service ) {
    ::dis_remove_service(m_service);
    m_service = 0;
  }
  removeAllServices();
}

void MessageLogger::help_fun() {
}

/// Remove unconditionally all connected services
void MessageLogger::removeAllServices() {
  for ( Services::iterator i=m_infos.begin(); i!=m_infos.end(); ++i )
    cleanupService((*i).second);
  m_infos.clear();
}

/// Retrieve message severity from DIM logger message
int MessageLogger::msgSeverity(const char* msg) {
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
void MessageLogger::printMessage(const char* msg, bool crlf)  {
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
  ::fprintf(m_output,msg);
  if ( crlf ) ::fprintf(m_output,"\n");
  if ( m_colors ) ::plain();
  if ( m_output != stdout ) ::fflush(m_output);
}

void MessageLogger::updateHistory(const char* msg) {
  if ( m_historySize > 0 ) {
    m_history.push_back(msg);
    if ( m_history.size() > m_historySize ) m_history.pop_front();
  }
}

static std::string msg_src(const std::string& m) {
  size_t idx = m.find("]"), idq = m.find(":");
  if ( idx != std::string::npos && idq != std::string::npos ) {
    while(m[++idx]==' ');
    return m.substr(idx,idq-idx);
  }
  return "";
}

/// Print history records from stored memory
void MessageLogger::printHistory(const std::string& pattern) {
  char text[132];
  int  match = 0, displayed=0;
  size_t numMsg = 999999999;

  size_t idq, id1 = pattern.find("#Node:{"), id2 = pattern.find("#Msg:{"), id3 = pattern.find("#Num:{");
  std::string node_pattern = "*", msg_pattern = "*", tmp;
  std::vector<const char*> messages;
  if ( id1 != std::string::npos ) {
    idq = pattern.find("}",id1);
    node_pattern = pattern.substr(id1+7,idq-id1-7);
  }
  if ( id2 != std::string::npos ) {
    idq = pattern.find("}",id2);
    msg_pattern = pattern.substr(id2+6,idq-id2-6);
  }
  if ( id3 != std::string::npos ) {
    idq = pattern.find("}",id3);
    std::string tmp = pattern.substr(id3+6,idq-id3-6);
    ::sscanf(tmp.c_str(),"%d",&numMsg);
  }
  printHeader("Logger history of:"+node_pattern+" matching:"+msg_pattern);
  for(History::const_iterator i=m_history.begin(); i != m_history.end(); ++i) {
    const std::string& m = *i;
    const std::string  src = msg_src(m);
    if ( !src.empty() ) {
      if ( ::strcase_match_wild(src.c_str(),node_pattern.c_str()) ) {
	if ( ::strcase_match_wild(m.c_str()+idq+1,msg_pattern.c_str()) ) {
	  messages.push_back(m.c_str());
	  ++match;
	}
      }
    }
  }
  for(size_t n=messages.size(), j=n>numMsg ? n-numMsg : 0; j<n; ++j, ++displayed)  
    printMessage(messages[j], false);
  ::sprintf(text,"History>    [WARN]  %d %s%s [%s]. %zd %s %d messages replayed.",
	    match,"messages matched the request:",node_pattern.c_str(),
	    msg_pattern.c_str(),numMsg,"messages requested.",displayed);
  printMessage(text,true);
}

/// Print summary of history records from stored memory
void MessageLogger::summarizeHistory() {
  typedef std::map<std::string,std::vector<int> > DataMap;
  int sev, num_msg=0;
  char text[512];
  DataMap data;
  printHeader("Logger history summary");
  for(History::const_iterator i=m_history.begin(); i != m_history.end(); ++i) {
    const std::string& m = *i;
    std::string src = msg_src(m);
    if ( !src.empty() ) {
      DataMap::iterator i = data.find(src);
      if ( i==data.end() ) data[src].resize(7);
      sev = msgSeverity(m.c_str());
      data[src][sev]++;
      num_msg++;
    }
  }
  for(DataMap::const_iterator j=data.begin(); j!=data.end();++j)  {
    const std::vector<int>& v = (*j).second;
    ::sprintf(text,"HistorySumm>[INFO] %-16s has sent %5d VERBOSE  %5d DEBUG  %5d INFO  %4d WARNING  %4d ERROR and %4d FATAL messages.",
	      (*j).first.c_str(), v[1], v[2], v[3], v[4], v[5], v[6]);
    printMessage(text,true);
  }
  ::sprintf(text,"HistorySumm>[WARN] Analysed a total of %d messages.",num_msg);
  printMessage(text,true);
}

/// Print header information before starting output
void MessageLogger::printHeader(const std::string& title) {
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
  ::fprintf(m_output,"\n                        Logger history of %s\n\n",title.c_str());
}

/// Print multi-line header information before starting output
void MessageLogger::printHeader(const std::vector<std::string>& titles) {
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
    ::fprintf(m_output,"                   -> %s\n",(*i).c_str());
}

/// Clear all history content
void MessageLogger::clearHistory() {
  size_t s = m_history.size();
  char text[132];
  m_history.clear();
  ::sprintf(text,"clear>      [WARN] .... Clear all history .... Deleted %zd messages",s);
  printMessage(text,true);
}

/// Load filters from string represntation
void MessageLogger::loadFilters(const std::string& s) {
  std::stringstream str(s);
  Filter filter;
  m_filters.clear();
  while(filter.read(str)>0) m_filters.push_back(filter);
  printHeader("Message filters:");
  for(Filters::const_iterator i=m_filters.begin();i!=m_filters.end();++i) {
    (*i).dump(std::cout);
  }
}

/// DIM command service callback
void MessageLogger::requestHandler(void* tag, void* address, int* size) {
  MessageLogger* h = *(MessageLogger**)tag;
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
  case 'Q': // Wildcard Messages mode
    idx = n.find(":");
    if ( idx != std::string::npos ) {
      h->printHistory(n.substr(idx+1));
    }
    return;
  case 'C': // Clear history
    h->clearHistory();
    return;
  case 'S': // Summarize history
    h->summarizeHistory();
    return;
  case 'F': // Load filters
    idx = n.find(":");
    if ( idx != std::string::npos ) {
      h->loadFilters(n.substr(idx+1));
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
  h->printMessage(">>>>>>>>>>> [ERROR] Received invalid request.");
}

/// DIM command service callback
void MessageLogger::handleMessages(const char* items, const char* end) {
  if ( items ) {
    std::vector<std::string> titles;
    time_t now = ::time(0);
    for(const char* p=items; p < end; p += ::strlen(p)+1 ) {
      if ( strlen(p)>0 ) {
	Services::iterator i=m_infos.find(p);
	if ( i == m_infos.end() ) {
	  Entry* e = m_infos[p] = new Entry;
	  ::lib_rtl_output(LIB_RTL_INFO,"Adding client:%s",p);
	  e->id      = ::dic_info_service((char*)p,MONITORED,0,0,0,messageInfoHandler,(long)e,0,0);
	  e->created = now;
	  e->self    = this;
	  e->name    = p;
	}
      }
    }
    for(Services::iterator i=m_infos.begin();i!=m_infos.end();++i) 
      titles.push_back((*i).first);
    printHeader(titles);
  }
}

/// DIM command service callback
void MessageLogger::handleRemoveMessages(const char* items, const char* end) {
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
    printHeader(titles);
  }
}

/// Cleanup service stack
void MessageLogger::cleanupServices(const std::string& match) {
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
void MessageLogger::cleanupService(Entry* e) {
  if ( e ) {
    ::dic_release_service(e->id);
    delete e;
  }
}

/// DIM command service callback
void MessageLogger::handleHistory(const std::string& nam) {
  time_t now = ::time(0);
  Entry* e = m_infos[nam] = new Entry;
  ::lib_rtl_output(LIB_RTL_INFO,"Adding client:%s", nam.c_str());
  e->id      = ::dic_info_service((char*)nam.c_str(),ONCE_ONLY,0,0,0,historyInfoHandler,(long)e,0,0);
  e->created = now;
  e->self    = this;
  e->name    = nam;
}

/// Check filters if this message should be printed....
bool MessageLogger::checkFilters(const char* msg) const {
  if ( msg ) {
    if ( !m_filters.empty() ) {
      for(Filters::const_iterator i=m_filters.begin();i!=m_filters.end();++i) {
	if ( !(*i).acceptMessage(msg) ) return false;
      }
    }
    return true;
  }
  return false;
}

/// Dim Info callback overload to process messages
void MessageLogger::messageInfoHandler(void* tag, void* address, int* size)  {
  if ( address && size && *size>0 ) {
    Entry* e = *(Entry**)tag;
    MessageLogger* l = e->self;
    const char* msg = (char*)address;
    l->updateHistory(msg);
    if ( l->m_display )  {
      if ( l->checkFilters(msg) ) {
	l->printMessage(msg,false);
      }
      else {
	l->printMessage(" ---------------------------- IGNORED:",false);
	l->printMessage(msg,false);
      }
    }
  }
}

/// Dim Info callback overload to process messages
void MessageLogger::historyInfoHandler(void* tag, void* address, int* size)  {
  if ( address && size && *size>0 ) {
    Entry* e = *(Entry**)tag;
    MessageLogger *logger = e->self;
    Services& s = logger->m_infos;
    char *msg = (char*)address, *end = msg + *size, *ptr = msg;
    std::string title = "Logger history of:";
    title += e->name;
    logger->printHeader(title);
    while (ptr<=end) {
      char* p = strchr(ptr,'\n');
      if ( p ) {
	*p = 0;
	logger->printMessage(ptr);
	ptr = p;
      }
      ++ptr;
    }
    for ( Services::iterator i=s.begin(); i != s.end(); ++i ) {
      if ( (*i).second == e ) {
	::lib_rtl_output(LIB_RTL_INFO,"Release client:%s", e->name.c_str());
	logger->cleanupService((*i).second);
	s.erase(i);
	break;
      }
    }
  }
}

/// Display callback handler
void MessageLogger::handle(const Event& ) {
  ::lib_rtl_output(LIB_RTL_INFO,"Received unknown input.....");
}

static size_t do_output(void*,int,const char* fmt, va_list args) {
  char buffer[1024];
  ::vsnprintf(buffer, sizeof(buffer), fmt, args);
  return ::printf("%s\n",buffer);
}

extern "C" int romon_logger(int argc, char** argv) {
  ::lib_rtl_install_printer(do_output,0);
  MessageLogger mon(argc, argv);
  ::dis_start_serving((char*)RTL::processName().c_str());
  while(1) ::lib_rtl_sleep(1000);
  return 1;
}
