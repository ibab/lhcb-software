// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MDFProducer.cpp,v 1.5 2010-09-21 14:28:25 frankb Exp $
//  ====================================================================
//  RawBufferCreator.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFHeader.h"
#include "MBM/Producer.h"
#include "MBM/mepdef.h"
#include "WT/wt_facilities.h"
#include "Event/RawBank.h"
#include <fcntl.h>
#include <cstring>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#define O_BINARY 0
#endif

namespace {
  int __dummyReadEvent(const char* fname,void* data, size_t bufLen, size_t& evtLen)  {
    static int nrewind = 0;
    static int file = open(fname, O_RDONLY|O_BINARY);
    LHCb::RawBank* b = (LHCb::RawBank*)data;
    LHCb::MDFHeader* h = b->begin<LHCb::MDFHeader>();
again:
    int status1 = ::read(file, h, sizeof(LHCb::MDFHeader));
    if ( status1 < (int)sizeof(LHCb::MDFHeader) )  {
      ::lseek(file, 0, SEEK_SET);
      ::printf("[0] Rewind # %d: End-of-file.\n", ++nrewind);
      goto again;
    }
    if ( status1 <= (int)bufLen )  {
      int len = h->recordSize()-sizeof(LHCb::MDFHeader);
      int status2 = ::read(file, h+1, len);
      if ( status2 < len )  {
        ::lseek(file, 0, SEEK_SET);
        ::printf("[1] Rewind # %d: End-of-file.\n", ++nrewind);
        goto again;
      }
      evtLen = h->recordSize()+8;
      b = (LHCb::RawBank*)data;
      b->setMagic();
      b->setType(LHCb::RawBank::DAQ);
      b->setSize(LHCb::MDFHeader::sizeOf(3));
      b->setVersion(DAQ_STATUS_BANK);
      // printf("MEP size: %d \n",evtLen);
      return 1;
    }
    return 0;
  }
}

namespace {
  static void help()  {
    ::lib_rtl_output(LIB_RTL_ALWAYS,"mdf_producer -opt [-opt]\n");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -n(ame)=<name>         buffer member name\n");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -a(synchronous)        Asynchonous mode (default is synchronous)\n");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -b=<name>      Buffer identifier \n");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -f(ile)=<name>         Input file name\n");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -s(pace)=<number>      Default space allocation in kBytes\n");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -p(artition)=<number>  Partition ID\n");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -d(ebug)               Invoke debugger\n");
  }
  struct MDFProducer  : public MBM::Producer  {
    std::string m_fname;
    int m_spaceSize;
    MDFProducer(const std::string& fnam, const std::string& buffer, const std::string& nam, int partitionID, size_t siz) 
    : MBM::Producer(buffer, nam, partitionID), m_fname(fnam), m_spaceSize(siz)
    {
      m_spaceSize *= 1024;  // Space size is in kBytes
      include();
    }
    ~MDFProducer()  {
    }
    int spaceRearm(int) {
      return MBM::Producer::spaceRearm(m_spaceSize);
    }
    int receiveEvent(void* data, size_t bufLen, size_t& evtLen)  {
      int status = __dummyReadEvent(m_fname.c_str(), data, bufLen, evtLen);
      return status==1 ? MBM_NORMAL : MBM_ERROR;
    }
    int spaceAction() {
      size_t evtLen = 0;
      MBM::EventDesc& e = event();
      int status = receiveEvent(e.data, m_spaceSize, evtLen);
      if ( status == MBM_NORMAL )  {
        LHCb::RawBank* b = (LHCb::RawBank*)e.data;
        LHCb::MDFHeader* h = b->begin<LHCb::MDFHeader>();
        LHCb::MDFHeader::SubHeader s = h->subHeader();
        e.type    = EVENT_TYPE_EVENT;
        ::memcpy(e.mask, (void*)s.H1->triggerMask(), sizeof(e.mask));
        e.len     = evtLen;
        declareEvent();
        status = sendSpace();
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

extern "C" int mdf_producer(int argc,char **argv) {
  RTL::CLI cli(argc, argv, help);
  int space = 64*1024;             // default 64 kB
  int partID = 0x103;              // default is LHCb partition id
  std::string buffer = "raw";
  std::string name   = "producer";
  std::string fname  = "./mdfData_0.dat";
  bool async = cli.getopt("asynchronous",1) != 0;
  bool debug = cli.getopt("debug",1) != 0;
  cli.getopt("name",1,name);
  cli.getopt("file",1,fname);
  cli.getopt("space",1,space);
  cli.getopt("buffer",1,buffer);
  cli.getopt("partitionid",1,partID);
  if ( debug ) ::lib_rtl_start_debugger();
  ::printf("%synchronous MBM Producer \"%s\" Partition:%d (pid:%d) included in buffers.\n",
     async ? "As" : "S", name.c_str(), partID, MDFProducer::pid());
  MDFProducer p(fname, buffer, name, partID, space);
  if ( async ) p.setNonBlocking(WT_FACILITY_DAQ_SPACE, true);
  return p.run();
}
