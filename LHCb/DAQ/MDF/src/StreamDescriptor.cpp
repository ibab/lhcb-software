//====================================================================
//	OnlineEvtSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : LHCb
//
//	Author     : M.Frank
//====================================================================
// $Id: StreamDescriptor.cpp,v 1.1 2005-12-20 16:33:39 frankb Exp $

// Include files
#include "MDF/StreamDescriptor.h"
#include <fcntl.h>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <exception>
#ifdef _WIN32
  #include <io.h>
  #include <sys/stat.h>
  static const int S_IRWXU = (S_IREAD|S_IWRITE);
  #define lseek64 _lseeki64
#else
  #include <ctype.h>
  #include <unistd.h>
  static const int O_BINARY = 0;
#endif

namespace Networking {
#ifdef _WIN32
  #include "Winsock2.h"
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
  typedef char SockOpt_t;
  typedef int  AddrLen_t;
#else
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  int (*closesocket)(int) = ::close;
  typedef int SockOpt_t;
  typedef size_t AddrLen_t;
#endif
  static const int _SOCK_STREAM = SOCK_STREAM;
  static const int _IPPROTO_IP  = IPPROTO_IP;
}
namespace FileIO {
  using ::close;
  using ::open;
  using ::read;
  using ::write;
  using ::lseek64;
}


namespace {
  typedef LHCb::StreamDescriptor::Access Access;
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
      if ( sc > 0 ) tmp -= sc;
      else          return false;
    }
    return true;
  }
  longlong file_tell(const Access& con)  {
    return FileIO::lseek64(con.ioDesc,0,SEEK_CUR);
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
  longlong ip_tell(const Access& /* con */)  {
    return -1;
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

void LHCb::StreamDescriptor::getFileConnection(const std::string& con, std::string& file)  {
  size_t idx = con.find("://");
  file = "";
  if ( idx != std::string::npos )  {
    file = con.substr(idx+3);
  }
}

void LHCb::StreamDescriptor::getInetConnection(const std::string& con, 
                                                      std::string& host,
                                                      Networking::in_addr* ip,
                                                      unsigned short& port)
{
  getFileConnection(con, host);
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

LHCb::StreamDescriptor::Access 
LHCb::StreamDescriptor::connect(const std::string& specs)  {
  Access result;
  std::string file;
  Networking::sockaddr_in sin;
  result.type = ::toupper(specs[0]);
  switch(result.type) {
    case 'F':          //  DATA='file://C:/Data/myfile.dat'
      getFileConnection(specs, file);
      result.ioDesc     = FileIO::open(file.c_str(), O_WRONLY|O_BINARY|O_CREAT, S_IRWXU );
      result.m_write    = file_write;
      result.m_read     = file_read;
      result.m_tell     = file_tell;
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
          result.m_tell     = ip_tell;
          return result;
        }
        Networking::closesocket(result.ioDesc);
        result.ioDesc = -1;
        break;
      }
      break;
    default:
      break;
  }
  return result;
}

LHCb::StreamDescriptor::Access 
LHCb::StreamDescriptor::bind(const std::string& specs)  {
  Access result;
  std::string file;
  Networking::sockaddr_in sin;
  result.type = ::toupper(specs[0]);
  switch(result.type) {
    case 'F':          //  DATA='file://C:/Data/myfile.dat'
      getFileConnection(specs, file);
      result.ioDesc     = FileIO::open(file.c_str(), O_RDONLY|O_BINARY );
      result.m_write    = file_write;
      result.m_read     = file_read;
      result.m_tell     = file_tell;
      break;
    case 'I':          //  DATA='ip://137.138.142.82:8000'
      result.ioDesc = Networking::socket(AF_INET,Networking::_SOCK_STREAM,Networking::_IPPROTO_IP);
      if ( result.ioDesc > 0 )   {
        int opt = 1;
        getInetConnection(specs, file, &sin.sin_addr, sin.sin_port);
        Networking::setsockopt(result.ioDesc,SOL_SOCKET,SO_REUSEADDR,(Networking::SockOpt_t*)&opt,sizeof(opt));
        sin.sin_family = AF_INET;
        if ( Networking::bind(result.ioDesc, (Networking::sockaddr*)&sin, sizeof(sin)) == 0) {
          if ( Networking::listen(result.ioDesc, SOMAXCONN) == 0 )  {
            result.m_write    = ip_send;
            result.m_read     = ip_recv;
            result.m_tell     = ip_tell;
            return result;
          }
        }
        Networking::closesocket(result.ioDesc);
        result.ioDesc = -1;
      }
      break;
    default:
      break;
  }
  return result;
}

int LHCb::StreamDescriptor::close(Access& specs) {
  int dsc = specs.ioDesc; 
  specs.ioDesc = -1; 
  switch(specs.type)  {
    case 'F':
      return dsc > 0 ? FileIO::close(dsc) : 0;
    case 'I':
      return dsc > 0 ? Networking::closesocket(dsc) : 0;
    default:
      return false;
  }
}

LHCb::StreamDescriptor::Access 
LHCb::StreamDescriptor::accept(const Access& specs)  {
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
          Networking::setsockopt(result.ioDesc,SOL_SOCKET,SO_REUSEADDR,(Networking::SockOpt_t*)&opt,sizeof(opt));
	      }
      }
      break;
    default:
      result.ioDesc = -1;
      break;
  }
  return result;
}
