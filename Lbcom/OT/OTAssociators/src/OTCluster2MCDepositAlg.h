// $Id: OTCluster2MCDepositAlg.h,v 1.1 2002-07-04 11:20:30 hierck Exp $
#ifndef OTCLUSTER2MCDepositALG_H
#define OTCLUSTER2MCDepositALG_H 1

#include <string>
#include <map>


#include "Relations/IAssociator.h" 
#include "Relations/IRelation.h"
#include "Relations/Relation1D.h"

#include "GaudiKernel/Algorithm.h"

class OTCluster;
class MCOTDeposit;

/** @class OTCluster2MCDepositAlg OTCluster2MCDepositAlg.h
 *  
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
                                      MCOTDeposit*& aHit);

  /// path to put table
  std::string outputData() const;
 
protected:

private:

  std::string m_outputData;

};

inline std::string OTCluster2MCDepositAlg::outputData() const {
  return  m_outputData;
}


#endif // OTCLUSTER2MCDepositALG_H







