// $Id: OTCluster2MCParticleAlg.h,v 1.1.1.1 2002-05-26 22:48:42 jvantilb Exp $
#ifndef OTCLUSTER2MCPARTICLEALG_H
#define OTCLUSTER2MCPARTICLEALG_H 1

#include <string>
#include <map>

#include "Relations/IAssociator.h" 
#include "Relations/IRelation.h"
#include "Relations/Relation1D.h"

#include "GaudiKernel/Algorithm.h"
#include "OTAssociators/OTCluster2MCHitAsct.h"

/** @class OTCluster2MCParticleAlg OTCluster2MCParticleAlg.h
 *  
 *
 *  @author Jeroen van Tilburg
 *  @date   14/05/2002
 */

class OTCluster;
class MCParticle;

class OTCluster2MCParticleAlg : public Algorithm {
    friend AlgFactory<OTCluster2MCParticleAlg>;
  
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
                                      MCParticle*& aParticle);

  /// path to put table
  std::string outputData() const;
 
protected:

private:

  std::string m_outputData;
  std::string m_nameAsct;

  // pointer to associator
  OTCluster2MCHitAsct::IAsct* m_hAsct;

};

inline std::string OTCluster2MCParticleAlg::outputData() const {
  return  m_outputData;
}


#endif // OTCLUSTER2MCPARTICLEALG_H







