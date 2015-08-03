// ============================================================================
// Include files 
// ============================================================================
// STL & STD 
// ============================================================================
#include <functional>
#include <algorithm>
// ============================================================================
/// from Gaudi
// ============================================================================
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/VectorsAsProperty.h"
// ============================================================================
// DetDesc 
// ============================================================================
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/Material.h"
#include "DetDesc/SolidBox.h"
#include "DetDesc/VolumeIntersectionIntervals.h"
// ============================================================================
// local
// ============================================================================
#include "VolumeCheckAlg.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/progress.hpp"
// ============================================================================
/** @file 
 *  Implementation file for class DetDesc::VolumeCheck
 *  @date 2002-05-25 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================
/*  Standard constructor
 *  @param name name of the algorithm
 *  @param svcloc pointer to Service Locator 
 */
// ============================================================================
DetDesc::VolumeCheck::VolumeCheck
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : GaudiHistoAlg  ( name , svcloc ) 
  , m_volumeName   ( "Undefined Volume" )
  , m_volume       ( 0            )
  , m_minx         ( -10 * Gaudi::Units::m      ) 
  , m_maxx         (  10 * Gaudi::Units::m      ) 
  , m_miny         ( -10 * Gaudi::Units::m      ) 
  , m_maxy         (  10 * Gaudi::Units::m      ) 
  , m_minz         ( -10 * Gaudi::Units::m      ) 
  , m_maxz         (  10 * Gaudi::Units::m      )
  , m_shotsSphere  ( 50000        )
  , m_shotsXY      ( 10000        )  
  , m_shotsYZ      ( 10000        ) 
  , m_shotsZX      ( 10000        ) 
  , m_vertex       (              ) 
{
  declareProperty ( "Volume"               , m_volumeName   ) ;
  declareProperty ( "MinX"                 , m_minx         ) ;
  declareProperty ( "MinY"                 , m_miny         ) ;
  declareProperty ( "MinZ"                 , m_minz         ) ;
  declareProperty ( "MaxX"                 , m_maxx         ) ;
  declareProperty ( "MaxY"                 , m_maxy         ) ;
  declareProperty ( "MaxZ"                 , m_maxz         ) ;
  declareProperty ( "Shots3D"              , m_shotsSphere  ) ;
  declareProperty ( "ShotsXY"              , m_shotsXY      ) ;
  declareProperty ( "ShotsYZ"              , m_shotsYZ      ) ;
  declareProperty ( "ShotsZX"              , m_shotsZX      ) ; 
  declareProperty ( "Null"                 , m_vertex       ) ;
 }
