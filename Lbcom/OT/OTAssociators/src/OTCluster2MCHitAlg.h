// $Id: OTCluster2MCHitAlg.h,v 1.7 2003-07-15 11:31:07 jvantilb Exp $
#ifndef OTASSOCIATORS_OTCLUSTER2MCHITALG_H
#define OTASSOCIATORS_OTCLUSTER2MCHITALG_H 1

#include "GaudiKernel/Algorithm.h"
#include "OTAssociators/OTDigit2MCHitAsct.h"

class OTCluster;
class MCHit;

/** @class OTCluster2MCHitAlg OTCluster2MCHitAlg.h
 *  
 *  Algorithm which makes the association from OTClusters to MCHits. This
 *  is used by the associator tool. This algorithm uses the associator
 *  from OTDigits to MCHits: OTDigit2MCHitAsct. Since an OTDigit 
 *  could contain more than one tdc-time (which gives >1 OTClusters), 
 *  the integer stored in the relation table from OTDigits to MCHits 
 *  is used to distinguish between the different OTClusters originating from 
 *  the same OTDigit.
 * 
 *  @author Jeroen van Tilburg
 *  @date   14/05/2002
 */

class OTCluster2MCHitAlg : public Algorithm {

  friend class AlgFactory<OTCluster2MCHitAlg>;
  
public:

  /// Standard constructor
  OTCluster2MCHitAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~OTCluster2MCHitAlg(); 

  /// initialize
  virtual StatusCode initialize();    

  /// execute
  virtual StatusCode execute();    

  /// finalize
  virtual StatusCode finalize();    

  /// find the MCHit given a OTCluster
  virtual StatusCode associateToTruth(const OTCluster* aCluster,
                                      std::vector<MCHit*>& hitVector);

  /// path to put relation table
  std::string outputData() const;
 
protected:

private:

  // job options:  
  std::string m_outputData;          ///< path to put relation table
  std::string m_nameAsct;            ///< Associator from OTDigits to MCHits

  OTDigit2MCHitAsct::IAsct* m_hAsct; ///< pointer to associator

};

inline std::string OTCluster2MCHitAlg::outputData() const {
  return  m_outputData;
}


#endif // OTASSOCIATORS_OTCLUSTER2MCHITALG_H
