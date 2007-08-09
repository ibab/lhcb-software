// $Id: ITrackMatchUpgrade.h,v 1.1 2007-08-09 13:58:07 hernando Exp $
#ifndef HLTBASE_ITRACKMATCHUPGRADE_H 
#define HLTBASE_ITRACKMATCHUPGRADE_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

//forward declarations
namespace LHCb {
  class Track;
  class State;
};

static const InterfaceID IID_ITrackMatchUpgrade ( "ITrackMatchUpgrade", 1, 0 );

/** @class ITrackMatchUpgrade
 *  
 *
 *  @author Jose A. Hernando
 *  @date   2007-08-04
 */
class ITrackMatchUpgrade : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackMatchUpgrade; }

  virtual StatusCode match(const LHCb::Track& track0, 
                           const LHCb::Track& track1,
                           LHCb::Track& track,
                           double& quality)=0;

};
#endif // HLTBASE_IMATCHTVELOTRACKS_H
