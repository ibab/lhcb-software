// $Id: Logger.cpp,v 1.13 2010-04-15 16:04:59 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/Logger.cpp,v 1.13 2010-04-15 16:04:59 frankb Exp $

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
using namespace std;

/// Standard constructor
Logger::Logger(const string& name) : m_name(name)
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

/// Send data string to logger service
int Logger::sendData(const string& msg) {
  if ( 1 != ::dic_cmnd_service((char*)m_name.c_str(),(char*)msg.c_str(),msg.length()+1) ) {
    ::upic_write_message2("Failed to update history server:%s with data:%s",m_name.c_str(), msg.c_str());
    return 0;
  }
  //::upic_write_message2("%s: %s",m_name.c_str()+23,msg.c_str());
  return 1;
}

/// Connect to messages of a given source
int Logger::connectMessages(bool con, const string& name) {
  stringstream s;
  s << "Messages" << (const char*)(con ? "-" : "+") << name << ends;
  return sendData(s.str());
}

/// Connect to messages of a given source
int Logger::connectMessages(const vector<string>& names, const string& title) {
  stringstream s;
  s << title << ":";
  for(vector<string>::const_iterator i=names.begin();i!=names.end();++i)
    s << *i << ends;
  s << ends;
  return sendData(s.str());
}

/// Display callback handler
void Logger::handle(const Event& ev) {
  ioc_data data(ev.data);
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {
  case CMD_CANCEL:
    sendData("C:CANCEL_QUEUE");
    return;
  case CMD_EXECUTE:
    sendData("E:"+*data.str);
    delete data.str;
    return;
  case CMD_SHOW:
    sendData("D:"+*data.str);
    delete data.str;
    return;
  case CMD_START:
    sendData("T:"+*data.str);
    delete data.str;
    return;
  case CMD_SUMM_HISTORY:
    sendData("S:SummarizeAllHistory");
    return;
  case CMD_APPLY:
    sendData("F:"+*data.str);
    delete data.str;
    return;
  case CMD_NODE_HISTORY:
    sendData("Q:"+*data.str);
    delete data.str;
    return;
  case CMD_SEVERITY:
    sendData("L:"+*data.str);
    delete data.str;
    return;
  case CMD_CONNECT_MONITORING:
    sendData("X:"+*data.str);
    delete data.str;
    return;
  case CMD_CONNECT_STORAGE:
    sendData("Y:"+*data.str);
    delete data.str;
    return;
   case CMD_CONNECT_RECONSTRUCTION:
    sendData("Z:"+*data.str);
    delete data.str;
    return;
  case CMD_CONNECT_CLUSTER:
  case CMD_DISCONNECT_CLUSTER:
    connectMessages(ev.type==CMD_CONNECT_CLUSTER,*data.str);
    delete data.str;
    return;
  case CMD_UPDATE_RUNNUMBER:   {
    char text[64];
    ::snprintf(text,sizeof(text),"R:%ld",ev.iocData());
    sendData(text);
    return;
  }
  case CMD_UPDATE_FARMS:
    connectMessages(*data.vec,"Messages");
    delete data.vec;
    return;
  case CMD_SET_PARTITION:
    sendData("O:"+*data.str);
    delete data.str;
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
