#ifndef VELOCLUSTER2MCParticleALG_H
#define VELOCLUSTER2MCParticleALG_H 1

#include <string>
#include <map>

#include "GaudiKernel/Algorithm.h"
#include "VeloAssociators/VeloCluster2MCParticleAsct.h"
#include "VeloAssociators/VeloCluster2MCHitAsct.h"

/** @class VeloCluster2MCParticleAlg VeloCluster2MCParticleAlg.h
 *  
 *
 *  @author Chris Parkes
 *  @date   21/05/2002
 */
class VeloCluster2MCParticleAlg : public Algorithm {

  friend class AlgFactory<VeloCluster2MCParticleAlg>;
  
public:

  /// Standard constructor
  VeloCluster2MCParticleAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~VeloCluster2MCParticleAlg(); 

  /// initialize
  virtual StatusCode initialize();    

  /// execute
  virtual StatusCode execute();    

  /// finalize
  virtual StatusCode finalize();    

  /// path to put table
  std::string outputData() const;
 
protected:

private:

  VeloCluster2MCHitAsct::IAsct *m_pV2MCHit;

  std::string m_outputData;

};

inline std::string VeloCluster2MCParticleAlg::outputData() const {
  return  m_outputData;
}


#endif // VELOCLUSTER2MCParticleALG_H







