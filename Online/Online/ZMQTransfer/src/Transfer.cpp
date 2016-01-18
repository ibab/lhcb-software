#define ZMQTRANSFER_INTERNALS

#include <cstdio>
#include <cerrno>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <stdexcept>
#include "RTL/rtl.h"
#include "RTL/time.h"
#include "NET/defs.h"
#include "ZMQTransfer/Context.h"
#include "ZMQTransfer/Transfer.h"
#include "ASIO/TanInterface.h"

using namespace ZMQ;
typedef  std::lock_guard<std::mutex> LOCK;

namespace ZMQ  {
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

  struct netentry_t {
    zmq::socket_t   chan;
    std::string     name;
    unsigned int    hash;
    sockaddr_in     addr;
    NET*            sys;
    netheader_t     header;
    netentry_t(int typ, const std::string& nam);
    ~netentry_t() {}
    void terminate();
    NetErrorCode setSockopts();
    NetErrorCode close();
  };

  struct  NET  {
    struct Client {
      net_handler_t data;
      net_handler_t death;
      void*         param;
      unsigned int  fac;
    };
    typedef std::vector<Client> Clients;
    typedef std::map<unsigned int,netentry_t*> netdb_t;
    explicit NET(const std::string& proc);
    ~NET();
    unsigned int             m_refCount;
    std::thread*             m_mgr;
    NetConnectionType           m_type;
    netentry_t               m_me;
    netdb_t                  m_db;
    std::mutex               m_lock;
    Clients                  m_clients;
    int                      m_cancelled;
    int                      m_stop;
    netentry_t&  connection()   { return m_me;   }
    NetConnectionType type() const { return m_type; }
    netentry_t* connect(const std::string& dest);
    netentry_t* accept(const netheader_t& header);
    void cancel();
    void sendShutdown(netentry_t *e);
    NetErrorCode disconnect(netentry_t* e);
    NetErrorCode init(NetConnectionType type);
    NetErrorCode handleMessage(netentry_t* e);
    NetErrorCode receive(netentry_t* e, void* data);
    NetErrorCode send(const void* buff, size_t size, const std::string& dest, int fac);
    NetErrorCode subscribe(void* param, unsigned int fac, net_handler_t data, net_handler_t death);
    NetErrorCode unsubscribe(void* param, unsigned int fac);
    unsigned int release();
    void handle_requests();
    static NET* instance(const std::string& proc, int nthreads, NetConnectionType type);
  };

}
using namespace ZMQ;
using namespace std;

static pair<NET*,mutex>& locked_instance()  {
  static pair<NET*,mutex> inst;
  return inst;
}

namespace  {
  //#define _net_printf printf
  static inline void _net_printf(const char*, ...) {}
  //----------------------------------------------------------------------------------
  void header_fill(netheader_t& header, const netheader_t& templ, size_t siz, int fac, NetErrorCode mtype)  {
    header.magic    = NET_MAGIC;
    header.msg_type = mtype;
    header.size     = siz;
    header.facility = fac;
    header.hash = templ.hash;
    ::memcpy(header.name,templ.name,sizeof(header.name));
  }
  netheader_t* new_header(const netheader_t& templ, size_t size, int fac, NetErrorCode mtype) {
    netheader_t* hdr = new netheader_t(templ);
    hdr->size = size; 
    hdr->facility = fac;
    hdr->msg_type = mtype;
    return hdr;
  }
  netheader_t* new_header(const netheader_t& templ)  {
    return new netheader_t(templ);
  }
}

//----------------------------------------------------------------------------------
netentry_t::netentry_t(int typ, const string& nam)
  : chan(Context::context(),typ), name(nam), hash(0), sys(0)
{
  hash = hash32(name.c_str());
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = 0;
  addr.sin_port = 0;
  header.hash = hash;
  ::strncpy(header.name,name.c_str(),sizeof(header.name)-1);
  header.name[sizeof(header.name)-1] = 0;
}

