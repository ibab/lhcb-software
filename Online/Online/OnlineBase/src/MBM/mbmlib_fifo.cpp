// $Id: $
//==========================================================================
//  LHCb Online software suite
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see OnlineSys/LICENSE.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "MBM/bmmessage.h"
#include "MBM/bmserver.h"

// C/C++ include files
#include <cerrno>
#include <poll.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/epoll.h>

namespace {

  int _read_request(MBMConnection& connection, void* buffer, size_t length)   {
    int tmp = 0;
    int len = length;
    char* p = (char*)buffer;
    ::memset(p,0,len);
    while ( tmp < len )  {
      errno = 0;
      int sc = ::read(connection.client.request,p+tmp,len-tmp);
      if ( sc >  0 )
	tmp += sc;
      else if ( sc <= 0 && errno == EINTR  )
	continue;
      else if ( sc == 0 && errno == EAGAIN )
	continue;
      else if ( sc  < 0 && errno == EAGAIN ) 
	return MBM_ERROR;
      else {
	::lib_rtl_output(LIB_RTL_ALWAYS,"MBM: Recv MBM message FAIL");
	return MBM_ERROR;
      }
    }
    return MBM_NORMAL;
  }

  int _send_response(MBMConnection& connection, const void* buffer, size_t length)   {
    const char* p = (const char*)buffer;
    int tmp = length, len = length;
    int fd = connection.client.response;
    while ( tmp>0 )  {
      int sc = ::write(fd,p+len-tmp,tmp);
      if ( sc > 0 ) tmp -= sc;
      else if ( sc == 0 && errno == EINTR ) continue;
      else return MBM_ERROR;
    }
    return MBM_NORMAL;
  }

  int _close(MBMConnection& connection)  {
    // First close the server connection
    if ( connection.client.request > 0 )  {
      ::close(connection.client.request);
    }
    // Now close the answer connection
    if ( connection.client.response > 0 ) {
      ::close(connection.client.response);
    }
    if ( 0 != connection.name[0] ) {
      ::unlink(connection.name);
    }
    connection.init();
    return MBM_NORMAL;
  }

  int _accept(MBMConnection& connection, const char* bm_name, const char* name)   {
    connection.init();
    //::snprintf(connection.name,sizeof(connection.name),"/dev/shm/bm_%s_%s",bm_name,name);
    ::snprintf(connection.name,sizeof(connection.name),"/tmp/bm_%s_%s",bm_name,name);
    if( -1 == (connection.client.response = ::open(connection.name, O_NONBLOCK | O_RDWR ))) {
      ::lib_rtl_signal_message(LIB_RTL_OS,
			       "MBM: [%s] Unable to open the answer connection %s.",
			       name,connection.name);
      return MBM_ERROR;
    }
    return MBM_NORMAL;
  }

  int _bind(MBMConnection& connection, const char* bm_name, int id)   {
    int fd;
    //::snprintf(connection.name,sizeof(connection.name),"/dev/shm/bm_%s_server_%d",bm_name,id);
    ::snprintf(connection.name,sizeof(connection.name),"/tmp/bm_%s_server_%d",bm_name,id);
    connection.name[sizeof(connection.name)-1] = 0;
    if( ::mkfifo(connection.name,0666)  ) {
      if ( errno != EEXIST ) { // It is not an error if the file already exists....
	::lib_rtl_output(LIB_RTL_OS,
			 "MBM: Unable to create the connection: %s.\n",
			 connection.name);
	return MBM_ERROR;
      }
    }
    ::chmod(connection.name,0666);
    if( -1 == (fd=::open(connection.name,O_RDWR|O_NONBLOCK)) ) {
      ::lib_rtl_output(LIB_RTL_OS,
		       "MBM: Unable to open the connection: %s\n",
		       connection.name);
      return MBM_ERROR;
    }
    connection.master.request = fd;
    if ( -1 == (::fcntl(fd,F_SETFL,::fcntl(fd,F_GETFL)|O_NONBLOCK)) ) {
      ::lib_rtl_output(LIB_RTL_OS,
		       "MBM: Unable to set connection: %s non-blocking\n",
		       connection.name);
    }
    connection.master.poll = ::epoll_create(1);
    if ( connection.master.poll < 0 ) {
      ::lib_rtl_output(LIB_RTL_OS,
		       "MBM: Failed to create pollset for server connection %s.\n",
		       connection.name);
      return MBM_ERROR;
    }
    struct epoll_event epoll;
    epoll.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;
    epoll.data.fd = fd;
    if ( 0 > ::epoll_ctl(connection.master.poll,EPOLL_CTL_ADD,fd,&epoll) ) {
      ::lib_rtl_output(LIB_RTL_OS,
		       "MBM: Failed to add server connection %s to epoll descriptor.\n",
		       connection.name);
      return MBM_ERROR;
    }
    return MBM_NORMAL;
  }

