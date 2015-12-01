// $Id: ErrorShow.cpp,v 1.2 2008-06-05 09:42:15 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/ErrorShow.cpp,v 1.2 2008-06-05 09:42:15 frankb Exp $

// Framework include files
#include <sstream>
#include <cerrno>
#include "RTL/rtl.h"
#include "UPI/upidef.h"
#include "CPP/Event.h"
#include "CPP/IocSensor.h"
#include "ROLogger/Logger.h"
#include "ROLogger/ErrorShow.h"
#include "ROLogger/ErrShowDisplay.h"
#include "ROLoggerDefs.h"

using namespace ROLogger;

/// Standard constructor
ErrorShow::ErrorShow(int, char**)    {
  m_msg = new Logger(RTL::processName()+"_display");
  m_display = new ErrShowDisplay(this,m_msg);
}

/// Standard destructor
ErrorShow::~ErrorShow()  {
  delete m_display;
  if ( m_msg ) delete m_msg;
  ::upic_write_message("Close window.","");
  ::upic_quit();
  ::lib_rtl_sleep(200);
  ::exit(0);
}

void ErrorShow::help_fun() {
}

/// Shutdown client windows
void ErrorShow::shutdown() {
  if ( m_msg ) delete m_msg;
  m_msg = 0;
}

void ErrorShow::handle(const CPP::Event& ev) {
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {
  case CMD_DELETE:
    delete this;
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

extern "C" {
#include "dis.hxx"
}
#include "UPI/UpiSensor.h"
extern "C" int rologger_error_show(int argc, char** argv) {
  int srows, scols, cols, rows, col, row;
  UpiSensor& s = UpiSensor::instance();
  ErrorShow mon(argc, argv);
  ::dis_start_serving((char*)RTL::processName().c_str());
  ::upic_get_screen_size(&srows, &scols);
  ::upic_get_message_window(&rows, &cols,&row, &col);
  ::upic_set_message_window (srows-15, scols-2, 16, 2);
  s.run();
  return 1;
}
