// $Id: SubClusterSelectorBase.cpp,v 1.1.1.1 2002-11-13 20:46:43 ibelyaev Exp $
// Include files 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2002/04/23 10:49:03  ibelyaev
// ============================================================================
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
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
  : CaloTool  ( type, name , parent ) 
  , m_modify  ( false               )
{
  /// declare the available interfaces
  declareInterface<ICaloClusterTool>   ( this )    ;
  declareInterface<ICaloSubClusterTag> ( this )    ;
  /// 
  declareProperty ( "ModifyFractions" , m_modify ) ;
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
SubClusterSelectorBase::~SubClusterSelectorBase() {};
// ============================================================================

// ============================================================================
/** standard finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode SubClusterSelectorBase::finalize   ()
{ return CaloTool::finalize(); }
// ============================================================================

// ============================================================================
/** standard initialization method 
 *  @return status code 
 */
// ============================================================================
StatusCode SubClusterSelectorBase::initialize ()
{
  // initialize the base class
  StatusCode sc = CaloTool::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!",sc);}
  // load and set the  detector
  setDet( get( detSvc() , detName() , (DeCalorimeter*) 0 ) );
  // 
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** The main processing method (functor interface) 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode SubClusterSelectorBase::process     ( CaloCluster* cluster ) const 
{ return tag ( cluster ) ; }  
// ============================================================================

// ============================================================================
/** The main processing method 
 *  @see ICaloSubClusterTag
 *  @see ICaloClusterTool
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode SubClusterSelectorBase::operator() ( CaloCluster* cluster ) const  
{ return tag ( cluster ) ; }  
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
