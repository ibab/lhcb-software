#include "MBM/Producer.h"
#include "RTL/rtl.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"

namespace {
  static void help()  {
    ::printf("mbm_prod_a -opt [-opt]\n");
    ::printf("    -n=<name>      buffer member name\n");
  }
  struct Prod  : public MBM::Producer  {
    int trnumber;
    Prod(const std::string& nam) : MBM::Producer("0",nam,0x103), trnumber(0)   {
      setNonBlocking(WT_FACILITY_DAQ_SPACE, true);
    }
    int spaceRearm(int) {
      return Producer::spaceRearm(1792);
    }
    int spaceAction() {
      *m_event.data  = trnumber++;
      m_event.mask[0] = 0x103;
      m_event.mask[1] = 0;
      m_event.mask[2] = 0;
      m_event.mask[3] = 0;
      m_event.type    = 1;
      return Producer::spaceAction();
    }
  };
}

extern "C" int mbm_prod_a(int argc,char **argv) {
  RTL::CLI cli(argc, argv, help);
  std::string name = "producer";
  cli.getopt("name",1,name);
  int status = wtc_init();
  if( status != WT_SUCCESS ) exit(status);
  ::printf("Asynchronous Producer \"%s\" (pid:%d) included in buffer:\"%s\"\n",name.c_str(),Prod::pid(),"0");
  return Prod(name).run();
}
