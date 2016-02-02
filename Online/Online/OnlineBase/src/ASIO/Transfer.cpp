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
#define DATATRANSFER_INTERNALS

#ifndef HAVE_ASIO_IPC
#define TRANSFER_NS BoostAsio
namespace BoostAsio  {
  typedef class TcpServer           TransferServer;
  typedef class TcpConnection       TransferConnection;
  typedef class AsyncTcpConnection  AsyncTransferConnection;
  typedef class TcpRequestHandler   TransferRequestHandler;
  typedef class TcpServerConfig     TransferServerConfig;
}
#endif
#include "NET/Transfer.h"
#include <string>
#include <thread>
#include <mutex>
#include "boost/asio.hpp"
#include "RTL/rtl.h"

#ifdef HAVE_ASIO_IPC
#include "ASIO/UnixServer.h"
#else
#include "ASIO/TcpServer.h"
#endif

namespace TRANSFER_NS  {
  class NetConnection;

  struct netentry_t {
    NetConnection* connection;
    std::string    name;
    unsigned long  hash;
    sockaddr_in    addr;
    NET*           sys;
    netheader_t    header;
    void*          data;
    netentry_t(NetConnection* c) : connection(c), sys(0), data(0)  {}
    ~netentry_t() {}
    NetErrorCode close();
  };

  struct  NET  {
    struct Client {
      net_handler_t data;
      net_handler_t death;
      void*         param;
      unsigned int  fac;
    };
    typedef std::lock_guard<std::mutex> netlock_t;
    typedef std::vector<Client> Clients;
    typedef std::map<unsigned long,netentry_t*> netdb_t;
    explicit NET(const std::string& proc);
    ~NET();
    struct {
      std::string   name;
      unsigned long hash;
      sockaddr_in   addr;
      NET*          sys;
      netheader_t   header;
    } m_me;
    NetConnectionType        m_type;
    netdb_t                  m_db;
    std::mutex               m_lock;
    Clients                  m_clients;
    TransferServer*          m_server;
    std::thread*             m_mgr;
    int                      m_refCount;
    int                      m_cancelled;
    int                      m_stop;
    int                      m_async_read;

    void setSockOpts(TransferConnection::socket_t& chan);
    NetConnectionType type() const { return m_type; }
    netentry_t* connect(const std::string& dest);
    netentry_t* accept(NetConnection* connection, const netheader_t& header);
    NetErrorCode remove(netentry_t *e);
    void cancel();
    void sendShutdown(netentry_t *e);
    NetErrorCode disconnect(netentry_t *e);
    NetErrorCode init(NetConnectionType type, int nthreads);
    NetErrorCode handleMessage(netentry_t* entry, unsigned char* data);
    NetErrorCode receive(netentry_t* entry, void* data);
    NetErrorCode send(const void* buff, size_t size, const std::string& dest, int fac);
    NetErrorCode subscribe(void* param, unsigned int fac, net_handler_t data, net_handler_t death);
    NetErrorCode unsubscribe(void* param, unsigned int fac);
    unsigned int release();
    void         handle_close(netentry_t* entry);
    netentry_t*  handle_data(NetConnection* conn, const netheader_t* header, unsigned char* data);
    static NET*  instance(const std::string& proc, int nthreads, NetConnectionType type);
  };
}

#include <cstdio>
#include <cerrno>
#include <memory>
#include <vector>
#include <map>
#include <stdexcept>
#include "RTL/rtl.h"
#include "NET/defs.h"
#include "ASIO/TanInterface.h"
//#include "TAN/TanInterface.h"
#include <boost/bind.hpp>

namespace TRANSFER_NS  {

  static const int RECV_BUFFER_SIZE = 100*1024;
  static const int SEND_BUFFER_SIZE = 100*1024;

