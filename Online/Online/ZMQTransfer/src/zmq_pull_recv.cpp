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
#include <thread>
#include <string>
#include <ctime>
#include <iostream>

// Framework include files
#include "ZMQTransfer/Context.h"

namespace {

  class Receiver {
  public:
    std::string  m_sinkCon;
    std::string  m_monCon;
    std::thread* m_thread;

    Receiver(int nthread, const std::string& sink)   {
      m_sinkCon  = sink;
      m_monCon   = "inproc://monitor";
      m_thread   = 0;
      ZMQ::Context::setNumberOfThreads(nthread);
    }
    time_t time_diff()  {
      static time_t start = ::time(0);
      return ::time(0) - start;
    }
    void function();
    void monitor();
  };

  using namespace std;
  using namespace ZMQ;

  //===============================================================================
  void Receiver::function()   {
    // Create frontend, backend, control and possible capture sockets
    //m_sink  = socket(ZMQ_PULL);
    //zmq::socket_t control = socket(ZMQ_SUB);
    zmq::socket_t m_sink = Context::socket(ZMQ_PULL);

    ::printf("ZMQ: starting monitor...\n");
    int rc = ::zmq_socket_monitor(m_sink, m_monCon.c_str(), ZMQ_EVENT_ALL);
    if (rc != 0)
      throw error_t ();

    m_thread = new thread([this]{monitor();});
    if ( !m_thread ) {
    }

    try {
      m_sink.bind(m_sinkCon.c_str());
      ::printf("Connected sink to %s\n",m_sinkCon.c_str());
    }
    catch (const zmq::error_t&) {
      ::printf("Failed to connect sinkend to: %s\n",m_sinkCon.c_str());
    }

    int count = 0;
    long long int bytes = 0;
    while(1)   {
      zmq::pollitem_t item[1];
      item[0].fd = 0;
      item[0].socket = m_sink;
      item[0].events = ZMQ_POLLIN|ZMQ_POLLERR;
      item[0].revents = 0;
      int nclient = zmq::poll(item,1,100);
      if ( nclient == 0 )
	continue;
      else if ( nclient < 0 )
	continue;

      if ( (item[0].revents&ZMQ_POLLIN) == ZMQ_POLLIN )  {
	zmq::message_t data;

	m_sink.recv(&data);
	int len = *(int*)data.data();
	if (!data.more())  {
	  ::printf("No more message frames present....\n");
	  break;
	}
	data.rebuild();
	m_sink.recv(&data);
	const char* text = (char*)data.data();
	bytes += len;
	if ( (++count%10000)==0 ) {
	  time_t diff = time_diff();
	  if ( diff < 1 ) diff = 1;
	  ::printf("%4d size:%6d len:%6d --> %-32s   %7.3f MB/sec\n",
		   (int)diff,(int)data.size(),
		   len,text,double(bytes)/1024./1024./double(diff));
	}
      }
      else if ( (item[0].revents&ZMQ_POLLERR) == ZMQ_POLLERR )  {
	cout << time_diff() << " POLL-ERROR:" << zmq_strerror(zmq_errno()) << endl;
      }
    }
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

  void Receiver::monitor()   {
    zmq_event_t event;
    static char addr[1025];
    zmq::socket_t s = Context::socket(ZMQ_PAIR);
    s.connect (m_monCon.c_str());
    while (read_msg(s, &event, addr)) {
      switch (event.event) {
      case ZMQ_EVENT_CONNECTED:
	printf ("connecting socket descriptor %d\n", event.value);
	printf ("connecting socket address %s\n", addr);
	break;
      case ZMQ_EVENT_LISTENING:
	printf ("listening socket descriptor %d\n", event.value);
	printf ("listening socket address %s\n", addr);
	break;
      case ZMQ_EVENT_BIND_FAILED:
	printf ("bind socket failure descriptor %d\n", event.value);
	printf ("bind socket address %s\n", addr);
	break;
      case ZMQ_EVENT_ACCEPTED:
	printf ("accepted socket descriptor %d\n", event.value);
	printf ("accepted socket address %s\n", addr);
	break;
      case ZMQ_EVENT_ACCEPT_FAILED:
	printf ("accepted socket failure descriptor %d\n", event.value);
	printf ("accepted socket address %s\n", addr);
	break;
      case ZMQ_EVENT_CLOSE_FAILED:
	printf ("socket close failure error code %d\n", event.value);
	printf ("socket address %s\n", addr);
	break;
      case ZMQ_EVENT_CLOSED:
	printf ("closed socket descriptor %d\n", event.value);
	printf ("closed socket address %s\n", addr);
	break;
      case ZMQ_EVENT_DISCONNECTED:
	printf ("disconnected socket descriptor %d\n", event.value);
	printf ("disconnected socket address %s\n", addr);
	break;
      }
    }
    s.close();
  }

  static void help() {
  }
}

#include "RTL/rtl.h"
extern "C" int zmq_pull_recv(int argc, char** argv)   {
  RTL::CLI cli(argc, argv, help);
  string sink= "tcp://*:5555";
  cli.getopt("sink", 1, sink);

  Receiver rcv(5,sink);
  rcv.function();
  return 1;
}

