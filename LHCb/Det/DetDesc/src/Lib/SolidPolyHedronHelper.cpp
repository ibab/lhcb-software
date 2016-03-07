// $Id: SolidPolyHedronHelper.cpp,v 1.14 2009-04-17 08:54:24 cattanem Exp $
// ===========================================================================
#include <algorithm>
#include "DetDesc/SolidPolyHedronHelper.h"

// ============================================================================
/** @file
 *
 *  Implementation file for class : SolidPolyHedronHelper
 *
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 * @date 06/08/2001
 */
// ============================================================================

// ============================================================================
/** standard constructor
 *  @param Name name of the solid object
 */
// ============================================================================
SolidPolyHedronHelper::SolidPolyHedronHelper ( const std::string& Name )
  : SolidBase ( Name )
{}

// ============================================================================
/// destructor
// ============================================================================
SolidPolyHedronHelper::~SolidPolyHedronHelper()
{
  m_ph_planes.clear();
  m_ph_vertices.clear();
}

// ============================================================================
/// set parameters for bounding solids (box, sphere and cylinder)
// ============================================================================
void SolidPolyHedronHelper::setBP()
{
  if( m_ph_vertices.empty() )
    { throw SolidException("SolidPHH::setBP(): no vertices are available!");}

  /// loop over all points
  const Gaudi::XYZPoint& point  = m_ph_vertices.front();
  setXMin   ( point.x() ) ;
  setXMax   ( point.x() ) ;
  setYMin   ( point.y() ) ;
  setYMax   ( point.y() ) ;
  setZMin   ( point.z() ) ;
  setZMax   ( point.z() ) ;
  setRMax   ( point.r() ) ;
  setRhoMax ( std::sqrt(point.perp2() ) ) ;

  for( const auto& vertex : m_ph_vertices )
    {
      setXMin   ( std::min(vertex.x(), xMin()) );
      setYMin   ( std::min(vertex.y(), yMin()) );
      setZMin   ( std::min(vertex.z(), zMin()) );
      setXMax   ( std::max(vertex.x(), xMax()) );
      setYMax   ( std::max(vertex.y(), yMax()) );
      setZMax   ( std::max(vertex.z(), zMax()) );
      setRMax   ( std::max(vertex.r(), rMax()) );
      setRhoMax ( std::max( std::sqrt(vertex.perp2() ) , rhoMax () ) );
    }
  ///
  checkBP();
}
// ============================================================================

// ============================================================================
/** - check for the given 3D-point.
 *    Point coordinated are in the local reference
 *    frame of the solid.
 *  - implementation of ISolid absstract interface
 *  @see ISolid
 *  @param point point (in local reference system of the solid)
 *  @return true if the point is inside the solid
 */
// ============================================================================
// ============================================================================
bool SolidPolyHedronHelper::isInside (  const Gaudi::XYZPoint& point ) const
{
  return isInsideImpl(point) ;
}
// ============================================================================
bool SolidPolyHedronHelper::isInside ( const Gaudi::Polar3DPoint& point) const
{
  return isInsideImpl(point);
}
// ============================================================================
bool SolidPolyHedronHelper::isInside ( const Gaudi::RhoZPhiPoint& point) const
{
  return isInsideImpl(point);
}
// ============================================================================
template <class aPoint>
bool SolidPolyHedronHelper::isInsideImpl( const aPoint& point ) const
{
  if( planes().empty()   ) { return false ; }
  /// check for bounding box
  if( isOutBBox( point ) ) { return false ; }
  /// loop over faces
  return std::all_of( planes().begin(), planes().end(),
                      [&](PLANES::const_reference plane) { return inside(point, plane) ; } );
}

