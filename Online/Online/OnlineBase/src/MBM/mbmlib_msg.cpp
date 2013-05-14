#include "MBM/bmstruct.h"
#include "MBM/bmmessage.h"
#include <cerrno>
#include <poll.h>

int MBMMessage::read(int fd) {
  int tmp = 0;
  int len = sizeof(MBMMessage);
  char* p = (char*)this;
  ::memset(this,0,len);
  while ( tmp < len )  {
    int sc = ::read(fd,p+tmp,len-tmp);
    if ( sc >  0 ) tmp += sc;
    else if ( sc <= 0 && errno == EINTR  ) continue;
    else if ( sc <= 0 && errno == EAGAIN ) continue;
    else if ( sc < 0 ) return MBM_ERROR;
    else               return MBM_ERROR;
  }
  if ( this->magic != MAGIC ) {
    ::lib_rtl_output(LIB_RTL_ERROR,"Wrong magic word in MBM message:%X instead of %X.\n",this->magic,MAGIC);
  }
  return MBM_NORMAL;
}

int MBMMessage::write(int fd) const {
  const char* p = (const char*)this;
  int tmp = sizeof(MBMMessage), len = sizeof(MBMMessage);
  while ( tmp>0 )  {
    int sc = ::write(fd,p+len-tmp,tmp);
    if ( sc > 0 ) tmp -= sc;
    else if ( sc == 0 && errno == EINTR ) continue;
    else return MBM_ERROR;
  }
  return MBM_NORMAL;
}

int MBMMessage::write(int fd, void* ptr, size_t len) const {
  const char* p = (const char*)ptr;
  int tmp = len;
  while ( tmp>0 )  {
    int sc = ::write(fd,p+len-tmp,tmp);
    if ( sc > 0 ) tmp -= sc;
    else if ( sc == 0 && errno == EINTR ) continue;
    else return MBM_ERROR;
  }
  return MBM_NORMAL;
}

int MBMMessage::wait(int fdin, int* cancelled)   {
  struct pollfd fds;
  while (1)  {
    fds.events  = POLLIN;
    fds.revents = POLLIN|POLLERR;
    fds.fd      = fdin;
    ::poll(&fds,1,100);
    if ( cancelled && *cancelled )
      return status=MBM_REQ_CANCEL;
    else if ( fds.revents&POLLIN ) break;
  }
  if ( !read(fdin) ) {
    ::lib_rtl_output(LIB_RTL_OS,"Failed to receive answer '%d' from server.\n",type);
    return MBM_ERROR;
  }
  // Communication now OK. Let's check if the actual request failed or not:
  if ( status != MBM_NORMAL && status != MBM_NO_EVENT && status != MBM_NO_FREE_US) {
    ::lib_rtl_output(LIB_RTL_OS,"Failed to execute request '%d' in server. Status=%d \n",
		     type,status);
    return status;
  }
  return status;
}

int MBMMessage::communicate(int fdout, int fdin)   {
  int typ = type;
  if ( !write(fdout) ) {
    ::lib_rtl_output(LIB_RTL_OS,"Failed to send request '%d' to server.\n",typ);
    return MBM_ERROR;
  }

  struct pollfd fds;
  fds.events  = POLLIN;
  fds.revents = 0;
  fds.fd      = fdin;
  ::poll(&fds,1,-1);

  if ( !read(fdin) ) {
    ::lib_rtl_output(LIB_RTL_OS,"Failed to receive answer '%d' from server.\n",typ);
    return MBM_ERROR;
  }
  // Communication now OK. Let's check if the actual request failed or not:
  if ( status != MBM_NORMAL && status != MBM_NO_EVENT && status != MBM_NO_FREE_US) {
    ::lib_rtl_output(LIB_RTL_OS,"Failed to execute request '%d' in server. Status=%d \n",
		     typ,status);
    return status;
  }
  return status;
}

