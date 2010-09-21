#include "MBM/Producer.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawEventDescriptor.h"
#include "MDF/MEPEvent.h"
#include "MBM/mepdef.h"

#include <map>
#include <fcntl.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#define O_BINARY 0
#endif

namespace  {
  static void help()  {
    ::printf("mep_filetest -opt [-opt]\n");
    ::printf("    -f(ile)=<name>         input file name\n");
  }
  struct MEPFileTest {
    typedef std::vector<LHCb::MEPFragment*> Frags;
    typedef std::map<unsigned int, Frags >  SubEvents;
    std::string m_name;
    int prt;
    MEPEVENT* m_mepData;
    LHCb::MEPEvent* m_mepEvent;
    LHCb::RawEventHeader* m_subEvent;

    MEPFileTest(const std::string& nam) : m_name(nam), prt(1)   {    
      m_mepData  = (MEPEVENT*)new char[1024*1024];
      m_mepEvent = (LHCb::MEPEvent*)m_mepData->data;
      m_subEvent = (LHCb::RawEventHeader*)new char[1024*32]; 
    }
    virtual ~MEPFileTest()  {    
      delete [] (char*)m_mepData;
      delete [] (char*)m_subEvent;
    }
    int readEvent(size_t& evtLen)  {
      static int nrewind = 0;
      static int file = ::open(m_name.c_str(), O_RDONLY|O_BINARY);
  again:
      char* ptr = (char*)m_mepData;
      int len = sizeof(MEPEVENT) - 4*sizeof(char);
      len += m_mepEvent->sizeOf();
      int status1 = ::read(file, ptr, len);
      if ( status1 < len )  {
        ::lseek(file, 0, SEEK_SET);
        ::printf("[0] Rewind # %d: End-of-file.\n", ++nrewind);
        goto again;
      }
      int len2 = m_mepEvent->size();
      int status2 = ::read(file, ptr+len, len2);
      if ( status2 < (int)m_mepEvent->size() )  {
        ::lseek(file, 0, SEEK_SET);
        ::printf("[1] Rewind # %d: End-of-file.\n", ++nrewind);
        goto again;
      }
      evtLen = m_mepEvent->size()+m_mepEvent->sizeOf();
      return 1;
    }
    virtual void declareSubEvents(SubEvents& events)  {
      int evID = 0;
      size_t numEvt = events.size();
      if ( prt ) ::printf("Declare MEP..%ld subevents..\n",long(numEvt));
      for(SubEvents::const_iterator i=events.begin(); i!=events.end(); ++i)  {
        if ( prt ) ::printf("----> Declare subevent:%d\n",evID+1);
        declareSubEvent(++evID, (*i).second);
      }
    }
    virtual void declareSubEvent(int evID, const Frags& frags)  {
      int sub_evt_len = LHCb::RawEventHeader::size(frags.size());
      if ( prt ) ::printf("0-Declare MEP fragment [%d]....\n",sub_evt_len);
      MEPEVENT* ev = m_mepData;
      LHCb::RawEventHeader* h = (LHCb::RawEventHeader*)m_subEvent;
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
        h->setOffset(j, int(long(f)-long(m_mepData)));
      }
    }
    int run()  {
      unsigned int pid = 0;
      size_t evtLen = 0;
      bool stop = false;
      while( stop == false ) {
        int status = readEvent(evtLen);
        if ( status == MBM_NORMAL )  {
          SubEvents events;
          decodeMEP2EventFragments(m_mepEvent, pid, events);
          if ( m_mepData->magic != mep_magic_pattern() )  {
            ::printf("Bad MEP magic pattern!!!!\n");
          }
          declareSubEvents(events);
          m_mepData->packing = events.size();
        }
      }
      return 1;
    }
  };
}

extern "C" int mep_filetest(int argc,char **argv) {
  std::string fname("Dummy.dat");
  RTL::CLI cli(argc, argv, help);
  cli.getopt("file",1,fname);
  MEPFileTest c(fname);
  return c.run();
}
