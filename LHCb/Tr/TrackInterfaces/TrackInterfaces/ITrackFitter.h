// $Id: ITrackFitter.h,v 1.1 2005-05-25 09:08:39 hernando Exp $
#ifndef TRACKINTERFACES_ITRACKFITTER_H 
#define TRACKINTERFACES_ITRACKFITTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class Track;
class State;

static const InterfaceID IID_ITrackFitter ( "ITrackFitter", 1, 0 );

/** @class ITrackFitter ITrackFitter.h TrackInterfaces/ITrackFitter.h
 *  
 *
 *  @author Jose A. Hernando 
 *  @date   2005-05-25
 *
 *  @author Rutger van der Eijk  07-04-1999
 *  @author Mattiew Needham 
 */
class ITrackFitter : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackFitter; }

  //! fit a track without a seed state
  StatusCode fit(Track& track) = 0;

  //! fit the track with a seed state
  StatusCode fit(Track& track, const State& seed) = 0;
  
  //! filter the track (only filter)
  StatusCode filter(Track& track, const State& seed) = 0;
  
  //! filter/update state with this measurement
  StatusCode filter(State& state, Measurement& meas) = 0;

};
#endif // TRACKINTERFACES_ITRACKFITTER_H
