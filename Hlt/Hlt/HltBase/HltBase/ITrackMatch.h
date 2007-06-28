// $Id: ITrackMatch.h,v 1.2 2007-06-28 22:06:28 hernando Exp $
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
  class State;
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
                           double& quality)=0;

};
#endif // HLTBASE_IMATCHTVELOTRACKS_H
