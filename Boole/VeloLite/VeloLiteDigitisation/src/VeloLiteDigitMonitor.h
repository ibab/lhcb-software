#ifndef VELOLITE_DIGIT_MONITOR_H 
#define VELOLITE_DIGIT_MONITOR_H 1

// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
// Event/MCEvent
#include "Event/MCVeloLiteDigit.h"
// Det/VeloLiteDet
#include "VeloLiteDet/DeVeloLite.h"

/** @class VeloLiteDigitMonitor VeloLiteDigitMonitor.h
 *  
 */

class VeloLiteDigitMonitor : public GaudiTupleAlg {

public: 
  /// Standard constructor
  VeloLiteDigitMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VeloLiteDigitMonitor(); 

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize();      ///< Algorithm finalization

private:

  void monitor();

  bool m_printInfo;
  bool m_detailed;
  LHCb::MCVeloLiteDigits* m_digits;
  DeVeloLite* m_det;
  double m_nDigits;
  double m_nDigits2;
  long m_nSignal;
  long m_nNoise;
  long m_nOther;
  long m_nEvents;

};
#endif
