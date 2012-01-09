#ifndef SRC_SERVICESTARTER_H
#define SRC_SERVICESTARTER_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

namespace LHCbAlgsTest {

/** @class ServiceStarter ServiceStarter.h src/ServiceStarter.h
 *
 * Simple test algorithm to trigger the instantiation of a service in
 * a specific phase (initialize, start or execute).
 *
 * @author Marco Clemencic
 * @date 09/01/2012
 */
class ServiceStarter: public GaudiAlgorithm {
public:
  /// Standard constructor
  ServiceStarter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~ServiceStarter(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode start     ();    ///< Algorithm start
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
protected:
private:
  /// Service to get.
  std::string m_serviceName;

  /// When to get the service;
  std::string m_phase;

  /// Internal reference to the retrieved service.
  SmartIF<IService> m_service;

  StatusCode i_retrieveService(const std::string &currentPhase);
};

}
#endif // SRC_SERVICESTARTER_H
