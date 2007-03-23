// $Id: STCluster2MCParticleLinker.h,v 1.8 2007-03-23 14:30:45 cattanem Exp $
#ifndef STCLUSTER2MCPARTICLELINKER_H
#define STCLUSTER2MCPARTICLELINKER_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

namespace LHCb{
  class MCParticle;
};

/** @class STCluster2MCParticleAlg STCluster2MCParticleLinker.h
 *  
 *
 *  @author Matt Needham
 *  @date   26/11/2005
 */
class STCluster2MCParticleLinker : public GaudiAlgorithm {

  friend class AlgFactory<STCluster2MCParticleLinker>;
  
public:

  typedef std::pair<const LHCb::MCParticle*,double> PartPair;
  typedef std::map<const LHCb::MCParticle*,double> ParticleMap;

  /// Standard constructor
  STCluster2MCParticleLinker(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~STCluster2MCParticleLinker(); 

  /// init
  virtual StatusCode initialize();

  /// execute
  virtual StatusCode execute();    

  /// path to put table
  std::string outputData() const;
 
protected:

private:

  void refsToRelate(std::vector<PartPair>& selectedRefs,
                    const ParticleMap& partMap, 
                    LHCb::MCParticles* particles) const;

  StatusCode associateToTruth(const LHCb::STCluster* aCluster,
                              ParticleMap& partMap);

  /// Linker table location path
  std::string m_asctLocation;

  // job options
  std::string m_inputData; ///< Location of the STClusters
  std::string m_outputData;///< Location of the STClusters linker table
  bool m_addSpillOverHits; ///< Flag to add spill-over to linker table
  double m_minFrac;        ///< Minimal charge fraction to link to MCParticle
  bool m_oneRef;           ///< Flag to allow only 1 link for each cluster
  std::string m_detType;   ///< Detector type (IT or TT)
};

inline std::string STCluster2MCParticleLinker::outputData() const {
  return  m_outputData;
}


#endif // STCLUSTER2MCPARTICLELINKER_H







