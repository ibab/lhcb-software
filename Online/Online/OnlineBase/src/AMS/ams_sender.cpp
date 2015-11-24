#include <cstdio>
#include <string>
#include <cstring>
#include "RTL/rtl.h"
#include "WT/wtdef.h"
#include "AMS/amsdef.h"

namespace {
  void fill (char *buff,int len)    {
    for (int i = 0; i < len; i++)
      buff [i] = (char)((len + i) & 0xFF);
  }
  void help() {
    ::printf("amsc_sender -opt [-opt]\n");
    ::printf("    -n=<name>              AMS task name\n");
    ::printf("    -t=<number>            Number of receive/send turns\n");
    ::printf("    -s=<target process>    Client mode (default:server)\n");
    ::printf("    -l=<number>            Message length (Client only)\n");
  }
}

extern "C" int amsc_sender(int argc, char **argv)  {
  std::string amsname("SEND_0"), target;
  char node[20], source[80], *wmessage;
  int wsize, length=256, loop=100000;
  unsigned int facility=WT_FACILITY_DAQ_EVENT;

  RTL::CLI cli(argc, argv, help);
  cli.getopt("sendto",1,target);
  cli.getopt("name",1,amsname);
  cli.getopt("length",1,length);
  cli.getopt("facility",1,facility);
  cli.getopt("turns",1,loop);

  if ( length<=0 ) length=10;
  wmessage = new char[length];
  ::printf (" Starting ams test task (%s) \n",amsname.c_str());
  ::strncpy(source,target.c_str(),sizeof(source));
  source[sizeof(source)-1] = 0;
  ::printf(" task %s: size %d  facility:%d turns:%d\n",source,length,int(facility),loop);
  int sc = ::amsc_init (amsname.c_str());
  if (sc != AMS_SUCCESS)  {
    ::printf ("Can't initialise ams\n");
    ::exit (sc);
  }
  ::amsc_get_node( node );

  // receive some messages and bounce them
  for (int i=0; loop > 0; --loop, ++i)  {
    //if ( (i%5) == 0 ) lib_rtl_sleep(1);
    wsize =  length;
    fill (wmessage, wsize);
    sc = ::amsc_send_message (wmessage, wsize, source, facility, 0);
    if (sc != AMS_SUCCESS)  {
      ::printf("amsc_send_message Failed: Error=%d\n",sc);
    }
    if (i % 100 == 0) printf ("%d\n", i);
  }
  ::printf("Hit key+Enter to exit ...");
  ::getchar();
  ::amsc_close();
  delete [] wmessage;
  return 0;
}
