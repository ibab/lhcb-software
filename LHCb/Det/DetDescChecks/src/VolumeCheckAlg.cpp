// $Id: VolumeCheckAlg.cpp,v 1.3 2004-06-29 14:14:01 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/03/01 15:03:44  ibelyaev
//  update of the package
//
// Revision 1.1.1.1  2002/05/26 12:47:06  ibelyaev
// New package: collection of components for checks of Detector Description
// 
// ============================================================================
// Include files 
// STL & STD 
#include <functional>
#include <algorithm>
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Vector3D.h"
// AIDA 
#include "AIDA/IHistogram2D.h"
/// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/RndmGenerators.h"
// DetDesc 
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/Material.h"
#include "DetDesc/SolidBox.h"
#include "DetDesc/VolumeIntersectionIntervals.h"
// local
#include "VolumeCheckAlg.h"
#include "DirHbookName.h"

// ============================================================================
/** @file 
 *  Implementation file for class : VolumeCheckAlg
 *  @date 2002-05-25 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

// ============================================================================
/** @var VolumeCheckAlgFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<VolumeCheckAlg>         s_factory ;
const        IAlgFactory&VolumeCheckAlgFactory = s_factory ; 
// ============================================================================


// ============================================================================
/** Standard constructor
 *  @param name name of the algorithm
 *  @param svcloc pointer to Service Locator 
 */
// ============================================================================
VolumeCheckAlg::VolumeCheckAlg
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : GaudiHistoAlg  ( name , svcloc ) 
  , m_volumeName   ( "Undefined Volume" )
  , m_volume       ( 0            )
  , m_minx         ( -10 * m      ) 
  , m_maxx         (  10 * m      ) 
  , m_miny         ( -10 * m      ) 
  , m_maxy         (  10 * m      ) 
  , m_minz         ( -10 * m      ) 
  , m_maxz         (  10 * m      )
  , m_shootsSphere ( 10000        )
  , m_shootsXY     ( 10000        )  
  , m_shootsYZ     ( 10000        ) 
  , m_shootsZX     ( 10000        ) 
  , m_rndmSvcName  ( "RndmGenSvc" )
  , m_rndmSvc      ( 0            )
  , m_vrtx         ( 3 , 0        ) 
  , m_vertex       (              ) 
  , m_sphere       ( 0            ) 
  , m_sphereNorm   ( 0            ) 
  , m_planeXY      ( 0            ) 
  , m_planeXYNorm  ( 0            ) 
  , m_planeYZ      ( 0            ) 
  , m_planeYZNorm  ( 0            ) 
  , m_planeZX      ( 0            ) 
  , m_planeZXNorm  ( 0            )
{
  declareProperty( "Volume"               , m_volumeName   ) ;
  declareProperty( "MinX"                 , m_minx         ) ;
  declareProperty( "MinY"                 , m_miny         ) ;
  declareProperty( "MinZ"                 , m_minz         ) ;
  declareProperty( "MaxX"                 , m_maxx         ) ;
  declareProperty( "MaxY"                 , m_maxy         ) ;
  declareProperty( "MaxZ"                 , m_maxz         ) ;
  declareProperty( "Shoots3D"             , m_shootsSphere ) ;
  declareProperty( "ShootsXY"             , m_shootsXY     ) ;
  declareProperty( "ShootsYZ"             , m_shootsYZ     ) ;
  declareProperty( "ShootsZX"             , m_shootsZX     ) ; 
  declareProperty( "Null"                 , m_vrtx         ) ;
  declareProperty( "RandomNumbersService" , m_rndmSvcName  ) ;
};
// ============================================================================

//=============================================================================
/// destructor (protected and virtual)
//=============================================================================
VolumeCheckAlg::~VolumeCheckAlg() {}; 
// ============================================================================

