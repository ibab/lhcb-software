// $Id: RichGlobalPIDTrTrackSel.h,v 1.3 2003-08-06 10:02:10 jonrob Exp $
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
#include "RichRecBase/IRichExpectedTrackSignal.h"

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

  /// Location of processing status object in TES
  std::string m_procStatLocation;

  /// Location of TrStoredTracks in TES
  std::string m_trTracksLocation;

  IRichExpectedTrackSignal * m_tkSignal; ///< Pointer to RichExpectedTrackSignal
  IRichTrackCreator * m_trackCr;         ///< Pointer to RichTrackCreator
  
  // Selection cuts
  double m_minPhysPtot; ///< Minimum momentum for physics quality tracks
  double m_minLLPtot;   ///< Minimum momentum for use in LL calculation
  bool m_resetToPion;   ///< Reset all track hypotheses to pion, ignoring initial types

  /// Maximum number of TrStoredTracks with RICH information
  int m_maxUsedTracks;

  /// Maximum number of TrStoredTracks
  int m_maxTrTracks;

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
