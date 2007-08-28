
//-----------------------------------------------------------------------------
/** @file Region.h
 *
 *  Header file for track finding 'Region' objects
 *
 *  CVS Log :-
 *  $Id: Region.h,v 1.4 2007-08-28 12:03:58 jonrob Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-05-30
 */
//-----------------------------------------------------------------------------

#ifndef TFKERNEL_REGION_H
#define TFKERNEL_REGION_H 1

#include "TfKernel/RegionID.h"
#include "LoKi/Range.h"
#include "DetDesc/SolidBox.h"

namespace Tf
{

  /** @class EnvelopeBase TfKernel/Region.h
   *
   *  Base class for all templated Envelope types.
   *  Defines a 'box' region around a DetectorElement in the global frame.
   *
   *  Useful to be able to answer some basic questions in a type neutral way.
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01
   **/
  class EnvelopeBase
  {
  public:
    /// Default Constructor
    EnvelopeBase() : m_numelements(0), m_xmin(1), m_xmax(0), m_cosT(1), m_sinT(0) {}
    /// The minimum value of x (in global coordinates) for the region
    inline double xmin() const { return m_xmin ; }
    /// The minimum value of y (in global coordinates) for the region
    inline double ymin() const { return m_ymin ; }
    /// The minimum value of z (in global coordinates) for the region
    inline double zmin() const { return m_zmin ; }
    /// The maximum value of x (in global coordinates) for the region
    inline double xmax() const { return m_xmax ; }
    /// The maximum value of y (in global coordinates) for the region
    inline double ymax() const { return m_ymax ; }
    /// The maximum value of z (in global coordinates) for the region
    inline double zmax() const { return m_zmax ; }
    /// The minimum value of XXX???XXX (in global coordinates) for the region
    inline double xminT() const { return m_xminT ; }
    /// The maximum value of XXX???XXX (in global coordinates) for the region
    inline double xmaxT() const { return m_xmaxT ; }
    /// The average value of x (in global coordinates) for the region (half way between xmin() and xmax())
    inline double x() const { return 0.5*(xmin()+xmax()) ; }
    /// The average value of y (in global coordinates) for the region (half way between ymin() and ymax())
    inline double y() const { return 0.5*(ymin()+ymax()) ; }
    /// The average value of z (in global coordinates) for the region (half way between zmin() and zmax())
    inline double z() const { return 0.5*(zmin()+zmax()) ; }
    /// The value of XXX???XXX (in global coordinates) for the region
    inline double cosT() const { return m_cosT ; }
    /// The value of XXX???XXX (in global coordinates) for the region
    inline double sinT() const { return m_sinT ; }
    /** Check if a given y value is compatible (inside) the region, to within a given tolerance
     *  @param[in] y   The y value to check
     *  @param[in] tol The allowable tolerance on y
     *  @return boolean indicating if the given value of y is inside the region, to within the given tolerance
     *  @retval TRUE  y value is INSIDE
     *  @retval FALSE y value is OUTSIDE
     */
    inline bool isYCompatible( const double y, const double tol ) const { return ymin() - tol <= y && y <= ymax() + tol ; }
    /** Check if a given x value is compatible (inside) the region, to within a given tolerance
     *  @param[in] x   The x value to check
     *  @param[in] tol The allowable tolerance on x
     *  @return boolean indicating if the given value of x is inside the region, to within the given tolerance
     *  @retval TRUE  x value is INSIDE
     *  @retval FALSE x value is OUTSIDE
     */
    inline bool isXCompatible( const double x, const double tol ) const { return xmin() - tol <= x && x <= xmax() + tol ; }
    /** Check if a given z value is compatible (inside) the region, to within a given tolerance
     *  @param[in] z   The y value to check
     *  @param[in] tol The allowable tolerance on z
     *  @return boolean indicating if the given value of z is inside the region, to within the given tolerance
     *  @retval TRUE  z value is INSIDE
     *  @retval FALSE z value is OUTSIDE
     */
    inline bool isZCompatible( const double z, const double tol ) const { return zmin() - tol <= z && z <= zmax() + tol ; }
    /// XXX???XXX Not sure at all what this is
    inline double xLocal(const double globalX, const double globalY) const { return globalX*m_cosT + globalY*m_sinT ; }
    /// XXX???XXX Not sure at all what this is
    inline double yLocal(const double globalX, const double globalY) const { return globalX*m_cosT - globalY*m_sinT ; }
  protected:
    unsigned int m_numelements ; ///< The number of elements
    double m_xmin ; ///< The min x value
    double m_xmax ; ///< The max x value
    double m_ymin ; ///< The min y value
    double m_ymax ; ///< The max y value
    double m_zmin ; ///< The min z value
    double m_zmax ; ///< The max z value
    double m_xminT ;
    double m_xmaxT ;
    double m_yminT ;
    double m_ymaxT ;
    double m_cosT ;
    double m_sinT ;
  } ;

  /** @class Envelope TfKernel/Region.h
   *  Defines a box around a DetectorElement in the global frame.
   *  Thisll needs to be made aware of alignment changes.
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01
   **/
  template < class DetectorElementType >
  class Envelope : public EnvelopeBase
  {
  public:
    /// Default Constructor
    Envelope() : EnvelopeBase() {}
    /** Constructor from a DetectorElementType */
    Envelope(const DetectorElementType& element) ;
    /** Add a envelope to the region
     *  @param[in] daughter Envelope to add
     */
    void add(const Envelope& daughter) ;
  } ;

