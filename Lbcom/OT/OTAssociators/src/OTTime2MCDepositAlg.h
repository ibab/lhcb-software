// $Id: OTTime2MCDepositAlg.h,v 1.2 2004-11-10 12:59:57 jnardull Exp $
#ifndef OTASSOCIATORS_OTTIME2MCDEPOSITALG_H
#define OTASSOCIATORS_OTTIME2MCDEPOSITALG_H 1

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// Event
#include "Event/OTTime.h"
#include "Event/MCOTTime.h"
#include "Event/MCOTDeposit.h"
#include "Event/MCTruth.h"

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

class OTTime;
class MCOTDeposit;

/** @class OTTime2MCDepositAlg OTTime2MCDepositAlg.h
 *  
 *  Algorithm which makes the association from OTTimes to MCOTDeposits. 
 *  This is used by the associator tool. The relations with deposits from 
 *  spillover are made as well.
 *  If other MCOTDeposits on the same channel but killed by dead time, are
 *  recorded within a certain (small) time-window, they can be associated as 
 *  well.
 *
 *  @author J. van Tilburg and Jacopo Nardulli
 *  @date   15/06/2004
 */

class OTTime2MCDepositAlg : public GaudiAlgorithm {

  friend class AlgFactory<OTTime2MCDepositAlg>;
  
public:

  /// Standard constructor
  OTTime2MCDepositAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~OTTime2MCDepositAlg(); 

  /// initialize
  virtual StatusCode initialize();    

  /// execute
  virtual StatusCode execute();    

  /// finalize
  virtual StatusCode finalize();    

  // associator function
  virtual StatusCode associateToTruth(const OTTime* aTime, 
                                      std::vector<MCOTDeposit*>& depVector );

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

inline std::string OTTime2MCDepositAlg::outputData() const {
  return  m_outputData;
}


#endif // OTASSOCIATORS_OTTIME2MCDEPOSITALG_H
