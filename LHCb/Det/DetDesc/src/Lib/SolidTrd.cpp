/// ===========================================================================
/// CVS $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log:
/// ===========================================================================
// DetDesc
#include "DetDesc/SolidTrd.h" 
#include "DetDesc/SolidBox.h" 
#include "DetDesc/SolidTicks.h" 
#include "DetDesc/SolidException.h" 
#include "DetDesc/SolidFactory.h" 

/// ===========================================================================
/** @file SolidTrd.cpp 
 *  
 *  Implementation of class SolidTrd 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date xx/xx/xxx 
 */
/// ===========================================================================

/// ===========================================================================
/// Factory business 
/// ===========================================================================
static const SolidFactory<SolidTrd>         s_Factory;
const       ISolidFactory&SolidTrdFactory = s_Factory;

/// ===========================================================================
/** constructor 
 *  @param name name of the solid object
 *  @param zHalfLength half-length in z-direction 
 *  @param XHalfLength1 half-length in x-direction at z1  
 *  @param YHalfLength1 half-length in y-direction at z1  
 *  @param XHalfLength2 half-length in x-direction at z2  
 *  @param YHalfLength2 half-length in y-direction at z2
 *  @exception SolidException wrong parameter range 
 */ 
/// ===========================================================================
SolidTrd::SolidTrd( const std::string& name              ,
                    const double       ZHalfLength       ,
                    const double       XHalfLength1      , 
                    const double       YHalfLength1      , 
                    const double       XHalfLength2      , 
                    const double       YHalfLength2      ) 
  : SolidPolyHedronHelper ( name         ) 
  , SolidBase             ( name         )
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
  ///
};

/// ===========================================================================
/** constructor
 *  @param name name of trd object
 */
/// ===========================================================================
SolidTrd::SolidTrd( const std::string& name )
  : SolidPolyHedronHelper ( name          )
  , SolidBase             ( name          )
  , m_trd_zHalfLength     ( 10000000      )
  , m_trd_xHalfLength1    ( 10000         )
  , m_trd_xHalfLength2    ( 10000         )
  , m_trd_yHalfLength1    ( 10000         )
  , m_trd_yHalfLength2    ( 10000         )
{
  ///
  makeAll();
  ///
};

/// ===========================================================================
/// destructor 
/// ===========================================================================
SolidTrd::~SolidTrd() { reset(); };

/// ===========================================================================
/** - serialization for reading
 *  - implementation of ISerialize abstract interface 
 *  - reimplementation of SolidBase::serialize 
 *  @see ISerialize 
 *  @see ISolid  
 *  @see SolidBase   
 *  @param      s               reference to stream buffer
 *  @exception  SolidException  wrong parameters range 
 *  @return reference to stream buffer
 */
/// ===========================================================================
StreamBuffer& SolidTrd::serialize( StreamBuffer& s ) 
{
  /// reset  
  reset();
  /// serialize the base class
  SolidBase::serialize( s ) ;
  /// serialize memebers 
  s >> m_trd_zHalfLength  
    >> m_trd_xHalfLength1 
    >> m_trd_xHalfLength2 
    >> m_trd_yHalfLength1 
    >> m_trd_yHalfLength2 ;  
  ///
  if( 0 >= zHalfLength () )
    { throw SolidException("SolidTrd::ZHalfLength is not positive!"); } 
  if( 0 >  xHalfLength1() )
    { throw SolidException("SolidTrd::XHalfLength1 is negative!"); } 
  if( 0 >  xHalfLength2() )
    { throw SolidException("SolidTrd::XHalfLength2 is negative!"); } 
  if( 0 >  yHalfLength1() )
    { throw SolidException("SolidTrd::YHalfLength1 is negative!"); } 
  if( 0 >  yHalfLength2() )
    { throw SolidException("SolidTrd::YHalfLengt2 is negative!"); } 
  if( 0 >=  xHalfLength1() + xHalfLength2() )
    { throw SolidException("SolidTrd::XSUM is not positive "); } 
  if( 0 >=  yHalfLength1() + yHalfLength2() )
    { throw SolidException("SolidTrd::YSUM is not positive "); } 
  ///
  makeAll();
  ///
  return s;
  ///
};

/// ===========================================================================
/** - serialization for writing
 *  - implementation of ISerialize abstract interface 
 *  - reimplementation of SolidBase::serialize 
 *  @see ISerialize 
 *  @see ISolid  
 *  @see SolidBase   
 *  @param s reference to stream buffer
 *  @return reference to stream buffer
 */
/// ===========================================================================
StreamBuffer& SolidTrd::serialize( StreamBuffer& s ) const
{
  /// serialize the base class 
  SolidBase::serialize( s );
  return s << m_trd_zHalfLength  
           << m_trd_xHalfLength1 
           << m_trd_xHalfLength2 
           << m_trd_yHalfLength1 
           << m_trd_yHalfLength2 ;  
};

