#ifndef TF_TFDATA_REGION_H
#define TF_TFDATA_REGION_H

#include "TfKernel/RegionID.h"
#include "LoKi/Range.h"
#include "DetDesc/SolidBox.h"

namespace Tf
{

  /** @class EnvelopeBase
   *  Base class for all templated Envelope types.
   *
   *  Useful to be able to answer some basic questions in a type neutral way.
   *
   *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01
   **/

  class EnvelopeBase
  {
  public:
    EnvelopeBase() : m_numelements(0), m_xmin(1), m_xmax(0), m_cosT(1), m_sinT(0) {}
    double xmin() const { return m_xmin ; }
    double ymin() const { return m_ymin ; }
    double zmin() const { return m_zmin ; }
    double xmax() const { return m_xmax ; }
    double ymax() const { return m_ymax ; }
    double zmax() const { return m_zmax ; }
    double xminT() const { return m_xminT ; }
    double xmaxT() const { return m_xmaxT ; }
    double x() const { return 0.5*(xmin()+xmax()) ; }
    double y() const { return 0.5*(ymin()+ymax()) ; }
    double z() const { return 0.5*(zmin()+zmax()) ; }
    double cosT() const { return m_cosT ; }
    double sinT() const { return m_sinT ; }
    bool isYCompatible( double y, double tol ) const { return ymin() - tol <= y && y <= ymax() + tol ; }
    bool isXCompatible( double x, double tol ) const { return xmin() - tol <= x && x <= xmax() + tol ; }
    bool isZCompatible( double z, double tol ) const { return zmin() - tol <= z && z <= zmax() + tol ; }
    double xLocal(double globalX, double globalY) const { return globalX*m_cosT + globalY*m_sinT ; }
    double yLocal(double globalX, double globalY) const { return globalX*m_cosT - globalY*m_sinT ; }
  protected:
    unsigned int m_numelements ;
    double m_xmin ;
    double m_xmax ;
    double m_ymin ;
    double m_ymax ;
    double m_zmin ;
    double m_zmax ;
    double m_xminT ;
    double m_xmaxT ;
    double m_yminT ;
    double m_ymaxT ;
    double m_cosT ;
    double m_sinT ;
  } ;

  /** @class Envelope
   *  Defines a box around a DetectorElement in the global frame.
   *  Thisll needs to be made aware of alignment changes.
   *
   *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01
   **/
  template < class DetectorElementType >
  class Envelope : public EnvelopeBase
  {
  public:
    Envelope() : EnvelopeBase() {}
    Envelope(const DetectorElementType& element) ;
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
    float xsize = mainBox->xsize();
    float ysize = mainBox->ysize();
    float zsize = mainBox->zsize();
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

  /** @class Region
   *  Collection of DetectorElements useful for pattern recognition
   *
   *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01
   **/
  
  template<class HitTypeT>
  class Region : public Envelope<typename HitTypeT::DetectorElementType>
  {
  public:
    virtual ~Region() {}
    typedef HitTypeT HitType ;
    typedef std::vector<const HitTypeT*> HitContainer ;
    typedef LoKi::Range_<HitContainer> HitRangeType ;
    //const HitRangeType& hits() const { return m_hitrange ; }
    virtual HitRangeType hits() const = 0 ;
    virtual HitRangeType hits(float xmin, float xmax) const = 0 ;
    virtual HitRangeType hits(float xmin, float xmax, float ymin, float ymax) const = 0 ;

    RegionID id() const { return m_id ; }

  protected:
    Region( const RegionID& id ) : m_id(id) {}
    //void setHitRange( const HitRangeType& r ) { m_hitrange = r ; }

  private:
    RegionID m_id ;
    //HitRangeType m_hitrange ;
  } ;

}

#endif
