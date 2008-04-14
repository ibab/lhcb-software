
//-----------------------------------------------------------------------------
/** @file LineHit.h
 *
 *  Header file for track find class Tf::LineHit
 *
 *  CVS Log :-
 *  $Id: LineHit.h,v 1.6 2008-04-14 15:09:10 krinnert Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-05-30
 */
//-----------------------------------------------------------------------------

#ifndef TFKERNEL_LINEHIT_H
#define TFKERNEL_LINEHIT_H 1

#include "TfKernel/HitBase.h"

// DetElems
#include "STDet/DeSTSector.h"
#include "OTDet/DeOTModule.h"

// Eventmodel
#include "Event/STLiteCluster.h"
#include "Event/OTLiteTime.h"

namespace Tf
{

  /** @class LineHit
   *
   *  Store a coordinate from tracking systems which
   *  can be represented by a line segment. The line is parameterized
   *  along its y-coordinate.
   *
   *  @authors S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   **/

  class LineHit : public HitBase
  {

  public: // typedefs

    typedef Gaudi::XYZPointF Point;    ///< Type for point-like class
    typedef Gaudi::XYZVectorF Vector;  ///< Type for vector-like class
    typedef line_hit_tag hit_type_tag; ///< the hit type tag

  public:

    // Simple accessors to internal data members

    /** Access the start point y coordinate for this LineHit */
    inline float yBegin()  const { return m_ybegin ; }

    /** Access the end point y coordinate for this LineHit */
    inline float yEnd()    const { return m_yend ; }

    /** Access the value of global x coordinate at the point y=0 for this LineHit */
    inline float xAtYEq0() const { return m_xAtYEq0 ; }

    /** Access the value of global z coordinate at the point y=0 for this LineHit */
    inline float zAtYEq0() const { return m_zAtYEq0 ; }

    /** Access the value of dx/dy  for this LineHit */
    inline float dxDy()    const { return m_dxdy ; }

    /** Access the value of dz/dy  for this LineHit */
    inline float dzDy()    const { return m_dzdy ; }

    /** x-coordinate in the rotated system (XXX???XXX what does rotated mean ???) */
    inline float xT()   const { return coord() ; }

    /** x coordinate of the line as function of y */
    inline float x(const float globalY = 0) const { return m_xAtYEq0 + globalY*m_dxdy ; }

    /** z coordinate of the line as function of y */
    inline float z(const float globalY = 0) const { return m_zAtYEq0 + globalY*m_dzdy ; }

    /** y coordinate in middle of the line */
    inline float y() const { return (float)0.5*(yBegin()+yEnd()); }

    /** tan of stereo angle */
    inline float tanT() const { return -m_dxdy ; }

    /** sin of stereo angle */
    inline float sinT() const { return tanT()*cosT() ; }

    /** cos of stereo angle */
    inline float cosT() const { return coord()/m_xAtYEq0 ; }

    /** is this hit in an X layer */
    inline bool  isX () const { return (layer() == 0 || layer() == 3) ; }

    /** Minimum y coordinate of line endpoints */
    inline float yMin() const { return std::min(yBegin(),yEnd()) ; }

    /** Maximum y coordinate of line endpoints */
    inline float yMax() const { return std::max(yBegin(),yEnd()) ; }

    /** Minimum x coordinate of line endpoints */
    inline float xMin() const { return std::min(x(yBegin()),x(yEnd())) ; }

    /** Maximum x coordinate of line endpoints */
    inline float xMax() const { return std::max(x(yBegin()),x(yEnd())) ; }

    /** y coordinate of line mid point */
    inline float yMid() const { return (float)0.5*(yBegin()+yEnd()) ; }

    /** x coordinate of line mid point */
    inline float xMid() const { return x(yMid()) ; }

    /** z coordinate of line mid point */
    inline float zMid() const { return z(yMid()) ; }

    /** Check if the y value is compatible with line */
    inline bool isYCompatible ( const float y, const float tol ) const 
    {
      // which one is faster?
      return fabs( y - 0.5*(yBegin() + yEnd()) ) < 0.5*fabs(yBegin() - yEnd()) + tol ;
      //return yMin() - tol <= y && y <= yMax() + tol ;
    }

    /** Position for y=0. Note: This is not the begin point of the
        line. However, it is what is needed to make the
        intersection calls efficient 
    */
    inline Point beginPoint() const { return Point(m_xAtYEq0,0,m_zAtYEq0) ; }

    /** direction, normalized to its y coordinate */
    inline Vector direction() const { return Vector(m_dxdy,1,m_dzdy) ; }

    /** position for any value of y */
    inline Point position(const float globalY) const { return Point(x(globalY),globalY,z(globalY)) ; }

    /** length of the line */
    inline float length() const { return fabs( (yEnd()-yBegin())*sqrt(1 + m_dxdy*m_dxdy + m_dzdy*m_dzdy )) ; }

  protected:

    /** Constructor from an OT module and a raw OT hit (OTLiteTime) */
    LineHit(const DeOTModule& aModule, const LHCb::OTLiteTime& rawhit) ;

    /** Constructor from an ST sector and a raw ST cluster (STLiteCluster) */
    LineHit(const DeSTSector& aSector, const LHCb::STLiteCluster& clus ) ;

  private:

    float m_dxdy ;     ///< The dx/dy value
    float m_dzdy ;     ///< The dz/dy value
    float m_ybegin ;   ///< The y value at the start point of the line
    float m_yend ;     ///< The y value at the end point of the line
    float m_xAtYEq0 ;  ///< The value of x at the point y=0
    float m_zAtYEq0 ;  ///< The value of z at the point y=0

  };


  ////////////////////////////////////////////////////////////////////////////////////
  // Inline function definitions.
  ////////////////////////////////////////////////////////////////////////////////////

  inline LineHit::LineHit(const DeOTModule& aModule, const LHCb::OTLiteTime& rawhit)
    : HitBase ( LHCb::LHCbID ( rawhit.channel() ),
                RegionID     ( rawhit.channel() ) )
  {
    aModule.trajectory( rawhit.channel().straw(), m_dxdy, m_dzdy, m_xAtYEq0, m_zAtYEq0, m_ybegin, m_yend) ;
    setCoord( m_xAtYEq0 * aModule.cosAngle() ) ;
  }

  inline LineHit::LineHit( const DeSTSector& aSector, 
                           const LHCb::STLiteCluster& clus )
    : HitBase ( LHCb::LHCbID ( clus.channelID() ),
                RegionID     ( clus.channelID() ),
                0,
                aSector.pitch()*aSector.pitch()/12.0)  
  {
    aSector.trajectory( clus.channelID().strip(), 
                        (float)clus.interStripFraction(), 
                        m_dxdy, m_dzdy, m_xAtYEq0, m_zAtYEq0, m_ybegin, m_yend );
    setCoord( m_xAtYEq0 * aSector.cosAngle() ) ;
  }

}

#endif // TFKERNEL_LINEHIT_H
