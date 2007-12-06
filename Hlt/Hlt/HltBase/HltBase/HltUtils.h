#ifndef HLT_HLTUTILS_H 
#define HLT_HLTUTILS_H 1

#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

//#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Event/L0CaloCandidate.h"

#include <algorithm>
#include <functional>

//forward declarations
namespace LHCb {
  class Track;
};

typedef Gaudi::XYZVector EVector;
typedef Gaudi::XYZPoint EPoint;

typedef LHCb::RecVertex TVertex;

namespace HltUtils
{

  //! return the impact parameter vector
  double rImpactParameter(const LHCb::RecVertex& vertex,
                          const LHCb::Track& track);
  
  //! return the impact parameter vector
  double impactParameter(const LHCb::RecVertex& vertex,
                         const LHCb::Track& track);

  double invariantMass(const LHCb::Track&, const LHCb::Track&,
                      double, double);
 

  //! return the RecVertex made with this 2 tracks
  //! WARNING NOTE: the caller get ownership of the LHCb::RecVertex and 
  //                the responsability of deleting it!
  LHCb::RecVertex* newRecVertex(const LHCb::Track& track1,
                                    const LHCb::Track& track2);

  //! return the closest distance between the 2 tracks (first State)
  EVector closestDistance(const LHCb::Track& track1, 
                          const LHCb::Track& track2);

  double deltaEta(const LHCb::Track& track1, const LHCb::Track& track2);

  double deltaPhi(const LHCb::Track& track1, const LHCb::Track& track2);

  double deltaAngle(const LHCb::Track& track1, const LHCb::Track& track2);

  //! retun the closest point between the 2 tracks (first State)
  EPoint closestPoint(const LHCb::Track& track1,
                      const LHCb::Track& track2);

  double FC(const LHCb::RecVertex& svertex, 
            const LHCb::RecVertex& pvertex );

  double FC2(const LHCb::RecVertex& svertex, 
             const LHCb::RecVertex& pvertex );


  double minPT(const LHCb::RecVertex& vertex);

  double maxPT(const LHCb::RecVertex& vertex);

  double matchIDsFraction(const LHCb::Track& trackreference, 
                          const LHCb::Track& track2);

  double vertexMatchIDsFraction(const LHCb::RecVertex& vreference, 
                                const LHCb::RecVertex& vertex);

  inline bool matchIDs(const LHCb::Track& treference, 
                       const LHCb::Track& track) {
    return (matchIDsFraction(treference,track) > 0.70);    
  }
  
  double closestDistanceMod(const LHCb::Track& track1,
                            const LHCb::Track& track2);

  double IPError(const LHCb::Track& track);

  //------------------------------------------------------------

  //! compute the impact parameter vector
  EVector impactParameterVector(const EPoint& vertex,
                                const EPoint& point,
                                const EVector& direction);

  //! return the modules of the impact parameter (signed)
  double impactParameter(const EPoint& vertex,
                         const EPoint& point,
                         const EVector& direction);
  
  //! return points in rays 
  bool closestPoints(const EPoint& ori1, const EVector& dir1,
                     const EPoint& ori2, const EVector& dir2,
                     EPoint& close1, EPoint& close2);

  //! old parameterization of Pt error
  double impactParameterError(double pt);
  
  
  //------------------------------------------------------------
  
  bool doShareM3(const LHCb::Track& track0, const LHCb::Track& track1);
  

  // calocellid tools
	
  
	std::vector<LHCb::CaloCellID> get3x3CellIDs( const LHCb::CaloCellID& centercell );	
	
	std::vector<LHCb::CaloCellID> get2x2CellIDs( const LHCb::CaloCellID& bottomleftcell );
	
	bool matchCellIDs( const std::vector<LHCb::CaloCellID>& oncells, 
					   const std::vector<LHCb::CaloCellID>& offcells );
	

};

#endif 

// namespace HltFuntors {
  
//   template <class Arg, class Res>
//   struct UniFunctor : public unary_function<Arg,Res> {
//     typedef Res (*PtrFun) (const Arg&) const;
//   protected:
//     PtrFun m_fun;
//   public:
//     explicit UniFun(PtrFun fun): m_fun(fun){};
//     Res operator() (const Arg& a) const 
//     {return (*m_fun) (a);}
//   };

//   template <class Arg1, class Arg2, class Res>
//   struct BiFunctor : public binary_function<Arg1,Arg2,Res> {
//     typedef Res (*PtrFun) (const Arg1&, const Arg2&) const;
//   protected:
//     PtrFun m_fun;
//   public:
//     explicit UniFun(PtrFun fun): m_fun(fun){};
//     Res operator() (const Arg1& a1, const Arg& a2) const 
//     {return (*m_fun) (a);}
//   };
  
//   typedef BiFunctor<double,TVertex,Track> BiFunctorVertexTrack;
  
// };





