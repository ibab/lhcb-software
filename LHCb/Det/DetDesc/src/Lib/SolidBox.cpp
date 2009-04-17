// $Id: SolidBox.cpp,v 1.20 2009-04-17 08:54:24 cattanem Exp $ 
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
unsigned int SolidBox::intersectionTicks( const Gaudi::XYZPoint&  Point,
                                          const Gaudi::XYZVector& Vector,
                                          ISolid::Ticks&          ticks ) const 
{
  return intersectionTicksImpl(Point, Vector, ticks);
};
// ============================================================================
unsigned int SolidBox::intersectionTicks( const Gaudi::Polar3DPoint&  Point,
                                          const Gaudi::Polar3DVector& Vector,
                                          ISolid::Ticks&              ticks ) const 
{
  return intersectionTicksImpl(Point, Vector, ticks);
};
// ============================================================================
unsigned int SolidBox::intersectionTicks( const Gaudi::RhoZPhiPoint&  Point,
                                          const Gaudi::RhoZPhiVector& Vector, 
                                          ISolid::Ticks&              ticks ) const 
{
  return intersectionTicksImpl(Point, Vector, ticks);
};

// ============================================================================
inline bool inrange( const double x, const double xmin, const double xmax) {
  // local helper function. is there a gsl/std solution for this?
  return xmin <= x && x <= xmax ; 
}
// ============================================================================
template<class aPoint, class aVector>
unsigned int SolidBox::intersectionTicksImpl( const aPoint & point  ,
                                              const aVector& vect   ,
                                              ISolid::Ticks& ticks  ) const
{  
  ///
  ticks.clear() ;
  Tick tick = 0;
  // use temporary static array, because push_backs are slow
  double theticks[6] ;
  int ntick(0u) ;
  
  // find intersection ticks with z-planes
  if ( vect.z() != 0 ) {
    tick = ( -zHalfLength() - point.z() ) / vect.z() ;
    if( inrange( point.x() + tick* vect.x(), xMin(), xMax() ) &&
	inrange( point.y() + tick* vect.y(), yMin(), yMax() ) )
      theticks[ntick++] = tick ;
    tick = (  zHalfLength() - point.z() ) / vect.z() ;
    if( inrange( point.x() + tick* vect.x(), xMin(), xMax() ) &&
	inrange( point.y() + tick* vect.y(), yMin(), yMax() ) )
      theticks[ntick++] = tick ;
  }
  
  if( ntick != 2) {
    // find intersection ticks with x-planes
    if ( vect.x() != 0 ) {
      tick = ( -xHalfLength() - point.x() ) / vect.x() ;
      if( inrange( point.z() + tick* vect.z(), zMin(), zMax() ) &&
	  inrange( point.y() + tick* vect.y(), yMin(), yMax() ) )
	theticks[ntick++] = tick ;
      tick = (  xHalfLength() - point.x() ) / vect.x() ;
      if( inrange( point.z() + tick* vect.z(), zMin(), zMax() ) &&
	  inrange( point.y() + tick* vect.y(), yMin(), yMax() ) )
	theticks[ntick++] = tick ;
    }
    
    if( ntick != 2) {
      // find intersection ticks with y-planes
      if ( vect.y() != 0 ) {
	tick = ( -yHalfLength() - point.y() ) / vect.y() ;
	if( inrange( point.z() + tick* vect.z(), zMin(), zMax() ) &&
	  inrange( point.x() + tick* vect.x(), xMin(), xMax() ) )
	  theticks[ntick++] = tick ;
	tick = (  yHalfLength() - point.y() ) / vect.y() ;
	if( inrange( point.z() + tick* vect.z(), zMin(), zMax() ) &&
	  inrange( point.x() + tick* vect.x(), xMin(), xMax() ) )
	  theticks[ntick++] = tick ;
      }
    }
  }
  
  if(ntick==2) {
    if( theticks[0] < theticks[1] ) {
      ticks.push_back( theticks[0] ) ;
      ticks.push_back( theticks[1] ) ;
    } else {
      ticks.push_back( theticks[1] ) ;
      ticks.push_back( theticks[0] ) ;
    }
  } else {
    ntick = 0 ;
  }
  
  return ntick;
}

// ===========================================================================
/** - check for the given 3D-point. 
 *    Point coordinated are in the local reference 
 *    frame of the solid.   
 *  - implementation of ISolid absstract interface  
 *  @see ISolid 
 *  @param point point (in local reference system of the solid)
 *  @return true if the point is inside the solid
 */
// ===========================================================================
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
  unsigned int rc = SolidBox::intersectionTicksImpl( Point,Vector,ticks );
  // we can optimize this because in a box there are at 0 or 2 ticks.
  if( rc !=0 ) {
    //assert( rc==2 ) ;
    Tick& first  = ticks[0] ;
    Tick& second = ticks[1] ;
    if( first > tickMax || second < tickMin ) {
      ticks.clear() ;
      rc =0 ;
    } else {
      if( first < tickMin ) first = tickMin ;
      if( second > tickMax) second = tickMax ;
    }
  }
  return rc ;
}

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
       << "]" << endmsg ;
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
