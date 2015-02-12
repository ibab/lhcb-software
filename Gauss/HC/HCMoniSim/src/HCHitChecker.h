#pragma once

// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

/** @class HCHitChecker HCHitChecker.h
 *
 *  Algorithm run in Gauss for monitoring Herschel MCHits. 
 *
 */

class MCHit;

class HCHitChecker : public GaudiTupleAlg {
public: 
  /// Standard constructor
  HCHitChecker(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~HCHitChecker();

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution

private:

  /// TES location of MCHits
  std::string m_hitsLocation;

};
