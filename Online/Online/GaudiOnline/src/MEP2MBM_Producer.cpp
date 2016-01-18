// $Id:$
//==========================================================================
//  LHCb Online software suite
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see OnlineSys/LICENSE.
//
// Author     : M.Frank
//
//==========================================================================
#include "MDF/RawEventHelpers.h"
#include "MDF/MEPEvent.h"
#include "MBM/Producer.h"
#include "MBM/mepdef.h"
#include "WT/wt_facilities.h"
#include <iostream>
#include <cstring>
#include <fcntl.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#define O_BINARY 0
#endif

using namespace std;

namespace {
  static void help()  {
    ::printf("mep_prod_a -opt [-opt]\n");
    ::printf("    -n(ame)=<name>         buffer member name\n");
    ::printf("    -a(synchronous)        Asynchonous mode (default is synchronous)\n");
    ::printf("    -i(nput)=<name>        Buffer name to place data.\n");
    ::printf("    -s(pace)=<number>      Default space allocation in kBytes\n");
    ::printf("    -c(ount)=<number>      Number of events to be sent. default: unlimited\n");
    ::printf("    -p(artition)=<number>  Partition ID\n");
    ::printf("    -r(efcount)=<number>   Initial MEP reference count\n");
    ::printf("    -m(apunused)           Map unused MEP buffers\n");
    ::printf("    -d(ebug)               Invoke debugger\n");
    ::printf("    -f(fifo)               Use fifos connections for communication");
    ::printf("    -t(cp)                 Use boost::asio tcp connections for communication");
  }

  struct EvtProducer  : public MBM::Producer  {
    string m_fname;
    int m_spaceSize, m_refCount, m_evtCount, m_etyp;
    long mepStart;
    EvtProducer(const string& buff, const string& nam, int partitionID, 
		const string& fn, int refcnt, size_t siz, int evtCount, 
		bool /* unused */, int etyp, int comm_typ) 
      : MBM::Producer(buff, nam, partitionID, comm_typ),
	m_fname(fn), m_spaceSize(siz), m_refCount(refcnt),
	m_evtCount(evtCount), m_etyp(etyp)
    {
      m_spaceSize *= 1024;  // Space size is in kBytes
      mepStart = (long)bufferAddress();
      ::printf(" Buffer space: %d bytes\n",m_spaceSize);
      ::printf(" MEP    buffer start: %08lX\n",mepStart);
      ::printf("Initial refcount value is %d\n",m_refCount);
    }
    virtual ~EvtProducer()  {
    }
    int __dummyReadEvent(void* data, size_t bufLen, size_t& evtLen)  {
      static int nrewind = 0;
      static int file = open(m_fname.c_str(), O_RDONLY|O_BINARY);
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
	evtLen = me->size()+me->sizeOf();
	return 1;
      }
      return 0;
    }
    int spaceRearm(int) {
      return MBM::Producer::spaceRearm(m_spaceSize);
    }
    int receiveEvent(void* data, size_t bufLen, size_t& evtLen)  {
      int status = __dummyReadEvent(data, bufLen, evtLen);
      return status==1 ? MBM_NORMAL : MBM_ERROR;
    }
    int spaceAction() {
      static int id = -1;
      size_t evtLen = 0;
      MBM::EventDesc& dsc = event();
      MEPEVENT* e = (MEPEVENT*)dsc.data;
      e->refCount    = m_refCount;
      e->evID        = ++id;
      e->begin       = long(long(e)-mepStart);
      e->packing     = -1;
      e->valid       = 1;
      e->magic       = mep_magic_pattern();
      for(size_t j=0; j<MEP_MAX_PACKING; ++j) {
	e->events[j].begin  = 0;
	e->events[j].evID   = 0;
	e->events[j].status = EVENT_TYPE_OK;
	e->events[j].signal = 0;
      }
      ::memset(e->events,0,sizeof(e->events));
      //::printf("MEP Buffer: [%d] Event at address %08X MEP:%p [%d] Pattern:%08X [Release MEP]\n",
      //  e->refCount, mepStart+e->begin, (void*)e, e->evID, e->magic);
      int status = receiveEvent(e->data, m_spaceSize-sizeof(MEPEVENT), evtLen);
      if ( status == MBM_NORMAL )  {
        m_event.len = evtLen+sizeof(MEPEVENT)-sizeof(e->data);
        m_event.mask[0] = partitionID();
        m_event.mask[1] = 0;
        m_event.mask[2] = 0;
        m_event.mask[3] = 0;
        m_event.type    = m_etyp;
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
      if ( m_evtCount>0 ) {
	m_evtCount--;
	if ( 0 == m_evtCount ) {
	  cout << "All events requested were decleared to MEP buffer." << endl;
	  exit(0);
	}
      }
      return status;
    }
  };
}

extern "C" int mep2mbm_producer(int argc,char **argv) {
  RTL::CLI cli(argc, argv, help);
  int space = 64*1024;             // default 64 kB
  int partID = 0x103;              // default is LHCb partition id
  int refCount = 2;
  int evtCount = -1;
  int etyp = EVENT_TYPE_MEP;
  int comm_type = BM_COM_FIFO;
  string name = "producer", buff="Events";
  string fname = "mepData_0.dat";
  bool async = cli.getopt("asynchronous",1) != 0;
  bool debug = cli.getopt("debug",1) != 0;
  bool unused = cli.getopt("mapunused",1) != 0;
  cli.getopt("etype",1,etyp);
  cli.getopt("name",1,name);
  cli.getopt("input",1,buff);
  cli.getopt("file",1,fname);
  cli.getopt("space",1,space);
  cli.getopt("partitionid",1,partID);
  cli.getopt("refcount",1,refCount);
  cli.getopt("count",1,evtCount);
  if ( cli.getopt("fifo",1) )
    comm_type = BM_COM_FIFO;
  else if ( cli.getopt("tcp",1) )
    comm_type = BM_COM_ASIO;
  if ( debug ) ::lib_rtl_start_debugger();
  ::printf("%synchronous MEP Producer \"%s\" Partition:%d (pid:%d) included in buffers. Will produce %d MEPs from %s Type:%d\n",
	   async ? "As" : "S", name.c_str(), partID, EvtProducer::pid(),evtCount,fname.c_str(),etyp);
  EvtProducer p(buff, name, partID, fname, refCount, space, evtCount, unused, etyp, comm_type);
  if ( async ) p.setNonBlocking(WT_FACILITY_DAQ_SPACE, true);
  return p.run();
}
