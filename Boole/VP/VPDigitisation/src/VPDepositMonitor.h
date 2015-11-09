#ifndef VP_DEPOSIT_MONITOR_H 
#define VP_DEPOSIT_MONITOR_H 1

// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
// LHCb
// Event/MCEvent
#include "Event/MCHit.h"
#include "Event/MCVPDigit.h"
// Det/VPDet
#include "VPDet/DeVP.h"

/** @class VPDepositMonitor VPDepositMonitor.h
 *
 *    
 */

class VPDepositMonitor : public GaudiTupleAlg {

public: 
  /// Standard constructor
  VPDepositMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VPDepositMonitor() {} 

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution

private:

  void monitorHits(const LHCb::MCHits*);
  void monitorDigits(const LHCb::MCVPDigits*);

  bool m_detailed;
  DeVP* m_det;

};
#endif
