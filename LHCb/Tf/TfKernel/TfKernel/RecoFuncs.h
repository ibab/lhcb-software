#ifndef TFKERNEL_RECOFUNCS_H 
#define TFKERNEL_RECOFUNCS_H 1

//-----------------------------------------------------------------------------
/** @file RecoFuncs.h
 *
 *  Collection of functions frequently used in all
 *  pattern reco algorithms
 *
 *  CVS Log :-
 *  $Id: RecoFuncs.h,v 1.7 2008-05-29 10:55:11 smenzeme Exp $
 *
 *  @authors S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-03
 **/
//-----------------------------------------------------------------------------

#include <algorithm>
#include "TfKernel/OTHit.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "LHCbMath/GeomFun.h"

namespace Tf
{

  /** @class compByX_LB TfKernel/RecoFuncs.h
   *  Binary function to compare a hit to a given x value and return true if hit is upstream of that point.
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   */
  template <class Hit>
  class compByX_LB : public std::binary_function<const Hit*, const double, bool>
  {
  public:
    compByX_LB() {}
    /** Comparison operator
     *  @param[in] obj     The Hit object to test
     *  @param[in] testVal The test value of x
     *  @return boolean
     *  @retval TRUE hit is upstream of testVal
     *  @retval TRUE hit is downstream of testVal
     */
    inline bool operator() (const Hit * obj, const double testVal) const
    {
      return obj->hit()->xAtYEq0() < testVal ;
    }
  };

  /** @class increasingByProjection TfKernel/RecoFuncs.h
   *  Binary sorting function to sort hits by projection
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   *  @author M. Schiller
   *  @date 2012-02-07
   *
   *  the template parameter forceOrder specifies if hits with same
   *  projection should be ordered by LHCbID
   */
  template<class Hit, bool forceOrder = true>
  class increasingByProjection :
    public std::binary_function<const Hit*, const Hit*, bool>
  {
  public:
    /** Comparison operator
     *  @param[in] lhs Hit object on lhs of < operator
     *  @param[in] rhs Hit object on rhs of < operator
     *  @return boolean indicating the sorting of lhs and rhs by projection
     */
    inline bool operator() ( const Hit* lhs,
                             const Hit* rhs ) const
    {
      if (lhs->projection() < rhs->projection()) return true;
      if (rhs->projection() < lhs->projection()) return false;
      return lhs->hit()->lhcbID() < rhs->hit()->lhcbID();
    }
  };

  template<class Hit>
  class increasingByProjection<Hit, false> :
    public std::binary_function<const Hit*, const Hit*, bool>
  {
  public:
    /** Comparison operator
     *  @param[in] lhs Hit object on lhs of < operator
     *  @param[in] rhs Hit object on rhs of < operator
     *  @return boolean indicating the sorting of lhs and rhs by projection
     */
    inline bool operator() ( const Hit* lhs,
	                     const Hit* rhs ) const
    { return lhs->projection() < rhs->projection(); }
  };

  /** @class increasingByZ TfKernel/RecoFuncs.h
   *  Binary sorting function to sort hits by z
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   *  @author M. Schiller
   *  @date 2012-02-07
   *
   *  the template parameter forceOrder specifies if hits with same
   *  z should be ordered by LHCbID
   */
  template<class Hit, bool forceOrder = true>
  class increasingByZ :
    public std::binary_function<const Hit*, const Hit*, bool>
  {
  public:
    /** Comparison operator
     *  @param[in] lhs Hit object on lhs of < operator
     *  @param[in] rhs Hit object on rhs of < operator
     *  @return boolean indicating the sorting of lhs and rhs by z
     */
    inline bool operator() ( const Hit* lhs,
                             const Hit* rhs ) const
    {
      if (lhs->z() < rhs->z()) return true;
      if (rhs->z() < lhs->z()) return false;
      return lhs->hit()->lhcbID() < rhs->hit()->lhcbID();
    }
  };

