#include "MBM/Producer.h"
#include "MBM/MepConsumer.h"
#include "WT/wt_facilities.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawEventDescriptor.h"
#include <map>

namespace  {
  static void help()  {
    ::printf("mep_evtprod -opt [-opt]\n");
    ::printf("    -n(ame)=<name>         buffer member name\n");
    ::printf("    -a(asynchronous)       Asynchonous mode (default is synchronous)\n");
    ::printf("    -p(artition)=<number>  Partition ID\n");
    ::printf("    -r(efcount)=<number>   Optional MEP reference count decrement\n");
  }

  struct EVENTGenerator  : public MEP::Consumer  {
    typedef std::vector<LHCb::MEPFragment*> Frags;
    typedef std::map<unsigned int, Frags >  SubEvents;
    MBM::Producer* m_evtProd;
    int prt, m_refCount;
    EVENTGenerator(const std::string& nam, int partID, int cnt)
    : MEP::Consumer(nam, partID), m_evtProd(0), m_refCount(cnt)    
    {
      prt = 0;
      unsigned int vetomask[4] = {0u,0u,0u,0u};
      unsigned int trmask[4]   = {~0x0u,~0x0u,~0x0u,~0x0u};
      m_flags = USE_EVT_BUFFER|USE_MEP_BUFFER;
      include();
      m_bmid = m_mepID->mepBuffer;
      m_evtProd = new MBM::Producer((BMID)m_mepID->evtBuffer, nam, partitionID());
      addRequest(EVENT_TYPE_MEP,trmask,vetomask,BM_MASK_ANY,BM_REQ_VIP,BM_FREQ_PERC,100.);
      ::printf(" MEP    buffer start: %08lX\n",m_mepID->mepStart);
      ::printf(" EVENT  buffer start: %08lX\n",m_mepID->evtStart);
      //::printf(" RESULT buffer start: %08lX\n",m_mepID->resStart);
      //mbm_register_alloc_event(m_mepID->evtBuffer,0,0);
    }
    virtual ~EVENTGenerator()  {
      if ( m_evtProd ) delete m_evtProd;
    }
    virtual void declareSubEvents(const EventDesc& evt, SubEvents& events)  {
      int evID = 0;
      size_t numEvt = events.size();
      if ( prt ) ::printf("Declare MEP..%ld subevents..\n",long(numEvt));
      for(SubEvents::const_iterator i=events.begin(); i!=events.end(); ++i)  {
        if ( prt ) ::printf("----> Declare subevent:%d\n",evID+1);
        declareSubEvent(evt, ++evID, (*i).second);
      }
    }
    virtual void declareSubEvent(const EventDesc& evt, int evID, const Frags& frags)  {
      long offset;
      int sub_evt_len = LHCb::RawEventHeader::size(frags.size());
      if ( prt ) ::printf("0-Declare MEP fragment [%d]....\n",sub_evt_len);
      if ( m_evtProd->getSpace(sub_evt_len) == MBM_NORMAL ) {
        if ( prt ) ::printf("1-Declare MEP fragment....\n");
        EventDesc& e = m_evtProd->event();
        MEPEVENT* ev = (MEPEVENT*)evt.data;
        LHCb::RawEventHeader* h = (LHCb::RawEventHeader*)e.data;
        h->setEventID(evID);
        h->setMEPID(ev->evID);
        h->setDataStart(ev->begin);
        h->setNumberOfFragments(frags.size());
        h->setErrorMask(0);
        h->setNumberOfMissing(0);
        h->setOffsetOfMissing(0);
        for(size_t j=0; j<frags.size(); ++j)  {
          LHCb::MEPFragment* f = frags[j];
          if ( prt ) LHCb::checkFragment(f);
          offset = long(long(f)-m_mepID->mepStart);
          h->setOffset(j, offset);
          ev->events[j].begin  = offset;
          ev->events[j].status = EVENT_TYPE_OK;
          ev->events[j].evID   = j;
        }
        e.mask[0] = partitionID();
        e.mask[1] = 0;
        e.mask[2] = 0;
        e.mask[3] = 0;
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
    virtual int eventAction() {
      unsigned int pid = 0;
      const EventDesc& evt = Consumer::event();
      SubEvents events;
      MEPEVENT* ev = (MEPEVENT*)evt.data;
      LHCb::MEPEvent* me = (LHCb::MEPEvent*)ev->data;
      decodeMEP2EventFragments(me, pid, events);
      if ( ev->magic != mep_magic_pattern() )  {
        ::printf("Bad MEP magic pattern!!!!\n");
      }
      // Increasing refcount in big chunk and avoid callback on m_evtProd->getSpace
      //mep_increment(m_mepID, ev, events.size());
      declareSubEvents(evt, events);
      ev->packing = events.size();
      mep_decrement(m_mepID, ev, m_refCount);
      return Consumer::eventAction();
    }
  };
}

extern "C" int mep2event_prod(int argc,char **argv) {
  RTL::CLI cli(argc, argv, help);
  int partID = 0x103;
  int refCount = 1;
  std::string name = "evtgenerator";
  bool async = cli.getopt("asynchronous",1) != 0;
  cli.getopt("name",1,name);
  cli.getopt("partitionid",1,partID);
  cli.getopt("refcount",1,refCount);
  ::printf("%synchronous MEP Event generator \"%s\" (pid:%d) included in buffers.\n",
     async ? "As" : "S", name.c_str(),EVENTGenerator::pid());
  //_asm int 3
  EVENTGenerator c(name,partID, refCount);
  if ( async ) c.setNonBlocking(WT_FACILITY_DAQ_EVENT, true);
  return c.run();
}
