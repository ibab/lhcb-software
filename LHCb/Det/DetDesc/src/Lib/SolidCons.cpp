// ===========================================================================
// Units
#include "GaudiKernel/SystemOfUnits.h"
// GaudiKernel
#include "GaudiKernel/MsgStream.h"
// DetDesc
#include "DetDesc/DetDesc.h"
#include "DetDesc/SolidCons.h"
#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidTrd.h"
#include "DetDesc/SolidTicks.h"
#include "DetDesc/SolidException.h"

#include "boost/container/static_vector.hpp"
// ============================================================================
/** @file SolidCons.cpp
 *
 *  Implementation of class SolidCons
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date xx/xx/xxxx
 */
// ============================================================================

// ============================================================================
/** standard constructor
 *  @param name                  name of conical tube segment
 *  @param ZHalfLength           half-length
 *  @param OuterRadiusAtMinusZ   outer radius at minus Z
 *  @param OuterRadiusAtPlusZ    outer radius at plus  Z
 *  @param InnerRadiusAtMinusZ   inner radius at minus Z
 *  @param InnerRadiusAtPlusZ    inner radius at plus  Z
 *  @param StartPhiAngle         start phi angle
 *  @param DeltaPhiAngle         delta phi angle
 *  @param CoverModel            covering model
 *  @exception SolidException    wrong parameters range
 */
// ============================================================================
SolidCons::SolidCons( const std::string & name  ,
                      double ZHalfLength        ,
                      double OuterRadiusMinusZ  ,
                      double OuterRadiusPlusZ   ,
                      double InnerRadiusMinusZ  ,
                      double InnerRadiusPlusZ   ,
                      double StartPhiAngle      ,
                      double DeltaPhiAngle      ,
                      int    CoverModel         )
  : SolidBase                ( name               )
  , m_cons_zHalfLength       ( ZHalfLength        )
  , m_cons_outerRadiusMinusZ ( OuterRadiusMinusZ  )
  , m_cons_outerRadiusPlusZ  ( OuterRadiusPlusZ   )
  , m_cons_innerRadiusMinusZ ( InnerRadiusMinusZ  )
  , m_cons_innerRadiusPlusZ  ( InnerRadiusPlusZ   )
  , m_cons_startPhiAngle     ( StartPhiAngle      )
  , m_cons_deltaPhiAngle     ( DeltaPhiAngle      )
  , m_cons_coverModel        ( CoverModel         )
{
  ///
  if( 0 >= zHalfLength()                                )
    { throw SolidException("SolidCons ::ZHalfLength       is not positive!");}
  if( 0 >= outerRadiusAtMinusZ()                        )
    { throw SolidException("SolidCons ::OuterRadiusMinusZ is not positive!");}
  if( 0 >= outerRadiusAtPlusZ()                         )
    { throw SolidException("SolidCons ::OuterRadiusPlusZ  is not positive!");}
  if( 0 >  innerRadiusAtMinusZ()                        )
    { throw SolidException("SolidCons ::InnerRadiusMinusZ is negative !   ");}
  if( innerRadiusAtMinusZ() >= outerRadiusAtMinusZ()    )
    { throw SolidException("SolidCons:InnerRadiusMinusZ>=OuterRadiusMinusZ!");}
  if( 0 >  innerRadiusAtPlusZ()                         )
    { throw SolidException("SolidCons ::InnerRadiusPlusZ  is negative !   ");}
  if( innerRadiusAtPlusZ() >= outerRadiusAtPlusZ()      )
    { throw SolidException("SolidCons::InnerRadiusPlusZ>=OuterRadiusPlusZ!");}
  if( -180.0 * Gaudi::Units::degree > startPhiAngle()                 )
    { throw SolidException("SolidCons::StartPhiAngle <-pi!");}
  if(  360.0 * Gaudi::Units::degree < startPhiAngle()                 )
    { throw SolidException("SolidCons ::StartPhiAngle >2pi degree!");}
  if(    0.0 * Gaudi::Units::degree > deltaPhiAngle()                 )
    { throw SolidException("SolidCons ::DeltaPhiAngle < 0 degree!"    );}
  if(  360.0 * Gaudi::Units::degree < startPhiAngle()+deltaPhiAngle() )
    { throw SolidException("SolidCons ::StartPhiAngle+DeltaPhiAngle>2pi");}
  //
  m_noPhiGap = true ;
  if(   0 * Gaudi::Units::degree != startPhiAngle () ) { m_noPhiGap = false ; }
  if( 360 * Gaudi::Units::degree != deltaPhiAngle () ) { m_noPhiGap = false ; }

  // set bounding parameters
  setBP();
  //
}

