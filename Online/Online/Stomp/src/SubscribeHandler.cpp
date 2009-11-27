// $Id: SubscribeHandler.cpp,v 1.2 2009-11-27 16:39:47 frankb Exp $
//====================================================================
//  Comet
//--------------------------------------------------------------------
//
//  Package    : Stomp
//
//  Description: DIM enabled Stomp
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Stomp/src/SubscribeHandler.cpp,v 1.2 2009-11-27 16:39:47 frankb Exp $
#include "Stomp/SubscribeHandler.h"
#include "Stomp/Connector.h"
#include "Stomp/Commands.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <memory>
using namespace std;
using namespace Stomp;

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
    if ( ::strncmp(body,"SUBSCRIBE:",10) == 0 )  {
      IocSensor::instance().send(this,CMD_SUBSCRIBE,new string(body+10));
    }
    else if ( ::strncmp(body,"UNSUBSCRIBE:",12) == 0 )  {
      IocSensor::instance().send(this,CMD_UNSUBSCRIBE,new string(body+12));
    }
    else if ( ::strncmp(body,"PUBLISH:",8) == 0 )  {
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
      if ( (i=m_con.find(*svc)) == m_con.end() )
        m_con[*svc] = new Connector(*svc,true,m_sender);
      else
        (*i).second->publish();
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
