// $Id: SubscribeHandler.cpp,v 1.1 2010/11/01 17:20:22 frankb Exp $
//====================================================================
//  Comet
//--------------------------------------------------------------------
//
//  Package    : ActiveMQ
//
//  Description: DIM enabled ActiveMQ
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /local/reps/lhcb/Online/ActiveMQ/src/SubscribeHandler.cpp,v 1.1 2010/11/01 17:20:22 frankb Exp $
#include "ActiveMQ/SubscribeHandler.h"
#include "ActiveMQ/Connector.h"
#include "ActiveMQ/Commands.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <memory>
using namespace std;
using namespace ActiveMQ;
#ifdef WIN32
#include <cstring>
inline int strcasecmp(const char *s1, const char *s2) {
  return ::_stricmp(s1, s2);
}
inline int strncasecmp(const char *s1, const char *s2, size_t n) {
  return ::_strnicmp(s1, s2, n);
}
#endif

/// Default constructor
SubscribeHandler::SubscribeHandler(const string& ini, Interactor* sender) : m_sender(sender) {
  loadServices(ini);
}

/// Default destructor
SubscribeHandler::~SubscribeHandler() {
  for(Connections::iterator i=m_con.begin(); i!=m_con.end(); ++i)
    delete (*i).second;
  m_con.clear();
}

/// Load services from file
void SubscribeHandler::loadServices(const string& ini)  {
  if ( !ini.empty() )  {
    ifstream f(ini.c_str(),ios_base::in);
    while (1)  {
      char text[1024];
      string s;
      text[0] = 0;
      f.getline(text,sizeof(text),'\n');
      if ( text[0] != 0 )  {
        m_con[text] = new Connector(text,false,m_sender);
        continue;
      }
      break;
    }
  }
}

/// General interrupt handler
void SubscribeHandler::handle(const Event& ev)  {
  switch(ev.eventtype)  {
    case NetEvent:
      handleNet(ev);
      break;
    case IocEvent:
      handleIoc(ev);
      break;
    default:
      break;
  }
}

/// Network event handler
void SubscribeHandler::handleNet(const Event& ev)  {
  const char* body = ev.buffer_ptr;
  if ( body )  {
    if ( ::strncasecmp(body,"SUBSCRIBE:",10) == 0 )  {
      IocSensor::instance().send(this,CMD_SUBSCRIBE,new string(body+10));
    }
    else if ( ::strncasecmp(body,"UNSUBSCRIBE:",12) == 0 )  {
      IocSensor::instance().send(this,CMD_UNSUBSCRIBE,new string(body+12));
    }
    else if ( ::strncasecmp(body,"PUBLISH:",8) == 0 )  {
      IocSensor::instance().send(this,CMD_PUBLISH,new string(body+8));
    }
  }
}

/// Ioc event handler
void SubscribeHandler::handleIoc(const Event& ev)  {
  auto_ptr<string> svc;
  Connections::iterator i;
  switch(ev.type)  {
    case CMD_SUBSCRIBE:
      svc = auto_ptr<string>(ev.iocPtr<string>());
      if ( (i=m_con.find(*(svc.get()))) == m_con.end() ) {
	if ( svc->find("lbWeb.lbWeb.") != string::npos ) 
	  ::lib_rtl_output(LIB_RTL_FATAL,"HDLR> IGNORE: Subscribe:%s",svc->c_str());
	else if ( svc->find("lbWeb.topic.") != string::npos ) 
	  ::lib_rtl_output(LIB_RTL_FATAL,"HDLR> IGNORE: Subscribe:%s",svc->c_str());
	else if ( svc->find("lbWeb.") != 0 ) 
	  ::lib_rtl_output(LIB_RTL_FATAL,"HDLR> IGNORE: Subscribe:%s",svc->c_str());
	//else if ( svc->find("lbWeb.LHCCOM/LHC.LHC.Machine.CollimatorPositions.") != string::npos ) 
	//  ::lib_rtl_output(LIB_RTL_FATAL,"HDLR> IGNORE: Subscribe:%s",svc->c_str());
	//else if ( svc->find("lbWeb.BCM_DP_") == 0 ) 
	//  ::lib_rtl_output(LIB_RTL_FATAL,"HDLR> IGNORE: Subscribe:%s",svc->c_str());
	else {
	  ::lib_rtl_output(LIB_RTL_VERBOSE,"HDLR> Subscribe:%s",svc->c_str());
	  m_con[*(svc.get())] = new Connector(*(svc.get()),true,m_sender);
	}
      }
      else {
	::lib_rtl_output(LIB_RTL_VERBOSE,"HDLR> Publish topic:%s",svc->c_str());
        (*i).second->publish();
      }
      break;
    case CMD_UNSUBSCRIBE:
      svc = auto_ptr<string>(ev.iocPtr<string>());
      if ( (i=m_con.find(*svc)) != m_con.end() )  {
        if ( (*i).second->mayRemove() )  {
          if ( (*i).second )  delete (*i).second;
          m_con.erase(i);
        }
      }
      break;
    case CMD_PUBLISH:
      svc = auto_ptr<string>(ev.iocPtr<string>());
      if ( (i=m_con.find(*svc)) != m_con.end() )
        (*i).second->publish();
      break;
    default:
      break;
  }
}
