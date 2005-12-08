#include "MBM/MepConsumer.h"
#include "MBM/Producer.h"
#include "RTL/rtl.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"
#include <ctype.h>

namespace {
  static void help()  {
    ::printf("mep_prod_a -opt [-opt]\n");
    ::printf("    -n=<name>      buffer member name\n");
    ::printf("    -i=<name>      input buffer name\n");
    ::printf("    -a             Asynchonous mode (default is synchronous)\n");
  }
  struct Cons : public MEP::Consumer  {
    Cons(const std::string& nam, const std::string& input) : MEP::Consumer(nam, 0x103)
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
      ::printf(" MEP    buffer start: %08X\n",m_mepID->mepStart);
      ::printf(" EVENT  buffer start: %08X\n",m_mepID->evtStart);
      ::printf(" RESULT buffer start: %08X\n",m_mepID->resStart);
    }
    int eventAction() {
      const MBM::EventDesc& evt = event();
      MEP_SINGLE_EVT* ev = (MEP_SINGLE_EVT*)evt.data;
      MEPEVENT*       e  = (MEPEVENT*)(int*)(m_mepID->mepStart + ev->begin);
      if ( e->valid == 0 ) {
	::printf("Found invalid MEP: %p [%d]\n",(void*)e,e->mepBufferID);
      }
      return Consumer::eventAction();
    }
  };
}

extern "C" int mep_cons_a(int argc,char **argv) {
  RTL::CLI cli(argc, argv, help);
  std::string name = "consumer", input="MEP";
  bool async = cli.getopt("asynchronous",1) != 0;
  cli.getopt("name",1,name);
  cli.getopt("input",1,input);
  ::printf("%synchronous MEP Consumer \"%s\" (pid:%d) included in buffers.\n",
	   async ? "As" : "S", name.c_str(),Cons::pid());
  Cons c(name,input);
  if ( async ) c.setNonBlocking(WT_FACILITY_DAQ_EVENT, true);
  return c.run();
}
