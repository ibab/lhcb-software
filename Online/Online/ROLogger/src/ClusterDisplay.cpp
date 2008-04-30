// $Id: ClusterDisplay.cpp,v 1.1 2008-04-30 08:39:24 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/ClusterDisplay.cpp,v 1.1 2008-04-30 08:39:24 frankb Exp $

#include "ROLogger/ClusterDisplay.h"
#include "UPI/UpiSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "UPI/upidef.h"
#include "RTL/rtl.h"
#include "ROLoggerDefs.h"

using namespace ROLogger;

/// Standard constructor with object setup through parameters
ClusterDisplay::ClusterDisplay(Interactor* parent, Interactor* logger, const std::string& name, const Nodes& nodes) 
: m_name(name), m_nodes(nodes), m_parent(parent), m_logger(logger)
{
  int cnt = 0;
  if ( 0 == m_parent ) m_parent = this;
  m_id = UpiSensor::instance().newID();
  ::upic_open_detached_menu(m_id,0,0,"Error logger",m_name.c_str(),RTL::nodeName().c_str());
  ::upic_add_comment(CMD_COM1,("Known nodes for cluster "+m_name).c_str(),"");
  for(Nodes::const_iterator n=m_nodes.begin(); n!=m_nodes.end(); ++n, ++cnt) {
    if ( (*n).find(name) != std::string::npos ) {
      ::upic_add_command(cnt+1, (*n).c_str(),"");
    }
  }
  ::upic_add_comment(CMD_COM2,        "----------------------------------------","");
  ::upic_add_command(CMD_FARM_HISTORY,"Show all histories","");
  ::upic_add_comment(CMD_COM4,        "----------------------------------------","");
  ::upic_add_command(CMD_CLOSE,       "Close","");
  ::upic_close_menu();
  ::upic_set_cursor(m_id,CMD_CLOSE,0);
  UpiSensor::instance().add(this,m_id);
}

/// Standard destructor
ClusterDisplay::~ClusterDisplay() {
  UpiSensor::instance().remove(this,m_id);
  ::upic_delete_menu(m_id);
}

/// Display callback handler
void ClusterDisplay::handle(const Event& ev) {
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {
    case CMD_DELETE_FARM_DISPLAY:
      delete this;
      ::upic_quit();
      ::lib_rtl_sleep(200);
      ::exit(0);
      break;
    default:
      break;
    }
    break;
  case UpiEvent:
    if ( ev.menu_id == m_id ) {
      IocSensor& ioc = IocSensor::instance();
      if ( ev.command_id == CMD_CLOSE )
	ioc.send(m_parent,CMD_DELETE_FARM_DISPLAY,this);
      else if ( ev.command_id == CMD_FARM_HISTORY )
	ioc.send(m_logger,CMD_FARM_HISTORY,new Nodes(m_nodes));
      else if ( ev.command_id > 0 && ev.command_id <= (int)m_nodes.size() )
	ioc.send(m_logger,CMD_NODE_HISTORY,new std::string(m_nodes[ev.command_id-1]));
    }
    break;
  default:
    break;
  }
}

