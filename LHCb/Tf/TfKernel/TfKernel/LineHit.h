#ifndef TF_LINEHIT_H 
#define TF_LINEHIT_H 1

// Include files
#include "TfKernel/HitBase.h"
#include "STDet/DeSTSector.h"
#include "OTDet/DeOTModule.h"
#include "Event/STLiteCluster.h"
#include "TfKernel/OTLiteTime.h"


namespace Tf {

  /** @class LineHit Store a coordinate from tracking systems which
   *  can be represented by a line segment. The line is parameterized
   *  along its y-coordinate.
   *
   *  @authors S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   **/

  class LineHit : public HitBase
  {
  public:
    typedef Gaudi::XYZPointF Point ;
    typedef Gaudi::XYZVectorF Vector ;

    typedef line_hit_tag hit_type_tag; ///< the hit type tag

    //== Simple accessors to internal data members 
    float yBegin()  const { return m_ybegin ; }
    float yEnd()    const { return m_yend ; }
    float xAtYEq0() const { return m_xAtYEq0 ; }
    float zAtYEq0() const { return m_zAtYEq0 ; }
    float dxDy()    const { return m_dxdy ; }
    float dzDy()    const { return m_dzdy ; }

    /** x-coordinate in rotated system */
    float xT()   const { return coord() ; }

    /** x coordinate as function of y */
    float x(float globalY = 0) const { return m_xAtYEq0 + globalY*m_dxdy ; }

    /** z coordinate as function of y */
    float z(float globalY = 0) const { return m_zAtYEq0 + globalY*m_dzdy ; }

    /** y coordinate in middle of strip */
    float y() const { return 0.5*(yBegin()+yEnd()); }

    /** tan of stereo angle */
    float tanT() const { return -m_dxdy ; }

    /** sin of stereo angle */
    float sinT() const { return tanT()*cosT()  ; }
    
    /** cos of stereo angle */
    float cosT() const { return coord()/m_xAtYEq0 ; }
    
    /** is X layer */
    bool  isX () const { return (layer() == 0 || layer() == 3) ; }

    /**  minimum y coordinate of segment endpoints */
    float yMin() const { return std::min(yBegin(),yEnd()) ; }

    /**  maximum y coordinate of segment endpoints */
    float yMax() const { return std::max(yBegin(),yEnd()) ; }

    /**  minimum x coordinate of segment endpoints */
    float xMin() const { return std::min(x(yBegin()),x(yEnd())) ; }

    /**  maximum x coordinate of segment endpoints */
    float xMax() const { return std::max(x(yBegin()),x(yEnd())) ; }

    /** y coordinate of segment mid point */
    float yMid() const { return 0.5*(yBegin()+yEnd()) ; }

    /** x coordinate of segment mid point */
    float xMid() const { return x(yMid()) ; }

    /** z coordinate of segment mid point */
    float zMid() const { return z(yMid()) ; }

    /** check of y is compatible with segment */
    bool isYCompatible ( float y, float tol ) const {
      // which one is faster?
      return fabs( y - 0.5*(yBegin() + yEnd()) ) < 0.5*fabs(yBegin() - yEnd()) + tol ;
      //return yMin() - tol <= y && y <= yMax() + tol ;
    }
     
    /** position for y=0. Note: This is not the begin point of the
	segment. However, it is what is needed to make the
	intersection calls efficient */
    Point beginPoint() const { return Point(m_xAtYEq0,0,m_zAtYEq0) ; }

    /** direction, normalized to its y coordinate */
    Vector direction() const { return Vector(m_dxdy,1,m_dzdy) ; }

    /** position for any value of y */
    Point position(float globalY) const { return Point(x(globalY),globalY,z(globalY)) ; }

    /** length of segment */
    float length() const { return fabs( (yEnd()-yBegin())*sqrt(1 + m_dxdy*m_dxdy + m_dzdy*m_dzdy )) ; }
    
  protected:
    LineHit(const DeOTModule& aModule, const LHCb::OTLiteTime& rawhit) ;
    LineHit(const DeSTSector& aSector, const LHCb::STLiteCluster& clus ) ;
  private:
    float m_dxdy ;
    float m_dzdy ;
    float m_ybegin ;
    float m_yend ;
    float m_xAtYEq0 ;
    float m_zAtYEq0 ;
  };


  ////////////////////////////////////////////////////////////////////////////////////
  // Inline function definitions.
  ////////////////////////////////////////////////////////////////////////////////////

  inline LineHit::LineHit(const DeOTModule& aModule, const LHCb::OTLiteTime& rawhit)
    : HitBase(LHCb::LHCbID(rawhit.channel()),RegionID(rawhit.channel()))
  {
    aModule.trajectory( rawhit.channel().straw(), m_dxdy, m_dzdy, m_xAtYEq0, m_zAtYEq0, m_ybegin, m_yend) ;
    setCoord( m_xAtYEq0 * aModule.cosAngle() ) ;
  }
  
  inline LineHit::LineHit( const DeSTSector& aSector, const LHCb::STLiteCluster& clus )
    : HitBase(LHCb::LHCbID(clus.channelID()),RegionID(clus.channelID()),0,aSector.pitch()*aSector.pitch()/12)
  {
    aSector.trajectory( clus.channelID().strip(), clus.interStripFraction(), m_dxdy, m_dzdy, m_xAtYEq0, m_zAtYEq0, m_ybegin, m_yend) ;
    setCoord( m_xAtYEq0 * aSector.cosAngle() ) ;
  }

}

#endif // TF_LINEHIT_H
