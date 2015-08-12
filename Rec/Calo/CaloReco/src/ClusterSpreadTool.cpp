// ============================================================================
// Include files
#include "CaloDet/DeCalorimeter.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "ClusterSpreadTool.h"
// ============================================================================
/** @file ClusterSpreadTool.cpp
 *
 *  Implementation file for class : ClusterSpreadTool
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date 23/11/2001
 */
// ============================================================================
DECLARE_TOOL_FACTORY( ClusterSpreadTool )
// ============================================================================
/*  Standard constructor
 *  @param type tool type (useless)
 *  @param name tool name
 *  @param parent pointer to parent object (service, algorithm or tool)  
 */
// ============================================================================
ClusterSpreadTool::ClusterSpreadTool
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GaudiTool    ( type , name , parent )
  , m_estimator (      )
  , m_detData   ( DeCalorimeterLocation::Ecal )
  , m_det(0){
  declareProperty( "Detector"        , m_detData  );

  // setup calo-dependent property
  m_detData    = LHCb::CaloAlgUtils::DeCaloLocation( name ) ;


  /// declare available interafces 
  declareInterface<ICaloClusterTool>(this);
}
// ============================================================================
// destructor, virtual and protected 
// ============================================================================
ClusterSpreadTool::~ClusterSpreadTool() {} 
// ============================================================================
/*  standard initialization method 
 *  @return status code 
 */
// ============================================================================
StatusCode ClusterSpreadTool::initialize ()
{
  /// initialize the base class 
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class ",sc);}
  ///  
  m_det = getDet<DeCalorimeter>( m_detData) ;
  /// configure the estimator 
  m_estimator.setDetector( m_det ) ;
  ///
  return StatusCode::SUCCESS;
}
// ============================================================================
/*  standard finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode ClusterSpreadTool::finalize   ()
{  
  if ( UNLIKELY(msgLevel ( MSG::DEBUG ) ) )
  {
    debug () << " Corrected Clusters, Ratio : " 
             << m_estimator.invalidRatio  () << endmsg ;
    debug () << " Corrected Clusters, Et    : " 
             << m_estimator.invalidEnergy () << endmsg ;
    debug () << " Corrected Clusters, Cells : " 
             << m_estimator.invalidCells  () << endmsg ;
  }
  /// finalize the base class
  return GaudiTool::finalize ();
}
// ============================================================================
/*  The main processing method 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode ClusterSpreadTool::process    
( LHCb::CaloCluster* cluster ) const { return (*this)( cluster ); }
// ============================================================================
/*  The main processing method (functor interface) 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode ClusterSpreadTool::operator() 
  ( LHCb::CaloCluster* cluster ) const
{
  /// check the argument 
  if( 0 == cluster                ) 
    { return Error( "CaloCluster*   points to NULL!") ; }
  if( 0 == m_estimator.detector() ) 
    { return Error( "DeCalorimeter* points to NULL!") ; }
  /// apply the estimator 
  return m_estimator( cluster );
}
// ============================================================================
// The END 
// ============================================================================

  
