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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/PartitionDisplay.cpp,v 1.19 2010-04-15 16:04:59 frankb Exp $

// Framework include files
#include "ROLogger/PartitionDisplay.h"
#include "ROLogger/ClusterDisplay.h"
#include "ROLogger/ErrShowDisplay.h"
#include "ROLogger/FilterDisplay.h"
#include "UPI/UpiSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "UPI/upidef.h"
#include "RTL/rtl.h"
#include "ROLoggerDefs.h"
#include <sstream>
#include <cstring>
extern "C" {
#include "dic.h"
}
using namespace ROLogger;
using namespace std;

static const char* s_show[]    = {"Show"};
static const char* s_config[]  = {"Configure"};
static const char* s_enable[]  = {"Enable "};
static const char* s_disable[] = {"Disable"};
static const int   s_NumList[] = {1,10,50,100,200,300,500,1000,5000,10000};
static const char* s_SevList[] = {"VERBOSE","DEBUG","INFO","WARNING","ERROR","FATAL"};
static char  s_enableDisableResult[80];
static char  s_param_buff[80];

static string item_name(const string& svc_name) {
  size_t idx = svc_name.find("/",5);
  string name = svc_name.substr(5,idx-5);
  if ( name=="STORECTL01" ) name = "STORE";
  return name;
}

static string setupParams(const string& name, bool val) {
  char text[132];
  ::sprintf(text,"%-12s %-10s  ^^^^     ^^^^^^^     ^^^^^^^^^",
    item_name(name).c_str(), val ? "[ENABLED]" : "[DISABLED]");
  ::upic_set_param(s_param_buff,1,"A4",s_show[0],0,0,s_show,1,1);
  ::upic_set_param(s_enableDisableResult,2,"A7",val?s_disable[0]:s_enable[0],
    0,0,val?s_disable:s_enable,1,1);
  ::upic_set_param(s_param_buff,3,"A9",s_config[0],0,0,s_config,1,1);
  return text;
}


