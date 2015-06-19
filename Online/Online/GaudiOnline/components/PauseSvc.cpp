// $Id: PauseAlg.cpp,v 1.1 2009-12-03 19:05:31 frankb Exp $

// Include files from Gaudi
#include "GaudiKernel/Service.h" 
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "CPP/TimeSensor.h" 
#include "CPP/Interactor.h" 
#include "CPP/Event.h" 
#include "RTL/rtl.h"

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

  /** Algorithm moving into pause state after timeout -- for debugging only
   *
   *  \author Markus Frank
   *  \date   2015-10-13
   */
  class IncidentProvoker : public Interactor, public Service  {
    /// Timeout in seconds
    int   m_tmo;
    std::string m_inc;
  public: 
    /// Standard constructor
    IncidentProvoker(const std::string& nam,ISvcLocator* pSvc) : Service(nam,pSvc), m_tmo(10)    {
      declareProperty("Timeout", m_tmo);
      declareProperty("Incident", m_inc="DAQ_PAUSE");
    }
    /// Destructor
    virtual ~IncidentProvoker()  {} 
    /// Interactor handling callback
    virtual void handle(const Event& ev)         {
      MsgStream log(msgSvc(), name());
      log << MSG::ALWAYS << "Interactor callback here! " << endmsg;
      if ( ev.eventtype == TimeEvent )  {
        IIncidentSvc* svc = 0;
        StatusCode sc = service("IncidentSvc", svc, true);
        if ( sc.isSuccess() )  {
          svc->fireIncident(Incident(name(),m_inc));
          svc->release();
          return;
        }
        MsgStream log(msgSvc(), name());
        log << MSG::ERROR << "Failed to retrieve IIncidentSvc interface." << endmsg;
      }
    }
    /// Initialize the algorithm
    virtual StatusCode initialize()  {
      Service::initialize();
      MsgStream log(msgSvc(), name());
      log << MSG::ALWAYS << "Setting timer to " << m_tmo << " seconds." << endmsg;
      TimeSensor::instance().add(this,m_tmo,this);
      return StatusCode::SUCCESS;
    }
    /// Initialize the algorithm
    virtual StatusCode finalize()  {
      TimeSensor::instance().remove(this,0);
      return Service::finalize();
    }
    /// Main execution
    virtual StatusCode execute()  {
      return StatusCode::SUCCESS;
    }
  };

}

#include "GaudiKernel/SvcFactory.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,IncidentProvoker)
