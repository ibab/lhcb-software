
//--------------------------------------------------------------------------
/** @file RichGlobalPIDAlg.h
 *
 *  Header file for RICH Global PID algorithm class : RichGlobalPIDAlg
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDAlg.h,v 1.14 2006-09-07 16:59:27 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */
//--------------------------------------------------------------------------

#ifndef RICHRECALGS_RICHGLOBALPIDALG_H
#define RICHRECALGS_RICHGLOBALPIDALG_H 1

// STD
#include <sstream>

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
 *  Note : This algorithm works with -(Log-likelihood), and thus attempts to
 *         minimise the negative log likelihoods
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 *
 *  @todo Update algorithm to work with "normal" positive likelihoods
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

private: // helper classes

  // private typedefs

  /// Container for changes to be made following an event iterations
  /// Contains a pointer to a track and the its new hypothesis
  typedef Rich::Map<LHCb::RichGlobalPIDTrack*,Rich::ParticleIDType> MinTrList;

  /// Track list entry. Its current best DLL change and a pointer to the trackk
  typedef std::pair<double,LHCb::RichGlobalPIDTrack*> TrackPair;

  /// List of all track list entries
  typedef std::vector<TrackPair> TrackList;

  class InitTrackInfo
  {
  public:
    /// Container
    typedef std::vector<InitTrackInfo> Vector;
    /// Constructor
    InitTrackInfo( LHCb::RichGlobalPIDTrack * track = NULL,
                   const Rich::ParticleIDType h = Rich::Pion,
                   const double mindll = 0 ) 
      : pidTrack(track), hypo(h), minDLL(mindll) { }
  public:
    LHCb::RichGlobalPIDTrack * pidTrack; ///< Pointer to the track
    Rich::ParticleIDType hypo;     ///< Track hypothesis
    double minDLL;                 ///< The DLL value
  };

private: // Private methods


  /** Starting with all tracks pion, calculate logLikelihood. Then for
   * each track in turn, holding all others to pion, calculate new
   * logLikelihood for each particle code. If less than with all pion,
   * set new minimum track hypothesis.
   * @return Number of tracks that changed mass hypothesis
   */
  unsigned int initBestLogLikelihood();

  /** Starting with all tracks with best hypotheses as set by
   * initBestLogLikelihood(), for each track in turn get
   * logLikelihood for each particle code, and return the track and
   * particle code which gave the optimal log likelihood. 
   * @return Number of tracks that changed mass hypothesis
   */
  void findBestLogLikelihood( MinTrList & minTracks );

  /// Do the event iterations
  unsigned int doIterations();

  /// Calculates logLikelihood for event with a given set of track hypotheses.
  /// Performs full loop over all tracks and hypotheses
  double logLikelihood();

  /// Computes the change in the logLikelihood produced by changing given
  /// track to the given hypothesis
  double deltaLogLikelihood( LHCb::RichRecTrack * track,
                             const Rich::ParticleIDType newHypo );

  /// Returns log( exp(signal) - 1 ) or an approximation for small signals
  double sigFunc( const double s ) const;

  /// Returns the freeze out Dll value
  double freezeOutDll() const;

  /// Returns the force change Dll value
  double forceChangeDll() const;

  /// Sets flags to say if the given set of tracks are in Rich1 and/or Rich2
  void updateRichFlags( const MinTrList & minTracks );

private:  // Private data members

  // tool pointers
  const IRichExpectedTrackSignal * m_tkSignal; ///< Track signal tool
  const IRichPhotonSignal * m_photonSig;       ///< Photon signal tool

  /// Track DLL value to freeze track out from future iterations
  double m_freezeOutDll;

  /// Track DLL value for a forced change
  double m_forceChangeDll;

  /// Threshold for likelihood maximisation
  double m_epsilon;

  /// Maximum number of track iterations
  unsigned int m_maxEventIterations;

  /// Minimum signal value for full calculation of log(exp(signal)-1)
  double m_minSig;

  /// Signal value below which to approximate log(exp(signal)-1) with log(signal)
  double m_apxSig;

  /// Cached value of log(exp(m_minSig) - 1) for efficiency
  double m_logMinSig;

  /// List of tracks ordered by change in likelihood
  TrackList m_trackList;

  /// Current best event log likelihood value
  double m_currentBestLL;

  /// Flag to turn on final DLL and hypothesis check
  bool m_doFinalDllCheck;

  /// Flag to turn on RICH check in LL minimisation
  bool m_richCheck;

  /// Maximum number of tracks to change in a single event iteration
  unsigned int m_maxTkChanges;

private:
  
  // working flags and variables
  bool m_inR1, m_inR2;

};

inline double RichGlobalPIDAlg::forceChangeDll() const
{
  return m_forceChangeDll;
}

inline double RichGlobalPIDAlg::sigFunc( const double s ) const
{
  return ( s>m_apxSig ? log(exp(s)-1.) : ( s>m_minSig ? log(s) : m_logMinSig ) );
}

inline double RichGlobalPIDAlg::freezeOutDll() const
{
  return m_freezeOutDll;
}

#endif // RICHRECALGS_RICHGLOBALPIDALG_H
