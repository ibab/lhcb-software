#ifndef VPCLUSTERLINKER_H
#define VPCLUSTERLINKER_H 1
// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class VPClusterLinker VPClusterLinker.h
 * This algorithm creates association tables between the VP clusters
 * and the corresponding MC hits and particles, based on the 
 * association tables for individual pixels produced by VPDigitLinker. 
 *
 */

class VPClusterLinker : public GaudiAlgorithm {

public:
  /// Standard constructor
  VPClusterLinker(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VPClusterLinker(); 

  /// Algorithm execution
  virtual StatusCode execute();

private:

  std::string m_clusterLocation;
  std::string m_inputLocation;
  std::string m_hitLinkLocation;
  std::string m_particleLinkLocation;

};

#endif
