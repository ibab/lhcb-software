// $Id: OTDigit2MCDepositAlg.h,v 1.1 2003-06-10 09:04:16 jvantilb Exp $
#ifndef OTASSOCIATORS_OTDIGIT2MCDEPOSITALG_H
#define OTASSOCIATORS_OTDIGIT2MCDEPOSITALG_H 1

#include "Relations/IAssociator.h" 
#include "Relations/IRelation.h"
#include "Relations/Relation1D.h"

#include "GaudiKernel/Algorithm.h"

class OTDigit;
class MCOTDeposit;

/** @class OTDigit2MCDepositAlg OTDigit2MCDepositAlg.h
 *  
 *  Algorithm which makes the association from OTDigits to MCOTDeposits. 
 *  This is used by the associator tool. The relations with deposits from 
 *  spillover are made as well.
 *
 *  @author Jeroen van Tilburg
 *  @date   05/06/2003
 */

class OTDigit2MCDepositAlg : public Algorithm {

  friend class AlgFactory<OTDigit2MCDepositAlg>;
  
public:

  /// Standard constructor
  OTDigit2MCDepositAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~OTDigit2MCDepositAlg(); 

  /// initialize
  virtual StatusCode initialize();    

  /// execute
  virtual StatusCode execute();    

  /// finalize
  virtual StatusCode finalize();    

  // associator function
  virtual StatusCode associateToTruth(const OTDigit* aDigit,
                                      std::vector<MCOTDeposit*>& depVector);

  /// path to put table
  std::string outputData() const;
 
protected:

private:

  std::string m_outputData;
  double m_acceptTime;
  MCOTDeposits* m_deposits;

};

inline std::string OTDigit2MCDepositAlg::outputData() const {
  return  m_outputData;
}


#endif // OTASSOCIATORS_OTDIGIT2MCDEPOSITALG_H
