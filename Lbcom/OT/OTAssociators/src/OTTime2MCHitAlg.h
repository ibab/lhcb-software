// $Id: OTTime2MCHitAlg.h,v 1.3 2004-11-23 14:21:17 cattanem Exp $
#ifndef OTASSOCIATORS_OTTIME2MCHITALG_H
#define OTASSOCIATORS_OTTIME2MCHITALG_H 1

// Event
#include "Event/OTTime.h"
#include "Event/MCOTTime.h"
#include "Event/MCOTDeposit.h"
#include "Event/MCHit.h"
#include "Event/MCTruth.h"

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "OTAssociators/OTTime2MCDepositAsct.h"

class OTTime;
class MCHit;

/** @class OTTime2MCHitAlg OTTime2MCHitAlg.h
 *  
 *  Algorithm which makes the association from OTTimes to MCHits. This
 *  is used by the associator tool. There is a flag to make relations with 
 *  spillover hits as well. By default those are not associated.
 *
 *  @author Jacopo Nardulli and J. van Tilburg
 *  @date   15/06/2004
 */

class OTTime2MCHitAlg : public GaudiAlgorithm {

  friend class AlgFactory<OTTime2MCHitAlg>;
  
public:

  /// Standard constructor
  OTTime2MCHitAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~OTTime2MCHitAlg(); 

  /// initialize
  virtual StatusCode initialize();    

  /// execute
  virtual StatusCode execute();    

  /// find the MCHit given a OTTime
  virtual StatusCode associateToTruth(const OTTime* aTime,
                                      std::vector<MCHit*>& hitVector);

  /// path to put relation table
  std::string outputData() const;
 
protected:

private:

  // job options:
  std::string m_outputData; ///< path to put relation table
  bool m_spillOver;         ///< Flag to store relations with spillover
  std::string m_nameAsct;   ///< Associator from OTTimes to MCOTDeposits
  
  MCHits* m_mcHits;         ///< Container of MCHits used to identify spillover
  OTTime2MCDepositAsct::IAsct* m_hAsct; ///< pointer to associator
  
};

inline std::string OTTime2MCHitAlg::outputData() const {
  return  m_outputData;
}


#endif // OTASSOCIATORS_OTTIME2MCHITALG_H
