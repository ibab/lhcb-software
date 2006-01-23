
//--------------------------------------------------------------------------
/** @file RichGlobalPIDAlg.h
 *
 *  Header file for RICH Global PID algorithm class : RichGlobalPIDAlg
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDAlg.h,v 1.11 2006-01-23 13:42:16 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */
//--------------------------------------------------------------------------

#ifndef RICHRECALGS_RICHGLOBALPIDALG_H
#define RICHRECALGS_RICHGLOBALPIDALG_H 1

// base class
#include "RichGlobalPIDAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// Event
#include "Event/RichRecStatus.h"

// RichKernel
#include "RichKernel/RichMap.h"

// interfaces
#include "RichRecBase/IRichPhotonSignal.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"

//--------------------------------------------------------------------------
/** @class RichGlobalPIDAlg RichGlobalPIDAlg.h
 *
 *  Performs a single iteration of the global PID overall event
 *  likelihood minimisation.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */
//--------------------------------------------------------------------------

class RichGlobalPIDAlg : public RichGlobalPIDAlgBase
{

public:

  /// Standard constructor
  RichGlobalPIDAlg( const std::string& name,
                    ISvcLocator* pSvcLocator );

  virtual ~RichGlobalPIDAlg();        ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // Private methods

  // private typedefs
  typedef Rich::Map<LHCb::RichGlobalPIDTrack*,Rich::ParticleIDType> minTrList;
  typedef std::pair<double,LHCb::RichGlobalPIDTrack*> TrackPair;
  typedef std::vector<TrackPair> TrackList;

  /// Starting with all tracks pion, calculate logLikelihood. Then for
  /// each track in turn, holding all others to pion, calculate new
  /// logLikelihood for each particle code. If less than with all pion,
  /// set new minimum track hypothesis.
  StatusCode initMinLogLikelihood();

  /// Starting with all tracks with minimum hypothesis as set by
  /// initMinLogLikelihood(), for each track in turn get
  /// logLikelihood for each particle code, and return the track and
  /// particle code which gave the lowest minumum.
  void findMinLogLikelihood( minTrList & minTracks );

  /// Calculates logLikelihood for event with a given set of track hypotheses.
  /// Performs full loop over all tracks and hypotheses
  double logLikelihood();

  /// Computes the change in the logLikelihood produced by changing given
  /// track to the given hypothesis
  double deltaLogLikelihood( LHCb::RichRecTrack * track,
                             const Rich::ParticleIDType newHypo );

  /// Returns log( exp(signal) - 1 ) or an approximation for small signals
  double sigFunc( double s );

  // Returns current freeze out value
  double freezeOutValue();

  // Update the current freeze out value
  void updateFreezeOutValue();

  // Returns current freeze out value
  double dllThres();

  // Update the current freeze out value
  void updateDllThres();

private:  // Private data members

  // tool pointers
  const IRichExpectedTrackSignal * m_tkSignal;
  const IRichPhotonSignal * m_photonSig;

  /// Threshold for likelihood maximisation
  double m_epsilon;

  /// Maximum number of track iterations
  int m_maxTrackIterations;

  /// Threshold for forced hypothesis change in track minimsation
  std::vector<double> m_cP;

  // parameters for freeze out function
  std::vector<double> m_fP;

  /// Current minimum hypothesis
  double m_currentBestLL;

  /// Minimum signal value for full calculation of log(exp(signal)-1)
  double m_minSig;

  /// Signal value below which to approximate log(exp(signal)-1) with log(signal)
  double m_apxSig;

  /// Cached value of log(exp(m_minSig) - 1) for efficiency
  double m_logMinSig;

  /// List of tracks ordered by change in likelihood
  TrackList m_trackList;

  // working flags and variables
  bool m_tryAgain, m_lastChance, m_inR1, m_inR2;
  int m_trackIteration;
  double m_currentFreezeOutValue;
  double m_currentDllTreshold;

};

inline double RichGlobalPIDAlg::dllThres()
{
  return m_currentDllTreshold;
}

inline double RichGlobalPIDAlg::sigFunc( double s )
{
  return ( s>m_apxSig ? log(exp(s)-1.) : ( s>m_minSig ? log(s) : m_logMinSig ) );
}

inline double RichGlobalPIDAlg::freezeOutValue()
{
  return m_currentFreezeOutValue;
}

#endif // RICHRECALGS_RICHGLOBALPIDALG_H
