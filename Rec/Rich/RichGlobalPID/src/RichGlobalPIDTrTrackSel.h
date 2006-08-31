
//--------------------------------------------------------------------------
/** @file RichGlobalPIDTrTrackSel.h
 *
 *  Header file for RICH Global PID algorithm class : RichGlobalPIDTrTrackSel
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDTrTrackSel.h,v 1.16 2006-08-31 12:11:38 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */
//--------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RICHGLOBALPIDTRTRACKSEL_H
#define RICHGLOBALPID_RICHGLOBALPIDTRTRACKSEL_H 1

// Base class
#include "RichGlobalPIDAlgBase.h"

// base definitions
#include "RichRecBase/RichRecProcCode.h"

// Event
#include "Event/ProcStatus.h"
#include "Event/RichRecStatus.h"

// interfaces
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichTrackSelector.h"

//--------------------------------------------------------------------------
/** @class RichGlobalPIDTrTrackSel RichGlobalPIDTrTrackSel.h
 *
 *  Track selection algorithm for Rich Global PID
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 *
 *  @todo Find a way to avoid the dynamic_casts when setting Track in RichPID
 */
//--------------------------------------------------------------------------

class RichGlobalPIDTrTrackSel : public RichGlobalPIDAlgBase 
{

public:

  /// Standard constructor
  RichGlobalPIDTrTrackSel( const std::string& name,
                           ISvcLocator* pSvcLocator );

  virtual ~RichGlobalPIDTrTrackSel(); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

  // Private methods
private:

  /// Clean up after event abortion
  void deleteEvent();

  /// Determine the global PID Status of a track.
  /// Determines how the track will be used in the global likelihood
  Rich::GlobalPID::TkQuality trackStatus( LHCb::RichRecTrack * track );

  // Private data members
private:

  /// Location of processing status object in TES
  std::string m_procStatLocation;

  const IRichExpectedTrackSignal * m_tkSignal; ///< Pointer to RichExpectedTrackSignal

  /// Track selector
  const Rich::IRichTrackSelector * m_trSelector;

  // Selection cuts
  double m_minPhysPtot; ///< Minimum momentum for physics quality tracks
  double m_minLLPtot;   ///< Minimum momentum for use in LL calculation
  bool m_resetToPion;   ///< Reset all track hypotheses to pion, ignoring initial types

  /// Maximum number of Tracks with RICH information
  int m_maxUsedTracks;

  /// Maximum total number of input TrStoredTracks
  int m_maxInputTracks;

};

inline void RichGlobalPIDTrTrackSel::deleteEvent()
{
  // Tidy up incase of event abort
  if ( m_GPIDtracks && !m_GPIDtracks->empty() ) m_GPIDtracks->clear();
  if ( m_GPIDs      && !m_GPIDs->empty()      ) m_GPIDs->clear();
}

#endif // RICHGLOBALPID_RICHGLOBALPIDTRTRACKSEL_H
