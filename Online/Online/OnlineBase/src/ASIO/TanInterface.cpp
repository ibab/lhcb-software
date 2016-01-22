// $Id: $
//==========================================================================
//  LHCb Online software suite
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see OnlineSys/LICENSE.
//
// Author     : M.Frank
//
//==========================================================================
#define _TanInterface_C_

#include "RTL/rtl.h"
#include "NET/defs.h"
#include "ASIO/Socket.h"
#include "ASIO/TanInterface.h"

#include "AMS/amsdef.h"
#define SLEEP(x)  lib_rtl_sleep(1000*x)

#include <map>
#include <mutex>
#include <string>
#include <stdexcept>

using namespace BoostAsio;

static std::mutex s_tan_mutex;
#define TAN_MAX_CONNECT_ATTEMPTS  5

// ----------------------------------------------------------------------------
// C Interface: Allocate port number from local server
// ----------------------------------------------------------------------------
extern "C" int tan_allocate_port_number (const char *name, TanMessage::Port *port)  {
  return BoostAsio::TanInterface::instance().allocatePort(name,false,port);
}
// ----------------------------------------------------------------------------
// C Interface: Allocate new port number from local server
// ----------------------------------------------------------------------------
extern "C" int tan_allocate_new_port_number (const char *name, TanMessage::Port *port)  {
  return BoostAsio::TanInterface::instance().allocatePort(name,true,port);
}
// ----------------------------------------------------------------------------
// C Interface: Deallocate port number on local nameserver
// ----------------------------------------------------------------------------
extern "C" int tan_deallocate_port_number (const char *name) {
  return BoostAsio::TanInterface::instance().deallocatePort(name);
}
// ----------------------------------------------------------------------------
// C Interface: Get remote address by name
// ----------------------------------------------------------------------------
extern "C" int tan_get_address_by_name ( const char* name, TanMessage::Address *sad)  {
  return BoostAsio::TanInterface::instance().addressByName(name,*sad);
}
// ----------------------------------------------------------------------------
// C Interface:
// ----------------------------------------------------------------------------
extern "C" int tan_declare_alias (const char* name)   {
  return BoostAsio::TanInterface::instance().declareAlias(name);
}
// ----------------------------------------------------------------------------
// C Interface:
// ----------------------------------------------------------------------------
extern "C" int tan_remove_alias (const char* name)   {
  return BoostAsio::TanInterface::instance().removeAlias(name);
}
// ----------------------------------------------------------------------------
// C Interface:
// ----------------------------------------------------------------------------
extern "C" int tan_host_name(char* node, size_t siz)   {
  return BoostAsio::TanInterface::instance().hostName(node,siz);
}
// ----------------------------------------------------------------------------
// C Interface:
// ----------------------------------------------------------------------------
extern "C" int tan_dump_dbase(const char* node)   {
  return BoostAsio::TanInterface::instance().dumpDB(node);
}
// ----------------------------------------------------------------------------
// C Interface:
// ----------------------------------------------------------------------------
extern "C" int tan_shutdown(const char* node)   {
  return BoostAsio::TanInterface::instance().shutdown(node);
}
// ----------------------------------------------------------------------------
// C++ Interface:
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Instantiator: create object
//                                      M.Frank
// ----------------------------------------------------------------------------
TanInterface& TanInterface::instance()   {
  static TanInterface s_instance;
  return s_instance;
}
// ----------------------------------------------------------------------------
// Constructor: initialise the service
// Note: The addresses are NOT in network byte ordering!
//                                      M.Frank
// ----------------------------------------------------------------------------
TanInterface::TanInterface() : m_conOut(0), m_conIn(0)   {
  char* dot;
  hostent* h = 0;
  const char* tan_host = ::getenv("TAN_NODE");
  ::lib_rtl_get_node_name(m_pcHostName, sizeof (m_pcHostName));
  m_portAllocated = 0;
  m_numPorts = 0;
  if ( tan_host   ) ::strncpy(m_pcHostName, tan_host, sizeof (m_pcHostName));
  h = hostByName(m_pcHostName);
  if ( h == 0 )   {
    throw std::runtime_error(std::string("Tan(ASIO):")+::strerror(errno));
  }
  dot  = strchr(m_pcHostName,'.');
  // Do not truncate host names in internet format like 192.168.xxx.yyy
  if ( dot && !isdigit(m_pcHostName[0]) ) *dot = 0;

  memcpy (&m_sintcp.sin_addr, h->h_addr, h->h_length);
  m_sintcp.sin_family = AF_INET;
  ::memset(m_sintcp.sin_zero,0,sizeof(m_sintcp.sin_zero));

#ifdef _SERVICE
  m_pServiceTCP       = ::getservbyname (NAME_SERVICE_NAME, "tcp");
  if ( m_pServiceUDP == 0 || m_pServiceTCP == 0 )
    goto Error;
  m_sintcp.sin_port   = htons(m_pServiceTCP->s_port);
#else
  m_sintcp.sin_port   = NAME_SERVICE_PORT;
#endif
}

