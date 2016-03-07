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
{
  if( !solid )
    { throw SolidException("SolidBoolean:: ISolid* points to NULL!"); }
  /// set bounding parameters
  setBP();
}

// ============================================================================
/** constructor - "main"("first") solid is mandatory!
 *  @param name name of the solid
 */
// ============================================================================
SolidBoolean::SolidBoolean( const std::string& name  )
  : SolidBase     ( name  )
{}

// ============================================================================
/// destructor
// ============================================================================
SolidBoolean::~SolidBoolean()
{
  /// delete all daughters
  for(auto it = childBegin(); childEnd() != it ; ++it )
  { delete *it ; }
}

// ============================================================================
/// set bounding parameters
// ============================================================================
void SolidBoolean::setBP()
{
  const SolidBase* base = dynamic_cast<SolidBase*> (m_sb_first.get());
  if( !base )
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
}
// ============================================================================

// ============================================================================
/** reset to the initial ("after constructor") state
 */
// ============================================================================
ISolid* SolidBoolean::reset()
{
  SolidBase::reset();
  if( m_sb_first ) { m_sb_first->reset(); }
  std::for_each( childBegin(), childEnd(),
                 [](SolidChild* s) { s->reset(); } );
  return this;
}

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
  if( !child  ) { return StatusCode::FAILURE; }
  SolidChild* pChild =
    new  SolidChild( child , mtrx , "Child For " + name () );
  if( !pChild ) { return StatusCode::FAILURE; }
  m_sb_childrens.push_back(pChild);
  checkTickContainerCapacity() ;
  return StatusCode::SUCCESS;
}

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
  if( !child  ) { return StatusCode::FAILURE; }
  SolidChild* pChild =
    new  SolidChild( child , position , rotation , "Child For " + name () );
  if( !pChild ) { return StatusCode::FAILURE; }
  m_sb_childrens.push_back(pChild);
  checkTickContainerCapacity() ;
  return StatusCode::SUCCESS;
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
unsigned int SolidBoolean::intersectionTicks( const Gaudi::XYZPoint& Point,
                                              const Gaudi::XYZVector& Vector,
                                              ISolid::Ticks&    ticks ) const
{
  return intersectionTicksImpl(Point, Vector, ticks);
}
// ============================================================================
unsigned int SolidBoolean::intersectionTicks( const Gaudi::Polar3DPoint& Point,
                                              const Gaudi::Polar3DVector& Vector,
                                              ISolid::Ticks&    ticks ) const
{
  return intersectionTicksImpl(Point, Vector, ticks);
}
// ============================================================================
unsigned int SolidBoolean::intersectionTicks( const Gaudi::RhoZPhiPoint& Point,
                                              const Gaudi::RhoZPhiVector& Vector,
                                              ISolid::Ticks&    ticks ) const
{
  return intersectionTicksImpl(Point, Vector, ticks);
}
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
  for( auto ci = childBegin() ; childEnd() != ci ; ++ci )
    {
      const ISolid* child = *ci;
      if( child )
        { child->intersectionTicks( Point , Vector, childTicks ); }
      std::copy( childTicks.begin() , childTicks.end() ,
                 std::back_inserter( ticks ) );
      childTicks.clear();
    }
  /// sort and remove adjancent and some EXTRA ticks and return
  return SolidTicks::RemoveAdjancentTicks( ticks , Point , Vector , *this );
}
// ============================================================================
/** Calculate the maximum number of ticks that a straight line could make with this solid
  *  @return maximum number of ticks
  */
ISolid::Ticks::size_type SolidBoolean::maxNumberOfTicks() const
{
  return std::accumulate( childBegin(), childEnd(),
                          first()->maxNumberOfTicks(),
                          [](ISolid::Ticks::size_type sum,const SolidChild* c) {
              return c ? sum + c->maxNumberOfTicks() : sum ;
  });

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
}

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
  os     << " ** 'Main' solid is \n";
  first()->printOut( os )  ;
  for( const auto& child : m_sb_childrens )
    {
      os << " ** 'Booled' with \n";
      child->printOut( os );
    }
  return os << std::endl ;
}
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
  for( const auto& child :m_sb_childrens)
    {
      os << " ** 'Booled' with "   << endmsg ;
      child->printOut( os );
    }
  return os << endmsg ;
}
// ============================================================================

// ============================================================================
// The END
// ============================================================================
