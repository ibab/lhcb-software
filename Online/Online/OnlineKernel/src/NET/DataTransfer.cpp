#define DATATRANSFER_INTERNALS

#include <cstdio>
#include <cerrno>
#include <memory>
#include <vector>
#include <stdexcept>
#include "RTL/que.h"
#include "RTL/Lock.h"
#include "NET/defs.h"
#include "NET/IOPortManager.h"
#include "NET/DataTransfer.h"
#include "TAN/TanInterface.h"

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
    netentry_t() : chan(-1), sndBuffSize(0), sys(0), status(NET_SUCCESS)    {}
    ~netentry_t() {}
    void terminate();
    NetErrorCode setSockopts();
    NetErrorCode close();
    NetErrorCode setSendBuff(unsigned int siz);
    NetErrorCode send(const void *buff, size_t siz, unsigned int flag);
    NetErrorCode recv(void *buffer, size_t siz, unsigned int flag);
  };
}

using namespace DataTransfer;

static const unsigned int COPY_LIMIT   = (8192*2);
static const int LINGER_VALUE          =  0;
static const unsigned int CHOP_SIZE    = (8192*4);
static const unsigned int LOWER_CHOP   = 4096;
static const int MAX_TCP_ERRORS        = 20;
static const int SNDBUF_VALUE          = 8192;
static const int RCVBUF_VALUE          = CHOP_SIZE;

namespace DataTransfer  {
  struct  NET  {
    typedef void (*user_callback_t)(netentry_t* hdr, void* param);
    NET(const std::string& proc, user_callback_t cb, void* param);
    ~NET();
    unsigned int             m_refCount;
    IOPortManager            m_mgr;
    netentry_t               m_me;
    std::map<unsigned int,netentry_t*> m_db;
    lib_rtl_lock_t           m_lockid;
    void*                    user_callback_param;
    void                   (*user_callback)(netentry_t* hdr, void* param);
    netentry_t* connect(const std::string& dest);
    NetErrorCode remove(netentry_t *e);
    void sendShutdown(netentry_t *e);
    NetErrorCode disconnect(netentry_t *e);
    NetErrorCode accept();
    NetErrorCode close();
    NetErrorCode init();
    NetErrorCode handleMessage(netentry_t* e);
    NetErrorCode getData(netentry_t* e, void* data);
    NetErrorCode setSockopts(int chan);
    NetErrorCode send(const void* buff, size_t size, netentry_t* dest, int fac);
    NetErrorCode send(const void* buff, size_t size, const std::string& dest, int fac);
    static int acceptAction (void* param);
    static int recvAction (void* param);
  };
}

//#define _net_printf printf
static inline void _net_printf(const char*, ...) {}

