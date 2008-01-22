// $Id: ITrackMatch.h,v 1.3 2008-01-22 09:35:00 hernando Exp $
#ifndef HLTBASE_ITRACKMATCH_H 
#define HLTBASE_ITRACKMATCH_H 1

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

/** @class ITrackMatch
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
#endif // HLTBASE_IMATCHTVELOTRACKS_H
