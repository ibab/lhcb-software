// $Id: IMatchTVeloTracks.h,v 1.1 2007-03-08 07:47:18 asatta Exp $
#ifndef HLTBASE_IMATCHTVELOTRACKS_H 
#define HLTBASE_IMATCHTVELOTRACKS_H 1

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

static const InterfaceID IID_IMatchTVeloTracks ( "IMatchTVeloTracks", 1, 0 );

/** @class IMatchTVeloTracks IMatchTVeloTracks.h tmp/IMatchTVeloTracks.h
 *  
 *
 *  @author Johannes Albrecht
 *  @date   2007-01-12
 */
class IMatchTVeloTracks : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMatchTVeloTracks; }

  
  virtual StatusCode match2dVelo(LHCb::Track& veloTrack,LHCb::Track& Ttrack)=0;
  
                                 
  
  virtual StatusCode match3dVelo(LHCb::Track& veloTrack,LHCb::Track& Ttrack, 
                                 LHCb::Track& matchedTrack)=0;
  

protected:

private:

};
#endif // HLTBASE_IMATCHTVELOTRACKS_H
