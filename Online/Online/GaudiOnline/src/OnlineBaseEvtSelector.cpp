// $Id: OnlineBaseEvtSelector.cpp,v 1.5 2008-10-06 11:49:19 frankb Exp $
//====================================================================
//  OnlineBaseEvtSelector.cpp
//--------------------------------------------------------------------
//
//  Package    : GaudiOnline
//
//  Description: OnlineBaseEvtSelector component is able
//               to produce a list of event references given 
//               a set of "selection criteria".
//
//  Author     : M.Frank
//====================================================================
//
#include "GaudiOnline/OnlineBaseEvtSelector.h"
#include <cstring>
#include "GaudiKernel/xtoa.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "MDF/StorageTypes.h"
#include "MDF/RawDataAddress.h"
#include "MDF/RawEventHelpers.h"
#include "MBM/mepdef.h"

using namespace std;
using namespace LHCb;

/// Standard constructor
OnlineContext::OnlineContext(const OnlineBaseEvtSelector* s) : m_sel(s), m_needFree(false) {
  m_evdesc.setHeader(0);
  m_evdesc.setSize(0);
}

OnlineBaseEvtSelector::OnlineBaseEvtSelector(const string& nam, ISvcLocator* svc)
: OnlineService(nam,svc), m_suspendLock(0), m_evtCount(0), m_reqCount(0), m_isWaiting(false)
{
  // Requirement format:
  // "EvType=x;TriggerMask=0xfeedbabe,0xdeadfeed,0xdeadbabe,0xdeadaffe;
  //  VetoMask=0x,0x,0x,0x;MaskType=ANY/ALL;UserType=USER/VIP/ONE;
  //  Frequency=MANY/PERC;Perc=20.5"
  declareProperty("Input", m_input);
  declareProperty("Decode",m_decode);
  declareProperty("AllowSuspend",m_allowSuspend = false);
  declareProperty("REQ1", m_Rqs[0] = "");
  declareProperty("REQ2", m_Rqs[1] = "");
  declareProperty("REQ3", m_Rqs[2] = "");
  declareProperty("REQ4", m_Rqs[3] = "");
  declareProperty("REQ5", m_Rqs[4] = "");
  declareProperty("REQ6", m_Rqs[5] = "");
  declareProperty("REQ7", m_Rqs[6] = "");
  declareProperty("REQ8", m_Rqs[7] = "");
  // Note: This is purely dummy! 
  // Only present for backwards compatibility with offline
  declareProperty("PrintFreq",m_printFreq);
}