/// Standard constructor
PartitionDisplay::PartitionDisplay(Interactor* parent, Interactor* msg, Interactor* history, const string& name) 
: m_name(name), m_parent(parent), m_msg(msg), m_history(history), m_numMsg(200)
{
  m_id = UpiSensor::instance().newID();
  ::strcpy(m_wildNode,"*");
  ::strcpy(m_wildMessage,"*");
  ::strcpy(m_msgSeverity,s_SevList[3]);
  ::strcpy(m_histSeverity,s_SevList[2]);
  ::upic_open_window();
  ::upic_open_menu(m_id,0,0,"Error logger",RTL::processName().c_str(),RTL::nodeName().c_str());
  ::upic_add_comment(CMD_COM0,        ("Known processor clusters for partition "+name).c_str(),"");
  ::upic_add_comment(CMD_COM1,         "----------------------------------------------------------","");
  ::upic_add_comment(CMD_COM2,         "----------------------------------------------------------","");
  ::upic_set_param(m_msgSeverity,1, "A7",m_msgSeverity, 0,0,s_SevList,sizeof(s_SevList)/sizeof(s_SevList[0]),1);
  ::upic_set_param(m_histSeverity,2,"A7",m_histSeverity,0,0,s_SevList,sizeof(s_SevList)/sizeof(s_SevList[0]),1);
  ::upic_add_command(CMD_SEVERITY,     "Severity for messages ^^^^^^^ history: ^^^^^^^","");
  ::upic_set_param(&m_numMsg,1,"I6",m_numMsg,0,0,s_NumList,sizeof(s_NumList)/sizeof(s_NumList[0]),0);
  ::upic_add_command(CMD_COM3,         "Show                  ^^^^^^ Messages","");
  ::upic_set_param(m_wildNode,1,"A16",m_wildNode,0,0,0,0,0);
  ::upic_add_command(CMD_WILD_NODE,    "Node match:           ^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_wildMessage,1,"A16",m_wildMessage,0,0,0,0,0);
  ::upic_add_command(CMD_WILD_MESSAGE, "...and match message: ^^^^^^^^^^^^^^^","");
  ::upic_add_command(CMD_SUMM_HISTORY, "History summary","");
  ::upic_add_command(CMD_EDIT,         "Edit filters","");
  ::upic_add_comment(CMD_COM5,         "----------------------------------------------------------","");
  ::upic_add_command(CMD_FILE,         "Process history files","");
  ::upic_add_command(CMD_CLOSE,        "Close","");
  ::upic_close_menu();
  UpiSensor::instance().add(this,m_id);
  IocSensor::instance().send(m_msg,CMD_SEVERITY,new string(m_msgSeverity));
}

/// Standard destructor
PartitionDisplay::~PartitionDisplay()  {
  UpiSensor::instance().remove(this,m_id);
  ::upic_delete_menu(m_id);
  ::upic_write_message("Close window.","");
}

/// Show processor cluster display
void PartitionDisplay::showCluster(int cmd) {
  Nodes nodes;
  dim_lock();
  string name = item_name(m_items[cmd].second);
  for(Nodes::const_iterator i=m_nodes.begin();i!=m_nodes.end();++i) {
    if ( strncmp((*i).c_str(),name.c_str(),name.length())==0 ) 
      nodes.push_back(*i);
  }
  new ClusterDisplay(this,m_history,name,nodes);
  m_menuCursor = cmd;
  dim_unlock();
}

/// Show history according to node and message pattern match
void PartitionDisplay::showHistory(const char* node_match, const char* msg_match) {
  stringstream s;
  s << "#Node:{" << node_match << "}#Msg:{" << msg_match << "}#Num:{" << m_numMsg << "}" << ends;
  IocSensor::instance().send(m_history,CMD_NODE_HISTORY,new string(s.str()));
}

/// Update farm content
void PartitionDisplay::updateFarms() {
  int nf = 0;
  for(Items::const_iterator i=m_items.begin(); i!=m_items.end(); ++i)
    ::upic_delete_command(m_id,(*i).first);
  m_items.clear();
  for(Farms::const_iterator f=m_farms.begin(); f!=m_farms.end(); ++f) {
    if ( (*f).find("/pvssconfig") == string::npos ) {
      ::upic_insert_param_line(m_id, CMD_COM2, ++nf, setupParams(*f,true).c_str(), "");
      m_items[nf] = make_pair(true,(*f));
    }
  }
  if ( !m_storage.empty() ) {
    ::upic_insert_param_line(m_id, CMD_COM2, ++nf, setupParams(m_storage,true).c_str(), "");
    m_items[nf] = make_pair(true,m_storage);
  }
  if ( !m_monitoring.empty() ) {
    ::upic_insert_param_line(m_id, CMD_COM2, ++nf, setupParams(m_monitoring,true).c_str(), "");
    m_items[nf] = make_pair(true,m_monitoring);
  }
  if ( !m_reconstruction.empty() ) {
    ::upic_insert_param_line(m_id, CMD_COM2, ++nf, setupParams(m_reconstruction,true).c_str(), "");
    m_items[nf] = make_pair(true,m_reconstruction);
  }
  //  IocSensor::instance().send(this,CMD_COMLAST,this);
  ::upic_set_cursor(m_id,m_menuCursor=CMD_WILD_NODE,0);
}

void PartitionDisplay::handle(const Event& ev) {
  typedef vector<string> _SV;

  IocSensor& ioc = IocSensor::instance();
  ioc_data data(ev.data);
  switch(ev.eventtype) {
  case IocEvent:
    //::upic_write_message2("Got IOC command: %d %p",ev.type,ev.data);
    switch(ev.type) {
    case CMD_COMLAST:
      ::upic_set_cursor(m_id,m_menuCursor=CMD_WILD_NODE,0);
      return;
    case CMD_FILE:
      m_menuCursor = CMD_FILE;
      new ErrShowDisplay(this,m_msg, m_name);
      return;
    case CMD_UPDATE: 
      {
        _SV f = m_farms;
        f.push_back(m_monitoring);
        if ( !m_storage.empty() ) f.push_back(m_storage);
        if ( !m_monitoring.empty() ) f.push_back(m_monitoring);
        if ( !m_reconstruction.empty() ) f.push_back(m_reconstruction);
        ::upic_write_message2("Updating farm content of %s [%ld nodes]",m_name.c_str(),f.size());
        ioc.send(this,CMD_UPDATE_CLUSTERS,this);
        ioc.send(m_msg,CMD_UPDATE_FARMS,new _SV(f));
        ioc.send(m_history,CMD_UPDATE_FARMS,new _SV(f));
      }
      return;
    case CMD_SET_PARTITION:
      ioc.send(m_msg,CMD_SET_PARTITION,new string(*data.str));
      ioc.send(m_history,CMD_SET_PARTITION,new string(*data.str));
      delete data.str;
      return;
    case CMD_UPDATE_NODES:
      m_nodes = *(Nodes*)ev.data;
      delete (Nodes*)ev.data;
      ::upic_write_message2("Updating node content of %s [%ld nodes]",m_name.c_str(),m_nodes.size());
      return;
    case CMD_UPDATE_FARMS:
      m_farms = *(Farms*)ev.data;
      ioc.send(this,CMD_UPDATE,this);
      return;
    case CMD_CONNECT_RECONSTRUCTION:
      m_reconstruction = *data.str;
      delete data.str;
      ioc.send(this,CMD_UPDATE,this);
      return;
    case CMD_DISCONNECT_RECONSTRUCTION:
      m_reconstruction = "";
      ioc.send(this,CMD_UPDATE,this);
      return;
    case CMD_CONNECT_MONITORING:
      m_monitoring = *data.str;
      delete data.str;
      ioc.send(this,CMD_UPDATE,this);
      return;
    case CMD_DISCONNECT_MONITORING:
      m_monitoring = "";
      ioc.send(this,CMD_UPDATE,this);
      return;
    case CMD_CONNECT_STORAGE:
      m_storage = *data.str;
      delete data.str;
      ioc.send(this,CMD_UPDATE,this);
      return;
    case CMD_DISCONNECT_STORAGE:
      m_storage = "";
      ioc.send(this,CMD_UPDATE,this);
      return;
    case CMD_UPDATE_CLUSTERS:
      updateFarms();
      return;
    case CMD_UPDATE_RUNNUMBER:
      ioc.send(m_msg,ev.type,ev.data);
      ioc.send(m_history,ev.type,ev.data);
      ::upic_write_message2("Run: %ld started...",ev.iocData<long>());
      break;
    case CMD_DELETE:
      upic_set_cursor(m_id,m_menuCursor,0);
      delete (Interactor*)ev.data;
      break;
    default:
      break;
    }
    break;
  case UpiEvent:
    //::upic_write_message2("Got UPI command: %d %d %d",ev.menu_id,ev.command_id,ev.param_id);
    m_wildNode[sizeof(m_wildNode)-1] = 0;
    m_wildMessage[sizeof(m_wildMessage)-1] = 0;
    m_msgSeverity[sizeof(m_msgSeverity)-1] = 0;
    m_histSeverity[sizeof(m_histSeverity)-1] = 0;
    if ( ::strchr(m_wildNode,' ')     ) *::strchr(m_wildNode,' ') = 0;
    if ( ::strchr(m_wildMessage,' ')  ) *::strchr(m_wildMessage,' ') = 0;
    if ( ::strchr(m_msgSeverity,' ')  ) *::strchr(m_msgSeverity,' ') = 0;
    if ( ::strchr(m_histSeverity,' ') ) *::strchr(m_histSeverity,' ') = 0;
    switch(ev.command_id) {
    case CMD_FILE:
      ioc.send(this,CMD_FILE,this);
      return;
    case CMD_CLOSE:
      ioc.send(m_parent,CMD_DELETE,this);
      return;
    case CMD_SUMM_HISTORY:
      ioc.send(m_history,CMD_SUMM_HISTORY,CMD_SUMM_HISTORY);
      return;
    case CMD_WILD_NODE:
      showHistory(m_wildNode,"*");
      return;
    case CMD_WILD_MESSAGE:
      showHistory(m_wildNode,m_wildMessage);
      return;
    case CMD_EDIT:
      new FilterDisplay(this,m_msg,m_history);
      return;
    case CMD_SEVERITY:
      switch(ev.param_id) {
      case 1:
        ioc.send(m_msg,ev.command_id,new string(m_msgSeverity));
        return;
      case 2:
        ioc.send(m_history,ev.command_id,new string(m_histSeverity));
        return;
      default:
        break;
      }
      break;
    default:
      if ( ev.command_id > 0 && ev.command_id <= (int)m_items.size() ) {
        int val, cmd = ev.command_id;
        switch(ev.param_id) {
        case 1:
          showCluster(cmd);
          break;
        case 2:
          upic_write_message2("Replace cmd %d",cmd);
          val = m_items[cmd].first = ::strcmp(s_enableDisableResult,s_enable[0])==0;
          ::upic_replace_param_line(m_id,cmd,setupParams(m_items[cmd].second,val==0?false:true).c_str(),"");
          ::upic_set_cursor(ev.menu_id,cmd,ev.param_id);
          ioc.send(m_msg,val ? CMD_DISCONNECT_CLUSTER : CMD_CONNECT_CLUSTER,new string(m_items[cmd].second));
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
