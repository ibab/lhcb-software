// $Id: OTCluster2MCParticleAlg.h,v 1.6 2003-07-15 11:31:07 jvantilb Exp $
#ifndef OTASSOCIATORS_OTCLUSTER2MCPARTICLEALG_H
#define OTASSOCIATORS_OTCLUSTER2MCPARTICLEALG_H 1

#include "GaudiKernel/Algorithm.h"
#include "OTAssociators/OTDigit2MCParticleAsct.h"

class OTCluster;
class MCParticle;

/** @class OTCluster2MCParticleAlg OTCluster2MCParticleAlg.h
 *  
 *  Algorithm which makes the association from OTClusters to MCParticles. 
 *  This is used by the associator tool. This algorithm uses the associator
 *  from OTDigits to MCParticles: OTDigit2MCParticleAsct. Since an OTDigit 
 *  could contain more than one tdc-time (which gives >1 OTClusters), 
 *  the integer stored in the relation table from OTDigits to MCParticles 
 *  is used to distinguish between the different OTClusters originating from 
 *  the same OTDigit.
 *
 *  @author Jeroen van Tilburg
 *  @date   14/05/2002
 */

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
  std::string m_outputData;               ///< path to put relation table
  std::string m_nameAsct;                 ///< Asct. from OTDigits 2 MCParticles

  OTDigit2MCParticleAsct::IAsct* m_hAsct; ///< pointer to associator

};

inline std::string OTCluster2MCParticleAlg::outputData() const {
  return  m_outputData;
}


#endif // OTASSOCIATORS_OTCLUSTER2MCPARTICLEALG_H
