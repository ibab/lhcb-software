#ifndef VELOCLUSTER2MCParticleALG_H
#define VELOCLUSTER2MCParticleALG_H 1

#include <string>

#include "GaudiAlg/GaudiAlgorithm.h"
#include "VeloAssociators/VeloCluster2MCHitAsct.h"

#include "Event/VeloCluster.h"


/** @class VeloCluster2MCParticleAlg VeloCluster2MCParticleAlg.h
 *  
 *
 *  @author Chris Parkes
 *  @date   21/05/2002
 */
class VeloCluster2MCParticleAlg : public GaudiAlgorithm {

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

  /// path to put table
  std::string outputData() const;

  /// Associate from VeloClusters2MCHits Linker table
  StatusCode associateFromLinker();
  
  /// Associate from VeloClusters2MCHits Relations table
  StatusCode associateFromRelations();
 
protected:

private:

  VeloCluster2MCHitAsct::IAsct *m_pV2MCHit;
  std::string m_outputData;
  VeloClusters* m_clusters; ///< Pointer to VeloClusters container

};

inline std::string VeloCluster2MCParticleAlg::outputData() const {
  return  m_outputData;
}


#endif // VELOCLUSTER2MCParticleALG_H
