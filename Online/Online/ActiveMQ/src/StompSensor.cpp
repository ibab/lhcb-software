// $Id: StompSensor.cpp,v 1.1 2010/11/01 17:20:22 frankb Exp $
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
// $Header: /local/reps/lhcb/Online/ActiveMQ/src/StompSensor.cpp,v 1.1 2010/11/01 17:20:22 frankb Exp $

//#include <cstdint>
#define INT64_C(c)      c ## L
#include "apr_poll.h"
#include "apr_time.h"
#include "stomp.h"
#include "ActiveMQ/StompSensor.h"
#include "CPP/Event.h"
#include "WT/wtdef.h"
#include "CPP/IocSensor.h"
#include <cstring>
#include <iostream>
#include <memory>

using namespace std;
using namespace ActiveMQ;

#define CMD_RESTART 12345

namespace ActiveMQ {
  template <class T> 
  ActiveMQSensor* createSensor(const std::string& name, const std::string& host, int port) {
    return new T(name,host,port);
  }
  template ActiveMQSensor* createSensor<StompSensor>(const std::string& name, const std::string& host, int port);
}

StompSensor::Message::~Message()  {
  if ( pool )  {
    sensor->destroyPool(pool);
    pool = 0;
  }
}
int StompSensor::Message::read()   {
  return sensor->read(frame, pool);
}

const char* StompSensor::Message::channel() const  {
  if ( frame ) {
    stomp_frame* f = (stomp_frame*)frame;
    if ( f->command && f->command[0] == 'M' )  {
      const char* chan = (const char*)::apr_hash_get(f->headers,"destination",APR_HASH_KEY_STRING);
      if ( chan )  {
        return chan;
      }
    }
  }
  return "";
}

/// Report message to output device
int StompSensor::report(int exitCode, const char *message, int reason) {
  char msgbuf[80];
  ::apr_strerror(reason, msgbuf, sizeof(msgbuf));
  ::lib_rtl_output(LIB_RTL_FATAL,"StompSensor> %s: %s (%d)\n", message, msgbuf, reason);
  if ( !m_stop ) ::exit(exitCode);
  return reason;
}

/// Initializing constructor
StompSensor::StompSensor(const string& nam, const string& host, int port)
  : ActiveMQSensor(nam,host,port), m_pool(0), m_con(0)
{
  apr_status_t rc = ::apr_initialize();
  if ( rc != APR_SUCCESS )  {
    report(-2, "StompSensor> Could not initialize", rc);
  }
  rc = ::apr_pool_create(&m_pool, NULL);
  if ( rc != APR_SUCCESS )  {
    report(-2, "StompSensor> Could not allocate pool", rc);
  }
  rc = ::stomp_connect( &m_con, m_host.c_str(), m_port, m_pool);
  if ( rc != APR_SUCCESS )  {
    report(-2, "StompSensor> Could not connect", rc);
  }
  rc = ::apr_socket_timeout_set(m_con->socket,apr_time_make(10,0));
  if ( rc != APR_SUCCESS )  {
    report(-2, "StompSensor> Could not set timeout", rc);
  }
}

/// Standad destructor
StompSensor::~StompSensor()  {
  if ( m_pool ) ::apr_pool_destroy(m_pool);	   
  m_pool = 0;
  ::apr_terminate();
}

/// Connect to stomp server
int StompSensor::connectServer()   {
  apr_status_t rc = ::stomp_connect( &m_con, m_host.c_str(), m_port, m_pool);
  if ( rc != APR_SUCCESS )  {
    report(-2, "StompSensor> Could not connect", rc);
    return 0;
  }
  rc = apr_socket_timeout_set(m_con->socket,apr_time_make(10,0));
  if ( rc != APR_SUCCESS )  {
    report(-2, "StompSensor> Could not set timeout", rc);
  }
  return 1;
}

/// Disconnect from stomp server
int StompSensor::disconnectServer()   {
  apr_status_t rc = ::stomp_disconnect(&m_con); 
  if ( rc != APR_SUCCESS )  {
    m_con = 0;
    report(-2, "StompSensor> Could not disconnect", rc);
    return 0;
  }
  m_con = 0;
  return 1;
}

