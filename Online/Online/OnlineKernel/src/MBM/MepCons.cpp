#include "MBM/MepConsumer.h"
#include "MBM/Producer.h"
#include "RTL/rtl.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"

namespace {
  static void help()  {
    ::printf("mep_prod_a -opt [-opt]\n");
    ::printf("    -n=<name>      buffer member name\n");
    ::printf("    -i=<name>      input buffer name\n");
  }
  struct Holder  : public MEP::Consumer  {
    Holder(const std::string& nam, const std::string& input)
    : MEP::Consumer(nam, 0x103)
    {
      m_flags = 0;
      switch(::toupper(input[0]))  {
        case 'E':
          m_flags |= USE_EVT_BUFFER;
          break;
        case 'M':
          m_flags |= USE_MEP_BUFFER;
          break;
        case 'R':
          m_flags |= USE_RES_BUFFER;
          break;
        default:
          printf("Bad input buffer name given:\"%s\"\n",input.c_str());
          help();
          exit(0);
      }
      include();
      switch(::toupper(input[0]))  {
        case 'E':
          m_bmid = m_mepID->evtBuffer;
          break;
        case 'R':
          m_bmid = m_mepID->resBuffer;
          break;
        case 'M':
          m_bmid = m_mepID->mepBuffer;
          break;
      }
      int vetomask[4] = {0,0,0,0};
      int trmask[4]   = {-1,-1,-1,-1};
      addRequest(1,trmask,vetomask,BM_MASK_ANY,BM_REQ_ALL,BM_FREQ_PERC,100.);
      setNonBlocking(WT_FACILITY_DAQ_EVENT, true);
      ::printf(" MEP    buffer start: %p\n",m_mepID->mepStart);
      ::printf(" EVENT  buffer start: %p\n",m_mepID->evtStart);
      ::printf(" RESULT buffer start: %p\n",m_mepID->resStart);
    }
    int eventAction() {
      const MBM::EventDesc& evt = event();
      return Consumer::eventAction();
    }
  };
}

extern "C" int mep_cons_a(int argc,char **argv) {
  RTL::CLI cli(argc, argv, help);
  std::string name = "consumer", input="MEP";
  cli.getopt("name",1,name);
  cli.getopt("input",1,input);
  ::printf("Asynchronous MEP Holder \"%s\" (pid:%d) included in buffers.\n",name.c_str(),Holder::pid());
  return Holder(name,input).run();
}
