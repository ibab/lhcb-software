// $Id: MessageLogger.cpp,v 1.21 2010-09-07 13:07:55 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/MessageLogger.cpp,v 1.21 2010-09-07 13:07:55 frankb Exp $
// Framework include files
#include <cerrno>
#include <cstring>
#include <cstdarg>
#include <sstream>
#include <iostream>
#include "RTL/rtl.h"
#include "RTL/strdef.h"
#include "RTL/graphics.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"

#include "ROLogger/MessageLogger.h"
#include "ROLogger/MessageLine.h"
#include "ROLogger/Filter.h"
#include "ROLoggerDefs.h"
extern "C" {
#include "dic.h"
#include "dis.h"
}
#ifdef _WIN32
#define vsnprintf _vsnprintf
#define popen     _popen
#endif

using namespace ROLogger;
using namespace graphics;
using namespace std;

/// Extract the source of a message from the message line
static string msg_src(const string& m) {
  size_t idx = m.find("]"), idq = m.find(":");
  if ( idx != string::npos && idq != string::npos ) {
    while(m[++idx]==' '){}
    return m.substr(idx,idq-idx);
  }
  return "";
}

/// Standard constructor
MessageLogger::MessageLogger(int argc, char** argv) 
  : m_severity(3), m_colors(true), m_historySize(0), m_wrapped(false), m_runNo(0), m_printRunNo(0)
{
  string name;
  RTL::CLI cli(argc, argv, help_fun);
  cli.getopt("service",1,name);
  cli.getopt("buffer",1,m_historySize);
  m_colors  = cli.getopt("colors",1)  != 0;
  m_debug   = cli.getopt("debug",2)   != 0;
  m_display = cli.getopt("display",2) != 0;
  m_alarm   = cli.getopt("alarm",2)   != 0;
  m_output  = stdout;
  m_numMsg.resize(MessageLine::Msg_Always+1);
  m_history.resize(m_historySize);
  for(History::iterator i=m_history.begin(); i != m_history.end(); ++i)
    (*i).reserve(200);
  if ( m_history.size()>0 ) m_histIter = m_history.begin();
  if ( name.empty() ) {
    ::lib_rtl_output(LIB_RTL_INFO,"You have to supply a service name to display its data.");
    ::lib_rtl_sleep(10000);
    ::exit(2);
  }
  ::lib_rtl_output(LIB_RTL_INFO,"Service name:         %s", name.c_str());
  ::lib_rtl_output(LIB_RTL_INFO,"Message buffer size:  %d", m_historySize);
  m_service = ::dis_add_cmnd((char*)name.c_str(),(char*)"C",requestHandler,(long)this);
  if ( m_colors ) bg_black();
}

/// Standard constructor, with initialisation
MessageLogger::MessageLogger(int sev, bool disp, bool colors)
: m_severity(sev), m_colors(colors), m_display(disp), m_historySize(0), m_wrapped(false)
{
  m_output  = stdout;
  m_history.resize(m_historySize);
  for(History::iterator i=m_history.begin(); i != m_history.end(); ++i)
    (*i).reserve(200);
  if ( m_history.size()>0 ) m_histIter = m_history.begin();
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
  using namespace std;
  cout 
    <<"Usage: romon_logger -option [-option]                                 "<<endl
    <<"        -buffer=<number>      History buffer size (Number of messages)"<<endl
    <<"        -service=<string>     Service name to accept commands.        "<<endl
    <<"        -colors               Use colors to indicate message severity "<<endl
    <<"        -display              Show recorded messages immeditely       "<<endl
    <<"                              rather than only recording to history   "<<endl
    <<endl;
}

/// Remove unconditionally all connected services
void MessageLogger::removeAllServices() {
  for ( Services::iterator i=m_infos.begin(); i!=m_infos.end(); ++i )
    cleanupService((*i).second);
  m_infos.clear();
}

