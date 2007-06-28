// $Id: ITrackUpgrade.h,v 1.3 2007-06-28 22:06:28 hernando Exp $
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


static const InterfaceID IID_ITrackUpgrade ( "ITrackUpgrade", 1, 0 );

/** @class ITrackUpgrade ITrackUpgrade.h HltBase/ITrackUpgrade.h
 *
 *  functionality: 
 *      upgrade a track adding info from other subdetectors
 *      uniform the tracking tools      
 *
 *  @author Jose A. Hernando
 *  @date   2007-02-08
 */
class ITrackUpgrade : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackUpgrade; }

  virtual StatusCode setReco(const std::string& name) = 0;

  virtual StatusCode upgrade( std::vector<LHCb::Track*>& itracks,
                              std::vector<LHCb::Track*>& otracks) = 0;
};
#endif 
