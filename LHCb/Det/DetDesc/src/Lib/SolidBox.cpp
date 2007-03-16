// $Id: SolidBox.cpp,v 1.18 2007-03-16 15:57:23 cattanem Exp $ 
// ===========================================================================
// DetDesc 
#include "DetDesc/DetDesc.h" 
#include "DetDesc/SolidBox.h" 
#include "DetDesc/SolidTicks.h" 
#include "DetDesc/SolidException.h" 
// Units
#include "GaudiKernel/SystemOfUnits.h"

// ============================================================================
/** @file SolidBox.cpp
 *
 *  implementation of class SolidBox 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru  
 *  @date xx/xx/xxxx
 */
// ============================================================================

// ============================================================================
/** standard(public)  constructor 
 *  @param      Name           name of this box  
 *  @param      xHalf          half-size of the box in x-direction
 *  @param      yHalf          half-size of the box in y-direction
 *  @param      zHalf          half-size of the box in z-direction
 *  @exception  SolidException wrong parameters range 
 */
// ============================================================================
SolidBox::SolidBox
( const std::string& Name  ,
  const double       xHalf , 
  const double       yHalf , 
  const double       zHalf )
  : SolidBase        ( Name  ) 
  , m_box_xHalfLength( xHalf )
  , m_box_yHalfLength( yHalf )
  , m_box_zHalfLength( zHalf )
{
  if( 0 >= xHalfLength() )
    { throw SolidException("SolidBox(): XHalfLength is non positive! "); }
  if( 0 >= yHalfLength() )
    { throw SolidException("SolidBox(): YHalfLength is non positive! "); }
  if( 0 >= zHalfLength() )
    { throw SolidException("SolidBox(): ZHalfLength is non positive! "); }
  /// set bounding paramters od SolidBase class
  setBP () ;
};
// ============================================================================

// ============================================================================
/// set parameters for bounding solids (box, sphere and cylinder)
// ============================================================================
void SolidBox::setBP() 
{
  /// set bounding paramters od SolidBase class
  setXMin   ( -xHalfLength() );
  setXMax   (  xHalfLength() );
  setYMin   ( -yHalfLength() );
  setYMax   (  yHalfLength() );
  setZMin   ( -zHalfLength() );
  setZMax   (  zHalfLength() );
  setRMax   ( sqrt( xMax() * xMax() + yMax() * yMax() + zMax() * zMax() ) );
  setRhoMax ( sqrt( xMax() * xMax() + yMax() * yMax()                   ) );
  //
  checkBP () ;
};
// ============================================================================

// ============================================================================
/** default (protected) constructor 
 */
