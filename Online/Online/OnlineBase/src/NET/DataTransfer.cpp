#define DATATRANSFER_INTERNALS

#include <cstdio>
#include <cerrno>
#include <memory>
#include <vector>
#include <map>
#include <stdexcept>
#include "RTL/rtl.h"
#include "RTL/que.h"
#include "RTL/time.h"
#include "NET/defs.h"
#include "NET/IOPortManager.h"
#include "NET/DataTransfer.h"
#include "TAN/TanInterface.h"
//#include "ASIO/TanInterface.h"
#include <mutex>
#define LOCK std::lock_guard<std::recursive_mutex>
// #include "RTL/Lock.h"
// #define LOCK RTL::Lock

/// one-at-time hash function
static unsigned int hash32(const char* key) {
  unsigned int hash;
  const char* k;
  for (hash = 0, k = key; *k; k++) {
    hash += *k; hash += (hash << 10); hash ^= (hash >> 6); 
  }
  hash += (hash << 3); hash ^= (hash >> 11); hash += (hash << 15);
  return hash;
}

static const unsigned int COPY_LIMIT   = (8192*2);
static const int LINGER_VALUE          =  0;
static const unsigned int CHOP_SIZE    = (8192*4);
//static const unsigned int CHOP_SIZE    = (8192*8);
static const unsigned int LOWER_CHOP   = 4096;
static const int MAX_TCP_ERRORS        = 20;
static const int SNDBUF_VALUE          = 8192;
//static const int SNDBUF_VALUE          = CHOP_SIZE;
static const int RCVBUF_VALUE        = CHOP_SIZE;
//static const int RCVBUF_VALUE          = 100*CHOP_SIZE;

static const int NET_KEEPALIVE_TIMEOUT     = 15;
static const int NET_KEEPALIVE_MAXPACKETS  = 3;
static const int NET_KEEPALIVE_INTERVAL    = NET_KEEPALIVE_TIMEOUT/3;


namespace DataTransfer  {
  struct netentry_t {
    int           chan;
    unsigned int  sndBuffSize;
    std::string   name;
    unsigned int  hash;
    sockaddr_in   addr;
    NET*          sys;
    netheader_t   header;
    NetErrorCode  status;
    void*         data;
    netentry_t() : chan(-1), sndBuffSize(0), sys(0), status(NET_SUCCESS)    {}
    ~netentry_t() {}
    void terminate();
    NetErrorCode setSockopts();
    NetErrorCode close();
    NetErrorCode setSendBuff(unsigned int siz);
    NetErrorCode send(const void *buff, size_t siz, unsigned int flag);
    NetErrorCode recv(void *buffer, size_t siz, unsigned int flag);
  };

  struct  NET  {
    //typedef DataTransfer::net_handler_t net_handler_t;
    struct Client {
      net_handler_t data;
      net_handler_t death;
      void*         param;
      unsigned int  fac;
    };
    typedef std::vector<Client> Clients;
    explicit NET(const std::string& proc);
    ~NET();
    unsigned int             m_refCount;
    IOPortManager            m_mgr;
    NetConnectionType        m_type;
    netentry_t               m_me;
    std::map<unsigned int,netentry_t*> m_db;
    std::recursive_mutex     m_lockid;
    //lib_rtl_lock_t           m_lockid;
    Clients                  m_clients;
    int                      m_cancelled;
    NetConnectionType type() const { return m_type; }
    netentry_t* connect(const std::string& dest);
    NetErrorCode remove(netentry_t *e);
    void cancel();
    void sendShutdown(netentry_t *e);
    NetErrorCode disconnect(netentry_t *e);
    NetErrorCode accept();
    NetErrorCode init(NetConnectionType type);
    NetErrorCode handleMessage(netentry_t* e);
    NetErrorCode receive(netentry_t* e, void* data);
    NetErrorCode setSockopts(int chan);
    NetErrorCode send(const void* buff, size_t size, netentry_t* dest, int fac);
    NetErrorCode send(const void* buff, size_t size, const std::string& dest, int fac);
    NetErrorCode subscribe(void* param, unsigned int fac, net_handler_t data, net_handler_t death);
    NetErrorCode unsubscribe(void* param, unsigned int fac);
    static int acceptAction (void* param);
    static int recvAction (void* param);
    unsigned int release();
    static NET* instance(const std::string& proc, NetConnectionType type);
  };
}
using namespace DataTransfer;

