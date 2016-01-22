#define _TanInterface_C_

#include "RTL/rtl.h"
#include "NET/defs.h"
#include "TAN/TanInterface.h"
#include "NET/UdpNetworkChannel.h"
#include "NET/TcpNetworkChannel.h"

#include "AMS/amsdef.h"
#define SLEEP(x)  lib_rtl_sleep(1000*x)

#include <map>
#include <string>
// ----------------------------------------------------------------------------
// C Interface: Access to the tan interface pointer
// ----------------------------------------------------------------------------
extern "C" TanInterface* taninterface() {
  return &TanInterface::instance();
}
// ----------------------------------------------------------------------------
// C Interface: Allocate port number from local server
// ----------------------------------------------------------------------------
extern "C" int tan_allocate_port_number (const char *name, TanMessage::Port *port)  {
  return TanInterface::instance().allocatePort(name,port);
}
// ----------------------------------------------------------------------------
// C Interface: Deallocate port number on local nameserver
// ----------------------------------------------------------------------------
extern "C" int tan_deallocate_port_number (const char *name) {
  return TanInterface::instance().deallocatePort(name);
}
// ----------------------------------------------------------------------------
// C Interface: Get remote address by name
// ----------------------------------------------------------------------------
extern "C" int tan_get_address_by_name ( const char* name, TanMessage::Address *sad)  {
  return TanInterface::instance().addressByName(name,*sad);
}
// ----------------------------------------------------------------------------
// C Interface:
// ----------------------------------------------------------------------------
extern "C" int tan_declare_alias (const char* name)   {
  return TanInterface::instance().declareAlias(name);
}
// ----------------------------------------------------------------------------
// C Interface:
// ----------------------------------------------------------------------------
extern "C" int tan_remove_alias (const char* name)   {
  return TanInterface::instance().removeAlias(name);
}
// ----------------------------------------------------------------------------
// C Interface:
// ----------------------------------------------------------------------------
extern "C" int tan_host_name(char* node, size_t siz)   {
  return TanInterface::instance().hostName(node,siz);
}
// ----------------------------------------------------------------------------
// C Interface:
// ----------------------------------------------------------------------------
extern "C" int tan_dump_dbase(const char* node)   {
  return TanInterface::instance().dumpDB(node);
}
// ----------------------------------------------------------------------------
// C Interface:
// ----------------------------------------------------------------------------
extern "C" int tan_shutdown(const char* node)   {
  return TanInterface::instance().shutdown(node);
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
TanInterface::TanInterface() : m_channel(0) {
  char* dot;
  hostent* h = 0;
  const char* tan_host = ::getenv("TAN_NODE");
  m_portAllocated = 0;
  if ( tan_host   ) {
    ::strncpy(m_pcHostName, tan_host, sizeof(m_pcHostName)-1);
    m_pcHostName[sizeof(m_pcHostName)-1] = 0;
  }
  else   {
    int status = ::lib_rtl_get_node_name(m_pcHostName, sizeof(m_pcHostName));
    if ( status < 0 ) goto Error;
  }
  h = hostByName(m_pcHostName);
  if ( h == 0 ) goto Error;
  dot  = strchr(m_pcHostName,'.');
  // Do not truncate host names in internet format like 192.168.xxx.yyy
  if ( dot && !isdigit(m_pcHostName[0]) ) *dot = 0;

#ifdef _VMS
  memcpy (&m_sinudp.sin_addr, *h->h_addr_list, h->h_length);
#else
  memcpy (&m_sinudp.sin_addr, h->h_addr, h->h_length);
#endif
  m_sinudp.sin_family = AF_INET;
  ::memset(m_sinudp.sin_zero,0,sizeof(m_sinudp.sin_zero));
  ::memcpy(&m_sintcp,&m_sinudp,sizeof(m_sintcp));

  //::fprintf(stdout,"%s> TAN interface [%s]: Host=%s [%s]\n",
  //          RTL::nodeName().c_str(),RTL::processName().c_str(),
  //          m_pcHostName,inet_ntoa(m_sintcp.sin_addr));
#ifdef _SERVICE
  m_pServiceUDP       = ::getservbyname (NAME_SERVICE_NAME, "udp");
  m_pServiceTCP       = ::getservbyname (NAME_SERVICE_NAME, "tcp");
  if ( m_pServiceUDP == 0 || m_pServiceTCP == 0 )                    goto Error;
  m_sinudp.sin_port   = htons(m_pServiceUDP->s_port);
  m_sintcp.sin_port   = htons(m_pServiceTCP->s_port);
#else
  m_sintcp.sin_port   = m_sinudp.sin_port = NAME_SERVICE_PORT;
#endif
  m_status            = TAN_SS_SUCCESS;
  return;
 Error:
  m_status = TAN_SS_ERROR;
}
// ----------------------------------------------------------------------------
// Destructor
//                                      M.Frank
// ----------------------------------------------------------------------------
TanInterface::~TanInterface()  {
}
// ----------------------------------------------------------------------------
// Convert TAN error codes to operating system specific ones
//                                      M.Frank
// ----------------------------------------------------------------------------
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
// ----------------------------------------------------------------------------
// Fatal nameserver connection error: close channel and return given error code
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::fatalError(int code)  {
  if ( code != TAN_SS_SUCCESS )  {
    ::lib_rtl_output(LIB_RTL_OS,"Closing Channel in error:%d errno=%d\n",code,errno);
  }
  delete m_channel;
  m_channel = 0;
  return code;
}
// ----------------------------------------------------------------------------
//  get hostentry from inet db by host name
//                                      M.Frank
// ----------------------------------------------------------------------------
hostent* TanInterface::hostByName(const char* name)  {
  hostent *h = 0;
#ifdef _OSK
  h = _gethostbyname((char*)name);
#endif
  if ( h == 0 ) h = gethostbyname(name);
  return h;
}
// ----------------------------------------------------------------------------
//  get local host name
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::hostName(char* name, size_t size)  const  {
  ::strncpy(name, m_pcHostName, size);
  return TAN_SS_SUCCESS;
}
// ----------------------------------------------------------------------------
// given a node name, fill the socket address from the inet db.
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::setInquireAddr(const char* node, NetworkChannel::Address& sin, 
                                 NetworkChannel::Address& rin )  
{
  short port = NAME_SERVICE_PORT;
  struct hostent *rp = hostByName (node);
  if ( rp == 0 ) return TAN_SS_ERROR;
#ifdef _VMS
  memcpy (&sin.sin_addr, *rp->h_addr_list, rp->h_length);
#else
  memcpy (&sin.sin_addr,  rp->h_addr, rp->h_length);
#endif
  sin.sin_family = AF_INET;
  memset(sin.sin_zero,0,sizeof(sin.sin_zero));
  sin.sin_port = htons(NAME_SERVICE_PORT);
#define __USING_TCP_ALLOCATOR
#if defined(__USING_TCP_ALLOCATOR) // || defined(_WIN32)
  static struct servent* se = ::getservbyname(NAME_SERVICE_NAME,"tcp");
  rin          = sin;
#else
  static struct servent* se = ::getservbyname(NAME_SERVICE_NAME,"udp");
  rin          = m_sinudp;
#endif
  // To use only ONE nameserver and no predefined service 
  // for both allocation and inquire enable this:
  // export TAN_PORT=YES;
  //static const char* mode = ::getenv("TAN_PORT");
  //rin.sin_port = se==0 ? mode ? htons(port) : htons(port+1) : se->s_port;
  // Ignore above. We only use ONE nameserver!
  rin.sin_port = se==0 ? htons(port) : se->s_port;
  return TAN_SS_SUCCESS;
}
// ----------------------------------------------------------------------------
//  Fill local address
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::setLocalAddress  ( NetworkChannel::Address& sin )       {
  ::memcpy(&sin,&m_sinudp,sizeof(m_sintcp));
  sin.sin_port = htons(m_sintcp.sin_port);
  return TAN_SS_SUCCESS;
}
// ----------------------------------------------------------------------------
// convert strings of the type NODE::TASK  and TASK@NODE.DOMAIN into a reasonable
// task - and node name 
//                                      M.Frank
// ----------------------------------------------------------------------------
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
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::addressByName(const char* name, NetworkChannel::Address& sad)  {
  if ( Status() == TAN_SS_SUCCESS )  {
    char node[32];
    NetworkChannel::Address radd;
    TanMessage msg(TanMessage::INQUIRE);
    nodeWithName(name,node,msg._Name());
    setLocalAddress(msg.m_sin);
    if ( setInquireAddr(node,msg.m_sin,radd) == TAN_SS_SUCCESS )  {
#if defined(__USING_TCP_ALLOCATOR) // || defined(_WIN32)
      int retry = 4, nbyte = 0;
      TcpNetworkChannel c;
      TcpNetworkChannel &rcv = c, &snd = c;
      c.reuseAddress();
      while(--retry > 0 && nbyte == 0)  {
        if ( c.connect(radd,Connect_TMO) == 0 )  {
          nbyte = snd.send(&msg,sizeof(msg));
          break;
        }
        ::lib_rtl_sleep(1000);
      }
      if ( nbyte <= 0 )  {
        ::lib_rtl_signal_message(LIB_RTL_OS,"Failed to connect to TAN(addressByName)");
        return errorCode(TAN_SS_NOTOPEN);
      }
#else
      UdpNetworkChannel snd, rcv;
      NetworkChannel::Address sadd = msg.sin;
      if ( !rcv.isValid() )                    return rcv.error();
      if ( !snd.isValid() )                    return snd.error();
      msg.sin.sin_port = radd.sin_port;
      for(int retry=0; retry<10; retry++ )  {   // Necessary to avaoid
        if ( rcv.bind(radd) < 0 ) SLEEP(10);    // Clashes on the same
        else                       break;       // node...
        if ( retry == 5 )                      return rcv.error();
      }
      int nbyte = snd.send(&msg,sizeof(msg),0,0,&sadd);
#endif
      if ( nbyte == sizeof(msg) )  {
        // lib_rtl_output(LIB_RTL_DEBUG,"receive on port: %04X\n",radd.sin_port);
        nbyte = rcv.recv(&msg,sizeof(msg),Receive_TMO);
        if ( nbyte > 0 )  {
          msg.Convert();
          if ( nbyte != int(msg._Length()) )    return errorCode(TAN_SS_ODDRESPONSE);
          if ( msg.error() != TAN_SS_SUCCESS )  return errorCode(msg.error());
          sad = msg.m_sin;                      return errorCode(TAN_SS_SUCCESS);
        }                                       // Timeout fired!
        else if ( rcv.isCancelled()          )  return errorCode(TAN_SS_RECV_TMO);
        else                                    return rcv.error();
      }                                         return snd.error();
    }                                           return errorCode(TAN_SS_HOSTNOTFOUND);
  }                                             return errorCode(TAN_SS_ERROR);
}
// ----------------------------------------------------------------------------
// Allocate port number from local name server
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::allocatePort(const char* name, NetworkChannel::Port *port)  {
  if ( Status() == TAN_SS_SUCCESS )  {
    if ( m_channel == 0 )  {
      int retry = 4, rc = -1;
      m_channel = new TcpNetworkChannel;
      TanMessage msg(TanMessage::ALLOCATE);
      m_channel->reuseAddress();
      setLocalAddress(msg.m_sin);
      while(--retry > 0 && rc != 0)  {
        rc = m_channel->connect(msg.m_sin,Connect_TMO);
        if ( rc == 0 ) break;
        ::lib_rtl_sleep(1000);
      }
      if ( rc == -1 )  {
        ::lib_rtl_signal_message(LIB_RTL_OS,"Failed to connect to TAN(allocatePort)");
      }
      else if ( rc != -1 )  {
        nodeWithName(name, 0 , msg._Name());
        //::fprintf(stdout,"%s> TAN:  ALLOCATE: %s [%s] Host:%s Port:%d %d %s\n",
        //	  RTL::nodeName().c_str(),RTL::processName().c_str(), msg._Name(),m_pcHostName, 
        //	  msg.m_sin.sin_port, ntohs(msg.m_sin.sin_port), inet_ntoa(msg.m_sin.sin_addr));
        //::fflush(stdout);
        if ( m_channel->send(&msg,sizeof(msg)) == sizeof(msg) )  {
          int num_byte = m_channel->recv ( &msg,sizeof(msg),Receive_TMO);
          if ( num_byte > 0 )  {
            msg.Convert();
            if ( num_byte != int(msg._Length()))return fatalError( errorCode(TAN_SS_ODDRESPONSE) );
            if ( msg.error() != TAN_SS_SUCCESS) return fatalError( errorCode(msg.error()) );
            m_portAllocated = msg.m_sin.sin_port;
	    *port = msg.m_sin.sin_port;
            return errorCode( TAN_SS_SUCCESS );
          }                                     // receive timeout fired
          else if ( m_channel->isCancelled())   return fatalError(errorCode(TAN_SS_RECV_TMO));
          else                                  return fatalError(m_channel->error());
        }                                       return fatalError(m_channel->error());
      }                                         // Connect timeout fired
      else if ( m_channel->isCancelled() )      return fatalError(m_channel->error());
      else                                      return fatalError(m_channel->error());
    }                                           return errorCode(TAN_SS_OPEN);
  }                                             return errorCode(TAN_SS_ERROR);
}
// ----------------------------------------------------------------------------
// Deallocate port number from local name server
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::deallocatePort(const char* name)  {
  if ( Status() == TAN_SS_SUCCESS )  {
    if ( m_channel != 0 )  {
      TanMessage msg(TanMessage::DEALLOCATE);
      nodeWithName(name, 0 ,msg._Name() );
      setLocalAddress(msg.m_sin);
      int num_byte = m_channel->send(&msg,sizeof(msg));
      if ( num_byte != sizeof(msg) )            return fatalError(m_channel->error());
      num_byte = m_channel->recv (&msg,sizeof(msg),Receive_TMO);
      if      ( m_channel->isCancelled() )      return errorCode(TAN_SS_RECV_TMO);
      else if ( num_byte == sizeof(msg)  )      {
        int scc= fatalError(TAN_SS_SUCCESS);
        //::lib_rtl_output(LIB_RTL_DEBUG,"TAN connection closed. %p\n",(void*)m_channel);
        return scc;
      }
      else                                      return fatalError(errorCode(TAN_SS_MADSRV));
    }                                           return errorCode(TAN_SS_NOTOPEN);
  }                                             return errorCode(TAN_SS_ERROR);
}
// ----------------------------------------------------------------------------
//  Declare alias to nameserver
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::declareAlias ( const char* name )  {
  if ( Status() == TAN_SS_SUCCESS )  {
    if ( m_channel != 0 )  {
      TanMessage msg(TanMessage::ALIAS);
      nodeWithName(name, 0, msg._Name());
      setLocalAddress(msg.m_sin);
      int num_byte = m_channel->send (&msg,sizeof(msg));
      if ( num_byte != sizeof(msg) )             return fatalError(m_channel->error());
      num_byte = m_channel->recv (&msg,sizeof(msg),Receive_TMO);
      msg.Convert();
      if      ( m_channel->isCancelled()  )      return errorCode(TAN_SS_RECV_TMO);
      else if ( num_byte < 0              )      return fatalError(m_channel->error());
      else if ( num_byte != int(msg._Length()) ) return errorCode(TAN_SS_ODDRESPONSE);
      else if ( msg.error() != TAN_SS_SUCCESS  ) return errorCode(msg.error());
      else                                       return errorCode(TAN_SS_SUCCESS);
    }                                            return errorCode(TAN_SS_NOTOPEN);
  }                                              return errorCode(TAN_SS_ERROR);
}
// ----------------------------------------------------------------------------
//  Remove alias from nameserver
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::removeAlias ( const char* name )  {
  if ( Status() == TAN_SS_SUCCESS )  {
    if ( m_channel != 0 )  {
      TanMessage msg(TanMessage::DEALIAS);
      nodeWithName(name, 0, msg._Name());
      setLocalAddress(msg.m_sin);
      int num_byte = m_channel->send ( &msg, sizeof(msg));
      if ( num_byte != sizeof(msg) )             return fatalError(m_channel->error());
      num_byte = m_channel->recv ( &msg, sizeof(msg), Receive_TMO);
      msg.Convert();
      if      ( m_channel->isCancelled()  )      return errorCode(TAN_SS_RECV_TMO);
      else if ( num_byte < 0              )      return fatalError(m_channel->error());
      else if ( num_byte != int(msg._Length()) ) return errorCode(TAN_SS_ODDRESPONSE);
      else if ( msg.error() != TAN_SS_SUCCESS  ) return errorCode(msg.error());
      else                                       return errorCode(TAN_SS_SUCCESS);
    }                                            return errorCode(TAN_SS_NOTOPEN);
  }                                              return errorCode(TAN_SS_ERROR);
}
// ----------------------------------------------------------------------------
//  Send shutdown request to nameserver
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::shutdown( const char* node )  {
  return sendAction(TanMessage::SHUTDOWN,node);
}
// ----------------------------------------------------------------------------
//  Dump remote database...
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::dumpDB (const char* node)   {
  return sendAction(TanMessage::DUMP,node);
}
// ----------------------------------------------------------------------------
//  Send shutdown request to nameserver
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::sendAction(int which,const char* node) {
  if ( node == 0 || *node == 0 ) node = m_pcHostName;
  if ( Status() == TAN_SS_SUCCESS )  {
    NetworkChannel::Address radd;
    TanMessage msg(which);
    if ( setInquireAddr(node,msg.m_sin,radd) == TAN_SS_SUCCESS )  {
#if defined(__USING_TCP_ALLOCATOR) // || defined(_WIN32)
      TcpNetworkChannel c;
      TcpNetworkChannel &rcv = c, &snd = c;
      if(c.connect(radd,Connect_TMO) == -1) return errorCode(TAN_SS_NOTOPEN);
      int nbyte = c.send(&msg,sizeof(msg));
#else
      UdpNetworkChannel snd, rcv;
      NetworkChannel::Address sadd = msg.m_sin;
      if ( !rcv.isValid() )                    return rcv.error();
      if ( !snd.isValid() )                    return snd.error();
      msg.m_sin.sin_port = radd.sin_port;
      for(int retry=0; retry<10; retry++ )  {   // Necessary to avaoid
        if ( rcv.bind(radd) < 0 ) SLEEP(10);    // Clashes on the same
        else                       break;       // node...
        if ( retry == 5 )                      return rcv.error();
      }
      int nbyte = snd.send(&msg,sizeof(msg),0,0,&sadd);
#endif
      if ( nbyte == sizeof(msg) )  {
        //lib_rtl_output(LIB_RTL_DEBUG,"receive on port: %04X\n",radd.sin_port);
        nbyte = rcv.recv(&msg,sizeof(msg),Receive_TMO);
        if ( nbyte > 0 )  {
          msg.Convert();
          if      (rcv.isCancelled()       )    return errorCode(TAN_SS_RECV_TMO);
          else if (nbyte != int(msg._Length())) return errorCode(TAN_SS_ODDRESPONSE);
          else if (msg.error()!=TAN_SS_SUCCESS) return errorCode(msg.error());
          else                                  return errorCode(TAN_SS_SUCCESS);
        }                                       return rcv.error();
      }                                         return snd.error();
    }                                           return errorCode(TAN_SS_HOSTNOTFOUND);
  }                                             return errorCode(TAN_SS_ERROR);
}
