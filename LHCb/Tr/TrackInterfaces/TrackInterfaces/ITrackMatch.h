// $Id: ITrackMatch.h,v 1.1 2008-05-22 07:43:41 cattanem Exp $
#ifndef TRACKINTERFACES_ITRACKMATCH_H 
#define TRACKINTERFACES_ITRACKMATCH_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

//forward declarations
namespace LHCb {
  class Track;
};

static const InterfaceID IID_ITrackMatch ( "ITrackMatch", 1, 0 );

/** @class ITrackMatch ITrackMatch.h TrackInterfaces/ITrackMatch.h
 *  
 *
 *  @author Jose A. Hernando
 *  @date   2007-06-16
 */
class ITrackMatch : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackMatch; }
  
  virtual StatusCode match(const LHCb::Track& track0, 
                           const LHCb::Track& track1,
                           LHCb::Track& matchTrack,
                           double& quality,
                           double& quality2) = 0;
  
};
#endif // TRACKINTERFACES_IMATCHTVELOTRACKS_H