/// Destructor
TanInterface::~TanInterface()  {
}

/// Convert TAN error codes to operating system specific ones
int TanInterface::errorCode(int tan_error)  {
  switch ( tan_error )    {
  case TAN_SS_TASKNOTFOUND:     return  AMS_TASKNOTFOUND;
  case TAN_SS_HOSTNOTFOUND:     return  AMS_HOSTNOTFOUND;
  case TAN_SS_DATABASEFULL:     return  AMS_DATABASEFULL;
  case TAN_SS_ODDRESPONSE:      return  AMS_ODDRESPONSE;
  case TAN_SS_UNKNOWNMODE:      return  AMS_UNKNOWNMODE;
  case TAN_SS_NOTOPEN:          return  AMS_TANCLOSED;
  case TAN_SS_RECV_TMO:         return  AMS_TIMEOUT;
  case TAN_SS_MADSRV:           return  AMS_TERRIBLE;
  case TAN_SS_DUPLNAM:          return  AMS_DUPLICATE_NAME;
  case TAN_SS_ENTNOTALLOC:      return  AMS_TERRIBLE;
  case TAN_SS_ODDREQUEST:       return  AMS_TERRIBLE;
  case TAN_SS_OPEN:             return  AMS_TANOPEN;
  case TAN_SS_ERROR:            return  AMS_ERROR;
  case TAN_SS_NOMEM:            return  AMS_NOMEMORY;
  case TAN_SS_SUCCESS:          return  AMS_SUCCESS;
  default:                      return  AMS_ERROR;
  }
}

/// Fatal nameserver connection error: close channel and return given error code
int TanInterface::fatalError(int code)  {
  if ( code != TAN_SS_SUCCESS )  {
    ::lib_rtl_output(LIB_RTL_OS,"Tan(ASIO): Closing Channel in error:%d errno=%d\n",code,errno);
  }
  //delete m_conOut;
  //m_conOut = 0;
  return code;
}

/// Get hostentry from inet db by host name
hostent* TanInterface::hostByName(const char* name)  {
  hostent *h = gethostbyname(name);
  return h;
}

/// Get local host name
int TanInterface::hostName(char* name, size_t size)  const  {
  ::strncpy(name, m_pcHostName, size);
  return TAN_SS_SUCCESS;
}

/// Given a node name, fill the socket address from the inet db.
int TanInterface::setInquireAddr(const char* node, NetworkChannel::Address& sin)  {
  struct hostent *rp = hostByName(node);
  static unsigned short port = NAME_SERVICE_PORT;
  if ( rp )    {
    memcpy (&sin.sin_addr, rp->h_addr, rp->h_length);
    sin.sin_family = AF_INET;
    memset(sin.sin_zero,0,sizeof(sin.sin_zero));
    sin.sin_port = port;
    return TAN_SS_SUCCESS;
  }
  return TAN_SS_HOSTNOTFOUND;
}

