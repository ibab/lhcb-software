#ifndef     DETDESC_SOLIDPOLYHEDRONHELPER_H
#define     DETDESC_SOLIDPOLYHEDRONHELPER_H 1 


#include <vector> 


#include "DetDesc/ISolid.h"

#include "DetDesc/SolidException.h" 
#include "DetDesc/SolidTicks.h" 

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Plane3D.h"



/** @class SolidPolyHedronHelper SolidPolyHedronHelper.h DetDesc/DolidPolyHedronHelper.h

    Small helper class to simplify the construction of concrete "polihedron-like" classes

    @author  Vanya Belyaev
    @date    18/01/2001
*/


class SolidPolyHedronHelper: virtual public ISolid
{
  ///
 public:
  ///
  typedef std::vector<HepPlane3D>   PLANES   ;
  ///
 public: 
  
  ///
  /// standard constructor
  ///
  SolidPolyHedronHelper          () : m_ph_planes() {};
  ///
  /// destructor 
  ///
  virtual ~SolidPolyHedronHelper () { m_ph_planes.clear() ; }; 
  ///
  ///
  ///  Check for the given point (local frame)
  ///
  inline virtual bool          isInside ( const HepPoint3D& ) const ;
  ///
  inline virtual const ISolid* reset    () const { m_ph_planes.clear() ;  return this; }
  /// 
  /// calculate the intersection points("ticks") with a given line. 
  /// Input - line, paramterised by 
  /// x_vect = Point + Vector * T 
  /// "tick" is just a value of T, at which the intersection occurs 
  /// Return the number of intersection points (=size of Ticks container) 
  ///
  inline virtual unsigned int  intersectionTicks ( const HepPoint3D & Point  ,         // initial point for the line 
                                                   const HepVector3D& Vector ,         // vector along the line  
                                                   ISolid::Ticks    & ticks  ) const ; // output container of "Ticks"
  ///
  /// calculate the intersection points("ticks") with a given line. 
  /// Input - line, paramterised by 
  /// x_vect = Point + Vector * T 
  /// "tick" is just a value of T, at which the intersection occurs 
  /// Return the number of intersection points (=size of Ticks container) 
  /// between tickMin and tickMax 
  ///
  inline virtual unsigned int  intersectionTicks ( const HepPoint3D   & Point   ,          /// initial point for the line 
                                                   const HepVector3D  & Vector  ,          /// vector along the line
                                                   const ISolid::Tick & tickMin ,          /// minimal value of "Tick"  
                                                   const ISolid::Tick & tickMax ,          /// maximal value of "Tick" 
                                                   ISolid::Ticks      & ticks   ) const ;  /// output container of "Ticks"
  ///
  /// specific methods 
  ///
  
  ///
  /// add a face/plane given with 3 points
  /// return "false" if 3 points belongd to one line 
  ///
  inline bool             addFace( const HepPoint3D& Point1 , 
                                   const HepPoint3D& Point2 , 
                                   const HepPoint3D& Point3 );
  ///
  /// add a face/plane given with 4points 
  /// throws SolidException if points do not belong to one plane 
  ///
  inline bool             addFace( const HepPoint3D& Point1 , 
                                   const HepPoint3D& Point2 , 
                                   const HepPoint3D& Point3 , 
                                   const HepPoint3D& Point4 );  
  ///
  /// return vector of faces/planes 
  ///
  inline const  PLANES&   planes  () const { return m_ph_planes   ; } 
  ///
  ///
  /// define "inside" method for the plane.
  /// Assume that normal direction is EXTERNAL!!!
  ///
  inline bool             inside  (  const HepPoint3D& Point , 
                                     const HepPlane3D& Plane ) const { return 0 >= Plane.distance( Point ) ; };
  ///
 protected: 
  ///
  mutable PLANES   m_ph_planes   ; 
  ///
};

///
///
///

inline bool SolidPolyHedronHelper::isInside( const HepPoint3D& Point ) const 
{
  ///
  if( planes().empty() ) { return false; } 
  ///
  PLANES::const_iterator Plane = planes().begin(); 
  while( planes().end() != Plane ) { if( !inside( Point , *Plane ) ) { return false; } } 
  ///
  return true ; 
}; 

///
///
///

