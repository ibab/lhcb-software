// $Id: RichTrackSelector.h,v 1.1 2003-10-13 16:10:54 jonrob Exp $
#ifndef RICHRECBASE_RICHTRACKSELECTOR_H
#define RICHRECBASE_RICHTRACKSELECTOR_H 1

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
  bool trackSelected( TrStoredTrack * trTrack );

  /// Test it the given TrStoredTrack is selected
  bool trackSelected( RichRecTrack * track );

  /// Returns vector of selected track types
  std::vector<std::string> & selectedTrackTypes() { return m_trNames; }

  /// Returns vector of selected track types
  const std::vector<std::string> & selectedTrackTypes() const { return m_trNames; }

  /// Configure the track selection
  bool configureTrackTypes();

private: // private data

  /// Track types to accept
  typedef std::vector<std::string> TrackNames;
  TrackNames m_trNames;

  /// Bit field for track selection
  unsigned long m_trBits;

  /// Unique tracks only ?
  bool m_uniqueTrOnly;

};

inline bool RichTrackSelector::trackSelected( TrStoredTrack * trTrack )
{
  return ( trTrack && 
           !(m_uniqueTrOnly && !trTrack->unique()) &&
           (trTrack->history() & m_trBits) );
}

inline bool RichTrackSelector::trackSelected( RichRecTrack * track )
{
  return ( track && 
           !(m_uniqueTrOnly && !track->trackID().unique()) &&
           (track->trackID().history() & m_trBits) );
}

#endif // RICHRECBASE_RICHTRACKSELECTOR_H