/// Fill local address
int TanInterface::setLocalAddress  ( NetworkChannel::Address& sin )       {
  ::memcpy(&sin,&m_sintcp,sizeof(m_sintcp));
  sin.sin_port = m_sintcp.sin_port;
  return TAN_SS_SUCCESS;
}

/// Convert strings of the type NODE::TASK  and TASK@NODE.DOMAIN into a reasonable task - and node name 
void TanInterface::nodeWithName(const char* name, char* node, char* proc)  {
  int n, s = 0;
  const char *p;
  if ( 0 != (p=::strstr(name,"::")) )    {      // DECNET STYLE
    s = 0;
    if (node != 0)  {
      ::strncpy (node, name, n = p - name);
      node[n] = 0;
    }
    if (proc!= 0)  {
      ::strcpy (proc, p + 2);
    }
  }
  else if ( 0 != (p=::strchr(name,'@')) )    {
    s = 1;
    if (node != 0)  {                         // INTERNET STYLE
      ::strcpy (node, p + 1);
    }
    if (proc!= 0)   {
      ::strncpy (proc, name, n = p-name);
      proc [n] = 0;
    }
  }
  else    {
    if (node != 0) ::strcpy (node, m_pcHostName);
    if (proc != 0) ::strcpy (proc, name);
  }
  for(char* q=node; q && *q; q++) *q = s==1 ? ::tolower(*q) : ::toupper(*q);
  for(char* q=proc; q && *q; q++) *q = s==1 ? ::tolower(*q) : ::toupper(*q);
}

// ----------------------------------------------------------------------------
//  retrieve network address of a task given his name
//  -- accesses the local/remote nameserver task.
//
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::addressByName(const char* name, NetworkChannel::Address& sad)  {
  char node[64];
  TanMessage msg(TanMessage::INQUIRE);
  nodeWithName(name,node,msg.m_name);
  setLocalAddress(msg.m_sin);
  if ( setInquireAddr(node,msg.m_sin) == TAN_SS_SUCCESS )  {
    int rc = communicate(msg, Receive_TMO);
    if ( rc == errorCode(TAN_SS_SUCCESS) )   {
      sad = msg.m_sin;
      sad.sin_port = msg.m_sin.sin_port;
    }
    return rc;
  }
  return errorCode(TAN_SS_HOSTNOTFOUND);
}

/// Allocate port number from local name server (force=false)
int TanInterface::allocatePort(const char* name, NetworkChannel::Port *port)  {
  return allocatePort(name,false,port);
}

/// Allocate port number from local name server
int TanInterface::allocatePort(const char* name, bool force, NetworkChannel::Port *port)  {
  std::lock_guard<std::mutex> lock(s_tan_mutex);
  TanMessage msg(TanMessage::ALLOCATE);
  setLocalAddress(msg.m_sin);
  nodeWithName(name, 0 , msg.m_name);
  bool is_open = 0 != m_portAllocated;
  if ( 0 == port )  {
    return errorCode(TAN_SS_ODDREQUEST);
  }
  *port = 0;
  if ( 0 == m_conOut )  {
    int on = 1, rc = -1;
    m_conOut = new Socket(Asio_REQ);
    for (int trials=0; trials <= TAN_MAX_CONNECT_ATTEMPTS; ++trials )  {
      rc = m_conOut->connect(msg);
      if ( 0 == rc ) break;
      // Sleep 1 second before re-trying ...
      ::lib_rtl_sleep(1000);
    }
    if ( rc == -1 )  {
      ::lib_rtl_signal_message(LIB_RTL_OS,"Tan(ASIO): Failed to connect as %s (allocatePort)", name);
      return fatalError(rc);
    }
    m_conOut->setsockopt(Asio_TCP_KEEPALIVE,&on,sizeof(on));
    m_conIn = m_conOut;
  }
  if ( force || !is_open )  {
    int rc = communicate(m_conOut, m_conIn, msg, Receive_TMO);
    if ( rc == errorCode(TAN_SS_SUCCESS) )   {
      if ( ++m_numPorts == 1 )  {
        m_portAllocated = msg.m_sin.sin_port;
      }
      *port = msg.m_sin.sin_port;
      return errorCode(TAN_SS_SUCCESS);
    }
    return rc;
  }
  *port = m_portAllocated;
  return errorCode(TAN_SS_OPEN);
}

