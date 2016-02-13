#ifndef TRACKINTERFACES_ITRACKSFROMTRACK_H 
#define TRACKINTERFACES_ITRACKSFROMTRACK_H 1

// Include files
// from STL
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb {
 class Track;
}


/** @class ITracksFromTrack ITracksFromTrack.h TrackInterfaces/ITracksFromTrack.h
 *  Interface to the forward pattern tool
 *
 *  @author David Hutchcroft
 *  @date   2007-05-24
 */
class ITracksFromTrack : public extend_interfaces<IAlgTool> {
public: 

  DeclareInterfaceID( ITracksFromTrack, 2, 0 );
  
  /// Take an existing track and make new tracks from it (usually with hits from more detectors)
  virtual StatusCode tracksFromTrack( const LHCb::Track& seed, 
                                      std::vector<LHCb::Track*>& tracks ) const = 0;


};
#endif // TRACKINTERFACES_ITRACKSFROMTRACK_H 
