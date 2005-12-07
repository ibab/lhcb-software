#include "MBM/MepConsumer.h"
#include "MBM/Producer.h"
#include "RTL/rtl.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"

namespace {
  static void help()  {
    ::printf("mep_reform_a -opt [-opt]\n");
    ::printf("    -n=<name>      buffer member name\n");
    ::printf("    -i=<name>      input buffer name\n");
    ::printf("    -o=<name>      output buffer name\n");
  }
  struct Cons  : public MEP::Consumer  {
    MBM::Producer* m_evtProd;
    Cons(const std::string& nam, const std::string& in, const std::string& out)
      : MEP::Consumer(nam, 0x103), m_evtProd(0)
    {
      BMID bm = MBM_INV_DESC;
      m_flags = 0;
      switch(::toupper(in[0]))  {
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
          printf("Bad input buffer name given:\"%s\"\n",in.c_str());
          help();
          exit(0);
      }
      switch(::toupper(out[0]))  {
        case 'M':
          m_flags |= USE_MEP_BUFFER;
          break;
        case 'E':
          m_flags |= USE_EVT_BUFFER;
          break;
        case 'R':
          m_flags |= USE_RES_BUFFER;
          break;
      }
      include();
      switch(::toupper(in[0]))  {
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
      switch(::toupper(out[0]))  {
        case 'M':
          bm = m_mepID->mepBuffer;
          break;
        case 'E':
          bm = m_mepID->evtBuffer;
          break;
        case 'R':
          bm = m_mepID->resBuffer;
          break;
      }
      if ( bm ) m_evtProd = new MBM::Producer(bm, nam, partitionID());
      int vetomask[4] = {0,0,0,0};
      int trmask[4]   = {-1,-1,-1,-1};
      addRequest(1,trmask,vetomask,BM_MASK_ANY,BM_REQ_ONE,BM_FREQ_PERC,100.);
      setNonBlocking(WT_FACILITY_DAQ_EVENT, true);
      ::printf(" MEP    buffer start: %p\n",m_mepID->mepStart);
      ::printf(" EVENT  buffer start: %p\n",m_mepID->evtStart);
      ::printf(" RESULT buffer start: %p\n",m_mepID->resStart);
    }
    ~Cons()  {
      if ( m_evtProd ) delete m_evtProd;
    }
    int eventAction() {
      const MBM::EventDesc& evt = event();
      size_t sub_evt_len = sizeof(MEP_SINGLE_EVT)+1024;
      if ( m_evtProd )   {
        if ( m_evtProd->getSpace(sub_evt_len) == MBM_NORMAL ) {
          MBM::EventDesc& e = m_evtProd->event();
          *(MEP_SINGLE_EVT*)e.data  = *(MEP_SINGLE_EVT*)evt.data;
          memcpy(e.mask, evt.mask, sizeof(e.mask));
          e.type    = evt.type;
          e.len     = sub_evt_len;
          m_evtProd->sendEvent();
        }
        else  {
          printf("Space error !\n");
        }
      }
      return Consumer::eventAction();
    }
  };
}

extern "C" int mep_reform_a(int argc,char **argv) {
  RTL::CLI cli(argc, argv, help);
  std::string name = "reformatter", input="EVT", output="RES";
  cli.getopt("name",1,name);
  cli.getopt("input",1,input);
  cli.getopt("output",1,output);
  ::printf("Asynchronous Reformatter \"%s\" (pid:%d) included in buffers.\n",name.c_str(),Cons::pid());
  return Cons(name,input,output).run();
}
