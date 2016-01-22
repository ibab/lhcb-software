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

// Framework include files
#include "TAN/TanDB.h"
#include "ASIO/Server.h"

// C/C++ include files
#include <set>
#include <iostream>

namespace BoostAsio  {

  /// The TAN nameserver request handler for all incoming requests.
  /**
   *  Handle TAN messages in TCP/IP mode
   *  A network connection is forseen to reply the 
   *  requested information.                       
   *
   *  \author   M.Frank
   *  \version  1.0
   *  \defgroup LHCB_ONLINE
   */
  class TanRequestHandler : public RequestHandler  {
    typedef TanMessage Msg;
    typedef TanDataBase::Entry Entry;

  protected:
    /// Reference to tan database object
    TanDataBase&         m_tandb;
    /// Set of file descriptors requiring cleanup procedure
    std::set<int>        m_cleanup;

  public:
    /// Construct request handler
    explicit TanRequestHandler(bool verbose, NetworkChannel::Port port);
    /// Default destructor
    virtual ~TanRequestHandler();
    /// RequestHandler overload: Handle a request and produce a reply.
    virtual void handle(Connection& connection, 
			const Request& request,
			Reply& reply);

    /// RequestHandler overload: Handle connection finalization for cleanups
    virtual void handleClose(Connection& connection);

    /// Callback to allocate a port number from the service
    void onAllocate(Connection& connection,
		    const Msg& request,
		    Msg& reply );

    /// Callback to deallocate a port number from the service
    void onDeallocate(Connection& connection,
		      const Msg& request,
		      Msg& reply );

    /// Callback to alias a port number given the allocating name
    void onAlias(Connection& connection,
		 const Msg& request,
		 Msg& reply );

    /// Callback to de-alias a port
    void onDealias(Connection& connection,
		   const Msg& request,
		   Msg& reply );

    /// Inquire a port number given the client name
    void onInquire(Connection& connection,
		   const Msg& request,
		   Msg& reply );

    /// Handle shutdown request from client
    void onShutdown(Connection& connection,
		    const Msg& request,
		    Msg& reply );
  };
}

#include <sstream>
using namespace BoostAsio;
using namespace std;

