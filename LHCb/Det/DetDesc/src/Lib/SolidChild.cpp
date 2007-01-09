// $Id: SolidChild.cpp,v 1.19 2007-01-09 16:01:23 cattanem Exp $ 
// ===========================================================================
/// Geometry definitions
#include "GaudiKernel/Transform3DTypes.h" 
#include "GaudiKernel/Point3DTypes.h" 
/// DetDesc 
#include "DetDesc/SolidChild.h" 
#include "DetDesc/Solid.h"

// ============================================================================
/** @file 
 *  
 *  implementation of class SolidChild
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date xx/xx/xxxx
 */
// ============================================================================

// ============================================================================
/** constructor
 *  @param Name name of this solid 
 */
// ============================================================================
SolidChild::SolidChild( const std::string& Name )
  : SolidBase      ( Name  )
  , m_sc_solid     (  0    )
  , m_sc_matrix    (  0    )
  , m_sc_simple    ( true  )
{};

// ============================================================================
/** constructor    
 *  @param solid pointer to ISolid object
 *  @param mtrx  pointer to transformation 
 *  @param Name name of this solid 
 */ 
// ============================================================================
SolidChild::SolidChild( ISolid*               solid , 
                        const Gaudi::Transform3D* mtrx  ,
                        const std::string&    Name  )
  : SolidBase      ( Name  ) 
  , m_sc_solid     ( solid )
  , m_sc_matrix    (   0   ) 
  , m_sc_simple    ( true  )
{
  if( 0 == solid ) 
    { throw SolidException("SolidChild(), ISolid* points to NULL!");}
  ///
  if( 0 != mtrx && !(Gaudi::Transform3D() == *mtrx) )
    { m_sc_matrix    = new Gaudi::Transform3D( *mtrx ); 
    if( 0 != m_sc_matrix) { m_sc_simple = false ; } }
  else
    { m_sc_simple = true  ; }
  /// set bounding parameters 
  setBP();  
  ///
};
// ============================================================================


