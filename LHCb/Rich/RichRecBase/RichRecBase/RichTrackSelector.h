// $Id: RichTrackSelector.h,v 1.3 2004-03-16 13:39:58 jonesc Exp $
#ifndef RICHRECBASE_RICHTRACKSELECTOR_H
#define RICHRECBASE_RICHTRACKSELECTOR_H 1

// from Gaudi
#include "GaudiKernel/GaudiException.h"

// Event model
#include "Event/TrStoredTrack.h"
#include "Event/RichRecTrack.h"

/** @class RichTrackSelector RichTrackSelector.h RichRecBase/RichTrackSelector.h
 *
 *  A utility class for the RICH reconstruction providing TrStoredTrack selection
 *
 *  @author Chris Jones
 *  @date   2003-06-20
 */

class RichTrackSelector {

public:

  /// Standard constructor
  RichTrackSelector();

  ~RichTrackSelector() {} ///< Destructor

  /// Test it the given TrStoredTrack is selected
  bool trackSelected( const TrStoredTrack * trTrack ) const;

  /// Test it the given TrStoredTrack is selected
  bool trackSelected( const RichRecTrack * track ) const;

  /// Returns vector of selected track types
  std::vector<std::string> & selectedTrackTypes() { return m_trNames; }

  /// Returns vector of selected track types
  const std::vector<std::string> & selectedTrackTypes() const { return m_trNames; }

  /// Configure the track selection
  bool configureTrackTypes();

  /// Is a given track type configured to be selected
  bool typeSelected( const Rich::Track::Type track ) const;

private: // private data

  /// Track types to accept
  typedef std::vector<std::string> TrackNames;
  TrackNames m_trNames;

  /// Bit field for track selection
  unsigned long m_trBits;

  /// Unique tracks only ?
  bool m_uniqueTrOnly;

};

inline bool RichTrackSelector::trackSelected( const TrStoredTrack * trTrack ) const
{
  return ( trTrack &&
           !(m_uniqueTrOnly && !trTrack->unique()) &&
           (trTrack->history() & m_trBits) );
}

inline bool RichTrackSelector::trackSelected( const RichRecTrack * track ) const
{
  return ( track &&
           !(m_uniqueTrOnly && !track->trackID().unique()) &&
           (track->trackID().history() & m_trBits) );
}

inline bool RichTrackSelector::typeSelected( const Rich::Track::Type track ) const
{
  return ( m_trNames.end() != std::find( m_trNames.begin(),
                                         m_trNames.end(),
                                         Rich::text(track) ) );
}

#endif // RICHRECBASE_RICHTRACKSELECTOR_H
