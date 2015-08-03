// ============================================================================
// Include files 
// ============================================================================
// STL & STD 
// ============================================================================
#include <set>
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
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// DetDesc 
// ============================================================================
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/Material.h"
#include "DetDesc/SolidBox.h"
#include "DetDesc/VolumeIntersectionIntervals.h"
#include "DetDesc/IntersectionErrors.h"
#include "DetDesc/ITransportSvc.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/progress.hpp"
#include "boost/format.hpp"
// ============================================================================
/** @file 
 *  Implementation file for class  DetDesc::CheckOverlap
 *  @date 2007-12-12
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
namespace DetDesc
{
  // ==========================================================================
  bool isOK ( const ILVolume::Intersections& cnt ) 
  {
    for ( ILVolume::Intersections::const_iterator i1 = cnt.begin() ; 
          cnt.end() != i1 ; ++i1 ) 
    {
      const double x1 = i1->first.first  ;
      const double x2 = i1->first.second ;
      for ( ILVolume::Intersections::const_iterator i2 = i1 + 1 ; 
            cnt.end() != i2 ; ++i2 ) 
      {
        if ( 0 == VolumeIntersectionIntervals::intersect ( *i1 , *i2 ) ) 
        { return false ; }
        
        const double y1 = i2->first.first  ;
        const double y2 = i2->first.second ;        
        if ( x2 < y1 || y2 < x1 ) { continue ; }
        const double z1 = std::max ( x1 , y1 ) ;
        const double z2 = std::min ( x2 , y2 ) ;
        // ??
        if ( z1 < z2 ) { return false ; }
      } 
    }
    return true ;
  }              
  // ==========================================================================
  std::pair<std::pair<int,int>,double> 
  notOK ( const ILVolume::Intersections& cnt ) 
  {
    for ( ILVolume::Intersections::const_iterator i1 = cnt.begin() ; 
          cnt.end() != i1 ; ++i1 ) 
    {
      const double x1 = i1->first.first  ;
      const double x2 = i1->first.second ;
      for ( ILVolume::Intersections::const_iterator i2 = i1 + 1 ; 
            cnt.end() != i2 ; ++i2 ) 
      {
        const double y1 = i2->first.first  ;
        const double y2 = i2->first.second ;        
        if ( x2 < y1 || y2 < x1 ) { continue ; }
        const double z1 = std::max ( x1 , y1 ) ;
        const double z2 = std::min ( x2 , y2 ) ;        
        if ( z1 < z2 ) 
        { 
          std::pair<int,int> p = std::make_pair
            ( i1 - cnt.begin() , i2 - cnt.begin  () ) ;
          return std::make_pair ( p , z2 - z1 ) ;
        }   
      } 
    }
    return std::pair<std::pair<int,int>,double>() ;
  }              
  // ==========================================================================
  /** @class CheckOverlap
   *  The helper algorithm to detect the various overlaps  
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-12-12
   */
  class CheckOverlap : public GaudiAlgorithm 
  {
    // ========================================================================
    /// the friend factory to instantiate the algorithm
    friend class AlgFactory<DetDesc::CheckOverlap> ;
    // ========================================================================
  public:
    // ========================================================================
    /** standard algorithm initialization
     *  @see IAlgorithm
     *  @return status code 
     */
    virtual StatusCode initialize () ;
    // ========================================================================
    /** standard execution of algorithm 
     *  @see IAlgorithm
     *  @return status code 
     */
    virtual StatusCode execute    () ;
    // ========================================================================
    /** standard finalization of algorithm 
     *  @see IAlgorithm
     *  @return status code 
     */
    virtual StatusCode finalize   () ;
    // ========================================================================
  protected:
    // ========================================================================
    /** Standard constructor
     *  @param name name of the algorithm
     *  @param svcloc pointer to Service Locator 
     */
    CheckOverlap 
    ( const std::string& name   , 
      ISvcLocator*       svcloc ) ;
    /// destructor (protected and virtual)
    virtual ~CheckOverlap(){}; 
    // ========================================================================
  private:
    // ========================================================================
    /// check one volume 
    StatusCode checkVolume 
    ( const ILVolume* volume , const unsigned int level ) const ;
    /// make all shoots 
    StatusCode makeShots   ( const ILVolume* volume ) const ;
    // ========================================================================
  private:
    // ========================================================================
    // volume name 
    std::string         m_volumeName     ; ///< the logical volume name 
    // volume itself 
    const ILVolume*     m_volume         ; ///< volume itself 
    
    // volume limits (for assemblies)
    double              m_minx           ;
    double              m_maxx           ;
    double              m_miny           ;
    double              m_maxy           ;
    double              m_minz           ;
    double              m_maxz           ;
    
    // number of shots  
    int                 m_shots          ; 
    
    // point of shooting for sphere 
    Gaudi::XYZPoint                   m_vertex   ;
    mutable std::set<const ILVolume*> m_checked  ;
  } ;
  // ==========================================================================
} //end of namespace DetDesk
// ============================================================================
/*  Standard constructor
 *  @param name name of the algorithm
 *  @param svcloc pointer to Service Locator 
 */
