#ifndef HLT_HLTUTILS_H 
#define HLT_HLTUTILS_H 1

#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "LHCbMath/GeomFun.h"
#include "LHCbMath/Line.h"

#include "Event/Track.h"
#include "Event/RecVertex.h"

#include <algorithm>
#include <functional>

namespace Hlt {
  /* Functor: to order tracks by decresing order of PT
   *
   */
  class SortTrackByPt {
  public:
    bool operator() (const LHCb::Track* lhs, 
                     const LHCb::Track* rhs ) const {
        double ptl = lhs->pt();
        double ptr = rhs->pt();
        return (ptl == ptr) ? (lhs->key() > rhs->key())
                            : (ptl > ptr) ;
}
  };
  /* It fills the vertex using the 2 tracks
   *
   */
  class VertexCreator 
  {
  public:
    explicit VertexCreator() {}
    void operator() (const LHCb::Track& track1,
                     const LHCb::Track& track2,
                     LHCb::RecVertex& ver) const;
  };
}


namespace HltUtils
{

  //! return the impact parameter vector
  double rImpactParameter(const LHCb::RecVertex& vertex,
                          const LHCb::Track& track);
  
  //! return the impact parameter vector
  inline double impactParameter(const LHCb::RecVertex& vertex,
                         const LHCb::Track& track) {
      const Gaudi::XYZPoint& vtx = vertex.position();
      const LHCb::State& state = track.firstState();

      Gaudi::XYZVector vec =Gaudi::Math::closestPoint( vtx, Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector>( state.position(), state.slopes() ) ) - vtx;
      return vec.Z() < 0 ? -vec.R() : vec.R() ;
  }

  inline double invariantMass(const LHCb::Track& track1, const LHCb::Track& track2,
                      double mass1, double mass2) {
      //TODO: is this the numerically most stable way of computing 
      //      invariant masses?? (esp. if mass^2 << mom.mag2 ! )
      double e = sqrt(mass1*mass1+track1.momentum().mag2())
               + sqrt(mass2*mass2+track2.momentum().mag2());
      return sqrt(e*e-(track1.momentum()+track2.momentum()).mag2());
  }

 


  //! return the closest distance between the 2 tracks (first State)
  Gaudi::XYZVector closestDistance(const LHCb::Track& track1, 
                                   const LHCb::Track& track2);

  inline double deltaEta(const LHCb::Track& track1, const LHCb::Track& track2) {
    return track2.slopes().Eta() - track1.slopes().Eta();
  }

  inline double deltaPhi(const LHCb::Track& track1, const LHCb::Track& track2) {
    return track2.slopes().Phi() -  track1.slopes().Phi();;
  }

  inline double deltaAngle(const LHCb::Track& track1, const LHCb::Track& track2) {
    return acos( track1.slopes().Unit().Dot(  track2.slopes().Unit()  ) );
  }

  //! retun the closest point between the 2 tracks (first State)
  Gaudi::XYZPoint closestPoint(const LHCb::Track& track1,
                      const LHCb::Track& track2);

  inline double FC(const LHCb::RecVertex& svtx, 
            const LHCb::RecVertex& pvtx )
  {
    const LHCb::Track& t1 = *(svtx.tracks()[0]);
    const LHCb::Track& t2 = *(svtx.tracks()[1]);

    double pperp =     (  t1.momentum()+t2.momentum()         )
                 .Cross( (svtx.position()-pvtx.position()).Unit() )
                 .R();

    return pperp/(pperp+t1.pt() + t2.pt());
  }

  double VertexMinPT(const LHCb::RecVertex& vertex);

  double VertexMaxPT(const LHCb::RecVertex& vertex);

  inline double matchIDsFraction(const LHCb::Track& tref, 
                                 const LHCb::Track& track) {
      return (tref.lhcbIDs().empty()) ? 0. :
                 double(tref.nCommonLhcbIDs( track) )/
                 double(tref.lhcbIDs().size());
  }

  double vertexMatchIDsFraction(const LHCb::RecVertex& vreference, 
                                const LHCb::RecVertex& vertex);

  inline bool matchIDs(const LHCb::Track& treference, 
                       const LHCb::Track& track) {
    return (matchIDsFraction(treference,track) > 0.70);    
  }
  

  inline double closestDistanceMod(const LHCb::Track& track1,
                                   const LHCb::Track& track2) {
    return closestDistance(track1,track2).R();
  } 


  bool doShareM3(const LHCb::Track& track0, const LHCb::Track& track1);
  

  double vertexMatchIDsFraction(const LHCb::RecVertex& vreference, 
                                const LHCb::RecVertex& vertex);
  
  //------------------------------------------------------------

  //! compute the impact parameter vector
  inline Gaudi::XYZVector impactParameterVector(const Gaudi::XYZPoint& vertex,
                                const Gaudi::XYZPoint& point,
                                const Gaudi::XYZVector& direction) {
      Gaudi::XYZVector d = direction.unit();
      return d.Cross((point-vertex).Cross(d));
  }

  //! return the modules of the impact parameter (signed)
  inline double impactParameter(const Gaudi::XYZPoint& vertex,
                         const Gaudi::XYZPoint& point,
                         const Gaudi::XYZVector& direction)
    {
      Gaudi::XYZVector ip = impactParameterVector(vertex,point,direction);
      return (ip.z() < 0) ? -ip.R() : ip.R();
    }
  
  //------------------------------------------------------------
  
  bool doShareM3(const LHCb::Track& track0, const LHCb::Track& track1);

  // calocellid tools
  std::vector<LHCb::CaloCellID> get3x3CellIDs( const LHCb::CaloCellID& centercell );	
	
  std::vector<LHCb::CaloCellID> get2x2CellIDs( const LHCb::CaloCellID& bottomleftcell );
	
  bool matchCellIDs( const std::vector<LHCb::CaloCellID>& oncells, 
                     const std::vector<LHCb::CaloCellID>& offcells );
	

};

#endif 
