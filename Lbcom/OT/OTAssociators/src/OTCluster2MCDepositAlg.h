// $Id: OTCluster2MCDepositAlg.h,v 1.4 2002-10-17 08:38:23 jvantilb Exp $
#ifndef OTASSOCIATORS_OTCLUSTER2MCDEPOSITALG_H
#define OTASSOCIATORS_OTCLUSTER2MCDEPOSITALG_H 1

#include "Relations/IAssociator.h" 
#include "Relations/IRelation.h"
#include "Relations/Relation1D.h"

#include "GaudiKernel/Algorithm.h"

class OTCluster;
class MCOTDeposit;

/** @class OTCluster2MCDepositAlg OTCluster2MCDepositAlg.h
 *  
 *  Algorithm which makes the association from OTClusters to MCOTDeposits. 
 *  This is used by the associator tool. The relations with deposits from 
 *  spillover are made as well.
 *
 *  @author Jeroen van Tilburg
 *  @date   14/05/2002
 */

class OTCluster2MCDepositAlg : public Algorithm {
    friend AlgFactory<OTCluster2MCDepositAlg>;
  
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

  std::string m_outputData;
  double m_acceptTime;
  MCOTDeposits* m_deposits;

};

inline std::string OTCluster2MCDepositAlg::outputData() const {
  return  m_outputData;
}


#endif // OTASSOCIATORS_OTCLUSTER2MCDEPOSITALG_H