static NET*& inst()  {
  static NET* s_net_instance = 0;
  return s_net_instance;
}

//#define _net_printf printf
namespace  {

  inline void _net_printf(const char*, ...) {}

  //----------------------------------------------------------------------------------
  void header_fill(netheader_t& header, const char* nam,unsigned int hash_val, size_t siz, int fac, int mtype, const void *buf)    {
#if 0
    header.magic    = htonl(NET_MAGIC);
    header.size     = htonl(siz);
    header.facility = htonl(fac);
    header.msg_type = htonl(mtype);
    header.hash     = htonl(hash_val);
#endif
    header.magic    = NET_MAGIC;
    header.size     = siz;
    header.facility = fac;
    header.msg_type = mtype;
    header.hash     = hash_val;
    ::strncpy(header.name,nam,sizeof(header.name));
    header.name[sizeof(header.name)-1] = 0;
    if (buf != 0 && siz>0)  {
      ::memcpy(&header+1,buf,siz);
    }
  }
  //----------------------------------------------------------------------------------
  void header_net_to_host (netheader_t&)  {}
#if 0
  void header_net_to_host (netheader_t& header)  {
    header.magic    = ntohl(NET_MAGIC);
    header.hash     = ntohl(header.hash);
    header.size     = ntohl(header.size);
    header.msg_type = ntohl(header.msg_type);
    header.facility = ntohl(header.facility);
  }
#endif
}