// ============================================================================
DetDesc::CheckOverlap::CheckOverlap
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : GaudiAlgorithm( name , svcloc ) 
  , m_volumeName   ( "Undefined Volume" )
  , m_volume       ( 0            )
  , m_minx         ( -10 * Gaudi::Units::m      ) 
  , m_maxx         (  10 * Gaudi::Units::m      ) 
  , m_miny         ( -10 * Gaudi::Units::m      ) 
  , m_maxy         (  10 * Gaudi::Units::m      ) 
  , m_minz         ( -10 * Gaudi::Units::m      ) 
  , m_maxz         (  10 * Gaudi::Units::m      )
  , m_shots        ( 10000 )
  , m_vertex       (  ) 
  , m_checked      ()
{
  declareProperty 
    ( "Volume"       , 
      m_volumeName   ,
      "Volume name ot be checked"
      ) ;
  declareProperty ( "MinX"                 , m_minx         ) ;
  declareProperty ( "MinY"                 , m_miny         ) ;
  declareProperty ( "MinZ"                 , m_minz         ) ;
  declareProperty ( "MaxX"                 , m_maxx         ) ;
  declareProperty ( "MaxY"                 , m_maxy         ) ;
  declareProperty ( "MaxZ"                 , m_maxz         ) ;
  declareProperty ( "Shots"                , m_shots        ) ;
  declareProperty ( "Null"                 , m_vertex       ) ;
 }