//----------------------------------------------------------------------------------
NetErrorCode netentry_t::setSockopts()  {
  int value = 0;
  ::zmq_setsockopt(chan,ZMQ_LINGER,&value,sizeof(int));
  return NET_SUCCESS;
}
//----------------------------------------------------------------------------------
void netentry_t::terminate () {
  if ( addr.sin_port )  {
    ::tan_deallocate_port_number(name.c_str());
    addr.sin_addr.s_addr = 0;
    addr.sin_port = 0;
  }
  if ( chan )  {
    close();
  }
}
//----------------------------------------------------------------------------------
NetErrorCode netentry_t::close()  {
  try {
    chan.close();
  }
  catch(...)  {
  }
  return NET_SUCCESS;
}
//----------------------------------------------------------------------------------
NET::NET(const string& proc) 
  : m_refCount(0), m_mgr(0), m_type(NET_SERVER), m_me(ZMQ_PULL, proc), m_cancelled(0), m_stop(0)
{
  m_me.sys = this;
  m_me.addr.sin_addr.s_addr = INADDR_ANY; //IN_CLASSA_HOST; // 
}
//----------------------------------------------------------------------------------
NET::~NET() {
  m_stop = 1;
  if ( m_mgr )   {
    m_mgr->join();
    delete m_mgr;
  }
  m_me.terminate();
  netheader_t header;
  header_fill(header,m_me.header,0,0,NET_CONNCLOSED);
  for (netdb_t::iterator i=m_db.begin(); i!=m_db.end();++i)  {
    auto_ptr<netentry_t> e((*i).second);
    if ( e.get() )    {
      zmq_msg_t msg;
      ::zmq_msg_init_data(&msg, new_header(header), sizeof(netheader_t), 0, 0);
      if ( ::zmq_msg_send(&msg,e->chan,0) != sizeof(netheader_t) )
	::zmq_msg_close(&msg);
    }
  }
  m_db.clear();
}