  template<class Hit>
  class increasingByZ<Hit, false> :
    public std::binary_function<const Hit*, const Hit*, bool>
  {
  public:
    /** Comparison operator
     *  @param[in] lhs Hit object on lhs of < operator
     *  @param[in] rhs Hit object on rhs of < operator
     *  @return boolean indicating the sorting of lhs and rhs by z
     */
    inline bool operator() ( const Hit* lhs,
	                     const Hit* rhs ) const
    { return lhs->z() < rhs->z(); }
  };

  /** @class increasingByX TfKernel/RecoFuncs.h
   *  Binary sorting function to sort hits by x
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   *  @author M. Schiller
   *  @date 2012-02-07
   *
   *  the template parameter forceOrder specifies if hits with same
   *  x should be ordered by LHCbID
   */
  template<class Hit, bool forceOrder = true>
  class increasingByX :
    public std::binary_function<const Hit*, const Hit*, bool>
  {
  public:
    /** Comparison operator
     *  @param[in] lhs Hit object on lhs of < operator
     *  @param[in] rhs Hit object on rhs of < operator
     *  @return boolean indicating the sorting of lhs and rhs by x
     */
    inline bool operator() ( const Hit* lhs,
                             const Hit* rhs ) const
    {
      if (lhs->x() < rhs->x()) return true;
      if (rhs->x() < lhs->x()) return false;
      return lhs->hit()->lhcbID() < rhs->hit()->lhcbID();
    }
  };

  template<class Hit>
  class increasingByX<Hit, false> :
    public std::binary_function<const Hit*, const Hit*, bool>
  {
  public:
    /** Comparison operator
     *  @param[in] lhs Hit object on lhs of < operator
     *  @param[in] rhs Hit object on rhs of < operator
     *  @return boolean indicating the sorting of lhs and rhs by x
     */
    inline bool operator() ( const Hit* lhs,
	                     const Hit* rhs ) const
    { return lhs->x() < rhs->x(); }
  };

  /** @class increasingByXAtYEq0 TfKernel/RecoFuncs.h
   *  Binary sorting function to sort hits by x at the point y=0
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   *  @author M. Schiller
   *  @date 2012-02-07
   *
   *  the template parameter forceOrder specifies if hits with same
   *  x at y= 0 should be ordered by LHCbID
   */
  template<class Hit, bool forceOrder = true>
  class increasingByXAtYEq0 :
    public std::binary_function<const Hit*, const Hit*, bool>
  {
  public:
    /** Comparison operator
     *  @param[in] lhs Hit object on lhs of < operator
     *  @param[in] rhs Hit object on rhs of < operator
     *  @return boolean indicating the sorting of lhs and rhs by x at the point y=0
     */
    inline bool operator() ( const Hit*  lhs,
                             const Hit*  rhs ) const
    {
      if (lhs->hit()->xAtYEq0() < rhs->hit()->xAtYEq0()) return true;
      if (rhs->hit()->xAtYEq0() < lhs->hit()->xAtYEq0()) return false;
      return lhs->hit()->lhcbID() < rhs->hit()->lhcbID();
    }
  };

  template<class Hit>
  class increasingByXAtYEq0<Hit, false> :
    public std::binary_function<const Hit*, const Hit*, bool>
  {
  public:
    /** Comparison operator
     *  @param[in] lhs Hit object on lhs of < operator
     *  @param[in] rhs Hit object on rhs of < operator
     *  @return boolean indicating the sorting of lhs and rhs by x at the point y=0
     */
    inline bool operator() ( const Hit* lhs,
	                     const Hit* rhs ) const
    { return lhs->xAtYEq0() < rhs->xAtYEq0(); }
  };

