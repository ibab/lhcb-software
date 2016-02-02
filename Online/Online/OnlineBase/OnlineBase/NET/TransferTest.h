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

#ifndef ONLINEKERNEL_NO_TESTS

// C/C++ include files
#include "NET/defs.h"
#include <vector>
#include <cstring>
#include <cerrno>
#include <string>

// Framework includes
#include "RTL/rtl.h"
#include "RTL/time.h"

namespace {

  void help_send() {
    ::printf("test_<type>_net_send -opt [-opt]\n");
    ::printf("    -n<ame>=<process-name>     Sender/own process name     \n");
    ::printf("    -ta<rget>=<process-name>   Receiver's process name     \n");
    ::printf("    -tu<rns>=<number>          Number of receive/send turns\n");
    ::printf("    -l<ength>=<number>         Message length in bytes     \n");
    ::printf("    -b<ounce>                  Run in message bounce mode  \n");
    ::printf("    -th<reeads>=<number>       Number of threads for underlying implementation\n");
  }

  void help_recv() {
    ::printf("test_<type>_net_recv -opt [-opt]\n");
    ::printf("    -b<ounce>                  Run in message bounce mode) \n");
    ::printf("    -n<ame>=<process-name>     Receiver's process name     \n");
    ::printf("    -th<reeads>=<number>       Number of threads for underlying implementation\n");
  }

  struct NetSensor  {
    std::vector<unsigned char> m_buffer;
    bool  m_bounce;
    NET*  m_net;
    long  m_bytes;

    static void handle_data(const netheader_t& hdr, void* param, netentry_t* entry)   {
      NetSensor* s = (NetSensor*)param;
      s->handleMessage(hdr,entry);
    }
    void handleMessage(const netheader_t& hdr, netentry_t* entry)   {
      static struct timeval start;
      static int cnt = 0;
      int print_freq = cnt > 50000 ? 10000 : 1000;
      if ( 0 == cnt )  {
	::gettimeofday(&start,0);
      }
      if ( m_buffer.size() < hdr.size ) m_buffer.resize(hdr.size);
      net_receive(m_net, entry, m_buffer.data());
      m_bytes += hdr.size;
      if ( (++cnt)%print_freq == 0 )  {
	struct timeval now;
	::gettimeofday(&now,0);
	double diff = double(now.tv_sec-start.tv_sec)+double(now.tv_usec-start.tv_usec)/1e6;
	if ( diff == 0 ) diff = 1;
	double diff2 = double(m_bytes)/diff;
	printf ("%-4ld %s %6d messages rate:%9.3f kHz %9.3f MB/sec  total:%7.3f [sec]. [%s]\n",
		time(0)-start.tv_sec,m_bounce ? "Bounced" : "Received",
		cnt, double(cnt)/diff/1000.0, diff2/1024.0/1024.0, diff, hdr.name);
      }
      if ( m_bounce )    {
        int sc = net_send(m_net,m_buffer.data(),hdr.size,hdr.name,hdr.facility);
        if ( sc != NET_SUCCESS )  {
          ::printf("Failed to send message. ret=%d\n",sc);
        }
      }
    }
    static void handle_death(const netheader_t& hdr, void* /* param */, netentry_t* /* data */){
      ::printf("Task died: %s\n",hdr.name);
    }
    explicit NetSensor(const std::string& proc, int nthreads, bool bounce=false)
      : m_bounce(bounce), m_bytes(0)
    {
      net_subscribe(m_net=net_init(proc,nthreads),this,1,handle_data,handle_death);
    }
    virtual ~NetSensor()    {
      net_unsubscribe(m_net,this,1);
    }
  };

  static std::string host_name()  {
    char host[64];
    ::gethostname(host,sizeof(host));
    hostent* h = gethostbyname(host);
    const char* add = inet_ntoa(*(in_addr*)h->h_addr_list[0]);
    return add;
  }
#define Offseto
  template <typename A, typename B> int offset_of(const A* obj, const B* member)  {
    const unsigned char* m = (const unsigned char*)member;
    const unsigned char* o = (const unsigned char*)obj;
    return int(m-o);
  }
}