static void _printEntry(const char* msg, const TanMessage& request, TanDataBase::Entry* ent) {
  if ( msg ) {
    if ( ent && &request ) {
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

/// Construct request handler
TanRequestHandler::TanRequestHandler(bool verbose, NetworkChannel::Port port)
  : m_tandb(TanDataBase::Instance())
{
  if ( verbose )  {
    std::string tm_str = ::lib_rtl_timestr();
    ::lib_rtl_output(LIB_RTL_INFO,"+======================================================================+\n");
    ::lib_rtl_output(LIB_RTL_INFO,"|         N A M E S E R V E R      S T A R T I N G                     |\n");
    ::lib_rtl_output(LIB_RTL_INFO,"|         %32s                             |\n",tm_str.c_str());
    ::lib_rtl_output(LIB_RTL_INFO,"+======================================================================+\n");
    ::lib_rtl_output(LIB_RTL_INFO,"|         B O O S T   A S I O    T C P / I P   N A M E   S E R V I C E |\n");
    ::lib_rtl_output(LIB_RTL_INFO,"|         Port(local): %6d %04X Network:%6d %04X                 |\n",
                     port, port, ntohs(port), ntohs(port));
    ::lib_rtl_output(LIB_RTL_INFO,"+======================================================================+\n");
    ::fflush(stdout);
  }
}

/// Default destructor
TanRequestHandler::~TanRequestHandler()   {
}

/// Handle a request and produce a reply.
void TanRequestHandler::handle(Connection& connection,
			       const Request& req,
			       Reply& response)
{
  const void* data   = boost::asio::detail::buffer_cast_helper(req);
  const Msg* request = reinterpret_cast<const Msg*>(data);
  Msg::Type func = Msg::Type(request->function());
  Msg reply = *request;

  reply.m_error  = TAN_SS_SUCCESS;
  reply.m_length = sizeof(reply);
  switch ( func )  {
  case Msg::ALLOCATE:
    onAllocate(connection,*request,reply);
    break;
  case Msg::DEALLOCATE:
    onDeallocate(connection,*request,reply);
    break;
  case Msg::ALIAS:
    onAlias(connection,*request,reply);
    break;
  case Msg::DEALIAS:
    onDealias(connection,*request,reply);
    break;
  case Msg::INQUIRE:
    onInquire(connection,*request,reply);
    break;
  case Msg::SHUTDOWN:
    onShutdown(connection,*request,reply);
    break;
  case Msg::DUMP:
    _printEntry("DUMP",*request,0);
    m_tandb.Dump(std::cout);
    break;
  default:
    _printEntry("TAN_SS_UNKNOWNMODE",*request,0);
    reply.m_error = TAN_SS_UNKNOWNMODE;
    reply.m_sin.sin_family      = 0;
    reply.m_sin.sin_port        = 0;
    reply.m_sin.sin_addr.s_addr = 0;
    break;
  }
  if ( connection.reply().size() < sizeof(Msg) )  {
    connection.reply().resize(sizeof(Msg));
  }
  ::memcpy(connection.reply().data(),&reply,sizeof(Msg));
  response.push_back(boost::asio::buffer(connection.reply().data(), sizeof(Msg)));
}

/// Handle connection finalization for cleanups
void TanRequestHandler::handleClose(Connection& connection)   {
  NetworkChannel::Channel fd = connection.socket().native();
  set<int>::iterator i = m_cleanup.find(fd);
  if ( i != m_cleanup.end() )  {
    m_cleanup.erase(i);
    Entry* ent = m_tandb.FindEntry(fd);
    while(ent)   {
      m_tandb.freePort(ent);
      ent = m_tandb.FindEntry(fd);
    }
  }
}

/// Callback to allocate a port number from the service
void TanRequestHandler::onAllocate(Connection& connection,
				   const Msg& request,
				   Msg& reply )
{
  NetworkChannel::Channel fd = connection.socket().native();
  Entry* ent = m_tandb.AllocateEntry(fd);
  m_cleanup.insert(fd);
  m_cleanup.insert(connection.socket().native());
  ent->_Message() = request;
  reply.m_sin.sin_family  = request.m_sin.sin_family;
  if ( (reply.m_sin.sin_port=htons(m_tandb.allocatePort(ent))) == 0 )  {
    reply.m_error = m_tandb.Error();
    reply.m_sin.sin_family      = 0;
    reply.m_sin.sin_addr.s_addr = 0;
  }
}

/// Callback to deallocate a port number from the service
void TanRequestHandler::onDeallocate(Connection& connection,
				     const Msg& request,
				     Msg& reply )
{
  Entry* ent = m_tandb.FindEntry(request._Name());
  reply.m_error               = TAN_SS_SUCCESS;
  reply.m_sin.sin_port        = 0;
  reply.m_sin.sin_family      = 0;
  reply.m_sin.sin_addr.s_addr = 0;
  m_cleanup.insert(connection.socket().native());
  if ( ent )  {
    ent->_Message() = request;
    _printEntry("DEALLOCATE",request,ent);
    if ( TAN_SS_SUCCESS != m_tandb.freePort(ent) ) 
      reply.m_error = m_tandb.Error();
    return;
  }
  reply.m_error = m_tandb.Error();
}

/// Callback to alias a port number given the allocating name
void TanRequestHandler::onAlias(Connection& connection,
				const Msg& request,
				Msg& reply )
{
  NetworkChannel::Channel fd = connection.socket().native();
  Entry* ent = m_tandb.FindEntry(fd);
  reply.m_error = TAN_SS_SUCCESS;
  m_cleanup.insert(connection.socket().native());
  if ( ent )  {
    ent->_Message() = request;
    _printEntry("DEALLOCATE",request,ent);
    if ( TAN_SS_SUCCESS != m_tandb.insertAlias(ent) )
      reply.m_error = m_tandb.Error();
    return;
  }
  reply.m_error = m_tandb.Error();
}

/// Callback to alias a port number given the allocating name
void TanRequestHandler::onDealias(Connection& connection,
				  const Msg& request,
				  Msg& reply )
{
  NetworkChannel::Channel fd = connection.socket().native();
  Entry* ent = m_tandb.FindEntry(fd);
  reply.m_error = TAN_SS_SUCCESS;
  reply.m_sin.sin_port        = 0;
  reply.m_sin.sin_family      = 0;
  reply.m_sin.sin_addr.s_addr = 0;
  m_cleanup.insert(connection.socket().native());
  if ( ent )  {
    ent->_Message() = request;
    _printEntry("DEALIAS",request,ent);
    if ( TAN_SS_SUCCESS != m_tandb.removeAlias(ent) )
      reply.m_error = m_tandb.Error();
    return;
  }
  reply.m_error = m_tandb.Error();
}

/// Inquire a port number given the client name
void TanRequestHandler::onInquire(Connection& /* connection */,
				  const Msg& request,
				  Msg& reply )
{
  reply.m_error        = TAN_SS_SUCCESS;
  reply.m_sin.sin_port = 0;
  _printEntry("INQUIRE",request,0);
  if ( (reply.m_sin.sin_port=htons(m_tandb.findPort(request))) == 0 )  {
    ::lib_rtl_output(LIB_RTL_ALWAYS,"TanMessage::INQUIRE: FAILED [%s]\n",request._Name());
    reply.m_error = m_tandb.Error();
  }
}

/// Handle shutdown request from client
void TanRequestHandler::onShutdown(Connection& /* connection */,
				   const Msg& request,
				   Msg& reply )
{
  _printEntry("SHUTDOWN",request,0);
  ::lib_rtl_output(LIB_RTL_ALWAYS,"+======================================================================+\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"|                  TAN NAMESERVER SHUTDOWN requested.                  |\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"+======================================================================+\n");
  reply.m_sin.sin_family      = 0;
  reply.m_sin.sin_port        = 0;
  reply.m_sin.sin_addr.s_addr = 0;
  ::kill(::lib_rtl_pid(),SIGTERM);
}

static void help()  {
  ::printf("Usage: boost_asio_tan_server -opt [-opt]\n"
	   "   -address=<address>   Listening address. Default: 0.0.0.0 (IN_ADDR_ANY)  \n"
	   "                        For ipv6 use: -address=0::0                        \n"
	   "   -port=<port>         Listeing port.     Default: %d (NAME_SERVICE_PORT) \n"
	   "   -verbose             Increase printout level                            \n"
#ifdef __linux
	   "   -delete              Unconditionally delete existing GBL (recommended)  \n"
#endif
	   "   -threads=<number>    Number of worker threads. Default: 1               \n",
	   NAME_SERVICE_PORT);
  ::_exit(0);
}

extern "C" int boost_asio_tan_server(int argc, char** argv)  {
  try  {
    // Check command line arguments.
    stringstream port;
    RTL::CLI cli(argc,argv,help);
    int num_threads = 1, port_number = NAME_SERVICE_PORT;
    string address = "0.0.0.0"; // Default: INADDR_ANY
    bool verbose = cli.getopt("verbose",1);
    bool del_gbl = cli.getopt("delete",1);
    cli.getopt("address", 1, address);
    cli.getopt("port",    1, port_number);
    cli.getopt("threads", 1, num_threads);
    port << port_number << ends;

    if ( del_gbl )   {
#ifdef __linux
      ::unlink("/dev/shm/sem.TANDB");
      ::unlink("/dev/shm/sem.TAN_PUBAREA_lock");
      ::unlink("/dev/shm/TAN_PUBAREA");
#endif
    }
    if ( TanDataBase::initialize() != TAN_SS_SUCCESS )  {
      lib_rtl_output(LIB_RTL_ERROR,"ZmqNameService> Error initializing the DataBase!\n");
      return 0x0;
    }
    
    Server server(address, port.str(), num_threads);
    server.config->handler.reset(new TanRequestHandler(verbose, port_number));
    server.handleSignals();
    server.setRecvSize(sizeof(TanMessage));
    // Run the server until stopped.
    server.run();
  }
  catch (const exception& e)  {
    ::lib_rtl_output(LIB_RTL_OS,"Boost::asio: exception: %s\n",e.what());
  }
  return 0;
}
