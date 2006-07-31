// $Id: ITrackInterpolator.h,v 1.1 2006-07-31 17:10:27 erodrigu Exp $
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
};

static const InterfaceID IID_ITrackInterpolator ( "ITrackInterpolator", 1, 0 );

/** @class ITrackInterpolator ITrackInterpolator.h TrackExtrapolators/ITrackInterpolator.h
 *  
 *  Interface for track interpolators
 *
 *  @author Eduardo Rodrigues
 *  @date   2006-07-27
 */
class ITrackInterpolator : virtual public IAlgTool {
public: 
  
  /// Interpolate a Track at a given z-position (the track may be re-fitted if needed!)
  virtual StatusCode interpolate( LHCb::Track& track,
                                  double z,
                                  LHCb::State& state ) = 0;
  
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackInterpolator; }

};
#endif // TRACKINTERFACES_ITRACKINTERPOLATOR_H
