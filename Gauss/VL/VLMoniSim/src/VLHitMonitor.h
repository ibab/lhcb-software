#ifndef VL_HIT_MONITOR_H 
#define VL_HIT_MONITOR_H 1

#include "GaudiAlg/GaudiTupleAlg.h"
#include "Event/MCHit.h"

/** @class VLHitMonitor VLHitMonitor.h
 *  
 */

class DeVL;

class VLHitMonitor : public GaudiTupleAlg {

public: 
  /// Constructor
  VLHitMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VLHitMonitor() {}

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize();      ///< Algorithm finalization

protected:

  void monitor();

private:

  DeVL* m_det;
  LHCb::MCHits* m_hits;
  bool m_detailed;
  bool m_printInfo;
  double m_nHits;
  double m_nHits2;
  int m_nEvents;

};
#endif 
