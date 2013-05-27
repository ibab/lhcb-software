// $Id: IMuonTrackMomRec.h,v 1.1 2010-02-10 19:07:50 ggiacomo Exp $
#ifndef MUONINTERFACES_IMUONTRACKMOMREC_H 
#define MUONINTERFACES_IMUONTRACKMOMREC_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from LHCb
#include "Event/Track.h"

class MuonTrack;
static const InterfaceID IID_IMuonTrackMomRec ( "IMuonTrackMomRec", 1, 0 );

/** @class IMuonTrackMomRec IMuonTrackMomRec.h MuonInterfaces/IMuonTrackMomRec.h
 *  
 *
 *  @author Giacomo Graziani
 *  @date   2010-02-10
 */
class IMuonTrackMomRec : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMuonTrackMomRec; }

  virtual StatusCode recMomentum(MuonTrack* track, 
                                 LHCb::Track* lbTrack) = 0;
  virtual double getBdl() = 0;
  virtual double getZcenter() = 0;
};
#endif // MUONINTERFACES_IMUONTRACKMOMREC_H