//----------------------------------------------------------------------------------
int NET::recvAction (void* param)    {
  netentry_t* e = (netentry_t*)param;
  e->sys->handleMessage(e);
  return NET_SUCCESS;
}
//----------------------------------------------------------------------------------
NetErrorCode netentry_t::setSockopts()  {
  int on = 1;
  struct linger Linger;
  Linger.l_onoff = 1;
  Linger.l_linger = LINGER_VALUE;
  ::setsockopt(chan, SOL_SOCKET, SO_LINGER,   (const char*)&Linger, sizeof(Linger));
  ::setsockopt(chan, SOL_SOCKET, SO_SNDBUF,   (const char*)&SNDBUF_VALUE, sizeof(int)) ;
  ::setsockopt(chan, SOL_SOCKET, SO_RCVBUF,   (const char*)&RCVBUF_VALUE, sizeof(int)) ;
  ::setsockopt(chan, SOL_SOCKET, SO_REUSEADDR,(const char*)&on, sizeof(on));
  ::setsockopt(chan, SOL_SOCKET, SO_BROADCAST,(const char*)&on, sizeof(on));
  ::setsockopt(chan, SOL_SOCKET, SO_OOBINLINE,(const char*)&on, sizeof(on));
  return NET_SUCCESS;
}
//----------------------------------------------------------------------------------
NetErrorCode netentry_t::setSendBuff(unsigned int siz) {
  unsigned int sndbuf = (siz>CHOP_SIZE) ? CHOP_SIZE : (siz<LOWER_CHOP) ? LOWER_CHOP : siz;
  sndbuf *= 2;
  //sndbuf = siz;
  if ( sndbuf != sndBuffSize )     {
    ::setsockopt(chan, SOL_SOCKET, SO_SNDBUF, (const char*)&sndbuf, sizeof(int));
    sndBuffSize = sndbuf;
  }
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
  setSockopts();
  ::shutdown(chan,2);
  ::socket_close(chan);
  chan = 0;
  return NET_SUCCESS;
}
//----------------------------------------------------------------------------------
NetErrorCode netentry_t::send(const void *buffer,size_t siz,u_int flag) {
  const char* buff = (const char*)buffer;
  u_int Npack = siz / CHOP_SIZE;
  u_int Nrest = siz % CHOP_SIZE;
  u_int already_sent = 0;
  for (size_t i = 0; i <= Npack; ++i )  {
    u_int tosend = i<Npack ? CHOP_SIZE : Nrest;
    u_int sent   = 0;
    while (tosend>0)  {
      int sent_now = ::send (chan, buff + already_sent, tosend, flag);
      if (sent_now == -1)  {
        errno = ::lib_rtl_socket_error();
        ::lib_rtl_signal_message(LIB_RTL_OS,"NET: send error size:%d/%d/%d/%d errno=%d",
                                 siz,already_sent,tosend,sent,errno);
        // lib_rtl_start_debugger();
        return NET_ERROR;
      }
      already_sent += sent_now;
      sent         += sent_now;
      tosend       -= sent_now;
    }
  }
  return NET_SUCCESS;
}
//----------------------------------------------------------------------------------
NetErrorCode netentry_t::recv(void *buffer, size_t siz, unsigned int flag)  {
  u_int got = 0;
  int count = 0;
  size_t toget = siz;
  char* buff = (char*)buffer;
  while (got != siz)  {
    int got_now = ::recv (chan, buff + got, toget, flag);
    if (got_now <= 0)    {
      errno = lib_rtl_socket_error();
      _net_printf("NET: receive error errno=%d\n",errno);
      switch(errno)  {
      case ESOCK_CONNREFUSED:
      case ESOCK_NOTCONN:
      case ESOCK_CONNABORTED:
      case ESOCK_CONNRESET:
      case ESOCK_NETRESET:
      case ESOCK_NETDOWN:
      case ESOCK_SHUTDOWN:
        return NET_CONNCLOSED;
      case ESOCK_NOTSOCK:
      case ESOCK_FAULT:
      case ESOCK_INVAL:
      case ESOCK_BADF:
        return NET_TERRIBLE;
      case ESOCK_AGAIN:
        continue;
      default:
        if (++count == MAX_TCP_ERRORS)
          return NET_NODATA;
        else {
          _net_printf("NET: receive error (ignored) errno=%d\n",errno);
          got_now = 0;
        }
      }
    }
    got += got_now;
    toget -= got_now;
  }
  return NET_SUCCESS;
}
//----------------------------------------------------------------------------------
NET::NET(const std::string& proc) 
  : m_refCount(0), m_mgr(0), m_type(NET_SERVER), m_lockid(), m_cancelled(0)
{
  m_me.sys = this;
  m_me.name = proc;
  m_me.hash = hash32(m_me.name.c_str());
  m_me.addr.sin_port = 0;
  m_me.addr.sin_family = AF_INET;
  m_me.addr.sin_addr.s_addr = INADDR_ANY; //IN_CLASSA_HOST; // 
  // Initialize header properties
  m_me.header.hash = m_me.hash;
  ::strncpy(m_me.header.name,proc.c_str(),sizeof(m_me.header.name)-1);
  m_me.header.name[sizeof(m_me.header.name)-1] = 0;
}
//----------------------------------------------------------------------------------
NET::~NET() {
  if ( m_me.chan )  {
    m_mgr.remove(m_me.chan, false);
  }
  m_me.terminate();
  for (std::map<unsigned int,netentry_t*>::iterator i=m_db.begin(); i!=m_db.end();++i)  {
    netentry_t* e = (*i).second;
    if ( e )    {
      netheader_t h;
      header_fill(h,m_me.name.c_str(),m_me.hash,0,0,NET_CONNCLOSED,0);
      m_mgr.remove(e->chan, false);
      e->setSendBuff(sizeof(h));
      e->send(&h, sizeof(h), 0);
      e->close();
      delete e;
    }
  }
  m_db.clear();
}
//----------------------------------------------------------------------------------
void NET::cancel()   {
  m_cancelled = true;
}
//----------------------------------------------------------------------------------
int NET::acceptAction (void* param)    {
  NET* sys = (NET*)param;
  return sys->accept();
}
//----------------------------------------------------------------------------------
NetErrorCode NET::remove(netentry_t *e)    {
  LOCK lock(m_lockid);
  std::map<unsigned int,netentry_t*>::iterator i=m_db.find(e->hash);
  if ( i != m_db.end() )  {
    m_db.erase(i);
    return NET_SUCCESS;
  }
  return NET_TASKNOTFOUND;
}
//----------------------------------------------------------------------------------
netentry_t *NET::connect(const std::string& dest)  {
  std::auto_ptr<netentry_t> e(new netentry_t);
  e->name = dest;
  e->hash = hash32(e->name.c_str());
  int retry, sc = ::tan_get_address_by_name(e->name.c_str(),&e->addr);
  if ( sc != TAN_SS_SUCCESS )  {
    return 0;
  }
  e->sys  = this;
  e->chan = ::socket(AF_INET,SOCK_STREAM,0); 
  if (e->chan == -1)  {
    return 0;
  }
  for(sc=1, retry=4; retry > 0 && sc != 0; --retry )  {
    sc = ::connect(e->chan,(sockaddr*)&e->addr,sizeof(sockaddr_in));
    if ( sc != 0 )  {
      ::lib_rtl_output(LIB_RTL_OS,"Failed to connect to DataTransfor(connect)");
      ::lib_rtl_sleep(1000);
    }
  }
  if (sc == -1)  {
    ::socket_close(e->chan);
    return 0;
  }
  netheader_t h = m_me.header;
  h.size = 0;
  h.facility = 0;
  h.msg_type = NET_MSG_HELLO;
  e->setSockopts();
  sc = e->send(&h, sizeof(netheader_t),0);
  if ( sc != NET_SUCCESS )  {
    e->close();
    return 0;
  }
  m_mgr.add(1,e->chan,recvAction,e.get());
  return e.release();
}
//----------------------------------------------------------------------------------
NetErrorCode NET::accept()   {
  std::auto_ptr<netentry_t> e(new netentry_t());
  socklen_t n = sizeof (e->addr);
  e->sys  = this;
  e->chan = ::accept(m_me.chan, (sockaddr*)&e->addr, &n);
  if ( e->chan > 0 )  {
    netheader_t h;
    /// Enable keepalive for the given channel
    int on = 1, timeout=NET_KEEPALIVE_TIMEOUT;
    int maxpacket=NET_KEEPALIVE_MAXPACKETS, interval=NET_KEEPALIVE_INTERVAL;
    ::setsockopt(e->chan, SOL_SOCKET, SO_KEEPALIVE, (char*)&on, sizeof(on));
    /// Set the keepalive poll interval to something small.
    /// Warning: this may not be portable!
    ::setsockopt(e->chan, IPPROTO_TCP, TCP_KEEPIDLE,  (char*)&timeout, sizeof(timeout));
    ::setsockopt(e->chan, IPPROTO_TCP, TCP_KEEPCNT,   (char*)&maxpacket, sizeof(maxpacket));
    ::setsockopt(e->chan, IPPROTO_TCP, TCP_KEEPINTVL, (char*)&interval, sizeof(interval));

    int status = e->recv(&e->header,sizeof(e->header),0);
    e->name = e->header.name;
    e->hash = e->header.hash;
    if ( status != NET_SUCCESS )  {
      e->close();
      return NET_ERROR;
    }
    e->setSockopts();
    {
      LOCK lock(m_lockid);
      m_db[e->hash] = e.get();
    }
    m_mgr.add(0,m_me.chan,acceptAction, this);
    m_mgr.add(1,e->chan,recvAction,e.get());
    e.release();
    return NET_SUCCESS;
  }
  errno = ::lib_rtl_socket_error();
  return NET_ERROR;
}
//----------------------------------------------------------------------------------
NetErrorCode NET::disconnect(netentry_t *e)   {
  //::printf("Close connection: chan=%d addr=%s\n",e->chan,inet_ntoa(e->addr.sin_addr));
  m_mgr.remove(e->chan, false);
  e->close();
  remove(e);
  delete e;
  return NET_SUCCESS;
}
//----------------------------------------------------------------------------------
void NET::sendShutdown(netentry_t *e)  {
  try {
    header_fill(e->header,e->name.c_str(),e->hash,0,0,NET_TASKDIED,0);
    header_net_to_host(e->header);
    for(Clients::iterator i=m_clients.begin(); i != m_clients.end(); ++i)  {
      if ( (*i).death ) (*((*i).death))(e->header, (*i).param, 0);
    }
  }
  catch(...)  {
    ::lib_rtl_output(LIB_RTL_ERROR,"Exception during disconnect handling.\n");
  }
  disconnect(e);
}
//----------------------------------------------------------------------------------
NetErrorCode NET::handleMessage(netentry_t* e) {
  LOCK lock(m_lockid);
  bool handled = false;
  e->status = e->recv(&e->header,sizeof(netheader_t),0);
  header_net_to_host(e->header);
  switch (e->status)  {
  case NET_SUCCESS:        // allocate memory for message
    switch(e->header.msg_type)  {
    case NET_CONNCLOSED:
      sendShutdown(e);
      break;
    default:
      e->data = 0;
      for(Clients::iterator i=m_clients.begin(); i != m_clients.end(); ++i)  {
        if ( (*i).data && (*i).fac == e->header.facility ) {
          (*((*i).data))(e->header,(*i).param, e);
	  handled = true;
        }
      }
      if ( !handled )  {
        char* buff = new char[e->header.size];
        e->status = receive(e,buff);
        delete [] buff;
      }
      switch(e->status)  {
      case NET_SUCCESS:
        m_mgr.add(1,e->chan,recvAction,e);
        return NET_SUCCESS;
      case NET_CONNCLOSED:
      default:
        sendShutdown(e);
        return NET_SUCCESS;
      }
      break;
    }
    return NET_SUCCESS;
  case NET_NODATA:
    return NET_SUCCESS;
  case NET_CONNCLOSED:
  default:
    sendShutdown(e);
    return NET_SUCCESS;
  }
}
//----------------------------------------------------------------------------------
NetErrorCode NET::send(const void* buff, size_t size, netentry_t* e, int facility)  {
  NetErrorCode status = NET_SUCCESS;
  // Build full source and destination names 
  if (size <= 0)  {
    return NET_ILLEGAL_LENGTH;
  }
  if (e == 0) {
    return NET_TASKNOTFOUND;
  }
  size_t len = size+sizeof(netheader_t);
  e->setSendBuff(len);
  std::auto_ptr<netheader_t> h(new(::operator new(len)) netheader_t);
  if (size <= COPY_LIMIT)    {
    header_fill(*h,m_me.name.c_str(),m_me.hash,size,facility,NET_MSG_DATA,buff);
    status = e->send(h.get(),len,0);
  }
  else    {
    header_fill(*h,m_me.name.c_str(),m_me.hash,size,facility,NET_MSG_DATA,0);
    status = e->send(h.get(),sizeof(netheader_t),0);
    if (status == NET_SUCCESS)  {
      status = e->send(buff,size,0);
    }
  }
  return status;
}
//----------------------------------------------------------------------------------
NetErrorCode NET::send(const void* buff, size_t size, const std::string& dest, int facility)  {
  //
  // This logic here is a bit complicated:
  // The IO port manager MUST be locked before sending, because concurrent messages
  // may arrive (if things go fast!). Taking the lock of the IO port manager
  // prevents the NET from receiving messages in case the sender died and the 
  // netentry_t must be removed. Otherwise a dead-lock situation cannot be
  // avoided.
  //
  // M.Frank  10/08/2007
  //
  NetErrorCode status = NET_CONNCLOSED;
  unsigned int hash = hash32(dest.c_str());
  netentry_t *e = 0;    {
    LOCK lock(m_lockid);
    std::map<unsigned int,netentry_t*>::iterator i=m_db.find(hash);
    e = i != m_db.end() ? (*i).second : 0;
    if ( !e ) {
      if ( !(e=connect(dest)) ) {
        return NET_TASKNOTFOUND;
      }
      m_db[e->hash] = e;                           // Connection ok 
    }
  }
  status = send(buff,size,e,facility);
  if (status != NET_SUCCESS)    {
    disconnect(e);
  }
  return status;
}
//----------------------------------------------------------------------------------
NetErrorCode NET::init(NetConnectionType type)  {
  // Only initialize if needed!
  if ( m_me.addr.sin_port == 0 )  {
    int status = 1;
    m_type = type;
    if ( type == NET_SERVER )  {
      status = ::tan_allocate_port_number(m_me.name.c_str(),&m_me.addr.sin_port);
      if ( status != TAN_SS_SUCCESS )  {
        ::lib_rtl_signal_message(LIB_RTL_OS,"Allocating port number. Status %d",status);
        return NET_ERROR;
      }
      m_mgr.setPort(m_me.addr.sin_port);
      m_me.chan = ::socket(AF_INET, SOCK_STREAM, 0);
      if (m_me.chan == -1)   {
        errno = ::lib_rtl_get_error();
        m_me.chan = 0;
        m_me.terminate();
        return NET_ERROR;
      }
      m_me.setSockopts();
      status = ::bind(m_me.chan,(sockaddr*)&m_me.addr,sizeof(sockaddr_in));
      if (status == -1)  {
        errno = ::lib_rtl_get_error();
        m_me.terminate();
        return NET_ERROR;
      }
      status = ::listen (m_me.chan, 5);
      if (status == -1)  {
        errno = ::lib_rtl_get_error();
        ::socket_close (m_me.chan);
        m_me.terminate();
        return NET_ERROR;
      }
      m_mgr.add(0,m_me.chan,acceptAction,this);
    }
    else   {
      m_mgr.setPort(0xFEED);
      m_mgr.add(0,0,acceptAction,this);
    }
  }
  return NET_SUCCESS;
}
//----------------------------------------------------------------------------------
NetErrorCode NET::receive(netentry_t* e, void* data)  {
  if ( data )  {
    e->status = e->recv(data,e->header.size,0);
    e->data =  data;
    return e->status;
  }
  data = ::operator new(e->header.size);
  e->status = e->recv(data, e->header.size,0);
  e->data = 0;
  ::operator delete(data);
  return e->status;
}
//----------------------------------------------------------------------------------
NET* NET::instance(const std::string& proc, NetConnectionType type)  {
  NET*& n = inst();
  if ( 0 == n )  {
    std::auto_ptr<NET> conn(new NET(proc));
    NetErrorCode status = conn->init(type);
    if ( status != NET_SUCCESS )  {
      throw std::runtime_error("Cannot initialize network worker.");
    }
    n = conn.release();
  }
  else if ( n->type() == NET_CLIENT && type == NET_SERVER )  {
    NetErrorCode status = n->init(type);
    if ( status != NET_SUCCESS )  {
      throw std::runtime_error("Cannot initialize network server.");
    }
  }
  LOCK lock(n->m_lockid);
  n->m_refCount++;
  return n;
}
//----------------------------------------------------------------------------------
unsigned int NET::release()  {
  NET*& n = inst();
  if ( n )  {
    n->m_lockid.lock();
    void* lock = n->m_mgr.lock();
    unsigned int cnt = --n->m_refCount;
    if ( cnt == 0 )  {
      __NetworkPort__  port = n->m_mgr.port();
      IOPortManager mgr(port);
      n->m_lockid.unlock();
      delete n;
      mgr.stop(lock);
      mgr.unlock(lock);
      mgr.join(lock);
      n = 0;
      return cnt;
    }
    n->m_lockid.unlock();
    n->m_mgr.unlock(lock);
    return cnt;
  }
  return 0;
}
//----------------------------------------------------------------------------------
NetErrorCode NET::subscribe(void* param, unsigned int fac, net_handler_t data, net_handler_t death) {
  LOCK lock(m_lockid);
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
  LOCK lock(m_lockid);
  for(Clients::iterator i=m_clients.begin(); i != m_clients.end(); ++i)  {
    if ( (*i).fac == fac && (*i).param == param ) {
      m_clients.erase(i);
      return NET_SUCCESS;
    }
  }
  return NET_SUCCESS;
}
//----------------------------------------------------------------------------------
NET* DataTransfer::net_init(const std::string& proc, int /* nthreads */, NetConnectionType type)
{ return NET::instance(proc, type);                               }
void DataTransfer::net_close(NET* net)
{ if ( net ) net->release();                                      }
int DataTransfer::net_receive(NET* net, netentry_t* e, void* buff) 
{ return net ? net->receive(e,buff) : NET_ERROR;                  }
int DataTransfer::net_send(NET* net, const void* buff, size_t size, const std::string& dest, unsigned int fac)
{ return net ? net->send(buff,size,dest,fac) : NET_ERROR;         }
int DataTransfer::net_subscribe(NET* net, void* param, unsigned int fac, net_handler_t data, net_handler_t death)
{ return net ? net->subscribe(param,fac,data,death) : NET_ERROR;  }
int DataTransfer::net_unsubscribe(NET* net, void* param, unsigned int fac)
{ return net ? net->unsubscribe(param,fac) : NET_SUCCESS;         }
void* DataTransfer::net_lock(NET* net)
{ return net ? net->m_mgr.lock() : 0;                             }
void DataTransfer::net_unlock(NET* net, void* lock)
{ if ( net && lock ) net->m_mgr.unlock(lock);                     }
void DataTransfer::net_cancel(NET* net)
{ if ( net ) net->cancel();                                       }

#define TRANSFERTEST(x) test_socket_net_##x
#include "NET/TransferTest.h"
