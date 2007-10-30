#include <cstdarg>

#ifndef __GCCXML
#define __GCCXML
#define __REAL_COMP
#ifdef _WIN32
#else
typedef void __va_list_tag;
#endif
#else
#endif

#include <iostream>
#include <ctime>
#include <map>
#include <vector>
#include "WT/wtdef.h"
#include "MBM/bmstruct.h"
#include "MBM/bmdef.h"
#include "RTL/rtl.h"
#include "RTL/Lock.h"
#include "RTL/Thread.h"
#include "RTL/GlobalSection.h"
#include "AMS/amsdef.h"
#include "NET/DataTransfer.h"
#include "NET/IOPortManager.h"
#include "NET/NetworkChannel.h"
#include "NET/NetworkConnection.h"
#include "NET/TcpNetworkChannel.h"
#include "NET/TcpConnection.h"
#include "NET/UdpNetworkChannel.h"
#include "NET/UdpConnection.h"

#if 0
namespace  {
  class Dictionary  {
    Dictionary()  {
    }
    ~Dictionary() {}
  };
}
#endif