/// ===========================================================================
/** - retrieve the pointer to "simplified" solid - "cover"
 *    -# The cover for general TRD is "symmetric" TRD 
 *    -# the cover for symmetric TRD is BOX 
 *  - implementation of ISolid abstract interface 
 *  @see ISolid 
 *  @return pointer to "simplified" solid - "cover"
 */
/// ===========================================================================
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
};

/// ===========================================================================
/** initialize the polihedron base 
 *  @exception SolidException wrong parameters  
 */
/// ===========================================================================
void SolidTrd::makeAll()
{   
  reset() ;
  m_ph_planes.clear();
  std::vector<HepPoint3D> points;
  {
    /// construct points (vertices)
    HepPoint3D p0( - xHalfLength1() , - yHalfLength1() , -zHalfLength() ) ;
    HepPoint3D p1( - xHalfLength1() ,   yHalfLength1() , -zHalfLength() ) ; 
    HepPoint3D p2(   xHalfLength1() ,   yHalfLength1() , -zHalfLength() ) ;
    HepPoint3D p3(   xHalfLength1() , - yHalfLength1() , -zHalfLength() ) ; 
    HepPoint3D p4( - xHalfLength1() , - yHalfLength1() ,  zHalfLength() ) ; 
    HepPoint3D p5( - xHalfLength1() ,   yHalfLength1() ,  zHalfLength() ) ; 
    HepPoint3D p6(   xHalfLength1() ,   yHalfLength1() ,  zHalfLength() ) ;
    HepPoint3D p7(   xHalfLength1() , - yHalfLength1() ,  zHalfLength() ) ;
    ///
    points.push_back( p0 ) ; 
    points.push_back( p1 ) ; 
    points.push_back( p2 ) ; 
    points.push_back( p3 ) ; 
    points.push_back( p4 ) ; 
    points.push_back( p5 ) ; 
    points.push_back( p6 ) ; 
    points.push_back( p7 ) ; 
    ///
  }
  if( 8 != points.size() ) 
    { throw SolidException("SolidTrd::makeAll: wrong # vertices!"); } 
  /// make faces
  addFace( points[0] , points[4] , points[5] , points[1] ) ;
  addFace( points[2] , points[3] , points[7] , points[6] ) ;
  addFace( points[1] , points[2] , points[6] , points[5] ) ;
  addFace( points[0] , points[3] , points[7] , points[4] ) ;
  addFace( points[0] , points[1] , points[3] , points[2] ) ; /// bottom face
  addFace( points[5] , points[4] , points[6] , points[7] ) ; /// top    face
  ///
  if( 6 != planes().size() ) 
    { throw SolidException("SolidTrd::makeALL: wrong # faces"); } 
  ///
};

/// ===========================================================================
/** - printout to STD/STL stream    
 *  - implementation  of ISolid abstract interface 
 *  - reimplementation of SolidBase::printOut( std::ostream& )
 *  @see SolidBase 
 *  @see ISolid 
 *  @param os STD/STL stream
 *  @return reference to the stream 
 */
/// ===========================================================================
std::ostream&  SolidTrd::printOut      ( std::ostream&  os ) const
{
  /// serialize the base class 
  SolidBase::printOut( os );
  return 
    os << "[" 
       << " sizeZ[mm]="  << zLength  () / millimeter   
       << " sizeX1[mm]=" << xLength1 () / millimeter 
       << " sizeY1[mm]=" << yLength1 () / millimeter  
       << " sizeX2[mm]=" << xLength2 () / millimeter 
       << " sizeY2[mm]=" << yLength2 () / millimeter << "]" ;
};

/// ===========================================================================
/** - printout to Gaudi MsgStream stream    
 *  - implementation  of ISolid abstract interface 
 *  - reimplementation of SolidBase::printOut( MsgStream& )
 *  @see SolidBase 
 *  @see ISolid 
 *  @param os  Gaudi MsgStream stream
 *  @return reference to the stream 
 */
/// ===========================================================================
MsgStream&     SolidTrd::printOut      ( MsgStream&     os ) const
{
  /// serialize the base class 
  SolidBase::printOut( os );
  return 
    os << "[" 
       << " sizeZ[mm]="  << zLength  () / millimeter   
       << " sizeX1[mm]=" << xLength1 () / millimeter 
       << " sizeY1[mm]=" << yLength1 () / millimeter  
       << " sizeX2[mm]=" << xLength2 () / millimeter 
       << " sizeY2[mm]=" << yLength2 () / millimeter << "]" ;
};

/// ===========================================================================








