#ifndef ONLINEKERNEL_NET_IOPORTMANAGER_H
#define ONLINEKERNEL_NET_IOPORTMANAGER_H

#include "NET/NetworkChannel.h"

class IOPortManager  {
  __NetworkPort__ m_port;  
public:
  IOPortManager(NetworkChannel::Port p): m_port(p)  {}
  int add(int typ, NetworkChannel::Channel c, int (*callback)(void*), void* param);
  int remove(NetworkChannel::Channel c);
};
struct PortEntry  {
  int type;
  int (*callback)(void*);
  void* param;
};
#endif // ONLINEKERNEL_NET_IOPORTMANAGER_H