  /** @class decreasingByProjection TfKernel/RecoFuncs.h
   *  Binary sorting function to sort hits by projection
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   *  @author M. Schiller
   *  @date 2012-02-07
   *
   *  the template parameter forceOrder specifies if hits with same
   *  projection should be ordered by LHCbID
   */
  template<class Hit, bool forceOrder = true>
  class decreasingByProjection :
    public std::binary_function<const Hit*, const Hit*, bool>
  {
  public:
    /** Comparison operator
     *  @param[in] lhs Hit object on lhs of < operator
     *  @param[in] rhs Hit object on rhs of < operator
     *  @return boolean indicating the sorting of lhs and rhs by projection
     */
    inline bool operator() ( const Hit* lhs,
                             const Hit* rhs ) const
    {
      if (rhs->projection() < lhs->projection()) return true;
      if (lhs->projection() < rhs->projection()) return false;
      return lhs->hit()->lhcbID() < rhs->hit()->lhcbID();
    }
  };

  template<class Hit>
  class decreasingByProjection<Hit, false> :
    public std::binary_function<const Hit*, const Hit*, bool>
  {
  public:
    /** Comparison operator
     *  @param[in] lhs Hit object on lhs of < operator
     *  @param[in] rhs Hit object on rhs of < operator
     *  @return boolean indicating the sorting of lhs and rhs by projection
     */
    inline bool operator() ( const Hit* lhs,
	                     const Hit* rhs ) const
    { return rhs->projection() < lhs->projection(); }
  };

  /** @class decreasingByZ TfKernel/RecoFuncs.h
   *  Binary sorting function to sort hits by z
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   *  @author M. Schiller
   *  @date 2012-02-07
   *
   *  the template parameter forceOrder specifies if hits with same
   *  z should be ordered by LHCbID
   */
  template<class Hit, bool forceOrder = true>
  class decreasingByZ :
    public std::binary_function<const Hit*, const Hit*, bool>
  {
  public:
    /** Comparison operator
     *  @param[in] lhs Hit object on lhs of < operator
     *  @param[in] rhs Hit object on rhs of < operator
     *  @return boolean indicating the sorting of lhs and rhs by z
     */
    inline bool operator() ( const Hit* lhs,
                             const Hit* rhs ) const
    {
      if (rhs->z() < lhs->z()) return true;
      if (lhs->z() < rhs->z()) return false;
      return lhs->hit()->lhcbID() < rhs->hit()->lhcbID();
    }
  };

  template<class Hit>
  class decreasingByZ<Hit, false> :
    public std::binary_function<const Hit*, const Hit*, bool>
  {
  public:
    /** Comparison operator
     *  @param[in] lhs Hit object on lhs of < operator
     *  @param[in] rhs Hit object on rhs of < operator
     *  @return boolean indicating the sorting of lhs and rhs by z
     */
    inline bool operator() ( const Hit* lhs,
	                     const Hit* rhs ) const
    { return rhs->z() < lhs->z(); }
  };

  /** @class decreasingByX TfKernel/RecoFuncs.h
   *  Binary sorting function to sort hits by x
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   *  @author M. Schiller
   *  @date 2012-02-07
   *
   *  the template parameter forceOrder specifies if hits with same
   *  x should be ordered by LHCbID
   */
  template<class Hit, bool forceOrder = true>
  class decreasingByX :
    public std::binary_function<const Hit*, const Hit*, bool>
  {
  public:
    /** Comparison operator
     *  @param[in] lhs Hit object on lhs of < operator
     *  @param[in] rhs Hit object on rhs of < operator
     *  @return boolean indicating the sorting of lhs and rhs by x
     */
    inline bool operator() ( const Hit* lhs,
                             const Hit* rhs ) const
    {
      if (rhs->x() < lhs->x()) return true;
      if (lhs->x() < rhs->x()) return false;
      return lhs->hit()->lhcbID() < rhs->hit()->lhcbID();
    }
  };

