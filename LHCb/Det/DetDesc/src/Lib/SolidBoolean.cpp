// $Id: SolidBoolean.cpp,v 1.9 2002-05-26 10:47:14 ibelyaev Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2002/05/11 18:25:47  ibelyaev
//  see $DETDESCROOT/doc/release.notes 11 May 2002
//
// ===========================================================================
// STD & STL 
#include <functional>
#include <algorithm>
///@{
/** GaudiKernel package */
#include   "GaudiKernel/StatusCode.h"
#include   "GaudiKernel/StreamBuffer.h" 
#include   "GaudiKernel/IInspector.h"
///@}
///@{
/** DetDesc package */  
#include   "DetDesc/SolidBoolean.h" 
#include   "DetDesc/SolidChild.h" 
#include   "DetDesc/SolidException.h"
#include   "DetDesc/ClhepToStream.h"
#include   "DetDesc/SolidTicks.h" 
#include   "DetDesc/Solid.h" 
///@}

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
  /// reset the solid;
  reset() ;
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
StatusCode SolidBoolean::setBP()
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
  
  return checkBP();
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
 *  @param chidl pointer to solid 
 *  @param mtrx  pointer to transformation 
 *  @return status code 
 */
// ============================================================================
StatusCode SolidBoolean::addChild( ISolid*                  child , 
                                   const HepTransform3D*    mtrx  ) 
{
  if( 0 == child  ) { return StatusCode::FAILURE; } 
  SolidChild* pChild = 
    new  SolidChild( child , mtrx , "Child For " + name () ); 
  if( 0 == pChild ) { return StatusCode::FAILURE; }
  m_sb_childrens.push_back(pChild); 
  return StatusCode::SUCCESS; 
};

// ============================================================================
/** add child to daughter container 
 *  @param pointer to solid 
 *  @param position position 
 *  @param rotation rotation 
 */
// ============================================================================
StatusCode SolidBoolean::addChild   ( ISolid*               child    , 
                                      const HepPoint3D&     position , 
                                      const HepRotation&    rotation )
{ 
  if( 0 == child  ) { return StatusCode::FAILURE; } 
  SolidChild* pChild =
    new  SolidChild( child , position , rotation , "Child For " + name () ); 
  if( 0 == pChild ) { return StatusCode::FAILURE; }
  m_sb_childrens.push_back(pChild); 
  return StatusCode::SUCCESS; 
};

// ============================================================================
/** serialization for reading
 *  @param sb reference to stream buffer
 *  @return reference to stream buffer
 */
// ============================================================================
StreamBuffer& SolidBoolean::serialize( StreamBuffer& s ) 
{
  reset();
  /// serialize the base class
  SolidBase::serialize( s ) ;
  /// first solid 
  if( 0 != m_sb_first ) { delete m_sb_first ; m_sb_first = 0 ; }
  std::string type;
  s >> type ;
  m_sb_first = Solid::createSolid( type );
  m_sb_first->serialize( s );
  /// childrens 
  for( SolidBoolean::SolidChildrens::iterator it = childBegin();
       childEnd() != it ; ++it ) 
    { if( 0 != *it ) { delete *it ; } }
  m_sb_childrens.clear(); 
  int n;
  s >> n ;
  while( --n >0 )
    {
      s >> type ;
      ISolid* sol = Solid::createSolid( type );
      sol->serialize( s );
      SolidChild* child =
        dynamic_cast<SolidChild*> (sol);
      if( 0 == child ) 
        { throw SolidException("SolidBoolean: wroing chidl type!"); }
      m_sb_childrens.push_back( child );
    }
  ///
  /// set bounding parameters 
  setBP();
  //
  return s;
};

// ============================================================================
/** serialization for writing
 *  @param sb reference to stream buffer
 *  @return reference to stream buffer
 */
// ============================================================================
StreamBuffer& SolidBoolean::serialize( StreamBuffer& s ) const 
{
  /// serialize the base class
  SolidBase::serialize( s ) ;
  ///
  s << m_sb_first->typeName() ;
  m_sb_first->serialize( s );
  ///
  s << (int) m_sb_childrens.size() ;
  for( SolidBoolean::SolidChildrens::const_iterator 
         it = m_sb_childrens.begin() ; m_sb_childrens.end() != it ; ++it ) 
    {
      s << (*it)->typeName() ;
      (*it)->serialize( s ) ;
    }
  ///
  return s;
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
unsigned int 
SolidBoolean::intersectionTicks 
( const HepPoint3D & point  ,      
  const HepVector3D& vect   ,      
  ISolid::Ticks    & ticks  ) const
{
  ///
  ticks.clear();
  /// line with null direction vector is not able to intersect any solid
  if( vect.mag2() <= 0 ) { return 0; }
  // find intersection with main solid:
  first()->intersectionTicks( point , vect , ticks ); 
  /// find intersections with child solids:
  ISolid::Ticks childTicks; 
  for( SolidChildrens::const_iterator ci = childBegin() ; 
       childEnd() != ci ; ++ci ) 
    {
      const ISolid* child = *ci; 
      if( 0 != child ) 
        { child->intersectionTicks( point , vect, childTicks ); } 
      std::copy( childTicks.begin() , childTicks.end() , 
                 std::back_inserter( ticks ) );
      childTicks.clear();
    } 
  /// sort and remove adjancent and some EXTRA ticks and return 
  return SolidTicks::RemoveAdjancentTicks( ticks , point , vect , *this );  
};

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
unsigned int 
SolidBoolean::intersectionTicks 
( const HepPoint3D  & point   ,
  const HepVector3D & vect    ,
  const ISolid::Tick& tickMin , 
  const ISolid::Tick& tickMax , 
  ISolid::Ticks     & ticks   ) const 
{
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
  os     << " ** 'Main' solid is " << endreq;
  first()->printOut( os )  ;
  for( SolidChildrens::const_iterator child = childBegin() ;
       childEnd() != child ; ++child ) 
    { 
      os << " ** 'Booled' with "   << endreq ;
      (*child)->printOut( os ); 
    }
  return os << std::endl ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
