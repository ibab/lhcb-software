#include "MBM/MepConsumer.h"
#include "MBM/Producer.h"
#include "RTL/rtl.h"
#include "WT/wtdef.h"

namespace {
  static void help()  {
    ::lib_rtl_output(LIB_RTL_ALWAYS,"mep_holder_a -opt [-opt]\n");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -n=<name>              Buffer member name\n");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -a                     Asynchonous mode (default is synchronous)\n");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -p(artition)=<number>  Partition ID\n");
  }
  struct Holder  : public MEP::Consumer  {
    Holder(const std::string& nam, int partID) : MEP::Consumer(nam, partID)  {
      m_flags = USE_MEP_BUFFER;
      include();
      m_bmid = m_mepID->mepBuffer;
      unsigned int vetomask[4] = {0,0,0,0};
      unsigned int trmask[4]   = {~0U,~0U,~0U,~0U};
      addRequest(EVENT_TYPE_MEP,trmask,vetomask,BM_MASK_ANY,BM_REQ_ALL,BM_FREQ_PERC,100.);
      ::lib_rtl_output(LIB_RTL_INFO," MEP    buffer start: %08X\n",m_mepID->mepStart);
      ::lib_rtl_output(LIB_RTL_INFO," EVENT  buffer start: %08X\n",m_mepID->evtStart);
      ::lib_rtl_output(LIB_RTL_INFO," RESULT buffer start: %08X\n",m_mepID->resStart);
    }
    virtual int run() {
      bool exec = true;
      while(exec) {
	::mep_scan(m_mepID,0);
#ifdef _WIN32
	::lib_rtl_sleep(1);
#else
	::lib_rtl_usleep(10);
#endif
      }
      return 1;
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
  ::lib_rtl_output(LIB_RTL_ALWAYS,"%synchronous MEP Holder \"%s\" (pid:%d) included in buffers.\n",
     async ? "As" : "S", name.c_str(),Holder::pid());
  Holder c(name, partID);
  if ( async ) c.setNonBlocking(WT_FACILITY_DAQ_EVENT, true);
  return c.run();
}
