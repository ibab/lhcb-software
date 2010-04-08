// $Id: SolidBoolean.cpp,v 1.20 2009-04-17 08:54:24 cattanem Exp $
// ===========================================================================
// STD & STL 
#include <functional>
#include <algorithm>

/** GaudiKernel package */
#include   "GaudiKernel/StatusCode.h"

/** DetDesc package */  
#include   "DetDesc/SolidBoolean.h" 
#include   "DetDesc/SolidChild.h" 
#include   "DetDesc/SolidException.h"
#include   "DetDesc/SolidTicks.h" 
#include   "DetDesc/Solid.h" 

// ============================================================================
/** @file SolidBoolean.cpp 
 *
 *  implementation file for class SolidBoolean
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru  
 *  @date   xx/xx/xxxx
 */
// ============================================================================

// ============================================================================
/** constructor - "main"("first") solid is mandatory! 
 *  @param name name of the solid 
 *  @param solid pointer to the "first"/"main" solid
 */
// ============================================================================
SolidBoolean::SolidBoolean( const std::string& name  , 
                            ISolid*            solid )
  : SolidBase     ( name  )
  , m_sb_first    ( solid )
  , m_sb_childrens(       )
{
  if( 0 == solid ) 
    { throw SolidException("SolidBoolean:: ISolid* points to NULL!"); }
  /// set bounding parameters 
  setBP();
};

// ============================================================================
/** constructor - "main"("first") solid is mandatory! 
 *  @param name name of the solid 
 */
// ============================================================================
SolidBoolean::SolidBoolean( const std::string& name  ) 
  : SolidBase     ( name  )
  , m_sb_first    (  0    )
  , m_sb_childrens(       )
{};

// ============================================================================
/// destructor 
// ============================================================================
SolidBoolean::~SolidBoolean()
{
  /// delete all daughters 
  for( SolidBoolean::SolidChildrens::iterator it = childBegin();
       childEnd() != it ; ++it ) 
    { if( 0 != *it ) { delete *it ; } }
  m_sb_childrens.clear(); 
  // remove first 
  if ( 0 != m_sb_first  ) { delete m_sb_first ; m_sb_first = 0; }
};

// ============================================================================
/// set bounding parameters 
// ============================================================================
void SolidBoolean::setBP()
{
  const SolidBase* base = dynamic_cast<SolidBase*> (m_sb_first);
  if( 0 == base ) 
    { throw SolidException("SolidChild::setBP(): ISolid is not SolidBase!");}
  
  setXMin   ( base->xMin   () );
  setYMin   ( base->yMin   () );
  setZMin   ( base->zMin   () );
  setXMax   ( base->xMax   () );
  setYMax   ( base->yMax   () );
  setZMax   ( base->zMax   () );
  setRMax   ( base->rMax   () );
  setRhoMax ( base->rhoMax () );
  
  checkBP();
};
// ============================================================================

// ============================================================================
/** reset to the initial ("after constructor") state
 */
// ============================================================================
ISolid* SolidBoolean::reset() 
{
  SolidBase::reset();
  if( 0 != m_sb_first ) { m_sb_first->reset() ; }
  std::for_each( childBegin() , childEnd() ,
                 std::mem_fun(&ISolid::reset) );
  return this;
};  

// ============================================================================
/** add child to daughter container 
 *  @param child pointer to solid 
 *  @param mtrx  pointer to transformation 
 *  @return status code 
 */
// ============================================================================
StatusCode SolidBoolean::addChild( ISolid*                  child , 
                                   const Gaudi::Transform3D*    mtrx  ) 
{
  if( 0 == child  ) { return StatusCode::FAILURE; } 
  SolidChild* pChild = 
    new  SolidChild( child , mtrx , "Child For " + name () ); 
  if( 0 == pChild ) { return StatusCode::FAILURE; }
  m_sb_childrens.push_back(pChild);
  checkTickContainerCapacity() ;
  return StatusCode::SUCCESS; 
};

// ============================================================================
/** add child to daughter container 
 *  @param child    pointer to solid 
 *  @param position position 
 *  @param rotation rotation 
 */
// ============================================================================
StatusCode SolidBoolean::addChild   ( ISolid*               child    , 
                                      const Gaudi::XYZPoint&     position , 
                                      const Gaudi::Rotation3D&    rotation )
{ 
  if( 0 == child  ) { return StatusCode::FAILURE; } 
  SolidChild* pChild =
    new  SolidChild( child , position , rotation , "Child For " + name () ); 
  if( 0 == pChild ) { return StatusCode::FAILURE; }
  m_sb_childrens.push_back(pChild);  
  checkTickContainerCapacity() ;
  return StatusCode::SUCCESS; 
};

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
unsigned int SolidBoolean::intersectionTicks( const Gaudi::XYZPoint& Point,
                                              const Gaudi::XYZVector& Vector,
                                              ISolid::Ticks&    ticks ) const 
{
  return intersectionTicksImpl(Point, Vector, ticks);
};
// ============================================================================
unsigned int SolidBoolean::intersectionTicks( const Gaudi::Polar3DPoint& Point,
                                              const Gaudi::Polar3DVector& Vector,
                                              ISolid::Ticks&    ticks ) const 
{
  return intersectionTicksImpl(Point, Vector, ticks);
};
// ============================================================================
unsigned int SolidBoolean::intersectionTicks( const Gaudi::RhoZPhiPoint& Point,
                                              const Gaudi::RhoZPhiVector& Vector, 
                                              ISolid::Ticks&    ticks ) const 
{
  return intersectionTicksImpl(Point, Vector, ticks);
};
// ============================================================================
template<class aPoint, class aVector>
unsigned int SolidBoolean::intersectionTicksImpl( const aPoint & Point,
                                                  const aVector& Vector,
                                                  ISolid::Ticks& ticks) const
{
  ///
  ticks.clear();
  /// line with null direction vector is not able to intersect any solid
  if( Vector.mag2() <= 0 ) { return 0; }
  // find intersection with main solid:
  first()->intersectionTicks( Point , Vector , ticks ); 
  /// find intersections with child solids:
  ISolid::Ticks childTicks; 
  for( SolidChildrens::const_iterator ci = childBegin() ; 
       childEnd() != ci ; ++ci ) 
    {
      const ISolid* child = *ci; 
      if( 0 != child ) 
        { child->intersectionTicks( Point , Vector, childTicks ); } 
      std::copy( childTicks.begin() , childTicks.end() , 
                 std::back_inserter( ticks ) );
      childTicks.clear();
    } 
  /// sort and remove adjancent and some EXTRA ticks and return 
  return SolidTicks::RemoveAdjancentTicks( ticks , Point , Vector , *this );  
};
// ============================================================================
/** Calculate the maximum number of ticks that a straight line could make with this solid
  *  @return maximum number of ticks
  */