// ============================================================================
/** standard algorithm initialization
 *  @see IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode VolumeCheckAlg::initialize() 
{  
  
  StatusCode sc = GaudiHistoAlg::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  
  // check the detector service 
  if( 0 == detSvc  () )  
    { return Error( "  detSvc() points to NULL!" ); }

  m_volume = getDet<ILVolume>( m_volumeName ) ;

  const std::string stars(80,'*');
  
  MsgStream& log = info() ;
  
  log << stars     << endreq ;
  log << m_volume  << endreq ;
  log << stars     << endreq ;
  if( !m_volume->pvolumes().empty() )
  {
    log << " Has " << m_volume->pvolumes().size() 
          << " daughter volumes " << endreq ;
    for( ILVolume::PVolumes::const_iterator pv = m_volume->pvBegin() ; 
         m_volume->pvEnd() != pv ; ++pv )
    {
      log << " **** Daughter Physical Volume #"
          << pv - m_volume->pvBegin() << endreq ;
      log << *pv << endreq ;
    }
  }
  
  if( !m_volume->isAssembly() ) 
  {
    const ISolid* solid       = m_volume->solid() ;
    log << " **** Solid "    << endreq ;
    log << solid             << endreq ;
    const Material* material  = m_volume->material() ;
    log << " **** Material " << endreq ;
    log << material          << endreq ;
  }
  
  // activate the vertex
  if( m_vrtx.size() <= 3 )
  { while( 3 != m_vrtx.size() ) { m_vrtx.push_back( 0.0 ); } }
  else 
  {
    warning()  << " Ignore extra fields in 'ShootingPoint' "<< endreq ;
  }
  m_vertex.setX( m_vrtx[0] ) ;
  m_vertex.setY( m_vrtx[1] ) ;
  m_vertex.setZ( m_vrtx[2] ) ;
  
  // prepare for shoots 
  m_rndmSvc = svc<IRndmGenSvc> ( m_rndmSvcName , true );
  
  if( 0 == histoSvc () ) 
  { return Error(" histoSvc() points to NULL!"); }
  
  // book the histogram
  const std::string hbookdir = histoPath() ;
  
  // sphere shoots 
  m_sphere      = 
    histoSvc()->book( hbookdir                   ,  // directory 
                      1                          ,  // id 
                      " 3D-Material Budget for " + m_volumeName ,
                      50                         ,  // bins in phi 
                      -180.                      ,  // min     phi
                      180.                       ,  // max     phi 
                      50                         ,  // bins in theta 
                      0.0                        ,  // min     theta
                      180                        ); // max     theta  
  if( 0 == m_sphere )
  { return Error(" Could not book histogram ' 3D-Material Budget'"); }
  
  m_sphereNorm  = 
    histoSvc()->book( hbookdir                   ,  // directory 
                      2                          ,  // id 
                      " 3D-Material Budget (N) for " + m_volumeName ,
                      50                         ,  // bins in phi 
                      -180.                      ,  // min     phi
                      180.                       ,  // max     phi 
                      50                         ,  // bins in theta 
                      0.0                        ,  // min     theta
                      180                        ); // max     theta
  if( 0 == m_sphereNorm )
  { return Error(" Could not book histogram '3D-Material Budget(Nor)'"); }
  
  if( m_volume->isAssembly() ) 
  {
    m_planeXY       = 
      histoSvc() -> book ( hbookdir               ,  // directory
                           3                      ,  // id  
                           " XY-Material Budget for " + m_volumeName , 
                           50                     ,  // bins in X 
                           m_minx                 ,  // min  X 
                           m_maxx                 ,  // max  X 
                           50                     ,  // bins in Y 
                           m_miny                 ,  // min  Y
                           m_maxy                 ); // max  Y    
    if( 0 == m_planeXY )
    { return Error(" Could not book histogram ' XY-Material Budget'"); }    
    m_planeXYNorm   = 
      histoSvc() -> book ( hbookdir               ,  // directory
                           4                      ,  // id  
                           " XY-Material Budget (N) for " + m_volumeName ,
                           50                     ,  // bins in X 
                           m_minx                 ,  // min  X 
                           m_maxx                 ,  // max  X 
                           50                     ,  // bins in Y 
                           m_miny                 ,  // min  Y
                           m_maxy                 ); // max  Y    
    if( 0 == m_planeXYNorm )
    { return Error(" Could not book histogram ' XY-Material Budget (Norm)'");}
    
    m_planeYZ       = 
      histoSvc() -> book ( hbookdir               ,  // directory
                           5                      ,  // id  
                           " YZ-Material Budget for " + m_volumeName ,
                           50                     ,  // bins in y
                           m_miny                 ,  // min  y 
                           m_maxy                 ,  // max  y 
                           50                     ,  // bins in z 
                           m_minz                 ,  // min  z
                           m_maxz                 ); // max  z    
    if( 0 == m_planeYZ )
    { return Error(" Could not book histogram ' YZ-Material Budget'"); }
    
    m_planeYZNorm   = 
      histoSvc() -> book ( hbookdir               ,  // directory
                           6                      ,  // id  
                           " YZ-Material Budget (N) for " + m_volumeName ,
                           50                     ,  // bins in Y 
                           m_miny                 ,  // min  Y 
                           m_maxy                 ,  // max  Y 
                           50                     ,  // bins in Z 
                           m_minz                 ,  // min  Z
                           m_maxz                 ); // max  Z    
    if( 0 == m_planeYZNorm )
    { return Error(" Could not book histogram ' YZ-Material Budget (Norm)'"); }
    
    m_planeZX       = 
      histoSvc() -> book ( hbookdir               ,  // directory
                           7                      ,  // id  
                           " ZX-Material Budget for " + m_volumeName ,
                           50                     ,  // bins in Z 
                           m_minz                 ,  // min  Z 
                           m_maxz                 ,  // max  Z 
                           50                     ,  // bins in X 
                           m_minx                 ,  // min  X
                           m_maxx                 ); // max  X    
    if( 0 == m_planeZX )
    {
      return Error(" Could not book histogram ' ZX-Material Budget'"); }
    
    m_planeZXNorm   = 
      histoSvc() -> book ( hbookdir               ,  // directory
                           8                      ,  // id  
                           " ZX-Material Budget (N) for " + m_volumeName ,
                           50                     ,  // bins in Z 
                           m_minz                 ,  // min  Z 
                           m_maxz                 ,  // max  Z 
                           50                     ,  // bins in X 
                           m_minx                 ,  // min  X
                           m_maxx                 ); // max  X    
    if( 0 == m_planeZXNorm )
    { return Error(" Could not book histogram ' ZX-Material Budget (Norm)'"); }
  }
  else 
  {
    const ISolid* top = m_volume->solid()->coverTop();
    if( 0 == top ) 
    { return Error( "CoverTop* points to NULL!" ) ; }
    const SolidBox* box = dynamic_cast<const SolidBox*> ( top );
    if( 0 == box ) 
    { return Error("SolidBox* points to NULL!"); }
    
    m_minx = -1 * box->xHalfLength() * 1.05 ;
    m_maxx =      box->xHalfLength() * 1.05 ;
    m_miny = -1 * box->yHalfLength() * 1.05 ;
    m_maxy =      box->yHalfLength() * 1.05 ;
    m_minz = -1 * box->zHalfLength() * 1.05 ;
    m_maxz =      box->zHalfLength() * 1.05 ;
    
    m_planeXY       = 
      histoSvc() -> book ( hbookdir               ,  // directory
                           3                      ,  // id  
                           " XY-Material Budget for " + m_volumeName ,
                           50                     ,  // bins in X 
                           m_minx                 ,  // min  X 
                           m_maxx                 ,  // max  X 
                           50                     ,  // bins in Y 
                           m_miny                 ,  // min  Y
                           m_maxy                 ); // max  Y    
    if( 0 == m_planeXY )
    { return Error(" Could not book histogram ' XY-Material Budget'"); }    
    m_planeXYNorm   = 
      histoSvc() -> book ( hbookdir               ,  // directory
                           4                      ,  // id  
                           " XY-Material Budget (N) for " + m_volumeName ,
                           50                     ,  // bins in X 
                           m_minx                 ,  // min  X 
                           m_maxx                 ,  // max  X 
                           50                     ,  // bins in Y 
                           m_miny                 ,  // min  Y
                           m_maxy                 ); // max  Y    
    if( 0 == m_planeXYNorm )
    { return Error(" Could not book histogram ' XY-Material Budget'"); }    
    
    m_planeYZ       = 
      histoSvc() -> book ( hbookdir               ,  // directory
                           5                      ,  // id  
                           " YZ-Material Budget for " + m_volumeName ,   
                           50                     ,  // bins in y
                           m_miny                 ,  // min  y 
                           m_maxy                 ,  // max  y 
                           50                     ,  // bins in z 
                           m_minz                 ,  // min  z
                           m_maxz                 ); // max  z    
    if( 0 == m_planeYZ )
    { return Error(" Could not book histogram ' XY-Material Budget'"); }    

    m_planeYZNorm   = 
      histoSvc() -> book ( hbookdir               ,  // directory
                           6                      ,  // id  
                           " YZ-Material Budget (N) for " + m_volumeName ,
                           50                     ,  // bins in Y 
                           m_miny                 ,  // min  Y 
                           m_maxy                 ,  // max  Y 
                           50                     ,  // bins in Z 
                           m_minz                 ,  // min  Z
                           m_maxz                 ); // max  Z    
    if( 0 == m_planeYZNorm )
    { return Error(" Could not book histogram ' XY-Material Budget'"); }
    
    m_planeZX       = 
      histoSvc() -> book ( hbookdir               ,  // directory
                           7                      ,  // id  
                           " ZX-Material Budget for " + m_volumeName ,
                           50                     ,  // bins in Z 
                           m_minz                 ,  // min  Z 
                           m_maxz                 ,  // max  Z 
                           50                     ,  // bins in X 
                           m_minx                 ,  // min  X
                           m_maxx                 ); // max  X    
    if( 0 == m_planeZX )
    { return Error(" Could not book histogram ' XY-Material Budget'"); }

    m_planeZXNorm   = 
      histoSvc() -> book ( hbookdir               ,  // directory
                           8                      ,  // id  
                           " ZX-Material Budget (N) for " + m_volumeName ,
                           50                     ,  // bins in Z 
                           m_minz                 ,  // min  Z 
                           m_maxz                 ,  // max  Z 
                           50                     ,  // bins in X 
                           m_minx                 ,  // min  X
                           m_maxx                 ); // max  X    
    if( 0 == m_planeZXNorm )
    { return Error(" Could not book histogram ' XY-Material Budget'"); }

  }
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** standard execution of algorithm 
 *  @see IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode VolumeCheckAlg::execute() 
{
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  // spherical shoots 
  {
    log << MSG::DEBUG << " Start 3D shoots " << endreq ;
    // get random number generator 
    Rndm::Numbers flat( m_rndmSvc , Rndm::Flat( -1.0 , 1.0 ) );
    for( int shoot = 0 ; shoot < m_shootsSphere ; ++shoot )
      {
        if( 0 == shoot%int(m_shootsSphere/10) ) 
          { log << MSG::VERBOSE   << " 3D shoot #" 
                << shoot          << "/" << m_shootsSphere << endreq ; }
        HepVector3D vect;

        double s = 0 ;
        do
          {
            vect.setX( flat.shoot() ) ;
            vect.setY( flat.shoot() ) ;
            s = vect.mag2()           ;
          }
        while ( s > 1.0 || s == 0.0 );
        
        vect.setZ( -1 + 2 * s );
        double a = 2. * sqrt( 1 - s ) ;
        
        vect.setX( vect.x() * a ) ;
        vect.setY( vect.y() * a ) ;

        ILVolume::Intersections intersections;
        m_volume->intersectLine ( m_vertex , vect , intersections , 0 );

        const double radLength = 
          std::accumulate
          (  intersections.begin()                                  ,  
             intersections.end  ()                                  , 
             0.0                                                    ,  
             VolumeIntersectionIntervals::AccumulateIntersections() );

        m_sphere     -> fill ( vect.phi   () / degree , 
                               vect.theta () / degree , radLength );
        m_sphereNorm -> fill ( vect.phi   () / degree , 
                               vect.theta () / degree , 1.0       );
        
      }
  }

  
  // xy - shoots 
  {
    log << MSG::DEBUG << " Start XY shoots " << endreq ;
    // get random number generator 
    Rndm::Numbers flat( m_rndmSvc , Rndm::Flat( 0.0 , 1.0 ) );
    for( int shoot = 0 ; shoot < m_shootsXY ; ++shoot )
      {
        if( 0 == shoot%int(m_shootsXY/10) ) 
          { log << MSG::VERBOSE   << " XY shoot #" 
                << shoot          << "/" << m_shootsXY << endreq ; }
        const double x = m_minx + flat.shoot() * ( m_maxx - m_minx ) ;
        const double y = m_miny + flat.shoot() * ( m_maxy - m_miny ) ;
        const double z = m_minz + flat.shoot() * ( m_maxz - m_minz ) ;
        HepPoint3D  point( x , y , z ) ;
        HepVector3D vect ( 0 , 0 , 1 ) ;
        
        ILVolume::Intersections intersections;
        m_volume->intersectLine ( point , vect , intersections , 0 );
        
        const double radLength = 
          std::accumulate
          (  intersections.begin()                                  ,  
             intersections.end  ()                                  , 
             0.0                                                    ,  
             VolumeIntersectionIntervals::AccumulateIntersections() );

        m_planeXY     -> fill ( point.x() , point.y() , radLength ) ;
        m_planeXYNorm -> fill ( point.x() , point.y() , 1.0       ) ;
        
      }
  }

  // yz - shoots 
  {
    log << MSG::DEBUG << " Start YZ shoots " << endreq ;
    // get random number generator 
    Rndm::Numbers flat( m_rndmSvc , Rndm::Flat( 0.0 , 1.0 ) );
    for( int shoot = 0 ; shoot < m_shootsYZ ; ++shoot )
      {
        if( 0 == shoot%int(m_shootsYZ/10) ) 
          { log << MSG::VERBOSE   << " YZ shoot #" 
                << shoot          << "/" << m_shootsYZ << endreq ; }
        const double x = m_minx + flat.shoot() * ( m_maxx - m_minx ) ;
        const double y = m_miny + flat.shoot() * ( m_maxy - m_miny ) ;
        const double z = m_minz + flat.shoot() * ( m_maxz - m_minz ) ;
        HepPoint3D  point( x , y , z ) ;
        HepVector3D vect ( 1 , 0 , 0 ) ;
        
        ILVolume::Intersections intersections;
        m_volume->intersectLine ( point , vect , intersections , 0 );
        
        const double radLength = 
          std::accumulate
          (  intersections.begin()                                  ,  
             intersections.end  ()                                  , 
             0.0                                                    ,  
             VolumeIntersectionIntervals::AccumulateIntersections() );

        m_planeYZ     -> fill ( point.y() , point.z() , radLength ) ;
        m_planeYZNorm -> fill ( point.y() , point.z() , 1.0       ) ;
        
      }
  }

  // zx - shoots 
  {
    log << MSG::DEBUG << " Start ZX shoots " << endreq ;
    // get random number generator 
    Rndm::Numbers flat( m_rndmSvc , Rndm::Flat( 0.0 , 1.0 ) );
    for( int shoot = 0 ; shoot < m_shootsZX ; ++shoot )
      {
        if( 0 == shoot%int(m_shootsZX/10) ) 
          { log << MSG::VERBOSE   << " ZX shoot #" 
                << shoot          << "/" << m_shootsZX << endreq ; }
        const double x = m_minx + flat.shoot() * ( m_maxx - m_minx ) ;
        const double y = m_miny + flat.shoot() * ( m_maxy - m_miny ) ;
        const double z = m_minz + flat.shoot() * ( m_maxz - m_minz ) ;
        HepPoint3D  point( x , y , z ) ;
        HepVector3D vect ( 0 , 1 , 0 ) ;
        
        ILVolume::Intersections intersections;
        m_volume->intersectLine ( point , vect , intersections , 0 );
        
        const double radLength = 
          std::accumulate
          (  intersections.begin()                                  ,  
             intersections.end  ()                                  , 
             0.0                                                    ,  
             VolumeIntersectionIntervals::AccumulateIntersections() );
        
        m_planeZX     -> fill ( point.z() , point.x() , radLength ) ;
        m_planeZXNorm -> fill ( point.z() , point.x() , 1.0       ) ;
        
      }
  }

  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================