// ============================================================================
/*  standard algorithm initialization
 *  @see IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode DetDesc::CheckOverlap::initialize() 
{
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  
  Assert ( 0 != randSvc () , "randSvc() points to NULL!" ) ;
  Assert ( 0 != detSvc  () , "detSvc()  points to NULL!" ) ;
  
  if ( !exist<ILVolume> ( detSvc() , m_volumeName ) && 
       exist<ILVolume>  ( detSvc() , "/dd/Geometry/"+ m_volumeName ) )
  {
    m_volumeName = "/dd/Geometry/" + m_volumeName ;
  }
  
  m_volume = getDet<ILVolume> ( m_volumeName ) ;
  
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
  
  /// check for Transport Service 
  svc<ITransportSvc> ( "TransportSvc" , true ) ;
  
  always() << " ATTENTION! THIS ALGORITHM DOES DESTROY THE GEOMETRY TREE" << endmsg ;
  always() << " ATTENTION! NEVER USED IT IN THE REAL JOB OR RELY ON THE " << endmsg ;
  always() << " ATTENTION! RESULT OF ANY OTHER 'GEOMETRY' TOOL/ALGORITHM" << endmsg ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  standard execution of algorithm 
 *  @see IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode DetDesc::CheckOverlap::execute() 
{
  StatusCode sc = checkVolume ( m_volume , 0 ) ;
  
  always() << " ATTENTION! THIS ALGORITHM DOES DESTROY THE GEOMETRY TREE" << endmsg ;
  always() << " ATTENTION! NEVER USED IT IN THE REAL JOB OR RELY ON THE " << endmsg ;
  always() << " ATTENTION! RESULT OF ANY OTHER 'GEOMETRY' TOOL/ALGORITHM" << endmsg ;
  
  return sc ;
}
// ============================================================================
// perform the actual checking of the volume 
// ============================================================================
StatusCode DetDesc::CheckOverlap::checkVolume 
( const ILVolume*    volume , 
  const unsigned int level  ) const 
{
  if ( 0 == volume ) { return Error ( "Invalid pointer to ILVolume!" ) ; }
  
  boost::format fmt ( "%-3d") ;
  const std::string lev = ( fmt % level ).str() ;
  
  // not checked yet ? 
  if ( m_checked.end() != m_checked.find ( volume ) ) 
  { return StatusCode::SUCCESS ; }
  
  
  always() 
    << lev << std::string ( 2*level , ' ' ) 
    << "Checking: " << volume -> name () << endmsg ;
  
  // ==========================================================================
  // loop over all daughter volumes and
  const ILVolume::PVolumes& pvs = volume->pvolumes() ;
  for ( ILVolume::PVolumes::const_iterator ipv = 
          pvs.begin() ; pvs.end() != ipv ; ++ipv )
  {
    const IPVolume* pv = *ipv ;
    if ( 0 == pv ) { return Error ( "IPVolume* points to NULL!" )  ; } // RETURN 
    const ILVolume* lv = pv->lvolume () ;
    if ( 0 == lv ) { return Error ( "ILVolume* points to NULL!" )  ; } // RETURN 
    // check the daughter volume
    StatusCode sc = checkVolume ( lv , level + 1) ;
    if ( sc.isFailure() ) { return sc ; }                              // RETURN 
  }
  // ==========================================================================
  /*  here all daughter volumes are OK and are CLEARED! 
   *  and one can start to make the own  shoots (MUST be efficient!)
   */
  const StatusCode result = makeShots ( volume ) ;
  //
  {
    // ATTENTION!!! 
    // clear daugher volumes :
    ILVolume* vol = const_cast<ILVolume*>( volume ) ;
    vol -> pvolumes().clear() ;
  }
  
  // ==========================================================================
  always() 
    << lev << std::string ( 2*level , ' ' ) 
    << "Checked:  " << volume -> name () << endmsg ;
  
  counter ("#volumes") += 1 ;
  if ( volume->isAssembly() ) { counter ("#assembly") += 1 ; }
  
  m_checked.insert ( volume ) ;
  
  return result ;
}
// ============================================================================
// make the random shoots 
// ============================================================================
StatusCode DetDesc::CheckOverlap::makeShots 
( const ILVolume* volume ) const 
{
  
  if ( 0 == volume ) 
  { return Error ( "Invalid pointer to ILVolume!" ) ; }
  
  // reset the counter of errors 
  DetDesc::IntersectionErrors::setCode ( StatusCode::SUCCESS , volume ) ;

  typedef  std::vector<Gaudi::XYZVector> Vectors ;
  Vectors vcts ;
  
  //
  vcts.push_back ( Gaudi::XYZVector ( 0 , 0 , 1 ) ) ;
  vcts.push_back ( Gaudi::XYZVector ( 0 , 1 , 0 ) ) ;
  vcts.push_back ( Gaudi::XYZVector ( 1 , 0 , 0 ) ) ;
  
  vcts.push_back ( Gaudi::XYZVector ( 1 ,  1 , 0 ) ) ;
  vcts.push_back ( Gaudi::XYZVector ( 1 , -1 , 0 ) ) ;
  vcts.push_back ( Gaudi::XYZVector ( 0 ,  1 , 1 ) ) ;
  vcts.push_back ( Gaudi::XYZVector ( 0 , -1 , 1 ) ) ;
  
  
  double xmin = m_minx ;
  double xmax = m_maxx ;
  double ymin = m_miny ;
  double ymax = m_maxy ;
  double zmin = m_minz ;
  double zmax = m_maxz ;
  
  if ( !volume->isAssembly() && 0 != volume->solid() ) 
  {
    const ISolid* top = volume->solid()->coverTop();
    if( 0 == top ) { return Error ( "CoverTop* points to NULL!" ) ; }
    const SolidBox* box = dynamic_cast<const SolidBox*> ( top );
    if( 0 == box ) { return Error ("SolidBox* points to NULL!"  ) ; }
    
    xmin = -1 * box -> xHalfLength () * 1.01 ;
    xmax =      box -> xHalfLength () * 1.01 ;
    ymin = -1 * box -> yHalfLength () * 1.01 ;
    ymax =      box -> yHalfLength () * 1.01 ;
    zmin = -1 * box -> zHalfLength () * 1.01 ;
    zmax =      box -> zHalfLength () * 1.01 ;
  }
  
  // get the number of shoots 
  int nShots = m_shots ;
  
  // check the simplest cases 
  if ( !volume ->isAssembly() && volume  -> pvolumes().empty() ) 
  {
    const ISolid* solid = volume->solid() ;
    if ( 0 != solid && 0 != dynamic_cast<const SolidBox*> ( solid ) ) 
    {
      // nothing to check, the case is just trivial  
      nShots = 0 ;
      return StatusCode::SUCCESS ;                                   // RETURN 
    }  
  }
  
  { //   
    boost::format fmt ( "Shooting #%8d:") ;
    info () << ( fmt %  nShots ).str() 
            << volume -> name () 
            << " #pv=" << volume->pvolumes().size() 
            << endmsg ;
  }
  
  // display the progress
  boost::progress_display progress ( nShots ) ;
  
  // get the random number generator 
  Rndm::Numbers flat  ( randSvc() , Rndm::Flat (  0.0 , 1.0 ) );
  Rndm::Numbers flat1 ( randSvc() , Rndm::Flat ( -1.0 , 1.0 ) );
  
  for ( int iShoot = 0 ; iShoot < nShots ; ++iShoot ) 
  {
    
    const double x1 = xmin + flat.shoot() * ( xmax - xmin ) ;
    const double y1 = ymin + flat.shoot() * ( ymax - ymin ) ;
    const double z1 = zmin + flat.shoot() * ( zmax - zmin ) ;
    
    const double x2 = xmin + flat.shoot() * ( xmax - xmin ) ;
    const double y2 = ymin + flat.shoot() * ( ymax - ymin ) ;
    const double z2 = zmin + flat.shoot() * ( zmax - zmin ) ;
    
    Gaudi::XYZPoint  point  ( x1 , y1 , z1 ) ;
    Gaudi::XYZPoint  p2     ( x2 , y2 , z2 ) ;
    
    vcts.push_back ( p2       - point  ) ;
    vcts.push_back ( m_vertex - point  ) ;
    vcts.push_back ( m_vertex - point  )  ;
    
    vcts.push_back ( Gaudi::XYZVector ( flat1() , flat1() , flat1() ) )  ;
    vcts.push_back ( Gaudi::XYZVector ( flat1() , flat1() , flat1() ) )  ;
    vcts.push_back ( Gaudi::XYZVector ( flat1() , flat1() , flat1() ) )  ;
    
    vcts.push_back ( Gaudi::XYZVector ( 0 , 0 , 1 + 0.1 * flat1()  ) ) ;
    vcts.push_back ( Gaudi::XYZVector ( 0 , 1 + 0.1 * flat1() , 0  ) ) ; 
    vcts.push_back ( Gaudi::XYZVector ( 1 + 0.1 * flat1() , 0  , 0 ) ) ; 
    
    vcts.push_back ( Gaudi::XYZVector ( 0 , 0 , 1 + 0.1 * flat1()  ) ) ;
    vcts.push_back ( Gaudi::XYZVector ( 0 , 1 + 0.1 * flat1() , 0  ) ) ; 
    vcts.push_back ( Gaudi::XYZVector ( 1 + 0.1 * flat1() , 0  , 0 ) ) ; 
    
    for ( Vectors::const_iterator iv = vcts.begin() ; vcts.end() != iv ; ++iv ) 
    {
      // reset the counter of errors 
      DetDesc::IntersectionErrors::setCode ( StatusCode::SUCCESS , volume ) ;
      
      ILVolume::Intersections intersections;
      volume -> intersectLine ( point , *iv , intersections , 0 );
      
      // get the status
      StatusCode sc = DetDesc::IntersectionErrors::code() ;
      
      if ( sc.isFailure() ) 
      {
        
        error() << "Problem is detected with volume " 
                << volume->name () 
                << " With P/V=" 
                << point << "/" << (*iv) << endmsg ;
        
        DetDesc::IntersectionErrors::inspect
          ( volume , point  , *iv ,  intersections ) ;
        
        return Error ( "Intersection problems with " + volume->name() , sc ) ;
      }
    } // vectors
    // remove last vectors 
    while ( 7 < vcts.size() ) { vcts.pop_back() ; }
    // show the progress  
    ++progress ;
  } // shoots
  
  return StatusCode::SUCCESS ;
}
// ========================================================================
/*  standard finalization of algorithm 
 *  @see IAlgorithm
 *  @return status code 
 */
// ========================================================================
StatusCode DetDesc::CheckOverlap::finalize   () 
{
  always () << " ATTENTION! THIS ALGORITHM DOES DESTROY THE GEOMETRY TREE" << endmsg ;
  always () << " ATTENTION! NEVER USED IT IN THE REAL JOB OR RELY ON THE " << endmsg ;
  always () << " ATTENTION! RESULT OF ANY OTHER 'GEOMETRY' TOOL/ALGORITHM" << endmsg ;
  
  return GaudiAlgorithm::finalize () ;
}
// ============================================================================
/// the factory, nesessary for instantiation 
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DetDesc,CheckOverlap)
// ============================================================================

// ============================================================================
// The END 
// ============================================================================


