// $Id: PVUtils.h,v 1.3 2010-01-20 13:46:49 rlambert Exp $
#ifndef PVUTILS_H 
#define PVUTILS_H 1

// Include files
#include <vector>

// Track info
#include "Event/State.h"

namespace LHCb {
  class Track;
}

class PVTrack 
{
public:
 PVTrack():refTrack(0),stateG(),unitVect(),isUsed(false),vd0(),d0(0),err2d0(0),chi2(0),weight(0),type(LHCb::Track::Velo){};
  
  const LHCb::Track* refTrack;
  // Current state of the track at the current point
  LHCb::State stateG;
  // Normalized vector of slope
  Gaudi::XYZVector unitVect;
  // Flag if the track has been used in a previous vertex
  bool isUsed;

  // Result for impact parameter
  Gaudi::XYZVector vd0;          // Impact parameter vector
  double d0;                     // Impact parameter
  double err2d0;                 // IP error squared
  double chi2;                   // chi2 = d02 / d0err**2
  double weight;                 // Weight assigned to track
  LHCb::Track::Types  type;

  inline double zClose() { 
    return stateG.z() - unitVect.z() * 
           (unitVect.x() * stateG.x() + unitVect.y() * stateG.y()) /
           (1.0 - pow(unitVect.z(),2)); 
  }
};

typedef std::vector<PVTrack> PVTracks;
typedef std::vector<PVTrack*> PVTrackPtrs;

class PVVertex {
public:
  PVTrackPtrs pvTracks;
  LHCb::RecVertex primVtx;
  
  PVVertex():pvTracks(0),primVtx(0){};

  
};
typedef std::vector<PVVertex> PVVertices;

#endif // PVUTILS_H
