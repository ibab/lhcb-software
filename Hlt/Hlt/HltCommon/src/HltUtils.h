#ifndef HltUtils_H 
#define HltUtils_H 1

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h"

#include "Event/Track.h"
#include "Event/TrackVertex.h"

typedef HepVector3D EVector;
typedef HepPoint3D EPoint;

class HltUtils
{

public:
  
  typedef TrackVertex TVertex;
  
  //! compute the impact parameter vector
  static EVector impactParameterVector(const EPoint& vertex,
                                       const EPoint& point,
                                       const EVector& direction);

  //! return the modules of the impact parameter (signed)
  static double impactParameter(const EPoint& vertex,
                                const EPoint& point,
                                const EVector& direction);
  
  static bool closestPoints(const EPoint& ori1, const EVector& dir1,
                            const EPoint& ori2, const EVector& dir2,
                            EPoint& close1, EPoint& close2);
  
  //! return the impact parameter vector
  static EVector impactParameterVector(const TVertex& vertex,
                                       const State& state);
  
  static double impactParameter(const TVertex& vertex,
                                const State& state);
  
  static EVector impactParameterVector(const TVertex& vertex,
                                       const Track& track) {
    const State& state = track.firstState();
    return impactParameterVector(vertex,state);
  }
  
  //! return the impact parameter vector
  static double impactParameter(const TVertex& vertex,
                                const Track& track) {
    const State& state = track.firstState();
    return impactParameter(vertex,state);  
  }

  //! return the closest distance between these 2 states
  static EVector closestDistance(const State& state1, 
                                 const State& state2);
  
  //! return the closest vertex between these 2 states
  static EPoint vertex(const State& state1, 
                       const State& state2);

  //! return the closest distance between these 2 states
  static EVector closestDistance(const Track& track1, 
                                 const Track& track2) {
    const State& state1 = track1.firstState();
    const State& state2 = track2.firstState();
    return closestDistance(state1,state2);
  }
  
  //! return the closest vertex between these 2 states
  static EPoint vertex(const Track& track1, 
                       const Track& track2) {
    const State& state1 = track1.firstState();
    const State& state2 = track2.firstState();
    return vertex(state1,state2);
  }

  static double impactParameterError(double pt);  

  class SortTrackByPt {
  public:
    bool operator() (const Track* iniTrack, 
                     const Track* endTrack ) const;
  };

};


#endif 
