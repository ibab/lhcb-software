#include "RTL/rtl.h"
#include "WT/wtdef.h"
#include "MBM/Consumer.h"

namespace {
   static void help()  {
    ::lib_rtl_printf("mbm_cons_a -opt [-opt]\n");
    ::lib_rtl_printf("    -n=<name>              buffer member name\n");
    ::lib_rtl_printf("    -s=<number>            sleep interval between events [milli seconds]\n");
    ::lib_rtl_printf("    -b=<name>              Buffer identifier \n");
    ::lib_rtl_printf("    -p(artition)=<number>  Partition ID\n");
    ::lib_rtl_printf("    -q                     Quiet mode (do not print trigger number mismatch)\n");
  }
  struct Cons : public MBM::Consumer  {
    int nbad, trnumber, quiet;
    Cons(const std::string& buff,const std::string& nam, int pid, bool q) 
      : MBM::Consumer(buff,nam,pid), nbad(0), trnumber(-1), quiet(q) {
      unsigned int vetomask[4] = {0,0,0,0};
      unsigned int trmask[4]   = {~0x0,~0x0,~0x0,~0x0};
      addRequest(1,trmask,vetomask,BM_MASK_ANY,BM_REQ_VIP,BM_FREQ_PERC,100.);
      setNonBlocking(WT_FACILITY_DAQ_EVENT, true);
    }
    int eventAction() {
      const MBM::EventDesc& e = event();
      if ( trnumber == -1 )  {
        trnumber = *e.data;
      }
      else if( trnumber != *e.data ) {
        if ( !quiet )  {
          ::lib_rtl_printf("======= Mismatch [%d] found %d %d [0x%p]\n",++nbad, trnumber, *e.data, (void*)e.data);
        }
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
  int partID = 0x103;
  bool quiet = cli.getopt("quiet",1) != 0;
  cli.getopt("name",1,name);
  cli.getopt("buffer",1,buffer);
  cli.getopt("partitionid",1,partID);
  int status = wtc_init();
  if( status != WT_SUCCESS ) exit(status);
  ::lib_rtl_printf("Asynchronous Consumer \"%s\" (pid:%d) running in buffer:\"%s\"\n",name.c_str(),Cons::pid(),buffer.c_str());
  return Cons(buffer,name,partID,quiet).run();
}