// ============================================================================
SolidBox::SolidBox()
  : SolidBase        ("anonymous box")
  , m_box_xHalfLength( 100000.0 ) 
  , m_box_yHalfLength( 100000.0 ) 
  , m_box_zHalfLength( 100000.0 ) 
{};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
SolidBox::~SolidBox(){ if ( this == m_cover ) { m_cover = 0 ; } ; };
// ============================================================================

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
unsigned int SolidBox::intersectionTicks( const Gaudi::XYZPoint& point,
                                           const Gaudi::XYZVector& vect,
                                           ISolid::Ticks&    ticks ) const 
{
  return intersectionTicksImpl(point, vect, ticks);
};
// ============================================================================
unsigned int SolidBox::intersectionTicks( const Gaudi::Polar3DPoint& point,
                                           const Gaudi::Polar3DVector& vect,
                                           ISolid::Ticks&    ticks ) const 
{
  return intersectionTicksImpl(point, vect, ticks);
};
// ============================================================================
unsigned int SolidBox::intersectionTicks( const Gaudi::RhoZPhiPoint& point,
                                           const Gaudi::RhoZPhiVector& vect, 
                                           ISolid::Ticks&    ticks ) const 
{
  return intersectionTicksImpl(point, vect, ticks);
};
// ============================================================================
template<class aPoint, class aVector>
unsigned int SolidBox::intersectionTicksImpl( const aPoint & point  ,
                                              const aVector& vect   ,
                                              ISolid::Ticks& ticks  ) const
{  

  ///
  ticks.clear();

  unsigned int nTicks = ticks.size();
  Tick tick = 0;

  // find intersection ticks with z-planes
  if ( vect.z() != 0 ) {
    tick = ( zHalfLength() - point.z() ) / vect.z() ;

    aPoint intersect = point + tick*vect;
    if( intersect.x() > xMin() && intersect.x() < xMax() &&
        intersect.y() > yMin() && intersect.y() < yMax() )
      ticks.push_back( tick );

    tick = ( -1.0*zHalfLength() - point.z() ) / vect.z() ;
    intersect = point + tick*vect;
    if( intersect.x() > xMin() && intersect.x() < xMax() &&
        intersect.y() > yMin() && intersect.y() < yMax() )
      ticks.push_back( tick );

  }
  if ( ticks.size() - nTicks == 2 ) {
    std::sort( ticks.begin() , ticks.end() ) ;
    return 2u;
  }


  // find intersection ticks with x-planes
  if ( vect.x() != 0 ) {
    tick = ( xHalfLength() - point.x() ) / vect.x() ;

    aPoint intersect = point + tick*vect;
    if( intersect.z() > zMin() && intersect.z() < zMax() &&
        intersect.y() > yMin() && intersect.y() < yMax() )
      ticks.push_back( tick );

    if ( ticks.size() - nTicks == 2u ) {
      std::sort( ticks.begin() , ticks.end() ) ;
      return 2u;
    }

    tick = ( -1.0*xHalfLength() - point.x() ) / vect.x() ;
    intersect = point + tick*vect;
    if( intersect.z() > zMin() && intersect.z() < zMax() &&
        intersect.y() > yMin() && intersect.y() < yMax() )
      ticks.push_back( tick );

    if ( ticks.size() - nTicks == 2u ) {
      std::sort( ticks.begin() , ticks.end() ) ;
      return 2u;
    }

  }

  // find intersection ticks with y-planes
  if ( vect.y() != 0 ) {
    tick = ( yHalfLength() - point.y() ) / vect.y() ;

    aPoint intersect = point + tick*vect;
    if( intersect.x() > xMin() && intersect.x() < xMax() &&
        intersect.z() > zMin() && intersect.z() < zMax() )
      ticks.push_back( tick );

    if ( ticks.size() - nTicks == 2 ) {
      std::sort( ticks.begin() , ticks.end() ) ;
      return 2u;
    }

    tick = ( -1.0*yHalfLength() - point.y() ) / vect.y() ;
    intersect = point + tick*vect;
    if( intersect.x() > xMin() && intersect.x() < xMax() &&
        intersect.z() > zMin() && intersect.z() < zMax() )
      ticks.push_back( tick );

    if ( ticks.size() - nTicks == 2 ) {
      std::sort( ticks.begin() , ticks.end() ) ;
      return 2u;
    }

  }
 return 0u;

  ///
  // find intersection ticks with x-planes
 //  SolidTicks::LineIntersectsTheX( point , vect ,        xHalfLength() , 
 //                                  std::back_inserter( ticks ) ); 
 // SolidTicks::LineIntersectsTheX( point , vect , -1.0 * xHalfLength() , 
 //                                  std::back_inserter( ticks ) ); 
  // find intersection ticks with y-planes
// SolidTicks::LineIntersectsTheY( point , vect ,        yHalfLength() , 
//                                 std::back_inserter( ticks ) ); 
//  SolidTicks::LineIntersectsTheY( point , vect , -1.0 * yHalfLength() , 
//                                  std::back_inserter( ticks ) ); 
  // find intersection ticks with z-planes
//  SolidTicks::LineIntersectsTheZ( point , vect ,        zHalfLength() , 
//                                  std::back_inserter( ticks ) ); 
//  SolidTicks::LineIntersectsTheZ( point , vect , -1.0 * zHalfLength() , 
//                                  std::back_inserter( ticks ) ); 
// sort and remove adjancent and some EXTRA ticks and return 
//  return SolidTicks::RemoveAdjancentTicks( ticks , point , vect , *this );  
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
bool SolidBox::isInside( const Gaudi::XYZPoint   & point ) const 
{
  return isInsideImpl(point);
}
// ============================================================================
bool SolidBox::isInside( const Gaudi::Polar3DPoint& point ) const 
{
  return isInsideImpl(point);
}
// ============================================================================
bool SolidBox::isInside( const Gaudi::RhoZPhiPoint   & point ) const 
{
  return isInsideImpl(point);
}
// ============================================================================
template <class aPoint>
bool SolidBox::isInsideImpl( const aPoint& point ) const
{ 
  /* if ( fabs( point.z() ) > zHalfLength() || 
       fabs( point.x() ) > xHalfLength() ||
       fabs( point.y() ) > yHalfLength() ) { return false; }
       return true;*/
  return (fabs(point.z()) < zHalfLength() &&
          fabs(point.y()) < yHalfLength() && 
          fabs(point.x()) < xHalfLength()); 
};
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
unsigned int SolidBox::intersectionTicks( const Gaudi::XYZPoint  & Point,
                                          const Gaudi::XYZVector & Vector,
                                          const ISolid::Tick& tickMin,
                                          const ISolid::Tick& tickMax,
                                          ISolid::Ticks     & ticks) const 
{
  return intersectionTicksImpl(Point, Vector, tickMin, tickMax, ticks);
}
// ============================================================================
unsigned int SolidBox::intersectionTicks( const Gaudi::Polar3DPoint  & Point,
                                          const Gaudi::Polar3DVector & Vector,
                                          const ISolid::Tick& tickMin,
                                          const ISolid::Tick& tickMax,
                                          ISolid::Ticks     & ticks) const 
{
  return intersectionTicksImpl(Point, Vector, tickMin, tickMax, ticks);
}
// ============================================================================
unsigned int SolidBox::intersectionTicks( const Gaudi::RhoZPhiPoint  & Point,
                                          const Gaudi::RhoZPhiVector & Vector,
                                          const ISolid::Tick& tickMin,
                                          const ISolid::Tick& tickMax,
                                          ISolid::Ticks     & ticks) const 
{
  return intersectionTicksImpl(Point, Vector, tickMin, tickMax, ticks);
}
// ============================================================================
template<class aPoint, class aVector>
unsigned int SolidBox::intersectionTicksImpl( const aPoint & Point,
                                              const aVector& Vector,
                                              const Tick&    tickMin ,
                                              const Tick&    tickMax ,
                                              Ticks&         ticks   ) const  
{
  if( isOutBBox( Point , Vector  , tickMin , tickMax  ) ) { return 0 ; }
  if( !crossBSphere( Point , Vector )                   ) { return 0 ; }
  
  //
  return SolidBase::intersectionTicks ( Point   , 
                                        Vector  ,
                                        tickMin , 
                                        tickMax ,
                                        ticks   );
};
// ============================================================================

// ============================================================================
/** printout to STD/STL stream    
 *  @param os STD/STL stream
 *  @return reference to the stream 
 */ 
// ============================================================================
std::ostream& SolidBox::printOut  ( std::ostream&  os ) const
{
  /// serialize the base class 
  SolidBase::printOut( os );
  return
    os << "[" 
       << " xsize[mm]=" << DetDesc::print( xsize() / Gaudi::Units::mm ) 
       << " ysize[mm]=" << DetDesc::print( ysize() / Gaudi::Units::mm ) 
       << " zsize[mm]=" << DetDesc::print( zsize() / Gaudi::Units::mm )  
       << "]" << std::endl ;
};
// ============================================================================

// ============================================================================
/** printout to Gaudi  stream    
 *  @param os Gaudi stream
 *  @return reference to the stream 
 */ 
// ============================================================================
MsgStream&    SolidBox::printOut  ( MsgStream&     os ) const
{
  /// serialize the base class 
  SolidBase::printOut( os );
  return 
    os << "[" 
       << " xsize[mm]=" << DetDesc::print( xsize() / Gaudi::Units::mm ) 
       << " ysize[mm]=" << DetDesc::print( ysize() / Gaudi::Units::mm ) 
       << " zsize[mm]=" << DetDesc::print( zsize() / Gaudi::Units::mm )  
       << "]" << endreq ;
};
// ============================================================================

// ============================================================================
/** - retrieve the pointer to "simplified" solid - "cover"
 *  - implementation of ISolid abstract interface 
 *  The Box is the most simple shape
 *  @see ISolid 
 *  @return pointer to "simplified" solid - "cover"
 */
// ============================================================================
const ISolid* SolidBox::cover() const { return this; };

// ============================================================================
/** - retrieve the pointer to "the most simplified cover" 
 *  - implementation  of ISolid abstract interface 
 *  - reimplementation of SolidBase::coverTop() 
 *  The Box is the most simple shape
 *  @see ISolid 
 *  @see SolidBase 
 *  @return pointer to the most simplified cover 
 */
// ============================================================================
const ISolid* SolidBox::coverTop () const { return this; };


// ============================================================================
// The END 
// ============================================================================
