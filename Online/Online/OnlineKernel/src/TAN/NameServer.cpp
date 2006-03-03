#include <ctime>
#include <cstdio>
#include <cstdlib>

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

class ReceiveHandler : public EventHandler  {
protected:
  TcpNetworkChannel*    _pNetwork;
  TanDataBase::Entry*   _pEntry;
public:
  ReceiveHandler ( EventReactor* reactor ) 
    : EventHandler(reactor), _pNetwork(0), _pEntry(0)  {
    }
    ReceiveHandler ( EventReactor* reactor, TcpNetworkChannel* chan, TanDataBase::Entry* entry ) 
      : EventHandler(reactor), _pNetwork(chan), _pEntry(entry)  
    {
    }
    virtual ~ReceiveHandler() {
    }
    TcpNetworkChannel* _Channel()     {
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
      TcpNetworkChannel* chan = _Channel();
      chan->_Cancel();
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
class NameService : public EventReactor  {
protected:
  //@Man: Protected member variables
  /// Reference to tan database object
  TanDataBase&         m_tandb;
  /// Smart pointer to hold the connection object
  NetworkConnection*   m_connection;
  /// Service port
  NetworkChannel::Port m_port;

public:
  //@Man Public member functions
  /// Standard constructor
  NameService( NetworkConnection* ptr = 0);
  /// Standard destructor
  virtual ~NameService();
  /// Handle Tan request
  virtual void HandleMessage(TanDataBase::Entry*& ent, TanMessage& rec_msg, TanMessage& snd_msg ); 
  /// Reactor's Event dispatching overlayed method
  virtual int Handle ( EventHandler* /* handler */ )  {
    return NAME_SERVER_SUCCESS;
  }
  /// Abstract member function: Act on Nameservice requests
  virtual void Handle() {
  }
  /// Abstract: Suspend the service
  virtual int Suspend() {
    return NAME_SERVER_SUCCESS;
  }
  /// Abstract: Resume the service
  virtual int Resume()   {
    return NAME_SERVER_SUCCESS;
  }
  /// Run the service
  virtual void Run();
};

/** @class UdpNameService NameServer.cpp TAN/NameServer.cpp
  *
  *  Handle TAN messages in UDP mode
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
  UdpNameService();
  /// Standard destructor
  virtual ~UdpNameService()   {
  }
  /// Reactor's Event dispatching overlayed method
  virtual int Handle ( EventHandler* /* handler */ )  {
    return NAME_SERVER_SUCCESS;
  }
  /// Handle Single request
  virtual void Handle();
  /// Suspend the service
  virtual int Suspend()       {
    return true;
  }
  /// Resume the service
  virtual int Resume()       {
    return true;
  }
};