/// Send command to stomp server
int StompSensor::_command(const string& cmd, const string& channel, const char* body, size_t /* len */)   {
  stomp_frame frame;
  const char* body_ptr = body ? body : "";
  apr_pool_t *spool = 0;
  apr_status_t rc = ::apr_pool_create(&spool, m_pool);
  if ( rc != APR_SUCCESS )  {
    report(-2, "StompSensor> Could not allocate pool", rc);
    return 0;
  }
  frame.command = (char*)cmd.c_str();
  if ( channel.empty() )   {
    frame.headers = 0;
  }
  else  {
    frame.headers = ::apr_hash_make(spool);
    ::apr_hash_set(frame.headers,"destination",APR_HASH_KEY_STRING,channel.c_str());
    ::lib_rtl_output(LIB_RTL_DEBUG,"%s : [%s]",cmd.c_str(),channel.c_str());
  }
  frame.body_length = strlen(body_ptr)+1;
  frame.body = (char*)body_ptr;
  rc = ::stomp_write(m_con, &frame, spool);
  if ( rc != APR_SUCCESS )  {
    report(-2, "StompSensor> Could not send frame", rc);
    ::apr_pool_destroy(spool);
    return 0;
  }
  ::apr_pool_destroy(spool);	   
  return 1;
}

/// Connect to stomp service
int StompSensor::connect()   {
  stomp_frame frame, *pframe;
  frame.command = (char*)"CONNECT";
  frame.headers = apr_hash_make(m_pool);
  ::apr_hash_set(frame.headers, "login", APR_HASH_KEY_STRING, "hchirino");          
  ::apr_hash_set(frame.headers, "passcode", APR_HASH_KEY_STRING, "letmein");          
  frame.body = NULL;
  frame.body_length = -1;
  apr_status_t rc = ::stomp_write(m_con, &frame, m_pool);
  if ( rc != APR_SUCCESS )  {
    report(-2, "StompSensor> Could not send frame", rc);
    return 0;
  }
  rc = ::stomp_read(m_con, &pframe, m_pool);
  if ( rc != APR_SUCCESS )  {
    report(-2, "StompSensor> Could not read frame", rc);
    return 0;
  }
  ::lib_rtl_output(LIB_RTL_VERBOSE,"StompSensor> Response: %s, %s\n", pframe->command, pframe->body);
  return 1;
}

/// Disconnect to stomp service
int StompSensor::disconnect()   {
  return _command("DISCONNECT","",0,0);
}  

/// Subscribe to data channel
int StompSensor::subscribe(const string& channel)   {
  InteractorTable::iterator i = m_clients.find(channel);
  if ( i == m_clients.end() )  {
    return _command("SUBSCRIBE",channel,0,0);
  }
  // We were already subscribed....
  return 1;
}

/// Unsubscribe to data channel
int StompSensor::unsubscribe(const string& channel)   {
  int rc = _command("UNSUBSCRIBE",channel,0,0);
  InteractorTable::iterator i = m_clients.find(channel);
  if ( i != m_clients.end() )  {
    m_clients.erase(i);
  }
  return rc;
}

/// Send data to the stomp service (push)
int StompSensor::send(const string& destination, const string& data)  {
  ::lib_rtl_output(LIB_RTL_VERBOSE,"Stomp> Send message: %s -> %s",
		   destination.c_str(),data.c_str());
  if ( destination.substr(0,7)=="/topic/" )
    return  _command("SEND",destination,data.c_str(),data.length());
  return _command("SEND","/topic/"+destination,data.c_str(),data.length());
}

/// Read pending data from channel
int StompSensor::read(Frame*& data_frame, apr_pool_t*& pool)  {
  stomp_frame* frame = 0;
  apr_status_t rc = ::apr_pool_create(&pool, m_pool);
  if ( rc != APR_SUCCESS )  {
    report(-2, "StompSensor> Could not allocate pool", rc);
    return 0;
  }
  rc = ::stomp_read(m_con, &frame, pool);
  if ( rc == APR_EOF )  {
    return 2;
  }
  else if ( rc != APR_SUCCESS )  {
    report(-2, "StompSensor> Could not read frame", rc);
    return 0;
  }
  frame->body[frame->body_length] = 0;
  data_frame = (Frame*)frame;
  return 1;
}

