//  ============================================================
//
//  MEPHolderSvc.cpp
//  ------------------------------------------------------------
//
//  Package   : GaudiOnline
//
//  Author    : Markus Frank
//
//  ===========================================================
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiOnline/MEPHolderSvc.h"
#include "GaudiOnline/MEPManager.h"
#include "MBM/Consumer.h"
#include "MBM/Requirement.h"

using MBM::Consumer;
using MBM::EventDesc;
using MBM::Requirement;

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPHolderSvc)

// Standard Constructor
LHCb::MEPHolderSvc::MEPHolderSvc(const std::string& nam, ISvcLocator* svc)
: OnlineService(nam, svc), m_mepMgr(0), m_consumer(0), m_receiveEvts(false),
  m_evtCount(0)
{
  declareProperty("Requirements",   m_req);
  declareProperty("ReleaseTimeout", m_releaseTMO=1000);
  declareProperty("MEPManager",     m_mepMgrName="LHCb::MEPManager/MEPManager");
}

// Standard Destructor
LHCb::MEPHolderSvc::~MEPHolderSvc()   {
}

// IInterface implementation: Query interface
StatusCode LHCb::MEPHolderSvc::queryInterface(const InterfaceID& riid,void** ppIf) {
  if ( riid == IID_IRunable )  {
    *ppIf = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return OnlineService::queryInterface(riid, ppIf);
}

// Incident handler implemenentation: Inform that a new incident has occured
void LHCb::MEPHolderSvc::handle(const Incident& inc)    {
  info("Got incident:"+inc.source()+" of type "+inc.type());
  if ( inc.type() == "DAQ_CANCEL" )  {
    m_receiveEvts = false;
    m_mepMgr->cancel();
  }
  else if ( inc.type() == "DAQ_ENABLE" )  {
    m_receiveEvts = true;
  }
}

StatusCode LHCb::MEPHolderSvc::initialize()  {
  StatusCode sc = OnlineService::initialize();
  if ( sc.isSuccess() )  {
    declareInfo("MEPsIn",m_evtCount=0,"Number of events received from network");
    if ( service(m_mepMgrName,m_mepMgr).isSuccess() )  {
      incidentSvc()->addListener(this,"DAQ_CANCEL");
      try {
        int partID = m_mepMgr->partitionID();
        const std::string& proc = m_mepMgr->processName();
        MEPID id = m_mepMgr->mepID();
        m_consumer = new Consumer(id->mepBuffer,proc,partID);
        for(Requirements::iterator i=m_req.begin(); i!=m_req.end(); ++i)  {
          Requirement r;
          r.parse(*i);
          m_consumer->addRequest(r);
        }
        return StatusCode::SUCCESS;
      }
      catch( std::exception& e)  {
        return error("Failed setup MEP buffers:"+std::string(e.what()));
      }
    }
    return error("Failed to access MEP manager service.");
  }
  return error("Failed to initialize service base class.");
}

StatusCode LHCb::MEPHolderSvc::finalize()  {
  undeclareInfo("MEPsIn");
  if ( m_consumer    )  {
    delete m_consumer;
    m_consumer = 0;
  }
  if ( m_mepMgr      )  {
    m_mepMgr->release();
    m_mepMgr = 0;
  }
  return OnlineService::finalize();
}

// Process single event
StatusCode LHCb::MEPHolderSvc::run()  {
  m_receiveEvts = true;
  MEPID id = m_mepMgr->mepID();
  int m_procType = 1;
  try  {
    debug("Starting event loop ....");
    if ( m_procType == 1 )  {
      while(m_receiveEvts)  {
        mep_scan(id,0);
        if ( m_receiveEvts )  {
          ::lib_rtl_usleep(m_releaseTMO);
        }
      }
      debug("Leaving event loop ....");
      return StatusCode::SUCCESS;
    }

    for(int sc=m_consumer->getEvent();sc==MBM_NORMAL && m_receiveEvts; sc=m_consumer->getEvent())  {
      const EventDesc& evt = m_consumer->event();
      MEPEVENT* e = (MEPEVENT*)(int*)evt.data;
      if ( e->magic != mep_magic_pattern() )  {
        lib_rtl_printf("Bad magic MEP pattern !!!!\n");
      }
      while ( 1 )  {
        if ( e->refCount <= 1 )    {
          if ( e->refCount != 1 )    {
            error("MEP release [%d] Event at address %08X MEP:%08X [%d] Pattern %08X",
              e->refCount,id->mepStart+e->begin,e,e->evID,e->magic);
          }
          break;
        }
        lib_rtl_usleep(100);
        if ( !m_receiveEvts )  {
          if ( e->refCount > 1 )    {
            lib_rtl_sleep(m_releaseTMO);
          }
          break;
        }
      }
      m_evtCount++;
      m_consumer->freeEvent();
    }
    debug("Leaving event loop ....");
    return StatusCode::SUCCESS;
  }
  catch(const std::exception& e)  {
    return error("Exception during event processing:"+std::string(e.what()));
    return StatusCode::FAILURE;
  }
  catch(...)  {
    return error("Unknown exception during event processing.");
  }
}
