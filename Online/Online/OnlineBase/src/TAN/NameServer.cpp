#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "NET/UdpConnection.h"
#include "NET/TcpConnection.h"
#include "CPP/EventHandler.h"
#include "CPP/SmartPointer.h"
#include "TAN/TanDB.h"
#include "WT/wtdef.h"

#define EINVOPER       0xDEAD

inline const char *timestr (void)  {
  time_t curr;
  time(&curr);
  char *tim = ctime(&curr);
  tim[24] = 0;
  return tim;
}          

class Receivehandler : public CPP::EventHandler  {
protected:
  TcpNetworkChannel*    _pNetwork;
  TanDataBase::Entry*   _pEntry;
public:
  explicit Receivehandler ( EventReactor* reactor ) 
    : EventHandler(reactor), _pNetwork(0), _pEntry(0)  {
  }
  Receivehandler ( EventReactor* reactor, TcpNetworkChannel* chan, TanDataBase::Entry* entry ) 
    : EventHandler(reactor), _pNetwork(chan), _pEntry(entry)  
  {
  }
  virtual ~Receivehandler() {
  }
  TcpNetworkChannel* channel()     {
    return _pNetwork;
  }
  TanDataBase::Entry* _Entry()     {
    return _pEntry;
  }
  void _Set( TcpNetworkChannel* chan, TanDataBase::Entry* entry )  {
    _pNetwork = chan;
    _pEntry   = entry;
  }
  void _Delete()  {
    TcpNetworkChannel* chan = channel();
    chan->cancel();
    delete chan;
    delete this;
  }
};


#define NAME_SERVER_SUCCESS   1
#define NAME_SERVER_ERROR     0

/** @class Nameservice NameServer.cpp TAN/NameServer.cpp
 *
 * Interpretation of nameserver requests
 * - Interprete the received message and build the appropriate
 *   reply.
 *
 *  @author  M.Frank
 *  @version 1.0
 */
class NameService : public CPP::EventReactor  {
protected:
  //@Man: Protected member variables
  /// Reference to tan database object
  TanDataBase&         m_tandb;
  /// Smart pointer to hold the connection object
  NetworkConnection*   m_connection;
  /// Service port
  NetworkChannel::Port m_port;
  /// Shutdown flag    
  bool                 m_shutdown;

public:
  //@Man Public member functions
  /// Standard constructor
  explicit NameService( NetworkConnection* ptr = 0, bool verbose = false);
  /// Standard destructor
  virtual ~NameService();
  /// handle Tan request
  virtual void handleMessage(TanDataBase::Entry*& ent, TanMessage& rec_msg, TanMessage& snd_msg ); 
  /// Reactor's Event dispatching overlayed method
  virtual int handle ( EventHandler* /* handler */ )  {
    return NAME_SERVER_SUCCESS;
  }
  /// Abstract member function: Act on Nameservice requests
  virtual void handle() {
  }
  /// Abstract: suspend the service
  virtual int suspend() {
    return NAME_SERVER_SUCCESS;
  }
  /// Abstract: resume the service
  virtual int resume()   {
    return NAME_SERVER_SUCCESS;
  }
  /// run the service
  virtual void run();
};

/** @class UdpNameService NameServer.cpp TAN/NameServer.cpp
 *
 *  handle TAN messages in UDP mode
 *  A network connection is forseen to reply the 
 *  requested information.                       
 *
 *  @author  M.Frank
 *  @version 1.0
 */
class UdpNameService : public NameService {
public:
  //@Man Public member functions
  /// Standard constructor
  explicit UdpNameService(bool verbose = false);
  /// Standard destructor
  virtual ~UdpNameService()   {
  }
  /// Reactor's Event dispatching overlayed method
  virtual int handle ( CPP::EventHandler* /* handler */ )  {
    return NAME_SERVER_SUCCESS;
  }
  /// handle Single request
  virtual void handle();
  /// suspend the service
  virtual int suspend()       {
    return true;
  }
  /// resume the service
  virtual int resume()       {
    return true;
  }
};


/** @class TcpNameService NameServer.cpp TAN/NameServer.cpp
 *
 *  handle TAN messages in TCP/IP mode
 *  A network connection is forseen to reply the 
 *  requested information.                       
 *
 *  @author  M.Frank
 *  @version 1.0
 */
