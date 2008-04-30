// $Id: Logger.cpp,v 1.1 2008-04-30 08:39:24 frankb Exp $
//====================================================================
//  ROLogger
//--------------------------------------------------------------------
//
//  Package    : ROLogger
//
//  Description: 
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/Logger.cpp,v 1.1 2008-04-30 08:39:24 frankb Exp $

#include "ROLogger/Logger.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "UPI/upidef.h"
#include "ROLoggerDefs.h"
#include <sstream>
extern "C" {
  #include "dic.h"
}

using namespace ROLogger;

namespace {
  union ioc_data {
    void* ptr;
    std::string* str;
    std::vector<std::string>* vec;
    ioc_data(void* p) : ptr(p) {}
  };
}

/// Standard constructor
Logger::Logger(const std::string& name) : m_name(name)
{
  ::upic_write_message2("Logging server:%s started ...",m_name.c_str());
}

/// Standard destructor
Logger::~Logger()  {
  shutdown();
}

/// Shutdown loggers
void Logger::shutdown() {
  int value = 1;
  if ( !m_name.empty() ) {
    ::dic_cmnd_service((char*)(m_name+"/EXIT").c_str(),&value,sizeof(value));
  }
}

/// Connect to messages of a given source
int Logger::connectMessages(bool con, const std::string& name, const std::string& item) {
  std::stringstream s;
  s << "Messages" << (const char*)(con ? "-" : "+") 
    << "/" << name << "/" << item << "/log" << std::ends << std::ends;
  std::string msg = s.str();
  ::dic_cmnd_service((char*)m_name.c_str(),(char*)msg.c_str(),msg.length()+1);
  return 1;
}

/// Connect to messages of a given source
int Logger::connectMessages(const std::vector<std::string>& names, const std::string& item) {
  std::stringstream s;
  s << "Messages:";
  for(std::vector<std::string>::const_iterator i=names.begin();i!=names.end();++i) {
    std::string n = *i;
    if ( n == "STORE" ) n = "STORECTL01";
    s << "/" << n << "/" << item << "/log" << std::ends;
  }
  s << std::ends;
  std::string msg = s.str();
  ::dic_cmnd_service((char*)m_name.c_str(),(char*)msg.data(),msg.length()+1);
  return 1;
}

/// Connect to processing cluster of a given name
int Logger::connectCluster(bool con, const std::string& name)   {
  std::string c = name;
  if ( c == "STORE" ) c = "STORECTL01";
  for(size_t i=0; i<c.length();++i) c[i]=::toupper(c[i]);
  return connectMessages(con, c, "gaudi");
}

/// Show history of a given node
void Logger::showNodeHistory(const std::string& node) {
  std::string data = "History:/"+node+"/history/last_log";
  if ( 1 != ::dic_cmnd_service((char*)m_name.c_str(),(char*)data.c_str(),data.length()+1) ) {
    ::upic_write_message2("Failed to update history server:%s",m_name.c_str());
    return;
  }
  ::upic_write_message2("Show history for node:%s",node.c_str());
}

/// Show farm history for all nodes of a farm
void Logger::showClusterHistory(const Nodes& nodes) {
  try {
    for(Nodes::const_iterator n=nodes.begin(); n!=nodes.end(); ++n)
      showNodeHistory(*n);
  }
  catch(std::exception& e) {
    ::upic_write_message2("Failed to show history for cluster:%s",e.what());
  }
  catch(...) {
    ::upic_write_message2("Failed to show history for cluster.");
  }
}

/// Display callback handler
void Logger::handle(const Event& ev) {
  ioc_data data(ev.data);
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {
    case CMD_FARM_HISTORY:
      showClusterHistory(*data.vec);
      delete data.vec;
      return;
    case CMD_NODE_HISTORY:
      showNodeHistory(*data.str);
      delete data.str;
      return;
    case CMD_CONNECT_STORAGE:
    case CMD_DISCONNECT_STORAGE:
      connectMessages(ev.type==CMD_CONNECT_STORAGE,"STORECTL01","gaudi");
      return;
    case CMD_CONNECT_MONITORING:
    case CMD_DISCONNECT_MONITORING:
      connectMessages(ev.type==CMD_CONNECT_MONITORING,"MONA08","gaudi");
      return;
    case CMD_CONNECT_CLUSTER:
    case CMD_DISCONNECT_CLUSTER:
      connectCluster(ev.type==CMD_CONNECT_CLUSTER,*data.str);
      delete data.str;
      return;
    case CMD_UPDATE_FARMS:
      connectMessages(*data.vec,"gaudi");
      delete data.vec;
      return;
    default:
      break;
    }
    break;
  case UpiEvent:
    break;
  default:
    break;
  }
}
