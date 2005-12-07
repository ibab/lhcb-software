#include "MBM/MepProducer.h"
#include "MBM/Producer.h"
#include "RTL/rtl.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"

#define PACKING_FACTOR 20
namespace {
  static void help()  {
    ::printf("mep_prod_a -opt [-opt]\n");
    ::printf("    -n=<name>      buffer member name\n");
  }
  struct Prod  : public MEP::Producer  {
    int mep_identifier;
    MBM::Producer* m_evtProd;
    Prod(const std::string& nam) : MEP::Producer(nam, 0x103), mep_identifier(0)   
    {
      m_flags = USE_EVT_BUFFER+USE_MEP_BUFFER;
      include();
      m_bmid = m_mepID->mepBuffer;
      m_evtProd = new MBM::Producer(m_mepID->evtBuffer, nam, partitionID());
      ::printf(" MEP    buffer start: %08X\n",m_mepID->mepStart);
      ::printf(" EVENT  buffer start: %08X\n",m_mepID->evtStart);
      ::printf(" RESULT buffer start: %08X\n",m_mepID->resStart);
      setNonBlocking(WT_FACILITY_DAQ_SPACE, true);
    }
    ~Prod()  {
      delete m_evtProd;
    }
    int spaceRearm(int) {
      return Producer::spaceRearm(MEPEVENT::length(PACKING_FACTOR));
    }
    int spaceAction() {
      MEPEVENT* ev = (MEPEVENT*)event().data;
      MEP_SINGLE_EVT* sub_events = ev->events;
      size_t sub_evt_len = sizeof(MEP_SINGLE_EVT);
      mep_identifier++;
      //printf("MEP address: %p\n",m_event.data);

      ev->refCount    = 1;
      ev->mepBufferID = mep_identifier;
      ev->begin       = int(int(ev)-m_mepID->mepStart);
      ev->packing     = PACKING_FACTOR;
      for (int j = 0; j < PACKING_FACTOR; ++j )   {
        ev->events[j].event = int(-m_mepID->mepStart+(int)&sub_events[j]);
        ev->events[j].begin = ev->begin;
      }
      m_event.mask[0] = 0x103;
      m_event.mask[1] = 0;
      m_event.mask[2] = 0;
      m_event.mask[3] = 0;
      m_event.type    = 1;
      for (int i = 0; i < PACKING_FACTOR; ++i )   {
        if ( m_evtProd->getSpace(sub_evt_len) == MBM_NORMAL ) {
          MBM::EventDesc& e = m_evtProd->event();
          ::memcpy(e.data, &ev->events[i], sizeof(ev->events[i]));
          e.type    = 1;
          e.mask[0] = 0x103;
          e.mask[1] = 0;
          e.mask[2] = 0;
          e.mask[3] = 0;
          e.len     = sub_evt_len;
          m_evtProd->sendEvent();
        }
        else  {
          printf("Space error !\n");
        }
      }
      return Producer::spaceAction();
    }
  };
}

extern "C" int mep_prod_a(int argc,char **argv) {
  RTL::CLI cli(argc, argv, help);
  std::string name = "producer";
  cli.getopt("name",1,name);
  ::printf("Asynchronous Producer \"%s\" (pid:%d) included in buffers.\n",name.c_str(),Prod::pid());
  return Prod(name).run();
}