  /// Create new poll pool
  int _poll_create(MBMConnection& connection, int max_count)   {
    connection.master.poll = ::epoll_create(max_count);
    if ( connection.master.poll < 0 ) {
      ::lib_rtl_output(LIB_RTL_OS,
		       "MBM: Failed to create client epollset: %s.\n",
		       connection.name);
      return MBM_ERROR;
    }
    return MBM_NORMAL;
  }

  /// Add connection to poll pool
  int _poll_add(MBMConnection& connection, MBMConnection& client)   {
    struct epoll_event epoll;
    epoll.events  = EPOLLERR | EPOLLHUP;
    epoll.data.fd = client.client.response;
    if ( 0 > ::epoll_ctl(connection.master.poll, EPOLL_CTL_ADD, client.client.response, &epoll) ) {
      ::lib_rtl_output(LIB_RTL_OS,
		       "MBM: Failed to client connection %s to epoll descriptor.\n",
		       client.name);
      return MBM_ERROR;
    }
    return MBM_NORMAL;
  }

  /// Remove connection from poll pool
  int _poll_del(MBMConnection& connection, MBMConnection& client)   {
    struct epoll_event epoll;
    int fd = client.client.response;
    if ( fd >= 0 )  {
      epoll.data.fd = fd;
      if ( 0 > ::epoll_ctl(connection.master.poll, EPOLL_CTL_DEL, fd, &epoll) ) {
	::lib_rtl_output(LIB_RTL_OS,"MBM: Failed to remove client connection %d.\n",fd);
      }
      return MBM_NORMAL;
    }
    return MBM_ERROR;
  }

  /// Poll on connections delivering data
  int _poll(MBMConnection& connection, int& events, int tmo)  {
    struct epoll_event epoll;
    int nclients = ::epoll_wait(connection.master.poll, &epoll, 1, tmo);
    events = epoll.events;
    if ( nclients > 0 )
      return epoll.data.fd;
    return nclients;
  }

  int _open(MBMConnection& connection, const char* bm_name, const char* name)  {
    char text[256];
    //::snprintf(text,sizeof(text),"/dev/shm/bm_%s_server_0",bm_name);
    ::snprintf(text,sizeof(text),"/tmp/bm_%s_server_0",bm_name);
    if( (connection.client.request=::open(text, O_WRONLY|O_NONBLOCK)) < 0 ) {
      ::lib_rtl_signal_message(LIB_RTL_OS,"MBM: %s: Cannot open connection '%s' for MBM buffer %s.",
			       name,text,bm_name);
      _close(connection);
      return MBM_ERROR;
    }
    //::snprintf(connection.name,sizeof(connection.name),"/dev/shm/bm_%s_%s",bm_name,name);  
    ::snprintf(connection.name,sizeof(connection.name),"/tmp/bm_%s_%s",bm_name,name);  
    if( ::mkfifo(connection.name,0666)  ) {
      if ( errno != EEXIST ) { // It is not an error if the file already exists....
	::lib_rtl_signal_message(LIB_RTL_OS,"MBM: %s: Cannot create MBM connection '%s'.",
				 name,connection.name);
	return MBM_ERROR;
      }
    }
    ::chmod(connection.name,0666);
    ::snprintf(connection.name,sizeof(connection.name),"/tmp/bm_%s_%s",bm_name,name);
    //::snprintf(connection.name,sizeof(connection.name),"/dev/shm/bm_%s_%s",bm_name,name);
    if( -1 == (connection.client.response = ::open(connection.name, O_NONBLOCK | O_RDWR ))) {
      ::lib_rtl_signal_message(LIB_RTL_OS,
			       "MBM: [%s] Unable to open the answer connection %s.",
			       name,connection.name);
      return MBM_ERROR;
    }
    return MBM_NORMAL;
  }

