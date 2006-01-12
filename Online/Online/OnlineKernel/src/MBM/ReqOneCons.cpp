#include "RTL/rtl.h"
#include "MBM/Consumer.h"

static void help()  {
  ::printf("mbm_cons_one -opt [-opt]\n");
  ::printf("    -n=<name>      buffer member name\n");
  ::printf("    -s=<number>    sleep interval between events [milli seconds]\n");
  ::printf("    -b=<name>      Buffer identifier \n");
}

extern "C" int mbm_cons_one(int argc,char **argv) {
  int  trnumber  = -1, sleep_msecs = 0;
  unsigned int  trmask[4] = {~0x0,~0x0,~0x0,~0x0};
  unsigned int  vetomask[4] = {0,0,0,0};

  RTL::CLI cli(argc, argv, help);
  std::string name = "cons_one", buffer="0";
  cli.getopt("name", 1,name);
  cli.getopt("sleep",1,sleep_msecs);
  cli.getopt("buffer",1,buffer);

  MBM::Consumer c(buffer,name,0x103);
  ::printf("Consumer \"%s\" (pid:%d) including buffer:\"%s\"\n",name.c_str(),c.pid(),buffer.c_str());
  c.addRequest(1,trmask,vetomask,BM_MASK_ANY,BM_REQ_ONE,BM_FREQ_PERC,100.);
  while( c.getEvent()==MBM_NORMAL )  {
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
  return 1;
}
