// $Id: ITrackStateprovider.h,v 1.20 2010-04-07 20:58:29 wouter Exp $
#ifndef TRACKINTERFACES_ITRACKSTATEPROVIDER_H
#define TRACKINTERFACES_ITRACKSTATEPROVIDER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/TrackParameters.h"

// Forward declarations
namespace LHCb {
 class Track;
 class State;
 class TrackTraj;
}

static const InterfaceID IID_ITrackStateProvider( "ITrackStateProvider" , 3, 0 );

/** @class ITrackStateProvider ITrackStateProvider.h
 *
 *  Interface for track stateprovider. TrackStateProvider provides the
 *  state of a track at a certain z position using several methods. To
 *  make this reasonably fast for use in DaVinci it caches every state
 *  it has ever computed for a given track.
 *
 *  @author Wouter Hulsbergen
 *  @date 16/08/2010
 */

class ITrackStateProvider: virtual public IAlgTool 
{

public:

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackStateProvider; }

  /// Compute the state of the track at position z.  The third
  /// argument is the tolerance: if an existing state is found within
  /// a z-distance 'tolerance', that state is returned.
  virtual StatusCode state(  LHCb::State& state,
			     const LHCb::Track& track, 
			     double z,
			     double ztolerance = TrackParameters::propagationTolerance ) const = 0 ;
  
  /// Compute the state of the track at a certain z position by using
  /// the trajectory approximation.
  virtual StatusCode stateFromTrajectory( LHCb::State& state,
					  const LHCb::Track& track, 
					  double z ) const = 0 ;
  
  /// Return the trajectory approximation of this track. Ownership
  /// stays with the tool. A zero pointer indicates an error.
  virtual const LHCb::TrackTraj* trajectory( const LHCb::Track& track ) const = 0 ;

};
#endif // TRACKINTERFACES_ITRACKSTATEPROVIDER_H
