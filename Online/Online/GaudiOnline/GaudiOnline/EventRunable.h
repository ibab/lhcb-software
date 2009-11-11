// $Id: EventRunable.h,v 1.5 2009-11-11 13:50:04 frankb Exp $
#ifndef GAUDISVC_EVENTRUNABLE_H
#define GAUDISVC_EVENTRUNABLE_H 1

// Framework includes
#include "GaudiKernel/IRunable.h"
#include "GaudiOnline/OnlineService.h"

// Forward declarations
class IIncidentSvc;
class IDataProvider;

/*
 *   LHCb namespace declaration
 */
namespace LHCb  {

  class MEPManager;

  /** Class definition of EventRunable.
    *
    * This is the online extension of the runnable of the application manager.
    * The runable handles the actual run() implementation of the
    * ApplicationMgr object.
    * This implementation just calls ApplicationMgr::nextEvent(...).
    *
    *  @author Markus Frank
    *  @version 1.0
    */
  class EventRunable : public OnlineService, virtual public IRunable  {

  protected:
    /// Reference to MEP manager service
    MEPManager*       m_mepMgr;
    /// IDataProvider pointer to event data service
    IDataProviderSvc* m_dataSvc;
    /// Property: MEP manager service name/type
    std::string       m_mepMgrName;
    /// Property: Timeout incident name
    std::string       m_tmoIncident;
    /// Flag indicating that MBM event retrieval is active
    bool              m_receiveEvts;
    /// Number of events to be processed
    int               m_evtMax;
    /// Property: Number of consecutive processing errors to stop
    int               m_nerrStop;
    /// Counter to consecutive processing errors
    int               m_nerr;
    /// Monitoring quantity: Number of events processed
    int               m_evtCount;
    /// Flag that indicates that we received an error event 
    bool	      m_errorFired;	 
    /// Flag that indicates that a timeout occurred
    bool	      m_eventTMO;

  public:
    /// Standard Constructor
    EventRunable(const std::string& nam, ISvcLocator* svcLoc);
    /// Standard Destructor
    virtual ~EventRunable();
    // IInterface implementation : queryInterface
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
    /// IService implementation: initialize the service
    virtual StatusCode initialize();
    /// IService implementation: finalize the service
    virtual StatusCode finalize();
    /// Incident handler implemenentation: Inform that a new incident has occured
    void handle(const Incident& inc);
    /// IRunable implementation : Run the class implementation
    virtual StatusCode run();
  };
}      // End namespace LHCb
#endif // GAUDISVC_EVENTRUNABLE_H
