// $Id: RichGlobalPIDTrTrackSel.h,v 1.1.1.1 2003-06-30 16:10:55 jonesc Exp $
#ifndef RICHRECALGS_RICHGLOBALPIDTRTRACKSEL_H
#define RICHRECALGS_RICHGLOBALPIDTRTRACKSEL_H 1

// Base
#include "RichGlobalPIDAlgBase.h"
#include "RichRecBase/RichRecProcCode.h"
#include "RichRecBase/TrTrackSelector.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/TrStoredTrack.h"
#include "Event/TrStateP.h"
#include "Event/ProcStatus.h"
#include "Event/RichRecStatus.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// interfaces
#include "RichRecBase/IRichTrackCreator.h"
#include "RichRecBase/IRichTrackProperties.h"

// RichKernel
#include "RichKernel/MessageSvcStl.h"

/** @class RichGlobalPIDTrTrackSel RichGlobalPIDTrTrackSel.h
 *
 *  TrStoredTrack selection algorithm for Rich Global PID
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */

class RichGlobalPIDTrTrackSel : public RichGlobalPIDAlgBase {

public:

  /// Standard constructor
  RichGlobalPIDTrTrackSel( const std::string& name,
                           ISvcLocator* pSvcLocator );

  virtual ~RichGlobalPIDTrTrackSel(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  // Private methods
private:

  /// Clean up after event abortion
  void deleteEvent();

  /// Determine the global PID Status of a track.
  /// Determines how the track will be used in the global likelihood
  Rich::GlobalPID::TkQuality trackStatus( TrStoredTrack * track );

  // Private data members
private: 

  // tool pointers
  IRichTrackCreator * m_trackCr;
  IRichTrackProperties * m_trackProp;
  
  // Selection cuts
  double m_minPhysPtot; ///< Minimum momentum for physics quality tracks
  double m_minLLPtot;   ///< Minimum momentum for use in LL calculation
  bool m_resetToPion;   ///< Reset all track hypotheses to pion, ignoring initial types

  /// Maximum number of usable tracks
  int m_maxUsedTracks;

  /// TrStoredTrack selector
  TrTrackSelector m_trSelector;

  // Track type tallys
  int m_nVeloTk[2];
  int m_nNonUniqueTk[2];
  int m_nSeedTk[2];
  int m_nMatchTk[2];
  int m_nForwardTk[2];
  int m_nUpstreamTk[2];
  int m_nVeloTTTk[2];
  int m_nVeloBackTk[2];

};

#endif // RICHRECALGS_RICHGLOBALPIDTRTRACKSEL_H