  template<class DetectorElementType>
  Envelope<DetectorElementType>::Envelope(const DetectorElementType& detelement)
    : EnvelopeBase()
  {
    m_numelements = 1;
    const ISolid* solid = detelement.geometry()->lvolume()->solid();
    const SolidBox* mainBox = dynamic_cast<const SolidBox*>(solid);
    if(!mainBox) throw std::runtime_error("Tf::HitCreatorGeom::Envelope: wrong type of volume") ;
    double xsize = mainBox->xsize();
    double ysize = mainBox->ysize();
    double zsize = mainBox->zsize();
    m_cosT = detelement.cosAngle() ;
    m_sinT = detelement.sinAngle() ;

    // Should be able to do this a bit smarter, but OK, too tired
    // now.
    bool first=true ;
    for( int ix=0; ix<2; ++ix)
      for(int iy=0; iy<2; ++iy)
        for(int iz=0; iz<2; ++iz) {
          Gaudi::XYZPoint p = detelement.globalPoint((-0.5+ix)*xsize,(-0.5+iy)*ysize,(-0.5+iz)*zsize) ;
          double xT = xLocal(p.x(),p.y()) ;
          double yT = yLocal(p.x(),p.y()) ;
          if(first) {
            m_xmin = m_xmax = p.x() ;
            m_ymin = m_ymax = p.y() ;
            m_zmin = m_zmax = p.z() ;
            m_xminT = m_xmaxT = xT ;
            m_yminT = m_ymaxT = yT ;
          } else {
            m_xmin = std::min( m_xmin, p.x() ) ;
            m_ymin = std::min( m_ymin, p.y() ) ;
            m_zmin = std::min( m_zmin, p.z() ) ;
            m_xmax = std::max( m_xmax, p.x() ) ;
            m_ymax = std::max( m_ymax, p.y() ) ;
            m_zmax = std::max( m_zmax, p.z() ) ;
            m_xminT = std::min( m_xminT, xT ) ;
            m_yminT = std::min( m_yminT, yT ) ;
            m_xmaxT = std::max( m_xmaxT, xT ) ;
            m_ymaxT = std::max( m_ymaxT, yT ) ;
          }
          first = false ;
        }
  }

  template<class DetectorElementType>
  void Envelope<DetectorElementType>::add(const Envelope& element)
  {
    if( m_numelements==0 ) {
      *this = element ;
    } else {
      m_xmin = std::min( m_xmin, element.m_xmin ) ;
      m_ymin = std::min( m_ymin, element.m_ymin ) ;
      m_zmin = std::min( m_zmin, element.m_zmin ) ;
      m_xmax = std::max( m_xmax, element.m_xmax ) ;
      m_ymax = std::max( m_ymax, element.m_ymax ) ;
      m_zmax = std::max( m_zmax, element.m_zmax ) ;
      m_xminT = std::min( m_xminT, element.m_xminT ) ;
      m_yminT = std::min( m_yminT, element.m_yminT ) ;
      m_xmaxT = std::max( m_xmaxT, element.m_xmaxT  ) ;
      m_ymaxT = std::max( m_ymaxT, element.m_ymaxT ) ;
      m_sinT  = (m_sinT*m_numelements + element.m_sinT)/(m_numelements+1) ;
      m_cosT  = sqrt(1-m_sinT*m_sinT) ;
      ++m_numelements ;
    }
  }

  /** @class Region TfKernel/Region.h
   *  Collection of DetectorElements useful for pattern recognition
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01
   **/

  template<class HitTypeT>
  class Region : public Envelope<typename HitTypeT::DetectorElementType>
  {
  public:
    virtual ~Region() {}
    typedef HitTypeT HitType ;
    typedef std::vector<const HitTypeT*> HitContainer ; ///< Type for container of pointers to hits
    typedef LoKi::Range_<HitContainer> HitRangeType ;   ///< Type for a range of elements from a HitContainer
    //const HitRangeType& hits() const { return m_hitrange ; }
    /// Access the all hits for this region
    virtual HitRangeType hits() const = 0 ;
    /** Access all the hits within a given region of x
     *  @param xmin the minimum x value
     *  @param xmax the maximum x value
     *  @return Range object covering the hits in the given x range
     */
    virtual HitRangeType hits(float xmin, float xmax) const = 0 ;
    /** Access all the hits within a given region of x and y
     *  @param xmin the minimum x value
     *  @param xmax the maximum x value
     *  @param ymin the minimum y value
     *  @param ymax the maximum y value
     *  @return Range object covering the hits in the given x and y range
     */
    virtual HitRangeType hits(float xmin, float xmax, float ymin, float ymax) const = 0 ;

    /// Returns the RegionID object for this region
    RegionID id() const { return m_id ; }

  protected:
    /// Constructor from a RegionID object
    Region( const RegionID& id ) : m_id(id) {}
    //void setHitRange( const HitRangeType& r ) { m_hitrange = r ; }

  private:
    RegionID m_id ; ///< The regionID object for this region
    //HitRangeType m_hitrange ;
  } ;

}

#endif // TFKERNEL_REGION_H
