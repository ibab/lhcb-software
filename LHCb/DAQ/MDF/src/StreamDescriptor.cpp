//====================================================================
//	OnlineEvtSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : LHCb
//
//	Author     : M.Frank
//====================================================================
// $Id: StreamDescriptor.cpp,v 1.9 2007-01-24 19:21:50 frankb Exp $

// Include files
#include "MDF/StreamDescriptor.h"
#include <fcntl.h>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <exception>
#include <map>

#ifdef _WIN32
  #include <io.h>
  #include <sys/stat.h>
  static const int S_IRWXU = (S_IREAD|S_IWRITE);
  static const int S_IRWXG = 0;
  #define lseek64 _lseeki64
#else
  #include <ctype.h>
  #include <unistd.h>
  static const int O_BINARY = 0;
#endif

namespace Networking {
#ifdef _WIN32
  #include "Winsock2.h"
  typedef char SockOpt_t;
  typedef int  AddrLen_t;
  struct __init__ {
    __init__()  {
      static bool g_first = true;
      if ( g_first )  {
        g_first = false;
        static WSADATA g_WSAData;
        memset(&g_WSAData, 0, sizeof(WSADATA));
        if (WSAStartup ( MAKEWORD(1,1), &g_WSAData) != 0)    {
          std::runtime_error("MDF::StreamDescriptor> WSAStartup failed!");
        }
      }
    }
  };
  static __init__ g_init;

#else
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  int (*closesocket)(int) = ::close;
  typedef int SockOpt_t;
  typedef socklen_t AddrLen_t;
#endif
  static const int _SOCK_STREAM = SOCK_STREAM;
  static const int _IPPROTO_IP  = IPPROTO_IP;
}
#include "MDF/PosixIO.h"
#include "GaudiKernel/System.h"
namespace FileIO {
  using ::close;
  using ::open;
  using ::read;
  using ::write;
  using ::lseek64;
}
typedef LHCb::StreamDescriptor::Access Access;

namespace {
  bool file_read(const Access& con, void* buff, int len)  {
    int tmp = 0;
    char* p = (char*)buff;
    while ( tmp < len )  {
      int sc = FileIO::read(con.ioDesc,p+tmp,len-tmp);
      if ( sc > 0 ) tmp += sc;
      else          return false;
    }
    return true;
  }
  bool file_write(const Access& con, const void* data, int len)  {
    const char* p = (const char*)data;
    int tmp = len;
    while ( tmp>0 )  {
      int sc = FileIO::write(con.ioDesc,p+len-tmp,tmp);
      // std::cout << "Write:" << con.ioDesc << "  " << sc << " bytes." << std::endl;
      if ( sc > 0 ) tmp -= sc;
      else          return false;
    }
    return true;
  }
  long long file_seek(const Access& con, long long offset, int where)  {
    return FileIO::lseek64(con.ioDesc,offset,where);
  }
  bool ip_recv(const Access& con, void* buff, int len)  {
    int tmp = 0;
    char* p = (char*)buff;
    while ( tmp < len )  {
      int sc = Networking::recv(con.ioDesc,p+tmp,len-tmp,0);
      if ( sc > 0 ) tmp += sc;
      else          return false;
    }
    return true;
  }
  bool ip_send(const Access& con, const void* data, int len)  {
    char* p = (char*)data;
    int tmp = len;
    while ( tmp>0 )  {
      int sc = Networking::send(con.ioDesc,p+len-tmp,tmp,0);
      if ( sc > 0 ) tmp -= sc;
      else          return false;
    }
    return true;
  }
  long long ip_seek(const Access& /* con */, long long /* offset */, int /* where */)  {
    return -1;
  }
  bool posix_read(const Access& con, void* buff, int len)  {
    if ( con.ioFuncs && con.ioFuncs->read )  {
      int tmp = 0;
      char* p = (char*)buff;
      while ( tmp < len )  {
        int sc = con.ioFuncs->read(con.ioDesc,p+tmp,len-tmp);
        if ( sc > 0 ) tmp += sc;
        else          return false;
      }
      return true;
    }
    return false;
  }
  bool posix_write(const Access& con, const void* data, int len)  {
    if ( con.ioFuncs && con.ioFuncs->write )  {
      const char* p = (const char*)data;
      int tmp = len;
      while ( tmp>0 )  {
        int sc = con.ioFuncs->write(con.ioDesc,p+len-tmp,tmp);
        // std::cout << "Write:" << con.ioDesc << "  " << sc << " bytes." << std::endl;
        if ( sc > 0 ) tmp -= sc;
        else          return false;
      }
      return true;
    }
    return false;
  }
  long long posix_seek(const Access& con, long long offset, int where)  {
    if ( con.ioFuncs && con.ioFuncs->lseek64 )  {
      return con.ioFuncs->lseek64(con.ioDesc,offset,where);
    }
    return -1;
  }
  LHCb::PosixIO* getIOModule(const std::string& proto)    {
    typedef std::map<std::string,LHCb::PosixIO*> _M;
    static _M modules;
    std::string mod = "MDF_"+proto;
    for(size_t i=0; i<mod.length();++i) mod[i] = ::toupper(mod[i]);
    _M::const_iterator itmod = modules.find(mod);
    LHCb::PosixIO* io = 0;
    if ( itmod == modules.end() )  {
      void* hdl = 0;
      long status = System::loadDynamicLib(mod, &hdl);
      if ( status&1 )   {
        LHCb::PosixIO* (*func)() = 0;
        status = System::getProcedureByName(hdl,mod,(System::Creator*)&func);
        io = (func)();
        if ( io )  {
          modules.insert(std::make_pair(mod,io));
          return io;
        }
      }
      return 0;
    }
    return (*itmod).second;
  }
}

