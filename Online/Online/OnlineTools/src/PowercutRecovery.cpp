// $Id: PowercutRecovery.cpp,v 1.1 2008-09-18 13:08:22 frankb Exp $
//====================================================================
//  OnlineTools
//--------------------------------------------------------------------
//
//  Package    : OnlineTools
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineTools/src/PowercutRecovery.cpp,v 1.1 2008-09-18 13:08:22 frankb Exp $

// Framework include files
#include <sstream>
#include <fstream>
#include <cerrno>
#include "RTL/rtl.h"
#include "UPI/upidef.h"
#include "CPP/Event.h"
#include "CPP/IocSensor.h"
#include "ROLogger/Logger.h"
#include "OnlineTools/Processor.h"
#include "OnlineTools/ProcessorDisplay.h"
#include "OnlineTools/PowercutRecovery.h"
#include "Defs.h"

using namespace std;
using namespace OnlineTools;

/// Load nodes from file
static void load(const string& name, 
		 const string& group,
		 size_t flag, 
		 ProcessorGroup& items,
		 Interactor* m)
{
  char txt[128];
  ifstream input(name.c_str());
  if (input.is_open() ) {
    txt[0] = 0;
    while ( input.getline(txt,sizeof(txt)).good() ) {
      char* p = ::strchr(txt,' ');
      if ( p ) *p = 0;
      p = ::strstr(txt,"-ipmi");
      if ( p ) *p = 0;
      if ( strlen(txt) > 0 ) {
	string n = txt;
	string g = (flag==0) ? n : n.substr(0,flag);
	items.add(new Processor(n,m),group,g);
      }
      txt[0] = 0;
    }
  }
}

static void load(ProcessorGroup& items,Interactor* msg) {
  load("/admin/scripts/ipmi/d1allpc.txt","D1",4,items,msg);
  load("/admin/scripts/ipmi/d2allpc.txt","D2",2,items,msg);
  load("/admin/scripts/ipmi/d3allpc.txt","D3",0,items,msg);
}

/// Standard constructor
PowercutRecovery::PowercutRecovery(int, char**) 
{
  m_messageLog = new ROLogger::Logger(RTL::processName()+"_display");
  m_processors = new ProcessorGroup("All",m_messageLog);
  load(*m_processors,m_messageLog);
  m_display = new ProcessorDisplay(this,m_messageLog,m_processors);
}

/// Standard destructor
PowercutRecovery::~PowercutRecovery()  {
  if ( m_messageLog ) delete m_messageLog;
  if ( m_display ) delete m_display;
  ::upic_write_message("Close window.","");
  ::upic_quit();
  ::lib_rtl_sleep(200);
  ::exit(0);
}

void PowercutRecovery::help_fun() {
}

/// Shutdown client windows
void PowercutRecovery::shutdown() {
  if ( m_messageLog ) delete m_messageLog;
  m_messageLog = 0;
}

void PowercutRecovery::handle(const Event& ev) {
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {
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
extern "C" int powercutrecovery_display(int argc, char** argv) {
  int srows, scols, cols, rows, col, row;
  UpiSensor& s = UpiSensor::instance();
  PowercutRecovery mon(argc, argv);
  ::dis_start_serving((char*)RTL::processName().c_str());
  ::upic_get_screen_size(&srows, &scols);
  ::upic_get_message_window(&rows, &cols,&row, &col);
  ::upic_set_message_window (srows-15, scols-2, 16, 2);
  s.run();
  return 1;
}
