// $Id: ErrorLog.cpp,v 1.11 2008-11-19 11:09:38 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/ErrorLog.cpp,v 1.11 2008-11-19 11:09:38 frankb Exp $

// Framework include files
#include <sstream>
#include <cerrno>
#include "RTL/rtl.h"
#include "UPI/upidef.h"
#include "CPP/Event.h"
#include "CPP/IocSensor.h"
#include "ROLogger/Logger.h"
#include "ROLogger/ErrorLog.h"
#include "ROLogger/PartitionDisplay.h"
#include "ROLogger/AlarmListener.h"
#include "ROLogger/PVSSLogListener.h"
#include "ROLogger/PartitionListener.h"

#include "ROLoggerDefs.h"
extern "C" {
#include "dic.h"
#include "dis.h"
}

using namespace ROLogger;

/// Standard constructor
ErrorLog::ErrorLog(int argc, char** argv) 
{
  RTL::CLI cli(argc, argv, help_fun);
  std::string name, info;
  cli.getopt("service",1,name);
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
  if ( cli.getopt("pvss",1) == 0 )  
    m_partListener = new PartitionListener(this,name);
  else if ( cli.getopt("alarms",1) == 0 )  
    m_partListener = new AlarmListener(this,name);
  else
    m_partListener = new PVSSLogListener(this,name);
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

void ErrorLog::handle(const Event& ev) {
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
      ioc.send(m_partDisplay,ev.type,ev.data);
      return;
    case CMD_DELETE:
      delete this;
      ::lib_rtl_sleep(200);
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
