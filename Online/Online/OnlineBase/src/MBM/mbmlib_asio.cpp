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
#include "MBM/bmstruct.h"
#include "MBM/bmserver.h"
#include "MBM/bmmessage.h"
#include "ASIO/Server.h"
#include "ASIO/TanInterface.h"

// C/C++ include files
#include <iostream>
#include <sstream>
#include <memory>

namespace BoostAsio  {

  /// The MBM server request handler for all incoming requests.
  /**
   *  Handle MBM messages/requests using TCP/IP as transport protocol
   *
   *  \author   M.Frank
   *  \version  1.0
   *  \defgroup LHCB_ONLINE
   */
  class MBMRequestHandler : public RequestHandler  {
  protected:
    ServerBMID m_bm;
  public:
    /// Construct request handler
    explicit MBMRequestHandler(ServerBMID bm) : m_bm(bm) {}
    /// Default destructor
    virtual ~MBMRequestHandler()  {}
    /// RequestHandler overload: Handle a request and produce a reply.
    virtual void handle(Connection& connection, const Request& request, Reply& reply);
    /// RequestHandler overload: Handle connection finalization for cleanups
    virtual void handleClose(Connection& connection);
  };
}

using namespace BoostAsio;
using namespace std;

/// Handle a request and produce a reply.
void MBMRequestHandler::handle(Connection& connection, const Request& req, Reply& response) {
  typedef  MBMMessage MSG;
  const MSG* const_req = reinterpret_cast<const MSG*>(boost::asio::detail::buffer_cast_helper(req));
  MSG* msg = const_cast<MSG*>(const_req);
  if ( msg->magic != MBMMessage::MAGIC ) {
    ::lib_rtl_output(LIB_RTL_ERROR,"Wrong magic word in MBM message:%X instead of %X.",
		     msg->magic,MBMMessage::MAGIC);
    return;
  }
  mbmsrv_handle_request(m_bm,&connection,*msg);
  if ( msg->status != MBM_NO_REPLY ) {
    response.push_back(boost::asio::buffer(connection.reply().data(), sizeof(MSG)));
  }
}

/// RequestHandler overload: Handle connection finalization for cleanups
void MBMRequestHandler::handleClose(Connection& connection)   {
  NetworkChannel::Channel fd = connection.socket().native();
  if ( fd > 0 )  {
    ::lib_rtl_output(LIB_RTL_ALWAYS,"++asio_bm_server++ Closing connection: %d\n",fd);
  }
}

namespace {
  int _close(MBMConnection& connection)  {
    connection.init();
    return MBM_NORMAL;
  }

  int _accept(MBMConnection& connection, const char* /* bm_name */, const char* /* name */)   {
    return connection.any.channel != 0 ? MBM_NORMAL : MBM_ERROR;
  }

  int _bind(MBMConnection& /* connection */, const char* /* bm_name */, int /* id */)   {
    return MBM_NORMAL;
  }

  int _poll_create(MBMConnection& /* connection */, int /* max_count */)   {
    return MBM_NORMAL;
  }

  int _poll_add(MBMConnection& /* connection */, MBMConnection& /* client */)   {
    return MBM_NORMAL;
  }

  int _poll_remove(MBMConnection& /* connection */, MBMConnection& /* client */)   {
    return MBM_NORMAL;
  }

  int _poll(MBMConnection& /* connection */, int& events, int tmo)  {
    if ( tmo > 0 ) ::lib_rtl_sleep(tmo);
    events = 0;
    return 0;
  }

  int _open(MBMConnection& /* connection */, const char* /* bm_name */, const char* /* name */)  {
    return MBM_NORMAL;
  }

  struct ServConn {
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket socket;
    ServConn() : io_service(), socket(io_service) {}
  };

  int _open_server_ex(MBMConnection& connection, const char* bm_name, const char* name, int id)  {
    TanMessage::Address addr;
    stringstream port;
    string server;
    int status;
    connection.any.channel = 0;
    port << "BM_" << bm_name << "_SERVER_" << id << ends;
    server = port.str();
    status = ::tan_get_address_by_name(server.c_str(), &addr);
    port.str("");
    if ( status == TAN_SS_SUCCESS )  {
      port << addr.sin_port << ends;
      try {
	using boost::asio::ip::tcp;
	auto_ptr<ServConn> conn(new ServConn());
	tcp::resolver resolver(conn->io_service);
	tcp::resolver::query query(tcp::v4(), "127.0.0.1", port.str());
	tcp::resolver::iterator iterator = resolver.resolve(query);
	boost::asio::connect(conn->socket, iterator);
	connection.any.channel = conn.release();
	return MBM_NORMAL;
      }
      catch(exception& e)   {
	::lib_rtl_output(LIB_RTL_ALWAYS,"MBM _open_server(%s, %s, %s) port:%s [%s]\n",
			 server.c_str(),bm_name,name,port.str().c_str(),e.what());
      }
    }
    return MBM_ERROR;
  }

  int _open_server(MBMConnection& connection, const char* bm_name, const char* name)  {
    return _open_server_ex(connection,bm_name,name,0);
  }

