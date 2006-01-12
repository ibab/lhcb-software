#include "MBM/MepConsumer.h"
#include "MBM/Producer.h"
#include "RTL/rtl.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"

namespace {
  static void help()  {
    ::printf("mep_holder_a -opt [-opt]\n");
    ::printf("    -n=<name>              Buffer member name\n");
    ::printf("    -a                     Asynchonous mode (default is synchronous)\n");
    ::printf("    -p(artition)=<number>  Partition ID\n");
  }
  struct Holder  : public MEP::Consumer  {
    Holder(const std::string& nam, int partID) : MEP::Consumer(nam, partID)  {
      m_flags = USE_MEP_BUFFER;
      include();
      m_bmid = m_mepID->mepBuffer;
      unsigned int vetomask[4] = {0,0,0,0};
      unsigned int trmask[4]   = {~0x0,~0x0,~0x0,~0x0};
      addRequest(EVENT_TYPE_MEP,trmask,vetomask,BM_MASK_ANY,BM_REQ_ALL,BM_FREQ_PERC,100.);
      ::printf(" MEP    buffer start: %08X\n",m_mepID->mepStart);
      ::printf(" EVENT  buffer start: %08X\n",m_mepID->evtStart);
      ::printf(" RESULT buffer start: %08X\n",m_mepID->resStart);
      // mep_set_watch(m_mepID);
      // mbm_register_free_event(m_bmid, 0, 0);
    }
    virtual int eventAction() {
      const MBM::EventDesc& evt = event();
      MEPEVENT* e = (MEPEVENT*)(int*)evt.data;
      if ( e->magic != mep_magic_pattern() )  {
        printf("Bad magic MEP pattern !!!!\n");
      }
      while ( e->refCount > 1 )  {
#ifdef _WIN32
        lib_rtl_sleep(1);
#else
        lib_rtl_usleep(100);
#endif
      }
      e->valid = 0;
      e->refCount = 0;
      return Consumer::eventAction();
    }
  };
}

extern "C" int mep_holder_a(int argc,char **argv) {
  RTL::CLI cli(argc, argv, help);
  std::string name = "holder";
  int partID = 0x103;
  bool async = cli.getopt("asynchronous",1) != 0;
  cli.getopt("name",1,name);
  cli.getopt("partitionid",1,partID);
  ::printf("%synchronous MEP Holder \"%s\" (pid:%d) included in buffers.\n",
	   async ? "As" : "S", name.c_str(),Holder::pid());
  Holder c(name, partID);
  if ( async ) c.setNonBlocking(WT_FACILITY_DAQ_EVENT, true);
  return c.run();
}
