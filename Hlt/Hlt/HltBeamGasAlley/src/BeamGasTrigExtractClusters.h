
#ifndef INCLUDE_BEAMGASTRIGEXTRACTCLUSTERS_H
#define INCLUDE_BEAMGASTRIGEXTRACTCLUSTERS_H 1

#include <string>
#include "GaudiAlg/GaudiAlgorithm.h"

namespace Tf { class DefaultVeloRHitManager; }

//---------------------------------------------------------------------------------
// @class BeamGasTrigExtractClusters BeamGasTrigExtractClusters.h
// A simple GaudiAlgorithm that extracts all unused R clusters, using a hit manager
// tool and puts them on a new location on the TES. 
//---------------------------------------------------------------------------------
 
class BeamGasTrigExtractClusters : public GaudiAlgorithm {

public:

  /// Standard Constructor
  BeamGasTrigExtractClusters(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~BeamGasTrigExtractClusters(); ///< Destructor

  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode    execute(); ///< Algorithm event execution
  virtual StatusCode   finalize(); ///< Algorithm finalize

private:

  std::string 	m_hitManagerName;
  std::string 	m_outputLocation;
  
  Tf::DefaultVeloRHitManager* m_rHitManager;
};
#endif // INCLUDE_BEAMGASTRIGEXTRACTCLUSTERS_H