// ============================================================================
/// destructor
// ============================================================================
SolidCons::~SolidCons() = default;

// ============================================================================
/// set parameters for bounding solids (box, sphere and cylinder)
// ============================================================================
void SolidCons::setBP()
{
  // set bounding parameters of SolidBase class
  setZMin   ( -zHalfLength() );
  setZMax   (  zHalfLength() );
  setRhoMax (  outerRadiusAtMinusZ() > outerRadiusAtPlusZ() ?
               outerRadiusAtMinusZ() : outerRadiusAtPlusZ() );
  setRMax   ( sqrt( zMax() * zMax() + rhoMax() * rhoMax () ) );

  typedef std::vector<double> Values ;

  const double phi1   = startPhiAngle   ()                      ;
  const double phi2   = startPhiAngle   () + deltaPhiAngle   () ;

  const double rhoMin =
    innerRadiusAtMinusZ() < innerRadiusAtPlusZ()  ?
    innerRadiusAtMinusZ() : innerRadiusAtPlusZ()  ;

  { // evaluate xmin & xmax
    Values values ; values.reserve(12);

    // regular cases
    values.push_back( rhoMax () * cos ( phi1 ) );
    values.push_back( rhoMax () * cos ( phi2 ) );
    values.push_back( rhoMin    * cos ( phi1 ) );
    values.push_back( rhoMin    * cos ( phi2 ) );

    // special cases
    if( phi1 <=    0*Gaudi::Units::degree &&    0*Gaudi::Units::degree <= phi2 )
      {
        values.push_back (   rhoMax () ) ;
        values.push_back (   rhoMin    )  ;
      }
    if( phi1 <=  360*Gaudi::Units::degree &&  360*Gaudi::Units::degree <= phi2 )
      {
        values.push_back (   rhoMax () ) ;
        values.push_back (   rhoMin    ) ;
      }

    // special cases
    if( phi1 <=  180*Gaudi::Units::degree &&  180*Gaudi::Units::degree <= phi2 )
      {
        values.push_back ( - rhoMax () ) ;
        values.push_back ( - rhoMin    ) ;
      }
    if( phi1 <= -180*Gaudi::Units::degree && -180*Gaudi::Units::degree <= phi2 )
      {
        values.push_back ( - rhoMax () ) ;
        values.push_back ( - rhoMin    ) ;
      }

    setXMax ( *std::max_element ( values.begin () , values.end () ) ) ;
    setXMin ( *std::min_element ( values.begin () , values.end () ) ) ;

  }

  { // evaluate ymin & ymax
    Values values ; values.reserve(10);

    // regular cases
    values.push_back( rhoMax () * sin ( phi1 ) );
    values.push_back( rhoMax () * sin ( phi2 ) );
    values.push_back( rhoMin    * sin ( phi1 ) );
    values.push_back( rhoMin    * sin ( phi2 ) );

    // special cases
    if( phi1 <=   90*Gaudi::Units::degree &&   90*Gaudi::Units::degree <= phi2 )
      {
        values.push_back (   rhoMax () ) ;
        values.push_back (   rhoMin    ) ;
      }

    // special cases
    if( phi1 <=  -90*Gaudi::Units::degree &&  -90*Gaudi::Units::degree <= phi2 )
      {
        values.push_back ( - rhoMax () ) ;
        values.push_back ( - rhoMin    ) ;
      }
    if( phi1 <=  270*Gaudi::Units::degree &&  270*Gaudi::Units::degree <= phi2 )
      {
        values.push_back ( - rhoMax () ) ;
        values.push_back ( - rhoMin    ) ;
      }

    setYMax ( *std::max_element ( values.begin () , values.end () ) ) ;
    setYMin ( *std::min_element ( values.begin () , values.end () ) ) ;

  }

  // check bounding parameters
  checkBP() ;
}
// ============================================================================

// ============================================================================
/** default protected  coinstructor
 *  @param Name name of conical tube segment
 */
