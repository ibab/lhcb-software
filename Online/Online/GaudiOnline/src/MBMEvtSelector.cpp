// $Id: MBMEvtSelector.cpp,v 1.7 2009-12-03 19:01:02 frankb Exp $
//====================================================================
//  MBMEvtSelector
//--------------------------------------------------------------------
//
//  Package    : GaudiOnline
//
//  Description: The MBMEvtSelector component is able
//               to produce a list of event references given a set of "selection
//               criteria" with input from MBM buffers.
//
//  Author     : M.Frank
//  Created    : 4/01/99
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MBMEvtSelector.cpp,v 1.7 2009-12-03 19:01:02 frankb Exp $
#ifndef GAUDIONLINE_MBMEVTSELECTOR_H
#define GAUDIONLINE_MBMEVTSELECTOR_H 1

// Include files
#include "GaudiOnline/OnlineBaseEvtSelector.h"
#include "GaudiOnline/MEPManager.h"
#include "MBM/Consumer.h"

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {
  // Forward declarations
  class MBMEvtSelector;
  class MBMContext;

  /** @class MBMContext
    *
    * @author M.Frank
    */
  class MBMContext : public OnlineContext  {
  private:
    typedef std::vector<LHCb::MEPFragment*> Frags;
    typedef std::map<unsigned int, Frags >  SubEvents;
    SubEvents             m_events;
    std::vector<int>      m_memory;
    MBMEvtSelector*       m_onlineSel;
    MBM::Consumer*        m_consumer;
    void*                 m_mepStart;
    int                   m_evID;

    /// Convert standard descriptor event structure to banks
    StatusCode convertDescriptor(const MBM::EventDesc& e);
    /// Convert MEP into a sequence of events being worked down.
    StatusCode convertMEP(const MBM::EventDesc& e);
    
    /// Connect to MEP buffer
    StatusCode connectMEP(const std::string& input);
    /// Connect to regular MBM buffer
    StatusCode connectMBM(const std::string& input);
    /// Test routine for run-time exceptions
    void testExceptions();
  public:
    /// Standard constructor
    MBMContext(MBMEvtSelector* pSelector);
    /// Standard destructor 
    virtual ~MBMContext() {}
    /// Connect to event data source
    virtual StatusCode connect(const std::string& input);
    /// Receive event information
    virtual StatusCode receiveEvent();
    /// Rearm event data request
    virtual StatusCode rearmEvent();
    /// Free event resources
    virtual StatusCode freeEvent();
    /// Force release of event resources
    virtual StatusCode releaseEvent();
    /// Flag event resources
    virtual StatusCode flagEvent(int flag);
    /// Close connection to event data source
    virtual void close();
  };

  /** @class MBMEvtSelector  MBMEvtSelector.h
    *
    * @author  M.Frank
    * @vrsion  1.0
    */
  class MBMEvtSelector : public OnlineBaseEvtSelector  {
    friend class MBMContext;
  public:
    IMEPManager* mepMgr() const { return m_mepMgr;    }

    /// IService implementation: Db event selector override
    virtual StatusCode initialize();

    /// IService overload: start MEP manager service
    virtual StatusCode start();

    /// IService overload: stop MEP manager service
    virtual StatusCode stop();

    // IService implementation: Service finalization
    virtual StatusCode finalize();

    /// Create a new event loop context
    /** @param refpCtxt   [IN/OUT]  Reference to pointer to store the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode createContext(Context*& refpCtxt) const;

    /// Release existing event iteration context
    /** @param refCtxt   [IN/OUT]  Reference to the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode releaseContext(Context*& refCtxt) const;

    /** Will set a new criteria for the selection of the next list of events and will change
      * the state of the context in a way to point to the new list.
      * 
      * @param cr The new criteria string.
      * @param c  Reference pointer to the Context object.
      */
    virtual StatusCode resetCriteria(const std::string& cr,Context& c)const;

    /// Service Constructor
    MBMEvtSelector(const std::string& name, ISvcLocator* svcloc);

    /// Standard destructor
    virtual ~MBMEvtSelector()    {}

  protected:
    /// Data Members
    /// Reference to MEP manager service
    LHCb::IMEPManager*  m_mepMgr;
    /// Maximum retries for consecutive events before going to error
    int                 m_maxRetry;
    /// Current context
    mutable MBMContext* m_currContext;
  };
}
#endif // GAUDIONLINE_MBMEVTSELECTOR_H