  int _move(MBMConnection& connection, const char* bm_name, USER*, int serverid)   {
    char text[256];
    //::snprintf(text,sizeof(text),"/dev/shm/bm_%s_server_%d",bm_name,serverid);
    ::snprintf(text,sizeof(text),"/tmp/bm_%s_server_%d",bm_name,serverid);
    ::close(connection.client.request);
    if( (connection.client.request=::open(text,O_WRONLY|O_NONBLOCK)) < 0 ) {
      ::lib_rtl_signal_message(LIB_RTL_OS,
			       "MBM: Failed to open server connection '%s' for MBM buffer %s.",
			       text,bm_name);
      return MBM_ERROR;
    }
    return MBM_NORMAL;
  }

  int _write(MBMMessage& msg, int fd) {
    const char* p = (const char*)&msg;
    int tmp = sizeof(MBMMessage), len = sizeof(MBMMessage);
#ifdef _DEBUG_MBM_MSG
    ::lib_rtl_output(LIB_RTL_ALWAYS,"MBM: Send MBM message %-16s --> %-16s status=%d\n",
		     msg.c_user(),msg.c_type(),msg.status);
#endif
    while ( tmp>0 )  {
      int sc = ::write(fd,p+len-tmp,tmp);
      if ( sc > 0 ) tmp -= sc;
      else if ( sc == 0 && errno == EINTR ) continue;
      else return msg.status=MBM_ERROR;
    }
    return msg.status=MBM_NORMAL;
  }

  int _read(MBMMessage& msg, int fd, int* cancel_flag) {
    int tmp = 0;
    int len = sizeof(MBMMessage);
    char* p = (char*)&msg;
    ::memset(p,0,len);
    while ( tmp < len )  {
      errno = 0;
      int sc = ::read(fd,p+tmp,len-tmp);
      if ( sc >  0 )
	tmp += sc;
      else if ( sc <= 0 && errno == EINTR  )
	continue;
      else if ( sc <= 0 && errno == EAGAIN && cancel_flag && *cancel_flag )
	return msg.status=MBM_ERROR;
      else if ( sc == 0 && errno == EAGAIN )
	continue;
      else if ( sc  < 0 && errno == EAGAIN ) 
	return msg.status=MBM_ERROR;
      else {
	::lib_rtl_output(LIB_RTL_ALWAYS,"MBM: Recv MBM message FAIL");
	return msg.status=MBM_ERROR;
      }
    }
#ifdef _DEBUG_MBM_MSG
    ::lib_rtl_output(LIB_RTL_ALWAYS,"MBM: Recv MBM message %-16s --> %-16s status=%d\n",
		     msg.c_user(),msg.c_type(),msg.status);
#endif
    if ( msg.magic != MBMMessage::MAGIC ) {
      ::lib_rtl_output(LIB_RTL_ERROR,"MBM: Wrong magic word in MBM message:%X instead of %X.\n",
		       msg.magic,MBMMessage::MAGIC);
    }
    return msg.status=MBM_NORMAL;
  }

