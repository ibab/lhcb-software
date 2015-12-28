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

/** @class IMuonTrackMomRec IMuonTrackMomRec.h MuonInterfaces/IMuonTrackMomRec.h
 *  
 *
 *  @author Giacomo Graziani
 *  @date   2010-02-10
 */
class IMuonTrackMomRec : public extend_interfaces<IAlgTool> {
public: 

  // Return the interface ID
  DeclareInterfaceID( IMuonTrackMomRec, 2, 0 );

  virtual StatusCode recMomentum(MuonTrack* track, 
                                 LHCb::Track* lbTrack) const = 0;
  virtual double getBdl() const = 0;
  virtual double getZcenter() const = 0;
};
#endif // MUONINTERFACES_IMUONTRACKMOMREC_H
