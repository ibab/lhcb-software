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
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <process.h>

extern "C" int console_read_test(int, char**)  {
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); 
  DWORD cNumRead, fdwMode, fdwSaveOldMode; 
  INPUT_RECORD irInBuf; 
  if (! GetConsoleMode(hStdin, &fdwSaveOldMode) ) 
    printf("GetConsoleMode"); 

  // Enable the window and mouse input events. 
  fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT; 
  if (! SetConsoleMode(hStdin, fdwMode) ) 
    printf("SetConsoleMode"); 
  while (1) {
    // Wait for the events. 
    if (! ReadConsoleInput( 
      hStdin,      // input buffer handle 
      &irInBuf,     // buffer to read into 
      1,         // size of read buffer 
      &cNumRead) ) // number of records read 
      printf("ReadConsoleInput"); 
    switch(irInBuf.EventType) 
    { 
    case KEY_EVENT: // keyboard input 
      printf("KEY_EVENT\n");
      break; 

    case MOUSE_EVENT: // mouse input 
      printf("MOUSE_EVENT\n");
      break; 

    case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing 
      printf("WINDOW_BUFFER_SIZE_EVENT\n");
      break; 

    case FOCUS_EVENT:  // disregard focus events 
      printf("FOCUS_EVENT\n");

    case MENU_EVENT:   // disregard menu events 
      printf("MENU_EVENT\n");
      break; 

    default: 
      printf("unknown event type"); 
      break; 
    } 
  }
}

extern "C" int console_read_test2(int, char**)  {
  char c=0;
  printf( "do not forget to execute: 'stty -icanon -echo'" );
  fflush(stdout);
  int fd = fileno(stdin);
  printf("Get console IO...type q to quit\n\n\n");
  fflush(stdout);
  while(c!='q') {
    read(fd,&c,1);
    printf("Got char:%02X\n",c);
    fflush(stdout);
  }
  printf( "do not forget to execute: 'stty icanon echo'" );
  return 1;
}
#endif

namespace {
  struct PortEntry  {
    int type;
    int (*callback)(void*);
    void* param;
    int armed;
  };

  class EntryMap : public std::map<__NetworkChannel__,PortEntry*> {
  protected:
    lib_rtl_thread_t m_thread;
    __NetworkPort__  m_port;
  public:
    static int threadCall(void* param);
    static int consoleCall(void* param);
    EntryMap(__NetworkPort__ p);
    int handle();
    int run();
  };

  int EntryMap::threadCall(void* param)  {
    return ((EntryMap*)param)->handle();
  }
  int s_fdPipe[2], s_fdPipeBytes=0;
  int EntryMap::consoleCall(void* param)  {
    EntryMap* m = (EntryMap*)param;
    char ch;
#ifdef _WIN32
    int status = _pipe(s_fdPipe,1024,O_BINARY);
#endif
    while(1)  {
      ::read(fileno(stdin),&ch,1);
      write(s_fdPipe[1],&ch,1);
      s_fdPipeBytes++;
      //printf("Wait for hit...\n");
      if ( ch == -1 ) continue;
      // printf("Got hit:%02X !!\n",ch);
      for(iterator i=m->begin(); i != m->end(); ++i)  {
        PortEntry* e = (*i).second;
        if ( e && e->armed )  {
          e->armed = 0;
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
        if ( (*i).second->armed )  {
          __NetworkChannel__ fd = (*i).first;
          if ( fd > mxsock ) mxsock = fd;
          FD_SET(fd, &read_fds);
          FD_SET(fd, &exc_fds);
          channels[nsock] = fd;
          nsock++;
        }
      }
      if ( nsock > 0 )  {
        timeval tv = { 0, 1000 };
        int res = select(mxsock+1, &read_fds, 0, &exc_fds, &tv);
        if ( res == 0 )  {
          continue;
        }
        else if (res < 0)  {
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
              int t = e->type, nb = IOPortManager(m_port).getAvailBytes(fd);
              // ::printf("got read request: %d bytes!\n",nb);
              if ( e->callback )   {
                if ( !(nb==0 && fd == fileno(stdin)) )
                  e->armed = 0;
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

typedef std::map<int,EntryMap* > PortMap;

static inline PortMap& portMap()  {
  static PortMap* s_map = (PortMap*)lib_rtl_alloc_int_pointer_map();
  return *s_map;
}
int IOPortManager::getChar(int fd, char* c)  {
#ifdef _WIN32
  if ( fd == fileno(stdin) )  {
    fd = s_fdPipe[0];
    if ( 1 == ::read(fd,c,1) ) s_fdPipeBytes--;
    return 1;
  }
#endif
  return ::read(fd,c,1);
}

int IOPortManager::getAvailBytes(int fd)  {
#ifdef _WIN32
  if ( fd == fileno(stdin) ) return s_fdPipeBytes;
#endif
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
  e->armed = 1;
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