extern "C" int TRANSFERTEST(print_net_header) (int, char**)   {
  netheader_t * hdr = new netheader_t();
  ::printf("+-------------------------------------------------------------------------+\n");
  ::printf("|        %-60s     |\n",__func__);
  ::printf("| netheader_t: size:%d bytes.\n",int(sizeof(netheader_t)));
  ::printf("| netheader_t: offset 'size':      %-32d\n", offset_of(hdr,&hdr->size) );
  ::printf("| netheader_t: offset 'msg_type':  %-32d\n", offset_of(hdr,&hdr->msg_type) );
  ::printf("| netheader_t: offset 'facility':  %-32d\n", offset_of(hdr,&hdr->facility) );
  ::printf("| netheader_t: offset 'magic':     %-32d\n", offset_of(hdr,&hdr->magic) );
  ::printf("| netheader_t: offset 'hash':      %-32d\n", offset_of(hdr,&hdr->hash) );
  ::printf("| netheader_t: offset 'name':      %-32d\n", offset_of(hdr,hdr->name) );
  ::printf("+-------------------------------------------------------------------------+\n");
  ::lib_rtl_sleep(999999);
  return 1;
}

extern "C" int TRANSFERTEST(send) (int argc, char **argv)  {
  RTL::CLI cli(argc, argv, help_send);
  std::string target, name, proc, to;
  int count=1, length=256, loop=100000, num_thread=1;
  int bounce = cli.getopt("bounce",1) != 0 ? 1 : 0;

  cli.getopt("count",  1, count);
  cli.getopt("length", 1, length);
  cli.getopt("turns",  2, loop);
  cli.getopt("target", 2, target);
  cli.getopt("name",   1, name);
  cli.getopt("threads",2, num_thread);

  if ( length <= 0 ) length = 10;
  count = bounce ? 1 : count;
  proc = name.empty() ? host_name()+"::SND_0" : name;
  to   = target.empty() ? host_name()+"::RCV_0" : target;
  ::printf ("Test: %s\n",__FILE__);
  ::printf ("+-------------------------------------------------------------------------+\n");
  ::printf ("|        %-60s     |\n",__func__);
  ::printf ("+-------------------------------------------------------------------------+\n");
  ::printf (" Starting net sender: msg-len:%d turns:%d name:%s target:%s PID:%d\n",
	    length, loop, proc.c_str(), to.c_str(), ::lib_rtl_pid());
  long num_bytes = 0;
  int print_freq = loop*count > 100000 ? 10000 : 1000;
  while(count-- > 0)  {
    NetSensor cl(proc,num_thread,bounce);
    cl.m_buffer.resize(length);
    unsigned char *wmessage = cl.m_buffer.data();
    for (int k = 0; k < length; k++) wmessage[k] = char((length + k) & 0xFF);

    // receive some messages and bounce them
    struct timeval start;
    ::gettimeofday(&start,0);
    for (int i=0, mx=loop; mx > 0; --mx, ++i)  {
      int sc = net_send(cl.m_net, wmessage, length, to, 1);
      if (sc != NET_SUCCESS)
	::printf("Client::send Failed: Error=%d errno=%d [%s]\n",sc,errno,::strerror(errno));
      num_bytes += length;
      if ( ((i+1) % print_freq) == 0 )  {
	struct timeval now;
	::gettimeofday(&now,0);
	double diff = double(now.tv_sec-start.tv_sec)+double(now.tv_usec-start.tv_usec)/1e6;
	if ( diff == 0 ) diff = 1;
	double diff2 = double(num_bytes)/diff;
	::printf("%-4ld Sent %8d messages rate:%8.3f kHz %7.2f MB/sec  total:%6.2f sec\n",
		 time(0)-start.tv_sec,i+1,double(i+1)/diff/1000.0,diff2/1024.0/1024.0,diff);
      }
    }
    if ( bounce )  {
      while(1)  {
	::lib_rtl_sleep(100);
      }
    }
  }
  printf("Hit key+Enter to exit ...");
  getchar();
  return 0x1;
}

extern "C" int TRANSFERTEST(recv) (int argc, char **argv)  {
  RTL::CLI cli(argc, argv, help_recv);
  std::string proc;
  int run = 1, num_thread = 3, bounce = cli.getopt("bounce",1) != 0;
  cli.getopt("threads",2,num_thread);
  cli.getopt("name",1,proc);

  if (proc.empty() ) proc = host_name()+"::RCV_0";
  ::printf ("Test: %s\n",__FILE__);
  ::printf ("+-------------------------------------------------------------------------+\n");
  ::printf ("|        %-60s     |\n",__func__);
  ::printf ("+-------------------------------------------------------------------------+\n");
  ::printf (" Starting receiver:%s. Bounce:%s  PID:%d\n",
	    proc.c_str(), bounce ? "true" : "false", ::lib_rtl_pid());
  NetSensor cl(proc, num_thread, bounce);
  while(run)  {
    ::lib_rtl_sleep(100);
  }
  return 0x1;
}
#endif