// ============================================================================
SolidCons::SolidCons( const std::string& Name )
  ///
  : SolidBase                ( Name                )
  , m_cons_zHalfLength       ( 1000000             )
  , m_cons_outerRadiusMinusZ ( 1000000             )
  , m_cons_outerRadiusPlusZ  ( 1000000             )
  , m_cons_innerRadiusMinusZ ( 0                   )
  , m_cons_innerRadiusPlusZ  ( 0                   )
  , m_cons_startPhiAngle     ( 0                   )
  , m_cons_deltaPhiAngle     ( 360 * Gaudi::Units::degree )
  , m_cons_coverModel        ( 0                   )
  , m_noPhiGap               ( true                )
{}
// ============================================================================

// ============================================================================
/** check for the given point (local frame)
 *  @param point point (in local reference system)
 *  @return true ifpoint is inside the solid
 */
// ============================================================================
bool SolidCons::isInside (  const Gaudi::XYZPoint& point ) const
{
  return isInsideImpl(point) ;
}
// ============================================================================
bool SolidCons::isInside ( const Gaudi::Polar3DPoint& point) const
{
  return isInsideImpl(point);
}
// ============================================================================
bool SolidCons::isInside ( const Gaudi::RhoZPhiPoint& point) const
{
  return isInsideImpl(point);
}
// ============================================================================
template<class aPoint>
bool SolidCons::isInsideImpl(  const aPoint& point ) const
{
  if(  isOutBBox ( point )  ) { return false ; }
  // check for phi
  if( !insidePhi ( point )  ) { return false ; }
  // check for radius
  const double rho2 = point.perp2();
  const double oR   = oR_z( point.z() ) ;
  if( rho2 > oR * oR        ) { return false ; }
  const double iR   = iR_z( point.z() ) ;
  if( rho2 < iR * iR        ) { return false ; }
  //
  return true ;
}
// ============================================================================

// ============================================================================/
/** -# retrieve the pointer to "simplified" solid - "cover"
 *    - for Model = 0
 *        -# the cover for the general conical tube segment is
 *           the conical tube
 *        -# the cover for the conical tube is conical cylinder
 *        -# the cover for the conical cylinder is TRD
 *    - for Model != 0
 *        -# the cover for conical tube serment is conical cylinder segment
 *        -# the cover for conical cylinder segment is conical cylinder
 *        -# the cover for conical cylinder is TRD
 *  -# implementation of ISolid abstract interface
 *  @see ISolid
 *  @return pointer to "simplified" solid - "cover"
 */
// ============================================================================
const ISolid* SolidCons::cover () const
{
  /// cover is calculated already
  if( m_cover ) { return m_cover; }
  //
  ISolid* cov = nullptr ;
  if( 0 == m_cons_coverModel )
    {
      // cover for conical tube segment is conical tube
      if (   0.0   * Gaudi::Units::degree  != startPhiAngle  () ||
             360.0 * Gaudi::Units::degree  != deltaPhiAngle  () )
        { cov =
            new SolidCons  ("Cover for " + name     () ,
                            zHalfLength             () ,
                            outerRadiusAtMinusZ     () ,
                            outerRadiusAtPlusZ      () ,
                            innerRadiusAtMinusZ     () ,
                            innerRadiusAtPlusZ      () ); }
      // cover for conical tube is "conical cylinder"
      else if ( 0.0 != innerRadiusAtMinusZ ()  ||
                0.0 != innerRadiusAtPlusZ  ()  )
        { cov =
            new SolidCons  ("Cover for " + name     () ,
                            zHalfLength             () ,
                            outerRadiusAtMinusZ     () ,
                            outerRadiusAtPlusZ      () );}
      // cover for "conical cylinder" is TRD
      else
        { cov =
            new SolidTrd  ("Cover for " + name     () ,
                           zHalfLength             () ,
                           outerRadiusAtMinusZ     () ,
                           outerRadiusAtMinusZ     () ,
                           outerRadiusAtPlusZ      () ,
                           outerRadiusAtPlusZ      () ); }
    }
  else
    {
      /// cover for conical tube segment is conical cylinder segment
      if ( 0.0 != innerRadiusAtMinusZ() ||
           0.0 != innerRadiusAtPlusZ () )
        { cov =
            new SolidCons  ("Cover for " + name     () ,
                            zHalfLength             () ,
                            outerRadiusAtMinusZ     () ,
                            outerRadiusAtPlusZ      () ,
                            0.0 * Gaudi::Units::mm     ,
                            0.0 * Gaudi::Units::mm     ,
                            startPhiAngle           () ,
                            deltaPhiAngle           () ,
                            m_cons_coverModel          );}
      /// cover for conical cylinder segment is conical cylinder
      else if ( 0.0   * Gaudi::Units::degree  != startPhiAngle  () ||
                360.0 * Gaudi::Units::degree  != deltaPhiAngle  () )
        { cov =
            new SolidCons  ("Cover for " + name     () ,
                            zHalfLength             () ,
                            outerRadiusAtMinusZ     () ,
                            outerRadiusAtPlusZ      () ,
                            innerRadiusAtMinusZ     () ,
                            innerRadiusAtPlusZ      () ,
                            0.0 * Gaudi::Units::degree ,
                            360.0*Gaudi::Units::degree ,
                            m_cons_coverModel          ); }
      /// cover for "conical cylinder" is TRD
      else
        { cov =
            new SolidTrd  ("Cover for " + name     () ,
                           zHalfLength             () ,
                           outerRadiusAtMinusZ     () ,
                           outerRadiusAtMinusZ     () ,
                           outerRadiusAtPlusZ      () ,
                           outerRadiusAtPlusZ      () ); }
    }
  ///
  if( !cov ) { return this; }
  m_cover = cov ;
  return m_cover ;
}

