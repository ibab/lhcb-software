// $Id: RichTrackSelector.h,v 1.5 2004-06-29 19:35:42 jonesc Exp $
#ifndef RICHRECBASE_RICHTRACKSELECTOR_H
#define RICHRECBASE_RICHTRACKSELECTOR_H 1

// from Gaudi
#include "GaudiKernel/GaudiException.h"

// Event model
#include "Event/TrStoredTrack.h"
#include "Event/RichRecTrack.h"

// Kernel
#include "RichKernel/BoostArray.h"

/** @class RichTrackSelector RichTrackSelector.h RichRecBase/RichTrackSelector.h
 *
 *  A utility class for the RICH reconstruction providing track selection
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-06-20
 */

class RichTrackSelector {

public:

  /// Standard constructor
  RichTrackSelector();

  /// Destructor
  ~RichTrackSelector() {}

  /// Returns the minimum track momentum cuts vector (for job options)
  std::vector<double> & minMomenta();

  /// Returns the maximum track momentum cuts vector (for job options)
  std::vector<double> & maxMomenta();

  /// Returns the minimum momentum cut value for given track type
  const double minMomentum( const Rich::Track::Type type ) const;

  /// Returns the maximum momentum cut value for given track type
  const double maxMomentum( const Rich::Track::Type type ) const;

  /// Test it the given TrStoredTrack is selected
  bool trackSelected( const TrStoredTrack * track ) const;

  /// Test it the given RichRecTrack is selected
  bool trackSelected( const RichRecTrack * track ) const;

  /// Returns vector of selected track types (non-const)
  std::vector<std::string> & selectedTrackTypes();

  /// Returns vector of selected track types (const)
  const std::vector<std::string> & selectedTrackTypes() const;

  /// Is a given track type configured to be selected
  bool typeSelected( const Rich::Track::Type track ) const;

  /// Returns selected track types as a single string
  const std::string selectedTracksAsString() const;

  /// Configure the track selection
  bool configureTrackTypes();

private: // methods

  const TrStateP * trStateP( const TrStoredTrack * track,
                             const double zPos = -999999 ) const;

private: // private data

  /// Track types to accept
  typedef std::vector<std::string> TrackNames;
  TrackNames m_trNames;

  /// Unique tracks only ?
  bool m_uniqueTrOnly;

  typedef boost::array<bool,Rich::Track::NTrTypes> TrackTypesSel;
  /// Track types to select
  TrackTypesSel m_tkTypeSel;

  /// Track charge selection -1 = negative only, +1 = positive only, 0 = all tracks
  int m_chargeSel;

  /// Minimum momentum cut
  std::vector<double> m_minP;

  /// Maximum momentum cut
  std::vector<double> m_maxP;

};

inline const TrStateP * RichTrackSelector::trStateP( const TrStoredTrack * track,
                                                     const double zPos ) const
{
  return ( track ?
           dynamic_cast<const TrStateP*>((const TrState*)track->closestState(zPos)) : 0 );
}

inline std::vector<double> & RichTrackSelector::minMomenta()
{
  return m_minP;
}

inline std::vector<double> & RichTrackSelector::maxMomenta()
{
  return m_maxP;
}

inline const double RichTrackSelector::minMomentum( const Rich::Track::Type type ) const
{
  return m_minP[type];
}

inline const double RichTrackSelector::maxMomentum( const Rich::Track::Type type ) const
{
  return m_maxP[type];
}

inline bool RichTrackSelector::trackSelected( const TrStoredTrack * track ) const
{
  const TrStateP * trackPState = trStateP( track );
  const Rich::Track::Type type = Rich::Track::type(track);
  return ( track && trackPState &&                  // Track info OK
           (!m_uniqueTrOnly || track->unique()) &&  // Unique tracks
           m_tkTypeSel[type] &&                     // tracking algorithm type
           ( m_chargeSel*track->charge() >= 0 ) &&  // track charge
           ( trackPState->p()/GeV > minMomentum(type) ) &&  // Momentum cuts
           ( trackPState->p()/GeV < maxMomentum(type) )     // Momentum cuts
           );
}

inline bool RichTrackSelector::trackSelected( const RichRecTrack * track ) const
{
  const Rich::Track::Type type = track->trackID().trackType();
  return ( track &&                                           // Track pointer OK
           (!m_uniqueTrOnly || track->trackID().unique()) &&  // Unique tracks
           m_tkTypeSel[type] &&                               // tracking algorithm type
           ( m_chargeSel*track->charge() >= 0 )  &&           // track charge
           ( track->vertexMomentum() > minMomentum(type) ) &&     // Momentum cut
           ( track->vertexMomentum() < maxMomentum(type) )        // Momentum cut
           );
}

inline std::vector<std::string> & RichTrackSelector::selectedTrackTypes()
{
  return m_trNames;
}

inline const std::vector<std::string> & RichTrackSelector::selectedTrackTypes() const
{
  return m_trNames;
}

inline bool RichTrackSelector::typeSelected( const Rich::Track::Type track ) const
{
  return ( m_trNames.end() != std::find( m_trNames.begin(),
                                         m_trNames.end(),
                                         Rich::text(track) ) );
}

inline const std::string RichTrackSelector::selectedTracksAsString() const
{
  std::vector<std::string>::const_iterator iT = selectedTrackTypes().begin();
  std::string names = *iT; ++iT;
  for ( ; iT != selectedTrackTypes().end(); ++iT ) { names += " " + *iT; }
  return names;
}

#endif // RICHRECBASE_RICHTRACKSELECTOR_H
