// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/SolidBoolean.cpp,v 1.4 2001-03-13 11:58:08 mato Exp $

#include   "GaudiKernel/StatusCode.h" 
#include   "GaudiKernel/IInspector.h" 
#include   "DetDesc/SolidBoolean.h" 
#include   "DetDesc/SolidChild.h" 
#include   "DetDesc/SolidException.h" 

#include   "DetDesc/SolidTicks.h" 
#include   "DetDesc/ISolidFromStream.h"
#include   "DetDesc/ClhepToStream.h"

#include   "GaudiKernel/StreamBuffer.h" 


// constructor //////////////////////////////////////////////////////////////////////////////////////
SolidBoolean::SolidBoolean( const std::string& name  , 
                            ISolid*            solid )
  : m_sb_name     ( name  )
  , m_sb_first    ( solid )
  , m_sb_childrens(       )
{};
// destructor  /////////////////////////////////////////////////////////////////////////////////////
SolidBoolean::~SolidBoolean()
{
  // remove all daughters 
  SolidBoolean::SolidChildrens::iterator it = childBegin();
  while( childEnd() != it ) 
    { 
      ISolid* child = *it++; 
      if( 0 != child ) { delete child; } 
    }
  m_sb_childrens.clear(); 
  // remove first 
  if   ( 0 != m_sb_first  ) { delete m_sb_first ; m_sb_first = 0; }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode SolidBoolean::addChild( ISolid*                  child , 
                                   const HepTransform3D*    mtrx  ) 
{
  if( 0 == child  ) { return StatusCode::FAILURE; } 
  SolidChild* pChild = new  SolidChild( child , mtrx ); 
  if( 0 == pChild ) { return StatusCode::FAILURE; }
  m_sb_childrens.push_back(pChild); 
  return StatusCode::SUCCESS; 
};
// add child to daughter container ///////////////////////////////////////////////////////////////// 
StatusCode SolidBoolean::addChild   ( ISolid*               child    , 
                                      const HepPoint3D&     position , 
                                      const HepRotation&    rotation )
{ 
  if( 0 == child  ) { return StatusCode::FAILURE; } 
  SolidChild* pChild = new  SolidChild( child , position , rotation ); 
  if( 0 == pChild ) { return StatusCode::FAILURE; }
  m_sb_childrens.push_back(pChild); 
  return StatusCode::SUCCESS; 
};
/// serialization for reading ////////////////////////////////////////////////////////////////////
StreamBuffer& SolidBoolean::serialize( StreamBuffer& s ) 
{
  s >> m_sb_name ; 
  ISolidFromStream constructor;  
  m_sb_first = constructor( s ) ; 
  unsigned long nC; 
  s >> nC; 
  while( nC-- > 0 )
    {
      HepTransform3D Mtrx;  
      ISolid*        sd = new SolidChild();
      s >> Mtrx >> *sd ;  
      addChild( sd , &Mtrx ) ;
    } 
  ///
  return s;
  ///
};
/// serialization for writing ///////////////////////////////////////////////////////////////////
StreamBuffer& SolidBoolean::serialize( StreamBuffer& s ) const 
{
  ///
  if( 0 == first() ) { throw SolidException("SolidBoolean:: first() points to NULL!") ;} 
  ///
  s << typeName   () << name       ()  << *first      () << noChildrens () ; 
  ///
  for( SolidBoolean::SolidChildrens::const_iterator ci = childBegin(); childEnd() != ci ; ++ci )
    {
      const ISolid* sc = *ci ;  
      s << *sc ; 
    } 
  
  ///
  return s; 
};
/** calculate the intersection points("ticks") with a given line. 
    Input - line, paramterised by (Point + Vector * Tick) 
    "Tick" is just a value of parameter, at which the intersection occurs 
    Return the number of intersection points (=size of Ticks container)   
*/
unsigned int SolidBoolean::intersectionTicks ( const HepPoint3D & point  ,         // initial point for teh line 
                                               const HepVector3D& vect   ,         // vector along the line 
                                               ISolid::Ticks    & ticks  ) const   // output container of "Ticks"
{
  ///
  ticks.clear();
  /// line with null direction vector is not able to intersect any solid
  if( vect.mag2() <= 0 ) { return 0; }
  // find intersection with main solid:
  first()->intersectionTicks( point , vect , ticks ); 
  /// find intersections with child solids:
  ISolid::Ticks childTicks; 
  for( SolidChildrens::const_iterator ci = childBegin() ; childEnd() != ci ; ++ci ) 
    {
      const ISolid* child = *ci; 
      if( 0 != child ) { child->intersectionTicks( point , vect, childTicks ); } 
      std::copy( childTicks.begin(),childTicks.end(),std::back_inserter( ticks ) );
      childTicks.clear();
    } 
  /// sort and remove adjancent and some EXTRA ticks and return 
  return SolidTicks::RemoveAdjancentTicks( ticks , point , vect , *this );  
};
/** calculate the intersection points("ticks") with a given line. 
    Input - line, paramterised by (Point + Vector * Tick) 
    "Tick" is just a value of parameter, at which the intersection occurs 
    Return the number of intersection points (=size of Ticks container)   
*/
unsigned int SolidBoolean::intersectionTicks ( const HepPoint3D  & point   ,         // initial point for teh line 
                                               const HepVector3D & vect    ,         // vector along the line 
                                               const ISolid::Tick& tickMin , 
                                               const ISolid::Tick& tickMax , 
                                               ISolid::Ticks     & ticks   ) const   // output container of "Ticks"
{
  ///
  intersectionTicks( point , vect , ticks ); 
  /// sort and remove adjancent and some EXTRA ticks and return 
  return SolidTicks::RemoveAdjancentTicks( ticks , point , vect , tickMin , tickMax , *this );  
  ///
};
//////////////////////////////////////////////////////////////////////////////////////////
bool SolidBoolean::acceptInspector( IInspector* pInspector ) 
{
  ///
  const ISolid* s = this; 
  return s->acceptInspector( pInspector ) ; 
};
////////////////////////////////////////////////////////////////////////////////////////
bool SolidBoolean::acceptInspector( IInspector* pInspector ) const 
{
  ///
  if( 0 == pInspector ) { return false; } 
  ///
  pInspector->inspectByValue   (  name()         , this , "BooleanName" ) ; 
  pInspector->inspectByValue   (  typeName()     , this , "TypeName"    ) ; 
  ///
  pInspector->inspectByRef     (  m_sb_first     , this , "FirstSolid"  ) ; 
  pInspector->inspectContByRef ( &m_sb_childrens , this , "Childrens"   ) ; 
  ///
  return true; 
  ///
};
/////////////////////////////////////////////////////////////////////////////////////////
std::ostream& SolidBoolean::printOut( std::ostream& os ) const 
{
  os << "\t" << typeName()      << "\tname=" << name()
     << std::endl 
     << "\t\t\tFirst="       << first()
     << "\t\t\tBooled with " << m_sb_childrens.size() << " childrens"
     << std::endl ;
  for( SolidChildrens::const_iterator it = childBegin() ; childEnd() != it ; ++it ) 
    {
      os << "\t\t\t\tchild#" << std::setw(2) << it - childBegin() 
         << std::endl 
         << "\t\t" << *it ;
    } 
  return os;
};
/////////////////////////////////////////////////////////////////////////////////////////
MsgStream&    SolidBoolean::printOut( MsgStream&    os ) const 
{
  os << "\t" << typeName()      << "\tname=" << name()
     << endreq    
     << "\t\t\tFirst="       << first()
     << "\t\t\tBooled with " << m_sb_childrens.size() << " childrens" 
     << endreq    ;
  for( SolidChildrens::const_iterator it = childBegin() ; childEnd() != it ; ++it ) 
    {
      os << "\t\t\t\tchild#" << std::setw(2) << it - childBegin()
         << endreq          
         << "\t\t" << *it ;
    } 
  return os;
};
/////////////////////////////////////////////////////////////////////////////////////////




