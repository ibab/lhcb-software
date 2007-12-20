// $Id: COOLConfSvc.h,v 1.1 2007-12-20 15:48:52 marcocle Exp $
#ifndef COMPONENT_COOLCONFSVC_H 
#define COMPONENT_COOLCONFSVC_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/GaudiException.h"
#include "DetCond/ICOOLConfSvc.h"

// Forward declarations
template <class TYPE> class SvcFactory;

namespace cool {
  class Application;
  class RecordSpecification;
}

/** @class COOLConfSvc COOLConfSvc.h
 *  
 *  Class used as interface to LCG COOL library API. It should expose as less as
 *  possible COOL internal details.
 *
 *  @author Marco Clemencic
 *  @date   2005-01-11
 */

class COOLConfSvc: public Service,
                       public virtual ICOOLConfSvc {
public: 

  /** Query interfaces of Interface
      @param riid       ID of Interface to be retrieved
      @param ppvUnknown Pointer to Location for interface pointer
  */
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);

  /// Initilize COOL (CondDB) Configuration Service
  virtual StatusCode initialize();
  
  /// Finalize Service
  virtual StatusCode finalize();

  // --------- ICOOLConfSvc implementation
  /// Access to the SEAL context holding COOL and CORAL (if needed).
  virtual seal::Context *context();
  
  /// Get the COOL Database service (used to connect to the databases).
  virtual cool::IDatabaseSvc& databaseSvc();

protected:
  /// Standard constructor
  COOLConfSvc(const std::string& name, ISvcLocator* svcloc); 

  virtual ~COOLConfSvc( ); ///< Destructor

private:
  
  inline cool::Application *coolApplication(){
    if (!m_coolApplication)
      throw GaudiException("Attempt to access COOL instance before initialization",
                           "(COOLConfSvc)" + name() + "::coolApplication",
                           StatusCode::FAILURE);
    return m_coolApplication;
  }
  
  /// Pointer to a shared instance of the COOL Application
  cool::Application *m_coolApplication;

  /// Flag to turn off/on the CORAL LFCReplicaService (option UseLFCReplicaSvc, default = false).
  /// Setting this option works only if it is set for the first COOLConfSvc initialized
  /// because of a "feature" of CORAL.
  bool m_useLFCReplicaSvc;

  /// Flag to turn off/on the CORAL Automatinc connection clean up
  /// (option EnableCoralConnectionCleanUp, default = false).
  /// Setting this option works only if it is set for the first COOLConfSvc initialized.
  bool m_coralConnCleanUp;
  
  /// Time between two connection trials (in seconds).
  /// Passed to CORAL when loaded.
  int m_retrialPeriod;
   
  /// How long to keep retrying before giving up (in seconds).
  /// Passed to CORAL when loaded.
  int m_retrialTimeOut;
  
  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<COOLConfSvc>;

};
#endif // COMPONENT_COOLCONFSVC_H
