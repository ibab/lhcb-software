// $Id: STCluster2MCParticleLinker.h,v 1.5 2006-04-12 13:29:17 mneedham Exp $
#ifndef STCLUSTER2MCPARTICLEALG_H
#define STCLUSTER2MCPARTICLEALG_H 1

#include <string>
#include <map>
#include <utility>
#include <vector>

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/MCParticle.h"


/** @class STCluster2MCParticleAlg STCluster2MCParticleAlg.h
 *  
 *
 *  @author Matt Needham
 *  @date   26/11/2005
 */
class STCluster2MCParticleLinker : public GaudiAlgorithm{

  friend class AlgFactory<STCluster2MCParticleLinker>;
  
public:

  typedef std::pair<const LHCb::MCParticle*,double> partPair;

  /// Standard constructor
  STCluster2MCParticleLinker( const std::string& name, ISvcLocator* pSvcLocator );

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

  StatusCode refsToRelate(std::vector<partPair>& selectedRefs,
                          const std::map<const LHCb::MCParticle*,double>& hitMap,
                          LHCb::MCParticles* particles) const;

  StatusCode associateToTruth(const LHCb::STCluster* aCluster,
			      std::map<const LHCb::MCParticle*,double>& particleMap);

  std::string m_inputData;
  std::string m_outputData;
  std::string m_detType;
  std::string m_asctLocation;


  bool m_addSpillOverHits;
  double m_minFrac;  
  bool m_oneRef;

 
};

inline std::string STCluster2MCParticleLinker::outputData() const {
  return  m_outputData;
}


#endif // STCLUSTER2MCPARTICLEALG_H







