// $Id: ActiveMQSensor.cpp,v 1.1 2010/11/01 17:20:22 frankb Exp $
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
// $Header: /local/reps/lhcb/Online/ActiveMQ/src/ActiveMQSensor.cpp,v 1.1 2010/11/01 17:20:22 frankb Exp $

#include "ActiveMQ/ActiveMQSensor.h"
#include "WT/wt_facilities.h"

using namespace std;
using namespace ActiveMQ;

/// Initializing constructor
ActiveMQSensor::ActiveMQSensor(const string& nam, const string& host, int port)
: Sensor(WT_FACILITY_RO1,nam.c_str(),true), 
  m_name(nam), m_host(host), m_port(port), m_thread(0), m_stop(false)
{
}

/// Standad destructor
ActiveMQSensor::~ActiveMQSensor()  {
}

/// Sensor overload: Add interactor to sensor. Dispatch criteria is topic name
void ActiveMQSensor::add(Interactor* client, void* topic)  {
  const char* channel = (const char*)topic;
  if ( client && channel && isascii(channel[0]) )  {
    InteractorTable::iterator i = m_clients.find(channel);
    if ( i == m_clients.end() )  {
      lib_rtl_output(LIB_RTL_INFO,"+++  Subscribed to channel: %s.",channel ==0 ? "???" : channel);
      subscribe(channel);
    }
    m_clients[channel].push_back(client);
  }
}

/// Sensor overload: Remove interactor from sensor. Dispatch criteria is topic name
void ActiveMQSensor::remove(Interactor* client, void* topic)  {
  if ( client )  {
    const char* channel = (const char*) topic;
    if ( channel && isascii(channel[0]) )  {  // Remove client from one topic
      InteractorTable::iterator i = m_clients.find(channel);
      if ( i != m_clients.end() )  {
        Clients& cl = (*i).second;
        for(size_t j=0, n=cl.size(); j<n; ++j)  {
          if ( cl[j] == client )  {
            cl.erase(cl.begin()+j);
            --j;--n;
          }
        }
        if ( cl.size() == 0 )  {
          m_clients.erase(i);
          unsubscribe(channel);
        }
      }
    }
    else if ( !topic )  {  // Remove client from ALL topics
      // First remove clients 
      for(InteractorTable::iterator i=m_clients.begin(); i!=m_clients.end(); ++i)  {
        Clients& cl = (*i).second;
        for(size_t j=0, n=cl.size(); j<n; ++j)  {
          if ( cl[j] == client )  {
            cl.erase(cl.begin()+j);
            --j;--n;
          }
        }
      }
      // Do cleanup....
      for(InteractorTable::iterator i=m_clients.begin(); i!=m_clients.end(); ++i)  {
        Clients& cl = (*i).second;
        if ( cl.size() == 0 )  {
          unsubscribe((*i).first);
          m_clients.erase(i);
          i=m_clients.begin();
	  if ( i==m_clients.end() ) break;
        }
      }
    }
  }
}

/// Poll thread start routine
int ActiveMQSensor::_exec(void* arg)   {
  ActiveMQSensor* s = (ActiveMQSensor*)arg;
  return s->execute();
}

/// Start polling for messages....
int ActiveMQSensor::start()  {
  int rc = connectServer();
  if ( 1 == rc )  {
    rc = connect();
    if ( 1 == rc )  {
      return ::lib_rtl_start_thread(_exec,this,&m_thread);
    }
  }
  return 0;
}