// ============================================================================
/** calculate the intersection points("ticks") with a given line.
 *  Input - line, paramterised by  x_vect = Point + Vector * T
 *  "tick" is just a value of T, at which the intersection occurs
 *  @param Point initial point for the line
 *  @param Vector vector along the line
 *  @param ticks output container of "Ticks"
 *  @return the number of intersection points (=size of Ticks container)
 */
// ============================================================================
unsigned int SolidCons::intersectionTicks( const Gaudi::XYZPoint&  Point,
                                           const Gaudi::XYZVector& Vector,
                                           ISolid::Ticks&          ticks ) const
{
  return intersectionTicksImpl(Point, Vector, ticks);
}
// ============================================================================
unsigned int SolidCons::intersectionTicks( const Gaudi::Polar3DPoint&  Point,
                                           const Gaudi::Polar3DVector& Vector,
                                           ISolid::Ticks&              ticks ) const
{
  return intersectionTicksImpl(Point, Vector, ticks);
}
// ============================================================================
unsigned int SolidCons::intersectionTicks( const Gaudi::RhoZPhiPoint&  Point,
                                           const Gaudi::RhoZPhiVector& Vector,
                                           ISolid::Ticks&              ticks ) const
{
  return intersectionTicksImpl(Point, Vector, ticks);
}
// ============================================================================
template<class aPoint, class aVector>
unsigned int SolidCons::intersectionTicksImpl( const aPoint & Point,
                                               const aVector& Vector,
                                               ISolid::Ticks& ticks  ) const
{
  // Clear the tick vector
  ticks.clear();

  // line with null direction vector is not able to intersect any solid
  if( Vector.mag2() <= 0 )  { return 0 ;}  ///< RETURN!!!

  // cross bounding cylinder ?
  if( !crossBCylinder( Point , Vector ) ) { return 0 ; }

  // intersect with first z-plane
  boost::container::static_vector<ISolid::Tick,6> tmpticks ;
  tmpticks.clear() ;
  if(SolidTicks::LineIntersectsTheZ( Point, Vector, -zHalfLength(), std::back_inserter( tmpticks ))) {
    double tick = tmpticks.front() ;
    double x = Point.x() + tick * Vector.x() ;
    double y = Point.y() + tick * Vector.y() ;
    double r = sqrt(x*x+y*y) ;
    if(innerRadiusAtMinusZ()<=r && r<=outerRadiusAtMinusZ() && (noPhiGap() || insidePhi( atan2(y,x) ) ) )
      ticks.push_back(tick) ;
  }

  // intersect with 2nd z-plane
  tmpticks.clear() ;
  if(SolidTicks::LineIntersectsTheZ( Point, Vector, zHalfLength(), std::back_inserter( tmpticks ))) {
    double tick = tmpticks.front() ;
    double x = Point.x() + tick * Vector.x() ;
    double y = Point.y() + tick * Vector.y() ;
    double r = sqrt(x*x+y*y) ;
    if(innerRadiusAtPlusZ()<=r && r<=outerRadiusAtPlusZ() && (noPhiGap() || insidePhi( atan2(y,x) ) ) )
      ticks.push_back(tick) ;
  }

  if( !noPhiGap() ) {
    // intersect with phi planes
    tmpticks.clear() ;
    SolidTicks::LineIntersectsThePhi( Point,Vector,startPhiAngle(), std::back_inserter( tmpticks ) );
    //if( deltaPhiAngle() != M_PI )
    SolidTicks::LineIntersectsThePhi( Point,Vector,startPhiAngle() + deltaPhiAngle(), std::back_inserter( tmpticks ));

    // check that we are anywhere inside this cylinder
    for( auto it = tmpticks.begin(); it != tmpticks.end(); ++it) {
      double zfrac = (Point.z() + *it * Vector.z())/zHalfLength() ;
      if( fabs(zfrac) <= 1 ) {
        double x = Point.x() + *it * Vector.x() ;
        double y = Point.y() + *it * Vector.y() ;
        double r = sqrt(x*x+y*y) ;
        if( r >= 0.5*( (1-zfrac)*innerRadiusAtMinusZ() + (1+zfrac)*innerRadiusAtMinusZ()) &&
            r<=  0.5*( (1-zfrac)*outerRadiusAtMinusZ() + (1+zfrac)*outerRadiusAtMinusZ()) )
          ticks.push_back(*it) ;
      }
    }
  }

  // intersect with outer conical surface
  tmpticks.clear() ;
  SolidTicks::LineIntersectsTheCone( Point, Vector, outerRadiusAtMinusZ(), outerRadiusAtPlusZ (),
				     -zHalfLength(), zHalfLength(), std::back_inserter( tmpticks ));
  // intersect with inner conical surface
  if( ( 0 < innerRadiusAtPlusZ() ) || ( 0 < innerRadiusAtMinusZ() )  )
    SolidTicks::LineIntersectsTheCone( Point, Vector, innerRadiusAtMinusZ(), innerRadiusAtPlusZ (),
				       -zHalfLength(), zHalfLength(), std::back_inserter( tmpticks ) );

  // check that we are in the right z and phi range
  for( auto it = tmpticks.begin(); it != tmpticks.end(); ++it)
    if( fabs(Point.z() + *it * Vector.z()) <= zHalfLength() &&
	(noPhiGap() ||
	 insidePhi(atan2( Point.y() + *it * Vector.y(), Point.x() + *it * Vector.x())) ) )
      ticks.push_back(*it) ;

  std::sort(ticks.begin(),ticks.end()) ;
  return SolidTicks::RemoveAdjacentTicksFast(ticks , Point , Vector , *this );
}