// ============================================================================
/** - calculate the intersection points("ticks") of the solid objects
 *    with given line.
 *  -# Line is parametrized with parameter \a t :
 *     \f$ \vec{x}(t) = \vec{p} + t \times \vec{v} \f$
 *      - \f$ \vec{p} \f$ is a point on the line
 *      - \f$ \vec{v} \f$ is a vector along the line
 *  -# \a tick is just a value of parameter \a t, at which the
 *    intersection of the solid and the line occurs
 *  -# both  \a Point  (\f$\vec{p}\f$) and \a Vector
 *    (\f$\vec{v}\f$) are defined in local reference system
 *   of the solid
 *  - implementation of ISolid abstract interface
 *  @see ISolid
 *  @param Point initial point for the line
 *  @param Vector vector along the line
 *  @param ticks output container of "Ticks"
 *  @return the number of intersection points
 */
// ============================================================================
unsigned int
SolidPolyHedronHelper::intersectionTicks( const Gaudi::XYZPoint&  Point,
                                          const Gaudi::XYZVector& Vector,
                                          ISolid::Ticks&          ticks ) const
{
  return intersectionTicksImpl(Point, Vector, ticks);
}
// ============================================================================
unsigned int
SolidPolyHedronHelper::intersectionTicks( const Gaudi::Polar3DPoint&  Point,
                                          const Gaudi::Polar3DVector& Vector,
                                          ISolid::Ticks&              ticks ) const
{
  return intersectionTicksImpl(Point, Vector, ticks);
}
// ============================================================================
unsigned int
SolidPolyHedronHelper::intersectionTicks( const Gaudi::RhoZPhiPoint&  Point,
                                          const Gaudi::RhoZPhiVector& Vector,
                                          ISolid::Ticks&              ticks ) const
{
  return intersectionTicksImpl(Point, Vector, ticks);
}
// ============================================================================
template <class aPoint, class aVector>
unsigned int
SolidPolyHedronHelper::intersectionTicksImpl(const aPoint&  Point,
                                             const aVector& Vector,
                                             ISolid::Ticks& ticks  ) const
{
  // clear the output container
  ticks.clear();
  // check for valid arguments
  if( 0 == Vector.mag2()              ) { return 0; }
  // line touches the bounding sphere?
  if( !crossBSphere( Point , Vector ) ) { return 0 ; }
  // loop over all faces
  for( auto iPlane = planes().begin();
       planes().end() != iPlane ; ++iPlane )
    {
      const Gaudi::Plane3D& Plane = *iPlane ;
      double vn =  Vector.Dot( Plane.Normal() ) ;
      if(  0 == vn ) { continue ; }
      ISolid::Tick tick = -1. * ( Plane.Distance( Point ) / vn ) ;
      ticks.push_back( tick );
    };
  ///
  return SolidTicks::RemoveAdjancentTicks( ticks  ,
                                           Point  ,
                                           Vector ,
                                           *this  );
}
// ============================================================================
// ============================================================================
/** Calculate the maximum number of ticks that a straight line could make with this solid
  *  @return maximum number of ticks
  */
ISolid::Ticks::size_type SolidPolyHedronHelper::maxNumberOfTicks() const
{
  // This is far more than possible, but I do not want to revise this solid now.
  return planes().size() ;
}

// ============================================================================
/**  add a face/plane given with 3 points
 *  @see addFace( const Gaudi::XYZPoint& , const Gaudi::XYZPoint& ,
 *                const Gaudi::XYZPoint& , const Gaudi::XYZPoint& )
 *  @param Point1  the first  3D-point of the plane
 *  @param Point2  the second 3D-point of the plane
 *  @param Point3  the third  3D-point of the plane
 *  @return "false" if 3 points belongs to one line
 */
// ============================================================================
bool SolidPolyHedronHelper::addFace
( const Gaudi::XYZPoint& Point1 ,
  const Gaudi::XYZPoint& Point2 ,
  const Gaudi::XYZPoint& Point3 )
{
  /// check for 3 points on the same line
  Gaudi::XYZVector v1( Point1 ) , v2( Point2 - Point1 ) , v3( Point3 - Point1);
  if( 0 == v1.Cross( v2 ).mag2() ||
      0 == v1.Cross( v3 ).mag2() ||
      0 == v2.Cross( v3 ).mag2()   ) { return false; }
  ///
  Gaudi::Plane3D Plane( Point1 , Point2 , Point3 );
  /// invert face orientation if needed
  if( !inside( Gaudi::XYZPoint( 0 , 0 , 0 ) , Plane ) )
    { Plane = Gaudi::Plane3D(  -Plane.Normal() , Point1 ) ; }
  ///
  //  Plane.Normalize(); // MathCore Plane3D is already normalised.
  ///
  m_ph_planes.push_back( Plane );
  ///
  return true;
  ///
}
// ============================================================================

