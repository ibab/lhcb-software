//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/PartitionDisplay.cpp,v 1.1 2008-04-30 08:39:24 frankb Exp $

// Framework include files
#include "ROLogger/PartitionDisplay.h"
#include "ROLogger/ClusterDisplay.h"
#include "UPI/UpiSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "UPI/upidef.h"
#include "RTL/rtl.h"
#include "ROLoggerDefs.h"
extern "C" {
  #include "dic.h"
}
using namespace ROLogger;

static char  s_enable[]  = "Enable ";
static char  s_disable[] = "Disable";
static char  s_enableDisableResult[80];
static char  s_param_buff[80];

/// Standard constructor
PartitionDisplay::PartitionDisplay(Interactor* parent, Interactor* msg, Interactor* history, const std::string& name) 
  : m_name(name), m_parent(parent), m_msg(msg), m_history(history), m_clDisp(0)
{
  m_id = UpiSensor::instance().newID();
  ::upic_open_window();
  ::upic_open_menu(m_id,0,0,"Error logger",RTL::processName().c_str(),RTL::nodeName().c_str());
  ::upic_add_comment(CMD_COM0,("Known processor clusters for partition "+name).c_str(),"");
  ::upic_add_comment(CMD_COM1,"---------------------------------------------------------","");
  ::upic_add_comment(CMD_COM2,"---------------------------------------------------------","");
  ::upic_add_command(CMD_CLOSE,"Close","");
  ::upic_close_menu();
  UpiSensor::instance().add(this,m_id);
}

/// Standard destructor
PartitionDisplay::~PartitionDisplay()  {
  m_clDisp = std::auto_ptr<Interactor>(0);
  UpiSensor::instance().remove(this,m_id);
  ::upic_delete_menu(m_id);
  ::upic_write_message("Close window.","");
}

/// Show processor cluster display
void PartitionDisplay::showCluster(int cmd) {
  Nodes nodes;
  dim_lock();
  const std::string& name = m_items[cmd].second;
  for(Nodes::const_iterator i=m_nodes.begin();i!=m_nodes.end();++i) {
    if ( strncmp((*i).c_str(),name.c_str(),name.length())==0 ) 
      nodes.push_back(*i);
  }
  m_clDisp = std::auto_ptr<Interactor>(new ClusterDisplay(this,m_history,name,nodes));
  m_menuCursor = cmd;
  dim_unlock();
}

static std::string setupParams(const std::string& name, bool val) {
  char text[132];
  ::sprintf(text,"%-12s %-10s  ^^^^     ^^^^^^^     ^^^^^^^^^",
	    name.c_str(), val ? "[ENABLED]" : "[DISABLED]");
  ::upic_set_param(s_param_buff,1,"A4","Show",0,0,0,0,0);
  ::upic_set_param(s_enableDisableResult,2,"A7",val?s_disable:s_enable,0,0,0,0,0);
  ::upic_set_param(s_param_buff,3,"A9","Configure",0,0,0,0,0);
  return text;
}

/// Update farm content
void PartitionDisplay::updateFarms() {
  int nf = 0;
  for(Items::const_iterator i=m_items.begin(); i!=m_items.end(); ++i)
    ::upic_delete_command(m_id,(*i).first);
  nf = 0;
  m_items.clear();
  for(Farms::const_iterator f=m_farms.begin(); f!=m_farms.end(); ++f) {
    setupParams(*f,true);
    ::upic_insert_param_line(m_id, CMD_COM2, ++nf, setupParams(*f,true).c_str(), "");
    m_items[nf] = std::make_pair(true,(*f));
  }
}

void PartitionDisplay::handle(const Event& ev) {
  IocSensor& ioc = IocSensor::instance();
  switch(ev.eventtype) {
  case IocEvent:
    //::upic_write_message2("Got IOC command: %d %p",ev.type,ev.data);
    switch(ev.type) {
    case CMD_UPDATE_NODES:
      m_nodes = *(Nodes*)ev.data;
      delete (Nodes*)ev.data;
      ::upic_write_message2("Updating node content of %s [%ld nodes]",m_name.c_str(),m_nodes.size());
      return;
    case CMD_UPDATE_FARMS:
      m_farms = *(Farms*)ev.data;
      ::upic_write_message2("Updating farm content of %s [%ld nodes]",m_name.c_str(),m_farms.size());
      ioc.send(this,CMD_UPDATE_CLUSTERS,this);
      ioc.send(m_msg,CMD_UPDATE_FARMS,ev.data);
      return;
    case CMD_UPDATE_CLUSTERS:
      updateFarms();
      return;
    case CMD_DELETE_FARM_DISPLAY:
      m_clDisp = std::auto_ptr<Interactor>(0);
      ::upic_set_cursor(m_id,m_menuCursor,0);
      return;
    case CMD_DELETE_PART_DISPLAY:
      delete this;
      break;
    default:
      break;
    }
    break;
  case UpiEvent:
    //::upic_write_message2("Got UPI command: %d %d %d",ev.menu_id,ev.command_id,ev.param_id);
    switch(ev.command_id) {
    case CMD_CLOSE:
      ioc.send(m_parent,CMD_DELETE_PART_DISPLAY,this);
      return;
    default:
      if ( ev.command_id > 0 && ev.command_id <= (int)m_farms.size() ) {
	int val, cmd = ev.command_id;
	switch(ev.param_id) {
	case 1:
	  showCluster(cmd);
	  break;
	case 2:
	  val = m_items[cmd].first = ::strcmp(s_enableDisableResult,s_enable)==0;
	  ::upic_replace_param_line(m_id,cmd,setupParams(m_items[cmd].second,val).c_str(),"");
	  ::upic_set_cursor(ev.menu_id,cmd,ev.param_id);
	  ioc.send(m_msg,val ? CMD_DISCONNECT_CLUSTER : CMD_CONNECT_CLUSTER,new std::string(m_items[cmd].second));
	  break;
	case 3:
	  ::upic_write_message("Configuration by menu not implemented...","");
	  break;
	default:
	  break;
	}
      }
    }
    break;
  default:  // Fall through: Handle request by client
    break;
  }
}
