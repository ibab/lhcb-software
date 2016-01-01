#ifndef PVUTILS_H
#define PVUTILS_H 1

// Include files
#include <vector>

// Track info
#include "Event/State.h"

namespace LHCb {
  class Track;
}

class PVTrack final
{
public:
  PVTrack() = default;

  const LHCb::Track* refTrack = nullptr;
  // Current state of the track at the current point
  LHCb::State stateG;
  // Normalized vector of slope
  Gaudi::XYZVector unitVect;
  // Flag if the track has been used in a previous vertex
  bool isUsed = false;

  // Result for impact parameter
  Gaudi::XYZVector vd0;          // Impact parameter vector
  double d0 = 0;                     // Impact parameter
  double err2d0 = 0;                 // IP error squared
  double chi2 = 0;                   // chi2 = d02 / d0err**2
  double weight = 0;                 // Weight assigned to track
  LHCb::Track::Types  type = LHCb::Track::Velo;

  inline double zClose() {
    return stateG.z() - unitVect.z() *
           (unitVect.x() * stateG.x() + unitVect.y() * stateG.y()) /
           (1.0 - std::pow(unitVect.z(),2));
  }
};

typedef std::vector<PVTrack> PVTracks;
typedef std::vector<PVTrack*> PVTrackPtrs;

class PVVertex final {
public:
  PVTrackPtrs pvTracks;
  LHCb::RecVertex primVtx;

  PVVertex():pvTracks(0),primVtx(0){};
};
typedef std::vector<PVVertex> PVVertices;

#endif // PVUTILS_H
