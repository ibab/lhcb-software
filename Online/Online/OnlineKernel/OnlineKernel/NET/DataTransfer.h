#ifndef ONLINEKERNEL_NET_DATATRANSFER_H
#define ONLINEKERNEL_NET_DATATRANSFER_H

extern "C" {
  struct NET;
  struct netheader_t;
  struct netentry_t;

  enum NetConnectionType  {
    NET_CLIENT = 1,
    NET_SERVER = 2
  };
  struct netheader_t {
    unsigned int    size;
    unsigned int    msg_type;
    unsigned int    facility;
    unsigned int    magic;
    unsigned int    hash;
    char            name[128];
    //netheader_t() : size(0), msg_type(NET_MSG_DATA), facility(0), magic(NET_MAGIC), hash(0) {}
    //~netheader_t() {}
    //void net_to_host();
    //void fill(const char* nam, unsigned int hash, size_t size, int fac, int mtype, const void *buf_ptr);
  };

  typedef void (*net_handler_t)(netentry_t* e, const netheader_t& hdr, void* param);
  NET* network_init(const char* proc, NetConnectionType type=NET_SERVER);
  void network_close(NET* net);
  void* network_lock(NET* net);
  void network_unlock(NET* net, void* lock);
  int network_subscribe(NET* net, void* param, unsigned int fac, net_handler_t data, net_handler_t death);
  int network_unsubscribe(NET* net, void* param, unsigned int fac);
  int network_receive(NET* net, netentry_t* e, void* buff);
  int network_send(NET* net, const void* buff, size_t size, const char* dest, unsigned int fac);
  int network_send2(NET* net, const void* buff, size_t size, netentry_t* dest, unsigned int fac);
}

#include <string>

namespace DataTransfer  {
  enum NetErrorCode {
    NET_ERROR          =  0,
    NET_SUCCESS        =  1,
    NET_TASKNOTFOUND   =  2,
    NET_TERRIBLE       =  4,
    NET_NODATA         =  6,
    NET_ILLEGAL_LENGTH =  8,
    NET_UNKNOWNMODE    = 10,
    NET_CONNCLOSED     = 12,  // Also message type
    NET_TASKDIED       = 14,  // Also message type
    NET_MSG_DATA       = 16,  // Also message type
    NET_MAGIC          =  0xDEADCAFE
  };
  using ::NET_CLIENT;
  using ::NET_SERVER;

  using ::NET;
  using ::netheader_t;
  using ::netentry_t;
  using ::net_handler_t;
  inline NET* net_init(const std::string& proc, NetConnectionType type=NET_SERVER) 
    {   return network_init(proc.c_str(),type); }
  inline void net_close(NET* net) {  network_close(net); }
  inline void* net_lock(NET* net) {  return network_lock(net); }
  inline void net_unlock(NET* net, void* lock) {  return network_unlock(net,lock); }
  inline int net_subscribe(NET* net, void* param, unsigned int fac, net_handler_t data, net_handler_t death)
    { return network_subscribe(net,param,fac,data,death);  }
  inline int net_unsubscribe(NET* net, void* param, unsigned int fac)
    { return network_unsubscribe(net,param,fac);  }
  inline int net_receive(NET* net, netentry_t* e, void* buff)
    { return network_receive(net,e,buff);  }
  inline int net_send(NET* net, const void* buff, size_t size, const std::string& dest, unsigned int fac)
    { return network_send(net,buff,size,dest.c_str(),fac);  }
  inline int net_send(NET* net, const void* buff, size_t size, netentry_t* dest, unsigned int fac)
    { return network_send2(net,buff,size,dest,fac);  }
}
#endif  /* ONLINEKERNEL_NET_DATATRANSFER_H */
