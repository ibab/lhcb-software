//====================================================================
//	OnlineEvtSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : GaudiOnline
//
//	Author     : M.Frank
//====================================================================
// $Id: StreamDescriptor.cpp,v 1.1.1.1 2005-04-18 15:31:41 frankb Exp $

// Include files
#include "GaudiOnline/StreamDescriptor.h"
#include <io.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <exception>
namespace DAQ {
  #include "DAQ/daq.h"
}

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
          std::runtime_error("GaudiOnline::StreamDescriptor> WSAStartup failed!");
        }
      }
    }
  };
  static __init__ g_init;
#else
#endif
}

namespace {
  typedef GaudiOnline::StreamDescriptor::Access Access;
  bool file_read(Access& con, void* buff, int len)  {
    int tmp = 0;
    char* p = (char*)buff;
    while ( tmp < len )  {
      int sc = ::read(con.ioDesc,p+tmp,len-tmp);
      if ( sc > 0 ) tmp += sc;
      else          return false;
    }
    return true;
  }
  bool file_read_len(Access& con, int& len)  {
    return file_read(con, &len, sizeof(len));
  }
  bool file_write(Access& con, const void* data, int len)  {
    const char* p = (const char*)data;
    int tmp = len;
    while ( tmp>0 )  {
      int sc = ::write(con.ioDesc,p+len-tmp,tmp);
      if ( sc > 0 ) tmp -= sc;
      else          return false;
    }
    return true;
  }
  bool ip_recv(Access& con, void* buff, int len)  {
    int tmp = 0;
    char* p = (char*)buff;
    while ( tmp < len )  {
      int sc = Networking::recv(con.ioDesc,p+tmp,len-tmp,0);
      if ( sc > 0 ) tmp += sc;
      else          return false;
    }
    return true;
  }
  bool ip_recv_len(Access& con, int& len)  {
    return ip_recv(con, &len, sizeof(len));
  }
  bool ip_send(Access& con, const void* data, int len)  {
    char* p = (char*)data;
    int tmp = len;
    while ( tmp>0 )  {
      int sc = Networking::send(con.ioDesc,p+len-tmp,tmp,0);
      if ( sc > 0 ) tmp -= sc;
      else          return false;
    }
    return true;
  }
  bool daq_recv(Access& con, void* data, int len)  {
    DAQ::read_buffer rd_buff;
    rd_buff.buffer = (char*)data;
    rd_buff.buffer_len = rd_buff.max_len = len;
    return DAQ::daq_read(&rd_buff) == 0;
  }
  bool daq_recv_len(Access& con, int& len)  {
    return DAQ::daq_read_length(&len) == 0;
  }
  bool daq_send(Access& con, const void* data, int len)  {
    DAQ::send_buffer snd_buff;
    snd_buff.buffer = (char*)data;
    snd_buff.buffer_len = len;
    return DAQ::daq_send(&snd_buff) == 0;
  }
}

// Standard Constructor
GaudiOnline::StreamDescriptor::StreamDescriptor()
: m_allocated(0), m_length(0), m_data(0), m_decision(NONE), m_type(NONE)
{
}

// Standard Destructor
GaudiOnline::StreamDescriptor::~StreamDescriptor()
{
  allocate(0);
}

