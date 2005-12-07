#include "MBM/MepConsumer.h"
#include "MBM/Producer.h"
#include "RTL/rtl.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"

namespace {
  static void help()  {
    ::printf("mep_holder_a -opt [-opt]\n");
    ::printf("    -n=<name>      buffer member name\n");
  }
  struct Holder  : public MEP::Consumer  {
    Holder(const std::string& nam) : MEP::Consumer(nam, 0x103)  {
      m_flags = USE_MEP_BUFFER;
      include();
      m_bmid = m_mepID->mepBuffer;
      int vetomask[4] = {0,0,0,0};
      int trmask[4]   = {-1,-1,-1,-1};
      addRequest(1,trmask,vetomask,BM_MASK_ANY,BM_REQ_ALL,BM_FREQ_PERC,100.);
      setNonBlocking(WT_FACILITY_DAQ_EVENT, true);
      ::printf(" MEP    buffer start: %08X\n",m_mepID->mepStart);
      ::printf(" EVENT  buffer start: %08X\n",m_mepID->evtStart);
      ::printf(" RESULT buffer start: %08X\n",m_mepID->resStart);
      mep_set_watch(m_mepID);
    }
  };
}

extern "C" int mep_holder_a(int argc,char **argv) {
  RTL::CLI cli(argc, argv, help);
  std::string name = "holder";
  cli.getopt("name",1,name);
  ::printf("Asynchronous MEP Holder \"%s\" (pid:%d) included in buffers.\n",name.c_str(),Holder::pid());
  return Holder(name).run();
}
