#ifndef GAUDIONLINE_ONLINECONTEXT_H
#define GAUDIONLINE_ONLINECONTEXT_H
// Include files
#include "GaudiOnline/OnlineEvtSelector.h"
#include "MDF/RawEventDescriptor.h"
#include "MBM/Consumer.h"

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  class RawBank;
  class OnlineEvtSelector;

  /** @class OnlineContext
    *
    * @author M.Frank
    */
  class OnlineContext : public IEvtSelector::Context   {
  public:
    std::vector<LHCb::RawBank*>    m_banks;
    LHCb::RawEventDescriptor       m_evdesc;
  private:
    const LHCb::OnlineEvtSelector* m_sel;
    MBM::Consumer*                 m_consumer;
    void*                          m_mepStart;
    bool                           m_needFree;
  public:
    /// Standard constructor
    OnlineContext(const LHCb::OnlineEvtSelector* pSelector)
    : m_sel(pSelector), m_consumer(0), m_mepStart(0), m_needFree(false) {}
    /// Standard destructor 
    virtual ~OnlineContext() {}
    /// IEvtSelector::Context overload; context identifier
    virtual void* identifier() const                           { return (void*)m_sel; }
    virtual const std::vector<LHCb::RawBank*>& banks() const   { return m_banks;      }
    const LHCb::RawEventDescriptor& descriptor() const         { return m_evdesc;     }
    StatusCode receiveEvent();
    StatusCode freeEvent();
    StatusCode connect(const std::string& input);
    StatusCode connectMEP(const std::string& input);
    StatusCode connectMBM(const std::string& input);
    void close();
  };
}
#endif // GAUDIONLINE_ONLINECONTEXT_H

// $Id: OnlineEvtSelector.cpp,v 1.28 2007-10-29 14:33:29 frankm Exp $
//====================================================================
//  OnlineEvtSelector.cpp
//--------------------------------------------------------------------
//
//  Package    : GaudiOnline
//
//  Description: The GaudiOnline::OnlineEvtSelector component is able
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
//  | Client |    | EvtSelector |  | OnlineContext |
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
//
#include <cstring>
#include "GaudiKernel/xtoa.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "RTL/rtl.h"
#include "MBM/mepdef.h"
#include "MDF/StorageTypes.h"
#include "MDF/RawDataAddress.h"
#include "MDF/RawEventHelpers.h"

#include "GaudiKernel/SvcFactory.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,OnlineEvtSelector)
using namespace LHCb;

