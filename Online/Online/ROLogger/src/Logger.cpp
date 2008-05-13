// $Id: Logger.cpp,v 1.3 2008-05-13 07:55:40 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/Logger.cpp,v 1.3 2008-05-13 07:55:40 frankb Exp $

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
  return sendData(s.str());
}

/// Send data string to logger service
int Logger::sendData(const std::string& msg) {
  if ( 1 != ::dic_cmnd_service((char*)m_name.c_str(),(char*)msg.c_str(),msg.length()+1) ) {
    ::upic_write_message2("Failed to update history server:%s with data:%s",m_name.c_str(), msg.c_str());
    return 0;
  }
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
  return sendData(s.str());
}

/// Connect to processing cluster of a given name
int Logger::connectCluster(bool con, const std::string& name)   {
  std::string c = name;
  if ( c == "STORE" ) c = "STORECTL01";
  for(size_t i=0; i<c.length();++i) c[i]=::toupper(c[i]);
  return connectMessages(con, c, "gaudi");
}

/// Display callback handler
void Logger::handle(const Event& ev) {
  ioc_data data(ev.data);
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {
    case CMD_SUMM_HISTORY:
      sendData("S:SummarizeAllHistory");
      return;      
    case CMD_CLEAR_HISTORY:
      sendData("C:ClearAllHistory");
      return;
    case CMD_APPLY_FILTERS:
      sendData("F:"+*data.str);
      delete data.str;
      return;
    case CMD_NODE_HISTORY:
      sendData("Q:"+*data.str);
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
