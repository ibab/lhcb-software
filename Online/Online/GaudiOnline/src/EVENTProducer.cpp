#include "MBM/Producer.h"
#include "MBM/MepConsumer.h"
#include "WT/wt_facilities.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawEventDescriptor.h"

namespace {
  static void help()  {
    ::printf("mep_evtprod -opt [-opt]\n");
    ::printf("    -n(ame)=<name>         buffer member name\n");
    ::printf("    -a(asynchronous)       Asynchonous mode (default is synchronous)\n");
    ::printf("    -p(artition)=<number>  Partition ID\n");
  }
  struct EVENTGenerator  : public MEP::Consumer  {
    typedef std::vector<LHCb::MEPFragment*>    Fragments;
    typedef std::map<unsigned int, Fragments > Events;
    bool prt;

    MBM::Producer* m_evtProd;
    EVENTGenerator(const std::string& nam, int partID)
    : MEP::Consumer(nam, partID), m_evtProd(0)
    {
      prt = false;
      unsigned int vetomask[4] = {0,0,0,0};
      unsigned int trmask[4]   = {~0x0,~0x0,~0x0,~0x0};
      m_flags = USE_EVT_BUFFER|USE_MEP_BUFFER;
      include();
      m_bmid = m_mepID->mepBuffer;
      m_evtProd = new MBM::Producer(m_mepID->evtBuffer, nam, partitionID());
      addRequest(EVENT_TYPE_MEP,trmask,vetomask,BM_MASK_ANY,BM_REQ_VIP,BM_FREQ_PERC,100.);
      ::printf(" MEP    buffer start: %08X\n",m_mepID->mepStart);
      ::printf(" EVENT  buffer start: %08X\n",m_mepID->evtStart);
      ::printf(" RESULT buffer start: %08X\n",m_mepID->resStart);
      mbm_register_free_event(m_bmid, 0, 0);
      mbm_register_alloc_event(m_mepID->evtBuffer,0,0);
    }
    ~EVENTGenerator()  {
      if ( m_evtProd ) delete m_evtProd;
    }
    void declareSubEvents(const EventDesc& evt, Events& events)  {
      if ( prt ) ::printf("Declare MEP....\n");
      for(Events::const_iterator i=events.begin(); i!=events.end(); ++i)  {
        declareSubEvent(evt, (*i).second);
      }
    }
    void declareSubEvent(const EventDesc& evt, const Fragments& frags)  {
      int sub_evt_len = LHCb::RawEventHeader::size(frags.size());
      if ( prt ) ::printf("0-Declare MEP fragment....\n");
      if ( m_evtProd->getSpace(sub_evt_len) == MBM_NORMAL ) {
        if ( prt ) ::printf("1-Declare MEP fragment....\n");
        EventDesc& e = m_evtProd->event();
        MEPEVENT* ev = (MEPEVENT*)evt.data;
        LHCb::RawEventHeader* h = (LHCb::RawEventHeader*)e.data;
        h->setDataStart(ev->begin);
        h->setNumberOfFragments(frags.size());
        h->setErrorMask(0);
        h->setNumberOfMissing(0);
        h->setOffsetOfMissing(0);
        for(size_t j=0; j<frags.size(); ++j)  {
          h->setOffset(j, int(int(frags[j])-m_mepID->mepStart));
        }
        e.mask[0] = evt.mask[0];
        e.mask[1] = evt.mask[1];
        e.mask[2] = evt.mask[2];
        e.mask[3] = evt.mask[3];
        e.type    = EVENT_TYPE_EVENT;
        e.len     = sub_evt_len;
        if ( prt ) ::printf("2-Declare MEP fragment....\n");
        m_evtProd->sendEvent();
        if ( prt ) ::printf("3-Declare MEP fragment....\n");
      }
      else  {
        ::printf("Space error !\n");
      }
    }
    int eventAction() {
      Events events;
      unsigned int partID = 0;
      const EventDesc& evt = event();
      MEPEVENT* ev = (MEPEVENT*)evt.data;
      LHCb::MEPEvent* me = (LHCb::MEPEvent*)ev->data;
      //printf("Data:%p\n",ev);
      decodeMEP2EventFragments(me, partID, events);
      if ( ev->magic != mep_magic_pattern() )  {
        ::printf("Bad MEP magic pattern!!!!\n");
      }
      // Increasing refcount in big chunk and avoid callback on m_evtProd->getSpace
      ev->refCount += (events.size()-1);
      declareSubEvents(evt, events);
      return Consumer::eventAction();
    }
  };
}

extern "C" int mep2event_prod(int argc,char **argv) {
  RTL::CLI cli(argc, argv, help);
  int partID = 0x103;
  std::string name = "evtgenerator";
  bool async = cli.getopt("asynchronous",1) != 0;
  cli.getopt("name",1,name);
  cli.getopt("partitionid",1,partID);
  ::printf("%synchronous MEP Event generator \"%s\" (pid:%d) included in buffers.\n",
	   async ? "As" : "S", name.c_str(),EVENTGenerator::pid());
  EVENTGenerator c(name,partID);
  if ( async ) c.setNonBlocking(WT_FACILITY_DAQ_EVENT, true);
  return c.run();
}
