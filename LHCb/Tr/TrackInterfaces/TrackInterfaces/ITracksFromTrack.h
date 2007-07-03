// $Id: ITracksFromTrack.h,v 1.1 2007-07-03 12:18:01 hernando Exp $
#ifndef TRACKINTERFACES_ITRACKSFROMTRACK_H 
#define TRACKINTERFACES_ITRACKSFROMTRACK_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Event/Track.h"

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
  
  /// Take an RZ Velo track and make 3D Velo track(s)
  virtual StatusCode tracksFromTrack( const LHCb::Track& seed, 
				      std::vector<LHCb::Track*>& tracks ) = 0;


protected:

private:

};
#endif // TRACKINTERFACES_ITRACKSFROMTRACK_H 
