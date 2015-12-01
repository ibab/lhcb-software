// $Id: ErrorLog.cpp,v 1.16 2010-04-15 16:04:59 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/ErrorLog.cpp,v 1.16 2010-04-15 16:04:59 frankb Exp $

// Framework include files
#include <sstream>
#include <cerrno>
#ifdef _WIN32
#include <cstring>
inline int strcasecmp(const char *s1, const char *s2) {
  return ::_stricmp(s1, s2);
}
inline int strncasecmp(const char *s1, const char *s2, size_t n) {
  return ::_strnicmp(s1, s2, n);
}
#else
#include <strings.h>
#endif
#include "RTL/rtl.h"
#include "UPI/upidef.h"
#include "CPP/Event.h"
#include "CPP/IocSensor.h"
#include "ROLogger/Logger.h"
#include "ROLogger/ErrorLog.h"
#include "ROLogger/PartitionDisplay.h"
#include "ROLogger/RecoListener.h"
#include "ROLogger/AlarmListener.h"
#include "ROLogger/PVSSLogListener.h"
#include "ROLogger/PartitionListener.h"

#include "ROLoggerDefs.h"
extern "C" {
#include "dic.h"
#include "dis.h"
}

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif
using namespace ROLogger;
using namespace std;

/// Standard constructor
ErrorLog::ErrorLog(int argc, char** argv) 
{
  RTL::CLI cli(argc, argv, help_fun);
  string name, facility;
  cli.getopt("service",1,name);
  cli.getopt("facility",1,facility);

  if ( strncasecmp(name.c_str(),"-pa",3)==0 ) {
    if ( name.find("=") != string::npos ) {
      name = name.substr(name.find("=")+1);
    }
  }
  else if ( strncasecmp(name.c_str(),"-re",3)==0 )
    name = "reco";

  if ( facility.empty() && name == "LHCb1" ) facility = "lhcb1";
  else if ( facility.empty() && name == "LHCb2" ) facility = "lhcb2";
  else if ( facility.empty() && name == "LHCbA" ) facility = "lhcba";
  else if ( facility.empty() ) facility = "gaudi";

  m_messageLog = new Logger(RTL::processName()+"_display");
  m_historyLog = new Logger(RTL::processName()+"_history");
  if ( name.empty() ) {
    ::upic_write_message("You have to supply a service name to display its data.","");
    ::lib_rtl_sleep(200);
    shutdown();
    IocSensor::instance().send(this,CMD_CLOSE,this);
    return;
  }
  m_partDisplay = new PartitionDisplay(this,m_messageLog,m_historyLog,name);
  if ( cli.getopt("pvss",3) != 0 )  
    m_partListener = new PVSSLogListener(this,name);
  else if ( cli.getopt("alarms",3) != 0 )  
    m_partListener = new AlarmListener(this,name);
  else if ( name == "reco" || cli.getopt("reco",3) != 0 )  
    m_partListener = new RecoListener(this,name);
  else
    m_partListener = new PartitionListener(this,name,facility);
}

/// Standard destructor
ErrorLog::~ErrorLog()  {
  delete m_partListener;
  delete m_partDisplay;
  if ( m_messageLog ) delete m_messageLog;
  if ( m_historyLog ) delete m_historyLog;
  ::upic_write_message("Close window.","");
  ::upic_quit();
  ::lib_rtl_sleep(200);
  ::exit(0);
}

void ErrorLog::help_fun() {
}

/// Shutdown client windows
void ErrorLog::shutdown() {
  if ( m_messageLog ) delete m_messageLog;
  m_messageLog = 0;
  if ( m_historyLog ) delete m_historyLog;
  m_historyLog = 0;
}

void ErrorLog::handle(const CPP::Event& ev) {
  IocSensor& ioc = IocSensor::instance();
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {
    case CMD_DISCONNECT_RECONSTRUCTION: // From PartitionListener
    case CMD_CONNECT_RECONSTRUCTION:    // From PartitionListener
    case CMD_DISCONNECT_MONITORING:     // From PartitionListener
    case CMD_CONNECT_MONITORING:        // From PartitionListener
    case CMD_DISCONNECT_STORAGE:        // From PartitionListener
    case CMD_CONNECT_STORAGE:           // From PartitionListener
    case CMD_UPDATE_NODES:              // From PartitionListener
    case CMD_UPDATE_FARMS:              // From PartitionListener
    case CMD_UPDATE_RUNNUMBER:          // From PartitionListener
    case CMD_SET_PARTITION:             // From PartitionListener
      ioc.send(m_partDisplay,ev.type,ev.data);
      return;
    case CMD_DELETE:
      delete this;
      ::lib_rtl_sleep(200);
      ioc.send(this,CMD_STOP,(void*)0);
      return;
    case CMD_STOP:
      ::exit(0);
      return;
    case CMD_CLOSE:
      ::lib_rtl_sleep(5000);
    ::exit(2);
    default:
      break;
    }
    break;
  default:
    break;
  }
}

#include "dis.hxx"
#include "UPI/UpiSensor.h"
extern "C" int romon_display_server(int argc, char** argv) {
  int srows, scols, cols, rows, col, row;
  UpiSensor& s = UpiSensor::instance();
  ErrorLog mon(argc, argv);
  ::dis_start_serving((char*)RTL::processName().c_str());
  ::upic_get_screen_size(&srows, &scols);
  ::upic_get_message_window(&rows, &cols,&row, &col);
  ::upic_set_message_window (srows-15, scols-2, 16, 2);
  s.run();
  return 1;
}
