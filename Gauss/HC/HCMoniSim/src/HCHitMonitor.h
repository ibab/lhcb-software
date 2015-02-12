#pragma once

// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

/** @class HCHitMonitor HCHitMonitor.h
 *
 *  Algorithm run in Gauss for monitoring Herschel MCHits. 
 *
 */

class MCHit;

class HCHitMonitor : public GaudiTupleAlg {
public: 
  /// Standard constructor
  HCHitMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~HCHitMonitor();

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution

private:

  /// TES location of MCHits
  std::string m_hitsLocation;

};
