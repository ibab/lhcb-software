/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#include "DetDesc/SolidPolyHedronHelper.h"

/// ===========================================================================
/** @file SolidPolyHedronHelper.cpp 
 *
 *  Implementation file for class : SolidPolyHedronHelper
 *
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 * @date 06/08/2001 
 */
/// ===========================================================================

/// ===========================================================================
/** standard constructor
 *  @param Name name of the solid object
 */
/// ===========================================================================
SolidPolyHedronHelper::SolidPolyHedronHelper ( const std::string& Name )
  : SolidBase ( Name ) 
  , m_ph_planes() 
{};

/// ===========================================================================
/// destructor 
/// ===========================================================================
SolidPolyHedronHelper::~SolidPolyHedronHelper() 
{ 
  reset() ;
  m_ph_planes.clear(); 
};

/// ===========================================================================
/** - check for the given 3D-point. 
 *    Point coordinated are in the local reference 
 *    frame of the solid.   
 *  - implementation of ISolid absstract interface  
 *  @see ISolid 
 *  @param point point (in local reference system of the solid)
 *  @return true if the point is inside the solid
 */
/// ===========================================================================
bool SolidPolyHedronHelper::isInside ( const HepPoint3D& point ) const 
{
  if( planes().empty() ) { return false; } 
  ///
  PLANES::const_iterator Plane = planes().begin(); 
  while( planes().end() != Plane ) 
    { if( !inside( point , *Plane ) ) { return false; } } 
  ///
  return true ;  
};

/// ===========================================================================
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
SolidPolyHedronHelper::intersectionTicks ( const HepPoint3D&  Point  ,       
                                           const HepVector3D& Vector ,       
                                           ISolid::Ticks   &   ticks  ) const
{
  /// clear the output container 
  ticks.clear(); 
  /// check for valid arguments 
  if( 0 == Vector.mag2() ) { return 0; } 
  /// loop over all faces 
  PLANES::const_iterator iPlane = planes().begin();  
  while( planes().end() != iPlane )  
    {
      const HepPlane3D& Plane = *iPlane++ ; 
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

/// ===========================================================================
/**  add a face/plane given with 3 points
 *  @see addFace( const HepPoint3D& , const HepPoint3D& ,
 *                const HepPoint3D& , const HepPoint3D& )
 *  @param Point1  the first  3D-point of the plane
 *  @param Point2  the second 3D-point of the plane 
 *  @param Point3  the third  3D-point of the plane 
 *  @return "false" if 3 points belongs to one line 
 */
/// ===========================================================================
bool SolidPolyHedronHelper::addFace( const HepPoint3D& Point1 , 
                                     const HepPoint3D& Point2 , 
                                     const HepPoint3D& Point3 ) 
{
  /// check for 3 points on the same line  
  Hep3Vector p1( Point1 ) , p2( Point2 - Point1 ) , p3( Point3 - Point1); 
  if( 0 == p1.cross( p2 ).mag2() || 
      0 == p1.cross( p3 ).mag2() || 
      0 == p2.cross( p3 ).mag2()   ) { return false; } 
  ///
  HepPlane3D Plane( Point1 , Point2 , Point3 ); 
  /// 
  if( !inside( HepPoint3D( 0 , 0 , 0 ) , Plane ) ) 
    { Plane = HepPlane3D( Point1 , -Plane.normal() ) ; }
  ///
  Plane.normalize();
  ///
  m_ph_planes.push_back( Plane );
  ///
  return true;
  ///
};

/// ===========================================================================
/**  add a face/plane given with 4 points
 *  @see addFace( const HepPoint3D& , const HepPoint3D& ,
 *                const HepPoint3D& )
 *  @param Point1  the first  3D-point of the plane
 *  @param Point2  the second 3D-point of the plane 
 *  @param Point3  the third  3D-point of the plane 
 *  @param Point4  the fourth 3D-point of the plane 
 *  @exception SolidException  if 4 pointd do not define the place
 *  @return "false" if 3 points belongs to one line 
 */
/// ===========================================================================
bool SolidPolyHedronHelper::addFace( const HepPoint3D& Point1 , 
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
  if     ( 0 != v1.cross( v2 ).dot( v3 ) ) 
    { throw SolidException("SolidPolyHedronHelper 'plane' is not planar!!") ; } 
  else if( 0 != v2.cross( v3 ).dot( v4 ) ) 
    { throw SolidException("SolidPolyHedronHelper 'plane' is not planar!!") ; } 
  else if( 0 != v3.cross( v4 ).dot( v1 ) ) 
    { throw SolidException("SolidPolyHedronHelper 'plane' is not planar!!") ; } 
  else if( 0 != v4.cross( v1 ).dot( v2 ) ) 
    { throw SolidException("SolidPolyHedronHelper 'plane' is not planar!!") ; } 
  ///
  if     ( addFace( Point1 , Point2 , Point3 ) ) { ;}
  else if( addFace( Point2 , Point3 , Point4 ) ) { ;}
  else if( addFace( Point3 , Point4 , Point1 ) ) { ;}
  else if( addFace( Point4 , Point1 , Point2 ) ) { ;}
  else 
    { throw SolidException("SolidPolyHedronHelper:: no 3 points!") ; } 
  ///
  return true;
  ///
};

/// ===========================================================================


