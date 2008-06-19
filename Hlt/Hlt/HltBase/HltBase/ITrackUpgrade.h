// $Id: ITrackUpgrade.h,v 1.4 2008-06-19 09:26:53 hernando Exp $
#ifndef HLTBASE_ITRACKUPGRADE_H 
#define HLTBASE_ITRACKUPGRADE_H 1

// Include files
// from STL
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
// #include "HltBase/ITrackView.h"

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

 //  // return the list of upgrade track reconstruction 
//   virtual std::vector<std::string>& recos() = 0;
  
  // set this reconstruction as the default one
  virtual StatusCode setReco(const std::string& name) = 0;
  
  // upgrade a list of input tracks
  virtual StatusCode upgrade( std::vector<LHCb::Track*>& itracks,
                              std::vector<LHCb::Track*>& otracks) = 0;
  
//   // upgrade an input track
//   virtual StatusCode upgrade( LHCb::Track&, 
//                               std::vector<LHCb*>& tracks) = 0;
  
//   // returns the view of the seed track (the volume of search window)
//   virtual Tf:IStationSelection& view(const LHCb::Track& seed) = 0;
  
//   // return the ids of the hits in the search window
//   virtual std::vector<LHCbID> lhcbIDsInView(const LHCb::Track& seed) = 0;

};
#endif 