/// Print single message retrieved from error logger
void MessageLogger::printMessage(const char* msg, bool crlf)  {
  int sev = MessageLine::msgSeverity(msg);
  bool is_pvss = m_partition.length()>0 && ::strstr(msg,m_partition.c_str())!=0;
  if ( m_numMsg.empty() ) m_numMsg.resize(MessageLine::Msg_Always+1);
  ++m_numMsg[sev];
  if ( sev >= m_severity || is_pvss ) {
    if ( m_colors ) {
      bg_black();
      switch(sev)      {
      case 0:             /* No label - white     */
        white();
        break;
      case MessageLine::Msg_Verbose:   /* VERBOSE - blue       */
        blue();
        break;
      case MessageLine::Msg_Debug:     /* DEBUG - green        */
        green();
        break;
      case MessageLine::Msg_Info:      /* INFO - white         */
        white();
        break;
      case MessageLine::Msg_Warning:   /* WARN - bold yellow   */
        bold(); yellow();
        break;
      case MessageLine::Msg_Error:     /* ERROR - bold red     */
        bold(); red();
        break;
      case MessageLine::Msg_Fatal:     /* FATAL - bold magenta */
        bold(); magenta();
        break;
      case MessageLine::Msg_Always:    /* ALWAYS - bold white  */
        bold(); white();
        break;
      } 
    }
    ::fputs(msg,m_output);
    if ( crlf ) ::fputc('\n',m_output);
    if ( m_colors ) ::plain();
    if ( m_output != stdout ) ::fflush(m_output);
  }
}

/// Print single message retrieved from error logger
void MessageLogger::printAlarm(const char* msg, bool crlf)  {
  int sev = MessageLine::msgSeverity(msg);
  bool is_pvss = m_partition.length()>0 && ::strstr(msg,m_partition.c_str())!=0;
  if ( m_numMsg.empty() ) m_numMsg.resize(MessageLine::Msg_Always+1);
  ++m_numMsg[sev];
  if ( sev >= m_severity || is_pvss ) {
    if ( m_colors ) {
      bg_black();
      switch(sev)      {
      case 0:             /* No label - white     */
        white();
        break;
      case MessageLine::Msg_Verbose:   /* VERBOSE - blue       */
        blue();
        break;
      case MessageLine::Msg_Debug:     /* DEBUG - green        */
        green();
        break;
      case MessageLine::Msg_Info:      /* INFO - white         */
        white();
        break;
      case MessageLine::Msg_Warning:   /* WARN - bold yellow   */
        bold(); yellow();
        break;
      case MessageLine::Msg_Error:     /* ERROR - bold red     */
        bold(); red();
        break;
      case MessageLine::Msg_Fatal:     /* FATAL - bold magenta */
        bold(); magenta();
        break;
      case MessageLine::Msg_Always:    /* ALWAYS - bold white  */
        bold(); white();
        break;
      } 
    }
    ::fputs(msg,m_output);
    if ( crlf ) ::fputc('\n',m_output);
    if ( m_colors ) ::plain();
    if ( m_output != stdout ) ::fflush(m_output);
  }
}

/// Update history records and flush obsolete records.
void MessageLogger::updateHistory(const char* msg) {
  if ( m_historySize > 0 ) {
    (*m_histIter) = msg;
    m_histIter++;
    if ( m_histIter == m_history.end() )  {
      m_histIter = m_history.begin();
      m_wrapped = true;
    }
  }
}

