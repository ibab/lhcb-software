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
    MBMEvtSelector*       m_onlineSel;
    MBM::Consumer*        m_consumer;
    int                   m_partID;
    bool                  m_decodeValue;

    /// Connect to regular MBM buffer
    StatusCode connectMBM(const std::string& input);
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

    bool beIntelligent() const {  return m_intelligentSetup;  }
    int  processTAE() const    {  return m_tae;               }

    /// Service Constructor
    MBMEvtSelector(const std::string& name, ISvcLocator* svcloc);

    /// Standard destructor
    virtual ~MBMEvtSelector()    {}

  protected:
    /// Data Members
    /// Reference to MEP manager service
    LHCb::IMEPManager*  m_mepMgr;
    /// Current context
    mutable OnlineContext* m_currContext;
    /// Property: Name of the MEP manager (Defule=MEPManager)
    std::string         m_mepManagerName;
    /// Property: Maximum retries for consecutive events before going to error
    int                 m_maxRetry;
    /// Property: Need to if TAEs should be processed (from run-info)
    int                 m_tae;
    /// Property: Have intelligent setup and recognize data input
    bool                m_intelligentSetup;
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
  : OnlineContext(s), m_onlineSel(s), m_consumer(0), m_partID(0)
{
  m_decodeValue = s->mustDecode();
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

StatusCode MBMContext::receiveEvent()  {
  m_banks.clear();
  if ( m_consumer )  {
    int org_retry = m_onlineSel->m_maxRetry;
    int max_retry = m_onlineSel->m_maxRetry;
  Retry:
    try  {
      // If the job options is -1, we loop forever
      if ( org_retry < 0 ) max_retry = 9999999;
      if ( m_events.empty() ) {
        m_sel->increaseReqCount();
        m_needFree = false;
        if ( m_sel->isCancelled() ) {
          // In case we got cancelled, just return error and 
          // let upper layers handle the problem
          return StatusCode::FAILURE;
        }
        else if ( m_consumer->getEvent() != MBM_NORMAL )  {
          return StatusCode::FAILURE;
        }
      }
      const MBM::EventDesc& e = m_consumer->event();
      //
      // Try auto-setup if requested:
      // (MEP and no TAE) --> decode
      // (MEP and TAE) --> no decode
      // (MDF) -> no decode
      //
      if ( m_onlineSel->beIntelligent() )   {
        if ( 0 == m_onlineSel->processTAE() && e.type == EVENT_TYPE_MEP )  {
          m_sel->setDecode(true);
        }
        else if ( m_onlineSel->processTAE() > 0 && e.type == EVENT_TYPE_MEP )  {
          m_sel->setDecode(false);
        }
        else if ( e.type == EVENT_TYPE_EVENT )  {
          m_sel->setDecode(false);
        }
        else {
          m_sel->setDecode(m_decodeValue);
        }
      }

      // The event is a MEP with multiple events, which must be decoded:
      if ( m_sel->mustDecode() && e.type == EVENT_TYPE_MEP )  {
        return convertMEP(m_partID,e);
      }
#if 0
      // This does no longer exist, since we have no descriptor events anymore.

      // The event is a descriptor event, which must be decoded using the MEP data:
      if ( m_sel->mustDecode() && e.type == EVENT_TYPE_EVENT )  {
        return convertDescriptor(m_partID,e);
      }
#endif
      else { // Or: simple case data are data - as it should be
        return convertMDF(m_partID,e);
      }
    }
    catch(const exception& e)  {
      string err = e.what();
      m_sel->info("Failed to read next event:"+err);
      // We only allow for a max of max_retry consecutive bad entries from MBM
      // Otherwise things are bad!
      if ( max_retry-- > 0 )  {
        if ( m_sel->isCancelled() ) {
          // In case we got cancelled, just return error and 
          // let upper layers handle the problem
          releaseEvent();
          return StatusCode::FAILURE;
        }
        else if (err.find("Error decoding raw banks")!= string::npos ||
                 err.find("Bad magic pattern")       != string::npos ||
                 err.find("Bad MEP")                 != string::npos ||
                 err.find("Unknown Bank")            != string::npos ||
                 err.find("Error in multi raw")      != string::npos ||
                 err.find("MEP event error")         != string::npos ||
                 err.find("MEP fragment error")      != string::npos ||
                 err.find("MEP multi fragment error")!= string::npos) {
          m_sel->info("Continue processing despite error.");
          releaseEvent();
          goto Retry;
        }
      }
      m_sel->error("Failed to read next event:"+err);
      m_sel->error("Maximum number of retries exceeded. Giving up...");
      ::exit(EILSEQ);
    }
    catch(...)  {
      if ( m_sel->isCancelled() ) {
        releaseEvent();
        return StatusCode::FAILURE;
      }
      m_sel->error("Failed to read next event - Unknown exception.");
      ::exit(EILSEQ);
    }
  }
  return StatusCode::FAILURE;
}

StatusCode MBMContext::connect(const string& input)  {
  StatusCode sc = StatusCode::SUCCESS;
  m_events.clear();
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

StatusCode MBMContext::connectMBM(const string& input)  {
  if ( m_sel )  {
    IMEPManager* m = m_onlineSel->mepMgr();
    map<string,BMID>::const_iterator i = m->buffers().find(input);
    if ( i != m->buffers().end() ) {
      m_consumer = new MBM::Consumer((*i).second,RTL::processName(),m->partitionID());
      if ( m_consumer->id() == MBM_INV_DESC ) {
        return StatusCode::FAILURE;
      }
      m_partID   = m_consumer->partitionID();
      m_baseAddr = (void*)m_consumer->bufferAddress();
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
  m_baseAddr = 0;
}

MBMEvtSelector::MBMEvtSelector(const string& nam, ISvcLocator* svc)
  : OnlineBaseEvtSelector(nam,svc), m_mepMgr(0), m_currContext(0)
{
  m_input = "Events";
  m_decode = true;
  declareProperty("IntelligentSetup",m_intelligentSetup=false);
  declareProperty("TAE",m_tae=0);
  declareProperty("MaxRetry",m_maxRetry=-1);
  declareProperty("MEPManager",m_mepManagerName="MEPManager");
}

// IService implementation: Db event selector override
StatusCode MBMEvtSelector::initialize()    {
  // Initialize base class
  StatusCode status = OnlineBaseEvtSelector::initialize();
  if ( !status.isSuccess() )    {
    return error("Error initializing base class OnlineBaseEvtSelector!");
  }
  status = service(m_mepManagerName,m_mepMgr);
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
