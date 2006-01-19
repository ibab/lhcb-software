#include <ctime>
#include <cstdio>
#include <cstdlib>

#include <NET/UdpConnection.h>
#include "NET/TcpConnection.h"
#include "CPP/EventHandler.h"
#include <CPP/SmartPointer.h>
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

/// Basic Nameservice handler class: Interpretation of nameserver requests
/**

{\Large{\bfclass NameService}}
\\
NameService class:                                                \\
- Interprete the received message and build the appropriate       \\
reply.                                                          \\
\\
M.Frank                                   \\
*/
class NameService : public EventReactor  {
protected:
  //@Man: Protected member variables
  /// Reference to tan database object
  TanDataBase&       tandb;
  /// Smart pointer to hold the connection object
  NetworkConnection* connection;
public:
  //@Man Public member functions
  /// Standard constructor
  NameService( NetworkConnection* ptr );
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

/// InquireNameService class: Handle INQUIRE messages
/**

{\Large{\bfclass InquireNameService}}
\\
InquireNameService class:                                        \\
A network connection is forseen to reply the          \\
requested information.                                          \\
\\
M.Frank                                   \\
*/
class InquireNameService : public NameService {
protected:
  /// Smart pointer to hold the connection object
  UdpConnection*    udpconnection;
  TcpNetworkChannel _tcpchan;
public:
  //@Man Public member functions
  /// Standard constructor
  InquireNameService();
  /// Standard destructor
  virtual ~InquireNameService()   {
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


/// AllocatorNameService class: Handle INQUIRE messages
/**

{\Large{\bfclass AllocatorNameService}}                       
\\
AllocatorNameService class:                                     \\
A network connection is forseen to reply the          \\
requested information.                                          \\
\\
M.Frank                                   \\
*/
class AllocatorNameService : public NameService {
protected:
  //@Man: Protected member variables
  /// Pointer to Accept Event handler
  EventHandler*   _pAcceptHandler;
  /// Pointer to TCP networkconnection
  TcpConnection*     _tcp;
  /// Pointer to Network Channel
  TcpNetworkChannel* _pNetwork;
public:
  //@Man Public member functions
  /// Standard constructor
  AllocatorNameService();
  /// Standard destructor
  virtual ~AllocatorNameService();
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
NameService::NameService(NetworkConnection* ptr) : tandb(TanDataBase::Instance()), connection(ptr)  
{
  ::fprintf(stdout,"+======================================================================+\n");
  ::fprintf(stdout,"|         N A M E S E R V E R      S T A R T I N G                     |\n");
  ::fprintf(stdout,"|         %32s                             |\n",timestr());
  ::fprintf(stdout,"+======================================================================+\n");
  ::fflush(stdout);
  if ( NetworkConnection::NETCONNECTION_SUCCESS != ptr->Status() )  {
    ::printf("NameService> Error initializing the network connection!\n");
    ::exit(ptr->Status());
  }
  ::wtc_init();
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
NameService::~NameService()  {
  delete connection;
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
  snd_msg.error    = 0;
  snd_msg.length = sizeof(snd_msg);
  switch ( func )  {
    case TanMessage::ALLOCATE:                                // Allocation service...
      //::printf("Handle message: ALLOCATE\n");
      if ( (port = tandb.AllocatePort(ent)) == 0 )
        snd_msg.error = tandb.Error();
      break;
    case TanMessage::DEALLOCATE:
      //::printf("Handle message: DEALLOCATE\n");
      if ( TAN_SS_SUCCESS != tandb.FreePort(ent) ) 
        snd_msg.error = tandb.Error();
      break;
    case TanMessage::ALIAS:
      //::printf("Handle message: ALIAS\n");
      if ( TAN_SS_SUCCESS != tandb.InsertAlias(ent) )
        snd_msg.error = tandb.Error();
      break;
    case TanMessage::DEALIAS:
      //::printf("Handle message: DEALIAS\n");
      if ( TAN_SS_SUCCESS != tandb.RemoveAlias(ent) )
        snd_msg.error = tandb.Error();
      break;
    case TanMessage::DUMP:
      //::printf("Handle message: DUMP\n");
      tandb.Dump(stdout);
      break;
    case TanMessage::INQUIRE:                                 // Inquire service...
      //::printf("Handle message: INQUIRE\n");
      if ( (port=tandb.FindPort(rec_msg)) == 0 )  {
        snd_msg.error = tandb.Error();
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
  else if ( snd_msg.error != 0 )  {
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
InquireNameService::InquireNameService() 
#ifdef SERVICE
: NameService(udpconnection=new UdpConnection(NAME_SERVICE_NAME))
#else
: NameService(udpconnection=new UdpConnection(NAME_SERVICE_PORT))
#endif
{
  ::fprintf(stdout,"|         I N Q U I R E            S E R V I C E                       |\n");
  ::fprintf(stdout,"+======================================================================+\n");
  ::fflush(stdout);
}

void InquireNameService::Handle ()   {
  // ----------------------------------------------------------------------------
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  TanMessage req, rep;
  TanDataBase::Entry *ent;
  NetworkChannel::Address addr;
  addr.sin_family      = udpconnection->_Family();
  addr.sin_port        = udpconnection->_Port();
  addr.sin_addr.s_addr = INADDR_ANY;
  memset(addr.sin_zero,0,sizeof(addr.sin_zero));
  int status = connection->_RecvChannel()._Recv( &req, sizeof(req), 0, 0, &addr);
  if ( status != sizeof(req) )  {
    printf("NameService::Handle> Error receiving message\n");
  }
  else  {      // handle the request....
    ent = 0;
    addr.sin_port = req.sin.sin_port;
    HandleMessage( ent, req, rep );
    // Swap port to reply connection
#ifndef SERVICE
    addr.sin_port = htons(htons(udpconnection->_Port())+1);
#endif
    status = connection->_SendChannel()._Send (&rep, sizeof(rep), 0, 0, &addr);
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
AllocatorNameService::AllocatorNameService() : NameService(_tcp=new TcpConnection(NAME_SERVICE_NAME))  {
#else
AllocatorNameService::AllocatorNameService() : NameService(_tcp=new TcpConnection(NAME_SERVICE_PORT))  {
#endif
  fprintf(stdout,"|         A L L O C A T I O N      S E R V I C E                       |\n");
  fprintf(stdout,"+======================================================================+\n");
  fflush(stdout);
  _pAcceptHandler = new EventHandler(this);
  _pNetwork = &((TcpNetworkChannel&)_tcp->_RecvChannel());
  _pNetwork->_QueueAccept ( _tcp->_Port(), _pAcceptHandler );              // Rearm
}

AllocatorNameService::~AllocatorNameService()   {
  // ----------------------------------------------------------------------------
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  _pNetwork->_UnqueueIO( _tcp->_Port() );
  _pNetwork->_Cancel();
  delete _pAcceptHandler;
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
void AllocatorNameService::Handle()   {
  void* par;
  unsigned int fac;
  int sub_status, status;
  status = wtc_wait( &fac, &par, &sub_status );
  printf("Wait (%d,%d) -> %s\n", status, sub_status, lib_rtl_error_message(sub_status));
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
int AllocatorNameService::Handle ( EventHandler* handler )  {
  if ( handler == _pAcceptHandler )
    return HandleAcceptRequest ( handler );
  else if ( handler != 0 )
    return HandleReceiveRequest ( handler );
  return NAME_SERVER_SUCCESS;
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
int AllocatorNameService::HandleAcceptRequest ( EventHandler* handler )  {
  int retry = 1, accept_error;                                       //
  NetworkChannel::Address address;                                   //
  NetworkChannel::Channel channel = _pNetwork->_Accept ( address );  // Accept
  accept_error = _pNetwork->_Error();                                //
  int status = _pNetwork->_QueueAccept ( _tcp->_Port(), handler );   // Rearm ACCEPT
  if ( !lib_rtl_is_success(status) )  {
    printf("HandleAcceptRequest> Accept Rearm FAILED %d RetryCount:%d %s",
      _pNetwork->_Error(),retry,                                   //
      _pNetwork->_ErrMsg());                                       //
  }                                                                //
  if ( channel <= 0 )   {                                          // Error!
    return NAME_SERVER_SUCCESS;                                    // Return status code
  }                                                                //
  else  {                                                          // Event handling:
    TanDataBase::Entry* entry = tandb.AllocateEntry(channel);      // Allocate database entry
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
int AllocatorNameService::HandleReceiveRequest ( EventHandler* handler )  {
  ReceiveHandler*     hand = (ReceiveHandler*)handler;
  TanDataBase::Entry*  ent = hand->_Entry();
  TcpNetworkChannel*  chan = hand->_Channel();
  int status = NAME_SERVER_SUCCESS, num_byte;

  num_byte = chan->_Recv( &ent->_Message(), sizeof(ent->_Message()));
  if ( num_byte <= 1 )  {       // Socket closed by Client
    tandb.Close ( ent );            // No need to return error
    //status = chan->_Error();      // condition in this case!
    chan->_UnqueueIO (_tcp->_Port());
  }
  else {
    TanMessage reply;
    HandleMessage( ent, ent->_Message(), reply);
    num_byte = chan->_Send(&reply, sizeof(reply));
    if ( num_byte <= 0 ) {
      tandb.Close ( ent );
      status = chan->_Error();
      chan->_UnqueueIO (_tcp->_Port());
    }
    else  {
      switch ( ntohl(reply._Function()) )    
      {
      case TanMessage::DEALLOCATE:
        // No task dead message! chan->_Recv(&reply, 1);
        chan->_UnqueueIO (_tcp->_Port());
        break;
      default:
        if ( ntohl(reply._Error()) == TAN_SS_SUCCESS )  {     // Only way to exit 
          status = chan->_QueueReceive (_tcp->_Port(), hand); // with success!
          if ( !lib_rtl_is_success(status) ) {
            printf("Error rearming receive: %s",chan->_ErrMsg());
          }
          return status;
        }
        tandb.Close ( ent );
        chan->_UnqueueIO (_tcp->_Port());
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
int AllocatorNameService::Suspend  ()   {
  delete connection;
  connection = 0;
  _tcp = 0;
  return NAME_SERVER_SUCCESS;
}

// ----------------------------------------------------------------------------
//                                      M.Frank
// ----------------------------------------------------------------------------
int AllocatorNameService::Resume ()   {
  int retry = 0;
New_allocation:
  if ( connection != 0 ) Suspend();
#ifdef SERVICE
  connection = _tcp = new TcpConnection(NAME_SERVICE_NAME);
#else
  connection = _tcp = new TcpConnection(NAME_SERVICE_PORT);
#endif
  _pNetwork = &((TcpNetworkChannel&)_tcp->_RecvChannel());
  if ( _pNetwork->_Error() != 0 && ++retry < 5 )  {
    printf("Resume-Retry# %d> %s\n", retry, _pNetwork->_ErrMsg());
    goto New_allocation;
  }
  return _pNetwork->_QueueAccept ( _tcp->_Port(), _pAcceptHandler );
}

extern "C" int tan_nameserver (int argc, char* argv[]) {
  // ----------------------------------------------------------------------------
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  char *c;
  NameService *srv;
  bool inquirer = false, allocator = false;
  while( --argc > 0 )      {
    if ( *(c = *++argv) == '-' )   {
      switch( *++c | 0x20 )  {
         case 'i':  inquirer = true;   break;
         case 'a':  allocator = true;   break;
         default:
Options:
           printf("NameServer -<opt>\n");
           printf("  -a             Listen and serve (DE)ALLOCATION requests\n");
           printf("  -i             Listen and serve INQUIRE        requests\n");
           return 0x1;
      }
    }
  }
  if ( (inquirer && allocator) || (!inquirer && !allocator) )
    goto Options;
  else if ( inquirer )   {
    srv = new InquireNameService();
  }
  else if ( allocator )   {
    if ( TanDataBase::Initialize() != TAN_SS_SUCCESS )  {
      printf("AllocatorNameService> Error initializing the DataBase!\n");
      return 1;
    }
    srv = new AllocatorNameService();
  }
  srv->Run();
  delete srv;
  return    0x1;
}