/** @class TcpNameService NameServer.cpp TAN/NameServer.cpp
  *
  *  Handle TAN messages in TCP/IP mode
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
  EventHandler*      m_pAcceptHandler;
  /// Pointer to TCP networkconnection
  TcpConnection*     m_tcp;
  /// Pointer to Network Channel
  TcpNetworkChannel* m_pNetwork;
public:
  //@Man Public member functions
  /// Standard constructor with initialization
  TcpNameService(int port);
  /// Standard constructor
  TcpNameService();
  /// Standard destructor
  virtual ~TcpNameService();
  /// Overloaded abstract member function: Act on Nameservice requests
  virtual void Handle();
  /// Overloaded abstract member function: Suspend the service
  virtual int Suspend();
  /// Overloaded abstract member function: Resume the service
  virtual int Resume();
  /// Reactor's Event dispatching overlayed method
  int Handle ( EventHandler* handler );
  /// Handle Accept request
  int HandleAcceptRequest ( EventHandler* handler );
  /// Handle Receive request
  int HandleReceiveRequest ( EventHandler* handler );
};

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
NameService::NameService(NetworkConnection* ptr) 
: m_tandb(TanDataBase::Instance()), m_connection(ptr), m_port(NAME_SERVICE_PORT)
{
  ::fprintf(stdout,"+======================================================================+\n");
  ::fprintf(stdout,"|         N A M E S E R V E R      S T A R T I N G                     |\n");
  ::fprintf(stdout,"|         %32s                             |\n",timestr());
  ::fprintf(stdout,"+======================================================================+\n");
  ::fflush(stdout);
  if ( m_connection )  {
    if ( NetworkConnection::NETCONNECTION_SUCCESS != m_connection->Status() )  {
      ::printf("NameService> Error initializing the network connection!\n");
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
void NameService::Run()  {
  while ( 1 ) {
    Handle();
  }
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
void NameService::HandleMessage( TanDataBase::Entry*& ent, TanMessage& rec_msg, TanMessage& snd_msg )    {
  TanMessage::Type func = (TanMessage::Type)ntohl (rec_msg.function);
  NetworkChannel::Port port = 0;

  if ( &snd_msg != &rec_msg ) snd_msg = rec_msg;
  snd_msg.error  = TAN_SS_SUCCESS;
  snd_msg.length = sizeof(snd_msg);
  switch ( func )  {
    case TanMessage::ALLOCATE:                                // Allocation service...
      //::printf("Handle message: ALLOCATE\n");
      if ( (port = m_tandb.AllocatePort(ent)) == 0 )
        snd_msg.error = m_tandb.Error();
      break;
    case TanMessage::DEALLOCATE:
      //::printf("Handle message: DEALLOCATE\n");
      if ( TAN_SS_SUCCESS != m_tandb.FreePort(ent) ) 
        snd_msg.error = m_tandb.Error();
      break;
    case TanMessage::ALIAS:
      //::printf("Handle message: ALIAS\n");
      if ( TAN_SS_SUCCESS != m_tandb.InsertAlias(ent) )
        snd_msg.error = m_tandb.Error();
      break;
    case TanMessage::DEALIAS:
      //::printf("Handle message: DEALIAS\n");
      if ( TAN_SS_SUCCESS != m_tandb.RemoveAlias(ent) )
        snd_msg.error = m_tandb.Error();
      break;
    case TanMessage::DUMP:
      //::printf("Handle message: DUMP\n");
      m_tandb.Dump(stdout);
      break;
    case TanMessage::INQUIRE:                                 // Inquire service...
      //::printf("Handle message: INQUIRE\n");
      if ( (port=m_tandb.FindPort(rec_msg)) == 0 )  {
        snd_msg.error = m_tandb.Error();
      }
      break;
    default:
      //::printf("Handle message: TAN_SS_UNKNOWNMODE\n");
      snd_msg.error = TAN_SS_UNKNOWNMODE;
      break;
  }
  if ( port != 0 )  {
    snd_msg.sin.sin_family  = rec_msg.sin.sin_family;
    snd_msg.sin.sin_port    = htons(port);
  }
  else if ( snd_msg.error != TAN_SS_SUCCESS )  {
    snd_msg.sin.sin_family      = 0;
    snd_msg.sin.sin_port        = 0;
    snd_msg.sin.sin_addr.s_addr = 0;
  }
  snd_msg.error  = htonl (snd_msg.error);
  snd_msg.length = htonl (snd_msg.length);
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
UdpNameService::UdpNameService() : NameService(0)  {
#ifdef SERVICE
  m_port = UdpConnection::servicePort(NAME_SERVICE_NAME);
#endif
  ::fprintf(stdout,"|         U D P         N A M E    S E R V I C E                       |\n");
  ::fprintf(stdout,"|         Port(local): %6d %04X Network:%6d %04X                 |\n",
      m_port, m_port, htons(m_port), htons(m_port));
  ::fprintf(stdout,"+======================================================================+\n");
  ::fflush(stdout);
}

void UdpNameService::Handle ()   {
  // ----------------------------------------------------------------------------
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  TanMessage               req, rep;
  TanDataBase::Entry      *ent;
  UdpNetworkChannel        snd;
  static UdpConnection     conn(m_port);
  NetworkChannel::Address  addr;

  addr.sin_port = conn._Port();
  addr.sin_family = conn._Family();
  addr.sin_addr.s_addr = INADDR_ANY;
  ::memset(addr.sin_zero,0,sizeof(addr.sin_zero));

  int status = conn._RecvChannel()._Recv(&req,sizeof(req),0,0,&addr);
  if ( status != sizeof(req) )  {
    printf("NameService::Handle> Error receiving message\n");
  }
  else  {      // handle the request....
    ent = 0;
    addr.sin_port = req.sin.sin_port;
    HandleMessage( ent, req, rep );
#ifndef SERVICE
    // Swap port to reply connection
    addr.sin_port = htons(m_port+1);
#endif
    //printf("Send to port:%04X\n",addr.sin_port);
    status = snd._Send(&rep,sizeof(rep),0,0,&addr);
    if ( status != sizeof(rep) )  {
      ::printf("NameService::Handle> Error sending message to [%s] on port 0x%X\n",
        inet_ntoa(rep._Address()), rep._Port());
    }
  }
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
#ifdef SERVICE
TcpNameService::TcpNameService() : NameService(m_tcp=new TcpConnection(NAME_SERVICE_NAME))
#else
TcpNameService::TcpNameService() : NameService(m_tcp=new TcpConnection(NAME_SERVICE_PORT))
#endif
{
  m_port = m_tcp->_Port();
  fprintf(stdout,"|         T C P / I P   N A M E    S E R V I C E                       |\n");
  ::fprintf(stdout,"|         Port(local): %6d %04X Network:%6d %04X                 |\n",
      m_port, m_port, htons(m_port), htons(m_port));
  fprintf(stdout,"+======================================================================+\n");
  fflush(stdout);
  m_pAcceptHandler = new EventHandler(this);
  m_pNetwork = &((TcpNetworkChannel&)m_tcp->_RecvChannel());
  m_pNetwork->_QueueAccept ( m_tcp->_Port(), m_pAcceptHandler );              // Rearm
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
TcpNameService::TcpNameService(int port) : NameService(m_tcp=new TcpConnection(port))  {
  m_port = m_tcp->_Port();
  fprintf(stdout,"|         T C P / I P   N A M E    S E R V I C E                       |\n");
  ::fprintf(stdout,"|         Port(local): %6d %04X Network:%6d %04X                 |\n",
      m_port, m_port, htons(m_port), htons(m_port));
  fprintf(stdout,"+======================================================================+\n");
  fflush(stdout);
  m_pAcceptHandler = new EventHandler(this);
  m_pNetwork = &((TcpNetworkChannel&)m_tcp->_RecvChannel());
  m_pNetwork->_QueueAccept ( m_tcp->_Port(), m_pAcceptHandler );              // Rearm
}

TcpNameService::~TcpNameService()   {
  // ----------------------------------------------------------------------------
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  m_pNetwork->_UnqueueIO( m_tcp->_Port() );
  m_pNetwork->_Cancel();
  delete m_pAcceptHandler;
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
void TcpNameService::Handle()   {
  void* par;
  unsigned int fac;
  int sub_status, status;
  status = wtc_wait( &fac, &par, &sub_status );
  printf("Wait (%d,%d) -> %s\n", status, sub_status, lib_rtl_error_message(sub_status));
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNameService::Handle ( EventHandler* handler )  {
  if ( handler == m_pAcceptHandler )
    return HandleAcceptRequest ( handler );
  else if ( handler != 0 )
    return HandleReceiveRequest ( handler );
  return NAME_SERVER_SUCCESS;
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNameService::HandleAcceptRequest ( EventHandler* handler )  {
  int retry = 1, accept_error;                                     //
  NetworkChannel::Address address;                                 //
  NetworkChannel::Channel channel = m_pNetwork->_Accept(address);  // Accept
  accept_error = m_pNetwork->_Error();                             //
  int status = m_pNetwork->_QueueAccept(m_port,handler);           // Rearm ACCEPT
  if ( !lib_rtl_is_success(status) )  {
    printf("HandleAcceptRequest> Accept Rearm FAILED %d RetryCount:%d %s",
      m_pNetwork->_Error(),retry,                                  //
      m_pNetwork->_ErrMsg());                                      //
  }                                                                //
  if ( channel <= 0 )   {                                          // Error!
    return NAME_SERVER_SUCCESS;                                    // Return status code
  }                                                                //
  else  {                                                          // Event handling:
    TanDataBase::Entry* entry = m_tandb.AllocateEntry(channel);    // Allocate database entry
    if ( entry != 0 )  {                                           // SUCCESS:
      ReceiveHandler*    hand = new ReceiveHandler(this);          // Add handler      
      TcpNetworkChannel* chan = new TcpNetworkChannel(channel);    // Create new connection
      hand->_Set ( chan, entry );                                  //   Update handler's parameters
      return HandleReceiveRequest( hand );                         //
    }                                                              // and return
    return TAN_SS_NOMEM;                                           // Failed to allocate TAN slot
  }                                                                //
}                                                                  //

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNameService::HandleReceiveRequest ( EventHandler* handler )  {
  ReceiveHandler*     hand = (ReceiveHandler*)handler;
  TanDataBase::Entry*  ent = hand->_Entry();
  TcpNetworkChannel*  chan = hand->_Channel();
  int status = NAME_SERVER_SUCCESS, num_byte;

  num_byte = chan->_Recv( &ent->_Message(), sizeof(ent->_Message()));
  if ( num_byte <= 1 )  {           // Socket closed by Client
    m_tandb.Close ( ent );          // No need to return error
    //status = chan->_Error();      // condition in this case!
    chan->_UnqueueIO (m_port);
  }
  else {
    TanMessage reply;
    HandleMessage( ent, ent->_Message(), reply);
    num_byte = chan->_Send(&reply, sizeof(reply));
    if ( num_byte <= 0 ) {
      m_tandb.Close ( ent );
      status = chan->_Error();
      chan->_UnqueueIO (m_port);
    }
    else  {
      switch ( ntohl(reply._Function()) )    
      {
      case TanMessage::DEALLOCATE:
        // No task dead message! chan->_Recv(&reply, 1);
        chan->_UnqueueIO (m_port);
        break;
      default:
        if ( ntohl(reply._Error()) == TAN_SS_SUCCESS )  {  // Only way to exit 
          status = chan->_QueueReceive (m_port, hand);     // with success!
          if ( !lib_rtl_is_success(status) ) {
            printf("Error rearming receive: %s",chan->_ErrMsg());
          }
          return status;
        }
        m_tandb.Close ( ent );
        chan->_UnqueueIO (m_port);
        break;
      }
    }
  }
  //printf("HandleReceiveRequest> Close Receive on %d %s\n",chan, strerror(status));
  hand->_Delete();
  return status;
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNameService::Suspend  ()   {
  delete m_connection;
  m_connection = 0;
  m_tcp = 0;
  return NAME_SERVER_SUCCESS;
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNameService::Resume ()   {
  int retry = 0;
New_allocation:
  if ( m_connection != 0 ) Suspend();
  m_connection = m_tcp = new TcpConnection(m_port);
  m_pNetwork = &((TcpNetworkChannel&)m_tcp->_RecvChannel());
  if ( m_pNetwork->_Error() != 0 && ++retry < 5 )  {
    printf("Resume-Retry# %d> %s\n", retry, m_pNetwork->_ErrMsg());
    goto New_allocation;
  }
  return m_pNetwork->_QueueAccept ( m_tcp->_Port(), m_pAcceptHandler );
}

extern "C" int tan_nameserver (int argc, char* argv[]) {
  // ----------------------------------------------------------------------------
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  char *c;
  NameService *srv = 0;
  bool inquirer = false, allocator = false, tcp = false, udp = false;
  while( --argc > 0 )      {
    if ( *(c = *++argv) == '-' )   {
      switch( *++c | 0x20 )  {
         case 'i':  inquirer  = true;   break;
         case 'a':  allocator = true;   break;
         case 't':  tcp       = true;   break;
         case 'u':  udp       = true;   break;
         default:
Options:
           printf("NameServer -<opt>\n");
           printf("  -a             Listen and serve (DE)ALLOCATION requests\n");
           printf("  -i             Listen and serve INQUIRE        requests\n");
           printf("  -tcp           Run service in tcp/ip mode (default:udp/INQUIRE tcp/ALLOCATE)\n");
           printf("  -udp           Run service in udp mode    (default:udp/INQUIRE tcp/ALLOCATE)\n");
           return 0x1;
      }
    }
  }
  if ( (inquirer && allocator) || (!inquirer && !allocator) ) {
    goto Options;
  }
  else if ( inquirer )   {
    if ( tcp ) srv = new TcpNameService(NAME_SERVICE_PORT+1);
    else       srv = new UdpNameService();
  }
  else if ( allocator )   {
    if ( TanDataBase::Initialize() != TAN_SS_SUCCESS )  {
      printf("TcpNameService> Error initializing the DataBase!\n");
      return 1;
    }
    if ( udp ) srv = new UdpNameService();
    else       srv = new TcpNameService();
  }
  if ( !srv ) goto Options;
  srv->Run();
  delete srv;
  return 0x1;
}