  /// Network connection class using Boost::asio
  class NetConnection : public AsyncTransferConnection  {
    NET* net;
  public:
    netentry_t* entry;
    std::string name;
    /// Construct a connection with the given io_service.
    explicit NetConnection(boost::asio::io_service& io_service,
			   TransferRequestHandler::pointer_t& handler,
			   NET* net)
      : AsyncTransferConnection(io_service, handler, sizeof(netheader_t)), net(net), entry(0)
    {
      //::lib_rtl_output(LIB_RTL_DEBUG,"Transfer(ASIO): Establish new connection  %p\n",this);
    }
    /// Default destructor
    virtual ~NetConnection()  {
      //::lib_rtl_output(LIB_RTL_DEBUG,"Transfer(ASIO): Remove connection  %p [%s]\n", this, name.c_str());
      if ( entry ) net->handle_close(entry);
      entry = 0;
    }
    /// Handle completion of a asynchonous unlocked read/write operation.
    virtual void handle_read(const error_t& error, size_t bytes_transferred)  {
      if ( !error )  {
	if ( bytes_transferred > 0 )    {
	  unsigned char* data = const_cast<unsigned char*>(m_recv_buffer.data());
	  netheader_t*   head = reinterpret_cast<netheader_t*>(data);
	  if ( bytes_transferred < sizeof(netheader_t) )  {
	    error_t ec;
	    size_t len = boost::asio::read(socket(),
					   boost::asio::buffer(data+bytes_transferred,
							       sizeof(netheader_t)-bytes_transferred),
					   ec);
	    if ( len != sizeof(netheader_t)-bytes_transferred )  {
	    }
	  }
	  if ( net->m_async_read && head->size > 0 )  {
	    if ( m_reply_buffer.size() < head->size )  {
	      m_reply_buffer.resize(head->size);
	    }
	    m_socket.async_read_some(boost::asio::buffer(m_reply_buffer.data(),head->size),
				     boost::bind(&TransferConnection::handle_additional_read,
						 shared_from_this(),
						 boost::asio::placeholders::error,
						 boost::asio::placeholders::bytes_transferred));
	    return;
	  }
	  else   {
	    net->handle_data(this, head, 0);
	  }
	}
	m_socket.async_read_some(boost::asio::buffer(m_recv_buffer.data(),m_recvSize),
				 boost::bind(&TransferConnection::handle_read,
					     shared_from_this(),
					     boost::asio::placeholders::error,
					     boost::asio::placeholders::bytes_transferred));
      }
    }
    /// Handle completion of a asynchonous unlocked read/write operation.
    virtual void handle_additional_read(const error_t& error, size_t bytes_transferred)  {
      if ( !error )  {
	if ( bytes_transferred > 0 )    {
	  unsigned char* data = const_cast<unsigned char*>(m_recv_buffer.data());
	  netheader_t*   head = reinterpret_cast<netheader_t*>(data);
	  data = m_reply_buffer.data();
	  if ( bytes_transferred < head->size )  {
	    error_t ec;
	    size_t len = boost::asio::read(socket(),
					   boost::asio::buffer(data+bytes_transferred,
							       head->size-bytes_transferred),
					   ec);
	    if ( len != head->size-bytes_transferred )  {
	    }
	  }
	  net->handle_data(this, head, data);
	}
	m_socket.async_read_some(boost::asio::buffer(m_recv_buffer.data(),m_recvSize),
				 boost::bind(&TransferConnection::handle_read,
					     shared_from_this(),
					     boost::asio::placeholders::error,
					     boost::asio::placeholders::bytes_transferred));
      }
    }
  };

  class NetServerConfig : public TransferServerConfig  {
    NET* net;
  public:
    NetServerConfig(TransferServer* s, NET* n) : TransferServerConfig(s), net(n) {}
    virtual ~NetServerConfig() {}
    /// Create a new blank server connection
    virtual TransferConnection* newConnection()   {
      return new NetConnection(io_service(), handler, net);
    }
  };
}