/// Allocate data block
char* GaudiOnline::StreamDescriptor::allocate(int len)  {
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

void GaudiOnline::StreamDescriptor::getFileConnection(const std::string& con, std::string& file)  {
  size_t idx = con.find("://");
  file = "";
  if ( idx != std::string::npos )  {
    file = con.substr(idx+3);
  }
}

void GaudiOnline::StreamDescriptor::getInetConnection(const std::string& con, 
                                                      std::string& host,
                                                      unsigned int& ip,
                                                      unsigned short& port)
{
  getFileConnection(con, host);
  size_t idx = host.find(":");
  ip = port = 0;
  if ( idx != std::string::npos )  {
    std::string prt = host.substr(idx+1);
    sscanf(prt.c_str(),"%d",&port);
    host = host.substr(0,idx);
  }
  if ( ::isalpha(host[0]) )  {
    Networking::hostent* h = Networking::gethostbyname(host.c_str());
    if ( h )  {
      if ( h->h_addr_list[0] )  {
        ip = *(unsigned int*)h->h_addr_list[0];
      }
    }
  }
  else {
    ip = Networking::inet_addr(host.c_str());
  }
}

GaudiOnline::StreamDescriptor::Access 
GaudiOnline::StreamDescriptor::connect(const std::string& specs)  {
  Access result;
  std::string file;
  Networking::sockaddr_in sin;
  result.type = ::toupper(specs[0]);
  switch(result.type) {
    case 'F':          //  DATA='file://C:/Data/myfile.dat'
      StreamDescriptor::getFileConnection(specs, file);
      result.ioDesc     = ::open(file.c_str(), _O_RDONLY|_O_BINARY );
      result.m_write    = file_write;
      result.m_read     = file_read;
      result.m_read_len = file_read_len;
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
      StreamDescriptor::getInetConnection(specs, file, sin.sin_addr.s_addr, sin.sin_port);
      result.ioDesc = Networking::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
      if ( result.ioDesc > 0 )   {        
        sin.sin_family      = AF_INET;
        ::memset(sin.sin_zero,0,sizeof(sin.sin_zero));
        int ret = Networking::connect(result.ioDesc, (Networking::sockaddr*)&sin, sizeof(sin));
        if ( ret == 0 )  {
          result.m_write    = ip_send;
          result.m_read     = ip_recv;
          result.m_read_len = ip_recv_len;
          return result;
        }
        Networking::closesocket(result.ioDesc);
        result.ioDesc = -1;
        break;
      }
      break;
    case 'S':          //  DATA='sfc://137.138.142.82:8000'
      StreamDescriptor::getInetConnection(specs, file, sin.sin_addr.s_addr, sin.sin_port);
      if ( 0 == DAQ::daq_register(file.c_str(), sin.sin_port) )  {
        result.ioDesc   = 1;
        result.m_write    = daq_send;
        result.m_read     = daq_recv;
        result.m_read_len = daq_recv_len;
        return result;
      }
      break;
    default:
      break;
  }
  return result;
}

GaudiOnline::StreamDescriptor::Access 
GaudiOnline::StreamDescriptor::bind(const std::string& specs)  {
  Access result;
  std::string file;
  Networking::sockaddr_in sin;
  result.type = ::toupper(specs[0]);
  switch(result.type) {
    case 'F':          //  DATA='file://C:/Data/myfile.dat'
      StreamDescriptor::getFileConnection(specs, file);
      result.ioDesc     = ::open(file.c_str(), _O_WRONLY|_O_BINARY|_O_CREAT );
      result.m_write    = file_write;
      result.m_read     = file_read;
      result.m_read_len = file_read_len;
      break;
    case 'I':          //  DATA='ip://137.138.142.82:8000'
      result.ioDesc = Networking::socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
      if ( result.ioDesc > 0 )   {
        StreamDescriptor::getInetConnection(specs, file, sin.sin_addr.s_addr, sin.sin_port);
        sin.sin_family = AF_INET;
        if ( Networking::bind(result.ioDesc, (Networking::sockaddr*)&sin, sizeof(sin)) == 0) {
          if ( Networking::listen(result.ioDesc, SOMAXCONN) == 0 )  {
            result.m_write    = ip_send;
            result.m_read     = ip_recv;
            result.m_read_len = ip_recv_len;
            return result;
          }
        }
        Networking::closesocket(result.ioDesc);
        result.ioDesc = -1;
      }
      break;
    case 'S':          //  DATA='sfc://137.138.142.82:8000'
      StreamDescriptor::getInetConnection(specs, file, sin.sin_addr.s_addr, sin.sin_port);
      if ( 0 == DAQ::daq_register(file.c_str(), sin.sin_port) )  {
        result.ioDesc     = 1;
        result.m_write    = daq_send;
        result.m_read     = daq_recv;
        result.m_read_len = daq_recv_len;
        return result;
      }
      break;
    default:
      break;
  }
  return result;
}

int GaudiOnline::StreamDescriptor::close(Access& specs)  {
  int dsc = specs.ioDesc; 
  specs.ioDesc = -1; 
  switch(specs.type)  {
    case 'F':
      return dsc > 0 ? ::close(dsc) : 0;
    case 'I':
      return dsc > 0 ? Networking::closesocket(dsc) : 0;
    case 'S':
      return DAQ::daq_unregister();
    default:
      return false;
  }
}

GaudiOnline::StreamDescriptor::Access 
GaudiOnline::StreamDescriptor::accept(Access& specs)  {
  Access result = specs;
  switch(specs.type)  {
    case 'F':
      result.ioDesc = specs.ioDesc;
      break;
    case 'I':
      if ( specs.ioDesc > 0 )  {
        int len;
        Networking::sockaddr sin;
        ::memset(&sin,0,sizeof(sin));
        result.ioDesc = Networking::accept(specs.ioDesc, &sin, &len);
      }
      break;
    case 'S':
      result.ioDesc = 1;
      break;
    default:
      result.ioDesc = -1;
      break;
  }
  return result;
}
