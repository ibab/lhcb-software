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

// C/C++ Include files
#include <string>
#include <thread>
#include <cstdio>
#include <cstdarg>

// Framework include files
#include "RTL/rtl.h"
#include "ZMQTransfer/Context.h"

namespace {

  const char* proc_name()  {
    static std::string proc = RTL::processName();
    return proc.c_str();
  }

  void output(const char* format, ...)   {
    va_list args;
    va_start( args, format );
    ::printf("%s > ",proc_name());
    ::vfprintf(stdout, format, args);
    va_end(args);
  }

  class Sender {
    std::string  m_monCon;
    std::thread* m_thread;
  public:
    Sender()   {
      m_monCon   = "inproc://monitor";
      m_thread   = 0;
    }
    int function(const std::string& targetCon)    {
      // Create frontend, backend, control and possible capture sockets
      zmq::socket_t target  = ZMQ::Context::socket(ZMQ_PUSH);

      output("ZMQ: starting monitor...\n");
      int rc = ::zmq_socket_monitor(target, m_monCon.c_str(), ZMQ_EVENT_ALL);
      if (rc != 0)
	throw error_t ();

      m_thread = new std::thread([this]{monitor();});
      if ( !m_thread ) {
      }

      try {
	target.connect(targetCon.c_str());
	output("Connected to target %s.\n", targetCon.c_str());
      }
      catch (const zmq::error_t&) {
	output("Failed to connect target to: %s.\n", targetCon.c_str());
      }
      size_t MBYTE = 1024*1024;
      struct Data {
	int len; char text[1]; 
      } *data = (Data*)::operator new(MBYTE+sizeof(int)+1);

      int count = 0;
      data->len = 10*1024;
      while(1)   {
	::snprintf(data->text, MBYTE, "%s: +1+2+3+4+5+6+7+8 %8d  [%8d]",proc_name(),++count,data->len);
	target.send( &(data->len), sizeof(data->len), ZMQ_SNDMORE);
	target.send( data->text, data->len, 0);
	if ( 0 == (count % 10000) )  {
	  output(" --> [%8d] %s\n",data->len,data->text);
	}
	data->len += 64;
	if ( data->len > 128*1024 ) data->len = 10*1024;
      }
      return 0;
    }
    static int read_msg(void* s, zmq_event_t* event, char* ep)  {
      int rc ;
      zmq_msg_t msg1; // binary part
      zmq_msg_init (&msg1);
      zmq_msg_t msg2; // address part
      zmq_msg_init (&msg2);
      rc = zmq_msg_recv (&msg1, s, 0);
      if (rc == -1 && zmq_errno() == ETERM)
	return 1;
      assert (rc != -1);
      assert (zmq_msg_more(&msg1) != 0);
      rc = zmq_msg_recv (&msg2, s, 0);
      if (rc == -1 && zmq_errno() == ETERM)
	return 1;
      assert (rc != -1);
      assert (zmq_msg_more(&msg2) == 0);
      // copy binary data to event struct
      const char* data = (char*)zmq_msg_data(&msg1);
      memcpy(&(event->event), data, sizeof(event->event));
      memcpy(&(event->value), data+sizeof(event->event), sizeof(event->value));
      // copy address part
      size_t len = zmq_msg_size(&msg2) ;
      ep = (char*)memcpy(ep, zmq_msg_data(&msg2), len);
      *(ep + len) = 0 ;
      return 1;
    }
    void monitor()   {
      zmq_event_t event;
      static char addr[1025];
      zmq::socket_t s = ZMQ::Context::socket(ZMQ_PAIR);
      s.connect (m_monCon.c_str());
      while (read_msg(s, &event, addr)) {
	switch (event.event) {
	case ZMQ_EVENT_CONNECTED:
	  output ("connecting socket descriptor %d\n", event.value);
	  output ("connecting socket address %s\n", addr);
	  break;
	case ZMQ_EVENT_LISTENING:
	  output ("listening socket descriptor %d\n", event.value);
	  output ("listening socket address %s\n", addr);
	  break;
	case ZMQ_EVENT_BIND_FAILED:
	  output ("bind socket failure descriptor %d\n", event.value);
	  output ("bind socket address %s\n", addr);
	  break;
	case ZMQ_EVENT_ACCEPTED:
	  output ("accepted socket descriptor %d\n", event.value);
	  output ("accepted socket address %s\n", addr);
	  break;
	case ZMQ_EVENT_ACCEPT_FAILED:
	  output ("accepted socket failure descriptor %d\n", event.value);
	  output ("accepted socket address %s\n", addr);
	  break;
	case ZMQ_EVENT_CLOSE_FAILED:
	  output ("socket close failure error code %d\n", event.value);
	  output ("socket address %s\n", addr);
	  break;
	case ZMQ_EVENT_CLOSED:
	  output ("closed socket descriptor %d\n", event.value);
	  output ("closed socket address %s\n", addr);
	  break;
	case ZMQ_EVENT_DISCONNECTED:
	  output ("disconnected socket descriptor %d\n", event.value);
	  output ("disconnected socket address %s\n", addr);
	  break;
	}
      }
      s.close();
    }
  };
  static void help() {
  }
}

#include "RTL/rtl.h"
extern "C" int zmq_push_send(int argc, char** argv)   {
  RTL::CLI cli(argc, argv, help);
  std::string target= "tcp://localhost:5555";
  cli.getopt("target", 1, target);
  return Sender().function(target);
}
