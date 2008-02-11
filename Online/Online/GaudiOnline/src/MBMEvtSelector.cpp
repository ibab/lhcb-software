// $Id: MBMEvtSelector.cpp,v 1.2 2008-02-11 20:02:08 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MBMEvtSelector.cpp,v 1.2 2008-02-11 20:02:08 frankm Exp $
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
    void*                 m_mepStart;
    StatusCode connectMEP(const std::string& input);
    StatusCode connectMBM(const std::string& input);
  public:
    /// Standard constructor
    MBMContext(MBMEvtSelector* pSelector);
    /// Standard destructor 
    virtual ~MBMContext() {}
    virtual StatusCode connect(const std::string& input);
    virtual StatusCode receiveEvent();
    virtual StatusCode rearmEvent();
    virtual StatusCode freeEvent();
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

    // IService implementation: Service finalization
    virtual StatusCode finalize();

    /// Create a new event loop context
    /** @param refpCtxt   [IN/OUT]  Reference to pointer to store the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode createContext(Context*& refpCtxt) const;

    /// Service Constructor
    MBMEvtSelector(const std::string& name, ISvcLocator* svcloc);

    /// Standard destructor
    virtual ~MBMEvtSelector()    {}

  protected:
    /// Data Members
    LHCb::IMEPManager*            m_mepMgr;
  };
}
#endif // GAUDIONLINE_MBMEVTSELECTOR_H

// $Id: MBMEvtSelector.cpp,v 1.2 2008-02-11 20:02:08 frankm Exp $
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

namespace LHCb {
  // Backwards component compatibility
  class OnlineEvtSelector : public MBMEvtSelector  {
  public:
    OnlineEvtSelector(const std::string& nam,ISvcLocator* svc): MBMEvtSelector(nam,svc) {}
    virtual ~OnlineEvtSelector()    {}
  };
}
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,OnlineEvtSelector)
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MBMEvtSelector)
using namespace LHCb;

MBMContext::MBMContext(MBMEvtSelector* s)
: OnlineContext(s), m_onlineSel(s), m_consumer(0), m_mepStart(0)
{
}

StatusCode MBMContext::freeEvent()  {
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

StatusCode MBMContext::rearmEvent()  {
  return StatusCode::SUCCESS;
}

StatusCode MBMContext::receiveEvent()  {
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

StatusCode MBMContext::connect(const std::string& input)  {
  StatusCode sc = StatusCode::SUCCESS;
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
  return sc;
}

StatusCode MBMContext::connectMEP(const std::string& input)  {
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

StatusCode MBMContext::connectMBM(const std::string& input)  {
  if ( m_sel )  {
    IMEPManager* m = m_onlineSel->mepMgr();
    std::map<std::string,BMID>::const_iterator i = m->buffers().find(input);
    if ( i != m->buffers().end() ) {
      m_consumer = new MBM::Consumer((*i).second,RTL::processName(),m->partitionID());
      if ( m_consumer->id() == MBM_INV_DESC ) {
        return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}

void MBMContext::close()  {
  if ( m_consumer )  {
    for (int i=0, n=m_sel->numReq(); i<n; ++i )  {
      m_consumer->delRequest(m_sel->req(i));
    }
    delete m_consumer;
    m_consumer = 0;
  }
  m_mepStart = 0;
}

MBMEvtSelector::MBMEvtSelector(const std::string& nam, ISvcLocator* svc)
: OnlineBaseEvtSelector(nam,svc), m_mepMgr(0)
{
  m_input = "EVENT";
  m_decode = true;
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

// IService implementation: Service finalization
StatusCode MBMEvtSelector::finalize()    {
  if ( m_mepMgr )  {
    m_mepMgr->release();
    m_mepMgr = 0;
  }
  return OnlineBaseEvtSelector::finalize();
}

// Create event selector iteration context
StatusCode MBMEvtSelector::createContext(Context*& refpCtxt) const  {
  MBMContext* ctxt = new MBMContext(const_cast<MBMEvtSelector*>(this));
  refpCtxt = ctxt;
  return ctxt->connect(m_input);
}