// IInterface::queryInterface
StatusCode OnlineBaseEvtSelector::queryInterface(const InterfaceID& riid, void** ppvIf) {
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
StatusCode OnlineBaseEvtSelector::initialize()    {
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
  suspend();
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
StatusCode OnlineBaseEvtSelector::finalize()    {
  // Release lock to steer suspend/resume operations
  ::lib_rtl_delete_event(m_suspendLock);
  m_suspendLock = 0;
  // Initialize base class
  m_evtCount = 0;
  m_nreqs = 0;
  undeclareInfo("EventsIn");
  undeclareInfo("EventsReq");
  m_evtCount = 0;
  m_reqCount = 0;
  return OnlineService::finalize();
}

// Incident handler implemenentation: Inform that a new incident has occured
void OnlineBaseEvtSelector::handle(const Incident& inc)    {
  info("Got incident:"+inc.source()+" of type "+inc.type());
  if ( inc.type() == "DAQ_CANCEL" )  {
    if ( m_allowSuspend ) lib_rtl_unlock(m_suspendLock);
  }
}

// ISuspendable implementation: suspend operation
StatusCode OnlineBaseEvtSelector::suspend() const  {
  if ( m_allowSuspend ) {
    if ( !lib_rtl_is_success(lib_rtl_clear_event(m_suspendLock)) )  {
      return error("Cannot lock to suspend operations.");
    }
  }
  return StatusCode::SUCCESS;
}

// ISuspendable implementation: resume operation
StatusCode OnlineBaseEvtSelector::resume() const  {
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

// Request next event
StatusCode OnlineBaseEvtSelector::next(Context& ctxt) const {
  try  {
    OnlineContext* pCtxt = dynamic_cast<OnlineContext*>(&ctxt);
    if ( pCtxt != 0 )   {
      StatusCode sc = pCtxt->freeEvent();
      if ( !sc.isSuccess() ) {}
      // Need to aquire the mutex if suspended
      sc = pCtxt->rearmEvent();
      if ( !sc.isSuccess() ) {
	return sc;
      }
      if ( m_allowSuspend )   {
        m_isWaiting = true;
        if ( !lib_rtl_is_success(lib_rtl_wait_for_event(m_suspendLock)) )  {
          error("Cannot lock to suspend operations.");
        }
        m_isWaiting = false;
      }
      sc = pCtxt->receiveEvent();
      return sc;
    }
  }
  catch(const exception& e)  {
    error("Exception: "+string(e.what())+" Failed to receive the next event.");
  }
  catch(...)  {
    error("Unknown exception: Failed to receive the next event.");
  }
  return StatusCode::FAILURE;
}

// Access last item in the iteration
StatusCode OnlineBaseEvtSelector::last(Context& /*refContext*/) const  {
  return error(" EventSelector::last(ctxt) not supported ");
}

StatusCode OnlineBaseEvtSelector::next(Context& /* ctxt */, int /* jump */) const {
  return error(" EventSelector::next(ctxt, jump) not supported ");
}

StatusCode OnlineBaseEvtSelector::previous(Context& /* ctxt */ ) const  {
  return error(" EventSelector::previous(ctxt) not supported ");
}

StatusCode OnlineBaseEvtSelector::previous(Context& /* ctxt */, int /* jump */) const {
  return error(" EventSelector::previous(ctxt,jump) -- not supported ");
}

StatusCode OnlineBaseEvtSelector::rewind(Context& /* ctxt */) const {
  return error(" EventSelector Iterator, operation rewind not supported ");
}

StatusCode 
OnlineBaseEvtSelector::createAddress(const Context& ctxt, IOpaqueAddress*& pAddr) const
{
  const OnlineContext* pctxt = dynamic_cast<const OnlineContext*>(&ctxt);
  if ( pctxt )   {
    const RawEventDescriptor& d = pctxt->descriptor();
    unsigned long   p0 = (unsigned long)&d;
    RawDataAddress* pA = new RawDataAddress(RAWDATA_StorageType,CLID_DataObject,"","0",p0,0);
    pA->setData(pair<char*,int>(0,0));
    if ( m_decode && d.eventType() == EVENT_TYPE_EVENT )  {
      pA->setType(RawDataAddress::BANK_TYPE);
      pA->setBanks(&pctxt->banks());
    }
    else if ( d.eventType() == EVENT_TYPE_EVENT ) {
      pA->setType(RawDataAddress::DATA_TYPE);
      if ( d.size()>0 ) pA->setData(make_pair((char*)d.header(),d.size()));
    }
    else if ( d.eventType() == EVENT_TYPE_MEP ) {
      pA->setType(RawDataAddress::MEP_TYPE);
      MEPEVENT* ev = (MEPEVENT*)d.header();
      pA->setData(make_pair(ev->data,d.size()));
    }
    else  {
      pA->setData(make_pair((char*)d.header(),d.size()));
    }
    pA->setFileOffset(0);
    pAddr = pA;
    //return pA->data().first ? StatusCode::SUCCESS : StatusCode::FAILURE;
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode OnlineBaseEvtSelector::releaseContext(Context*& ctxt) const  {
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
OnlineBaseEvtSelector::resetCriteria(const string& crit,Context& ct) const {
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