  template<class Hit>
  class decreasingByX<Hit, false> :
    public std::binary_function<const Hit*, const Hit*, bool>
  {
  public:
    /** Comparison operator
     *  @param[in] lhs Hit object on lhs of < operator
     *  @param[in] rhs Hit object on rhs of < operator
     *  @return boolean indicating the sorting of lhs and rhs by x
     */
    inline bool operator() ( const Hit* lhs,
	                     const Hit* rhs ) const
    { return rhs->x() < lhs->x(); }
  };

  /** @class decreasingByXAtYEq0 TfKernel/RecoFuncs.h
   *  Binary sorting function to sort hits by x at the point y=0
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   *  @author M. Schiller
   *  @date 2012-02-07
   *
   *  the template parameter forceOrder specifies if hits with same
   *  x at y= 0 should be ordered by LHCbID
   */
  template<class Hit, bool forceOrder = true>
  class decreasingByXAtYEq0 :
    public std::binary_function<const Hit*, const Hit*, bool>
  {
  public:
    /** Comparison operator
     *  @param[in] lhs Hit object on lhs of < operator
     *  @param[in] rhs Hit object on rhs of < operator
     *  @return boolean indicating the sorting of lhs and rhs by x at the point y=0
     */
    inline bool operator() ( const Hit*  lhs,
                             const Hit*  rhs ) const
    {
      if (rhs->hit()->xAtYEq0() < lhs->hit()->xAtYEq0()) return true;
      if (lhs->hit()->xAtYEq0() < rhs->hit()->xAtYEq0()) return false;
      return lhs->hit()->lhcbID() < rhs->hit()->lhcbID();
    }
  };

  template<class Hit>
  class decreasingByXAtYEq0<Hit, false> :
    public std::binary_function<const Hit*, const Hit*, bool>
  {
  public:
    /** Comparison operator
     *  @param[in] lhs Hit object on lhs of < operator
     *  @param[in] rhs Hit object on rhs of < operator
     *  @return boolean indicating the sorting of lhs and rhs by x at the point y=0
     */
    inline bool operator() ( const Hit* lhs,
	                     const Hit* rhs ) const
    { return rhs->xAtYEq0() < lhs->xAtYEq0(); }
  };

  template<class Hit>
  inline void updateTTHitForTrack ( Hit* hit, 
                                    const double y0, 
                                    const double dyDz ) 
  {
    const double y  = ( y0 + dyDz * hit->hit()->zAtYEq0() ) / ( 1. - hit->hit()->dzDy() * dyDz );
    hit->setZ( hit->hit()->z(y) ) ;
    hit->setX( hit->hit()->x(y) ) ;
  }


  template<class Hit>
  inline void updateHitForTrack ( Hit* hit, 
                                  const double y0, 
                                  const double dyDz ) 
  {
    const double y  = ( y0 + dyDz * hit->hit()->zAtYEq0() ) / ( 1. - hit->hit()->dzDy() * dyDz );
    hit->setZ( hit->hit()->z(y) ) ;
    hit->setX( hit->hit()->x(y) ); 
    const Tf::OTHit* otHit = hit->hit()->othit();
    if(otHit) 
    {
      hit->setDriftDistance(otHit->untruncatedDriftDistance(y));
    }
  }

  /** Function to find the intersection of a (line) hit with a plane
   *  @param[in] hit The hit object pointer
   *  @param[in] plane The plane to find the intersection with
   *  @param[out] intersect The interestion point
   *  @return boolean indicating the status of the interestion
   *  @retval TRUE An intersection point was successfully found
   *  @retval FALSE No intersection point was found
   */
  template<class Hit>
  inline bool intersection( const Hit* hit,
                            const Gaudi::Plane3D& plane,
                            Gaudi::XYZPoint& intersect )
  {
    double mu = 0.0;
    return Gaudi::Math::intersection(*hit->hit(), plane, intersect, mu);
  }

}

#endif // TFKERNEL_RECOFUNCS_H
