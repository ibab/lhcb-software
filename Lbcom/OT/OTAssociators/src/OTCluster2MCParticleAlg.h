// $Id: OTCluster2MCParticleAlg.h,v 1.5 2003-01-17 14:07:44 sponce Exp $
#ifndef OTASSOCIATORS_OTCLUSTER2MCPARTICLEALG_H
#define OTASSOCIATORS_OTCLUSTER2MCPARTICLEALG_H 1

#include "Relations/IAssociator.h" 
#include "Relations/IRelation.h"
#include "Relations/Relation1D.h"

#include "GaudiKernel/Algorithm.h"
#include "OTAssociators/OTCluster2MCHitAsct.h"

/** @class OTCluster2MCParticleAlg OTCluster2MCParticleAlg.h
 *  
 *  Algorithm which makes the association from OTClusters to MCParticles. 
 *  This is used by the associator tool. This algorithm uses the associator
 *  with MCHits.
 *
 *  @author Jeroen van Tilburg
 *  @date   14/05/2002
 */

class OTCluster;
class MCParticle;

class OTCluster2MCParticleAlg : public Algorithm {

  friend class AlgFactory<OTCluster2MCParticleAlg>;
  
public:

  /// Standard constructor
  OTCluster2MCParticleAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~OTCluster2MCParticleAlg(); 

  /// initialize
  virtual StatusCode initialize();    

  /// execute
  virtual StatusCode execute();    

  /// finalize
  virtual StatusCode finalize();    

  // associator function
  virtual StatusCode associateToTruth(const OTCluster* aCluster,
                                      std::vector<MCParticle*>& partVector);

  /// path to put table
  std::string outputData() const;
 
protected:

private:

  // job options:
  std::string m_outputData;   ///< path to put relation table
  std::string m_nameAsct;     ///< name of the associator to MCHits

  // pointer to associator
  OTCluster2MCHitAsct::IAsct* m_hAsct; ///< pointer to associator

};

inline std::string OTCluster2MCParticleAlg::outputData() const {
  return  m_outputData;
}


#endif // OTASSOCIATORS_OTCLUSTER2MCPARTICLEALG_H







