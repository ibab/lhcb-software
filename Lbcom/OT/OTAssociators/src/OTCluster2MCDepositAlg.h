// $Id: OTCluster2MCDepositAlg.h,v 1.6 2003-07-15 11:31:06 jvantilb Exp $
#ifndef OTASSOCIATORS_OTCLUSTER2MCDEPOSITALG_H
#define OTASSOCIATORS_OTCLUSTER2MCDEPOSITALG_H 1

#include "GaudiKernel/Algorithm.h"
#include "OTAssociators/OTDigit2MCDepositAsct.h"

class OTCluster;
class MCOTDeposit;

/** @class OTCluster2MCDepositAlg OTCluster2MCDepositAlg.h
 *  
 *  Algorithm which makes the association from OTClusters to MCOTDeposits. 
 *  This is used by the associator tool. This algorithm uses the associator
 *  from OTDigits to MCOTDeposits: OTDigit2MCDepositAsct. Since an OTDigit 
 *  could contain more than one tdc-time (which gives >1 OTClusters), 
 *  the integer stored in the relation table from OTDigits to MCOTDeposits 
 *  is used to distinguish between the different OTClusters originating from 
 *  the same OTDigit.
 *
 *  @author Jeroen van Tilburg
 *  @date   14/05/2002
 */

class OTCluster2MCDepositAlg : public Algorithm {

  friend class AlgFactory<OTCluster2MCDepositAlg>;
  
public:

  /// Standard constructor
  OTCluster2MCDepositAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~OTCluster2MCDepositAlg(); 

  /// initialize
  virtual StatusCode initialize();    

  /// execute
  virtual StatusCode execute();    

  /// finalize
  virtual StatusCode finalize();    

  // associator function
  virtual StatusCode associateToTruth(const OTCluster* aCluster,
                                      std::vector<MCOTDeposit*>& depVector);

  /// path to put table
  std::string outputData() const;
 
protected:

private:

  // job options:
  std::string m_outputData;              ///< path to put the relation table
  std::string m_nameAsct;                ///< Associator from digits to deposits

  OTDigit2MCDepositAsct::IAsct* m_hAsct; ///< pointer to associator

};

inline std::string OTCluster2MCDepositAlg::outputData() const {
  return  m_outputData;
}


#endif // OTASSOCIATORS_OTCLUSTER2MCDEPOSITALG_H