class TcpNameService : public NameService {
protected:
  //@Man: Protected member variables
  /// Pointer to Accept Event handler
  EventHandler*      m_pAccepthandler;
  /// Pointer to TCP networkconnection
  TcpConnection*     m_tcp;
  /// Pointer to Network Channel
  TcpNetworkChannel* m_pNetwork;
public:
  //@Man Public member functions
  /// Standard constructor with initialization
  TcpNameService(int port, bool verbose = false);
  /// Standard constructor
  explicit TcpNameService(bool verbose = false);
  /// Standard destructor
  virtual ~TcpNameService();
  /// Overloaded abstract member function: Act on Nameservice requests
  virtual void handle();
  /// Overloaded abstract member function: suspend the service
  virtual int suspend();
  /// Overloaded abstract member function: resume the service
  virtual int resume();
  /// Reactor's Event dispatching overlayed method
  int handle ( CPP::EventHandler* handler );
  /// handle Accept request
  int handleAcceptRequest ( CPP::EventHandler* handler );
  /// handle receive request
  int handleReceiveRequest ( CPP::EventHandler* handler );
};

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
NameService::NameService(NetworkConnection* ptr, bool verbose) 
  : m_tandb(TanDataBase::Instance()), m_connection(ptr), m_port(NAME_SERVICE_PORT), m_shutdown(false)
{
  if ( verbose )  {
    ::lib_rtl_output(LIB_RTL_INFO,"+======================================================================+\n");
    ::lib_rtl_output(LIB_RTL_INFO,"|         N A M E S E R V E R      S T A R T I N G                     |\n");
    ::lib_rtl_output(LIB_RTL_INFO,"|         %32s                             |\n",timestr());
    ::lib_rtl_output(LIB_RTL_INFO,"+======================================================================+\n");
    ::fflush(stdout);
  }
  if ( m_connection )  {
    if ( NetworkConnection::NETCONNECTION_SUCCESS != m_connection->Status() )  {
      ::lib_rtl_output(LIB_RTL_ERROR,"NameService> Error initializing the network connection!\n");
      ::exit(ptr->Status());
    }
  }
  ::wtc_init();
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
NameService::~NameService()  {
  if ( m_connection ) delete m_connection;
}
// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
void NameService::run()  {
  while ( 1 ) {
    handle();
  }
}
static void _printEntry(const char* msg, TanDataBase::Entry* ent) {
  if ( msg ) {
    if ( ent ) {
      /*
        ::lib_rtl_output(LIB_RTL_DEBUG,"%s> handle message: %s Name:%s Port=%d [%d] %p\n",
        RTL::nodeName().c_str(),msg,ent->_Name(),ent->port(),
        ntohs(ent->port()),(void*)ent);
        }
        else {
        ::lib_rtl_output(LIB_RTL_DEBUG,"%s> handle message: %s [No entry]\n",RTL::nodeName().c_str(),msg);
      */
    }
  }
}
// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
void NameService::handleMessage( TanDataBase::Entry*& ent, TanMessage& rec_msg, TanMessage& snd_msg )    {
  TanMessage::Type func = (TanMessage::Type)ntohl (rec_msg.function());
  NetworkChannel::Port port = 0;

  if ( &snd_msg != &rec_msg ) snd_msg = rec_msg;
  snd_msg.m_error  = TAN_SS_SUCCESS;
  snd_msg.m_length = sizeof(snd_msg);
  switch ( func )  {
  case TanMessage::ALLOCATE:                                // Allocation service...
    if ( (port = m_tandb.allocatePort(ent)) == 0 )
      snd_msg.m_error = m_tandb.Error();
    _printEntry("ALLOCATE",ent);
    break;
  case TanMessage::DEALLOCATE:
    _printEntry("DEALLOCATE",ent);
    if ( TAN_SS_SUCCESS != m_tandb.freePort(ent) ) 
      snd_msg.m_error = m_tandb.Error();
    break;
  case TanMessage::ALIAS:
    _printEntry("ALIAS",ent);
    if ( TAN_SS_SUCCESS != m_tandb.insertAlias(ent) )
      snd_msg.m_error = m_tandb.Error();
    break;
  case TanMessage::DEALIAS:
    _printEntry("DEALIAS",ent);
    if ( TAN_SS_SUCCESS != m_tandb.removeAlias(ent) )
      snd_msg.m_error = m_tandb.Error();
    break;
  case TanMessage::DUMP:
    _printEntry("DUMP",ent);
    m_tandb.Dump(std::cout);
    break;
  case TanMessage::SHUTDOWN:
    _printEntry("SHUTDOWN",ent);
    ::lib_rtl_output(LIB_RTL_ALWAYS,"+======================================================================+\n");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"|                  TAN NAMESERVER SHUTDOWN requested.                  |\n");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"+======================================================================+\n");
    m_shutdown = true;
    break;
  case TanMessage::INQUIRE:                                 // Inquire service...
    _printEntry("INQUIRE",ent);
    if ( (port=m_tandb.findPort(rec_msg)) == 0 )  {
      snd_msg.m_error = m_tandb.Error();
    }
    break;
  default:
    _printEntry("TAN_SS_UNKNOWNMODE",0);
    snd_msg.m_error = TAN_SS_UNKNOWNMODE;
    break;
  }
  if ( port != 0 )  {
    snd_msg.m_sin.sin_family  = rec_msg.m_sin.sin_family;
    snd_msg.m_sin.sin_port    = htons(port);
  }
  else if ( snd_msg.m_error != TAN_SS_SUCCESS )  {
    snd_msg.m_sin.sin_family      = 0;
    snd_msg.m_sin.sin_port        = 0;
    snd_msg.m_sin.sin_addr.s_addr = 0;
  }
  snd_msg.m_error  = htonl (snd_msg.m_error);
  snd_msg.m_length = htonl (snd_msg.m_length);
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
UdpNameService::UdpNameService(bool verbose) : NameService(0)  {
#ifdef SERVICE
  m_port = UdpConnection::servicePort(NAME_SERVICE_NAME);
#endif
  if ( verbose )  {
    ::lib_rtl_output(LIB_RTL_INFO,"|         U D P         N A M E    S E R V I C E                       |\n");
    ::lib_rtl_output(LIB_RTL_INFO,"|         Port(local): %6d %04X Network:%6d %04X                 |\n",
                     m_port, m_port, htons(m_port), htons(m_port));
    ::lib_rtl_output(LIB_RTL_INFO,"+======================================================================+\n");
    ::fflush(stdout);
  }
}

