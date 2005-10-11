#include "RTL/rtl.h"
#include "MBM/Consumer.h"

static void help()  {
  ::printf("mbm_cons_one -opt [-opt]\n");
  ::printf("    -n=<name>      buffer member name\n");
  ::printf("    -s=<number>    sleep interval between events [milli seconds]\n");
}

extern "C" int mbm_cons_one(int argc,char **argv) {
  int  trnumber  = -1, nbad = 0, sleep_msecs = 0;
  int  trmask[4] = {-1,-1,-1,-1};
  int  vetomask[4] = {0,0,0,0};

  RTL::CLI cli(argc, argv, help);
  std::string name = "cons_one";
  cli.getopt("name", 1,name);
  cli.getopt("sleep",1,sleep_msecs);

  MBM::Consumer c("0",name,0x103);
  ::printf("Consumer \"%s\" (pid:%d) including buffer:\"%s\"\n",name.c_str(),c.pid(),"0");
  c.addRequest(1,trmask,vetomask,BM_MASK_ANY,BM_REQ_ONE,BM_FREQ_PERC,100.);
  while(1)  {
    c.getEvent();
    const MBM::EventDesc& e = c.event();
    if ( -1 == trnumber )  {
      trnumber = *e.data;
    }
    else if( trnumber != *e.data ) {
      //::printf("======= Mismatch [%d] found %d %d [0x%p]\n", nbad++, trnumber, *e.data, e.data);
      trnumber = *e.data;
    }
    trnumber++;
    if ( sleep_msecs ) lib_rtl_sleep(sleep_msecs);
    c.freeEvent();
  }
}
