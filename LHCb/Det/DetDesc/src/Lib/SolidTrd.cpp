// $Id: SolidTrd.cpp,v 1.19 2009-04-17 08:54:24 cattanem Exp $ 
// ===========================================================================

// Units
#include "GaudiKernel/SystemOfUnits.h" 

// DetDesc
#include "DetDesc/DetDesc.h" 
#include "DetDesc/SolidTrd.h" 
#include "DetDesc/SolidBox.h" 
#include "DetDesc/SolidTicks.h" 
#include "DetDesc/SolidException.h" 

// ============================================================================
/** @file SolidTrd.cpp 
 *  
 *  Implementation of class SolidTrd 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date xx/xx/xxx 
 */
// ============================================================================

// ============================================================================
/** constructor 
 *  @param name name of the solid object
 *  @param ZHalfLength half-length in z-direction 
 *  @param XHalfLength1 half-length in x-direction at z1  
 *  @param YHalfLength1 half-length in y-direction at z1  
 *  @param XHalfLength2 half-length in x-direction at z2  
 *  @param YHalfLength2 half-length in y-direction at z2
 *  @exception SolidException wrong parameter range 
 */ 
// ============================================================================
SolidTrd::SolidTrd
( const std::string& name              ,
  const double       ZHalfLength       ,
  const double       XHalfLength1      , 
  const double       YHalfLength1      , 
  const double       XHalfLength2      , 
  const double       YHalfLength2      ) 
  : SolidBase             ( name         ) 
  , SolidPolyHedronHelper ( name         ) 
  , m_trd_zHalfLength     ( ZHalfLength  )
  , m_trd_xHalfLength1    ( XHalfLength1 )
  , m_trd_xHalfLength2    ( XHalfLength2 )
  , m_trd_yHalfLength1    ( YHalfLength1 )
  , m_trd_yHalfLength2    ( YHalfLength2 )
{
  //
  if( 0 >= ZHalfLength  )
    { throw SolidException("SolidTrd::ZHalfLength is not positive!"); } 
  if( 0 >  XHalfLength1 )
    { throw SolidException("SolidTrd::XHalfLength1 is negative!"); } 
  if( 0 >  XHalfLength2 )
    { throw SolidException("SolidTrd::XHalfLength2 is negative!"); } 
  if( 0 >  YHalfLength1 )
    { throw SolidException("SolidTrd::YHalfLength1 is negative!"); } 
  if( 0 >  YHalfLength2 )
    { throw SolidException("SolidTrd::YHalfLengt2 is negative!"); } 
  if( 0 >=  XHalfLength1 + XHalfLength2 )
    { throw SolidException("SolidTrd::XSUM is not positive "); } 
  if( 0 >=  YHalfLength1 + YHalfLength2 )
    { throw SolidException("SolidTrd::YSUM is not positive "); } 
  ///
  makeAll();
  /// set bounding parameters
  setBP();
  ///
  checkTickContainerCapacity() ;
}

// ============================================================================
/** constructor
 *  @param name name of trd object
 */
// ============================================================================
SolidTrd::SolidTrd( const std::string& name )
  : SolidBase             ( name          )
  , SolidPolyHedronHelper ( name          ) 
  , m_trd_zHalfLength     ( 10000000      )
  , m_trd_xHalfLength1    ( 10000         )
  , m_trd_xHalfLength2    ( 10000         )
  , m_trd_yHalfLength1    ( 10000         )
  , m_trd_yHalfLength2    ( 10000         )
{
  ///
  makeAll();
  ///
}

// ============================================================================
/// destructor 
// ============================================================================
SolidTrd::~SolidTrd() {}

// ============================================================================
/** - retrieve the pointer to "simplified" solid - "cover"
 *    -# The cover for general TRD is "symmetric" TRD 
 *    -# the cover for symmetric TRD is BOX 
 *  - implementation of ISolid abstract interface 
 *  @see ISolid 
 *  @return pointer to "simplified" solid - "cover"
 */
// ============================================================================
const ISolid* SolidTrd::cover () const 
{
  /// cover is calculated already 
  if( 0 != m_cover ) { return m_cover; }
  ///
  ISolid* cov = 0 ;
  if(  ( xHalfLength1() != yHalfLength1() )  || 
       ( xHalfLength2() != yHalfLength2() ) ) 
    { cov = 
        new SolidTrd("Cover for " + name() , 
                     zHalfLength() , 
                     xHalfLength1() > yHalfLength1() ? 
                     xHalfLength1() : yHalfLength1() , 
                     xHalfLength1() > yHalfLength1() ? 
                     xHalfLength1() : yHalfLength1() , 
                     xHalfLength2() > yHalfLength2() ? 
                     xHalfLength2() : yHalfLength2() , 
                     xHalfLength2() > yHalfLength2() ? 
                     xHalfLength2() : yHalfLength2() ) ; }
  else
    { cov = 
        new SolidBox("Cover for " + name() , 
                     xHalfLength1() > xHalfLength2() ? 
                     xHalfLength1() : xHalfLength2() , 
                     yHalfLength1() > yHalfLength2() ? 
                     yHalfLength1() : yHalfLength2() , 
                     zHalfLength () ); }
  ///
  if( 0 == cov ) { return this ; } 
  ///
  m_cover = cov; 
  ///
  return m_cover;
}