// ============================================================================
/**  add a face/plane given with 4 points
 *  @see addFace( const Gaudi::XYZPoint& , const Gaudi::XYZPoint& ,
 *                const Gaudi::XYZPoint& )
 *  @param Point1  the first  3D-point of the plane
 *  @param Point2  the second 3D-point of the plane
 *  @param Point3  the third  3D-point of the plane
 *  @param Point4  the fourth 3D-point of the plane
 *  @exception SolidException  if 4 points do not define the place
 *  @return "false" if 3 points belongs to one line
 */
// ============================================================================
bool SolidPolyHedronHelper::addFace
( const Gaudi::XYZPoint& Point1 ,
  const Gaudi::XYZPoint& Point2 ,
  const Gaudi::XYZPoint& Point3 ,
  const Gaudi::XYZPoint& Point4 )
{
  ///
  const Gaudi::XYZPoint cPoint( 0.25*(Point1.x()+Point2.x()+Point3.x()+Point4.x()),
                                0.25*(Point1.y()+Point2.y()+Point3.y()+Point4.y()),
                                0.25*(Point1.z()+Point2.z()+Point3.z()+Point4.z()) );

  ///
  const Gaudi::XYZVector v1( Point1 - cPoint ) ;
  const Gaudi::XYZVector v2( Point2 - cPoint ) ;
  const Gaudi::XYZVector v3( Point3 - cPoint ) ;
  const Gaudi::XYZVector v4( Point4 - cPoint ) ;
  ///
  const double t1   = v2.Cross(  v3 ).Dot( v4 )     ;
  const double v234 = std::sqrt(v2.mag2() * v3.mag2() * v4.mag2() );
  const double t2   = v3.Cross(  v4 ).Dot( v1 )     ;
  const double v341 = std::sqrt(v3.mag2() * v4.mag2() * v1.mag2() );
  const double t3   = v4.Cross(  v1 ).Dot( v2 )     ;
  const double v412 = std::sqrt(v4.mag2() * v1.mag2() * v2.mag2() );
  const double t4   = v1.Cross(  v2 ).Dot( v3 )     ;
  const double v123 = std::sqrt(v1.mag2() * v2.mag2() * v3.mag2() );

  if      ( 0 != v234 && 1.e-6 < fabs( t1 / v234 ) )
    { throw SolidException("SolidPolyHedronHelper 'plane' is not planar 1 ") ; }
  else if ( 0 != v341 && 1.e-6 < fabs( t2 / v341 ) )
    { throw SolidException("SolidPolyHedronHelper 'plane' is not planar 2 ") ; }
  else if ( 0 != v412 && 1.e-6 < fabs( t3 / v412 ) )
    { throw SolidException("SolidPolyHedronHelper 'plane' is not planar 3 ") ; }
  else if ( 0 != v123 && 1.e-6 < fabs( t4 / v123 ) )
    { throw SolidException("SolidPolyHedronHelper 'plane' is not planar 4 ") ; }

  ///
  if     ( addFace( cPoint , Point1 , Point2 ) ) { ;}
  else if( addFace( cPoint , Point1 , Point3 ) ) { ;}
  else if( addFace( cPoint , Point1 , Point4 ) ) { ;}
  else if( addFace( cPoint , Point2 , Point3 ) ) { ;}
  else if( addFace( cPoint , Point1 , Point4 ) ) { ;}
  else if( addFace( cPoint , Point3 , Point4 ) ) { ;}
  else
    { throw SolidException("SolidPolyHedronHelper:: no 3 points!") ; }
  ///
  return true;
  ///
}
// ============================================================================

// ============================================================================
// The END
// ============================================================================


