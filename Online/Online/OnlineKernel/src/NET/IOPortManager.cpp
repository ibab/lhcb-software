#include "RTL/rtl.h"
#include "NET/IOPortManager.h"
#include <map>
#include <vector>
#include <stdexcept>

#ifdef __linux
#include <sys/ioctl.h>
#define ioctlsocket ioctl

#elif _WIN32
#include <conio.h>
#endif

namespace {
  struct PortEntry  {
    int type;
    int (*callback)(void*);
    void* param;
  };

  class EntryMap : public std::map<__NetworkChannel__,PortEntry*> {
  protected:
    lib_rtl_thread_t m_thread;
    __NetworkPort__  m_port;
  public:
    static int threadCall(void* param);
    static int consoleCall(void* param);
    EntryMap(__NetworkPort__ p);
    virtual int handle();
    int run();
  };

  int EntryMap::threadCall(void* param)  {
    return ((EntryMap*)param)->handle();
  }
  int EntryMap::consoleCall(void* param)  {
    EntryMap* m = (EntryMap*)param;
    int ch;
    while(1)  {
      lib_rtl_sleep(10);
#ifdef _WIN32
      ch = getch();
      ungetch(ch);
#endif
      //printf("Wait for hit...\n");
      if ( ch == -1 ) continue;
      // printf("Got hit:%02X !!\n",ch);
      for(iterator i=m->begin(); i != m->end(); ++i)  {
        PortEntry* e = (*i).second;
        if ( e )  {
          if ( e->callback ) (*e->callback)(e->param);
        }
      }
    }
  }
  int EntryMap::handle()  {
    std::vector<__NetworkChannel__> channels;
    while(1)  {
      int nsock = 0, mxsock = 0;
      size_t len = size();
      if(channels.size()<len) channels.resize(len+32);
      fd_set read_fds, exc_fds;
      FD_ZERO(&exc_fds);
      FD_ZERO(&read_fds);
      for(iterator i=begin(); i != end(); ++i)  {
        __NetworkChannel__ fd = (*i).first;
        if ( fd > mxsock ) mxsock = fd;
        FD_SET(fd, &read_fds);
        FD_SET(fd, &exc_fds);
        channels[nsock] = fd;
        nsock++;
      }
      if ( nsock > 0 )  {
        int res = select(mxsock+1, &read_fds, 0, &exc_fds, 0);
        if (res < 0)  {
          return res;
        }
      }
      else  {
        timeval tv = { 0, 1000 };
        ::select(nsock, 0, 0, 0, &tv);
        continue;
      }
      for ( int j=0; j<nsock; ++j )  {
        __NetworkChannel__ fd = channels[j];      
        if (FD_ISSET(fd, &read_fds))  {
          iterator k=find(fd);
          if ( k != end() )  {
            PortEntry* e = (*k).second;
            if ( e )  {
              int t = e->type, nb = IOPortManager::getAvailBytes(fd);
              // ::printf("got read request: %d bytes!\n",nb);
              if ( e->callback )   {
		if ( !(nb==0 && fd == fileno(stdin)) )
		  (*e->callback)(e->param);
	      }
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

  EntryMap::EntryMap(__NetworkPort__ p) : m_thread(0), m_port(p) {
  }

  int EntryMap::run()  {
    if ( !m_thread )  {
      int (*call)(void*);
      switch(m_port)  {
        case 0:
#ifdef _WIN32
          call = consoleCall;
          break;
#endif
        default: 
          //printf("Installing thread call!\n");
          call = threadCall;
          break;
      }
      int sc = lib_rtl_start_thread(call, this, &m_thread);
      if ( !lib_rtl_is_success(sc) )  {
        ::printf("Failed to create port-thread\n");
        throw std::runtime_error("Failed to create port-thread");
      }
    }
    return 1;
  }
}

typedef std::map<__NetworkPort__,EntryMap* > PortMap;
static inline PortMap& portMap()  {
  static PortMap s_map;
  return s_map;
}

int IOPortManager::getAvailBytes(int fd)  {
  unsigned long ret;
  if (ioctlsocket(fd, FIONREAD, &ret) != -1)
    return int(ret);
  else    {
    if (errno == EINVAL) // Server socket
      return -1;
    return 0;
  }
}

int IOPortManager::add(int typ, NetworkChannel::Channel c, int (*callback)(void*), void* param)  {
  EntryMap* em = portMap()[m_port];
  if ( !em ) {
    //printf("Install port watcher for %d\n",m_port);
    portMap()[m_port] = em = new EntryMap(m_port);
  }
  PortEntry* e = (*em)[c];
  if ( !e ) {
    //printf("Install channel watcher for %d\n",c);
    (*em)[c] = e = new PortEntry;
  }
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