using namespace std;
using namespace boost;
using namespace TRANSFER_NS;
using boost::asio::ip::tcp;
namespace boost  {
  namespace asio  {
    typedef boost::system::error_code error_code;
  }
}
namespace {
  /// one-at-time hash function
  unsigned int hash32(const char* key) {
    unsigned int hash;
    const char* k;
    for (hash = 0, k = key; *k; k++) {
      hash += *k; hash += (hash << 10); hash ^= (hash >> 6); 
    }
    hash += (hash << 3); hash ^= (hash >> 11); hash += (hash << 15);
    return hash;
  }

  inline pair<NET*,mutex>& locked_instance()  {
    static pair<NET*,mutex> inst;
    return inst;
  }

  //#define _net_printf printf
  inline void _net_printf(const char*, ...) {}
}

//----------------------------------------------------------------------------------
NetErrorCode netentry_t::close()  {
  //asio::error_code ec;
  //TransferConnection::socket_t& chan = connection->socket();
  connection->entry = 0;
  //if ( chan.is_open() )  {
    //chan.set_option(asio::socket_base::linger(true,0));
    //chan.shutdown(TransferConnection::socket_t::shutdown_both,ec);
    //chan.close(ec);
  //}
  return NET_SUCCESS;
}

//----------------------------------------------------------------------------------
NET::NET(const string& proc)
  : m_type(NET_SERVER), m_lock(), m_server(0), m_mgr(0), 
    m_refCount(0), m_cancelled(0),
    m_stop(0), m_async_read(0)
{
  m_me.sys = this;
  m_me.name = proc;
  m_me.hash = hash32(m_me.name.c_str());
  //::memset(&m_me.header,0,sizeof(m_me.header));
  m_me.header.hash = m_me.hash;
  ::strncpy(m_me.header.name,proc.c_str(),sizeof(m_me.header.name)-1);
  m_me.header.name[sizeof(m_me.header.name)-1] = 0;
  m_me.addr.sin_port = 0;
  m_me.addr.sin_family = AF_INET;
  m_me.addr.sin_addr.s_addr = INADDR_ANY; //IN_CLASSA_HOST; // 
}

//----------------------------------------------------------------------------------
NET::~NET() {
  m_stop = 1;
  {
    netlock_t lock(m_lock);
    for (netdb_t::iterator i=m_db.begin(); i!=m_db.end();++i)  {
      netentry_t* e = (*i).second;
      if ( e->connection )  {
	TransferConnection::socket_t& chan = e->connection->socket();
	e->connection->entry = 0;
	if ( chan.is_open() )    {
	  try {
	    asio::write(chan, asio::buffer(&m_me.header,sizeof(netheader_t)));
	  }
	  catch(const std::exception& e)  {
	  }
	}
      }
    }
  }
  if ( m_server )   {
    m_server->handleStop();
  }
  if ( m_mgr )   {
    m_mgr->join();
    delete m_mgr;
    m_mgr = 0;
  }
  if ( m_server )   {
    delete m_server;
    m_server = 0;
  }
  m_clients.clear();
  {
    netlock_t lock(m_lock);
    for (netdb_t::iterator i=m_db.begin(); i!=m_db.end();++i)  {
      netentry_t* e = (*i).second;
      (*i).second = 0;
      delete e;
    }
    m_db.clear();
  }
  if ( m_me.addr.sin_port )  {
    ::tan_deallocate_port_number(m_me.name.c_str());
    m_me.addr.sin_addr.s_addr = 0;
    m_me.addr.sin_port = 0;
  }
  m_me.header.size = 0;
  m_me.header.facility = 0;
  m_me.header.msg_type = NET_CONNCLOSED;
}

//----------------------------------------------------------------------------------
NET* NET::instance(const string& proc, int nthreads, NetConnectionType type)  {
  pair<NET*,mutex>& inst = locked_instance();
  netlock_t  instance_lock(inst.second);
  NET*& ptr = inst.first;
  if ( 0 == ptr )  {
    auto_ptr<NET> conn(new NET(proc));
    NetErrorCode status = conn->init(type,nthreads);
    if ( status != NET_SUCCESS )  {
      throw runtime_error("Transfer: Cannot initialize network worker.");
    }
    ptr = conn.release();
  }
  else if ( ptr->type() == NET_CLIENT && type == NET_SERVER )  {
    NetErrorCode status = ptr->init(type,nthreads);
    if ( status != NET_SUCCESS )  {
      throw runtime_error("Transfer: Cannot initialize network server.");
    }
  }
  ptr->m_refCount++;
  return ptr;
}