// Standard Constructor
LHCb::StreamDescriptor::StreamDescriptor()
: m_allocated(0), m_length(0), m_data(0)
{
}

// Standard Destructor
LHCb::StreamDescriptor::~StreamDescriptor()  {
  allocate(0);
}

/// Allocate data block
char* LHCb::StreamDescriptor::allocate(int len)  {
  m_length = 0;
  if ( len > 0 )  {
    if ( len < m_allocated )  {
      return m_data;
    }
    if ( m_data ) delete [] m_data;
    m_data = new char[m_allocated = len];
    return m_data;
  }
  if ( m_data ) delete [] m_data;
  m_allocated = 0;
  m_data = 0;
  return m_data;
}

void LHCb::StreamDescriptor::getFileConnection(const std::string& con, 
                                               std::string& file, 
                                               std::string& proto)
{
  size_t idx = con.find("://");
  if ( idx != std::string::npos )  {
    file = con.substr(idx+3);
    proto = con.substr(0,idx);
    return;
  }
  file = proto = "";
}

void LHCb::StreamDescriptor::getInetConnection( const std::string& con, 
                                                std::string& host,
                                                Networking::in_addr* ip,
                                                unsigned short& port)
{
  std::string proto;
  getFileConnection(con, host, proto);
  size_t idx = host.find(":");
  ip->s_addr = port = 0;
  if ( idx != std::string::npos )  {
    std::string prt = host.substr(idx+1);
    ::sscanf(prt.c_str(),"%hd",&port);
#ifdef ntohs
    port = ntohs(port);
#else
    port = Networking::ntohs(port);
#endif
    host = host.substr(0,idx);
  }
  if ( ::isalpha(host[0]) )  {
    Networking::hostent* h = Networking::gethostbyname(host.c_str());
    if ( h )  {
      if ( h->h_addr_list[0] )  {
        ip->s_addr = *(unsigned long*)h->h_addr_list[0];
      }
    }
  }
  else {
    ip->s_addr = Networking::inet_addr(host.c_str());
  }
}

Access LHCb::StreamDescriptor::connect(const std::string& specs)  {
  Access result;
  std::string file, proto;
  Networking::sockaddr_in sin;
  result.type = ::toupper(specs[0]);
  switch(result.type) {
    case 'F':          //  DATA='file://C:/Data/myfile.dat'
      getFileConnection(specs, file, proto);
      result.ioDesc     = FileIO::open(file.c_str(), O_WRONLY|O_BINARY|O_CREAT, S_IRWXU|S_IRWXG );
      result.m_write    = file_write;
      result.m_read     = file_read;
      result.m_seek     = file_seek;
      break;
    case 'I':          //  DATA='ip://137.138.142.82:8000'
      /*
        struct sockaddr_in {
        short   sin_family;
        u_short sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8];
        };
      */
      getInetConnection(specs, file, &sin.sin_addr, sin.sin_port);
      result.ioDesc = Networking::socket(AF_INET, Networking::_SOCK_STREAM, Networking::_IPPROTO_IP);
      if ( result.ioDesc > 0 )   {        
        sin.sin_family      = AF_INET;
        ::memset(sin.sin_zero,0,sizeof(sin.sin_zero));
        int ret = Networking::connect(result.ioDesc, (Networking::sockaddr*)&sin, sizeof(sin));
        if ( ret == 0 )  {
          result.m_write    = ip_send;
          result.m_read     = ip_recv;
          result.m_seek     = ip_seek;
          return result;
        }
        Networking::closesocket(result.ioDesc);
        result.ioDesc = -1;
        break;
      }
      break;
    default:           //  DATA='rfio:/castor/cern.ch/......
      getFileConnection(specs, file, proto);
      if ( !proto.empty() )  {
        PosixIO* io = getIOModule(proto);
        if ( io && io->open && io->close && io->write && io->read && io->lseek64 )  {
          result.ioFuncs    = io;
          result.ioDesc     = io->open(file.c_str(), O_RDONLY|O_BINARY, S_IRWXU);
          result.m_write    = posix_write;
          result.m_read     = posix_read;
          result.m_seek     = posix_seek;
          return result;
        }
      }
      break;
  }
  return result;
}