void UdpNameService::handle ()   {
  // ----------------------------------------------------------------------------
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  TanMessage               req, rep;
  TanDataBase::Entry      *ent;
  UdpNetworkChannel        snd;
  static UdpConnection     conn(m_port);
  NetworkChannel::Address  addr;

  addr.sin_port = conn.port();
  addr.sin_family = conn.family();
  addr.sin_addr.s_addr = INADDR_ANY;
  ::memset(addr.sin_zero,0,sizeof(addr.sin_zero));

  int status = conn.recvChannel().recv(&req,sizeof(req),0,0,&addr);
  if ( status != sizeof(req) )  {
    lib_rtl_output(LIB_RTL_ERROR,"NameService::handle> Error receiving message\n");
  }
  else  {      // handle the request....
    ent = 0;
    addr.sin_port = req.m_sin.sin_port;
    handleMessage( ent, req, rep );
#ifndef SERVICE
    // Swap port to reply connection
    addr.sin_port = htons(m_port+1);
#endif
    //lib_rtl_output(LIB_RTL_DEBUG,"send to port:%04X\n",addr.sin_port);
    status = snd.send(&rep,sizeof(rep),0,0,&addr);
    if ( status != sizeof(rep) )  {
      ::lib_rtl_output(LIB_RTL_ERROR,"NameService::handle> Error sending message to [%s] on port 0x%X\n",
                       inet_ntoa(rep.address()), rep.port());
    }
  }
  if ( m_shutdown ) ::exit(0);
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
#ifdef SERVICE
TcpNameService::TcpNameService(bool verbose) : NameService(m_tcp=new TcpConnection(NAME_SERVICE_NAME),verbose)
#else
                                               TcpNameService::TcpNameService(bool verbose) : NameService(m_tcp=new TcpConnection(NAME_SERVICE_PORT),verbose)
#endif
{
  m_port = m_tcp->port();
  if ( verbose )  {
    ::lib_rtl_output(LIB_RTL_INFO,"|         T C P / I P   N A M E    S E R V I C E                       |\n");
    ::lib_rtl_output(LIB_RTL_INFO,"|         Port(local): %6d %04X Network:%6d %04X                 |\n",
                     m_port, m_port, htons(m_port), htons(m_port));
    ::lib_rtl_output(LIB_RTL_INFO,"+======================================================================+\n");
  }
  m_pAccepthandler = new EventHandler(this);
  m_pNetwork = &((TcpNetworkChannel&)m_tcp->recvChannel());
  m_pNetwork->queueAccept ( m_port, m_pAccepthandler );              // Rearm
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
TcpNameService::TcpNameService(int port, bool verbose) : NameService(m_tcp=new TcpConnection(port), verbose)  {
  m_port = m_tcp->port();
  if ( verbose )  {
    ::lib_rtl_output(LIB_RTL_INFO,"|         T C P / I P   N A M E    S E R V I C E                       |\n");
    ::lib_rtl_output(LIB_RTL_INFO,"|         Port(local): %6d %04X Network:%6d %04X                 |\n",
                     m_port, m_port, htons(m_port), htons(m_port));
    ::lib_rtl_output(LIB_RTL_INFO,"+======================================================================+\n");
  }
  m_pAccepthandler = new EventHandler(this);
  m_pNetwork = &((TcpNetworkChannel&)m_tcp->recvChannel());
  m_pNetwork->reuseAddress();
  m_pNetwork->queueAccept ( m_port, m_pAccepthandler );              // Rearm
}

TcpNameService::~TcpNameService()   {
  // ----------------------------------------------------------------------------
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  m_pNetwork->_unqueueIO( m_tcp->port() );
  m_pNetwork->cancel();
  delete m_pAccepthandler;
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
void TcpNameService::handle()   {
  void* par;
  unsigned int fac;
  int sub_status, status;
  status = ::wtc_wait( &fac, &par, &sub_status );
  ::lib_rtl_output(LIB_RTL_INFO,"Wait (%d,%d) -> %s\n", status, sub_status, lib_rtl_error_message(sub_status));
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNameService::handle ( EventHandler* handler )  {
  int status = NAME_SERVER_SUCCESS;
  try {
    if ( handler == m_pAccepthandler )
      status=handleAcceptRequest ( handler );
    else if ( handler != 0 )
      status=handleReceiveRequest ( handler );
    if ( m_shutdown ) ::exit(0);
  }
  catch(const std::exception& e)  {
    ::lib_rtl_output(LIB_RTL_ERROR,"TcpNameService::handle> Exception: %s",e.what());
  }
  catch(...)  {
    ::lib_rtl_output(LIB_RTL_ERROR,"TcpNameService::handle> UNKNOWN Exception");
  }
  return status;
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNameService::handleAcceptRequest ( EventHandler* handler )  {
  int retry = 1;                                                   //
  NetworkChannel::Address address;                                 //
  NetworkChannel::Channel channel = m_pNetwork->accept(address);   // Accept
  //int accept_error = m_pNetwork->error();                          //
  int status = m_pNetwork->queueAccept(m_port,handler);            // Rearm ACCEPT
  if ( !lib_rtl_is_success(status) )  {
    lib_rtl_output(LIB_RTL_ERROR,"handleAcceptRequest> Accept Rearm FAILED %d RetryCount:%d %s",
                   m_pNetwork->error(),retry,m_pNetwork->errMsg());             //
  }                                                                //
  if ( channel <= 0 )   {                                          // Error!
    return NAME_SERVER_SUCCESS;                                    // Return status code
  }                                                                //
  else  {                                                          // Event handling:
    TanDataBase::Entry* entry = m_tandb.AllocateEntry(channel);    // Allocate database entry
    if ( entry != 0 )  {                                           // SUCCESS:
      Receivehandler*    hand = new Receivehandler(this);          // Add handler      
      TcpNetworkChannel* chan = new TcpNetworkChannel(channel);    // Create new connection
      chan->reuseAddress();
      hand->_Set ( chan, entry );                                  //   Update handler's parameters
      return handleReceiveRequest( hand );                         //
    }                                                              // and return
    return TAN_SS_NOMEM;                                           // Failed to allocate TAN slot
  }                                                                //
}                                                                  //

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNameService::handleReceiveRequest ( EventHandler* handler )  {
  Receivehandler*     hand = (Receivehandler*)handler;
  TanDataBase::Entry*  ent = hand->_Entry();
  TcpNetworkChannel*  chan = hand->channel();
  int status = NAME_SERVER_SUCCESS, num_byte;

  num_byte = chan->recv( &ent->_Message(), sizeof(ent->_Message()));
  if ( num_byte <= 1 )  {          // Socket closed by Client
    m_tandb.Close ( ent );         // No need to return error
    //status = chan->error();      // condition in this case!
    chan->_unqueueIO (m_port);
  }
  else {
    TanMessage reply;
    handleMessage( ent, ent->_Message(), reply);
    num_byte = chan->send(&reply, sizeof(reply));
    if ( num_byte <= 0 ) {
      m_tandb.Close ( ent );
      status = chan->error();
      chan->_unqueueIO (m_port);
    }
    else  {
      int func = ntohl(reply.function());
      switch( func ) {
        //case TanMessage::DUMP:
        //case TanMessage::INQUIRE:       // Inquire service...
      case TanMessage::DEALLOCATE:    // Deallocate port
        // Here the channel must be closed.
        // No task dead message! chan->recv(&reply, 1);
        chan->_unqueueIO (m_port);
        break;
      default:
        if ( ntohl(reply.error()) == TAN_SS_SUCCESS )  {  // Only way to exit 
          status = chan->queueReceive (m_port, hand);     // with success!
          if ( !lib_rtl_is_success(status) ) {
            ::lib_rtl_output(LIB_RTL_ERROR,"Error rearming receive: %s",chan->errMsg());
          }
          if ( func != TanMessage::INQUIRE )
            return status;
        }
        m_tandb.Close ( ent );
        chan->_unqueueIO (m_port);
        break;
      }
    }
  }
  //lib_rtl_output(LIB_RTL_DEBUG,"handleReceiveRequest> Close receive on %d %s\n",chan, strerror(status));
  hand->_Delete();
  return status;
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNameService::suspend  ()   {
  delete m_connection;
  m_connection = 0;
  m_tcp = 0;
  return NAME_SERVER_SUCCESS;
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNameService::resume ()   {
  int retry = 0;
 New_allocation:
  if ( m_connection != 0 ) suspend();
  m_connection = m_tcp = new TcpConnection(m_port);
  m_pNetwork = &((TcpNetworkChannel&)m_tcp->recvChannel());
  if ( m_pNetwork->error() != 0 && ++retry < 5 )  {
    lib_rtl_output(LIB_RTL_INFO,"resume-Retry# %d> %s\n", retry, m_pNetwork->errMsg());
    goto New_allocation;
  }
  return m_pNetwork->queueAccept ( m_tcp->port(), m_pAccepthandler );
}

extern "C" int tan_nameserver (int argc, char* argv[]) {
  // ----------------------------------------------------------------------------
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  char *c;
  NameService *srv = 0;
  bool inquirer = false, allocator = false, tcp = false, udp = false, nowait = false;
  bool delgbl = false, verbose = false;
  while( --argc > 0 )      {
    if ( *(c = *++argv) == '-' )   {
      switch( *++c | 0x20 )  {
      case 'i':  inquirer  = true;   break;
      case 'a':  allocator = true;   break;
      case 't':  tcp       = true;   break;
      case 'u':  udp       = true;   break;
      case 'n':  nowait    = true;   break;
      case 'v':  verbose   = true;   break;
      case 'd':  delgbl    = true;   break;
      default:
      Options:
        ::lib_rtl_output(LIB_RTL_ALWAYS,"NameServer -<opt>\n");
        ::lib_rtl_output(LIB_RTL_ALWAYS,"  -a(llocator)   listen and serve (DE)ALLOCATION requests\n");
        ::lib_rtl_output(LIB_RTL_ALWAYS,"  -i(nqquirer)   listen and serve INQUIRE        requests\n");
        ::lib_rtl_output(LIB_RTL_ALWAYS,"  -tcp           run service in tcp/ip mode (default:udp/INQUIRE tcp/ALLOCATE)\n");
        ::lib_rtl_output(LIB_RTL_ALWAYS,"  -udp           run service in udp mode    (default:udp/INQUIRE tcp/ALLOCATE)\n");
        ::lib_rtl_output(LIB_RTL_ALWAYS,"  -n(owait)      Continue execution after routine call. Requires wtc_wait later!\n");
        ::lib_rtl_output(LIB_RTL_ALWAYS,"  -v(erbose)     Print header at startup.\n");
        return 0x1;
      }
    }
  }
  if ( (inquirer && allocator) || (!inquirer && !allocator) ) {
    goto Options;
  }
  else if ( inquirer )   {
    if ( tcp ) srv = new TcpNameService(NAME_SERVICE_PORT+1, verbose);
    else       srv = new UdpNameService(verbose);
  }
  else if ( allocator )   {
    if ( delgbl ) {
#ifdef __linux
      ::unlink("/dev/shm/sem.TANDB");
      ::unlink("/dev/shm/sem.TAN_PUBAREA_lock");
      ::unlink("/dev/shm/TAN_PUBAREA");
#endif
    }
    if ( TanDataBase::initialize() != TAN_SS_SUCCESS )  {
      lib_rtl_output(LIB_RTL_ERROR,"TcpNameService> Error initializing the DataBase!\n");
      return 0x0;
    }
    if ( udp ) srv = new UdpNameService(verbose);
    else       srv = new TcpNameService(verbose);
  }
  if ( !srv ) goto Options;
  if ( !nowait )  {
    srv->run();
    delete srv;
  }
  return 0x1;
}