  int _write(MBMConnection& connection, MBMMessage& msg) {
    ServConn* c = reinterpret_cast<ServConn*>(connection.any.channel);
    boost::asio::write(c->socket, boost::asio::buffer(&msg, sizeof(msg)));
    return msg.status=MBM_NORMAL;
  }

  int _read(MBMConnection& connection, MBMMessage& msg, int* cancel_flag) {
    ServConn* c = reinterpret_cast<ServConn*>(connection.any.channel);
    size_t reply_length;
    struct pollfd fds;

    fds.events  = POLLIN;
    fds.fd = c->socket.native();
    while(1) {
      fds.revents = 0;
      ::poll(&fds,1,100);
      if ( fds.revents&POLLIN )
	break;
      else if ( cancel_flag && *cancel_flag )
	return MBM_REQ_CANCEL;
    }
    reply_length = boost::asio::read(c->socket,boost::asio::buffer(&msg,sizeof(msg)));
    if ( reply_length != sizeof(msg) )  {
      return msg.status=MBM_ERROR;
    }
    if ( msg.magic != MBMMessage::MAGIC ) {
      ::lib_rtl_output(LIB_RTL_ERROR,
		       "MBM: Wrong magic word in MBM message:%X instead of %X.\n",
		       msg.magic,MBMMessage::MAGIC);
      return msg.status=MBM_ERROR;
    }
    return msg.status=MBM_NORMAL;
  }

  /// Exchange MBM message with server
  int _communicate(MBMConnection& connection, MBMMessage& msg, int* cancelled)   {
    if ( MBM_NORMAL == _write(connection,msg) )
      return _read(connection,msg,cancelled);
    return msg.status;
  }

  /// Clear possibly pending data from communication channel
  int _clear(MBMConnection& /* connection */)   {
    return MBM_NORMAL;
  }

  /// Send request to server
  int _send_request(MBMConnection& connection, MBMMessage& msg, bool clear_before)   {
    if ( clear_before )
      _clear(connection);
    return msg.status=_write(connection,msg);
  }

  /// Read server response
  int _read_response(MBMConnection& connection, MBMMessage& msg, int* cancelled)  {
    return _read(connection, msg, cancelled);
  }

  int _move(MBMConnection& connection, const char* bm_name, USER* user_cookie, int serverid)   {
    if ( serverid != 0 )  {
      auto_ptr<ServConn> conn(reinterpret_cast<ServConn*>(connection.any.channel));
      int status = _open_server_ex(connection,bm_name,"reconnect",serverid);
      if ( status == MBM_NORMAL )  {
	MBMMessage msg(MBMMessage::RECONNECT,user_cookie);
	return _communicate(connection,msg,0);
      }
      return status;
    }
    return MBM_NORMAL;
  }

  int _send_response(MBMConnection& connection, const void* buff, size_t length)   {
    BoostAsio::Connection* c = reinterpret_cast<BoostAsio::Connection*>(connection.any.channel);
    if ( c )  {
      boost::asio::write(c->socket(), boost::asio::buffer((void*)buff, length));
      return MBM_NORMAL;
    }
    return MBM_ERROR;
  }

  int _dispatch(ServerBMID bm, int which)  {
    string srv_name;
    ::lib_rtl_output(LIB_RTL_ALWAYS,"BM: %s Starting dispatch thread %d\n",bm->bm_name,which);
    try  {
      NetworkChannel::Port port_number = -1;
      int status, num_threads = 2;
      stringstream port;
      port << "BM_" << bm->bm_name << "_SERVER_" << which << ends;
      srv_name = port.str();
      port.str("");
      status = ::tan_allocate_new_port_number(srv_name.c_str(), &port_number);
      port << port_number << ends;
      if ( status == TAN_SS_SUCCESS )  {
	Server server("127.0.0.1", port.str(), num_threads);
	server.config->synchronous = -1;
	server.config->setHandler(new MBMRequestHandler(bm));
	::lib_rtl_output(LIB_RTL_ALWAYS,"Dispatching boost::asio requests..%s..Port:%d\n",
			 srv_name.c_str(), int(port_number));
	// Run the server until stopped.
	server.run();
	::tan_deallocate_port_number(srv_name.c_str());
	return 1;
      }
      return ENOENT;
    }
    catch (const exception& e)  {
      ::tan_deallocate_port_number(srv_name.c_str());
      ::lib_rtl_output(LIB_RTL_ERROR,"mbmsrv_asio: exception: %s\n",e.what());
    }
    return 1;
  }
}

int _mbm_connections_use_asio(MBMCommunication& com)   {
  com.type          = BM_COM_ASIO;

  /** MBM Server communication functions   */
  com.accept        = _accept;
  com.bind          = _bind;
  com.open          = _open;
  com.close         = _close;
  com.poll          = _poll;
  com.poll_del      = _poll_remove;
  com.poll_add      = _poll_add;
  com.poll_create   = _poll_create;
  com.send_response = _send_response;
  com.dispatch      = _dispatch;

  /** MBM Client communication functions   */
  com.open_server   = _open_server;
  com.move_server   = _move;
  com.communicate   = _communicate;
  com.clear         = _clear;
  com.send_request  = _send_request;
  com.read_response = _read_response;

  return MBM_NORMAL;
}
