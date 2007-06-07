#ifndef ONLINEKERNEL_NET_IOPORTMANAGER_H
#define ONLINEKERNEL_NET_IOPORTMANAGER_H

#include "NET/NetworkChannel.h"

class IOPortManager  {
  __NetworkPort__ m_port;  
public:
  IOPortManager(NetworkChannel::Port p): m_port(p)  {}
  void setPort(NetworkChannel::Port p)  {  m_port = p;    }
  __NetworkPort__ port() const          {  return m_port; }
  void stop(void* entry);
  void join(void* entry);
  int add(int typ, NetworkChannel::Channel c, int (*callback)(void*), void* param);
  int remove(NetworkChannel::Channel c, bool need_lock=true);
  void* lock();
  void unlock(void* entry);
  static int getAvailBytes(int fd);
  static int getChar(int fd, char* c);
};
#endif // ONLINEKERNEL_NET_IOPORTMANAGER_H
