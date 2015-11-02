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
namespace coral {
  class IReplicaSortingAlgorithm;
}

/** @class COOLConfSvc COOLConfSvc.h
 *
 *  Class used as interface to LCG COOL library API. It should expose as less as
 *  possible COOL internal details.
 *
 *  @author Marco Clemencic
 *  @date   2005-01-11
 */

class COOLConfSvc: public extends1<Service, ICOOLConfSvc> {
public:
  /// Initialize COOL (CondDB) Configuration Service
  virtual StatusCode initialize();

  /// Finalize Service
  virtual StatusCode finalize();

  // --------- ICOOLConfSvc implementation
  /// Access to the CORAL connection service used by COOL (if needed).
  virtual coral::IConnectionService& connectionSvc();

  /// Get the COOL Database service (used to connect to the databases).
  virtual cool::IDatabaseSvc& databaseSvc();

protected:
  /// Standard constructor
  COOLConfSvc(const std::string& name, ISvcLocator* svcloc);

  virtual ~COOLConfSvc( ); ///< Destructor

private:

  inline cool::Application *coolApplication(){
    if (!m_coolApplication.get())
      throw GaudiException("Attempt to access COOL instance before initialization",
                           "(COOLConfSvc)" + name() + "::coolApplication",
                           StatusCode::FAILURE);
    return m_coolApplication.get();
  }

  /// Pointer to a shared instance of the COOL Application
  std::unique_ptr<cool::Application> m_coolApplication;

  std::unique_ptr<coral::IReplicaSortingAlgorithm> m_replicaSortAlg;

  /// Flag to turn off/on the CORAL LFCReplicaService (option UseLFCReplicaSvc, default = false).
  /// Setting this option works only if it is set for the first COOLConfSvc initialized
  /// because of a "feature" of CORAL.
  bool m_useLFCReplicaSvc;

  /// Name of the grid site the application is running on. It is meaningful only
  /// if m_useLFCReplicaSvc (option UseLFCReplicaSvc) is set to true. If not
  /// specified the value of the environment variables "DIRACSITE" or
  /// "LHCBPRODSITE" is used. If even the environment variables are not set,
  /// then it is assumed to be "CERN.ch". (Note: the value is case insensitive)
  std::string m_localSite;

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