// ============================================================================
/** printout to STD/STL stream
 *  @param os STD/STL stream
 *  @return reference to the stream
 */
// ============================================================================
std::ostream&  SolidCons::printOut      ( std::ostream&  os ) const
{
  /// serialize the base class
  SolidBase::printOut( os );
  /// serialize members
  os << "[" ;
  os << " sizeZ[mm]"          << DetDesc::print( zLength()             / Gaudi::Units::mm )
     << " outerRadiusPZ[mm]=" << DetDesc::print( outerRadiusAtPlusZ () / Gaudi::Units::mm )
     << " outerRadiusMZ[mm]=" << DetDesc::print( outerRadiusAtMinusZ() / Gaudi::Units::mm ) ;
  if( 0 < innerRadiusAtPlusZ() )
    { os << " innerRadiusPZ[mm]="
         << DetDesc::print( innerRadiusAtPlusZ () / Gaudi::Units::mm ) ; }
  if( 0 < innerRadiusAtMinusZ() )
    { os << " innerRadiusMZ[mm]="
         << DetDesc::print( innerRadiusAtMinusZ() / Gaudi::Units::mm ) ; }
  if(   0 * Gaudi::Units::degree != startPhiAngle() ||
      360 * Gaudi::Units::degree != deltaPhiAngle()  )
    {
      os << " startPhiAngle[deg]"
         << DetDesc::print( startPhiAngle() / Gaudi::Units::degree ) ;
      os << " deltaPhiAngle[deg]"
         << DetDesc::print( deltaPhiAngle() / Gaudi::Units::degree ) ;
    }
  return os << "]" << std::endl;
}

// ============================================================================
/** printout to Gaudi  stream
 *  @param os Gaudi stream
 *  @return reference to the stream
 */
