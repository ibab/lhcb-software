// $Id: OTDigit2MCHitAlg.h,v 1.2 2003-07-15 11:31:07 jvantilb Exp $
#ifndef OTASSOCIATORS_OTDIGIT2MCHITALG_H
#define OTASSOCIATORS_OTDIGIT2MCHITALG_H 1

#include "GaudiKernel/Algorithm.h"
#include "OTAssociators/OTDigit2MCDepositAsct.h"

class OTDigit;
class MCHit;

/** @class OTDigit2MCHitAlg OTDigit2MCHitAlg.h
 *  
 *  Algorithm which makes the association from OTDigits to MCHits. This
 *  is used by the associator tool. There is a flag to make relations with 
 *  spillover hits as well. By default those are not associated.
 *  Since an OTDigit could contain more than one tdc-time, an integer is 
 *  stored in the relation table from OTDigits to MCHits which points
 *  to the tdc-time.
 *
 *  @author Jeroen van Tilburg
 *  @date   05/06/2003
 */

class OTDigit2MCHitAlg : public Algorithm {

  friend class AlgFactory<OTDigit2MCHitAlg>;
  
public:

  /// Standard constructor
  OTDigit2MCHitAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~OTDigit2MCHitAlg(); 

  /// initialize
  virtual StatusCode initialize();    

  /// execute
  virtual StatusCode execute();    

  /// finalize
  virtual StatusCode finalize();    

  /// find the MCHit given a OTDigit
  virtual StatusCode associateToTruth(const OTDigit* aDigit,
                                      std::vector<MCHit*>& hitVector,
                                      std::vector<int>& hitNumbers);

  /// path to put relation table
  std::string outputData() const;
 
protected:

private:

  // job options:
  std::string m_outputData; ///< path to put relation table
  bool m_spillOver;         ///< Flag to store relations with spillover
  std::string m_nameAsct;   ///< Associator from OTDigits to MCOTDeposits

  MCHits* m_mcHits;         ///< Container of MCHits used to identify spillover
  OTDigit2MCDepositAsct::IAsct* m_hAsct; ///< pointer to associator

};

inline std::string OTDigit2MCHitAlg::outputData() const {
  return  m_outputData;
}


#endif // OTASSOCIATORS_OTDIGIT2MCHITALG_H
