#include <cstdio>
#include <string>
#include "RTL/rtl.h"
#include "MBM/Producer.h"

static void help()  {
  ::printf("mbm_prod -opt [-opt]\n");
  ::printf("    -n=<name>      buffer member name\n");
  ::printf("    -m=<number>    number of events\n");
  ::printf("    -s=<number>    event size [bytes]\n");
}

extern "C" int mbm_prod(int argc,char **argv) {
  RTL::CLI cli(argc, argv, help);
  std::string name = "producer";
  int trnumber = -1, len = 1792, nevt = 1000000;
  cli.getopt("name",1,name);
  cli.getopt("m",1,nevt);
  cli.getopt("size",1,len);
  MBM::Producer p("0",name,0x103);
  ::printf("Producer \"%s\" (pid:%d) included in buffer:\"%s\" len=%d nevt=%d\n",
	   name.c_str(), MBM::Producer::pid(), "0", len, nevt);
  while(nevt--)  {
    if ( p.getSpace(len) == MBM_NORMAL ) {
      MBM::EventDesc& e = p.event();
      *e.data   = ++trnumber;
      e.type    = 1;
      e.mask[0] = 0x103;
      e.mask[1] = 0;
      e.mask[2] = 0;
      e.mask[3] = 0;
      e.len     = len;
      p.sendEvent();
   }
  }
  //exit(0);
  return 1;
}