inline bool SolidPolyHedronHelper::addFace( const HepPoint3D& Point1 , 
                                            const HepPoint3D& Point2 , 
                                            const HepPoint3D& Point3 ) 
{
  /// check for 3 points on the same line  
  Hep3Vector p1( Point1 ) , p2( Point2 - Point1 ) , p3( Point3 - Point1); 
  if( 0 == p1.cross(p2).mag2() || 0 == p1.cross(p3).mag2() || 0 == p2.cross(p3).mag2() ) { return false; } 
  ///
  HepPlane3D Plane( Point1 , Point2 , Point3 ); 
  /// 
  if( !inside( HepPoint3D( 0 , 0 , 0 ) , Plane ) ) { Plane = HepPlane3D( Point1 , -Plane.normal() ) ; }
  ///
  Plane.normalize();
  ///
  m_ph_planes.push_back( Plane );
  ///
  return true;
  ///
};

///
///
///
///
inline bool SolidPolyHedronHelper::addFace( const HepPoint3D& Point1 , 
                                            const HepPoint3D& Point2 , 
                                            const HepPoint3D& Point3 , 
                                            const HepPoint3D& Point4 ) 
{
  ///
  HepPoint3D cPoint( ( Point1 + Point2 + Point3 + Point4 ) * 0.25 ) ; 
  ///
  HepVector3D v1( Point1 - cPoint ) , v2( Point2 - cPoint ) , v3( Point3 - cPoint ) , v4( Point4 - cPoint ) ;
  ///
  if     ( 0 != v1.cross(v2).dot(v3) ) { throw SolidException("SolidPolyHedronHelper 'plane' is not planar!!") ; } 
  else if( 0 != v2.cross(v3).dot(v4) ) { throw SolidException("SolidPolyHedronHelper 'plane' is not planar!!") ; } 
  else if( 0 != v3.cross(v4).dot(v1) ) { throw SolidException("SolidPolyHedronHelper 'plane' is not planar!!") ; } 
  else if( 0 != v4.cross(v1).dot(v2) ) { throw SolidException("SolidPolyHedronHelper 'plane' is not planar!!") ; } 
  ///
  if     ( addFace( Point1 , Point2 , Point3 ) ) { ;}
  else if( addFace( Point2 , Point3 , Point4 ) ) { ;}
  else if( addFace( Point3 , Point4 , Point1 ) ) { ;}
  else if( addFace( Point4 , Point1 , Point2 ) ) { ;}
  else { throw SolidException("SolidPolyHedronHelper:: could not find 3 points on the plane!") ; } 
  ///
  return true;
  ///
};


///
///  
///
///
/// calculate the intersection points("ticks") with a given line. 
/// Input - line, paramterised by (Point + Vector * Tick) 
/// "Tick" is just a value of parameter, at which the intercestion occurs 
/// Return the number of intersection points (=size of Ticks container)   
///
///
inline  unsigned int SolidPolyHedronHelper::intersectionTicks ( const HepPoint3D&  Point  ,          // initial point for teh line 
                                                                const HepVector3D& Vector ,          // vector along the line 
                                                                ISolid::Ticks   &  ticks  ) const   // output container of "Ticks"
{
  ///
  ticks.clear(); 
  /// 
  if( 0 == Vector.mag2() ) { return 0; } 
  ///
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
  return SolidTicks::RemoveAdjancentTicks( ticks , Point , Vector , *this );  
  ///
}; 


/// calculate the intersection points("ticks") with a given line. 
/// Input - line, paramterised by (Point + Vector * Tick) 
/// "Tick" is just a value of parameter, at which the intercestion occurs 
/// Return the number of intersection points (=size of Ticks container)   
inline  unsigned int SolidPolyHedronHelper::intersectionTicks ( const HepPoint3D  & Point   ,          // initial point for teh line 
                                                                const HepVector3D & Vector  ,          // vector along the line 
                                                                const ISolid::Tick& tickMin ,          // minimal value for the Tick 
                                                                const ISolid::Tick& tickMax ,          // maximal value for the tick 
                                                                ISolid::Ticks     & ticks   ) const    // output container of "Ticks"
{
  ///  
  intersectionTicks( Point , Vector , ticks ) ; 
  return SolidTicks::RemoveAdjancentTicks( ticks , Point , Vector , tickMin , tickMax , *this );  
  ///
};




#endif  //  DETDESC_SOLIDPOLYHEDRONHELPER_H 1 










