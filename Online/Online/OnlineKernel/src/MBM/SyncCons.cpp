#include "RTL/rtl.h"
#include "MBM/Consumer.h"

static void help()  {
  ::lib_rtl_printf("mbm_cons -opt [-opt]\n");
  ::lib_rtl_printf("    -n=<name>      buffer member name\n");
  ::lib_rtl_printf("    -s=<number>    sleep interval between events [milli seconds]\n");
  ::lib_rtl_printf("    -b=<name>      Buffer identifier \n");
  ::lib_rtl_printf("    -q             Quiet mode (do not print trigger number mismatch)\n");
}

extern "C" int mbm_cons(int argc,char **argv) {
  RTL::CLI cli(argc, argv, help);
  std::string name = "consumer", buffer="0";
  int  trnumber = -1, nbad = 0, sleep_msecs = 0;
  unsigned int  trmask[4] = {~0x0,~0x0,~0x0,~0x0};
  unsigned int  vetomask[4] = {0,0,0,0};
  bool quiet = cli.getopt("quiet",1) != 0;
  cli.getopt("name",1,name);
  cli.getopt("sleep",1,sleep_msecs);
  cli.getopt("buffer",1,buffer);

  MBM::Consumer c(buffer,name,0x103);
  ::lib_rtl_printf("Consumer \"%s\" (pid:%d) including buffer:\"%s\"\n",name.c_str(),c.pid(),buffer.c_str());
  c.addRequest(1,trmask,vetomask,BM_MASK_ANY,BM_REQ_VIP,BM_FREQ_PERC,100.);
  while( c.getEvent() )  {
    const MBM::EventDesc& e = c.event();
    if ( -1 == trnumber )  {
      trnumber = *e.data;
    }
    else if( trnumber != *e.data ) {
      if ( !quiet )  {
        ::lib_rtl_printf("======= Mismatch [%d] found %d %d [0x%p]\n", 
	         nbad++, trnumber, *e.data, (void*)e.data);
      }
      trnumber = *e.data;
    }
    if ( sleep_msecs ) lib_rtl_sleep(sleep_msecs);
    trnumber++;
    c.freeEvent();
  }
  return 1;
}
