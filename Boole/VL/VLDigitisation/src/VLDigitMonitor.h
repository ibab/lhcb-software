#ifndef VL_DIGIT_MONITOR_H 
#define VL_DIGIT_MONITOR_H 1

// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
// Event/MCEvent
#include "Event/MCVLDigit.h"
// Det/VLDet
#include "VLDet/DeVL.h"

/** @class VLDigitMonitor VLDigitMonitor.h
 *
 *    
 */

class VLDigitMonitor : public GaudiTupleAlg {

public: 
  /// Standard constructor
  VLDigitMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VLDigitMonitor() {} 

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize();      ///< Algorithm finalization

private:

  void monitor();

  bool m_detailed;
  LHCb::MCVLDigits* m_digits;
  DeVL* m_det;
  double m_nDigits;
  double m_nDigits2;
  long m_nSignal;
  long m_nNoise;
  long m_nOther;
  long m_nEvents;

};
#endif
