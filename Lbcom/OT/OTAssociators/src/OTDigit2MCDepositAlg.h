// $Id: OTDigit2MCDepositAlg.h,v 1.2 2003-07-15 11:31:07 jvantilb Exp $
#ifndef OTASSOCIATORS_OTDIGIT2MCDEPOSITALG_H
#define OTASSOCIATORS_OTDIGIT2MCDEPOSITALG_H 1

#include "GaudiKernel/Algorithm.h"

class OTDigit;
class MCOTDeposit;

/** @class OTDigit2MCDepositAlg OTDigit2MCDepositAlg.h
 *  
 *  Algorithm which makes the association from OTDigits to MCOTDeposits. 
 *  This is used by the associator tool. The relations with deposits from 
 *  spillover are made as well. Since an OTDigit could contain more than 
 *  one tdc-time, an integer is stored in the relation table from OTDigits
 *  to MCOTDeposits which points to the tdc-time.
 *  If other MCOTDeposits on the same channel but killed by dead time, are
 *  recorded within a certain (small) time-window, they can be associated as 
 *  well.
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
                                      std::vector<MCOTDeposit*>& depVector,
                                      std::vector<int>& depNumbers);

  /// path to put table
  std::string outputData() const;
 
protected:

private:

  // job options:
  std::string m_outputData; ///< path to put relation table
  double m_acceptTime;      ///< Time window for deposits killed by dead time

  /// MCOTDeposits container used to find deposits killed by dead time
  MCOTDeposits* m_deposits;

};

inline std::string OTDigit2MCDepositAlg::outputData() const {
  return  m_outputData;
}


#endif // OTASSOCIATORS_OTDIGIT2MCDEPOSITALG_H
