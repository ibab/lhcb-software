#include "RTL/rtl.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"
#include "MBM/Consumer.h"

namespace {
   static void help()  {
    ::printf("mbm_cons_a -opt [-opt]\n");
    ::printf("    -n=<name>      buffer member name\n");
    ::printf("    -s=<number>    sleep interval between events [milli seconds]\n");
    ::printf("    -b=<name>      Buffer identifier \n");
  }
  struct Cons : public MBM::Consumer  {
    int nbad, trnumber;
    Cons(const std::string& buff,const std::string& nam) : MBM::Consumer(buff,nam,0x103), nbad(0), trnumber(-1) {
      int vetomask[4] = {0,0,0,0};
      int trmask[4]   = {-1,-1,-1,-1};
      addRequest(1,trmask,vetomask,BM_MASK_ANY,BM_REQ_VIP,BM_FREQ_PERC,100.);
      setNonBlocking(WT_FACILITY_DAQ_EVENT, true);
    }
    int eventAction() {
      const MBM::EventDesc& e = event();
      if ( trnumber == -1 )  {
        trnumber = *e.data;
      }
      else if( trnumber != *e.data ) {
        ::printf("======= Mismatch [%d] found %d %d [0x%p]\n",++nbad, trnumber, *e.data, (void*)e.data);
        trnumber = *e.data;
      }
      trnumber++;
      return Consumer::eventAction();
    }
  };
}

extern "C" int mbm_cons_a(int argc,char **argv) {  
  RTL::CLI cli(argc, argv, help);
  std::string name = "consumer", buffer="0";
  cli.getopt("name",1,name);
  cli.getopt("buffer",1,buffer);
  int status = wtc_init();
  if( status != WT_SUCCESS ) exit(status);
  ::printf("Asynchronous Consumer \"%s\" (pid:%d) running in buffer:\"%s\"\n",name.c_str(),Cons::pid(),buffer.c_str());
  return Cons(buffer,name).run();
}
