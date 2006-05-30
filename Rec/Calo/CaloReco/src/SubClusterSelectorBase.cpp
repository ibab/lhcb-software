// $Id: SubClusterSelectorBase.cpp,v 1.4 2006-05-30 09:42:06 odescham Exp $
// Include files 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2005/11/07 12:12:44  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.2  2004/02/17 12:08:11  ibelyaev
//  update for new CaloKernel and CaloInterfaces
//
// Revision 1.1.1.1  2002/11/13 20:46:43  ibelyaev
// new package 
//
// Revision 1.6  2002/04/23 10:49:03  ibelyaev
// ============================================================================
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
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
  , m_modify  ( false               )
  , m_detData   ( DeCalorimeterLocation::Ecal )
{
  /// declare the available interfaces
  declareInterface<ICaloClusterTool>   ( this )    ;
  declareInterface<ICaloSubClusterTag> ( this )    ;
  /// 
  declareProperty ( "ModifyFractions" , m_modify ) ;
  declareProperty( "Detector"         , m_detData  );
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
};
// ============================================================================

// ============================================================================
/** The main processing method (functor interface) 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode SubClusterSelectorBase::process     ( LHCb::CaloCluster* cluster ) const 
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
StatusCode SubClusterSelectorBase::operator() ( LHCb::CaloCluster* cluster ) const  
{ return tag ( cluster ) ; }  
// ============================================================================

