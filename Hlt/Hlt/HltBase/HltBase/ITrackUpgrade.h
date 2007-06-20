// $Id: ITrackUpgrade.h,v 1.1 2007-06-20 12:08:41 hernando Exp $
#ifndef HLTBASE_ITRACKUPGRADE_H 
#define HLTBASE_ITRACKUPGRADE_H 1

// Include files
// from STL
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

//forward declarations
namespace LHCb {
  class Track;
  class State;
};


static const InterfaceID IID_IITrackUpgrade ( "ITrackUpgrade", 1, 0 );

/** @class IITrackUpgrade IITrackUpgrade.h HltBase/IITrackUpgrade.h
 *
 *  functionality: 
 *      upgrade a track adding info from other subdetectors
 *      uniform the tracking tools      
 *
 *  @author Jose A. Hernando
 *  @date   2007-02-08
 */
class IITrackUpgrade : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IITrackUpgrade; }

  virtual StatusCode upgrade( const LHCb::Track& seed,
                              std::vector<LHCb::Track*>& tracks) = 0;
};
#endif 
