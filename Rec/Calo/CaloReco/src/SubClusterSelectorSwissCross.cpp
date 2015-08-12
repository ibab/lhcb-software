// ============================================================================
// Include files
// Event 
#include "Event/CaloCluster.h"
#include "Event/CaloDigit.h"
// CaloUtils 
#include "CaloUtils/ClusterFunctors.h"
// local
#include "SubClusterSelectorSwissCross.h"

// ============================================================================
/** @file SubClusterSelectorSwissCross.cpp
 *  
 *  Implementation file for class : SubClusterSelectorSwissCross
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 07/11/2001 
 */
// ============================================================================

DECLARE_TOOL_FACTORY( SubClusterSelectorSwissCross )

// ============================================================================
/** Standard Tool Constructor
 *  @param type type of the tool (useless ? )
 *  @param name name of the tool 
 *  @param parent the tool parent 
 */
// ============================================================================

SubClusterSelectorSwissCross::SubClusterSelectorSwissCross( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : SubClusterSelectorBase ( type, name , parent ) 
  , m_matrix ()
{}

// ============================================================================
/** destructor 
 */
// ============================================================================
SubClusterSelectorSwissCross::~SubClusterSelectorSwissCross() {}

// ============================================================================
/** standard initiliazation 
 *  @return status code 
 */
// ============================================================================
StatusCode SubClusterSelectorSwissCross::initialize()
{
  /// initliaze the base class 
  StatusCode sc = SubClusterSelectorBase::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!",sc); }
  if( 0 != det() )
    { m_matrix.setDet( det() ) ; }
  else 
    { return Error("DeCalorimeter* ponts to NULL!"); }
  ///
  return StatusCode::SUCCESS ;
}

// ============================================================================
/** The main processing method  
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode SubClusterSelectorSwissCross::tag ( LHCb::CaloCluster* cluster ) const{


  StatusCode sc = LHCb::ClusterFunctors::tagTheSubCluster( cluster  , 
                                                           m_matrix , 
                                                           modify() , 
                                                           mask(),
                                                           LHCb::CaloDigitStatus::ModifiedBySwissCrossTagger) ;
  //
  if( sc.isFailure() ){ return Error( "Error from 'tagTheSubCluster()'" , sc ); }
  //
  return StatusCode::SUCCESS ;  
}

// ============================================================================
/** The main processing method (untag) 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode SubClusterSelectorSwissCross::untag ( LHCb::CaloCluster* cluster ) const{
  StatusCode sc = 
    LHCb::ClusterFunctors::
    untagTheSubCluster( cluster  , 
                        m_matrix , 
                        LHCb::CaloDigitStatus::ModifiedBySwissCrossTagger );
  //
  if( sc.isFailure() ) 
    { return Error( "Error from 'untagTheSubCluster()'" , sc ); }
  //
  return StatusCode::SUCCESS ;  
}

// ============================================================================
// The End 
// ============================================================================
