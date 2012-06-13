#ifndef VELOLITE_HIT_MONITOR_H 
#define VELOLITE_HIT_MONITOR_H 1

#include "GaudiAlg/GaudiTupleAlg.h"
#include "Event/MCHit.h"

/** @class VeloLiteHitMonitor VeloLiteHitMonitor.h
 *  
 */

class DeVeloLite;

class VeloLiteHitMonitor : public GaudiTupleAlg {

public: 
  /// Constructor
  VeloLiteHitMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VeloLiteHitMonitor() {}

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize();      ///< Algorithm finalization

protected:

  StatusCode monitorHits();
  StatusCode monitorPuHits();

private:

  DeVeloLite* m_det;
  LHCb::MCHits* m_hits;
  LHCb::MCHits* m_puHits;
  bool m_printInfo;
  double m_nHits;
  double m_nHits2;
  double m_nPuHits;
  double m_nPuHits2;
  int m_nEvents;

};
#endif 
