// $Id: SolidPolyHedronHelper.cpp,v 1.5 2002-05-13 18:29:54 ibelyaev Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2002/05/11 18:25:47  ibelyaev
//  see $DETDESCROOT/doc/release.notes 11 May 2002
//
// ===========================================================================
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
  , m_ph_planes  () 
  , m_ph_vertices() 
{};

// ============================================================================
/// destructor 
// ============================================================================
SolidPolyHedronHelper::~SolidPolyHedronHelper() 
{ 
  reset() ;
  m_ph_planes.clear(); 
  m_ph_vertices.clear(); 
};

// ============================================================================
/** set parameters for bounding solids (box, sphere and cylinder)
 *  @return status code 
 */
// ============================================================================
StatusCode SolidPolyHedronHelper::setBP() 
{
  if( m_ph_vertices.empty() )
    { throw SolidException("SolidPHH::setBP(): no vertices are available!");}
  
  /// loop over all points
  const HepPoint3D& point  = m_ph_vertices.front();
  setXMin   ( point.x    () ) ;
  setXMax   ( point.x    () ) ;
  setYMin   ( point.y    () ) ;
  setYMax   ( point.y    () ) ;
  setZMin   ( point.z    () ) ;
  setZMax   ( point.z    () ) ;
  setRMax   ( point.r    () ) ;
  setRhoMax ( point.perp () ) ;
  
  for( VERTICES::const_iterator vertex = m_ph_vertices.begin() ;
       m_ph_vertices.end() != vertex ; ++vertex )
    {
      setXMin   ( vertex->x    () < xMin   () ? vertex->x    () : xMin   () );
      setYMin   ( vertex->y    () < yMin   () ? vertex->y    () : yMin   () );
      setZMin   ( vertex->z    () < zMin   () ? vertex->z    () : zMin   () );
      setXMax   ( vertex->x    () > xMax   () ? vertex->x    () : xMax   () );
      setYMax   ( vertex->y    () > yMax   () ? vertex->y    () : yMax   () );
      setZMax   ( vertex->z    () > zMax   () ? vertex->z    () : zMax   () );
      setRMax   ( vertex->r    () > rMax   () ? vertex->r    () : rMax   () );
      setRhoMax ( vertex->perp () > rhoMax () ? vertex->perp () : rhoMax () ); 
    }
  ///
  return checkBP();
};
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
bool SolidPolyHedronHelper::isInside ( const HepPoint3D& point ) const 
{
  if( planes().empty()   ) { return false ; } 
  /// ckeck for bounding box 
  if( isOutBBox( point ) ) { return false ; }
  /// loop over faces 
  for( PLANES::const_iterator Plane = planes().begin(); 
       planes().end() != Plane ; ++Plane ) 
    { if( !inside( point , *Plane ) ) { return false; } } 
  ///
  return true ;  
};

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
unsigned int 
SolidPolyHedronHelper::intersectionTicks 
( const HepPoint3D&  Point  ,       
  const HepVector3D& Vector ,       
  ISolid::Ticks   &   ticks  ) const
{
  // clear the output container 
  ticks.clear(); 
  // check for valid arguments 
  if( 0 == Vector.mag2()              ) { return 0; } 
  // line touches the bounding sphere? 
  if( !crossBSphere( Point , Vector ) ) { return 0 ; }
  // loop over all faces 
  for( PLANES::const_iterator iPlane = planes().begin();  
       planes().end() != iPlane ; ++iPlane )
    {
      const HepPlane3D& Plane = *iPlane ; 
      double vn =  Vector*Plane.normal() ; 
      if(  0 == vn ) { continue ; } 
      ISolid::Tick tick = -1. * ( Plane.distance( Point ) / vn ) ; 
      ticks.push_back( tick );   
    };  
  /// 
  return SolidTicks::RemoveAdjancentTicks( ticks  , 
                                           Point  , 
                                           Vector , 
                                           *this  );
};
// ============================================================================

// ============================================================================
/**  add a face/plane given with 3 points
 *  @see addFace( const HepPoint3D& , const HepPoint3D& ,
 *                const HepPoint3D& , const HepPoint3D& )
 *  @param Point1  the first  3D-point of the plane
 *  @param Point2  the second 3D-point of the plane 
 *  @param Point3  the third  3D-point of the plane 
 *  @return "false" if 3 points belongs to one line 
 */
// ============================================================================
bool SolidPolyHedronHelper::addFace
( const HepPoint3D& Point1 , 
  const HepPoint3D& Point2 , 
  const HepPoint3D& Point3 ) 
{
  /// check for 3 points on the same line  
  HepVector3D v1( Point1 ) , v2( Point2 - Point1 ) , v3( Point3 - Point1); 
  if( 0 == v1.cross( v2 ).mag2() || 
      0 == v1.cross( v3 ).mag2() || 
      0 == v2.cross( v3 ).mag2()   ) { return false; } 
  ///
  HepPlane3D Plane( Point1 , Point2 , Point3 ); 
  /// invert face orientation if needed 
  if( !inside( HepPoint3D( 0 , 0 , 0 ) , Plane ) ) 
    { Plane = HepPlane3D(  -Plane.normal() , Point1 ) ; }
  ///
  Plane.normalize();
  ///
  m_ph_planes.push_back( Plane );
  ///
  return true;
  ///
};
// ============================================================================

// ============================================================================
/**  add a face/plane given with 4 points
 *  @see addFace( const HepPoint3D& , const HepPoint3D& ,
 *                const HepPoint3D& )
 *  @param Point1  the first  3D-point of the plane
 *  @param Point2  the second 3D-point of the plane 
 *  @param Point3  the third  3D-point of the plane 
 *  @param Point4  the fourth 3D-point of the plane 
 *  @exception SolidException  if 4 points do not define the place
 *  @return "false" if 3 points belongs to one line 
 */
// ============================================================================
bool SolidPolyHedronHelper::addFace
( const HepPoint3D& Point1 , 
  const HepPoint3D& Point2 , 
  const HepPoint3D& Point3 , 
  const HepPoint3D& Point4 ) 
{
  ///
  const HepPoint3D cPoint( ( Point1 + Point2 + Point3 + Point4 ) * 0.25 ) ; 
  /// 
  const HepVector3D v1( Point1 - cPoint ) ;
  const HepVector3D v2( Point2 - cPoint ) ;
  const HepVector3D v3( Point3 - cPoint ) ; 
  const HepVector3D v4( Point4 - cPoint ) ;
  ///
  const double t1   = v2.cross(  v3 ). dot( v4 )     ;
  const double v234 = v2.mag() * v3.mag() * v4.mag() ;
  const double t2   = v3.cross(  v4 ). dot( v1 )     ;
  const double v341 = v3.mag() * v4.mag() * v1.mag() ;
  const double t3   = v4.cross(  v1 ). dot( v2 )     ;
  const double v412 = v4.mag() * v1.mag() * v2.mag() ;
  const double t4   = v1.cross(  v2 ). dot( v3 )     ;
  const double v123 = v1.mag() * v2.mag() * v3.mag() ;
  
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
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================


