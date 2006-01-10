// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MEPProducer.cpp,v 1.3 2006-01-10 14:08:17 frankb Exp $
//	====================================================================
//  RawBufferCreator.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "MDF/RawEventHelpers.h"
#include "MDF/MEPEvent.h"
#include "MBM/MepProducer.h"
#include "WT/wt_facilities.h"

#include <fcntl.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#define O_BINARY 0
#endif

namespace {
  int __dummyReadEvent(void* data, size_t bufLen, size_t& evtLen)  {
    static int nrewind = 0;
    static int file = open("./mepBuffer.dat", O_RDONLY|O_BINARY);
    LHCb::MEPEvent* me = (LHCb::MEPEvent*)data;
again:
    int status1 = ::read(file, me, me->sizeOf());
    if ( status1 < (int)me->sizeOf() )  {
      ::lseek(file, 0, SEEK_SET);
      ::printf("[0] Rewind # %d: End-of-file.\n", ++nrewind);
      goto again;
    }
    if ( status1 <= (int)bufLen )  {
      int status2 = ::read(file, ((char*)data)+me->sizeOf(), me->size());
      if ( status2 < (int)me->size() )  {
        ::lseek(file, 0, SEEK_SET);
        ::printf("[1] Rewind # %d: End-of-file.\n", ++nrewind);
        goto again;
      }
      evtLen += me->size()+me->sizeOf();
      // printf("MEP size: %d \n",evtLen);
      return 1;
    }
    return 0;
  }
}

namespace {
  static void help()  {
    ::printf("mep_prod_a -opt [-opt]\n");
    ::printf("    -n(ame)=<name>         buffer member name\n");
    ::printf("    -a(synchronous)        Asynchonous mode (default is synchronous)\n");
    ::printf("    -s(pace)=<number>      Default space allocation in kBytes\n");
    ::printf("    -p(artition)=<number>  Partition ID\n");
  }
  struct MEPProducer  : public MEP::Producer  {
    int m_spaceSize;
    MEPProducer(const std::string& nam, int partitionID, size_t siz) 
    : MEP::Producer(nam, partitionID), m_spaceSize(siz)
    {
      m_spaceSize *= 1024;  // Space size is in kBytes
      m_flags = USE_MEP_BUFFER;
      include();
      m_bmid = m_mepID->mepBuffer;
      ::printf(" MEP    buffer start: %08X\n",m_mepID->mepStart);
      ::printf(" EVENT  buffer start: %08X\n",m_mepID->evtStart);
      ::printf(" RESULT buffer start: %08X\n",m_mepID->resStart);
    }
    ~MEPProducer()  {
    }
    int spaceRearm(int) {
      return MEP::Producer::spaceRearm(m_spaceSize);
    }
    int receiveEvent(void* data, size_t bufLen, size_t& evtLen)  {
      int status = __dummyReadEvent(data, bufLen, evtLen);
      return status==1 ? MBM_NORMAL : MBM_ERROR;
    }
    int spaceAction() {
      size_t evtLen = 0;
      MBM::EventDesc& dsc = event();
      MEPEVENT* ev = (MEPEVENT*)dsc.data;
      ev->refCount    = 1;
      ev->mepBufferID = 0;
      ev->begin       = int(int(ev)-m_mepID->mepStart);
      ev->packing     = -1;
      ev->valid       = 1;
      int status = receiveEvent(ev->data, m_spaceSize-sizeof(MEPEVENT), evtLen);
      if ( status == MBM_NORMAL )  {
        m_event.len = evtLen+sizeof(MEPEVENT);
        m_event.mask[0] = 0x103;
        m_event.mask[1] = 0;
        m_event.mask[2] = 0;
        m_event.mask[3] = 0;
        m_event.type    = EVENT_TYPE_MEP;
        declareEvent();
        return sendSpace();
      }
      return status;
    }
    // Run the application in synchonous mode
    int runSynchronous() {
      int status = spaceRearm(0);
      if ( status == MBM_NORMAL )  {
        status = spaceAction();
      }
      return status;
    }
  };
}

extern "C" int mep_producer(int argc,char **argv) {
  RTL::CLI cli(argc, argv, help);
  int space = 64*1024;             // default 64 kB
  int partID = 0x103;              // default is LHCb partition id
  std::string name = "producer";
  bool async = cli.getopt("asynchronous",1) != 0;
  cli.getopt("name",1,name);
  cli.getopt("space",1,space);
  cli.getopt("partitionid",1,partID);
  ::printf("%synchronous MEP Producer \"%s\" Partition:%d (pid:%d) included in buffers.\n",
	   async ? "As" : "S", name.c_str(), partID, MEPProducer::pid());
  MEPProducer p(name, partID, space);
  if ( async ) p.setNonBlocking(WT_FACILITY_DAQ_SPACE, true);
  return p.run();
}