StatusCode OnlineContext::freeEvent()  {
  if ( m_consumer )  {
    if ( m_needFree )   {
      try  {
        m_consumer->freeEvent();
        m_needFree = false;
      }
      catch(...) {
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode OnlineContext::receiveEvent()  {
  m_banks.clear();
  if ( m_consumer )  {
    try  {
      m_sel->increaseReqCount();
      m_needFree = false;
      if ( m_consumer->getEvent() == MBM_NORMAL )  {
        const MBM::EventDesc& e = m_consumer->event();
        m_evdesc.setPartitionID(m_consumer->partitionID());
        m_evdesc.setTriggerMask(e.mask);
        m_evdesc.setEventType(e.type);
        m_evdesc.setHeader(e.data);
        m_evdesc.setSize(e.len);
        m_needFree = true;
        if ( m_sel->mustDecode() && e.type == EVENT_TYPE_EVENT )  {
          m_evdesc.setMepBuffer(m_mepStart);
          m_sel->increaseEvtCount();
          for(int i=0, n=m_evdesc.numberOfFragments(); i<n; ++i)
            decodeFragment(m_evdesc.fragment(i), m_banks);
        }
        return StatusCode::SUCCESS;
      }
    }
    catch(const std::exception& e)  {
      m_sel->error(std::string("Failed to read next event:")+e.what());
    }
    catch(...)  {
      m_sel->error("Failed to read next event - Unknown exception.");
    }
  }
  return StatusCode::FAILURE;
}

StatusCode OnlineContext::connect(const std::string& input)  {
  StatusCode sc = StatusCode::SUCCESS;
  if ( input=="EVENT" || input=="RESULT" || input=="MEP" )
    sc = connectMEP(input);
  else 
    sc = connectMBM(input);
  if ( sc.isSuccess() )  {
    for (int i=0, n=m_sel->m_nreqs; i<n; ++i )  {
      m_consumer->addRequest(m_sel->m_Reqs[i]);
    }
    return sc;
  }
  return sc;
}

StatusCode OnlineContext::connectMEP(const std::string& input)  {
  MEPID mepID = m_sel->m_mepMgr->mepID();
  if ( mepID != MEP_INV_DESC )  {
    m_mepStart = (void*)mepID->mepStart;
    if ( input == "EVENT" )  {
      m_consumer = new MBM::Consumer(mepID->evtBuffer,mepID->processName,mepID->partitionID);
    }
    else if ( input == "MEP" )  {
      m_consumer = new MBM::Consumer(mepID->mepBuffer,mepID->processName,mepID->partitionID);
    }
    else if ( input == "RESULT" )  {
      m_consumer = new MBM::Consumer(mepID->resBuffer,mepID->processName,mepID->partitionID);
    }
    if ( m_consumer )  {
      if ( m_consumer->id() != MBM_INV_DESC )  {
        return StatusCode::SUCCESS;
      }
    }
  }
  return StatusCode::FAILURE;
}

StatusCode OnlineContext::connectMBM(const std::string& input)  {
  if ( m_sel )  {
    std::map<std::string,BMID>::const_iterator i = m_sel->m_mepMgr->buffers().find(input);
    if ( i != m_sel->m_mepMgr->buffers().end() ) {
      m_consumer = new MBM::Consumer((*i).second,RTL::processName(),m_sel->m_mepMgr->partitionID());
      if ( m_consumer->id() == MBM_INV_DESC ) {
        return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}

void OnlineContext::close()  {
  if ( m_consumer )  {
    for (int i=0, n=m_sel->m_nreqs; i<n; ++i )  {
      m_consumer->delRequest(m_sel->m_Reqs[i]);
    }
    delete m_consumer;
    m_consumer = 0;
  }
  m_mepStart = 0;
}

OnlineEvtSelector::OnlineEvtSelector(const std::string& nam, ISvcLocator* svc)
: OnlineService(nam,svc), m_suspendLock(0), m_mepMgr(0), m_evtCount(0),m_reqCount(0)
{
  // Requirement format:
  // "EvType=x;TriggerMask=0xfeedbabe,0xdeadfeed,0xdeadbabe,0xdeadaffe;
  //  VetoMask=0x,0x,0x,0x;MaskType=ANY/ALL;UserType=USER/VIP/ONE;
  //  Frequency=MANY/PERC;Perc=20.5"
  declareProperty("Input",m_input  = "EVENT");
  declareProperty("Decode",m_decode = true);
  declareProperty("AllowSuspend",m_allowSuspend = false);
  // declareProperty("PartitionID",m_partID = 0x103);
  // declareProperty("PartitionBuffer",m_partitionBuffer=false);
  declareProperty("REQ1", m_Rqs[0] = "");
  declareProperty("REQ2", m_Rqs[1] = "");
  declareProperty("REQ3", m_Rqs[2] = "");
  declareProperty("REQ4", m_Rqs[3] = "");
  declareProperty("REQ5", m_Rqs[4] = "");
  declareProperty("REQ6", m_Rqs[5] = "");
  declareProperty("REQ7", m_Rqs[6] = "");
  declareProperty("REQ8", m_Rqs[7] = "");
}

// IInterface::queryInterface
StatusCode OnlineEvtSelector::queryInterface(const InterfaceID& riid, void** ppvIf) {
  if ( riid == IID_IEvtSelector )
    *ppvIf = (IEvtSelector*)this;
  else if ( riid == IID_ISuspendable )
    *ppvIf = (ISuspendable*)this;
  else
    return OnlineService::queryInterface( riid, ppvIf );
  addRef();
  return SUCCESS;
}

// IService implementation: Db event selector override
StatusCode OnlineEvtSelector::initialize()    {
  // Initialize base class
  StatusCode status = OnlineService::initialize();
  if ( !status.isSuccess() )    {
    return error("Error initializing base class Service!");
  }
  // Create lock to steer suspend/resume operations
  if ( !lib_rtl_is_success(lib_rtl_create_event(0,&m_suspendLock)) )  {
    return error("Cannot create lock to suspend operations.");
  }
  // Need to set initial event state in case selector works standalone
  // ie. without throttle using suspend/resume
  if ( !lib_rtl_is_success(lib_rtl_set_event(m_suspendLock)) )  {
    return error("Cannot activate event lock.");
  }
  status = service("MEPManager",m_mepMgr);
  if ( !status.isSuccess() )   {
    return error("Failed to access service MEPManager.");
  }
  for ( m_nreqs=0; m_nreqs<8; ++m_nreqs )  {
    if ( !m_Rqs[m_nreqs].empty() )   {
      m_Reqs[m_nreqs].parse(m_Rqs[m_nreqs]);
      continue;
    }
    break;
  }
  m_evtCount = 0;
  m_reqCount = 0;
  declareInfo("EventsIn",m_evtCount=0,"Event received counter");
  declareInfo("EventsReq",m_reqCount=0,"Event request counter");
  incidentSvc()->addListener(this,"DAQ_CANCEL");
  return status;
}

// IService implementation: Service finalization
StatusCode OnlineEvtSelector::finalize()    {
  // Release lock to steer suspend/resume operations
  ::lib_rtl_delete_event(m_suspendLock);
  m_suspendLock = 0;
  // Initialize base class
  m_evtCount = 0;
  m_nreqs = 0;
  if ( m_mepMgr )  {
    m_mepMgr->release();
    m_mepMgr = 0;
  }
  undeclareInfo("EventsIn");
  undeclareInfo("EventsReq");
  m_evtCount = 0;
  m_reqCount = 0;
  return OnlineService::finalize();
}

// Incident handler implemenentation: Inform that a new incident has occured
void OnlineEvtSelector::handle(const Incident& inc)    {
  info("Got incident:"+inc.source()+" of type "+inc.type());
  if ( inc.type() == "DAQ_CANCEL" )  {
    if ( m_allowSuspend ) lib_rtl_unlock(m_suspendLock);
  }
}

// ISuspendable implementation: suspend operation
StatusCode OnlineEvtSelector::suspend()  {
  if ( m_allowSuspend ) {
    if ( !lib_rtl_is_success(lib_rtl_clear_event(m_suspendLock)) )  {
      return error("Cannot lock to suspend operations.");
    }
  }
  return StatusCode::SUCCESS;
}

// ISuspendable implementation: resume operation
StatusCode OnlineEvtSelector::resume()   {
  if ( m_allowSuspend ) {
    if ( !lib_rtl_is_success(lib_rtl_set_event(m_suspendLock)) )  {
      // What to do ?? 
      // This is called too often and thread safe access to the state
      // is not simple....it would require a second lock to embrace:
      //
      // obj->lock();
      // if ( obj->isSuspended() )  {
      //   obj->resume()
      // }
      // obj->unlock();
      //
      // return error("Cannot unlock to resume operations.");
      error("Cannot unlock to resume operations.");
    }
  }
  return StatusCode::SUCCESS;
}

// Create event selector iteration context
StatusCode OnlineEvtSelector::createContext(Context*& refpCtxt) const  {
  OnlineContext* ctxt = new OnlineContext(this);
  refpCtxt = ctxt;
  return ctxt->connect(m_input);
}

// Request next event
StatusCode OnlineEvtSelector::next(Context& ctxt) const {
  try  {
    OnlineContext* pCtxt = dynamic_cast<OnlineContext*>(&ctxt);
    if ( pCtxt != 0 )   {
      pCtxt->freeEvent();
      // Need to aquire the mutex if suspended
      if ( m_allowSuspend ) {
        if ( !lib_rtl_is_success(lib_rtl_wait_for_event(m_suspendLock)) )  {
          error("Cannot lock to suspend operations.");
        }
      }
      StatusCode sc = pCtxt->receiveEvent();
      return sc;
    }
  }
  catch(const std::exception& e)  {
    error("Exception: "+std::string(e.what())+" Failed to receive the next event.");
  }
  catch(...)  {
    error("Unknown exception: Failed to receive the next event.");
  }
  return StatusCode::FAILURE;
}

// Access last item in the iteration
StatusCode OnlineEvtSelector::last(Context& /*refContext*/) const  {
  return error(" EventSelector::last(ctxt) not supported ");
}

StatusCode OnlineEvtSelector::next(Context& /* ctxt */, int /* jump */) const {
  return error(" EventSelector::next(ctxt, jump) not supported ");
}

StatusCode OnlineEvtSelector::previous(Context& /* ctxt */ ) const  {
  return error(" EventSelector::previous(ctxt) not supported ");
}

StatusCode OnlineEvtSelector::previous(Context& /* ctxt */, int /* jump */) const {
  return error(" EventSelector::previous(ctxt,jump) -- not supported ");
}

StatusCode OnlineEvtSelector::rewind(Context& /* ctxt */) const {
  return error(" EventSelector Iterator, operation rewind not supported ");
}

StatusCode 
OnlineEvtSelector::createAddress(const Context& ctxt, IOpaqueAddress*& pAddr) const
{
  const OnlineContext* pctxt = dynamic_cast<const OnlineContext*>(&ctxt);
  if ( pctxt )   {
    const RawEventDescriptor& d = pctxt->descriptor();
    unsigned long   p0 = (unsigned long)&d;
    RawDataAddress* pA = new RawDataAddress(RAWDATA_StorageType,CLID_DataObject,"","0",p0,0);
    pA->setData(std::pair<char*,int>(0,0));
    if ( m_decode && d.eventType() == EVENT_TYPE_EVENT )  {
      pA->setType(RawDataAddress::BANK_TYPE);
      pA->setBanks(&pctxt->banks());
    }
    else if ( d.eventType() == EVENT_TYPE_EVENT ) {
      pA->setType(RawDataAddress::DATA_TYPE);
      pA->setData(std::make_pair((char*)d.header(),d.size()));
    }
    else if ( d.eventType() == EVENT_TYPE_MEP ) {
      pA->setType(RawDataAddress::MEP_TYPE);
      MEPEVENT* ev = (MEPEVENT*)d.header();
      pA->setData(std::make_pair(ev->data,d.size()));
    }
    else  {
      pA->setData(std::make_pair((char*)d.header(),d.size()));
    }
    pA->setFileOffset(0);
    pAddr = pA;
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode OnlineEvtSelector::releaseContext(Context*& ctxt) const  {
  OnlineContext* pCtxt = dynamic_cast<OnlineContext*>(ctxt);
  if ( pCtxt ) {
    pCtxt->close();
    delete pCtxt;
    pCtxt = 0;
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode 
OnlineEvtSelector::resetCriteria(const std::string& crit,Context& ct) const {
  OnlineContext* ctxt = dynamic_cast<OnlineContext*>(&ct);
  if ( ctxt )  {
    ctxt->close();
    if ( ctxt->connect(m_input).isSuccess() )  {
      return StatusCode::SUCCESS;
    }
    return error("Failed to connect to:"+crit);
  }
  return error("Invalid iteration context. Cannot connect to:"+crit);
}
