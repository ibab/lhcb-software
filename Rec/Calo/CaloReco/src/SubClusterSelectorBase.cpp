// ============================================================================
// Include files 
// ============================================================================
// CaloInterfaces
#include "CaloInterfaces/ICaloClusterTool.h"
// CaloDet 
#include "CaloDet/DeCalorimeter.h"
// CaloEvent 
#include "Event/CaloCluster.h"
// local
#include "SubClusterSelectorBase.h"

// ============================================================================
/** @file SubclusterSelectorBase.cpp
 * 
 *  Implementation file for class : SubClusterSelectorBase
 * 
 *  @date 07/11/2001 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 */
// ============================================================================

// ============================================================================
/** Standard Tool Constructor
 *  @param type type of the tool (useless ? )
 *  @param name name of the tool 
 *  @param parent the tool parent 
 */
// ============================================================================
SubClusterSelectorBase::SubClusterSelectorBase( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : GaudiTool  ( type, name , parent ) 
  , m_mask(defaultStatus)
  , m_modify  ( false)
  , m_detData   ( DeCalorimeterLocation::Ecal )
  , m_det(0){
  /// declare the available interfaces
  declareInterface<ICaloClusterTool>   ( this )    ;
  declareInterface<ICaloSubClusterTag> ( this )    ;
  /// 
  declareProperty ( "ModifyFractions" , m_modify ) ;
  declareProperty( "Detector"         , m_detData  );
  

}
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
SubClusterSelectorBase::~SubClusterSelectorBase() {}
// ============================================================================

// ============================================================================
/** standard finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode SubClusterSelectorBase::finalize   ()
{ return GaudiTool::finalize(); }
// ============================================================================

// ============================================================================
/** standard initialization method 
 *  @return status code 
 */
// ============================================================================
StatusCode SubClusterSelectorBase::initialize ()
{
  // initialize the base class
  StatusCode sc = GaudiTool::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!",sc);}
  // load and set the  detector
  m_det = getDet<DeCalorimeter>( m_detData ) ;
  // 
  return StatusCode::SUCCESS;
}
// ============================================================================

// ============================================================================
/** The main processing method (functor interface) 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode SubClusterSelectorBase::process     ( LHCb::CaloCluster* cluster ) const{ 
  return tag ( cluster ) ; 
}  

// ============================================================================

// ============================================================================
/** The main processing method 
 *  @see ICaloSubClusterTag
 *  @see ICaloClusterTool
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode SubClusterSelectorBase::operator() ( LHCb::CaloCluster* cluster ) const{ 
  return tag ( cluster ) ; 
}  
// ============================================================================

