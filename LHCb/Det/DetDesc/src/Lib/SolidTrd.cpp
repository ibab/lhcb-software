/// DetDesc
#include "DetDesc/SolidTrd.h" 
#include "DetDesc/SolidBox.h" 
#include "DetDesc/SolidTicks.h" 
#include "DetDesc/SolidException.h" 
//


///////////////////////////////////////////////////////////////////////////////////////////
SolidTrd::SolidTrd( const std::string& name              ,
                    const double       ZHalfLength       ,
                    const double       XHalfLength1      , 
                    const double       YHalfLength1      , 
                    const double       XHalfLength2      , 
                    const double       YHalfLength2      ) 
  : SolidPolyHedronHelper (              ) 
  , m_trd_name            ( name         )
  , m_trd_zHalfLength     ( ZHalfLength  )
  , m_trd_xHalfLength1    ( XHalfLength1 )
  , m_trd_xHalfLength2    ( XHalfLength2 )
  , m_trd_yHalfLength1    ( YHalfLength1 )
  , m_trd_yHalfLength2    ( YHalfLength2 )
  //
  , m_trd_cover           (      0       )
  //
{
  //
  if( 0 >= ZHalfLength  )
    { throw SolidException("SolidTrd constructor::ZHalfLength is not positive!"); } 
  if( 0 >  XHalfLength1 )
    { throw SolidException("SolidTrd constructor::XHalfLength1 is negative!"); } 
  if( 0 >  XHalfLength2 )
    { throw SolidException("SolidTrd constructor::XHalfLength2 is negative!"); } 
  if( 0 >  YHalfLength1 )
    { throw SolidException("SolidTrd constructor::YHalfLength1 is negative!"); } 
  if( 0 >  YHalfLength2 )
    { throw SolidException("SolidTrd constructor::YHalfLengt2 is negative!"); } 
  if( 0 >=  XHalfLength1 + XHalfLength2 )
    { throw SolidException("SolidTrd constructor::XSUM is not positive "); } 
  if( 0 >=  YHalfLength1 + YHalfLength2 )
    { throw SolidException("SolidTrd constructor::YSUM is not positive "); } 
  ///
  makeAll();
  ///
};
/// fictive default construcor /////////////////////////////////////////////////////////////////
SolidTrd::SolidTrd()
  : SolidPolyHedronHelper (               )
  , m_trd_name            ( "unnamed Trd" )
  , m_trd_zHalfLength     ( 10000000      )
  , m_trd_xHalfLength1    ( 10000         )
  , m_trd_xHalfLength2    ( 10000         )
  , m_trd_yHalfLength1    ( 10000         )
  , m_trd_yHalfLength2    ( 10000         )
  //
  , m_trd_cover           (      0       )
  //
{
  ///
  makeAll();
  ///
};
////////////////////////////////////////////////////////////////////////////////////////////
SolidTrd::~SolidTrd()
{
  SolidPolyHedronHelper::reset();
  if( this == m_trd_cover ){                   m_trd_cover = 0 ; } 
  if(    0 != m_trd_cover ){ delete m_trd_cover; m_trd_cover = 0 ; }
};
/// serialization for reading  //////////////////////////////////////////////////////////////
StreamBuffer& SolidTrd::serialize( StreamBuffer& s ) 
{
  ///
  SolidPolyHedronHelper::reset();
  ///
  if( 0 != m_trd_cover ) { delete m_trd_cover ; m_trd_cover = 0 ; } 
  ///
  s >> m_trd_name 
    >> m_trd_zHalfLength  
    >> m_trd_xHalfLength1 
    >> m_trd_xHalfLength2 
    >> m_trd_yHalfLength1 
    >> m_trd_yHalfLength2 ;  
  ///
  if( 0 >= zHalfLength () )
    { throw SolidException("SolidTrd constructor::ZHalfLength is not positive!"); } 
  if( 0 >  xHalfLength1() )
    { throw SolidException("SolidTrd constructor::XHalfLength1 is negative!"); } 
  if( 0 >  xHalfLength2() )
    { throw SolidException("SolidTrd constructor::XHalfLength2 is negative!"); } 
  if( 0 >  yHalfLength1() )
    { throw SolidException("SolidTrd constructor::YHalfLength1 is negative!"); } 
  if( 0 >  yHalfLength2() )
    { throw SolidException("SolidTrd constructor::YHalfLengt2 is negative!"); } 
  if( 0 >=  xHalfLength1() + xHalfLength2() )
    { throw SolidException("SolidTrd constructor::XSUM is not positive "); } 
  if( 0 >=  yHalfLength1() + yHalfLength2() )
    { throw SolidException("SolidTrd constructor::YSUM is not positive "); } 
  ///
  makeAll();
  ///
  return s;
  ///
};
////////////////////////////////////////////////////////////////////////////////////
StreamBuffer& SolidTrd::serialize( StreamBuffer& s ) const
{
  ///
  return s << typeName() 
           << m_trd_name 
           << m_trd_zHalfLength  
           << m_trd_xHalfLength1 
           << m_trd_xHalfLength2 
           << m_trd_yHalfLength1 
           << m_trd_yHalfLength2 ;  
  ///
};
// cover for Trd is "symmetric trd", cover for "symmetric trd" is Box //////////////// 
const ISolid*           SolidTrd::cover         () const 
{
  if( 0 != m_trd_cover ) { return m_trd_cover; }              // cover is calculated already 
  //
  ISolid* cov = 0 ;
  if(  ( xHalfLength1() != yHalfLength1() )  || ( xHalfLength2() != yHalfLength2() ) ) 
    { cov = new SolidTrd("Cover for " + name() , zHalfLength() , 
                         xHalfLength1() > yHalfLength1() ? xHalfLength1() : yHalfLength1() , 
                         xHalfLength1() > yHalfLength1() ? xHalfLength1() : yHalfLength1() , 
                         xHalfLength2() > yHalfLength2() ? xHalfLength2() : yHalfLength2() , 
                                       xHalfLength2() > yHalfLength2() ? xHalfLength2() : yHalfLength2() ) ; }
  else
    { cov = new SolidBox("Cover for " + name() , 
                         xHalfLength1() > xHalfLength2() ? xHalfLength1() : xHalfLength2() , 
                         yHalfLength1() > yHalfLength2() ? yHalfLength1() : yHalfLength2() , 
                         zHalfLength      ()                      ); }
  //
  if( 0 == cov ) { return this ; } 
  m_trd_cover = cov; 
  return m_trd_cover;
};
////////////////////////////////////////////////////////////////////////////////////////
void SolidTrd::makeAll()
{   
  SolidPolyHedronHelper::reset() ; 
  std::vector<HepPoint3D> points;
  ///
  {
    ///
    /// construct points (vertoces)
    ///
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
    { throw SolidException("SolidTrd constructor:: wrong dimension of array of vertices!"); } 
  /// make faces
  addFace( points[0] , points[4] , points[5] , points[1] ) ;
  addFace( points[2] , points[3] , points[7] , points[6] ) ;
  addFace( points[1] , points[2] , points[6] , points[5] ) ;
  addFace( points[0] , points[3] , points[7] , points[4] ) ;
  addFace( points[0] , points[1] , points[3] , points[2] ) ; /// bottom face
  addFace( points[5] , points[4] , points[6] , points[7] ) ; /// top    face
  ///
  if( 6 != planes().size() ) { throw SolidException("SolidTrd constructor::wrong number of constructed faces"); } 
  ///
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SolidTrd::acceptInspector( IInspector* pInspector ) 
{
  ///
  const ISolid* s = this; 
  return s->acceptInspector( pInspector ) ;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SolidTrd::acceptInspector( IInspector* pInspector )  const 
{
  ///
  if( 0 == pInspector ) { return false ; } 
  ///
  pInspector->inspectByRef( &m_trd_name         , this , "TrdName"   ) ; 
  pInspector->inspectByRef( &m_trd_zHalfLength  , this , "X-size/2"  ) ; 
  pInspector->inspectByRef( &m_trd_xHalfLength1 , this , "X1-size/2" ) ; 
  pInspector->inspectByRef( &m_trd_xHalfLength2 , this , "X2-size/2" ) ; 
  pInspector->inspectByRef( &m_trd_yHalfLength1 , this , "Y1-size/2" ) ; 
  pInspector->inspectByRef( &m_trd_yHalfLength2 , this , "Y2-size/2" ) ; 
  ///
  return true; 
  ///
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::ostream&  SolidTrd::printOut      ( std::ostream&  os ) const
{
  return 
    os << "\t" << typeName()   << "\tname=" << name() 
       << std::endl 
       << "\t\t\tzLength [mm] =" << std::setw(12) << zLength        () / millimeter 
       << std::endl 
       << "\t\t\txsize1  [mm] =" << std::setw(12) << xLength1       () / millimeter
       << std::endl 
       << "\t\t\tysize1  [mm] =" << std::setw(12) << yLength1       () / millimeter
       << std::endl 
       << "\t\t\txsize2  [mm] =" << std::setw(12) << xLength2       () / millimeter
       << std::endl 
       << "\t\t\tysize2  [mm] =" << std::setw(12) << yLength2       () / millimeter 
       << std::endl ;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////
MsgStream&     SolidTrd::printOut      ( MsgStream&     os ) const
{
  return 
    os << "\t" << typeName()   << "\tname=" << name() 
       << endreq    
       << "\t\t\tzLength [mm] =" << std::setw(12) << zLength        () / millimeter 
       << endreq    
       << "\t\t\txsize1  [mm] =" << std::setw(12) << xLength1       () / millimeter
       << endreq    
       << "\t\t\tysize1  [mm] =" << std::setw(12) << yLength1       () / millimeter
       << endreq    
       << "\t\t\txsize2  [mm] =" << std::setw(12) << xLength2       () / millimeter
       << endreq    
       << "\t\t\tysize2  [mm] =" << std::setw(12) << yLength2       () / millimeter 
       << endreq    ;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////









