#define _TanInterface_C_

#include "RTL/rtl.h"
#include "NET/defs.h"
#include "TAN/TanInterface.h"
#include "NET/UdpNetworkChannel.h"
#include "NET/TcpNetworkChannel.h"

#include "AMS/amsdef.h"
#define SLEEP(x)  lib_rtl_sleep(1000*x);

#include <map>
#include <string>
extern "C" TanInterface* taninterface() {
  return &TanInterface::Instance();
}
// ----------------------------------------------------------------------------
// C Interface: Allocate port number from local server
// ----------------------------------------------------------------------------
extern "C" int tan_allocate_port_number (const char *name, TanMessage::Port *port)  {
  return TanInterface::Instance().AllocatePort(name,port);
}
// ----------------------------------------------------------------------------
// C Interface: Deallocate port number on local nameserver
// ----------------------------------------------------------------------------
extern "C" int tan_deallocate_port_number (const char *name) {
  return TanInterface::Instance().DeallocatePort(name);
}
// ----------------------------------------------------------------------------
// C Interface: Get remote address by name
// ----------------------------------------------------------------------------
extern "C" int tan_get_address_by_name ( const char* name, TanMessage::Address *sad)  {
  return TanInterface::Instance().GetAddressByName(name,*sad);
}
// ----------------------------------------------------------------------------
// C Interface:
// ----------------------------------------------------------------------------
extern "C" int tan_declare_alias (const char* name)   {
  return TanInterface::Instance().DeclareAlias(name);
}
// ----------------------------------------------------------------------------
// C Interface:
// ----------------------------------------------------------------------------
extern "C" int tan_remove_alias (const char* name)   {
  return TanInterface::Instance().RemoveAlias(name);
}
// ----------------------------------------------------------------------------
// C Interface:
// ----------------------------------------------------------------------------
extern "C" int tan_dump_dbase (const char* node)   {
  return TanInterface::Instance().DumpDB(node);
}
// ----------------------------------------------------------------------------
// C Interface:
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Instantiator: create object
//                                      M.Frank
// ----------------------------------------------------------------------------
TanInterface& TanInterface::Instance()   {
  static TanInterface s_instance;
  return s_instance;
}
// ----------------------------------------------------------------------------
// Constructor: initialise the service
// Note: The addresses are NOT in network byte ordering!
//                                      M.Frank
// ----------------------------------------------------------------------------
TanInterface::TanInterface()  {
  int status = gethostname (_pcHostName, sizeof (_pcHostName));
  char *dot  = strchr(_pcHostName,'.');
  _portAllocated = 0;
  if ( dot != 0   )  *dot = 0;
  if ( status < 0 )  status = gethostname (_pcHostName, sizeof (_pcHostName));
  if ( status < 0 )                                                  goto Error;
  _pLocalHost = GetHostByName(_pcHostName);
  if ( _pLocalHost == 0 )                                            goto Error;
  SetLocalAddress(_sinudp);
  SetLocalAddress(_sintcp);
#ifdef _SERVICE
  _pServiceUDP       = ::getservbyname (NAME_SERVICE_NAME, "udp");
  _pServiceTCP       = ::getservbyname (NAME_SERVICE_NAME, "tcp");
  if ( _pServiceUDP == 0 || _pServiceTCP == 0 )                      goto Error;
  _sinudp.sin_port   = htons(_pServiceUDP->s_port);
  _sintcp.sin_port   = htons(_pServiceTCP->s_port);
#else
  _sintcp.sin_port   = _sinudp.sin_port = NAME_SERVICE_PORT;
#endif
  m_channel          = 0;
  m_status           = TAN_SS_SUCCESS;
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
int TanInterface::ErrorCode(int tan_error)  {
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
int TanInterface::FatalError(int code)  {
  delete m_channel;
  m_channel = 0;
  return code;
}
// ----------------------------------------------------------------------------
//  get hostentry from inet db by host name
//                                      M.Frank
// ----------------------------------------------------------------------------
hostent* TanInterface::GetHostByName(const char* name)  {
  hostent *h = 0;
#ifdef _OSK
  h = _gethostbyname((char*)name);
#endif
  if ( h == 0 ) h = gethostbyname(name);
  return h;
}
// ----------------------------------------------------------------------------
// given a node name, fill the socket address from the inet db.
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::SetInquireAddr(const char* node, NetworkChannel::Address& sin, 
                                 NetworkChannel::Address& rin )  
{
  struct hostent *rp = GetHostByName (node);
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
  rin          = _sinudp;
#endif
  rin.sin_port = se ? se->s_port : htons(NAME_SERVICE_PORT+1);
  // To use only ONE nameserver and no predefined service 
  // for both allocation and inquire enable this:
  // rin.sin_port = se ? se->s_port : htons(NAME_SERVICE_PORT);
  return TAN_SS_SUCCESS;
}
// ----------------------------------------------------------------------------
//  Fill local address
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::SetLocalAddress  ( NetworkChannel::Address& sin )       {
#ifdef _VMS
  memcpy (&sin.sin_addr, *_pLocalHost->h_addr_list, _pLocalHost->h_length);
#else
  memcpy (&sin.sin_addr, _pLocalHost->h_addr, _pLocalHost->h_length);
#endif
  sin.sin_family = AF_INET;
  memset(sin.sin_zero,0,sizeof(sin.sin_zero));
  sin.sin_port = htons(_sintcp.sin_port);
  return TAN_SS_SUCCESS;
}
// ----------------------------------------------------------------------------
// convert strings of the type NODE::TASK  and TASK@NODE.DOMAIN into a reasonable
// task - and node name 
//                                      M.Frank
// ----------------------------------------------------------------------------
void TanInterface::GetNodeWithName(const char* name, char* node, char* proc)  {
  int n, s = 0;
  char *p;
  if ( 0 != (p=strstr(name,"::")) )    {      // DECNET STYLE
    s = 0;
    if (node != 0)  {
      strncpy (node, name, n = p - name);
      node [n] = 0;
    }
    if (proc!= 0)  strcpy (proc, p + 2);
  }
  else if ( 0 != (p=strchr(name,'@')) )    {
    s = 1;
    if (node != 0)  {                         // INTERNET STYLE
      strcpy (node, p + 1);
    }
    if (proc!= 0)   {
      strncpy (proc, name, n = p-name);
      proc [n] = 0;
    }
  }
  else    {
    if (node != 0) strcpy (node, _pcHostName);
    if (proc != 0) strcpy (proc, name);
  }
  for(p=node; p && *p; p++) *p = s==1 ? ::tolower(*p) : ::toupper(*p);
  for(p=proc; p && *p; p++) *p = s==1 ? ::tolower(*p) : ::toupper(*p);
}
// ----------------------------------------------------------------------------
//  retrieve network address of a task given his name
//  -- accesses the local/remote nameserver task.
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::GetAddressByName(const char* name, NetworkChannel::Address& sad)  {
  if ( Status() == TAN_SS_SUCCESS )  {
    char node[32];
    NetworkChannel::Address radd;
    TanMessage msg(TanMessage::INQUIRE);
    GetNodeWithName(name,node,msg._Name());
    SetLocalAddress(msg.sin);
    if ( SetInquireAddr(node,msg.sin,radd) == TAN_SS_SUCCESS )  {
#if defined(__USING_TCP_ALLOCATOR) // || defined(_WIN32)
      TcpNetworkChannel c;
      TcpNetworkChannel &rcv = c, &snd = c;
      if(c._Connect(radd,Connect_TMO) == -1) return ErrorCode(TAN_SS_NOTOPEN);
      int nbyte = snd._Send(&msg,sizeof(msg));
#else
      UdpNetworkChannel snd, rcv;
      NetworkChannel::Address sadd = msg.sin;
      if ( !rcv._IsValid() )                    return rcv._Error();
      if ( !snd._IsValid() )                    return snd._Error();
      msg.sin.sin_port = radd.sin_port;
      for(int retry=0; retry<10; retry++ )  {   // Necessary to avaoid
        if ( rcv._Bind(radd) < 0 ) SLEEP(10)    // Clashes on the same
        else                       break;       // node...
        if ( retry == 5 )                     return rcv._Error();
      }
      int nbyte = snd._Send(&msg,sizeof(msg),0,0,&sadd);
#endif
      if ( nbyte == sizeof(msg) )  {
        // printf("Receive on port: %04X\n",radd.sin_port);
        nbyte = rcv._Recv(&msg,sizeof(msg),Receive_TMO);
        if ( nbyte > 0 )  {
          msg.Convert();
          if ( nbyte != int(msg._Length()) )    return ErrorCode(TAN_SS_ODDRESPONSE);
          if ( msg._Error() != TAN_SS_SUCCESS ) return ErrorCode(msg._Error());
          sad = msg.sin;                        return ErrorCode(TAN_SS_SUCCESS);
        }                                       // Timeout fired!
        else if ( rcv._IsCancelled()         )  return ErrorCode(TAN_SS_RECV_TMO);
        else                                    return rcv._Error();
      }                                         return snd._Error();
    }                                           return ErrorCode(TAN_SS_HOSTNOTFOUND);
  }                                             return ErrorCode(TAN_SS_ERROR);
}
// ----------------------------------------------------------------------------
// Allocate port number from local name server
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::AllocatePort(const char* name, NetworkChannel::Port *port)  {
  if ( Status() == TAN_SS_SUCCESS )  {
    if ( m_channel == 0 )  {
      m_channel = new TcpNetworkChannel;
      TanMessage msg(TanMessage::ALLOCATE);
      SetLocalAddress(msg.sin);
      if ( m_channel->_Connect(msg.sin,Connect_TMO) != -1 )  {
        GetNodeWithName(name, 0 , msg._Name());
        if ( m_channel->_Send(&msg,sizeof(msg)) == sizeof(msg) )  {
          int num_byte = m_channel->_Recv ( &msg, sizeof(msg), Receive_TMO);
          if ( num_byte > 0 )  {
            msg.Convert();
            if ( num_byte != int(msg._Length()))return FatalError( ErrorCode(TAN_SS_ODDRESPONSE) );
            if ( msg._Error() != TAN_SS_SUCCESS)return FatalError( ErrorCode(msg._Error()) );
            _portAllocated = ntohs(msg.sin.sin_port)+1;
            *port = msg.sin.sin_port;           return ErrorCode( TAN_SS_SUCCESS );
          }                                     // Receive timeout fired
          else if ( m_channel->_IsCancelled())  return FatalError(ErrorCode(TAN_SS_RECV_TMO));
          else                                  return FatalError(m_channel->_Error() );
        }                                       return FatalError(m_channel->_Error() );
      }                                         // Connect timeout fired
      else if ( m_channel->_IsCancelled() )     return FatalError(m_channel->_Error());
      else                                      return FatalError(m_channel->_Error());
    }                                           return ErrorCode(TAN_SS_OPEN);
  }                                             return ErrorCode(TAN_SS_ERROR);
}
// ----------------------------------------------------------------------------
// Deallocate port number from local name server
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::DeallocatePort(const char* name)  {
  if ( Status() == TAN_SS_SUCCESS )  {
    if ( m_channel != 0 )  {
      TanMessage msg(TanMessage::DEALLOCATE);
      GetNodeWithName(name, 0 ,msg._Name() );
      SetLocalAddress(msg.sin);
      int num_byte = m_channel->_Send(&msg,sizeof(msg));
      if ( num_byte != sizeof(msg) )            return FatalError(m_channel->_Error());
      num_byte = m_channel->_Recv (&msg,sizeof(msg),Receive_TMO);
      if      ( m_channel->_IsCancelled() )     return ErrorCode(TAN_SS_RECV_TMO);
      else if ( num_byte == sizeof(msg)  )      return FatalError(TAN_SS_SUCCESS);
      else                                      return FatalError(ErrorCode(TAN_SS_MADSRV));
    }                                           return ErrorCode(TAN_SS_NOTOPEN);
  }                                             return ErrorCode(TAN_SS_ERROR);
}
// ----------------------------------------------------------------------------
//  Declare alias to nameserver
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::DeclareAlias ( const char* name )  {
  if ( Status() == TAN_SS_SUCCESS )  {
    if ( m_channel != 0 )  {
      TanMessage msg(TanMessage::ALIAS);
      GetNodeWithName(name, 0, msg._Name());
      SetLocalAddress(msg.sin);
      int num_byte = m_channel->_Send (&msg,sizeof(msg));
      if ( num_byte != sizeof(msg) )             return FatalError(m_channel->_Error());
      num_byte = m_channel->_Recv (&msg,sizeof(msg),Receive_TMO);
      msg.Convert();
      if      ( m_channel->_IsCancelled()  )     return ErrorCode(TAN_SS_RECV_TMO);
      else if ( num_byte < 0              )      return FatalError(m_channel->_Error());
      else if ( num_byte != int(msg._Length()) ) return ErrorCode(TAN_SS_ODDRESPONSE);
      else if ( msg._Error() != TAN_SS_SUCCESS ) return ErrorCode(msg._Error());
      else                                       return ErrorCode(TAN_SS_SUCCESS);
    }                                            return ErrorCode(TAN_SS_NOTOPEN);
  }                                              return ErrorCode(TAN_SS_ERROR);
}
// ----------------------------------------------------------------------------
//  Remove alias from nameserver
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::RemoveAlias ( const char* name )  {
  if ( Status() == TAN_SS_SUCCESS )  {
    if ( m_channel != 0 )  {
      TanMessage msg(TanMessage::DEALIAS);
      GetNodeWithName(name, 0, msg._Name());
      SetLocalAddress(msg.sin);
      int num_byte = m_channel->_Send ( &msg, sizeof(msg));
      if ( num_byte != sizeof(msg) )             return FatalError(m_channel->_Error());
      num_byte = m_channel->_Recv ( &msg, sizeof(msg), Receive_TMO);
      msg.Convert();
      if      ( m_channel->_IsCancelled()  )     return ErrorCode(TAN_SS_RECV_TMO);
      else if ( num_byte < 0              )      return FatalError(m_channel->_Error());
      else if ( num_byte != int(msg._Length()) ) return ErrorCode(TAN_SS_ODDRESPONSE);
      else if ( msg._Error() != TAN_SS_SUCCESS ) return ErrorCode(msg._Error());
      else                                       return ErrorCode(TAN_SS_SUCCESS);
    }                                            return ErrorCode(TAN_SS_NOTOPEN);
  }                                              return ErrorCode(TAN_SS_ERROR);
}
// ----------------------------------------------------------------------------
//  Dump remote database...
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanInterface::DumpDB (const char* node)   {
  if ( Status() == TAN_SS_SUCCESS )  {
    NetworkChannel::Address radd;
    TanMessage msg(TanMessage::DUMP);
    if ( SetInquireAddr(node,msg.sin,radd) == TAN_SS_SUCCESS )  {
#if defined(__USING_TCP_ALLOCATOR) // || defined(_WIN32)
      TcpNetworkChannel c;
      TcpNetworkChannel &rcv = c, &snd = c;
      if(c._Connect(radd,Connect_TMO) == -1) return ErrorCode(TAN_SS_NOTOPEN);
      int nbyte = c._Send(&msg,sizeof(msg));
#else
      UdpNetworkChannel snd, rcv;
      NetworkChannel::Address sadd = msg.sin;
      if ( !rcv._IsValid() )                    return rcv._Error();
      if ( !snd._IsValid() )                    return snd._Error();
      msg.sin.sin_port = radd.sin_port;
      for(int retry=0; retry<10; retry++ )  {   // Necessary to avaoid
        if ( rcv._Bind(radd) < 0 ) SLEEP(10)    // Clashes on the same
        else                       break;       // node...
        if ( retry == 5 )                     return rcv._Error();
      }
      int nbyte = snd._Send(&msg,sizeof(msg),0,0,&sadd);
#endif
      if ( nbyte == sizeof(msg) )  {
        //printf("Receive on port: %04X\n",radd.sin_port);
        nbyte = rcv._Recv(&msg,sizeof(msg),Receive_TMO);
        if ( nbyte > 0 )  {
          msg.Convert();
          if      (rcv._IsCancelled()       )   return ErrorCode(TAN_SS_RECV_TMO);
          else if (nbyte != int(msg._Length())) return ErrorCode(TAN_SS_ODDRESPONSE);
          else if (msg._Error()!=TAN_SS_SUCCESS)return ErrorCode(msg._Error());
          else                                  return ErrorCode(TAN_SS_SUCCESS);
        }                                       return rcv._Error();
      }                                         return snd._Error();
    }                                           return ErrorCode(TAN_SS_HOSTNOTFOUND);
  }                                             return ErrorCode(TAN_SS_ERROR);
}