/// Print history records from stored memory
void MessageLogger::printHistory(const string& pattern) {
  char text[132];
  int  num=200, match = 0, displayed=0;
  size_t numMsg = num;
  size_t idq=0, id1=pattern.find("#Node:{"), id2=pattern.find("#Msg:{"), id3=pattern.find("#Num:{");
  string node_pattern = "*", msg_pattern = "*";
  vector<const char*> messages;

  if ( id1 != string::npos ) {
    idq = pattern.find("}",id1);
    node_pattern = pattern.substr(id1+7,idq-id1-7);
  }
  if ( id2 != string::npos ) {
    idq = pattern.find("}",id2);
    msg_pattern = pattern.substr(id2+6,idq-id2-6);
  }
  if ( id3 != string::npos ) {
    idq = pattern.find("}",id3);
    string tmp = pattern.substr(id3+6,idq-id3-6);
    if ( 1 == ::sscanf(tmp.c_str(),"%d",&num) ) {
      numMsg = num;
    }
  }
  printHeader("Logger history of:"+node_pattern+" matching:"+msg_pattern);
  History::iterator i = m_histIter, n = m_histIter;
  if ( m_wrapped ) {
    for(; i != m_history.end(); ++i) {
      const string& m = *i;
      const string  src = msg_src(m);
      if ( !src.empty() ) {
        if ( ::strcase_match_wild(src.c_str(),node_pattern.c_str()) ) {
          if ( ::strcase_match_wild(m.c_str()+idq+1,msg_pattern.c_str()) ) {
            int sev = MessageLine::msgSeverity(m.c_str());
            if ( sev >= m_severity ) {
              messages.push_back(m.c_str());
              ++match;
            }
          }
        }
      }
    }
  }
  for(i=m_history.begin(); i != n; ++i) {
    const string& m = *i;
    const string  src = msg_src(m);
    if ( !src.empty() ) {
      if ( ::strcase_match_wild(src.c_str(),node_pattern.c_str()) ) {
        if ( ::strcase_match_wild(m.c_str()+idq+1,msg_pattern.c_str()) ) {
          int sev = MessageLine::msgSeverity(m.c_str());
          if ( sev >= m_severity ) {
            messages.push_back(m.c_str());
            ++match;
          }
        }
      }
    }
  }
  for(size_t e=messages.size(), j=e>numMsg ? e-numMsg : 0; j<e; ++j, ++displayed)  
    printMessage(messages[j], false);
  ::sprintf(text,"History>    [ALWAYS]  %d %s%s [%s]. %d %s %d messages replayed.",
            match,"messages matched the request:",node_pattern.c_str(),
            msg_pattern.c_str(),(int)numMsg,"messages requested.",displayed);
  printMessage(text,true);
}

/// Print summary of history records from stored memory
void MessageLogger::summarizeHistory() {
  typedef map<string,vector<int> > DataMap;
  int sev, num_msg=0;
  char text[512];
  DataMap data;
  printHeader("Logger history summary");
  for(History::const_iterator i=m_history.begin(); i != m_history.end(); ++i) {
    const string& m = *i;
    if ( !m.empty() ) {
      string src = msg_src(m);
      if ( !src.empty() ) {
        DataMap::iterator di = data.find(src);
        if ( di==data.end() ) data[src].resize(MessageLine::Msg_Always+1);
        sev = MessageLine::msgSeverity(m.c_str());
        data[src][sev]++;
        num_msg++;
      }
    }
  }
  for(DataMap::const_iterator j=data.begin(); j!=data.end();++j)  {
    const vector<int>& v = (*j).second;
    ::sprintf(text,"%s %-12s sent %5d %s %6d %s %6d %s %5d %s %5d %s %4d %s and %4d %s msgs.",
      "Summary>    [ALWAYS]",(*j).first.c_str(),
      v[1], "VERBOSE",
      v[2], "DEBUG",
      v[3], "INFO",
      v[4], "WARNING",
      v[5], "ERROR",
      v[6], "FATAL",
      v[7], "ALWAYS");
    printMessage(text,true);
  }
  ::sprintf(text,"Summary>    [ALWAYS] Analysed a total of %d messages.",num_msg);
  printMessage(text,true);
}

string MessageLogger::getSummary() {
  char text[255];
  ::sprintf(text,"Total: %5d %s %6d %s %6d %s %5d %s %5d %s %4d %s and %4d %s Messages.",
    m_numMsg[1], "VERBOSE",
    m_numMsg[2], "DEBUG",
    m_numMsg[3], "INFO",
    m_numMsg[4], "WARNING",
    m_numMsg[5], "ERROR",
    m_numMsg[6], "FATAL",
    m_numMsg[7], "ALWAYS");
  return text;
}

/// Print header information before starting output
void MessageLogger::printHeader(const string& title, bool with_guards) {
  vector<string> v(1,title);
  printHeader(v,with_guards);
}