// ============================================================================
// destructor (protected and virtual)
// ============================================================================
DetDesc::VolumeCheck::~VolumeCheck() {}
// ============================================================================
/*  standard algorithm initialization
 *  @see IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode DetDesc::VolumeCheck::initialize() 
{  
  
  StatusCode sc = GaudiHistoAlg::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  
  Assert ( 0 != randSvc () , "randSvc() points to NULL!" ) ;
  Assert ( 0 != detSvc  () , "detSvc()  points to NULL!" ) ;
  
  m_volume = getDet<ILVolume>( m_volumeName ) ;
  
  const std::string stars(80,'*');
  
  info() << stars     << endmsg ;
  info() << m_volume  << endmsg ;
  info() << stars     << endmsg ;
  if ( !m_volume->pvolumes().empty() )
  {
    info() << " Has " << m_volume->pvolumes().size() 
           << " daughter volumes " << endmsg ;
    for ( ILVolume::PVolumes::const_iterator pv = m_volume->pvBegin() ; 
          m_volume->pvEnd() != pv ; ++pv )
    {
      info() << " **** Daughter Physical Volume #"
             << pv - m_volume->pvBegin() << endmsg ;
      info() << *pv << endmsg ;
    }
  }
  
  if ( !m_volume->isAssembly() ) 
  {
    const ISolid* solid       = m_volume->solid() ;
    info() << " **** Solid "    << endmsg ;
    info() << solid             << endmsg ;
    const Material* material  = m_volume->material() ;
    info() << " **** Material " << endmsg ;
    info() << material          << endmsg ;
  }
  
  if ( !m_volume->isAssembly() && 0 != m_volume->solid() ) 
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
  }
  
  return StatusCode::SUCCESS;
}
// ============================================================================
/*  standard execution of algorithm 
 *  @see IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode DetDesc::VolumeCheck::execute() 
{
  // spherical (3d) shots 
  {
    info() << " Start 3D shots #" << m_shotsSphere << endmsg ;    
    // get random number generator 
    Rndm::Numbers flat( randSvc() , Rndm::Flat( -1.0 , 1.0 ) );
    // display the progress 
    boost::progress_display progress ( m_shotsSphere ) ;
    for ( int shot = 0 ; shot < m_shotsSphere ; ++shot )
    {
      Gaudi::XYZVector vect;
      
      double s = 0 ;
      do
      {
        vect.SetX( flat.shoot() ) ;
        vect.SetY( flat.shoot() ) ;
        s = vect.mag2()           ;
      }
      while ( s > 1.0 || s == 0.0 );
      
      vect.SetZ( -1 + 2 * s );
      const double a = 2. * sqrt( 1 - s ) ;
      
      vect.SetX( vect.x() * a ) ;
      vect.SetY( vect.y() * a ) ;
      
      ILVolume::Intersections intersections;
      m_volume->intersectLine ( m_vertex , vect , intersections , 0 );
      
      const double radLength = 
        std::accumulate
        (  intersections.begin()                                  ,  
           intersections.end  ()                                  , 
           0.0                                                    ,  
           VolumeIntersectionIntervals::AccumulateIntersections() );
      
      plot2D ( vect.phi   () / Gaudi::Units::degree , 
               vect.theta () / Gaudi::Units::degree , 
               1      , " 3D-Material Budget     for " + m_volumeName ,
               -180.0 , 180.0                                         , 
               0.0    , 180.0                                         , 
               50     , 50                                            , 
               radLength                                              ) ;
      
      plot2D ( vect.phi   () / Gaudi::Units::degree , 
               vect.theta () / Gaudi::Units::degree , 
               2      , " 3D-Material Budget (N) for " + m_volumeName ,
               -180.0 , 180.0                                         , 
               0.0    , 180.0                                         , 
               50     , 50                                            ) ;
      
      // show the progress
      ++progress ;
    }
  }
  
  // xy - shots 
  {
    info() << " Start XY shots #" << m_shotsXY  << endmsg ;
    // get random number generator 
    Rndm::Numbers flat( randSvc() , Rndm::Flat( 0.0 , 1.0 ) );
    // display the progress 
    boost::progress_display progress ( m_shotsXY ) ;
    for ( int shot = 0 ; shot < m_shotsXY ; ++shot )
    {
      const double x = m_minx + flat.shoot() * ( m_maxx - m_minx ) ;
      const double y = m_miny + flat.shoot() * ( m_maxy - m_miny ) ;
      const double z = m_minz + flat.shoot() * ( m_maxz - m_minz ) ;
      Gaudi::XYZPoint  point( x , y , z ) ;
      Gaudi::XYZVector vect ( 0 , 0 , 1 ) ;
      
      ILVolume::Intersections intersections;
      m_volume->intersectLine ( point , vect , intersections , 0 );
      
      const double radLength = 
        std::accumulate
        (  intersections.begin()                                  ,  
           intersections.end  ()                                  , 
           0.0                                                    ,  
           VolumeIntersectionIntervals::AccumulateIntersections() );
  
      plot2D ( point.x() , point.y()                             , 
               3 , " XY-Material Budget     for " + m_volumeName , 
               m_minx    , m_maxx                                , 
               m_miny    , m_maxy                                , 
               50        , 50                                    , 
               radLength                                         )  ;
      
      plot2D ( point.x() , point.y()                             , 
               4 , " XY-Material Budget (N) for " + m_volumeName , 
               m_minx    , m_maxx                                , 
               m_miny    , m_maxy                                , 
               50        , 50                                    ) ;
      
      // show the progress
      ++progress ;
    }
  }
  
  // yz - shots 
  {
    info() << " Start YZ shots #" << m_shotsYZ << endmsg ;
    // get random number generator 
    Rndm::Numbers flat( randSvc() , Rndm::Flat( 0.0 , 1.0 ) );
    // display the progress 
    boost::progress_display progress ( m_shotsYZ ) ;
    for ( int shot = 0 ; shot < m_shotsYZ ; ++shot )
    {
      const double x = m_minx + flat.shoot() * ( m_maxx - m_minx ) ;
      const double y = m_miny + flat.shoot() * ( m_maxy - m_miny ) ;
      const double z = m_minz + flat.shoot() * ( m_maxz - m_minz ) ;
      Gaudi::XYZPoint  point( x , y , z ) ;
      Gaudi::XYZVector vect ( 1 , 0 , 0 ) ;
      
      ILVolume::Intersections intersections;
      m_volume->intersectLine ( point , vect , intersections , 0 );
      
      const double radLength = 
        std::accumulate
        (  intersections.begin()                                  ,  
           intersections.end  ()                                  , 
           0.0                                                    ,  
           VolumeIntersectionIntervals::AccumulateIntersections() );
      
      plot2D ( point.y() , point.z()                             , 
               5 , " YZ-Material Budget     for " + m_volumeName , 
               m_miny    , m_maxy                                , 
               m_minz    , m_maxz                                , 
               50        , 50                                    , 
               radLength                                         )  ;
      
      plot2D ( point.y() , point.z()                             , 
               6 , " YZ-Material Budget (N) for " + m_volumeName , 
               m_miny    , m_maxy                                , 
               m_minz    , m_maxz                                , 
               50        , 50                                    ) ;
      
      
      // show the progress
      ++progress ;
      
    }
  }
  
  // zx - shoots 
  {
    info()  << " Start ZX shots #" << m_shotsZX << endmsg ;
    // get random number generator 
    Rndm::Numbers flat( randSvc() , Rndm::Flat( 0.0 , 1.0 ) );
    // display the progress 
    boost::progress_display progress ( m_shotsZX ) ;
    for ( int shot = 0 ; shot < m_shotsZX ; ++shot )
    {
      
      const double x = m_minx + flat.shoot() * ( m_maxx - m_minx ) ;
      const double y = m_miny + flat.shoot() * ( m_maxy - m_miny ) ;
      const double z = m_minz + flat.shoot() * ( m_maxz - m_minz ) ;
      Gaudi::XYZPoint  point( x , y , z ) ;
      Gaudi::XYZVector vect ( 0 , 1 , 0 ) ;
      
      ILVolume::Intersections intersections;
      m_volume->intersectLine ( point , vect , intersections , 0 );
      
      const double radLength = 
        std::accumulate
        (  intersections.begin()                                  ,  
           intersections.end  ()                                  , 
           0.0                                                    ,  
           VolumeIntersectionIntervals::AccumulateIntersections() );
      
      plot2D ( point.y() , point.z()                             ,
               7 , " ZX-Material Budget     for " + m_volumeName , 
               m_minz    , m_maxz                                , 
               m_minx    , m_maxx                                , 
               50        , 50                                    , 
               radLength                                         )  ;
      
      plot2D ( point.y() , point.z()                             ,
               8 , " ZX-Material Budget (N) for " + m_volumeName , 
               m_minz    , m_maxz                                , 
               m_minx    , m_maxx                                , 
               50        , 50                                    ) ;
      
      // show the progress
      ++progress ;
      
    }
  }
  
  return StatusCode::SUCCESS;
}
// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DetDesc,VolumeCheck)
// ============================================================================
// The END 
// ============================================================================