  /// Exchange MBM message with server
  int _communicate(MBMConnection& connection, MBMMessage& msg, int* cancelled)   {
    int fdout = connection.server.request;
    int fdin  = connection.server.response;
    int typ   = msg.type;
    if ( MBM_NORMAL != (msg.status=_write(msg, fdout)) ) {
      ::lib_rtl_signal_message(LIB_RTL_OS,"MBM: Failed to send request '%d' to server.\n",typ);
      return MBM_ERROR;
    }
#ifdef _DEBUG_MBM_MSG
    ::lib_rtl_output(LIB_RTL_ALWAYS,"MBM: Poll MBM message %-16s --> %-16s status=%d\n",
		     msg.c_user(),msg.c_type(),status);
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
	::lib_rtl_output(LIB_RTL_ALWAYS,"MBM: Recv CANCEL %-16s --> %-16s status=%d\n",
			 msg.c_user(),msg.c_type(),msg.status);
#endif
	return msg.status=MBM_REQ_CANCEL;
      }
      else if ( fds[0].revents&POLLHUP )    {
	::lib_rtl_output(LIB_RTL_ALWAYS,"MBM: Recv POLLHUP(IN) %-16s --> %-16s status=%d\n",
			 msg.c_user(),msg.c_type(),msg.status);
      }
      else if ( fds[0].revents&POLLERR )    {
	::lib_rtl_output(LIB_RTL_ALWAYS,"MBM: Recv POLLERR(IN) %-16s --> %-16s status=%d\n",
			 msg.c_user(),msg.c_type(),msg.status);
      }
      else if ( fds[1].revents&POLLHUP )    {
	::lib_rtl_output(LIB_RTL_ALWAYS,"MBM: Recv POLLHUP(OUT) %-16s --> %-16s status=%d\n",
			 msg.c_user(),msg.c_type(),msg.status);
      }
      else if ( fds[1].revents&POLLERR )    {
	::lib_rtl_output(LIB_RTL_ALWAYS,"MBM: Recv POLLERR(OUT) %-16s --> %-16s status=%d\n",
			 msg.c_user(),msg.c_type(),msg.status);
      }
    }
    if ( MBM_NORMAL != (msg.status=_read(msg,fdin,cancelled)) ) {
      ::lib_rtl_output(LIB_RTL_OS,
		       "MBM: Failed to receive answer '%d' from server.\n",typ);
      return MBM_ERROR;
    }
    // Communication now OK. Let's check if the actual request failed or not:
    if ( msg.status != MBM_NORMAL && msg.status != MBM_NO_EVENT && msg.status != MBM_NO_FREE_US) {
      ::lib_rtl_output(LIB_RTL_OS,
		       "MBM: Failed to execute request '%d' in server. Status=%d \n",
		       typ,msg.status);
      return msg.status;
    }
    return msg.status;
  }

  /// Clear possibly pending data from communication channel
  int _clear(MBMConnection& connection)   {
    char buff[sizeof(MBMMessage)];
    while (1)  {
      // Connection is non-blocking, so just read some junks
      int sc = ::read(connection.server.response,buff,sizeof(MBMMessage));
      if ( sc > 0 ) {
	continue;
      }
      else if ( sc == -1 && errno == EINTR ) {
	continue;
      }
      break;
    }
    return MBM_NORMAL;
  }

  /// Send request to server
  int _send_request(MBMConnection& connection, MBMMessage& msg, bool clear_before)   {
    if ( clear_before )  {
      _clear(connection);
    }
    return msg.status=_write(msg,connection.server.request);
  }

  /// Read server response
  int _read_response(MBMConnection& connection, MBMMessage& msg, int* cancelled)  {
    int fd = connection.server.response;
    struct pollfd fds;
    while (1)  {
      fds.events  = POLLIN;
      fds.revents = POLLIN|POLLERR;
      fds.fd      = fd;
      ::poll(&fds,1,100);
      if ( cancelled && *cancelled )  {
#ifdef _DEBUG_MBM_MSG
	::lib_rtl_output(LIB_RTL_ALWAYS,"MBM CANCEL %-16s --> %-16s status=%d\n",
			 msg.c_user(),msg.c_type(),msg.status);
#endif
	return msg.status=MBM_REQ_CANCEL;
      }
      else if ( fds.revents&POLLIN )
	break;
    }
    if ( MBM_NORMAL != (msg.status=_read(msg, fd, cancelled)) ) {
      if ( cancelled && *cancelled )   {
	::lib_rtl_output(LIB_RTL_OS,"Failed to receive answer '%d' [Request cancelled].\n",msg.type);
	return MBM_ERROR;
      }
      ::lib_rtl_output(LIB_RTL_OS,"MBM: Failed to receive answer '%d' from server.\n",msg.type);
      return msg.status=MBM_ERROR;
    }
    // Communication now OK. Let's check if the actual request failed or not:
    if ( msg.status != MBM_NORMAL && msg.status != MBM_NO_EVENT && msg.status != MBM_NO_FREE_US) {
      ::lib_rtl_signal_message(LIB_RTL_OS,"Failed to execute request '%d' in server. Status=%d \n",
		       msg.type,msg.status);
      return msg.status;
    }
    return msg.status;
  }

  int _dispatch(ServerBMID bm, int which)  {
    ServerBMID_t::Server& s = bm->server[which];
    MBMConnection connection;
    MBMMessage msg(0);

    ::lib_rtl_output(LIB_RTL_DEBUG,"++bm_server++ Thread %3d Dispatching Fifo connections....\n",which);
    while ( !bm->stop )   {
      int events = 0;
      if ( bm->stop )  {
	break;
      }
      connection.client.request = _poll(s.connection, events, 100);
      if ( bm->stop )   {
	break;
      }
      else if ( connection.client.request <= 0 && bm->ctrl->i_events > 0 )   {
	/* int free_slots = */ /* mbmsrv_check_pending_tasks(bm); */
	continue;
      }
      else if ( connection.client.request < 0 && errno == EINTR )   {
	continue;
      }
      else if ( connection.client.request <= 0 )   {
	continue;
      }
      else if( events&(EPOLLERR|EPOLLHUP) ) {
	::lib_rtl_output(LIB_RTL_ERROR,"MBM input epoll error/hup signal...");
	mbmsrv_check_clients(bm);
      }
      else if ( events&EPOLLIN ) {
	if ( MBM_NORMAL != (msg.status=_read_request(connection,&msg,sizeof(msg))) ) {
	  ::lib_rtl_output(LIB_RTL_ERROR,"++bm_server++ Error while reading : message is lost [%d: %s]",
			   ::lib_rtl_get_error(), RTL::errorString(::lib_rtl_get_error()));
	  continue;
	}
	else if ( msg.magic != MBMMessage::MAGIC ) {
	  ::lib_rtl_output(LIB_RTL_ERROR,"++bm_server++ Wrong magic word in MBM message:%X instead of %X.",
			   msg.magic,MBMMessage::MAGIC);
	  continue;
	}
	mbmsrv_handle_request(bm,0,msg);
	if ( msg.status != MBM_NO_REPLY ) {
	  // Do not send a message if the user is already free'ed
	  if ( msg.user && msg.user->uid != -1 )  {
	    msg.status = _send_response(msg.user->connection,&msg,sizeof(msg));
	  }
	  continue;
	}
	//if ( nreq == 200 ) ProfilerStart("mbm_server.prof");
	//if ( nreq == 5000000 ) break;
      }
    }
    //ProfilerStop();
    return 1;
  }
}

int _mbm_connections_use_fifos(MBMCommunication& com)   {
  com.type          = BM_COM_FIFO;

  /** MBM Server communication functions   */
  com.accept        = _accept;
  com.bind          = _bind;
  com.open          = _open;
  com.close         = _close;
  com.send_response = _send_response;
  com.dispatch      = _dispatch;
  com.poll          = _poll;
  com.poll_del      = _poll_del;
  com.poll_add      = _poll_add;
  com.poll_create   = _poll_create;

  /** MBM Client communication functions   */
  com.open_server   = _open;
  com.move_server   = _move;
  com.communicate   = _communicate;
  com.clear         = _clear;
  com.send_request  = _send_request;
  com.read_response = _read_response;
  return MBM_NORMAL;
}
