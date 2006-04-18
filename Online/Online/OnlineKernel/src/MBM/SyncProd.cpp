#include "RTL/rtl.h"
#include "MBM/Producer.h"

static void help()  {
  ::lib_rtl_printf("mbm_prod -opt [-opt]\n");
  ::lib_rtl_printf("    -n=<name>      buffer member name\n");
  ::lib_rtl_printf("    -m=<number>    number of events\n");
  ::lib_rtl_printf("    -s=<number>    event size [bytes]\n");
  ::lib_rtl_printf("    -b=<name>      Buffer identifier \n");
}

extern "C" int mbm_prod(int argc,char **argv) {
  RTL::CLI cli(argc, argv, help);
  std::string name = "producer", buffer="0";
  int trnumber = -1, len = 1792, nevt = 1000000;
  cli.getopt("name",1,name);
  cli.getopt("m",1,nevt);
  cli.getopt("size",1,len);
  cli.getopt("buffer",1,buffer);
  MBM::Producer p(buffer,name,0x103);
  ::lib_rtl_printf("Producer \"%s\" (pid:%d) included in buffer:\"%s\" len=%d nevt=%d\n",
      name.c_str(), MBM::Producer::pid(), buffer.c_str(), len, nevt);
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
      // lib_rtl_sleep(1);
   }
  }
  //exit(0);
  return 1;
}
