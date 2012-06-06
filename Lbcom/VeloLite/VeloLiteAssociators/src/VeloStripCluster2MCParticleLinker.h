#ifndef VELOSTRIPCLUSTER2MCPARTICLELINKER_H 
#define VELOSTRIPCLUSTER2MCPARTICLELINKER_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

/** @class VeloStripCluster2MCParticleLinker VeloStripCluster2MCParticleLinker.h
 *  
 */

class VeloStripCluster2MCParticleLinker : public GaudiAlgorithm {
public: 
  /// constructor
  VeloStripCluster2MCParticleLinker(const std::string& name, 
                                    ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VeloStripCluster2MCParticleLinker() {}

  virtual StatusCode execute();    ///< Algorithm execution

protected:

  std::string asctName() {return m_asctName;}
  
private:

  std::string m_asctName;
  std::string m_inputClusters;
  std::string m_inputParticles;

};
#endif
