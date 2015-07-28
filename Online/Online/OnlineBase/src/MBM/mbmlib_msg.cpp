#include "MBM/bmstruct.h"
#include "MBM/bmmessage.h"
#include <cerrno>
#include <poll.h>

namespace {
#define CHECK(x) case MBMMessage::x : return #x ;
  const char* __msg_type(int typ)  {
    switch(typ)  {
      CHECK(INCLUDE)
        CHECK(EXCLUDE)
        //Consummer part
        CHECK(ADD_REQUEST)
        CHECK(DEL_REQUEST)
        CHECK(GET_EVENT)
        CHECK(WAIT_EVENT)
        CHECK(FREE_EVENT)
        CHECK(STOP_CONSUMER)
        CHECK(PAUSE)
        //Producer part
        CHECK(GET_SPACE_TRY)
        CHECK(GET_SPACE)
        CHECK(WAIT_SPACE_A)
        CHECK(FREE_SPACE)
        CHECK(SEND_SPACE)
        CHECK(SEND_EVENT)
        CHECK(DECLARE_EVENT)
        CHECK(CANCEL_REQUEST)
        CHECK(STOP_PRODUCER)
        CHECK(GRANT_UPDATE)
        //
        //statistics
        CHECK(PROCESS_EXISTS)
        CHECK(STAT_EVENTS_ACTUAL)
        CHECK(STAT_EVENTS_SEEN)
        CHECK(STAT_EVENTS_PRODUCED)
        CHECK(STAT_RESET)
        CHECK(STAT_MIN_ALLOC)
        CHECK(STAT_BUFFER_SIZE)
        CHECK(STAT_EVENTS_IN_BUFFER)
        CHECK(STAT_SPACE_IN_BUFFER)
    default:      return "Unknown";
    }
  }
#ifdef _DEBUG_MBM_MSG
#endif
  const char* __msg_user(const USER* user)  {
    static char text[32];
    ::snprintf(text,sizeof(text),"%12X",(void*)user);
    return text;
  }
}

const char* MBMMessage::typeStr(int typ) {
  return __msg_type(typ);
}

int MBMMessage::read(int fd) {
  int cancel = 0;
  return this->read(fd, &cancel);
}

int MBMMessage::read(int fd, int* cancel_flag) {
  int tmp = 0;
  int len = sizeof(MBMMessage);
  char* p = (char*)this;
  ::memset(this,0,len);
  while ( tmp < len )  {
    errno = 0;
    int sc = ::read(fd,p+tmp,len-tmp);
    if ( sc >  0 )
      tmp += sc;
    else if ( sc <= 0 && errno == EINTR  )
      continue;
    else if ( sc <= 0 && errno == EAGAIN && cancel_flag && *cancel_flag )
      return MBM_ERROR;
    else if ( sc == 0 && errno == EAGAIN )
      continue;
    else if ( sc  < 0 && errno == EAGAIN ) 
      return MBM_ERROR;
    else {
      ::lib_rtl_output(LIB_RTL_ALWAYS,"Recv MBM message FAIL");
      return MBM_ERROR;
    }
  }
#ifdef _DEBUG_MBM_MSG
  ::lib_rtl_output(LIB_RTL_ALWAYS,"Recv MBM message %-16s --> %-16s status=%d\n",__msg_user(user),__msg_type(type),status);
#endif
  if ( this->magic != MAGIC ) {
    ::lib_rtl_output(LIB_RTL_ERROR,"Wrong magic word in MBM message:%X instead of %X.\n",this->magic,MAGIC);
  }
  return MBM_NORMAL;
}

int MBMMessage::write(int fd) const {
  const char* p = (const char*)this;
  int tmp = sizeof(MBMMessage), len = sizeof(MBMMessage);
#ifdef _DEBUG_MBM_MSG
  ::lib_rtl_output(LIB_RTL_ALWAYS,"Send MBM message %-16s --> %-16s status=%d\n",__msg_user(user),__msg_type(type),status);
#endif
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
#ifdef _DEBUG_MBM_MSG
  ::lib_rtl_output(LIB_RTL_ALWAYS,"Recv MBM message %-16s --> %-16s status=%d\n",__msg_user(user),__msg_type(type),status);
#endif
  while ( tmp>0 )  {
    int sc = ::write(fd,p+len-tmp,tmp);
    if ( sc > 0 ) tmp -= sc;
    else if ( sc == 0 && errno == EINTR ) continue;
    else return MBM_ERROR;
  }
  return MBM_NORMAL;
}

