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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineTools/src/ProcessorDisplay.cpp,v 1.4 2010-09-17 10:05:18 frankb Exp $

// Framework include files
#include "OnlineTools/ProcessorDisplay.h"
#include "OnlineTools/Processor.h"
#include "UPI/UpiSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "UPI/upidef.h"
#include "RTL/rtl.h"
#include <sstream>
#include <cstring>
#include "Defs.h"

using namespace std;
using namespace OnlineTools;

static const char* s_action[]  = {"Ping","Status","On","Off","Reset"};
static char  s_action_buff[80];

static string setupParams(const string& name, bool flag=false) {
  char text[132];
  int ipar = 0;
  if ( flag ) {
    const char* list[] = {name.c_str()};
    ::sprintf(text,"^^^^^^^^^^^^^^  ^^^^^^");
    ::upic_set_param(s_action_buff,++ipar,"A14",list[0],0,0,list,1,1);
  }
  else {
    ::sprintf(text,"%-10s ^^^^^^",name.c_str());
  }
  ::upic_set_param(s_action_buff,++ipar,"A6",s_action[0],0,0,s_action,5,1);
  return text;
}

/// Standard constructor
ProcessorDisplay::ProcessorDisplay(CPP::Interactor* par, CPP::Interactor* msg, NodeManipulator* items) 
  : m_parent(par), m_msg(msg), m_nodes(items), m_cursor(CMD_CLOSE)
{
  char txt[128];
  int count = 0;
  NodeManipulator::Children c = items->children();
  m_id = UpiSensor::instance().newID();
  ::upic_open_window();
  ::upic_open_menu(m_id,0,0,"Processor Control",m_nodes->name().c_str(),RTL::nodeName().c_str());

  ::upic_add_command(CMD_CLOSE,        "Close","");
  ::upic_add_command(CMD_CANCEL,       "Cancel","");
  ::upic_add_comment(CMD_COM1,         "-----------","");
  ::upic_add_param_line(CMD_APPLY,setupParams("All devices",false).c_str(),"");
  ::upic_add_comment(CMD_COM2,         "-----------","");
  for(NodeManipulator::Children::iterator i=c.begin();i!=c.end();++i) {
    if ( (*i)->numChildren()>0 ) {
      ::sprintf(txt,"%s [%ld]",(*i)->name().c_str(),long((*i)->numChildren()));
      ::upic_add_param_line(++count,setupParams(txt,(*i)->hasChildren()).c_str(),"");
    }
    else {
      ::upic_add_param_line(++count,setupParams((*i)->name(),(*i)->hasChildren()).c_str(),"");
    }
  }
  ::upic_close_menu();
  UpiSensor::instance().add(this,m_id);
}

/// Standard destructor
ProcessorDisplay::~ProcessorDisplay()  {
  UpiSensor::instance().remove(this,m_id);
  ::upic_delete_menu(m_id);
  ::upic_write_message("Close window.","");
}

void ProcessorDisplay::handle(const CPP::Event& ev) {
  IocSensor& ioc = IocSensor::instance();
  NodeManipulator::Children::iterator i;
  NodeManipulator::Children c;
  NodeManipulator* manip;
  int (NodeManipulator::*pmf)()=0;
  char* p;
  int par;

  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {
    case CMD_DELETE:
      upic_set_cursor(m_id,m_cursor,0);
      delete (Interactor*)ev.data;
      break;
    default:
      break;
    }
    break;
  case UpiEvent:
    if ( (p=::strchr(s_action_buff,' ')) ) *p = 0;
    if ( ::strcmp(s_action_buff,s_action[0])==0 )
      pmf = &NodeManipulator::doPing;	   // PING
    else if ( ::strcmp(s_action_buff,s_action[1])==0 )
      pmf = &NodeManipulator::doStatus;       // STATUS
    else if ( ::strcmp(s_action_buff,s_action[2])==0 )
      pmf = &NodeManipulator::doSwitchON;     // Switch ON
    else if ( ::strcmp(s_action_buff,s_action[3])==0 )
      pmf = &NodeManipulator::doSwitchOFF;    // Switch OFF
    else if ( ::strcmp(s_action_buff,s_action[4])==0 )
      pmf = &NodeManipulator::doReset;        // Reset

    switch(ev.command_id) {
    case CMD_CLOSE:
      ioc.send(m_parent,CMD_DELETE,this);
      return;
    case CMD_CANCEL:
      ioc.send(m_msg,CMD_CANCEL,this);
      return;
    case CMD_APPLY:
      c=m_nodes->children();
      for( i=c.begin(); pmf!=0 && i!=c.end(); ++i) ((*i)->*pmf)();
      ioc.send(m_msg,CMD_SHOW,new string("Execute>    [WARN]   Command queued for execution"));
      return;
    default:
      if ( ev.command_id >0 && ev.command_id<=(int)m_nodes->numChildren() )  {
	c=m_nodes->children();
	manip = c[ev.command_id-1];
	par = manip->hasChildren() ? ev.param_id-1 : ev.param_id;
	m_cursor = ev.command_id;
	switch(par) {
	case 0:
	  new ProcessorDisplay(this,m_msg,manip);
	  return;
	case 1:
	  if ( pmf ) (manip->*pmf)();
	  ioc.send(m_msg,CMD_SHOW,new string("Execute>    [WARN]   Command queued for execution"));
	  return;
	default:
	  return;
	}
      }
    }
    return;
  default:  // Fall through: Handle request by client
    break;
  }
}
