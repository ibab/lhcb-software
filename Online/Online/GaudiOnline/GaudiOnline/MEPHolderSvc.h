//	============================================================
//
//	MEPHolderSvc.h
//	------------------------------------------------------------
//
//	Package   : GaudiOnline
//
//	Author    : Markus Frank
//
//	===========================================================
#ifndef GAUDIONLINE_MEPHOLDERSVC_H
#define GAUDIONLINE_MEPHOLDERSVC_H 1

// Framework include files
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiOnline/OnlineService.h"

// Forward declarations
class IIncidentSvc;
namespace MBM  {  
  class Consumer;
  class Producer;
}

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  // Forward declarations
  class MEPManager;

  /** @class MEPHolderSvc MEPHolderSvc.h GaudiOnline/MEPHolderSvc.h
    *
    *  @author Markus Frank
    *  @version 1.0
    */
  class MEPHolderSvc : public OnlineService, virtual public IRunable  {
  public:
    typedef std::vector<std::string>        Requirements;

  protected:
    /// Reference to MEP manager service
    MEPManager*      m_mepMgr;
    /// MBM Consumer pointer
    MBM::Consumer*   m_consumer;
    /// MBM Producer pointer for ERROR stream
    MBM::Producer*   m_producer;
    /// Option: Request specification
    Requirements     m_req;
    /// Property: Name of output buffer for error events (requires handleErrs to be set)
    std::string      m_prodBuffer;
    /// Property: MEP manager service name/type
    std::string      m_mepMgrName;
    /// Property: 4rth. Word of trigger mask for sending ERROR MEPs. 
    unsigned int     m_routingBits;
    /// Property: Flag to register callback for MEP error event handling. 0=No handling, 1=save as MEP, 2=save as events
    int              m_handleErrs;
    /// Flag indicating that MBM event retrieval is active
    bool             m_receiveEvts;
    /// Timeout before finally releasing MEP after MBM CANCEL
    int              m_releaseTMO;
    /// Monitoring quantity: Number of events received from network
    int              m_evtCount;

  public:
    /// Standard Constructor
    MEPHolderSvc(const std::string& name, ISvcLocator* svc);
    /// Standard Destructor
    virtual ~MEPHolderSvc();
    /// IInterface implementation: Query interface
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
    /// Incident handler implemenentation: Inform that a new incident has occured
    void handle(const Incident& inc);
    /// Service overload: initialize()
    virtual StatusCode initialize();
    /// Service overload: finalize()
    virtual StatusCode finalize();
    /// Process events
    virtual StatusCode run();
    /// Save event data on error
    StatusCode saveOnError(void* data, size_t length);
    /// Save MEP event data on error
    StatusCode saveMEP(void* data, size_t length);
    /// Save event data on error as individual events
    StatusCode saveEvents(void* data, size_t length);
  };
}      // End namespace LHCb
#endif //  GAUDIONLINE_MEPHOLDERSVC_H
