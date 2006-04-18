#include "MBM/MepConsumer.h"
#include "MBM/Producer.h"
#include "RTL/rtl.h"
#include "WT/wtdef.h"
#include <cctype>

namespace {
  static void help()  {
    ::lib_rtl_printf("mep_cons_a -opt [-opt]\n");
    ::lib_rtl_printf("    -n=<name>              buffer member name\n");
    ::lib_rtl_printf("    -i=<name>              input buffer name\n");
    ::lib_rtl_printf("    -p(artition)=<number>  Partition ID\n");
    ::lib_rtl_printf("    -e(venttype)=<number>  Event type for request (2=EVENT, 1=MEP)\n");
    ::lib_rtl_printf("    -a                     Asynchonous mode (default is synchronous)\n");
  }
  struct Cons : public MEP::Consumer  {
    Cons(const std::string& nam, const std::string& input, int pid, int evtyp)
    : MEP::Consumer(nam, pid)
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
          lib_rtl_printf("Bad input buffer name given:\"%s\"\n",input.c_str());
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
      unsigned int vetomask[4] = {0,0,0,0};
      unsigned int trmask[4]   = {~0x0,~0x0,~0x0,~0x0};
      addRequest(evtyp,trmask,vetomask,BM_MASK_ANY,BM_REQ_ALL,BM_FREQ_PERC,100.);
      ::lib_rtl_printf(" EVENT Type (2=EVENT, 1=MEP): %d\n", evtyp);
      ::lib_rtl_printf(" MEP    buffer start: %08X\n",m_mepID->mepStart);
      ::lib_rtl_printf(" EVENT  buffer start: %08X\n",m_mepID->evtStart);
      ::lib_rtl_printf(" RESULT buffer start: %08X\n",m_mepID->resStart);
      // mep_print_release(true);
    }
    int eventAction() {
      const MBM::EventDesc& evt = event();
      MEP_SINGLE_EVT* ev = (MEP_SINGLE_EVT*)evt.data;
      MEPEVENT*       e  = (MEPEVENT*)(int*)(m_mepID->mepStart + ev->begin);
      if ( e->valid == 0 || e->magic != mep_magic_pattern() || e->refCount<=1 ) {
        ::lib_rtl_printf("Found invalid MEP: %p [%d] magic:%08X id=%d refCount:%d valid:%d\n",
          (void*)e,e->evID,e->magic,e->evID,e->refCount,e->valid);
      }
      return Consumer::eventAction();
    }
  };
}

extern "C" int mep_cons_a(int argc,char **argv) {
  RTL::CLI cli(argc, argv, help);
  int partID = 0x103;
  int evtyp = EVENT_TYPE_EVENT;
  std::string name = "consumer", input="MEP";
  bool async = cli.getopt("asynchronous",1) != 0;
  cli.getopt("name",1,name);
  cli.getopt("input",1,input);
  cli.getopt("eventtype",1,evtyp);
  cli.getopt("partitionid",1,partID);
  ::lib_rtl_printf("%synchronous MEP Consumer \"%s\" (pid:%d) included in buffers.\n",
	   async ? "As" : "S", name.c_str(),Cons::pid());
  Cons c(name,input,partID,evtyp);
  if ( async ) c.setNonBlocking(WT_FACILITY_DAQ_EVENT, true);
  return c.run();
}
