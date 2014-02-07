#ifndef GAUDIONLINE_MONITORINGCLIENT_H
#define GAUDIONLINE_MONITORINGCLIENT_H

// Framework include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IMonitorSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"

// C++ includes
#include <stdexcept>

// Forward declarations
class MsgStream;



namespace AIDA {  class IHistogram; }

/*
 *   LHCb namespace declaration
 */
namespace LHCb  {

  /** @class MonitoringClient MonitoringClient.h GaudiOnline/MonitoringClient.h
    *
    *  This class used the IMonitorSvc interface, allocates the monitoring service
    *  and releases it.
    *
    *  @author Markus Frank
    */
  class OnlineService : public Service, virtual public IIncidentListener {
  protected:
    /// TYpe name of the monitoring service
    std::string m_monitorSvcType;
    /// Reference to monitoring service
    IMonitorSvc*  m_pMonitorSvc;
    /// Reference to incident service
    IIncidentSvc* m_incidentSvc;

    /// If not already located try to locate monitoring service with forcing a creation
    IMonitorSvc* monSvc()   const;
    /// Access incident service (Pointer invalid before initialize()!)
    IIncidentSvc* incidentSvc()  const;

    /// Helper to release interface pointers
    template <class T> inline void releaseInterface(T*& ptr) {
      if ( ptr ) {
	ptr->release();
	ptr = 0;
      }
    }

  public:
    /// Service constructor
    OnlineService(const std::string& nam, ISvcLocator* svc);

    /// Standard destructor
    virtual ~OnlineService();

    /// Implementation of IService::initialize()
    virtual StatusCode initialize();

    /// Implementation of IService::finalize()
    virtual StatusCode finalize();

    /// IInterface implementation: Query interface
    virtual StatusCode queryInterface(const InterfaceID& riid,void** ppIf);

    /** Declare named monitoring information items
      * @param nam    Monitoring information name known to the external system
      * @param var    Monitoring variable (by reference!)
      * @param dsc    Description of monitoring variable
      */
    template <class T>
    void declareInfo(const std::string& nam,const T& var,const std::string& dsc) const {
      monSvc()->declareInfo(nam, var, dsc, this);
    }
    //void declareInfo(const std::string& nam,const char* var,const std::string& dsc) const {
    //  monSvc()->declareInfo(nam, var, dsc, this);
    //}
    /** Undeclare monitoring information
      * @param nam    Monitoring information name knwon to the external system
      */
    virtual void undeclareInfo(const std::string& nam);
    
    /// Undeclare monitoring information
    virtual void undeclareAll();

    /// Dummy IIncidentListener implementation: Inform that a new incident has occured
    virtual void handle(const Incident& /* incident */ );

    /// Debug printout handling
    void debug(const char* msg,...) const;
    void debug(const std::string& msg) const;
    /// Info printout handling
    void info(const char* msg,...) const;
    void info(const std::string& msg) const;
    /// Warning printout handling
    void warning(const char* msg,...) const;
    void warning(const std::string& msg) const;
    /// Error handling
    StatusCode error(const std::string& msg) const;
    StatusCode error(const char* msg,...) const;
    StatusCode errorException(const std::string& msg, const std::exception& e) const;
    StatusCode throwError(const std::string& msg) const;
    StatusCode throwError(const char* msg,...) const;
    void output(int level, const char* msg,...)   const;
  };
}      // End namespace gaudi
#endif // GAUDIONLINE_MONITORINGCLIENT_H
