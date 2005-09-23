#include <cstdio>
#include "MBM/Producer.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"

namespace {
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
  const char *name = argc>1 ? argv[1] : "producer";  
  int status = wtc_init();
  if( status != WT_SUCCESS ) exit(status);
  ::printf("Asynchronous Producer \"%s\" (pid:%d) included in buffer:\"%s\"\n",name,Prod::pid(),"0");
  return Prod(name).run();
}
