#ifndef TRACKINTERFACES_ITRACKINTERPOLATOR_H 
#define TRACKINTERFACES_ITRACKINTERPOLATOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb {
  class Track;
  class State;
}


/** @class ITrackInterpolator ITrackInterpolator.h TrackExtrapolators/ITrackInterpolator.h
 *  
 *  Interface for track interpolators
 *
 *  @author Eduardo Rodrigues
 *  @date   2006-07-27
 */
class ITrackInterpolator : public extend_interfaces<IAlgTool> {
public: 
  
  DeclareInterfaceID( ITrackInterpolator, 2, 0 );

  /// Interpolate a Track at a given z-position (the track may be re-fitted if needed!)
  virtual StatusCode interpolate( const LHCb::Track& track,
                                  double z,
                                  LHCb::State& state ) const = 0;

};
#endif // TRACKINTERFACES_ITRACKINTERPOLATOR_H
