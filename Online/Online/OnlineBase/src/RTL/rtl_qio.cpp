#include "RTL/QIO.h"

int lib_rtl_cancel_io(int channel)    {
#ifdef _VMS
  int status = sys$cancel(channel);
#else
  int status = channel == 0 ? 0 : 1;
#endif
  return status;
}

int lib_rtl_qio(int channel, const IOAction action, IOSB_t* iosb,
                lib_rtl_qio_ast_t ast,void* ast_param,
                int64_t p1, int64_t p2, int64_t p3, int64_t p4)
{
  switch(action)  {
  case LIB_RTL_IO_RECEIVE_WAIT:
    return 0;
  case LIB_RTL_IO_RECEIVE:
    // select(ndfs, rdfset,wrfdset,eceptfdset,timeout);
    // status = ::recv(m_socket, (char*)buff, len, flags);  
    return 0;
  case LIB_RTL_IO_ACCEPT:
    return 0;
  case LIB_RTL_IO_ACCEPT_WAIT:
    return 0;
  case LIB_RTL_IO_CONNECT:
    return 0;
  case LIB_RTL_IO_CONNECT_WAIT:
    return 0;
  case LIB_RTL_IO_SEND:
    return 0;
  case LIB_RTL_IO_SEND_WAIT:
    return 0;
  default:
    break;
  }
  return p1||p2||p3||p4||ast_param||ast||iosb||channel ? 0 : 0;
}

