#ifndef BCMHITCHECKER_H
#define BCMHITCHECKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "Event/MCHit.h"

// from Bcm
#include "BcmDet/DeBcmSens.h"
#include "BcmDet/DeBcm.h"

/** @class BcmHitChecker BcmHitChecker.h
 *
 *
 *  @author Tomasz Szumlak & Chris Parkes
 *  @date   2005-12-13
 */

class DeBcm;
class DeBcmSens;

class BcmHitChecker : public GaudiTupleAlg {
public:
  /// Standard constructor
  BcmHitChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BcmHitChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  StatusCode getData();
  bool checkStation(LHCb::MCHit* Hit);
  StatusCode bcmHitMonitor();

private:

  std::string m_bcmDetLocation;
  std::string m_bcmHitsLocation;
  bool m_detailedMonitor;
  DeBcm* m_bcmDet;
  LHCb::MCHits* m_bcmMCHits;

};
#endif