//----------------------------------------------------------------------------------
unsigned int NET::release()  {
  pair<NET*,mutex>& inst = locked_instance();
  netlock_t  instance_lock(inst.second);
  if ( inst.first )  {
    unsigned int cnt = --inst.first->m_refCount;
    if ( cnt == 0 )  {
      delete inst.first;
      inst.first = 0;
      return cnt;
    }
    return cnt;
  }
  return 0;
}

//----------------------------------------------------------------------------------
NetErrorCode NET::init(NetConnectionType type, int nthreads)  {
  // Only initialize if needed!
  if ( m_me.addr.sin_port == 0 )  {
    m_type = type;
    if ( type == NET_SERVER )  {
      char port[1024];
      try {
#ifdef HAVE_ASIO_IPC
	::snprintf(port,sizeof(port),"%s",m_me.name.c_str());
	std::remove(port);
	auto_ptr<TransferServer> srv(new TransferServer(m_me.name, nthreads));
#else
	char conn[64];
	int status = ::tan_allocate_port_number(m_me.name.c_str(),&m_me.addr.sin_port);
	if ( status != TAN_SS_SUCCESS )  {
	  ::lib_rtl_signal_message(LIB_RTL_OS,"Transfer(ASIO): Failed allocating port number. Status %d\n",status);
	  return NET_ERROR;
	}
	::snprintf(conn,sizeof(conn),"0.0.0.0");
	::snprintf(port,sizeof(port),"%d",htons(m_me.addr.sin_port));
	auto_ptr<TransferServer> srv(new TransferServer(conn,port,nthreads));
#endif
	srv->config = TransferServer::config_t(new NetServerConfig(srv.get(),this));
	srv->config->setHandler(0);
	srv->config->recvSize = sizeof(netheader_t);
	srv->start();
	m_server = srv.release();
	m_mgr = new thread([this]{ this->m_server->join(); });
      }
      catch(const std::exception& e)  {
	::lib_rtl_output(LIB_RTL_OS,"Transfer(ASIO): Failed to start NET "
			 "message pump. [%s] Port:%s\n",e.what(), port);
        return NET_ERROR;
      }
      return NET_SUCCESS;
    }
    else  {
      m_mgr = 0;
    }
  }
  return NET_SUCCESS;
}

//----------------------------------------------------------------------------------
void NET::cancel()   {
  m_cancelled = 1;
  if ( m_server )  {
    //::lib_rtl_output(LIB_RTL_ERROR,"Transfer(ASIO): Stop server.....\n");
    //m_server->handleStop();
  }
}

//----------------------------------------------------------------------------------
NetErrorCode NET::remove(netentry_t *e)    {
  netdb_t::iterator i=m_db.find(e->hash);
  if ( i != m_db.end() )  {
    m_db.erase(i);
    return NET_SUCCESS;
  }
  return NET_TASKNOTFOUND;
}

//----------------------------------------------------------------------------------
void NET::setSockOpts(TransferConnection::socket_t& chan)   {
  if ( chan.is_open() )  {
#ifdef HAVE_ASIO_IPC
    //chan.set_option(asio::socket_base::send_buffer_size(SEND_BUFFER_SIZE));
#else
    chan.set_option(asio::socket_base::reuse_address(true));
    chan.set_option(asio::socket_base::linger(true,0));
    //chan.set_option(asio::socket_base::receive_buffer_size(RECV_BUFFER_SIZE));
    //chan.set_option(asio::socket_base::send_buffer_size(SEND_BUFFER_SIZE));
#endif
  }
}