/// Clean possibly pending messages from the receive fifo (e.g. after a cancel)
int MBMMessage::clearFifo(int fd) {
  //struct pollfd fds;
  char buff[sizeof(MBMMessage)];
  while (1)  {
    //fds.events  = POLLIN;
    //fds.revents = POLLIN;
    //fds.fd      = fd;
    //int sc = ::poll(&fds,1,5);
    // Fifo is non-blocking, so just read some junks
    int sc = ::read(fd,buff,sizeof(MBMMessage));
    if ( sc > 0 ) {
      continue;
    }
    else if ( sc == -1 && errno == EINTR ) {
      continue;
    }
    //::lib_rtl_output(LIB_RTL_ALWAYS,"Request fifo cleaned up.....\n");
    return MBM_NORMAL;
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
    if ( cancelled && *cancelled )   {
#ifdef _DEBUG_MBM_MSG
      ::lib_rtl_output(LIB_RTL_ALWAYS,"Recv MBM CANCEL %-16s --> %-16s status=%d\n",__msg_user(user),__msg_type(type),status);
#endif
      return status=MBM_REQ_CANCEL;
    }
    else if ( fds.revents&POLLIN ) break;
  }
  if ( !read(fdin, cancelled) ) {
    if ( cancelled && *cancelled )   {
      ::lib_rtl_output(LIB_RTL_OS,"Failed to receive answer '%d' [Request cancelled].\n",type);
      return MBM_ERROR;
    }
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
  int cancel = 0;
  return communicate(fdout, fdin, &cancel);
}
int MBMMessage::communicate(int fdout, int fdin, int* cancelled)   {
  int typ = type;
  if ( !write(fdout) ) {
    ::lib_rtl_output(LIB_RTL_OS,"Failed to send request '%d' to server.\n",typ);
    return MBM_ERROR;
  }

#ifdef _DEBUG_MBM_MSG
  ::lib_rtl_output(LIB_RTL_ALWAYS,"Poll MBM message %-16s --> %-16s status=%d\n",__msg_user(user),__msg_type(type),status);
#endif
  struct pollfd fds[2];
  while (1)  {
    fds[0].events  = POLLIN;
    fds[0].revents = POLLIN|POLLERR|POLLHUP;
    fds[0].fd      = fdin;
    fds[1].events  = POLLIN;
    fds[1].revents = POLLIN|POLLERR|POLLHUP;
    fds[1].fd      = fdout;

    ::poll(fds,2,100);
    if ( fds[0].revents&POLLIN )   {
      break;
    }
    else if ( cancelled && *cancelled )   {
#ifdef _DEBUG_MBM_MSG
      ::lib_rtl_output(LIB_RTL_ALWAYS,"Recv MBM CANCEL %-16s --> %-16s status=%d\n",
                       __msg_user(user),__msg_type(type),status);
#endif
      return status=MBM_REQ_CANCEL;
    }
    else if ( fds[0].revents&POLLHUP )    {
      ::lib_rtl_output(LIB_RTL_ALWAYS,"Recv MBM POLLHUP(IN) %-16s --> %-16s status=%d\n",
                       __msg_user(user),__msg_type(type),status);
    }
    else if ( fds[0].revents&POLLERR )    {
      ::lib_rtl_output(LIB_RTL_ALWAYS,"Recv MBM POLLERR(IN) %-16s --> %-16s status=%d\n",
                       __msg_user(user),__msg_type(type),status);
    }
    else if ( fds[1].revents&POLLHUP )    {
      ::lib_rtl_output(LIB_RTL_ALWAYS,"Recv MBM POLLHUP(OUT) %-16s --> %-16s status=%d\n",
                       __msg_user(user),__msg_type(type),status);
    }
    else if ( fds[1].revents&POLLERR )    {
      ::lib_rtl_output(LIB_RTL_ALWAYS,"Recv MBM POLLERR(OUT) %-16s --> %-16s status=%d\n",
                       __msg_user(user),__msg_type(type),status);
    }
  }
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

