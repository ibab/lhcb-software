// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/SolidTrap.cpp,v 1.5 2001-03-13 11:58:08 mato Exp $

// GaudiKernel 
#include "GaudiKernel/IInspector.h"
#include "GaudiKernel/StreamBuffer.h" 
// DetDesc 
#include "DetDesc/SolidTrd.h" 
#include "DetDesc/SolidBox.h" 
#include "DetDesc/SolidTicks.h" 
#include "DetDesc/SolidException.h" 
#include "DetDesc/SolidTrap.h" 



/// constructor //////////////////////////////////////////////////////////////////////////////////////////////////
SolidTrap::SolidTrap( const std::string&  Name             , /* name of this solid */ 
                      const double        ZHalfLength      , /* half length along z-axes for given solid */
                      const double        Theta            , /* polar angle of "axe" of this trapezoid   */  
                      const double        Phi              , /* asimuthal angle of the "axe" of this trapezoid */   
                      const double        DyAtMinusZ       ,  
                      const double        DxAtMinusZMinusY , 
                      const double        DxAtMinusZPlusY  ,
                      const double        AlphaAtMinusZ    ,
                      const double        DyAtPlusZ        ,  
                      const double        DxAtPlusZMinusY  , 
                      const double        DxAtPlusZPlusY   ,
                      const double        AlphaAtPlusZ    )
  ///
  : SolidPolyHedronHelper   (                  ) 
  , m_trap_name             ( Name             ) 
  ///
  , m_trap_zHalfLength      ( ZHalfLength      ) 
  ///
  , m_trap_theta            ( Theta            ) 
  , m_trap_phi              ( Phi              ) 
  ///
  , m_trap_dyAtMinusZ       ( DyAtMinusZ       ) 
  , m_trap_dxAtMinusZMinusY ( DxAtMinusZMinusY ) 
  , m_trap_dxAtMinusZPlusY  ( DxAtMinusZPlusY  ) 
  , m_trap_alphaAtMinusZ    ( AlphaAtMinusZ    ) 
  ///
  , m_trap_dyAtPlusZ        ( DyAtPlusZ        ) 
  , m_trap_dxAtPlusZMinusY  ( DxAtPlusZMinusY  ) 
  , m_trap_dxAtPlusZPlusY   ( DxAtPlusZPlusY   ) 
  , m_trap_alphaAtPlusZ     ( AlphaAtPlusZ     ) 
  ///
  , m_trap_cover            (  0               ) 
  ///
  , m_trap_vertices         (                  )
  ///
{
  ///
  if( 0 >= ZHalfLength      ) { throw SolidException("SolidTrap constructor::ZHalfLength      is not positive!"   ); } 
  ///
  if( 0 >= DyAtMinusZ       ) { throw SolidException("SolidTrap constructor::dyAtMinusZ       is not positive!"   ); } 
  ///
  if( 0 >= DxAtMinusZMinusY ) { throw SolidException("SolidTrap constructor::dxAtMinusZMinusY is not positive!"   ); } 
  ///
  if( 0 >= DxAtMinusZPlusY  ) { throw SolidException("SolidTrap constructor::dxAtMinusZPlusY  is not positive!"   ); } 
  ///
  if( 0 >= DyAtPlusZ        ) { throw SolidException("SolidTrap constructor::dyAtMinusZ       is not positive!"   ); } 
  ///
  if( 0 >= DxAtPlusZMinusY  ) { throw SolidException("SolidTrap constructor::dxAtMinusZMinusY is not positive!"   ); } 
  ///
  if( 0 >= DxAtPlusZPlusY   ) { throw SolidException("SolidTrap constructor::dxAtMinusZPlusY  is not positive!"   ); } 
  ///  
  makeAll();
  ///
}; 
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void SolidTrap::makeAll()
{   
  ///
  m_trap_vertices.clear() ; 
  SolidPolyHedronHelper::reset() ; 
  ///
  {
    ///
    /// construct points (vertoces)
    /// codes are copied from G4 
    ///
    double fDz         = zHalfLength(); 
    double fTthetaCphi = tan( theta() ) * cos( phi() ); 
    double fTthetaSphi = tan( theta() ) * sin( phi() ); 
    double fDy1        = dyAtMinusZ()           ; 
    double fDx1        = dxAtMinusZMinusY()     ;
    double fDx2        = dxAtMinusZPlusY ()     ; 
    double fTalpha1    = tan( alphaAtMinusZ() ) ; 
    double fDy2        = dyAtPlusZ()            ; 
    double fDx3        = dxAtPlusZMinusY()      ;
    double fDx4        = dxAtPlusZPlusY ()      ; 
    double fTalpha2    = tan( alphaAtPlusZ() )  ; 
    ///
    HepPoint3D p0( -fDz * fTthetaCphi - fDy1 * fTalpha1 - fDx1 , -fDz * fTthetaSphi - fDy1 , -fDz ) ;
    HepPoint3D p1( -fDz * fTthetaCphi - fDy1 * fTalpha1 + fDx1 , -fDz * fTthetaSphi - fDy1 , -fDz ) ;
    HepPoint3D p2( -fDz * fTthetaCphi + fDy1 * fTalpha1 - fDx2 , -fDz * fTthetaSphi + fDy1 , -fDz ) ;
    HepPoint3D p3( -fDz * fTthetaCphi + fDy1 * fTalpha1 + fDx2 , -fDz * fTthetaSphi + fDy1 , -fDz ) ;
    HepPoint3D p4( +fDz * fTthetaCphi - fDy2 * fTalpha2 - fDx3 , +fDz * fTthetaSphi - fDy2 , +fDz ) ;
    HepPoint3D p5( +fDz * fTthetaCphi - fDy2 * fTalpha2 + fDx3 , +fDz * fTthetaSphi - fDy2 , +fDz ) ;
    HepPoint3D p6( +fDz * fTthetaCphi + fDy2 * fTalpha2 - fDx4 , +fDz * fTthetaSphi + fDy2 , +fDz ) ;
    HepPoint3D p7( +fDz * fTthetaCphi + fDy2 * fTalpha2 + fDx4 , +fDz * fTthetaSphi + fDy2 , +fDz ) ;
    ///
    m_trap_vertices.push_back( p0 ) ; 
    m_trap_vertices.push_back( p1 ) ; 
    m_trap_vertices.push_back( p2 ) ; 
    m_trap_vertices.push_back( p3 ) ; 
    m_trap_vertices.push_back( p4 ) ; 
    m_trap_vertices.push_back( p5 ) ; 
    m_trap_vertices.push_back( p6 ) ; 
    ///
  }
  if( 8 != m_trap_vertices.size() ) 
    { throw SolidException("SolidTrap constructor:: wrong dimension of array of vertices!"); } 
  /// make faces
  addFace( point(0) , point(4) , point(5) , point(1) ) ;
  addFace( point(2) , point(3) , point(7) , point(6) ) ;
  addFace( point(0) , point(2) , point(6) , point(4) ) ;
  addFace( point(1) , point(5) , point(7) , point(3) ) ;
  addFace( point(0) , point(1) , point(3) , point(2) ) ; /// bottom face
  addFace( point(5) , point(4) , point(6) , point(7) ) ; /// top    face
  ///
  if( 6 != planes().size() ) { throw SolidException("SolidTrap constructor::wrong number of constructed faces"); } 
  ///
};
/// fictive default constructor /////////////////////////////////////////////////////////////////
SolidTrap::SolidTrap()
  ///
  : SolidPolyHedronHelper   (                  )
  , m_trap_name             ( "unnamed Trap"   ) 
  ///
  , m_trap_zHalfLength      ( 10000000         ) 
  ///
  , m_trap_theta            ( 0                ) 
  , m_trap_phi              ( 0                ) 
  ///
  , m_trap_dyAtMinusZ       ( 100000           ) 
  , m_trap_dxAtMinusZMinusY ( 100000           ) 
  , m_trap_dxAtMinusZPlusY  ( 100000           ) 
  , m_trap_alphaAtMinusZ    ( 0                ) 
  ///
  , m_trap_dyAtPlusZ        ( 100000           ) 
  , m_trap_dxAtPlusZMinusY  ( 100000           ) 
  , m_trap_dxAtPlusZPlusY   ( 100000           ) 
  , m_trap_alphaAtPlusZ     ( 0                ) 
  ///
  , m_trap_cover            ( 0                ) 
  ///
  , m_trap_vertices         (                  )
  ///
{
  ///
  makeAll();
  ///
};
/// serialization ////////////////////////////////////////////////////////////////////////
StreamBuffer& SolidTrap::serialize( StreamBuffer& s ) 
{
  ///
  SolidPolyHedronHelper::reset(); 
  if( 0 != m_trap_cover ) { delete m_trap_cover ; m_trap_cover = 0 ; }  
  m_trap_vertices.clear();
  ///
  s >>  m_trap_name         
    ///
    >> m_trap_zHalfLength    
    ///
    >> m_trap_theta           
    >> m_trap_phi              
    ///
    >> m_trap_dyAtMinusZ       
    >> m_trap_dxAtMinusZMinusY 
    >> m_trap_dxAtMinusZPlusY  
    >> m_trap_alphaAtMinusZ    
    ///
    >> m_trap_dyAtPlusZ        
    >> m_trap_dxAtPlusZMinusY  
    >> m_trap_dxAtPlusZPlusY   
    >> m_trap_alphaAtPlusZ ;    
  ///
  ///
  if( 0 >= zHalfLength()      ) { throw SolidException("SolidTrap constructor::ZHalfLength      is not positive!"   ); } 
  if( 0 >= dyAtMinusZ ()      ) { throw SolidException("SolidTrap constructor::dyAtMinusZ       is not positive!"   ); } 
  if( 0 >= dxAtMinusZMinusY() ) { throw SolidException("SolidTrap constructor::dxAtMinusZMinusY is not positive!"   ); } 
  if( 0 >= dxAtMinusZPlusY () ) { throw SolidException("SolidTrap constructor::dxAtMinusZPlusY  is not positive!"   ); } 
  if( 0 >= dyAtPlusZ       () ) { throw SolidException("SolidTrap constructor::dyAtMinusZ       is not positive!"   ); } 
  if( 0 >= dxAtPlusZMinusY () ) { throw SolidException("SolidTrap constructor::dxAtMinusZMinusY is not positive!"   ); } 
  if( 0 >= dxAtPlusZPlusY  () ) { throw SolidException("SolidTrap constructor::dxAtMinusZPlusY  is not positive!"   ); } 
  ///
  makeAll();
  ///
  return s;
  ///
};
/// serialization //////////////////////////////////////////////////////////////////////////////////////////////
StreamBuffer& SolidTrap::serialize( StreamBuffer& s ) const 
{
  return s << typeName() 
           << m_trap_name         
           << m_trap_zHalfLength    
           << m_trap_theta           
           << m_trap_phi              
           << m_trap_dyAtMinusZ       
           << m_trap_dxAtMinusZMinusY 
           << m_trap_dxAtMinusZPlusY  
           << m_trap_alphaAtMinusZ    
           << m_trap_dyAtPlusZ        
           << m_trap_dxAtPlusZMinusY  
           << m_trap_dxAtPlusZPlusY   
           << m_trap_alphaAtPlusZ  ; 
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
SolidTrap::~SolidTrap()
{
  ///
  SolidPolyHedronHelper::reset() ;
  if( 0 != m_trap_cover ) { delete m_trap_cover ; m_trap_cover = 0 ; }  
  ///
};
/// cover for Trap is Trd ////////////////////////////////////////////////////////////////////////////////////
const ISolid*           SolidTrap::cover         () const 
{
  ///
  if( 0 != m_trap_cover ) { return m_trap_cover; }              // cover is calculated already 
  ///
  double ymx1 = abs( point(0).y() )  ;
  double xmx1 = abs( point(0).x() ) ;
  double ymx2 = abs( point(4).y() )  ;
  double xmx2 = abs( point(4).x() ) ;
  ///  
  for( VERTICES::size_type i = 1 ; i <  4 ; ++i )
    {
      xmx1 = abs( point(i).x() ) > xmx1 ? abs( point(i).x() ) : xmx1 ; 
      ymx1 = abs( point(i).y() ) > ymx1 ? abs( point(i).y() ) : ymx1 ; 
    }  
  for( VERTICES::size_type i1 = 4 ; i1 <  8 ; ++i1 )
    {
      xmx2 = abs( point(i1).x() ) > xmx2 ? abs( point(i1).x() ) : xmx2 ; 
      ymx2 = abs( point(i1).y() ) > ymx2 ? abs( point(i1).y() ) : ymx2 ; 
    }    
  ISolid* cov = new SolidTrd( "Cover for " + name  () , 
                              zHalfLength          () ,
                              xmx1         , ymx1     , 
                              xmx2         , ymx2     );
  m_trap_cover = cov; 
  return m_trap_cover;
};
///////////////////////////////////////////////////////////////////////////
bool SolidTrap::acceptInspector( IInspector* pInspector ) 
{
  ///
  const ISolid* s = this; 
  return s->acceptInspector( pInspector ) ;
};
//////////////////////////////////////////////////////////////////////////
bool SolidTrap::acceptInspector( IInspector* pInspector ) const 
{
  ///
  if( 0 == pInspector ) { return false; } 
  ///
  pInspector->inspectByRef ( &m_trap_name             , this , "TrapName"  ) ;
  pInspector->inspectByRef ( &m_trap_zHalfLength      , this , "Z-size/2"  ) ; 
  pInspector->inspectByRef ( &m_trap_theta            , this , "Theta"     ) ; 
  pInspector->inspectByRef ( &m_trap_phi              , this , "phi"       ) ; 
  pInspector->inspectByRef ( &m_trap_dyAtMinusZ       , this , "DyAt-Z"    ) ; 
  pInspector->inspectByRef ( &m_trap_dxAtMinusZMinusY , this , "DxAt-Z-Y"  ) ; 
  pInspector->inspectByRef ( &m_trap_dxAtMinusZPlusY  , this , "DxAt-Z+Y"  ) ;  
  pInspector->inspectByRef ( &m_trap_alphaAtMinusZ    , this , "AlphaAt-Z" ) ; 
  pInspector->inspectByRef ( &m_trap_dyAtPlusZ        , this , "DyAt+Z"    ) ; 
  pInspector->inspectByRef ( &m_trap_dxAtPlusZMinusY  , this , "DxAt+Z-Y"  ) ; 
  pInspector->inspectByRef ( &m_trap_dxAtPlusZPlusY   , this , "DxAt+Z+Y"  ) ;  
  pInspector->inspectByRef ( &m_trap_alphaAtPlusZ     , this , "AlphaAt+Z" ) ; 
  ///
  return true;
  ///
};
///////////////////////////////////////////////////////////////////////////////
std::ostream&  SolidTrap::printOut      ( std::ostream&  os ) const
{
  return 
    os << "\t" << typeName() << "\tname=" << name()
       << std::endl 
       << "\t\t\tzHalfLength      [mm] =" << std::setw(12) << zHalfLength      () / millimeter 
       << std::endl 
       << "\t\t\ttheta        [degree] =" << std::setw(12) << theta            () / degree     
       << std::endl 
       << "\t\t\tphi          [degree] =" << std::setw(12) << phi              () / degree     
       << std::endl 
       << "\t\t\tdyAtMinusZ       [mm] =" << std::setw(12) << dyAtMinusZ       () / millimeter 
       << std::endl 
       << "\t\t\tdxAtMinusZMinusY [mm] =" << std::setw(12) << dxAtMinusZMinusY () / millimeter 
       << std::endl 
       << "\t\t\tdxAtMinusZPlusY  [mm] =" << std::setw(12) << dxAtMinusZPlusY  () / millimeter 
       << std::endl 
       << "\t\t\talphaAtMinusZ[degree] =" << std::setw(12) << alphaAtMinusZ    () / degree     
       << std::endl 
       << "\t\t\tdyAtPlusZ        [mm] =" << std::setw(12) << dyAtPlusZ        () / millimeter 
       << std::endl 
       << "\t\t\tdxAtPlusZMinusY  [mm] =" << std::setw(12) << dxAtPlusZMinusY  () / millimeter 
       << std::endl 
       << "\t\t\tdxAtPlusZPlusY   [mm] =" << std::setw(12) << dxAtPlusZPlusY   () / millimeter 
       << std::endl 
       << "\t\talphaAtPlusZ   [degree] =" << std::setw(12) << alphaAtPlusZ     () / degree    
       << std::endl ; 
};
///////////////////////////////////////////////////////////////////////////////
MsgStream&     SolidTrap::printOut      ( MsgStream&     os ) const
{
  return 
    os << "\t" << typeName() << "\tname=" << name()
       << endreq    
       << "\t\t\tzHalfLength      [mm] =" << std::setw(12) << zHalfLength      () / millimeter 
       << endreq    
       << "\t\t\ttheta        [degree] =" << std::setw(12) << theta            () / degree     
       << endreq    
       << "\t\t\tphi          [degree] =" << std::setw(12) << phi              () / degree     
       << endreq    
       << "\t\t\tdyAtMinusZ       [mm] =" << std::setw(12) << dyAtMinusZ       () / millimeter 
       << endreq    
       << "\t\t\tdxAtMinusZMinusY [mm] =" << std::setw(12) << dxAtMinusZMinusY () / millimeter 
       << endreq    
       << "\t\t\tdxAtMinusZPlusY  [mm] =" << std::setw(12) << dxAtMinusZPlusY  () / millimeter 
       << endreq    
       << "\t\t\talphaAtMinusZ[degree] =" << std::setw(12) << alphaAtMinusZ    () / degree     
       << endreq    
       << "\t\t\tdyAtPlusZ        [mm] =" << std::setw(12) << dyAtPlusZ        () / millimeter 
       << endreq    
       << "\t\t\tdxAtPlusZMinusY  [mm] =" << std::setw(12) << dxAtPlusZMinusY  () / millimeter 
       << endreq    
       << "\t\t\tdxAtPlusZPlusY   [mm] =" << std::setw(12) << dxAtPlusZPlusY   () / millimeter 
       << endreq    
       << "\t\talphaAtPlusZ   [degree] =" << std::setw(12) << alphaAtPlusZ     () / degree    
       << endreq    ; 
};
////////////////////////////////////////////////////////////////////////////////////////////////



