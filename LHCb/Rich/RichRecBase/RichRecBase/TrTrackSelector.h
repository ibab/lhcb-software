// $Id: TrTrackSelector.h,v 1.2 2003-08-06 09:55:31 jonrob Exp $
#ifndef RICHRECBASE_TRTRACKSELECTOR_H 
#define RICHRECBASE_TRTRACKSELECTOR_H 1

// Include files
#include <iostream>
#include <vector>
#include <string>

// Event model
#include "Event/TrStoredTrack.h"

/** @class TrTrackSelector TrTrackSelector.h RichRecBase/TrTrackSelector.h
 *  
 *  A utility class for the RICH reconstruction providing TrStoredTrack selection
 *
 *  @author Chris Jones
 *  @date   2003-06-20
 */

class TrTrackSelector {

public:

  /// Standard constructor
  TrTrackSelector(); 

  ~TrTrackSelector() {} ///< Destructor

  /// Test it the given TrStoredTrack is selected
  bool trackSelected( TrStoredTrack * trTrack );

  /// Returns vector of selected track types
  std::vector<std::string> & selectedTrackTypes() { return m_trNames; }

  /// Returns vector of selected track types
  const std::vector<std::string> & selectedTrackTypes() const { return m_trNames; }

  /// Configure the track selection
  bool configureTrackTypes();

private: // private data

  /// Track types to accept
  std::vector<std::string> m_trNames;

  /// Bit field for track selection
  unsigned long m_trBits;

  /// Unique tracks only ?
  bool m_uniqueTrOnly;

};

inline bool TrTrackSelector::trackSelected( TrStoredTrack * trTrack ) 
{
  return ( !(m_uniqueTrOnly && !trTrack->unique()) && 
           (trTrack->history() & m_trBits) );
}

#endif // RICHRECBASE_TRTRACKSELECTOR_H
