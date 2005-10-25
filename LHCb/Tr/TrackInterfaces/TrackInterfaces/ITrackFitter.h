// $Id: ITrackFitter.h,v 1.4 2005-10-25 12:59:30 erodrigu Exp $
#ifndef TRACKINTERFACES_ITRACKFITTER_H 
#define TRACKINTERFACES_ITRACKFITTER_H 1

// Include files
// -------------
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class Track;
class State;
class Measurement;

static const InterfaceID IID_ITrackFitter ( "ITrackFitter", 1, 0 );

/** @class ITrackFitter ITrackFitter.h TrackInterfaces/ITrackFitter.h
 *  
 *  Interface for a track fitting tool.
 *
 *  @author Jose A. Hernando, Eduardo Rodrigues
 *  @date   2005-05-25
 *
 *  @author Rutger van der Eijk  07-04-1999
 *  @author Mattiew Needham 
 */
class ITrackFitter : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackFitter; }

  //! fit a track upstream without a seed state (filter and smooth)
  virtual StatusCode fitUpstream( Track& track ) = 0;

  //! fit a track downstream without a seed state (filter and smooth)
  virtual StatusCode fitDownstream( Track& track ) = 0;

  //! fit the track upstream with a seed state (filter and smooth)
  virtual StatusCode fitUpstream( Track& track, State& seed ) = 0;

  //! fit the track downstream with a seed state (filter and smooth)
  virtual StatusCode fitDownstream( Track& track, State& seed ) = 0;

  //! filter the track (only filter) with a seed state
  virtual StatusCode filter( Track& track, State& seed ) = 0;

  //! filter/update state with this measurement
  virtual StatusCode filter( State& state, Measurement& meas ) = 0;

};
#endif // TRACKINTERFACES_ITRACKFITTER_H
