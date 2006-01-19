#include "RTL/rtl.h"
#include "NET/IOPortManager.h"
#include <map>
#include <vector>
#include <stdexcept>

class EntryMap : public std::map<__NetworkChannel__,PortEntry*> {
protected:
  lib_rtl_thread_t m_thread;
  __NetworkPort__  m_port;
public:
  static int threadCall(void* param);
  EntryMap(__NetworkPort__ p);
  int handle();
  int run();
  unsigned int getAvailBytes(int fd)  {
    unsigned long ret;
    if (ioctlsocket(fd, FIONREAD, &ret) != -1)
      return ret;
    else    {
      if (errno == EINVAL) // Server socket
        return -1;
      return 0;
    }
  }
};

int EntryMap::threadCall(void* param)  {
  return ((EntryMap*)param)->handle();
}
int EntryMap::handle()  {
  std::vector<__NetworkChannel__> channels;
  while(1)  {
    int nsock = 0;
    size_t len = size();
    if(channels.size()<len) channels.resize(len+32);
    fd_set read_fds;
    FD_ZERO(&read_fds);
    for(iterator i=begin(); i != end(); ++i)  {
      __NetworkChannel__ fd = (*i).first;
      FD_SET(fd, &read_fds);
      channels[nsock] = fd;
      nsock++;
    }
    if ( nsock > 0 )  {
      int res = select(nsock, &read_fds, 0, 0, 0);
      if (res < 0)  {
        return res;
      }
    }
    else  {
      timeval tv = { 0, 1000 };
      ::select(nsock, &read_fds, 0, 0, &tv);
      continue;
    }
    for ( int j=0; j<nsock; ++j )  {
      __NetworkChannel__ fd = channels[j];      
      if (FD_ISSET(fd, &read_fds))  {
        iterator k=find(fd);
        if ( k != end() )  {
          PortEntry* e = (*k).second;
          if ( e )  {
            int t = e->type, nb = getAvailBytes(fd);
            // ::printf("got read request: %d bytes!\n",nb);
            if ( e->callback ) (*e->callback)(e->param);
            if ( t == 1 && nb <= 0 )  {
              k = find(fd);
              if ( k != end() )  {
                if ( (*k).second ) delete (*k).second;
                erase(k);
              }
            }
          }
        }
      }
    }
  }
}

EntryMap::EntryMap(__NetworkPort__ p) : m_port(0), m_thread(0) {
}

int EntryMap::run()  {
  if ( !m_thread )  {
    int sc = lib_rtl_start_thread(threadCall, this, &m_thread);
    if ( !lib_rtl_is_success(sc) )  {
      ::printf("Failed to create port-thread");
      throw std::runtime_error("Failed to create port-thread");
    }
  }
  return 1;
}

typedef std::map<__NetworkPort__,EntryMap* > PortMap;
static inline PortMap& portMap()  {
  static PortMap s_map;
  return s_map;
}

int IOPortManager::add(int typ, NetworkChannel::Channel c, int (*callback)(void*), void* param)  {
  EntryMap* em = portMap()[m_port];
  if ( !em ) portMap()[m_port] = em = new EntryMap(m_port);
  PortEntry* e = (*em)[c];
  if ( !e ) (*em)[c] = e = new PortEntry;
  e->callback = callback;
  e->param = param;
  e->type = typ;
  return em->run();
}

int IOPortManager::remove(NetworkChannel::Channel c)  {
  EntryMap* em = portMap()[m_port];
  if ( em ) {
    EntryMap::iterator i = em->find(c);
    if ( i != em->end() )  {
      if ( (*i).second ) delete (*i).second;
      em->erase(i);
    }
  }
  return 1;
}