ISolid::Ticks::size_type SolidBoolean::maxNumberOfTicks() const 
{
  ISolid::Ticks::size_type sum =first()->maxNumberOfTicks() ;
  for( SolidChildrens::const_iterator ci = childBegin() ;
       childEnd() != ci ; ++ci ) 
    if(*ci) sum += (*ci)->maxNumberOfTicks() ;
  return sum ;
}
// ============================================================================
/** calculate the intersection points("ticks") with a given line.
 *  Input - line, paramterised by  x_vect = Point + Vector * T
 *  "tick" is just a value of T, at which the intersection occurs
 *  @param Point initial point for the line
 *  @param Vector vector along the line
 *  @param tickMin minimal value of "Tick"
 *  @param tickMax maximal value of "Tick"
 *  @param ticks output container of "Ticks"
 *  @return the number of intersection points (=size of Ticks container)
 *  between tickMin and tickMax
 */
// ============================================================================
unsigned int SolidBoolean::intersectionTicks( const Gaudi::XYZPoint  & Point,
                                           const Gaudi::XYZVector & Vector,
                                           const ISolid::Tick& tickMin,
                                           const ISolid::Tick& tickMax,
                                           ISolid::Ticks     & ticks) const 
{
  return intersectionTicksImpl(Point, Vector, tickMin, tickMax, ticks);
}
// ============================================================================
unsigned int SolidBoolean::intersectionTicks( const Gaudi::Polar3DPoint  & Point,
                                           const Gaudi::Polar3DVector & Vector,
                                           const ISolid::Tick& tickMin,
                                           const ISolid::Tick& tickMax,
                                           ISolid::Ticks     & ticks) const 
{
  return intersectionTicksImpl(Point, Vector, tickMin, tickMax, ticks);
}
// ============================================================================
unsigned int SolidBoolean::intersectionTicks( const Gaudi::RhoZPhiPoint  & Point,
                                           const Gaudi::RhoZPhiVector & Vector,
                                           const ISolid::Tick& tickMin,
                                           const ISolid::Tick& tickMax,
                                           ISolid::Ticks     & ticks) const 
{
  return intersectionTicksImpl(Point, Vector, tickMin, tickMax, ticks);
}
// ============================================================================
template<class aPoint, class aVector>
unsigned int SolidBoolean::intersectionTicksImpl( const aPoint  & point,
                                                  const aVector & vect,
                                                  const ISolid::Tick& tickMin,
                                                  const ISolid::Tick& tickMax,
                                                  ISolid::Ticks& ticks) const 
{
  // check for bounding box 
  if( isOutBBox( point , vect , tickMin , tickMax ) ) { return 0; }
  ///
  intersectionTicks( point , vect , ticks ); 
  // sort and remove adjancent and some EXTRA ticks and return 
  return 
    SolidTicks::RemoveAdjancentTicks( ticks , point , vect , 
                                      tickMin , tickMax , *this );  
};

// ============================================================================
/** printout to STD/STL stream
 *  @param os STD/STL stream
 *  @return reference to the stream
 */
// ============================================================================
std::ostream& SolidBoolean::printOut( std::ostream& os ) const 
{
  // printout the base class 
  SolidBase::printOut( os );
  os     << " ** 'Main' solid is " << std::endl ;
  first()->printOut( os )  ;
  for( SolidChildrens::const_iterator child = childBegin() ;
       childEnd() != child ; ++child ) 
    { 
      os << " ** 'Booled' with "   << std::endl ;
      (*child)->printOut( os ); 
    }
  return os << std::endl ;
};
// ============================================================================

// ============================================================================
/** printout to Gaudi  stream
 *  @param os Gaudi stream
 *  @return reference to the stream
 */
// ============================================================================
MsgStream&    SolidBoolean::printOut( MsgStream&    os ) const 
{
  // printout the base class 
  SolidBase::printOut( os );
  os     << " ** 'Main' solid is " << endmsg;
  first()->printOut( os )  ;
  for( SolidChildrens::const_iterator child = childBegin() ;
       childEnd() != child ; ++child ) 
    { 
      os << " ** 'Booled' with "   << endmsg ;
      (*child)->printOut( os ); 
    }
  return os << endmsg ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