// $Id: MBMEvtSelector.cpp,v 1.7 2009-12-03 19:01:02 frankb Exp $
//====================================================================
//  MBMEvtSelector.cpp
//--------------------------------------------------------------------
//
//  Package    : GaudiOnline
//
//  Description: The GaudiOnline::MBMEvtSelector component is able
//               to produce a list of event references given 
//               a set of "selection criteria".
//
//  Author     : M.Frank, B.Jost
//====================================================================
//
//   State diagram(s) for blocking execution:
//
//
//  +--------+    +-------------+  +---------------+
//  | Client |    | EvtSelector |  | MBMContext |
//  +--------+    +-------------+  +---------------+
//     |  next()        |                  |
//     +--------------->+                  |
//     |                |                  |
//     |                |   rtl_lock()     |     if call succeeds
//     |                +------+           |     operation is allowed
//     |                |      |           |
//     |                +<-----+           |
//     |                |   receiveEvent() |
//     |                +----------------->+
//     |                |                  |
//     |                |   rtl_unlock()   |     Unlock mutex
//     |                +------+           |     and allow client access
//     |                |      |           |     to process the event
//     |                +<-----+           |
//     |                |                  |
// -------------------------------------------------------
//     |                |                  |
//     |  resume()      |                  |    
//     +--------------->+                  |    Will result in end of
//     |                | rtl_unlock()     |    a possibly pending call 
//     |                +------+           |    to rtl_lock in next()
//     |                |      |           |    
//     |                +<-----+           |
//     |                |                  |
//     |                |                  |
// -------------------------------------------------------
//     |                |                  |
//     |  suspend()     |                  |    
//     +--------------->+                  |    
//     |                |   rtl_lock()     |    if call succeeds
//     |                +------+           |    operation access to
//     |                |      |           |    the next event is blocked
//     |                +<-----+           |    until resume() is called
//     |                |                  |   
//
//====================================================================
#include "GaudiKernel/SvcFactory.h"
#include "MDF/RawEventHelpers.h"
#include <cerrno>

namespace LHCb {
  // Backwards component compatibility
  class OnlineEvtSelector : public MBMEvtSelector  {
  public:
    OnlineEvtSelector(const std::string& nam,ISvcLocator* svc)  : MBMEvtSelector(nam,svc) {}
    virtual ~OnlineEvtSelector()    {}
  };
}
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,OnlineEvtSelector)
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MBMEvtSelector)

using namespace std;
using namespace LHCb;

MBMContext::MBMContext(MBMEvtSelector* s)
: OnlineContext(s), m_onlineSel(s), m_consumer(0), m_mepStart(0)
{
}

void MBMContext::testExceptions() {
  static int test = 888;
  --test;
  if ( test >300 && test<330 ) {
    throw runtime_error("Bad magic pattern --- test");
  }
  if ( test >30 && test<50 ) {
    throw runtime_error("Unknown Bank --- test");
  }
  if ( test == 0 ) test = 888;
}

StatusCode MBMContext::releaseEvent()  {
  if ( !m_events.empty() ) {
    m_events.clear();
    m_needFree = true;
  }
  return freeEvent();
}

