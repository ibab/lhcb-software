// $Id: ClusterDisplay.cpp,v 1.10 2009-11-13 18:56:53 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/ClusterDisplay.cpp,v 1.10 2009-11-13 18:56:53 frankb Exp $

#include "ROLogger/ClusterDisplay.h"
#include "UPI/UpiSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "UPI/upidef.h"
#include "RTL/rtl.h"
#include "ROLoggerDefs.h"
#include <sstream>
#include <cstring>

using namespace ROLogger;
using namespace std;

static int s_NumList[] = {1,10,50,100,200,300,500,1000,5000,10000};
static const char* s_MsgList[] = {"*",
				  "*MEPRx*",
				  "*GauchoJob_0*",
				  "*GauchoJob_1*",
				  "*GauchoJob_2*",
				  "*GauchoJob_3*",
				  "*GauchoJob_4*",
				  "*GauchoJob_5*",
				  "*GauchoJob_6*",
				  "*GauchoJob_7*",
				  "*EvtProd*",
				  "*EvtHolder*"
};

/// Standard constructor with object setup through parameters
ClusterDisplay::ClusterDisplay(Interactor* parent, Interactor* logger, const string& name, const Nodes& nodes) 
  : m_name(name), m_nodes(nodes), m_parent(parent), m_logger(logger), m_numMsg(200)
{
  int cnt = 0;
  if ( 0 == m_parent ) m_parent = this;
  m_id = UpiSensor::instance().newID();
  ::snprintf(m_wildNode,sizeof(m_wildNode),"%s*",m_name.c_str());
  ::strncpy(m_wildMessage,"*",sizeof(m_wildNode));
  m_wildNode[sizeof(m_wildNode)-1] = 0;
  ::upic_open_detached_menu(m_id,0,0,"Error logger",m_name.c_str(),RTL::nodeName().c_str());
  ::upic_declare_callback(m_id,CALL_ON_BACK_SPACE,(Routine)backSpaceCallBack,this);
  m_nodeList = new const char*[m_nodes.size()+1];
  m_nodeList[0] = "*";
  if ( !m_nodes.empty() ) {
    ::upic_add_comment(CMD_COM1,("Known nodes for cluster "+m_name).c_str(),"");
    for(Nodes::const_iterator n=m_nodes.begin(); n!=m_nodes.end(); ++n, ++cnt) {
      if ( (*n).find(name) != string::npos ) {
	m_nodeList[cnt+1] = (char*)(*n).c_str();
        ::upic_add_command(cnt+1, (*n).c_str(),"");
      }
    }
    ::upic_add_comment(CMD_COM2,        "-----------------------------------------------","");
  }
  ::upic_set_param(&m_numMsg,1,"I6",m_numMsg,0,0,s_NumList,sizeof(s_NumList)/sizeof(s_NumList[0]),0);
  ::upic_add_command(CMD_COM3,        "Show                  ^^^^^^ Messages","");
  if ( !m_nodes.empty() ) {
    ::upic_add_command(CMD_FARM_HISTORY,"Show all histories","");
  }
  ::upic_set_param(m_wildNode,1,"A16",m_wildNode,0,0,m_nodeList,m_nodes.size()+1,0);
  ::upic_add_command(CMD_WILD_NODE,   "Node match:           ^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_wildMessage,1,"A16",m_wildMessage,0,0,s_MsgList,sizeof(s_MsgList)/sizeof(s_MsgList[0]),0);
  ::upic_add_command(CMD_WILD_MESSAGE,"...and match message: ^^^^^^^^^^^^^^^","");
  ::upic_add_comment(CMD_COM5,        "-----------------------------------------------","");
  ::upic_add_command(CMD_CLOSE,       "Close","");
  ::upic_close_menu();
  ::upic_set_cursor(m_id,CMD_CLOSE,0);
  UpiSensor::instance().add(this,m_id);
}

/// Standard destructor
ClusterDisplay::~ClusterDisplay() {
  UpiSensor::instance().remove(this,m_id);
  ::upic_delete_menu(m_id);
  if ( m_nodeList ) delete [] m_nodeList;
}

/// Show history according to node and message pattern match
void ClusterDisplay::showHistory(const char* node_match, const char* msg_match) {
  stringstream s;
  s << "#Node:{" << node_match << "}#Msg:{" << msg_match << "}#Num:{" << m_numMsg << "}" << ends;
  IocSensor::instance().send(m_logger,CMD_NODE_HISTORY,new string(s.str()));
}

/// Display callback handler
void ClusterDisplay::handle(const Event& ev) {
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {
    case CMD_DELETE:
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
    m_wildNode[sizeof(m_wildNode)-1] = 0;
    m_wildMessage[sizeof(m_wildMessage)-1] = 0;
    if ( ::strchr(m_wildNode,' ') ) *::strchr(m_wildNode,' ') = 0;
    if ( ::strchr(m_wildMessage,' ') ) *::strchr(m_wildMessage,' ') = 0;
    if ( ev.menu_id == m_id ) {
      IocSensor& ioc = IocSensor::instance();
      if ( ev.command_id == CMD_CLOSE || ev.command_id == CMD_BACKSPACE )
              ioc.send(m_parent,CMD_DELETE,this);
      else if ( ev.command_id == CMD_FARM_HISTORY )
              for(Nodes::const_iterator i=m_nodes.begin();i!=m_nodes.end();++i)
                showHistory((*i).c_str(),"*");
      else if ( ev.command_id == CMD_WILD_NODE )
              showHistory(m_wildNode,"*");
      else if ( ev.command_id == CMD_WILD_MESSAGE )
              showHistory(m_wildNode,m_wildMessage);
      else if ( ev.command_id > 0 && ev.command_id <= (int)m_nodes.size() )
              showHistory(m_nodes[ev.command_id-1].c_str(),"*");
    }
    break;
  default:
    break;
  }
}

