//	============================================================
//
//	MEPConverterSvc.h
//	------------------------------------------------------------
//
//	Package   : GaudiOnline
//
//	Author    : Markus Frank
//
//	===========================================================
#ifndef GAUDIONLINE_MEPCONVERTERSVC_H
#define GAUDIONLINE_MEPCONVERTERSVC_H 1

// Framework include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/IIncidentListener.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawEventDescriptor.h"
#include <map>

// Forward declarations
class IIncidentSvc;
namespace MBM  {
  class Producer;
  class Consumer;
  class EventDesc;
};

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  // Forward declarations
  class MEPManager;

  /** @class MEPConverterSvc MEPConverterSvc.h GaudiOnline/MEPConverterSvc.h
    *
    *  @author Markus Frank
    *  @version 1.0
    */
  class MEPConverterSvc : public Service, 
                          virtual public IRunable,
                          virtual public IIncidentListener
  {
  public:
    typedef std::vector<LHCb::MEPFragment*> Frags;
    typedef std::map<unsigned int, Frags >  SubEvents;
    typedef std::vector<std::string>        Requirements;
  protected:
    /// Reference to MEP manager service
    MEPManager*      m_mepMgr;
    /// Reference to incident service
    IIncidentSvc*    m_incidentSvc;
    /// MBM Producer pointer
    MBM::Producer*   m_producer;
    /// MBM Consumer pointer
    MBM::Consumer*   m_consumer;
    /// Option: Request specification
    Requirements     m_req;
    /// Flag indicating that MBM event retrieval is active
    bool             m_receiveEvts;
    /// Property: MEP manager service name/type
    std::string      m_mepMgrName;
    /// Declare all subevents of a full MEP
    int declareSubEvents(const MBM::EventDesc& evt, SubEvents& events);
    /// Declare single subevent
    int declareSubEvent(const MBM::EventDesc& evt, int evID, const Frags& frags);
  public:
    /// Standard Constructor
    MEPConverterSvc(const std::string& name, ISvcLocator* svc);
    /// Standard Destructor
    virtual ~MEPConverterSvc();
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
  };
}      // End namespace LHCb
#endif //  GAUDIONLINE_MEPCONVERTERSVC_H