/// Destroy pool e.g. after reading messages
void StompSensor::destroyPool(apr_pool_t*& pool)  {
  if ( pool )   {
    ::apr_pool_destroy(pool);	   
    pool = 0;
  }
}

/// Sensor overload: Dispatch Stomp message to clients
void StompSensor::dispatch( void* arg )  {
  auto_ptr<Message> msg((Message*)arg);
  const char* channel = msg->channel();
  if ( channel )  {
    stomp_frame* f = (stomp_frame*)msg->frame;
    InteractorTable::iterator i = m_clients.find(channel);
    if ( i != m_clients.end() )  {
      Clients& cl = (*i).second;
      for(Clients::iterator j=cl.begin(); j!=cl.end(); ++j)  {
        Event ev(*j,NetEvent);
        ev.message    = (::Message*)msg.get();
        ev.device     = (char*)channel;
        ev.data       = f->command;
        ev.buffer_ptr = f->body;
        ev.buffer_len = f->body_length;
        ev.target->handle(ev);       
      }        
    }
  }
}

/// interactor overload: Handle external interrupts
void StompSensor::handle(const Event& ev) {
  if ( ev.eventtype == IocEvent )  {
    switch(ev.type)  {
    case CMD_RESTART:
      m_stop = false;
      ::lib_rtl_output(LIB_RTL_ALWAYS,"StompSensor> Serious error detector. Invoke AUTO-restart.");
      ::lib_rtl_output(LIB_RTL_ALWAYS,"StompSensor> Wait for worker thread to finish.");
      ::lib_rtl_join_thread(m_thread);
      m_thread = 0;
      ::lib_rtl_output(LIB_RTL_ALWAYS,"StompSensor> Worker thread dead. Disconnect now.");
      disconnect();
      ::lib_rtl_output(LIB_RTL_ALWAYS,"StompSensor> Disconnect from server.");
      disconnectServer();
      ::lib_rtl_output(LIB_RTL_ALWAYS,"StompSensor> Starting server connection.");
      m_stop = false;
      //install_printer(1);
      start();
      ::lib_rtl_output(LIB_RTL_ALWAYS,"StompSensor> Restarted successfully. See what happens");
      break;
    default:
      break;
    }
  }
}

/// Poll for messages....
int StompSensor::execute()  {
  int recvd;
  apr_pool_t *p = 0;
  apr_pollset_t* pollset;
  const apr_pollfd_t* activefds;
  apr_status_t rc = ::apr_pool_create(&p, m_pool);
  if ( rc != APR_SUCCESS )  {
    report(-2, "StompSensor> Could not allocate pool", rc);
    return 0;
  }
  apr_pollfd_t* fds = (apr_pollfd_t*)apr_pcalloc(p,1*sizeof(apr_pollfd_t));
  apr_status_t rv = apr_pollset_create(&pollset, 1, p, 0);
  if (rv != APR_SUCCESS) {
    return rv;
  }
  fds[0].desc_type = APR_POLL_SOCKET;
  fds[0].reqevents = APR_POLLIN;
  fds[0].p = m_pool;
  fds[0].desc.s = m_con->socket;
  fds[0].client_data = (void *)0;
  apr_pollset_add (pollset, &fds[0]);
  while(!m_stop)  {
    rv = ::apr_pollset_poll(pollset, 50000, &recvd, &activefds);
    switch(rv)  {
      case APR_TIMEUP:
        break;
      case APR_EBADF:
        m_stop = true;
	destroyPool(p);
        report(-2, "StompSensor> Failed to poll for messages. Bad channel.", rc);
	IocSensor::instance().send(this,CMD_RESTART,this);
	return 0;

        break;
      case APR_SUCCESS:  {
        if ( recvd > 0 )  {
          if ( !m_stop )   {
            auto_ptr<Message> msg(new Message(this));
            int res = msg->read();
            if ( res == 1 )  {
              ::wtc_insert(m_facility, msg.release());
            }
	    else if ( res == 2 ) {
	      m_stop = true;
	      report(-2, "StompSensor> EOF detected. Bad channel.", res);
	      IocSensor::instance().send(this,CMD_RESTART,this);
	      return 0;
	    }
          }
        }
        break;
      default:
        break;
      }
    }
  }
  m_stop = false;
  return 1;
}