//----------------------------------------------------------------------------------
NET* NET::instance(const string& proc, int nthreads, NetConnectionType type)  {
  pair<NET*,mutex>& inst = locked_instance();
  LOCK  instance_lock(inst.second);
  NET*& ptr = inst.first;
  if ( 0 == ptr )  {
    Context::setNumberOfThreads(nthreads);
    auto_ptr<NET> conn(new NET(proc));
    NetErrorCode status = conn->init(type);
    if ( status != NET_SUCCESS )  {
      throw runtime_error("Cannot initialize network worker.");
    }
    ptr = conn.release();
  }
  else if ( ptr->type() == NET_CLIENT && type == NET_SERVER )  {
    NetErrorCode status = ptr->init(type);
    if ( status != NET_SUCCESS )  {
      throw runtime_error("Cannot initialize network server.");
    }
  }
  ptr->m_refCount++;
  return ptr;
}
//----------------------------------------------------------------------------------
unsigned int NET::release()  {
  pair<NET*,mutex>& inst = locked_instance();
  LOCK  instance_lock(inst.second);
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
NetErrorCode NET::init(NetConnectionType type)  {
  if ( m_me.addr.sin_port == 0 )  {
    m_type = type;
    // Only initialize if needed!
    if ( m_type == NET_SERVER )  {
      char conn[64];
      int status = ::tan_allocate_port_number(m_me.name.c_str(),&m_me.addr.sin_port);
      if ( status != TAN_SS_SUCCESS )  {
        ::lib_rtl_output(LIB_RTL_OS,"Allocating port number. Status %d",status);
        return NET_ERROR;
      }
      ::snprintf(conn,sizeof(conn),"tcp://*:%d",int(htons(m_me.addr.sin_port)));
      status = ::zmq_bind(m_me.chan,conn);
      if (status == -1)  {
        errno = ::lib_rtl_get_error();
        m_me.terminate();
        return NET_ERROR;
      }
      m_me.setSockopts();
      m_mgr = new thread([this]{ this->handle_requests(); });
    }
  }
  return NET_SUCCESS;
}
//----------------------------------------------------------------------------------
void NET::handle_requests()   {
  int sc;
 NextMessage:
  while( 0 == m_stop )   {
    zmq::pollitem_t item[1];
    item[0].fd = 0;
    item[0].socket = m_me.chan;
    item[0].events = ZMQ_POLLIN|ZMQ_POLLERR;
    item[0].revents = 0;
    int nclient = zmq::poll(item,1,100);
    if ( nclient == 0 )
      continue;
    else if ( nclient < 0 )
      continue;
    if ( (item[0].revents&ZMQ_POLLIN) == ZMQ_POLLIN )  {
      zmq_msg_t data;
      ::zmq_msg_init(&data);
      sc = ::zmq_msg_recv(&data, m_me.chan, 0);
      if ( sc == sizeof(netheader_t) )  {
	netentry_t* e;
	netdb_t::iterator i;
	netheader_t* header = (netheader_t*)::zmq_msg_data(&data);
	switch(header->msg_type)  {
	case NET_MSG_DATA:
	  i = m_db.find(header->hash);
	  e = (i == m_db.end()) ? accept(*header) : (*i).second;
	  if ( e && ::zmq_msg_more(&data) )  {
	    e->header.size = header->size;
	    e->header.facility = header->facility;
	    e->header.msg_type = header->msg_type;
	    ::zmq_msg_close(&data);
	    handleMessage(e);
	    goto NextMessage;
	  }
	  break;
	case NET_CONNCLOSED:
 	  i = m_db.find(header->hash);
	  if ( i != m_db.end() )  {
	    sendShutdown((*i).second);
	  }
	  break;
	case NET_MSG_HELLO:
	  accept(*header);
	  break;
	case NET_MSG_TERMINATE:
	  m_stop = 1;
	  break;
	default:
	  break;
	}
      }
      ::zmq_msg_close(&data);
    }
  }
}
//----------------------------------------------------------------------------------
void NET::cancel()   {
  m_cancelled = true;
}
//----------------------------------------------------------------------------------
netentry_t *NET::connect(const string& dest)  {
  char conn[256];
  auto_ptr<netentry_t> e(new netentry_t(ZMQ_PUSH, dest));
  int sc = ::tan_get_address_by_name(e->name.c_str(),&e->addr);
  if ( sc == TAN_SS_SUCCESS )  {
    e->sys  = this;
    e->chan = Context::socket(ZMQ_PUSH);
    ::snprintf(conn,sizeof(conn),"tcp://%s:%d",
	       inet_ntoa(e->addr.sin_addr),
	       int(htons(e->addr.sin_port)));
    sc = ::zmq_connect(e->chan,conn);
    if ( sc == 0 )  {
      e->setSockopts();
      zmq_msg_t msg;
      ::zmq_msg_init_data(&msg, new_header(m_me.header,0,0,NET_MSG_HELLO), sizeof(netheader_t), 0, 0);
      if ( ::zmq_msg_send(&msg,e->chan,0) == sizeof(netheader_t) )  {
	m_db[e->hash] = e.get();   // Connection ok 
	::zmq_msg_close(&msg);
	return e.release();
      }
      ::zmq_msg_close(&msg);
    }
  }
  return 0;
}
//----------------------------------------------------------------------------------
NetErrorCode NET::disconnect(netentry_t *e)   {
  auto_ptr<netentry_t> entry(e);
  entry->close();
  netdb_t::iterator i=m_db.find(entry->hash);
  if ( i != m_db.end() )  {
    m_db.erase(i);
  }
  return NET_SUCCESS;
}
//----------------------------------------------------------------------------------
netentry_t* NET::accept(const netheader_t& header)   {
  netdb_t::iterator i=m_db.find(header.hash);
  if ( i == m_db.end() )   {
    int sc;
    char conn[256];
    auto_ptr<netentry_t> e(new netentry_t(ZMQ_PUSH,header.name));
    e->sys  = this;
    e->name = header.name;
    e->hash = header.hash;
    e->header = header;
    sc = ::tan_get_address_by_name(e->name.c_str(),&e->addr);
    if ( sc != TAN_SS_SUCCESS )  {
      return 0;
    }
    ::snprintf(conn,sizeof(conn),"tcp://%s:%d",
	       inet_ntoa(e->addr.sin_addr),
	       int(htons(e->addr.sin_port)));
    sc = ::zmq_connect(e->chan,conn);
    if (sc == -1)  {
      e->close();
      return 0;
    }
    e->setSockopts();
    LOCK lock(m_lock);
    m_db[e->hash] = e.get();
    return e.release();
  }
  return (*i).second;
}
//----------------------------------------------------------------------------------
void NET::sendShutdown(netentry_t *e)  {
  try {
    netheader_t header;
    header_fill(header,e->header,0,0,NET_TASKDIED);
    for(Clients::iterator i=m_clients.begin(); i != m_clients.end(); ++i)  {
      Client& c = *i;
      if ( c.death ) c.death(header,c.param,0);
    }
  }
  catch(...)  {
    ::lib_rtl_output(LIB_RTL_ERROR,"Exception during disconnect handling.\n");
  }
  disconnect(e);
}
//----------------------------------------------------------------------------------
NetErrorCode NET::send(const void* buff, size_t size, const string& dest, int facility)  {
  if (size > 0)  {
    zmq_msg_t    header, msg;
    int          nbytes = 0;
    unsigned int hash   = hash32(dest.c_str());
    netdb_t::iterator i = m_db.find(hash);
    netentry_t *e = 0;
    LOCK lock(m_lock);  {
      e = (i != m_db.end()) ? (*i).second : 0;
      if ( !e )   {
	e = connect(dest);
      }
      if ( !e )  {
	return NET_TASKNOTFOUND;
      }
    }
    ::zmq_msg_init_data(&header, new_header(m_me.header, size, facility, NET_MSG_DATA), sizeof(netheader_t), 0, 0);
    nbytes = ::zmq_msg_send(&header, e->chan, ZMQ_SNDMORE);
    if ( nbytes == int(sizeof(netheader_t)) )  {
      ::zmq_msg_init_size(&msg, size);
      ::memcpy(::zmq_msg_data(&msg), (void*)buff, size);
      nbytes = ::zmq_msg_send(&msg, e->chan, 0);
      if ( nbytes == int(size) )  {
	return NET_SUCCESS;
      }
      ::zmq_msg_close(&msg);
    }
    ::zmq_msg_close(&header);
    disconnect(e);
    return NET_ERROR;
  }
  return NET_ILLEGAL_LENGTH;
}
//----------------------------------------------------------------------------------
NetErrorCode NET::subscribe(void* param, unsigned int fac, net_handler_t data, net_handler_t death) {
  LOCK lock(m_lock);
  for(Clients::iterator i=m_clients.begin(); i != m_clients.end(); ++i)  {
    if ( (*i).fac == fac && (*i).param == param ) {
      (*i).data = data;
      (*i).death = death;
      return NET_SUCCESS;
    }
  }
  m_clients.push_back(Client());
  m_clients.back().death = death;
  m_clients.back().data = data;
  m_clients.back().param = param;
  m_clients.back().fac = fac;
  return NET_SUCCESS;
}
//----------------------------------------------------------------------------------
NetErrorCode NET::unsubscribe(void* param, unsigned int fac) {
  LOCK lock(m_lock);
  for(Clients::iterator i=m_clients.begin(); i != m_clients.end(); ++i)  {
    if ( (*i).fac == fac && (*i).param == param ) {
      m_clients.erase(i);
      return NET_SUCCESS;
    }
  }
  return NET_SUCCESS;
}
//----------------------------------------------------------------------------------
NetErrorCode NET::handleMessage(netentry_t* e) {
  if ( e )  {
    for(Clients::iterator i=m_clients.begin(); i != m_clients.end(); ++i)  {
      Client& c = (*i);
      if ( c.data && c.fac == e->header.facility ) {
	c.data(e->header, c.param, e);
      }
    }
    return NET_SUCCESS;
  }
  return NET_ERROR;
}
//----------------------------------------------------------------------------------
NetErrorCode NET::receive(netentry_t* entry, void* buffer)   {
  zmq_msg_t data;
  ::zmq_msg_init(&data);
  int sc = ::zmq_msg_recv(&data, m_me.chan, 0);
  if ( sc > 0 && sc == int(entry->header.size) )  {
    void*  ptr = ::zmq_msg_data(&data);
    size_t len = ::zmq_msg_size(&data);
    if ( len != entry->header.size )  {
      ::zmq_msg_close(&data);
      return NET_ERROR;
    }
    if ( buffer ) ::memcpy(buffer, ptr, len);
    ::zmq_msg_close(&data);
    return NET_SUCCESS;
  }
  ::zmq_msg_close(&data);
  return NET_ERROR;
}
//----------------------------------------------------------------------------------
NET*  ZMQ::net_init(const string& proc, int nthreads, NetConnectionType type)
{ return NET::instance(proc, nthreads, type);                        }
void  ZMQ::net_close(NET* net)
{ if ( net ) net->release();                                         }
int   ZMQ::net_receive(NET* net, netentry_t* entry, void* buffer) 
{ return net ? net->receive(entry,buffer) : NET_ERROR;               }
int   ZMQ::net_send(NET* net, const void* buff, size_t size, const string& dest, unsigned int fac)
{ return net ? net->send(buff,size,dest,fac) : NET_ERROR;            }
int   ZMQ::net_subscribe(NET* net, void* param, unsigned int fac, net_handler_t data, net_handler_t death)
{ return net ? net->subscribe(param,fac,data,death) : NET_ERROR;     }
int   ZMQ::net_unsubscribe(NET* net, void* param, unsigned int fac)
{ return net ? net->unsubscribe(param,fac) : NET_SUCCESS;            }
void* ZMQ::net_lock(NET* net)
{ if ( net ) net->m_lock.lock(); return net ? net : 0;               }
void  ZMQ::net_unlock(NET* net, void* lock)
{ if ( net && lock ) net->m_lock.unlock();                           }
void  ZMQ::net_cancel(NET* net)
{ if ( net ) net->cancel();                                          }

#define TRANSFERTEST_SEND test_zmq_net_send
#define TRANSFERTEST_RECV test_zmq_net_recv
#include "NET/TransferTest.h"