StatusCode MBMContext::freeEvent()  {
  if ( m_consumer )  {
    if ( m_needFree )   {
      try  {
        m_needFree = false;
        m_consumer->freeEvent();
        m_evdesc.setHeader(0);
        m_evdesc.setSize(0);
      }
      catch(...) {
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode MBMContext::flagEvent(int flag)  {
  if ( m_consumer )  {
    if ( m_sel->eventContext() == this ) {
      // Can only flag Descriptor events!
      const MBM::EventDesc& e = m_consumer->event();
      if ( m_sel->mustDecode() && e.type == EVENT_TYPE_EVENT )  {
	MEP_SINGLE_EVT* sevt = (MEP_SINGLE_EVT*)e.data;
	MEPEVENT* m = (MEPEVENT*)(m_evdesc.buffer() + sevt->begin);
	m->events[sevt->evID].status = EVENT_TYPE_BADPROC;
	m->events[sevt->evID].signal = flag;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode MBMContext::rearmEvent()  {
  return StatusCode::SUCCESS;
}

StatusCode MBMContext::convertDescriptor(const MBM::EventDesc& e)  {
  m_evdesc.setPartitionID(m_consumer->partitionID());
  m_evdesc.setTriggerMask(e.mask);
  m_evdesc.setEventType(e.type);
  m_evdesc.setHeader(e.data);
  m_evdesc.setSize(e.len);
  m_needFree = true;
  //testExceptions();
  m_evdesc.setMepBuffer(m_mepStart);
  m_sel->increaseEvtCount();
  for(int i=0, n=m_evdesc.numberOfFragments(); i<n; ++i)
    decodeFragment(m_evdesc.fragment(i), m_banks);
  return StatusCode::SUCCESS;
}

StatusCode MBMContext::convertMEP(const MBM::EventDesc& e)  {
  RawEventHeader*   h = 0;
  MEPEVENT*        ev = (MEPEVENT*)e.data;
  unsigned int    pid = m_consumer->partitionID();
  unsigned int mask[] = {pid,0,0,0};
  if ( m_events.empty() )   {
    MEPEvent* event = (MEPEvent*)ev->data;
    if ( ev->magic != mep_magic_pattern() )  {
      throw runtime_error("Bad MEP magic pattern!!!!");
    }
    decodeMEP2EventFragments(event, pid, m_events);
    if ( m_events.empty() )  {
      throw runtime_error("Bad MEP received. No sub-events !!!!");
    }
    for(size_t evID=1; evID<=m_events.size(); ++evID)  {
      ev->events[evID].begin  = long(ev)-long(m_mepStart);
      ev->events[evID].status = EVENT_TYPE_OK;
      ev->events[evID].signal = 0;
      ev->events[evID].evID   = evID;
    }
    //cout << "Decoded " << m_events.size() << " sub events" << endl;
    m_evID = 0;
  }
  const Frags& frags = (*m_events.begin()).second;
  size_t evt_len = ((RawEventHeader::size(frags.size())+1)*sizeof(int))/sizeof(int);
  if ( m_memory.size() < evt_len ) {
    m_memory.resize(evt_len+256); // Increase memmory size with some safety margin
  }
  h = (RawEventHeader*)&m_memory[0];
  m_evdesc.setPartitionID(pid);
  m_evdesc.setTriggerMask(mask);
  m_evdesc.setEventType(EVENT_TYPE_EVENT);
  m_evdesc.setHeader(h);
  m_evdesc.setSize(evt_len);
  m_evdesc.setMepBuffer(m_mepStart);
  m_sel->increaseEvtCount();
	
  h->setEventID(++m_evID);
  h->setMEPID(ev->evID);
  h->setDataStart(ev->begin);
  h->setNumberOfFragments(frags.size());
  h->setErrorMask(0);
  h->setNumberOfMissing(0);
  h->setOffsetOfMissing(0);
  for(size_t j=0; j<frags.size(); ++j)  {
    MEPFragment* f = frags[j];
    long off =  long(long(f)-long(m_mepStart));
    h->setOffset(j, off);
  }
  for(int k=0, n=m_evdesc.numberOfFragments(); k<n; ++k)
    decodeFragment(m_evdesc.fragment(k), m_banks);
  m_events.erase(m_events.begin());
  if ( m_events.empty() ) m_needFree = true;
  return StatusCode::SUCCESS;
}

StatusCode MBMContext::receiveEvent()  {
  m_banks.clear();
  if ( m_consumer )  {
    int max_retry = m_onlineSel->m_maxRetry;
    Retry:
    try  {
      if ( m_events.empty() ) {
	m_sel->increaseReqCount();
	m_needFree = false;
	if ( m_consumer->getEvent() != MBM_NORMAL )  {
	  return StatusCode::FAILURE;
	}
      }
      const MBM::EventDesc& e = m_consumer->event();
      // The event is a descriptor event, which must be decoded using the MEP data:
      if ( m_sel->mustDecode() && e.type == EVENT_TYPE_EVENT )  {
	return convertDescriptor(e);
      }
      // The event is a MEP with multiple events, which must be decoded:
      else if ( m_sel->mustDecode() && e.type == EVENT_TYPE_MEP )  {
        return convertMEP(e);
      }
      else { // Or: simple case data are data - as it should be
        m_evdesc.setPartitionID(m_consumer->partitionID());
        m_evdesc.setTriggerMask(e.mask);
        m_evdesc.setEventType(e.type);
        m_evdesc.setHeader(e.data);
        m_evdesc.setSize(e.len);
        m_needFree = true;
        return StatusCode::SUCCESS;
      }
    }
    catch(const exception& e)  {
      string err = e.what();
      m_sel->error("Failed to read next event:"+err);
      // We only allow for a max of max_retry consecutive bad entries from MBM
      // Otherwise things are bad!
      if ( max_retry-- > 0 )  {
	if (err.substr(0,9)  == "Bad magic" ||
            err.substr(0,7)  == "Bad MEP"   ||
	    err.substr(0,12) == "Unknown Bank" ) {
	  freeEvent();
	  goto Retry;
	}
      }
      m_sel->error("Maximum number of retries exceeded. Giving up...");
      ::exit(EILSEQ);
    }
    catch(...)  {
      m_sel->error("Failed to read next event - Unknown exception.");
      ::exit(EILSEQ);
    }
  }
  return StatusCode::FAILURE;
}

StatusCode MBMContext::connect(const string& input)  {
  StatusCode sc = StatusCode::SUCCESS;
  m_events.clear();
  if ( input=="EVENT" || input=="RESULT" || input=="MEP" )
    sc = connectMEP(input);
  else 
    sc = connectMBM(input);
  if ( sc.isSuccess() )  {
    for (int i=0, n=m_sel->numReq(); i<n; ++i )  {
      m_consumer->addRequest(m_sel->req(i));
    }
    return sc;
  }
  m_sel->error("Failed to connect to buffer manager \""+input+"\".");
  return sc;
}

StatusCode MBMContext::connectMEP(const string& input)  {
  IMEPManager* m = m_onlineSel->mepMgr();
  MEPID mepID = m->mepID();
  if ( mepID != MEP_INV_DESC )  {
    m_mepStart = (void*)mepID->mepStart;
    if ( input == "MEP" )  {
      m_consumer = new MBM::Consumer(mepID->mepBuffer,mepID->processName,mepID->partitionID);
    }
    else if ( input == "EVENT" )  {
      m_consumer = new MBM::Consumer(mepID->evtBuffer,mepID->processName,mepID->partitionID);
      m_onlineSel->setDecode(true);
    }
    else if ( input == "RESULT" )  {
      m_consumer = new MBM::Consumer(mepID->resBuffer,mepID->processName,mepID->partitionID);
      m_onlineSel->setDecode(true);
    }
    if ( m_consumer )  {
      if ( m_consumer->id() != MBM_INV_DESC )  {
        return StatusCode::SUCCESS;
      }
    }
  }
  return StatusCode::FAILURE;
}

StatusCode MBMContext::connectMBM(const string& input)  {
  if ( m_sel )  {
    IMEPManager* m = m_onlineSel->mepMgr();
    map<string,BMID>::const_iterator i = m->buffers().find(input);
    if ( i != m->buffers().end() ) {
      m_consumer = new MBM::Consumer((*i).second,RTL::processName(),m->partitionID());
      if ( m_consumer->id() == MBM_INV_DESC ) {
        return StatusCode::FAILURE;
      }
      m_mepStart = (void*)m_consumer->bufferAddress();
      return StatusCode::SUCCESS;
    }
    m_sel->error("Data buffer \""+input+"\" is not mapped. "
		 "How do you think you can connect to it?");
  }
  return StatusCode::FAILURE;
}

void MBMContext::close()  {
  if ( !m_events.empty() ) m_needFree=true;
  m_events.clear();
  if ( m_consumer )  {
    for (int i=0, n=m_sel->numReq(); i<n; ++i )  {
      m_consumer->delRequest(m_sel->req(i));
    }
    delete m_consumer;
    m_consumer = 0;
  }
  m_mepStart = 0;
}

MBMEvtSelector::MBMEvtSelector(const string& nam, ISvcLocator* svc)
  : OnlineBaseEvtSelector(nam,svc), m_mepMgr(0), m_currContext(0)
{
  m_input = "EVENT";
  m_decode = true;
  declareProperty("MaxRetry",m_maxRetry=10000);
}

// IService implementation: Db event selector override
StatusCode MBMEvtSelector::initialize()    {
  // Initialize base class
  StatusCode status = OnlineBaseEvtSelector::initialize();
  if ( !status.isSuccess() )    {
    return error("Error initializing base class OnlineBaseEvtSelector!");
  }
  status = service("MEPManager",m_mepMgr);
  if ( !status.isSuccess() )   {
    return error("Failed to access service MEPManager.");
  }
  return status;
}

/// IService overload: start MEP manager service
StatusCode MBMEvtSelector::start() {
  StatusCode sc = OnlineService::start();
  setProperties();
  if ( m_currContext ) {
    StatusCode sc = m_currContext->connect(input());
    if ( !sc.isSuccess() ) {
      return sc;
    }
  }
  else if ( mepMgr()->connectWhen() == "start" ) {
    error("Attempt to connect MBM buffer "+input()+" with invalid context.");
  }
  return sc;
}

/// IService overload: stop MEP manager service
StatusCode MBMEvtSelector::stop() {
  if ( mepMgr()->connectWhen() == "start" ) {
    m_currContext->close();
  }
  return OnlineBaseEvtSelector::stop();
}

// IService implementation: Service finalization
StatusCode MBMEvtSelector::finalize()    {
  if ( m_currContext ) {
    m_currContext->close();
    m_currContext = 0;
  }
  if ( m_mepMgr )  {
    m_mepMgr->release();
    m_mepMgr = 0;
  }
  return OnlineBaseEvtSelector::finalize();
}

// Create event selector iteration context
StatusCode MBMEvtSelector::createContext(Context*& refpCtxt) const  {
  MBMContext* ctxt = new MBMContext(const_cast<MBMEvtSelector*>(this));
  m_currContext = ctxt;
  refpCtxt = ctxt;
  if ( mepMgr()->connectWhen() == "initialize" ) {
    return ctxt->connect(m_input);
  }
  return StatusCode::SUCCESS;
}

StatusCode MBMEvtSelector::releaseContext(Context*& ctxt) const  {
  MBMContext* pCtxt = dynamic_cast<MBMContext*>(ctxt);
  if ( pCtxt == m_currContext ) {
    m_context = 0;
    pCtxt->close();
    delete pCtxt;
    pCtxt = 0;
    m_currContext = 0;
    //error("MBMEvtSelector::releaseContext> MBM buffer context deleted!");
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode 
MBMEvtSelector::resetCriteria(const string& crit,Context& ct) const {
  MBMContext* ctxt = dynamic_cast<MBMContext*>(&ct);
  if ( ctxt )  {
    ctxt->close();
    if ( mepMgr()->connectWhen() == "initialize" ) {
      if ( ctxt->connect(m_input).isSuccess() )  {
	return StatusCode::SUCCESS;
      }
      return error("Failed to connect to:"+crit);
    }
    m_currContext = ctxt;
    return StatusCode::SUCCESS;
  }
  return error("Invalid iteration context. Cannot connect to:"+crit);
}
