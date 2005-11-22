// $Id: TrackVeloTTChecker.h,v 1.1 2005-11-22 16:53:28 erodrigu Exp $
#ifndef TRACKCHECKERS_TRACKVELOTTCHECKER_H 
#define TRACKCHECKERS_TRACKVELOTTCHECKER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

// from XxxAssociators
#include "VeloAssociators/VeloCluster2MCParticleAsct.h"
#include "ITAssociators/ITCluster2MCParticleAsct.h"

class Track;
class DeSTDetector;

/** @class TrackVeloTTChecker TrackVeloTTChecker.h
 *  
 *
 *  @author Eduardo Rodrigues (adaptations to new Track Event Model)
 *  @date   2005-11-04
 *  @author :  Yuehong Xie
 *  @date   :  15/10/2002
 *
 */
class TrackVeloTTChecker : public GaudiTupleAlg {
public: 
  /// Standard constructor
  TrackVeloTTChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackVeloTTChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  MCParticle* VeloTrackMCTruth( Track* track );

  double CenterVeloTr( Track* track );

protected:

private:

  int m_nEvents;     ///< Number of events processed

  DeSTDetector* m_STDet;

  VeloCluster2MCParticleAsct::IAsct* m_VeloAssociator;
  ITCluster2MCParticleAsct::IAsct*   m_ITAssociator;

};
#endif // TRACKCHECKERS_TRACKVELOTTCHECKER_H