/// Print multi-line header information before starting output
void MessageLogger::printHeader(const vector<string>& titles, bool with_guards) {
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
      if ( with_guards ) {
	::fputs(buffer,stdout);
	::fputc('\n',stdout);
      }
      for(vector<string>::const_iterator i=titles.begin();i!=titles.end();++i) {
        const string& title = *i;
        ::memcpy(&buffer[20],title.c_str(),title.length()+1);
        ::fputs(buffer,stdout);
        ::fputc('\n',stdout);
        ::memset(&buffer[20],' ',title.length());
        buffer[cols-1] = 0;
      }
      if ( with_guards ) {
	::fputs(buffer,stdout);
	::fputc('\n',stdout);
      }
      ::normal();
      ::plain();
      ::fputc('\n',stdout);
      return;
    }
  } 
  for(vector<string>::const_iterator i=titles.begin();i!=titles.end();++i)
    ::fprintf(m_output,"                      %s\n",(*i).c_str());
}

/// Set partition name to filter PVSS names
void MessageLogger::setPartition(const string& part) {
  m_partition = "PVSS("+part+")";
}

/// Set message severity level for display
void MessageLogger::setMessageSeverity(const string& severity) {
  char text[132];
  switch(::toupper(severity[0])) {
  case 'V':
    m_severity = MessageLine::Msg_Verbose;
    break;
  case 'D':
    m_severity = MessageLine::Msg_Debug;
    break;
  case 'I':
    m_severity = MessageLine::Msg_Info;
    break;
  case 'W':
    m_severity = MessageLine::Msg_Warning;
    break;
  case 'E':
    m_severity = MessageLine::Msg_Error;
    break;
  case 'F':
    m_severity = MessageLine::Msg_Always;
    break;
  default:
    ::sprintf(text,"severity>   [ALWAYS] .... Keep old message severity level of %d",m_severity);
    printMessage(text,true);
    return;
  }
  ::sprintf(text,"severity>   [ALWAYS] .... New message severity level is %d [%s]",m_severity,severity.c_str());
  printMessage(text,true);
}

/// Execute command from message
void MessageLogger::execute(const string& cmd) {
  FILE * f = ::popen(cmd.c_str(),"r");
  if ( f ) {
    size_t siz;
    string result = "Execute>    [ALWAYS] ";
    char buff[1024];
    while((siz=::fread(buff,1,sizeof(buff)-1,f))>0) {
      buff[siz]=0;
      result += buff;
    }
    while((siz=result.find('%'))!=string::npos) result.replace(siz,1,"");
    printMessage(result.c_str(),false);
  }
  ::fclose(f);
}

/// Load filters from string represntation
void MessageLogger::loadFilters(const string& s) {
  stringstream str(s);
  Filter filter;
  m_filters.clear();
  while(filter.read(str)>0) m_filters.push_back(filter);
  printHeader("Message filters:");
  for(Filters::const_iterator i=m_filters.begin();i!=m_filters.end();++i) {
    (*i).dump(cout);
  }
}

/// Set new run number in callback
void MessageLogger::newRunNumber(const string& run) {
  errno = 0;
  int res = ::strtol(run.c_str(),0,10);
  if ( 0 == errno ) {
    m_runNo = res;
    if ( m_printRunNo >= 0 ) {
      for(int i=0; i<m_printRunNo;++i)
	printMessage( "                    ",true);
      printMessage(  ("                      Run:"+run+" started.").c_str(),true);
      for(int j=0; j<m_printRunNo;++j)
	printMessage( "                    ",true);
    }
    return;
  }
}