// ============================================================================
/// set bounding parameters 
// ============================================================================
StatusCode SolidChild::setBP() 
{  
  const SolidBase* base = dynamic_cast<SolidBase*> (m_sc_solid);
  if( 0 == base ) 
    { throw SolidException("SolidChild::setBP(): ISolid is not SolidBase!");}
  if( m_sc_simple )
    {
      setXMin   ( base->xMin   () );
      setYMin   ( base->yMin   () );
      setZMin   ( base->zMin   () );
      setXMax   ( base->xMax   () );
      setYMax   ( base->yMax   () );
      setZMax   ( base->zMax   () );
      setRMax   ( base->rMax   () );
      setRhoMax ( base->rhoMax () );
    }
  else
    {
      /// position of center of solid child in the mother reference frame 
      const Gaudi::XYZPoint center = (*m_sc_matrix).Inverse() * Gaudi::XYZPoint();
      if( ! (*m_sc_matrix == Gaudi::Transform3D() ) )
        { // rotation 
          setRMax  ( center.r    () + base->rMax   () ) ;
          setRhoMax( std::sqrt(center.perp2()) + base->rMax   () ) ;
          setZMax  ( center.z    () + base->rMax   () ) ;
          setZMin  ( center.z    () - base->rMax   () ) ;
          setXMax  ( center.x    () + base->rMax   () ) ;
          setXMin  ( center.x    () - base->rMax   () ) ;
          setYMax  ( center.y    () + base->rMax   () ) ;
          setYMin  ( center.y    () - base->rMax   () ) ;
        }
      else 
        { // no rotation 
          setRMax  ( center.r    () + base->rMax   () ) ;
          setRhoMax( std::sqrt(center.perp2()) + base->rhoMax () ) ;          
          setZMax  ( center.z    () + base->zMax   () ) ;
          setZMin  ( center.z    () + base->zMin   () ) ;
          setXMax  ( center.x    () + base->xMax   () ) ;
          setXMin  ( center.x    () + base->xMin   () ) ;
          setYMax  ( center.y    () + base->yMax   () ) ;
          setYMin  ( center.y    () + base->yMin   () ) ;
        }
    }
  //
  checkBP();
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** constructor 
 *  @param solid pointer ot ISolid object
 *  @param pos   position
 *  @param rot   rotation
 *  @param Name name of this solid 
 */
// ============================================================================
SolidChild::SolidChild
( ISolid*               solid ,
  const Gaudi::XYZPoint&     pos   , 
  const Gaudi::Rotation3D&    rot   ,
  const std::string&    Name  ) 
  : SolidBase      ( Name  ) 
  , m_sc_solid     ( solid )
  , m_sc_matrix    (   0   ) 
  , m_sc_simple    ( true  )
{
  ////
  if( 0 == solid ) 
    { throw SolidException("SolidChild(), ISolid* points to NULL!");}
  ///
  const Gaudi::XYZPoint NullPoint(0,0,0);
  ///
  if( NullPoint == pos && Gaudi::Rotation3D()==rot ) 
    { m_sc_simple = true  ; }         
  else 
    {
      m_sc_matrix    = 
        new Gaudi::Transform3D(Gaudi::XYZVector(-1.*pos), rot );
      if( 0 != m_sc_matrix ) { m_sc_simple = false ; } 
    }
  /// set bounding parameters 
  setBP();  
  ///
};

// ============================================================================
/// destructor 
// ============================================================================
SolidChild::~SolidChild()
{ 
  if( 0 != m_sc_solid  ){ delete m_sc_solid ; m_sc_solid  = 0; } 
  if( 0 != m_sc_matrix ){ delete m_sc_matrix; m_sc_matrix = 0; } 
};

// ============================================================================
/** printout to STD/STL stream
 *  @param os STD/STL stream
 *  @return reference to the stream
 */
// ============================================================================
std::ostream& SolidChild::printOut     ( std::ostream& os ) const
{
  SolidBase::printOut( os );
  return os << solid(); 
};

// ============================================================================
/** printout to Gaudi  stream
 *  @param os Gaudi stream
 *  @return reference to the stream
 */
// ============================================================================
MsgStream&    SolidChild::printOut     ( MsgStream&    os ) const
{
  SolidBase::printOut( os );
  return os << solid(); 
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
// ============================================================================
unsigned int SolidChild::intersectionTicks( const Gaudi::XYZPoint&  Point  ,
                                            const Gaudi::XYZVector& Vector ,
                                            ISolid::Ticks&     ticks  ) const
{
  return intersectionTicksImpl(Point, Vector, ticks);
};
// ============================================================================
unsigned int SolidChild::intersectionTicks( const Gaudi::Polar3DPoint& Point,
                                            const Gaudi::Polar3DVector& Vector,
                                            ISolid::Ticks&              ticks
                                            ) const
{
  return intersectionTicksImpl(Point, Vector, ticks);
};
// ============================================================================
unsigned int SolidChild::intersectionTicks( const Gaudi::RhoZPhiPoint& Point,
                                            const Gaudi::RhoZPhiVector& Vector,
                                            ISolid::Ticks&              ticks
                                            ) const
{
  return intersectionTicksImpl(Point, Vector, ticks);
};
// ============================================================================
template<class aPoint, class aVector>
unsigned int SolidChild::intersectionTicksImpl ( const aPoint&  Point  ,
                                                 const aVector& Vector ,
                                                 ISolid::Ticks& ticks  ) const
{
  return solid()->
    intersectionTicks(  simple() ? Point  : matrix() * Point  , 
                        simple() ? Vector : matrix() * Vector , 
                        ticks                                 ) ;
};
// ============================================================================
/** reset to the initial ("after constructor") state
 */
// ============================================================================
ISolid*  SolidChild::reset()
{
  SolidBase::reset();
  if( 0 != solid() ){ m_sc_solid->reset() ; } ; 
  return this ; 
};

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
bool SolidChild::isInside ( const Gaudi::XYZPoint& point) const 
{ 
  return isInsideImpl(point);
};
// ============================================================================
bool SolidChild::isInside ( const Gaudi::Polar3DPoint& point) const 
{ 
  return isInsideImpl(point);
};
// ============================================================================
bool SolidChild::isInside ( const Gaudi::RhoZPhiPoint& point) const 
{ 
  return isInsideImpl(point);
};
// ============================================================================
template<class aPoint>
bool SolidChild::isInsideImpl ( const aPoint& point) const 
{ 
  if( isOutBBox( point ) ) { return false ; }
  return  
    isOutBBox( point ) ? false : 
    simple() ? 
    solid()->isInside(point)   : solid()->isInside( matrix() * point ) ;
}; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