Access LHCb::StreamDescriptor::bind(const std::string& specs)  {
  Access result;
  std::string file, proto;
  Networking::sockaddr_in sin;
  result.type = ::toupper(specs[0]);
  switch(result.type) {
    case 'F':          //  DATA='file://C:/Data/myfile.dat'
      getFileConnection(specs, file, proto);
      result.ioDesc  = FileIO::open(file.c_str(), O_RDONLY|O_BINARY );
      result.m_write = file_write;
      result.m_read  = file_read;
      result.m_seek  = file_seek;
      break;
    case 'I':          //  DATA='ip://137.138.142.82:8000'
      result.ioDesc = Networking::socket(AF_INET,Networking::_SOCK_STREAM,Networking::_IPPROTO_IP);
      if ( result.ioDesc > 0 )   {
        int opt = 1;
        getInetConnection(specs, file, &sin.sin_addr, sin.sin_port);
        Networking::setsockopt(result.ioDesc,SOL_SOCKET,SO_REUSEADDR,
                              (Networking::SockOpt_t*)&opt,sizeof(opt));
        sin.sin_family = AF_INET;
        if ( Networking::bind(result.ioDesc, (Networking::sockaddr*)&sin, sizeof(sin)) == 0) {
          if ( Networking::listen(result.ioDesc, SOMAXCONN) == 0 )  {
            result.m_write = ip_send;
            result.m_read  = ip_recv;
            result.m_seek  = ip_seek;
            return result;
          }
        }
        Networking::closesocket(result.ioDesc);
        result.ioDesc = -1;
      }
      break;
    default:
      getFileConnection(specs, file, proto);
      if ( !proto.empty() )  {
        PosixIO* io = getIOModule(proto);
        if ( io && io->open && io->close && io->write && io->read && io->lseek64 )  {
          result.ioFuncs    = io;
          result.ioDesc     = io->open(file.c_str(), O_RDONLY|O_BINARY, S_IREAD);
          if ( result.ioDesc == -1 && io->serror )  {
            //const char* msg = io->serror();
            //std::cout << "Error connection POSIX IO:" << file << std::endl
            //          << (char*)(msg ? msg : "Unknown error") << std::endl;
          }
          result.m_write    = posix_write;
          result.m_read     = posix_read;
          result.m_seek     = posix_seek;
          return result;
        }
      }
      break;
  }
  return result;
}

int LHCb::StreamDescriptor::close(Access& c) {
  int dsc = c.ioDesc; 
  c.ioDesc = -1; 
  switch(c.type)  {
    case 'F':
      return dsc > 0 ? FileIO::close(dsc) : 0;
    case 'I':
      return dsc > 0 ? Networking::closesocket(dsc) : 0;
    default:
      return dsc > 0 && c.ioFuncs != 0 && c.ioFuncs->close != 0 ? c.ioFuncs->close(dsc) : 0;
  }
  return false;
}

Access LHCb::StreamDescriptor::accept(const Access& specs)  {
  Access result = specs;
  switch(specs.type)  {
    case 'F':
      result.ioDesc = specs.ioDesc;
      break;
    case 'I':
      if ( specs.ioDesc > 0 )  {
        Networking::sockaddr sin;
        Networking::AddrLen_t len = sizeof(sin);
        ::memset(&sin,0,len);
        result.ioDesc = Networking::accept(specs.ioDesc, &sin, &len);
        if ( result.ioDesc > 0 ) {
          int opt = 1;
          Networking::setsockopt(result.ioDesc,SOL_SOCKET,SO_REUSEADDR,
                                 (Networking::SockOpt_t*)&opt,sizeof(opt));
	      }
      }
      break;
    default:
      result.ioDesc = specs.ioDesc;
      break;
  }
  return result;
}