// ============================================================================
/** initialize the polihedron base 
 *  @exception SolidException wrong parameters  
 */
// ============================================================================
void SolidTrd::makeAll()
{   
  reset() ;
  m_ph_planes.clear();
  m_ph_vertices.clear();
  {
    /// construct points (vertices)
    Gaudi::XYZPoint p0( - xHalfLength1() , - yHalfLength1() , -zHalfLength() ) ;
    Gaudi::XYZPoint p1( - xHalfLength1() ,   yHalfLength1() , -zHalfLength() ) ; 
    Gaudi::XYZPoint p2(   xHalfLength1() ,   yHalfLength1() , -zHalfLength() ) ;
    Gaudi::XYZPoint p3(   xHalfLength1() , - yHalfLength1() , -zHalfLength() ) ; 
    Gaudi::XYZPoint p4( - xHalfLength2() , - yHalfLength2() ,  zHalfLength() ) ; 
    Gaudi::XYZPoint p5( - xHalfLength2() ,   yHalfLength2() ,  zHalfLength() ) ; 
    Gaudi::XYZPoint p6(   xHalfLength2() ,   yHalfLength2() ,  zHalfLength() ) ;
    Gaudi::XYZPoint p7(   xHalfLength2() , - yHalfLength2() ,  zHalfLength() ) ;
    ///
    m_ph_vertices.push_back( p0 ) ; 
    m_ph_vertices.push_back( p1 ) ; 
    m_ph_vertices.push_back( p2 ) ; 
    m_ph_vertices.push_back( p3 ) ; 
    m_ph_vertices.push_back( p4 ) ; 
    m_ph_vertices.push_back( p5 ) ; 
    m_ph_vertices.push_back( p6 ) ; 
    m_ph_vertices.push_back( p7 ) ; 
    ///
  }
  if( 8 != m_ph_vertices.size() ) 
    { throw SolidException("SolidTrd::makeAll: wrong # vertices!"); } 

  /// make faces
  addFace( m_ph_vertices[0] , 
           m_ph_vertices[4] , 
           m_ph_vertices[5] , 
           m_ph_vertices[1] ) ;

  addFace( m_ph_vertices[2] , 
           m_ph_vertices[6] , 
           m_ph_vertices[7] , 
           m_ph_vertices[3] ) ;

  addFace( m_ph_vertices[1] , 
           m_ph_vertices[5] , 
           m_ph_vertices[6] , 
           m_ph_vertices[2] ) ;

  addFace( m_ph_vertices[0] , 
           m_ph_vertices[3] , 
           m_ph_vertices[7] , 
           m_ph_vertices[4] ) ;

  addFace( m_ph_vertices[0] , 
           m_ph_vertices[1] , 
           m_ph_vertices[2] , 
           m_ph_vertices[3] ) ; /// bottom face

  addFace( m_ph_vertices[4] , 
           m_ph_vertices[5] , 
           m_ph_vertices[6] , 
           m_ph_vertices[7] ) ; /// top    face

  ///
  if( 6 != planes().size() ) 
    { throw SolidException("SolidTrd::makeALL: wrong # faces"); } 
  ///
}

// ============================================================================
/** - printout to STD/STL stream    
 *  - implementation  of ISolid abstract interface 
 *  - reimplementation of SolidBase::printOut( std::ostream& )
 *  @see SolidBase 
 *  @see ISolid 
 *  @param os STD/STL stream
 *  @return reference to the stream 
 */
// ============================================================================
std::ostream&  SolidTrd::printOut      ( std::ostream&  os ) const
{
  /// serialize the base class 
  SolidBase::printOut( os );
  return 
    os << "[" 
       << " sizeZ[mm]="  << DetDesc::print( zLength  () / Gaudi::Units::mm )  
       << " sizeX1[mm]=" << DetDesc::print( xLength1 () / Gaudi::Units::mm )  
       << " sizeY1[mm]=" << DetDesc::print( yLength1 () / Gaudi::Units::mm )  
       << " sizeX2[mm]=" << DetDesc::print( xLength2 () / Gaudi::Units::mm ) 
       << " sizeY2[mm]=" << DetDesc::print( yLength2 () / Gaudi::Units::mm )
       << "]" << std::endl ;
}

// ============================================================================
/** - printout to Gaudi MsgStream stream    
 *  - implementation  of ISolid abstract interface 
 *  - reimplementation of SolidBase::printOut( MsgStream& )
 *  @see SolidBase 
 *  @see ISolid 
 *  @param os  Gaudi MsgStream stream
 *  @return reference to the stream 
 */
// ============================================================================
MsgStream&     SolidTrd::printOut      ( MsgStream&     os ) const
{
  /// serialize the base class 
  SolidBase::printOut( os );
  return 
    os << "[" 
       << " sizeZ[mm]="  << DetDesc::print( zLength  () / Gaudi::Units::mm )  
       << " sizeX1[mm]=" << DetDesc::print( xLength1 () / Gaudi::Units::mm )  
       << " sizeY1[mm]=" << DetDesc::print( yLength1 () / Gaudi::Units::mm )  
       << " sizeX2[mm]=" << DetDesc::print( xLength2 () / Gaudi::Units::mm ) 
       << " sizeY2[mm]=" << DetDesc::print( yLength2 () / Gaudi::Units::mm )
       << "]" << endmsg ;
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
