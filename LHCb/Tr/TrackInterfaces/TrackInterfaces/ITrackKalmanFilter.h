// $Id: ITrackKalmanFilter.h,v 1.2 2009-07-13 15:43:04 cattanem Exp $
#ifndef TRACKINTERFACES_ITRACKKALMANFILTER_H 
#define TRACKINTERFACES_ITRACKKALMANFILTER_H 

// Include files
// -------------

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb {
 class Track;
};

static const InterfaceID IID_ITrackKalmanFilter ( "ITrackKalmanFilter", 1, 0 );

/** @class ITrackKalmanFilter ITrackKalmanFilter.h TrackInterfaces/ITrackKalmanFilter.h
 *  
 *  Interface for a track fitting tool.
 *
 *  @author Jose A. Hernando, Eduardo Rodrigues
 *  @date   2005-05-25
 *
 *  @author Rutger van der Eijk  07-04-1999
 *  @author Mattiew Needham 
 */
class ITrackKalmanFilter : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackKalmanFilter; }

  //! fit a track 
  virtual StatusCode fit( LHCb::Track& track ) const = 0;

};
#endif // TRACKINTERFACES_ITRACKFITTER_H