// ============================================================================
MsgStream&     SolidCons::printOut      ( MsgStream&     os ) const
{
  /// serialize the base class
  SolidBase::printOut( os );
  /// serialize members
  os << "[" ;
  os << " sizeZ[mm]"          << DetDesc::print( zLength()             / Gaudi::Units::mm )
     << " outerRadiusPZ[mm]=" << DetDesc::print( outerRadiusAtPlusZ () / Gaudi::Units::mm )
     << " outerRadiusMZ[mm]=" << DetDesc::print( outerRadiusAtMinusZ() / Gaudi::Units::mm ) ;
  if( 0 < innerRadiusAtPlusZ() )
    { os << " innerRadiusPZ[mm]="
         << DetDesc::print( innerRadiusAtPlusZ () / Gaudi::Units::mm ) ; }
  if( 0 < innerRadiusAtMinusZ() )
    { os << " innerRadiusMZ[mm]="
         << DetDesc::print( innerRadiusAtMinusZ() / Gaudi::Units::mm ) ; }
  if(   0 * Gaudi::Units::degree != startPhiAngle() ||
      360 * Gaudi::Units::degree != deltaPhiAngle()  )
    {
      os << " startPhiAngle[deg]"
         << DetDesc::print( startPhiAngle() / Gaudi::Units::degree ) ;
      os << " deltaPhiAngle[deg]"
         << DetDesc::print( deltaPhiAngle() / Gaudi::Units::degree ) ;
    }
  return os << "]" << endmsg ;
}

// ============================================================================

/** calculate the intersection points("ticks") of the solid objects
 *  with given line.
 *  - Line is parametrized with parameter \a t :
 *     \f$ \vec{x}(t) = \vec{p} + t \times \vec{v} \f$
 *      - \f$ \vec{p} \f$ is a point on the line
 *      - \f$ \vec{v} \f$ is a vector along the line
 *  - \a tick is just a value of parameter \a t, at which the
 *    intersection of the solid and the line occurs
 *  - both  \a Point  (\f$\vec{p}\f$) and \a Vector
 *    (\f$\vec{v}\f$) are defined in local reference system
 *   of the solid
 *  Only intersection ticks within the range
 *   \a tickMin and \a tickMax are taken into account.
 *  @see ISolid::intersectionTicks()
 *  @param Point initial point for the line
 *  @param Vector vector along the line
 *  @param tickMin minimum value of Tick
 *  @param tickMax maximu value of Tick
 *  @param ticks output container of "Ticks"
 *  @return the number of intersection points
 */
// ============================================================================
unsigned int SolidCons::intersectionTicks( const Gaudi::XYZPoint  & Point,
                                           const Gaudi::XYZVector & Vector,
                                           const ISolid::Tick& tickMin,
                                           const ISolid::Tick& tickMax,
                                           ISolid::Ticks     & ticks) const
{
  return intersectionTicksImpl(Point, Vector, tickMin, tickMax, ticks);
}
// ============================================================================
unsigned int SolidCons::intersectionTicks( const Gaudi::Polar3DPoint  & Point,
                                           const Gaudi::Polar3DVector & Vector,
                                           const ISolid::Tick& tickMin,
                                           const ISolid::Tick& tickMax,
                                           ISolid::Ticks     & ticks) const
{
  return intersectionTicksImpl(Point, Vector, tickMin, tickMax, ticks);
}
// ============================================================================
unsigned int SolidCons::intersectionTicks( const Gaudi::RhoZPhiPoint  & Point,
                                           const Gaudi::RhoZPhiVector & Vector,
                                           const ISolid::Tick& tickMin,
                                           const ISolid::Tick& tickMax,
                                           ISolid::Ticks     & ticks) const
{
  return intersectionTicksImpl(Point, Vector, tickMin, tickMax, ticks);
}
// ============================================================================
template<class aPoint, class aVector>
unsigned int SolidCons::intersectionTicksImpl( const aPoint & Point,
                                               const aVector& Vector,
                                               const Tick&    tickMin,
                                               const Tick&    tickMax ,
                                               Ticks&         ticks) const
{
  return SolidBase::intersectionTicks ( Point   ,
                                        Vector  ,
                                        tickMin ,
                                        tickMax ,
                                        ticks   );
}
// ============================================================================

// ============================================================================
// The END
// ============================================================================
