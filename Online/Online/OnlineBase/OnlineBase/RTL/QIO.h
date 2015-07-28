#ifndef _RTL_QIO_H
#define _RTL_QIO_H

#include "RTL/rtl.h"

#ifdef __cplusplus
extern "C" {
#endif
  typedef int (*lib_rtl_qio_ast_t)(void*);
  enum IOAction {
    LIB_RTL_IO_ACCEPT,
    LIB_RTL_IO_ACCEPT_WAIT,
    LIB_RTL_IO_CONNECT,
    LIB_RTL_IO_CONNECT_WAIT,
    LIB_RTL_IO_SEND,
    LIB_RTL_IO_SEND_WAIT,
    LIB_RTL_IO_RECEIVE,
    LIB_RTL_IO_RECEIVE_WAIT
  };
  int lib_rtl_qio(int channel, const IOAction action, IOSB_t* iosb,
                  lib_rtl_qio_ast_t ast, void* ast_param,
                  int64_t p1=0, int64_t p2=0, int64_t p3=0, int64_t p4=0);
#ifdef __cplusplus
}
#endif

#endif // _RTL_QIO_H