/// Deallocate port number from local name server
int TanInterface::deallocatePort(const char* name)  {
  std::lock_guard<std::mutex> lock(s_tan_mutex);
  TanMessage msg(TanMessage::DEALLOCATE);
  nodeWithName(name, 0 ,msg.m_name);
  setLocalAddress(msg.m_sin);
  int rc = communicate(m_conOut, m_conIn, msg, Receive_TMO);
  if ( rc == errorCode(TAN_SS_SUCCESS) )  {
    if ( --m_numPorts == 0 )  {
      m_portAllocated = 0;
      m_conOut->close();
      delete m_conOut;
      m_conOut = 0;
    }
  }
  return rc;
}

/// Declare alias to nameserver
int TanInterface::declareAlias ( const char* name )  {
  TanMessage msg(TanMessage::ALIAS);
  nodeWithName(name, 0, msg.m_name);
  setLocalAddress(msg.m_sin);
  return communicate(m_conOut, m_conIn, msg, Receive_TMO);
}

/// Remove alias from nameserver
int TanInterface::removeAlias ( const char* name )  {
  TanMessage msg(TanMessage::DEALIAS);
  nodeWithName(name, 0, msg.m_name);
  setLocalAddress(msg.m_sin);
  return communicate(m_conOut, m_conIn, msg, Receive_TMO);
}

/// Send shutdown request to nameserver
int TanInterface::shutdown( const char* node )  {
  return sendAction(TanMessage::SHUTDOWN,node);
}

/// Dump remote database...
int TanInterface::dumpDB (const char* node)   {
  return sendAction(TanMessage::DUMP,node);
}

/// Send shutdown request to nameserver
int TanInterface::sendAction(int request, const char* node) {
  if ( node == 0 || *node == 0 ) node = m_pcHostName;
  TanMessage msg(request);
  if ( setInquireAddr(node,msg.m_sin) == TAN_SS_SUCCESS )
    return communicate(msg,Receive_TMO);
  return errorCode(TAN_SS_ERROR);
}

/// Communicate request with target name-server
int TanInterface::communicate(TanMessage& msg, int tmo)  {
  Socket chan(Asio_REQ);
  int rc = -1;
  for (int trials=0; trials <= TAN_MAX_CONNECT_ATTEMPTS; ++trials )  {
    rc = chan.connect(msg);
    if ( 0 == rc )   {
      return communicate(&chan,&chan,msg,tmo);
    }
    // Sleep 1 second before re-trying ...
    ::lib_rtl_sleep(1000);
  }
  return fatalError(chan.error());
}

/// Communicate request with any target name-server
int TanInterface::communicate(Socket* out, Socket* in, TanMessage& msg, int tmo)  {
  if ( out && in )  {
    errno = 0;
    if ( out->send(&msg,sizeof(msg)) == sizeof(msg) )  {
      int num_byte = in->recv ( &msg,sizeof(msg), tmo);
      if ( num_byte == sizeof(msg) )  {
        msg.Convert();
        if ( msg.error() != TAN_SS_SUCCESS )
          return fatalError( errorCode(msg.error()) );
        return errorCode(TAN_SS_SUCCESS);
      }
      else {
        if ( num_byte != int(msg._Length()) )
          return fatalError( errorCode(TAN_SS_ODDRESPONSE) );
        else if ( out->isCancelled())
          return fatalError(errorCode(TAN_SS_RECV_TMO));
        else
          return fatalError(out->error());
      }
    }
    return fatalError(in->error());
  }
  errno = ENOTSOCK;
  return errorCode(TAN_SS_NOTOPEN);
}
