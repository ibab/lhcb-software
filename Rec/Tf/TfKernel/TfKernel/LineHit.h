
//-----------------------------------------------------------------------------
/** @file LineHit.h
 *
 *  Header file for track find class Tf::LineHit
 *
 *  CVS Log :-
 *  $Id: LineHit.h,v 1.12 2010-03-16 08:24:31 smenzeme Exp $
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
    inline double yBegin()  const noexcept { return m_ybegin ; }

    /** Access the end point y coordinate for this LineHit */
    inline double yEnd()    const noexcept { return m_yend ; }

    /** Access the value of global x coordinate at the point y=0 for this LineHit */
    inline double xAtYEq0() const noexcept { return m_xAtYEq0 ; }

    /** Access the value of global z coordinate at the point y=0 for this LineHit */
    inline double zAtYEq0() const noexcept { return m_zAtYEq0 ; }

    /** Setter for value of global x coordinate at the point y=0 for this LineHit */
    inline void setXAtYEq0( const double xAtYEq0 ) const noexcept { m_xAtYEq0 = xAtYEq0 ; }

    /** Setter for value of global z coordinate at the point y=0 for this LineHit */
    inline void setZAtYEq0( const double zAtYEq0) const noexcept { m_zAtYEq0 = zAtYEq0; }

    /** Access the value of dx/dy  for this LineHit */
    inline double dxDy()    const noexcept { return m_dxdy ; }

    /** Access the value of dz/dy  for this LineHit */
    inline double dzDy()    const noexcept { return m_dzdy ; }

    /** x-coordinate in the rotated system (XXX???XXX what does rotated mean ???) */
    inline double xT()   const noexcept { return coord() ; }

    /** x coordinate of the line as function of y */
    inline double x(const double globalY = 0.) const noexcept { return m_xAtYEq0 + globalY*m_dxdy ; }

    /** z coordinate of the line as function of y */
    inline double z(const double globalY = 0.) const noexcept { return m_zAtYEq0 + globalY*m_dzdy ; }

    /** y coordinate in middle of the line */
    inline double y() const noexcept { return 0.5*(yBegin()+yEnd()); }

    /** tan of stereo angle */
    inline double tanT() const noexcept { return -m_dxdy ; }

    /** sin of stereo angle */
    inline double sinT() const noexcept { return tanT()*cosT() ; }

    /** cos of stereo angle */
    inline double cosT() const noexcept { 
	if (fabs(m_xAtYEq0) < 1.0E-9)
	    return 1 / std::sqrt(1 + m_dxdy * m_dxdy); //cos(atan(-m_dxdy));
	return coord()/m_xAtYEq0 ; 
    }

      /** Minimum y coordinate of line endpoints */
    inline double yMin() const noexcept { return std::min(yBegin(),yEnd()) ; }

    /** Maximum y coordinate of line endpoints */
    inline double yMax() const noexcept { return std::max(yBegin(),yEnd()) ; }

    /** Minimum x coordinate of line endpoints */
    inline double xMin() const noexcept { return std::min(x(yBegin()),x(yEnd())) ; }

    /** Maximum x coordinate of line endpoints */
    inline double xMax() const noexcept { return std::max(x(yBegin()),x(yEnd())) ; }

    /** y coordinate of line mid point */
    inline double yMid() const noexcept { return 0.5*(yBegin()+yEnd()) ; }

    /** x coordinate of line mid point */
    inline double xMid() const noexcept { return x(yMid()) ; }

    /** z coordinate of line mid point */
    inline double zMid() const noexcept { return z(yMid()) ; }

    /** Check if the y value is compatible with line */
    inline bool isYCompatible ( const double y, const double tol ) const noexcept
    {
      return yMin() - tol <= y && y <= yMax() + tol ;
    }

    /** Position for y=0. Note: This is not the begin point of the
        line. However, it is what is needed to make the
        intersection calls efficient 
    */
    inline Point beginPoint() const noexcept { return Point((float)m_xAtYEq0,0.f,(float)m_zAtYEq0) ; }

    /** direction, normalized to its y coordinate */
    inline Vector direction() const noexcept { return Vector((float)m_dxdy,1.f,(float)m_dzdy) ; }

    /** position for any value of y */
    inline Point position(const double globalY) const noexcept { return Point((float)x(globalY),(float)globalY,(float)z(globalY)) ; }

    /** length of the line */
    inline double length() const noexcept { return std::abs(yEnd()-yBegin())*std::sqrt(1 + m_dxdy*m_dxdy + m_dzdy*m_dzdy ) ; }

    inline bool isX() const noexcept { if (type()==RegionID::TT || type()==RegionID::UT) return (layer()==station());
    else return !((layer() + 1u) & 2u);}

  protected:

    /** Constructor from an OT module and a raw OT hit (OTLiteTime) */
    LineHit(const DeOTModule& aModule, const LHCb::OTLiteTime& rawhit, const double error) ;

    /** Constructor from an ST sector and a raw ST cluster (STLiteCluster) */
    LineHit(const DeSTSector& aSector, const LHCb::STLiteCluster& clus ) ;

  private:

    double m_dxdy ;     ///< The dx/dy value
    double m_dzdy ;     ///< The dz/dy value
    double m_ybegin ;   ///< The y value at the start point of the line
    double m_yend ;     ///< The y value at the end point of the line
    mutable double m_xAtYEq0 ;  ///< The value of x at the point y=0
    mutable double m_zAtYEq0 ;  ///< The value of z at the point y=0

  };


  ////////////////////////////////////////////////////////////////////////////////////
  // Inline function definitions.
  ////////////////////////////////////////////////////////////////////////////////////

  inline LineHit::LineHit(const DeOTModule& aModule, const LHCb::OTLiteTime& rawhit,
	  const double error = 4.9 / std::sqrt(12.))
    : HitBase ( LHCb::LHCbID ( rawhit.channel() ),
                RegionID     ( rawhit.channel() ), 0, error)
  {

    aModule.trajectory( rawhit.channel().straw(), m_dxdy, m_dzdy, m_xAtYEq0, m_zAtYEq0, m_ybegin, m_yend) ;
    setCoord( m_xAtYEq0 * aModule.cosAngle() ) ;
  }

  inline LineHit::LineHit( const DeSTSector& aSector, 
                           const LHCb::STLiteCluster& clus )
    : HitBase ( LHCb::LHCbID ( clus.channelID() ),
                RegionID     ( clus.channelID() ),
                0,
                aSector.pitch()/std::sqrt(12.0))  
  {
    aSector.trajectory( clus.channelID().strip(), 
                        clus.interStripFraction(), 
                        m_dxdy, m_dzdy, m_xAtYEq0, m_zAtYEq0, m_ybegin, m_yend );
    setCoord( m_xAtYEq0 * aSector.cosAngle() ) ;
  }

}

#endif // TFKERNEL_LINEHIT_H
