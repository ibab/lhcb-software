// $Id: ITracksFromTrack.h,v 1.3 2007-07-05 14:05:17 cattanem Exp $
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

static const InterfaceID IID_ITracksFromTrack ( "ITracksFromTrack", 1, 0 );

/** @class ITracksFromTrack ITracksFromTrack.h TrackInterfaces/ITracksFromTrack.h
 *  Interface to the forward pattern tool
 *
 *  @author David Hutchcroft
 *  @date   2007-05-24
 */
class ITracksFromTrack : virtual public IAlgTool {
public: 

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITracksFromTrack; }
  
  /// Take an existing track and make new tracks from it (usually with hits from more detectors)
  virtual StatusCode tracksFromTrack( const LHCb::Track& seed, 
				      std::vector<LHCb::Track*>& tracks ) = 0;


protected:

private:

};
#endif // TRACKINTERFACES_ITRACKSFROMTRACK_H 
