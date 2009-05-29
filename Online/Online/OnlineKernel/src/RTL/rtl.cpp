#include "RTL/rtl.h"
#include <cerrno>
#include <cstring>
#include <iostream>

using namespace std;

#ifndef WIN32
const char* RTL::errorString(int status)  {
  return strerror(status);
}

#define INSTALL_SIGNAL(x,y) install(x , #x , y);

#else
#include <windows.h>
#include <process.h>

const char* RTL::errorString(int status)  {
  static char s[1024] = "No error reporting implemented";
  static int len = sizeof(s)-1;
  void* lpMessageBuffer;
  ::FormatMessage( 
    FORMAT_MESSAGE_ALLOCATE_BUFFER |  FORMAT_MESSAGE_FROM_SYSTEM,
    0,
    status,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //The user default language
    (LPTSTR) &lpMessageBuffer,
    0,
    0 );
  if ( lpMessageBuffer )  {
    strncpy(s, (const char*)lpMessageBuffer, len);
    s[len] = 0;
    size_t l = strlen(s);
    if ( l > 1 )  {
      s[l-1] = 0;
      if ( !::isalnum(s[l-2]) ) s[l-2]=0;
    }
    ::LocalFree( lpMessageBuffer ); 
  }
  else {
    sprintf(s,"RTL Error: Unknown error code: %08X",status);
  }
  return s;
}
#endif

namespace RTL {
  const char* errorString()  {
    return errorString(lib_rtl_get_error());
  }
  const string& processName()  {
    static string s;
    if ( s.empty() )  {
      char txt[64];
      ::lib_rtl_get_process_name(txt, sizeof(txt));
      s = txt;
    }
    return s;
  }
  const string& dataInterfaceName()  {
    static string s;
    if ( s.empty() )  {
      char txt[64];
      ::lib_rtl_get_datainterface_name(txt, sizeof(txt));
      s = txt;
    }
    return s;
  }
  const string& nodeName()  {
    static string s;
    if ( s.empty() )  {
      char txt[64];
      ::lib_rtl_get_node_name(txt,sizeof(txt));
      s = txt;
    }
    return s;
  }
  const string& nodeNameShort()  {
    static string s;
    if ( s.empty() )  {
      s = nodeName();
      if ( s.find(".") != string::npos )  {
        s = s.substr(0,s.find("."));
      }
    }
    return s;
  }
}
