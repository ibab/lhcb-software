#ifndef RECOFUNCS_H 
#define RECOFUNCS_H 1

/** @file RecoFuncs.h
 *  Collection of functions frequently used in all
 *  pattern reco algorithms
 *  @authors S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-03
 **/

#include "TfKernel/OTHit.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "LHCbMath/GeomFun.h"

namespace Tf {

  template <class Hit>
  class compByX_LB: public std::binary_function<const Hit*, const double, bool> 
  {
  public:
    compByX_LB() {}
    inline bool operator() (const Hit * obj, const double& testVal) const
    {
      return obj->hit()->xAtYEq0() < testVal ;
    }
  };


  template<class Hit>
    class increasingByProjection{
    public:
    bool operator() ( const Hit* lhs, 
                      const Hit* rhs ) const {
      return lhs->projection() < rhs->projection();
    }
  };
  

  template<class Hit>
    class increasingByZ{
    public:
    bool operator() (const Hit*  lhs,  
		     const Hit*  rhs){
      return lhs->z() < rhs->z();
    } 
  };
   

  template<class Hit>
    class increasingByX{
    public:
    bool operator() (const Hit*  lhs,  
		     const Hit*  rhs){
      return lhs->x() < rhs->x();
    }
  };

   template<class Hit>
    class increasingByXAtYEq0{
    public:
    bool operator() (const Hit*  lhs,  
		     const Hit*  rhs){
      return lhs->hit()->xAtYEq0() < rhs->hit()->xAtYEq0();
    }
  };


   // y0  is here y of track @ z position plane
   template<class Hit>
   inline void updateTTHitForTrack(Hit* hit, double y0, double dyDz){
     double z = hit->hit()->zAtYEq0() + dyDz * hit->hit()->dzDy();
     double y = y0 + dyDz*(z-hit->hit()->zAtYEq0());
     hit->setZ( z ) ;
     hit->setX( hit->hit()->x(y) ) ;
   }
  
  

   // y0 is here y of track @ z=0
  template<class Hit>
  inline void updateHitForTrack(Hit* hit, double y0, double dyDz){
    double y  = ( y0 + dyDz * hit->hit()->zAtYEq0() ) / ( 1. - hit->hit()->dzDy() * dyDz );
    hit->setZ( hit->hit()->z(y) ) ;
    hit->setX( hit->hit()->x(y) ) ;
    const Tf::OTHit* otHit = hit->hit()->othit();
    if(otHit) {
      hit->setDriftDistance(otHit->untruncatedDriftDistance(y));
    }
  }

  template<class Hit>
  inline bool intersection( Hit* hit,
                            const Gaudi::Plane3D& plane,
                            Gaudi::XYZPoint& intersect ) 
  {
    double mu = 0.0;
    return Gaudi::Math::intersection(*hit->hit(), plane, intersect, mu);
  }

}

#endif // RECOFUNCS_H
