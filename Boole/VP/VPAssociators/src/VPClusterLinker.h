#ifndef VPCLUSTERLINKER_H
#define VPCLUSTERLINKER_H 1
// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// Event
#include "Event/MCHit.h"
#include "Event/VPDigit.h"

/** @class VPClusterLinker VPClusterLinker.h
 *
 */

class VPClusterLinker : public GaudiAlgorithm {

public:
  /// Standard constructor
  VPClusterLinker(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VPClusterLinker(); 

  virtual StatusCode execute();          ///< Algorithm execution

private:

  std::string m_clusterLocation;
  std::string m_digitLocation;
  std::string m_asctLocation;
  std::string m_hitLinkLocation;
  std::string m_particleLinkLocation;

};

#endif