/// DIM command service callback
void MessageLogger::requestHandler(void* tag, void* address, int* size) {
  MessageLogger* h = *(MessageLogger**)tag;
  const char* p = (const char*)address;
  string n = p;
  size_t idx = n.find(":");
  if ( idx != string::npos ) {
    string nam = n.substr(++idx);
    switch(::toupper(n[0])) {
    case 'C': // Cancel command queue
      IocSensor::instance().sendHead(h,CMD_CANCEL,(void*)0);
      return;
    case 'E': // Execute command
      IocSensor::instance().send(h,CMD_START,new string(p+idx));
      return;
    case 'D': // Display line
      h->printMessage(p+idx,true);
      return;
    case 'F': // Load filters
      h->loadFilters(nam);
      return;
    case 'H':  // History mode
      h->cleanupServices(nam);
      h->handleHistory(nam);
      return;
    case 'L': // Set message severity level
      h->setMessageSeverity(nam);
      return;
    case 'M': // Messages mode
      h->removeAllServices();
      h->handleMessages(p+idx,p + (*size));
      return;
    case 'O': // Set partition
      h->setPartition(nam);
      return;
    case 'P': // Messages mode
      h->handleMessages(p+idx,p + (*size));
      return;
    case 'Q': // Wildcard Messages mode
      h->printHistory(nam);
      return;
    case 'S': // Summarize history
      h->summarizeHistory();
      return;
    case 'R': // Run number change
      h->newRunNumber(nam);
      return;
    case 'T': // Show title
      h->printHeader(nam);
      return;
    case 'X': // Connect monitoring slice
      h->cleanupServices(h->m_monitoring);
      h->m_monitoring = nam;
      h->handleMessages(p+idx,p + (*size));      
      return;
    case 'Y': // Connect storage slice
      h->cleanupServices(h->m_storage);
      h->m_storage = nam;
      h->handleMessages(p+idx,p + (*size));
      return;
    case 'Z': // Connect reconstruction slice
      h->cleanupServices(h->m_reconstruction);
      h->m_reconstruction = nam;
      h->handleMessages(p+idx,p + (*size));      
      return;
    default:
      break;
    }
  }
  switch(::toupper(n[0])) {
  case 'M': // Messages mode
    idx = n.find("+");
    if ( idx != string::npos ) {
      h->handleMessages(p+idx+1,p + (*size));
      return;
    }
    idx = n.find("-");
    if ( idx != string::npos ) {
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
    vector<string> titles;
    time_t now = ::time(0);
    for(const char* p=items; p < end; p += ::strlen(p)+1 ) {
      if ( strlen(p)>0 ) {
        Services::iterator i=m_infos.find(p);
        if ( i == m_infos.end() ) {
          Entry* e = m_infos[p] = new Entry;
          // ::lib_rtl_output(LIB_RTL_INFO,"Adding client:%s",p);
          e->id      = ::dic_info_service((char*)p,MONITORED,0,0,0,messageInfoHandler,(long)e,0,0);
          e->created = now;
          e->self    = this;
          e->name    = p;
        }
      }
    }
    if ( !m_partition.empty() ) titles.push_back("Configuration match:"+m_partition);
    for(Services::iterator i=m_infos.begin();i!=m_infos.end();++i) 
      titles.push_back((*i).first);
    printHeader(titles);
  }
}

/// DIM command service callback
void MessageLogger::handleRemoveMessages(const char* items, const char* end) {
  if ( items ) {
    vector<string> titles;
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
void MessageLogger::cleanupServices(const string& match) {
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
void MessageLogger::handleHistory(const string& nam) {
  time_t now = ::time(0);
  Entry* e = m_infos[nam] = new Entry;
  //::lib_rtl_output(LIB_RTL_INFO,"Adding client:%s", nam.c_str());
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
    l->handleMessage((const char*)address);
  }
}

/// Handle dim logger message
void MessageLogger::handleMessage(const char* msg) {
  updateHistory(msg);
  if ( m_display )  {
    if ( checkFilters(msg) ) {
      if ( m_alarm )
        printAlarm(msg,false);
      else
        printMessage(msg,false);
    }
    else if ( m_debug ) {
      printMessage(" ---------------------------- IGNORED:",false);
      printMessage(msg,false);
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
    string title = logger->m_alarm ? "Alarm history of:" : "Logger history of:";
    title += e->name;
    logger->printHeader(title);
    while (ptr<=end) {
      char* p = strchr(ptr,'\n');
      if ( p ) {
        *p = 0;
        logger->m_alarm ? logger->printAlarm(ptr) : logger->printMessage(ptr);
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
void MessageLogger::handle(const Event& ev) {
  ioc_data data(ev.data);
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {      
    case CMD_CANCEL:
      printMessage("Cancelled command queue",true);
      IocSensor::instance().clear();
      return;
    case CMD_START:
      execute(*data.str);
      delete data.str;
      return;
    default:
      break;
    }
    break;
  default:
    break;
  }
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
  IocSensor::instance().run();
  return 1;
}