//----------------------------------------------------------------------------------
void netheader_t::fill(const char* nam,unsigned int hash_val, size_t siz, int fac, int mtype, const void *buf)    {
  magic    = NET_MAGIC;
  size     = htonl(siz);
  facility = htonl(fac);
  msg_type = htonl(mtype);
  hash     = htonl(hash_val);
  strncpy(name,nam,sizeof(name));
  name[sizeof(name)-1] = 0;
  if (buf != 0 && siz>0)  {
    ::memcpy(this+1,buf,siz);
  }
}
//----------------------------------------------------------------------------------
void netheader_t::net_to_host ()  {
  hash     = ntohl(hash);
  size     = ntohl(size);
  msg_type = ntohl(msg_type);
  facility = ntohl(facility);
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
    setSockopts(); 
    ::socket_close(chan);
  }
}
//----------------------------------------------------------------------------------
NetErrorCode netentry_t::close()  {
  setSockopts();
  ::shutdown(chan,2);
  ::socket_close(chan);
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
    u_int sent  = 0;
    while (sent != siz)    {
      int sent_now = ::send (chan, buff + already_sent, tosend, flag);
      if (sent_now == -1)  {
        errno = ::lib_rtl_socket_error();
        ::lib_rtl_signal_message(LIB_RTL_OS,"NET: send error size:%d/%d/%d errno=%d",
                                 siz,already_sent,tosend,errno);
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
NET::NET(const std::string& proc, user_callback_t cb, void* param) 
: m_refCount(0), m_mgr(0), m_lockid(0), 
  user_callback_param(param), 
  user_callback(cb)  
{
  m_lockid = 0;
  m_me.sys = this;
  m_me.name = proc;
  m_me.hash = hash32(m_me.name.c_str());
  m_me.addr.sin_port = 0;
  m_me.addr.sin_family = AF_INET;
  m_me.addr.sin_addr.s_addr = INADDR_ANY; //IN_CLASSA_HOST; // 
}
//----------------------------------------------------------------------------------
NET::~NET() {
  close();
}
//----------------------------------------------------------------------------------
int NET::acceptAction (void* param)    {
  NET* sys = (NET*)param;
  return sys->accept();
}
//----------------------------------------------------------------------------------
NetErrorCode NET::remove(netentry_t *e)    {
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
  int sc = ::tan_get_address_by_name(e->name.c_str(),&e->addr);
  if ( sc != TAN_SS_SUCCESS )  {
    return 0;
  }
  e->sys  = this;
  e->chan = ::socket(AF_INET,SOCK_STREAM,0); 
  if (e->chan == -1)  {
    return 0;
  }
  e->setSockopts();
  sc = ::connect(e->chan,(sockaddr*)&e->addr,sizeof(sockaddr_in));
  if (sc == -1)  {
    ::socket_close(e->chan);
    return 0;
  }
  sc = e->send(m_me.name.c_str(),sizeof(e->header.name),0);
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
  if (e->chan > 0 )  {
    netheader_t h;
    int status = e->recv(h.name,sizeof(h.name),0);
    h.name[sizeof(h.name)-1] = 0;
    e->name = h.name;
    e->hash = hash32(e->name.c_str());
    if ( status != NET_SUCCESS )  {
      e->close();
      return NET_ERROR;
    }
    RTL::Lock lock(m_lockid);
    e->setSockopts();
    m_db[e->hash] = e.get();
    ::printf("New connection: %s chan=%d addr=%s\n",
      e->name.c_str(),e->chan,inet_ntoa(e->addr.sin_addr));
    m_mgr.add(0,m_me.chan,acceptAction, this);
    m_mgr.add(1,e->chan,recvAction,e.get());
    e.release();
    return NET_SUCCESS;
  }
  errno = lib_rtl_socket_error();
  return NET_ERROR;
}
//----------------------------------------------------------------------------------
NetErrorCode NET::disconnect(netentry_t *e)   {
  ::printf("Close connection: chan=%d addr=%s\n",e->chan,inet_ntoa(e->addr.sin_addr));
  m_mgr.remove(e->chan);
  e->close();
  remove(e);
  delete e;
  return NET_SUCCESS;
}
//----------------------------------------------------------------------------------
void NET::sendShutdown(netentry_t *e)  {
  try {
    e->header.fill(e->name.c_str(),e->hash,0,0,NET_TASKDIED,0);
    e->header.net_to_host();
    (*this->user_callback)(e,this->user_callback_param);
  }
  catch(...)  {
    ::lib_rtl_printf("Exception during disconnect handling.\n");
  }
  disconnect(e);
}
//----------------------------------------------------------------------------------
NetErrorCode NET::handleMessage(netentry_t* e) {
  RTL::Lock lock(m_lockid);
  e->status = e->recv(&e->header,sizeof(netheader_t),0);
  e->header.net_to_host();
  switch (e->status)  {
  case NET_SUCCESS:        // allocate memory for message
    switch(e->header.msg_type)  {
    case NET_CONNCLOSED:
      sendShutdown(e);
      break;
    default:
      (*user_callback)(e,user_callback_param);
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
    h->fill(m_me.name.c_str(),m_me.hash,size,facility,NET_MSG_DATA,buff);
    status = e->send(h.get(),len,0);
  }
  else    {
    h->fill(m_me.name.c_str(),m_me.hash,size,facility,NET_MSG_DATA,0);
    status = e->send(h.get(),sizeof(netheader_t),0);
    if (status == NET_SUCCESS)  {
      status = e->send(buff,size,0);
    }
  }
  return status;
}
//----------------------------------------------------------------------------------
NetErrorCode NET::send(const void* buff, size_t size, const std::string& dest, int facility)  {
  RTL::Lock lock(m_lockid);
  unsigned int hash = hash32(dest.c_str());
  std::map<unsigned int,netentry_t*>::iterator i=m_db.find(hash);
  netentry_t *e = i != m_db.end() ? (*i).second : 0;
  if ( !e ) {
    if ( !(e=connect(dest)) )
      return NET_TASKNOTFOUND;
    m_db[e->hash] = e;                           // Connection ok 
  }
  NetErrorCode status = send(buff,size,e,facility);
  if (status != NET_SUCCESS)    {
    disconnect(e);
  }
  return status;
}
//----------------------------------------------------------------------------------
NetErrorCode NET::close()   {
  {
    RTL::Lock lock(m_lockid);
    m_me.terminate();
    for (std::map<unsigned int,netentry_t*>::iterator i=m_db.begin(); i!=m_db.end();++i)  {
      netentry_t* e = (*i).second;
      if ( e )    {
        netheader_t h;
        h.fill(m_me.name.c_str(),m_me.hash,0,0,NET_CONNCLOSED,0);
        m_mgr.remove(e->chan);
        e->setSendBuff(sizeof(h));
        e->send(&h, sizeof(h), 0);
        e->close();
        delete e;
      }
    }
    m_db.clear();
  }
  if (m_lockid) lib_rtl_delete_lock(m_lockid);
  m_lockid = 0;
  return NET_SUCCESS;
}
//----------------------------------------------------------------------------------
NetErrorCode NET::init()  {
  int status = ::lib_rtl_create_lock(0, &m_lockid);
  if ( !lib_rtl_is_success(status) )  {
    ::lib_rtl_signal_message(LIB_RTL_OS,"Error creating NET lock. Status %d",status);
    return NET_ERROR;
  }
  printf("%s\n\n",m_me.name.c_str());
  status = ::tan_allocate_port_number(m_me.name.c_str(),&m_me.addr.sin_port);
  if ( status != TAN_SS_SUCCESS )  {
    ::lib_rtl_signal_message(LIB_RTL_OS,"Allocating port number. Status %d",status);
    return NET_ERROR;
  }
  m_mgr.setPort(m_me.addr.sin_port);
  m_me.chan = ::socket(AF_INET, SOCK_STREAM, 0);
  if (m_me.chan == -1)   {
    errno = ::lib_rtl_get_error();
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
    ::socket_close (m_me.chan);
    m_me.terminate();
    return NET_ERROR;
  }
  m_mgr.add(0,m_me.chan,acceptAction,this);
  return NET_SUCCESS;
}
//----------------------------------------------------------------------------------
NetErrorCode NET::getData(netentry_t* e, void* data)  {
  e->status = e->recv(data,e->header.size,0);
  return e->status;
}
//----------------------------------------------------------------------------------
Worker::Worker(const std::string& proc)  {
  std::auto_ptr<NET> conn(new NET(proc,recvAction,this));
  NetErrorCode status = conn->init();
  if ( status != NET_SUCCESS )  {
    throw std::runtime_error("Cannot initialize network worker.");
  }
  m_net = conn.release();
}
Worker::~Worker()  
{  if ( m_net ) delete m_net;                   }
NetErrorCode Worker::send(const void* buff, size_t size, const std::string& dest, int fac) {
  return m_net->send(buff,size,dest,fac);       
}
NetErrorCode Worker::send(const void* buff, size_t size, netentry_t* dest, int fac)
{ return m_net->send(buff,size,dest,fac);       }
NetErrorCode Worker::get(netentry_t* e, void* data)  
{ return m_net->getData(e,data);                }
void Worker::recvAction(netentry_t* e, void* param)  
{ ((Worker*)param)->receive(e,e->header);       }
void Worker::receive(netentry_t* e, const netheader_t& hdr)  {
  char* buff = new char[hdr.size];
  this->get(e, buff);
  delete [] buff;
}

#include "RTL/rtl.h"
namespace {
  void help() {
    ::printf("net_send -opt [-opt]\n");
    ::printf("    -t<urns>=<number>          Number of receive/send turns\n");
    ::printf("    -length=<number>           Message length (Client only)\n");
  }
  void help_recv() {
    ::printf("net_recv -opt [-opt]\n");
  }
  class NetSensor : public Worker  {
    bool m_bounce;
  public:
    NetSensor(const std::string& proc,bool bounce=false)
    : Worker(proc), m_bounce(bounce) {}
    virtual ~NetSensor() {}
    virtual void receive(netentry_t* e, const netheader_t& hdr)  {
      static int cnt = 0;
      static time_t start = time(0);
      static char buff[1024*128];
      if ( e->header.msg_type == NET_TASKDIED )  {
        ::printf("Task died: %s chan=%d addr=%s\n",hdr.name,e->chan,inet_ntoa(e->addr.sin_addr));
        return;
      }
      get(e, buff);
      cnt++;
      if ( cnt%1000 == 0 )  {
        printf("%3ld %s %d messages [%s]. chan:%d port:%d addr:%s\n",
          time(0)-start, m_bounce ? "Bounced" : "Received",cnt,
          hdr.name,e->chan,e->addr.sin_port,inet_ntoa(e->addr.sin_addr));
      }
      if ( m_bounce )  {
        int sc = send(buff,e->header.size,e,e->header.facility);
        if ( sc != NET_SUCCESS )  {
          printf("Failed to send message. ret=%d\n",sc);
        }
      }
    }
  };
}
static std::string host_name()  {
  char host[64];
  ::gethostname(host,sizeof(host));
  hostent* h = gethostbyname(host);
  const char* add = inet_ntoa(*(in_addr*)h->h_addr_list[0]);
  return add;
}
extern "C" int net_send(int argc, char **argv)  {
  char *wmessage;
  int count=1, wsize, length=256, loop=100000;
  unsigned int facility=1;
  std::string target = "RCV_0", name="SND_0";

  RTL::CLI cli(argc, argv, help);
  bool bounce = cli.getopt("bounce",1) != 0;
  cli.getopt("count",1,count);
  cli.getopt("length",1,length);
  cli.getopt("facility",1,facility);
  cli.getopt("turns",2,loop);
  cli.getopt("target",2,target);
  cli.getopt("name",1,name);

  count= bounce ? 1 : count;
  //loop = bounce ? 1 : loop;
  std::string node = host_name(); // RTL::nodeNameShort()
  std::string proc = node+"::"+name;
  std::string to   = node+"::"+target;

  ::printf (" Starting net test sender task: %d facility:%d turns:%d\n",
    length,facility,loop);

  while(count-- > 0)  {
    NetSensor c1(proc,bounce);
    if ( length<=0 ) length=10;
    wmessage = new char[length];

    // receive some messages and bounce them
    for (int i=0, mx=loop; mx > 0; --mx, ++i)  {
      wsize =  length;
      for (int k = 0; k < length; k++)
        wmessage[k] = (length + k) & 0xFF;
      NetErrorCode sc = c1.send(wmessage, wsize, to, facility);
      if (sc != NET_SUCCESS)
        printf("Client::send Failed: Error=%d\n",sc);
      if (i % 1000 == 0) printf ("%ld Sent %d messages\n",time(0),i);
    }
    if ( bounce )  {
      while(1)  {
        ::lib_rtl_sleep(100);
      }
    }
  }
  printf("Hit key+Enter to exit ...");
  getchar();
  return 0x1;
}

extern "C" int net_recv(int argc, char **argv)  {
  RTL::CLI cli(argc, argv, help_recv);
  bool bounce = cli.getopt("bounce",1) != 0;
  std::string name = "RCV_0";
  std::string proc = host_name()+"::"+name;
  printf (" Starting net receiver task:%s. Bounce:%s\n",
    proc.c_str(),bounce ? "true" : "false");
  NetSensor cl(proc, bounce);
  while(1)  {
    ::lib_rtl_sleep(100);
  }
  return 0x1;
}
