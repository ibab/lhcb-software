// $Id: IPVFitterTool.h,v 1.1.1.1 2007-10-09 18:46:14 smenzeme Exp $
#ifndef IPVFITTERTOOL_H 
#define IPVFITTERTOOL_H 1

// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

namespace LHCb {
  class Track;
  class RecVertex;
}

class PVTrack;
typedef std::vector<PVTrack*> PVTrackPtrs;
typedef std::vector<PVTrack> PVTracks;

// Interface ID 
static const InterfaceID IID_IPVFitterTool("IPVFitterTool",1,0);

class IPVFitterTool : virtual public IAlgTool {
 public:
  // Retrieve interface ID
  static const InterfaceID& interfaceID() {return IID_IPVFitterTool;}

  // Fit
  virtual StatusCode fit(LHCb::RecVertex& vtx, PVTrackPtrs& tracks) = 0;
  virtual StatusCode fit(LHCb::RecVertex& vtx, PVTracks& tracks) = 0;
  virtual StatusCode fit(LHCb::RecVertex& vtx, LHCb::Tracks& tracks) = 0;
  virtual StatusCode fit(LHCb::RecVertex& vtx, 
                         std::vector<LHCb::Track*>& tracks) = 0;
  // Make PVTracks from TrStoredTracks
  virtual StatusCode addTrackForPV(LHCb::Track* str, PVTracks& pvTracks) = 0;

};
#endif // IPVFITTERTOOL_H