//----------------------------------------------------------------------------------
netentry_t* NET::accept(NetConnection* connection, const netheader_t& header)   {
  netdb_t::iterator i=m_db.find(header.hash);
  netentry_t* e = 0;
  if ( i == m_db.end() )   {
    e = new netentry_t(connection);
#ifndef HAVE_ASIO_IPC
    int sc = ::tan_get_address_by_name(header.name,&e->addr);
    if ( sc != TAN_SS_SUCCESS )  {
      ::lib_rtl_output(LIB_RTL_ERROR,
		       "Transfer(ASIO): Retrieved unsolicited message from %s.",
		       header.name);
      return 0;
    }
#endif
    setSockOpts(connection->socket());
    e->sys  = this;
    e->name = header.name;
    e->hash = header.hash;
    e->header = header;
    netlock_t lock(m_lock);
    m_db[e->hash] = e;
    return e;
  }
  else   {
    e = (*i).second;
    if ( e->connection != connection )  {
      connection->entry = e;
      connection->name = e->name;
      e->connection->entry = 0;
      e->connection = connection;
      setSockOpts(e->connection->socket());
    }
  }
  return (*i).second;
}

//----------------------------------------------------------------------------------
netentry_t *NET::connect(const string& dest)  {
  sockaddr_in   addr;
#ifdef HAVE_ASIO_IPC
  int sc = TAN_SS_SUCCESS;
#else
  int sc = ::tan_get_address_by_name(dest.c_str(),&addr);
#endif
  if ( sc == TAN_SS_SUCCESS )  {
    try  {
      int retry = 4;
      TransferConnection::pointer_t c(m_server
				      ->config
				      ->newConnection());
      NetConnection* conn = (NetConnection*)c.get();
      TransferConnection::socket_t&   chan = c->socket();
      for(retry=4; retry > 0; --retry )  {
	try  {
#ifdef HAVE_ASIO_IPC
	  conn->connect(dest);
#else
	  conn->connect(inet_ntoa(addr.sin_addr), addr.sin_port);
#endif
	  break;
	}
	catch(const std::exception& e)  {
	  ::lib_rtl_sleep(1000);
	}
      }
      if ( retry <= 0 )   {
	::lib_rtl_output(LIB_RTL_OS,"Transfer(ASIO): Failed to connect to %s.",dest.c_str());
	return 0;
      }
      setSockOpts(chan);
      auto_ptr<netentry_t> entry(new netentry_t(conn));
      entry->sys  = this;
      entry->name = dest;
      entry->addr = addr;
      entry->hash = hash32(entry->name.c_str());
      entry->header.hash = entry->hash;
      entry->header.size = 0;
      entry->header.facility = 0;
      ::strncpy(entry->header.name,entry->name.c_str(),sizeof(entry->header.name)-1);
      entry->header.name[sizeof(entry->header.name)-1] = 0;
      conn->name = entry->name;
      conn->start();
      netheader_t h = m_me.header;
      h.size = 0;
      h.facility = 0;
      h.msg_type = NET_MSG_HELLO;
      size_t len = asio::write(chan, asio::buffer(&h,sizeof(netheader_t)));
      if ( len == sizeof(netheader_t) )  {
	m_db[entry->hash] = entry.get();
	return entry.release();
      }
    }
    catch(const std::exception& e)  {
      ::lib_rtl_output(LIB_RTL_OS,
		       "Transfer(ASIO): Failed to connect to destination: %s. [%s]\n",
		       dest.c_str(), e.what());
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------
NetErrorCode NET::disconnect(netentry_t *e)   {
  auto_ptr<netentry_t> entry(e);
  entry->close();
  remove(entry.get());
  return NET_SUCCESS;
}

//----------------------------------------------------------------------------------
void NET::sendShutdown(netentry_t *entry)  {
  try {
    entry->header.size = 0;
    entry->header.facility = 0;
    entry->header.msg_type = NET_TASKDIED;
    for(Clients::iterator i=m_clients.begin(); i != m_clients.end(); ++i)
      if ( (*i).death ) (*i).death(entry->header, (*i).param, entry);
  }
  catch(const std::exception& e)  {
    ::lib_rtl_output(LIB_RTL_ERROR,
		     "Transfer(ASIO): Exception during disconnect handling: %s\n", 
		     e.what());
  }
  catch(...)  {
    ::lib_rtl_output(LIB_RTL_ERROR,
		     "Transfer(ASIO): Exception during disconnect handling.\n");
  }
  disconnect(entry);
}

//----------------------------------------------------------------------------------
netentry_t* NET::handle_data(NetConnection* connection, const netheader_t* header, unsigned char* data)   {
  netentry_t* entry;
  netdb_t::iterator i;
  switch(header->msg_type)  {
  case NET_MSG_DATA:
    entry = accept(connection,*header);
    if ( entry )  {
      entry->header.size = header->size;
      entry->header.facility = header->facility;
      entry->header.msg_type = header->msg_type;
      connection->name  = header->name;
      connection->entry = entry;
      handleMessage(entry, data);
    }
    return entry;
  case NET_CONNCLOSED:
    i = m_db.find(header->hash);
    if ( i != m_db.end() )  {
      sendShutdown((*i).second);
    }
    connection->entry = 0;
    return 0;
  case NET_MSG_HELLO:
    entry = accept(connection,*header);
    connection->name  = header->name;
    connection->entry = entry; // connection.userParam((void*)entry->hash);
    return entry;
  case NET_MSG_TERMINATE:
    m_stop = 1;
    return 0;
  default:
    return 0;
  }
}

//----------------------------------------------------------------------------------
NetErrorCode NET::handleMessage(netentry_t* entry, unsigned char* ptr) {
  entry->data = ptr;
  for(Clients::iterator i=m_clients.begin(); i != m_clients.end(); ++i)  {
    Client& c = (*i);
    if ( c.data && c.fac == entry->header.facility ) {
      c.data(entry->header, c.param, entry);
    }
  }
  return NET_SUCCESS;
}

//----------------------------------------------------------------------------------
void NET::handle_close(netentry_t* entry)   {
  if ( entry )  {
    netlock_t lock(m_lock);
    ::lib_rtl_output(LIB_RTL_DEBUG,"Transfer(ASIO): Closing connection to: %s\n",
		     entry->name.c_str());
    disconnect(entry);
  }
}

//----------------------------------------------------------------------------------
NetErrorCode NET::send(const void* buff, size_t size, const string& dest, int facility)  {
  if (size > 0)  {
    unsigned int hash = hash32(dest.c_str());
    netentry_t *entry = 0;
    try {
      netheader_t h;   {
	netlock_t lock(m_lock);
	netdb_t::iterator i=m_db.find(hash);
	entry = i != m_db.end() ? (*i).second : 0;
	if ( !entry ) {
	  if ( !(entry=connect(dest)) ) {
	    return NET_TASKNOTFOUND;
	  }
	}
      }
      h = m_me.header;
      h.size = size;
      h.facility = facility;
      h.msg_type = NET_MSG_DATA;
      vector<asio::const_buffer> msg;
      TransferConnection::socket_t& chan = entry->connection->socket();
      msg.push_back(asio::buffer(&h,sizeof(netheader_t)));
      msg.push_back(asio::buffer(buff,size));
      size_t nbytes = asio::write(chan, msg);
      if ( nbytes == sizeof(h) + size )  {
	return NET_SUCCESS;
      }
    }
    catch(const std::exception& e)  {
      ::lib_rtl_output(LIB_RTL_OS,
		       "Transfer(ASIO): Failed to send message to %s. [%s]\n",
		       entry->name.c_str(), e.what());
    }
    disconnect(entry);
    return NET_CONNCLOSED;
  }
  return NET_ILLEGAL_LENGTH;
}

//----------------------------------------------------------------------------------
NetErrorCode NET::receive(netentry_t* entry, void* buffer)  {
  if ( !entry->data && entry->header.size > 0 )  {
    try  {
      const netheader_t& header = entry->header;
      TransferConnection::socket_t& chan   = entry->connection->socket();
      if ( chan.is_open() )  {
	asio::error_code ec;
	if ( buffer )  {
	  size_t len = asio::read(chan,asio::buffer(buffer, header.size), ec);
	  return (len == header.size) ? NET_SUCCESS : NET_ERROR;
	}
	vector<unsigned char> buff;
	buff.resize(header.size);
	// asio::read is used to read a certain number of bytes of data from a stream. 
	// The call will block until one of the following conditions is true:
	// -- The supplied buffers are full. That is, the bytes transferred is equal to 
	//    the sum of the buffer sizes.
	// -- An error occurred.
	// Hence: No need to loop until all bytes are received.
	size_t len = asio::read(chan,asio::buffer(buff.data(), header.size), ec);
	return (len == header.size) ? NET_SUCCESS : NET_ERROR;
      }
    }
    catch(const std::exception& e)  {
      ::lib_rtl_output(LIB_RTL_OS,"Transfer(ASIO): Exception receiving data from %s. [%s]\n",
		       entry->name.c_str(), e.what());
    }
    return NET_ERROR;
  }
  ::memcpy(buffer, entry->data, entry->header.size);
  return NET_SUCCESS;
}
//----------------------------------------------------------------------------------
NetErrorCode NET::subscribe(void* param, unsigned int fac, net_handler_t data, net_handler_t death) {
  netlock_t lock(m_lock);
  for(Clients::iterator i=m_clients.begin(); i != m_clients.end(); ++i)  {
    if ( (*i).fac == fac && (*i).param == param ) {
      (*i).data  = data;
      (*i).death = death;
      return NET_SUCCESS;
    }
  }
  m_clients.push_back(Client());
  m_clients.back().death = death;
  m_clients.back().data  = data;
  m_clients.back().param = param;
  m_clients.back().fac   = fac;
  return NET_SUCCESS;
}

//----------------------------------------------------------------------------------
NetErrorCode NET::unsubscribe(void* param, unsigned int fac) {
  netlock_t lock(m_lock);
  for(Clients::iterator i=m_clients.begin(); i != m_clients.end(); ++i)  {
    if ( (*i).fac == fac && (*i).param == param ) {
      m_clients.erase(i);
      return NET_SUCCESS;
    }
  }
  return NET_SUCCESS;
}

//----------------------------------------------------------------------------------
NET*  TRANSFER_NS::net_init(const string& proc, int nthreads, NetConnectionType type)
{ return NET::instance(proc, nthreads, type);                      }
void  TRANSFER_NS::net_close(NET* net)
{ if ( net ) net->release();                                       }
int   TRANSFER_NS::net_send(NET* net, const void* buff, size_t size, const string& dest, unsigned int fac)
{ return net ? net->send(buff,size,dest,fac) : NET_ERROR;          }
int   TRANSFER_NS::net_receive(NET* net, netentry_t* entry, void* buff)
{ return net ? net->receive(entry, buff) : NET_ERROR;              }
int   TRANSFER_NS::net_subscribe(NET* net, void* param, unsigned int fac, net_handler_t data, net_handler_t death)
{ return net ? net->subscribe(param,fac,data,death) : NET_ERROR;   }
int   TRANSFER_NS::net_unsubscribe(NET* net, void* param, unsigned int fac)
{ return net ? net->unsubscribe(param,fac) : NET_SUCCESS;          }
void* TRANSFER_NS::net_lock(NET* net)
{ if ( net ) net->m_lock.lock(); return net ? net : 0;             }
void  TRANSFER_NS::net_unlock(NET* net, void* lock)
{ if ( net && lock ) net->m_lock.unlock();                         }
void  TRANSFER_NS::net_cancel(NET* net)
{ if ( net ) net->cancel();                                        }

#ifdef HAVE_ASIO_IPC
#define TRANSFERTEST_SEND test_asio_ipc_net_send
#define TRANSFERTEST_RECV test_asio_ipc_net_recv
#else
#define TRANSFERTEST_SEND test_asio_net_send
#define TRANSFERTEST_RECV test_asio_net_recv
#endif
#include "NET/TransferTest.h"
