// $Id: StompSensor.cpp,v 1.1 2009-07-03 18:10:08 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Stomp/src/StompSensor.cpp,v 1.1 2009-07-03 18:10:08 frankb Exp $

#include "Stomp/StompSensor.h"
#include "Stomp/stomp.h"
#include "WT/wt_facilities.h"
#include "apr_poll.h"
#include "CPP/Event.h"
#include "WT/wtdef.h"
#include "CPP/Interactor.h"
#include <cstring>

using namespace std;
using namespace Stomp;

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

static int report(int exitCode, const char *message, apr_status_t reason) {
  char msgbuf[80];
  ::apr_strerror(reason, msgbuf, sizeof(msgbuf));
  ::fprintf(stderr, "%s: %s (%d)\n", message, msgbuf, reason);
  ::exit(exitCode);
	return reason;
}

/// Initializing constructor
StompSensor::StompSensor(const string& nam, const string& host, int port)
: Sensor(WT_FACILITY_RO1,nam.c_str(),true), 
  m_name(nam), m_host(host), m_port(port), m_stop(false),
  m_pool(0), m_con(0)
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
  return 1;
}

/// Disconnect from stomp server
int StompSensor::disconnectServer()   {
  apr_status_t rc = ::stomp_disconnect(&m_con); 
  if ( rc != APR_SUCCESS )  {
    report(-2, "StompSensor> Could not disconnect", rc);
    return 0;
  }
  return 1;
}

/// Send command to stomp server
int StompSensor::_command(const string& cmd, const string& channel, const void* body, int len)   {
  stomp_frame frame;
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
  }
  frame.body_length = len;
  frame.body = (char*)body;
  rc = ::stomp_write(m_con, &frame, spool);
  if ( rc != APR_SUCCESS )  {
    report(-2, "StompSensor> Could not send frame", rc);
    ::apr_pool_destroy(spool);	   
  }
  ::apr_pool_destroy(spool);	   
  return 1;
}

/// Connect to stomp service
int StompSensor::connect()   {
  stomp_frame frame, *pframe;
  frame.command = "CONNECT";
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
  //::fprintf(stdout, "StompSensor> Response: %s, %s\n", pframe->command, pframe->body);
  return 1;
}

/// Disconnect to stomp service
int StompSensor::disconnect()   {
  return _command("DISCONNECT","",0,-1);
}  

/// Subscribe to data channel
int StompSensor::subscribe(const string& channel)   {
  InteractorTable::iterator i = m_clients.find(channel);
  if ( i == m_clients.end() )  {
    return _command("SUBSCRIBE",channel,0,-1);
  }
  // We were already subscribed....
  return 1;
}

/// Unsubscribe to data channel
int StompSensor::unsubscribe(const string& channel)   {
  int rc = _command("UNSUBSCRIBE",channel,0,-1);
  InteractorTable::iterator i = m_clients.find(channel);
  if ( i != m_clients.end() )  {
    m_clients.erase(i);
  }
  return rc;
}

/// Send data to the stomp service (push)
int StompSensor::send(const string& destination, const void* data, size_t len)  {
  return _command("SEND",destination,data,int(len));
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
    return 0;
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

/// Sensor overload: Add interactor to sensor. Dispatch criteria is topic name
void StompSensor::add( Interactor* client, void* topic)  {
  const char* channel = (const char*)topic;
  if ( client && channel && isascii(channel[0]) )  {
    InteractorTable::iterator i = m_clients.find(channel);
    if ( i == m_clients.end() )  {
      subscribe(channel);
    }
    m_clients[channel].push_back(client);
  }
}

/// Sensor overload: Remove interactor from sensor. Dispatch criteria is topic name
void StompSensor::remove(Interactor* client, void* topic)  {
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
          m_clients.erase(i);
          unsubscribe((*i).first);
          i=m_clients.begin();
        }
      }
    }
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

/// Poll thread start routine
int StompSensor::_exec(void* arg)   {
  StompSensor* s = (StompSensor*)arg;
  return s->execute();
}

/// Start polling for messages....
int StompSensor::start()  {
  int rc = connectServer();
  if ( 1 == rc )  {
    rc = connect();
    if ( 1 == rc )  {
      return ::lib_rtl_start_thread(_exec,this,&m_thread);
    }
  }
  return 0;
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
        report(-2, "StompSensor> Failed to poll for messages. Bad channel.", rc);
        m_stop = true;

        break;
      case APR_SUCCESS:  {
        if ( recvd > 0 )  {
          if ( !m_stop )   {
            auto_ptr<Message> msg(new Message(this));
            int res = msg->read();
            if ( res == 1 )  {
              ::wtc_insert(m_facility, msg.release());
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
