// $Id: SubClusterSelectorBase.cpp,v 1.3 2001-11-12 19:04:28 ibelyaev Exp $
// Include files 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/11/08 20:07:05  ibelyaev
//  new tools are added into  the package
//
// Revision 1.1  2001/11/08 10:58:35  ibelyaev
//  new tools are added for selection of subclusters within the cluster
// 
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
#include "CaloEvent/CaloCluster.h"
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
  : CaloTool ( type, name , parent ) 
{};


// ============================================================================
/** destructor
 */
// ============================================================================
SubClusterSelectorBase::~SubClusterSelectorBase() {};

// ============================================================================
/** standard finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode SubClusterSelectorBase::finalize   ()
{ return CaloTool::finalize(); }

// ============================================================================
/** standard initialization method 
 *  @return status code 
 */
// ============================================================================
StatusCode SubClusterSelectorBase::initialize ()
{
  /// initialize the base class
  StatusCode sc = CaloTool::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!",sc);}
  /// check for detector
  if( 0 == det()     )
    {
      if( 0 == detSvc() ) 
        { return Error("Detector Data Service is ivnalid!"); }
      /// 
      SmartDataPtr<DeCalorimeter> calo( detSvc() , detName() );
      if( !calo )
        { return Error("Could not locate detector='"+detName()+"'"); }
      /// set detector
      setDet( calo );
    }
  ///  
  if( 0 == det()     )
    { return Error("Detector '"+detName()+"' is not located!");}
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** Methods for IInterface interface implementation,
 *  Query interface.
 *  @param id   ID of Interface to be retrieved
 *  @param ppI  Pointer to Location for interface pointer
 *  @return status code 
 */
// ============================================================================
StatusCode 
SubClusterSelectorBase::queryInterface( const  InterfaceID& id , 
                                        void** ppI             )
{
  ///
  if( 0 == *ppI ) { return StatusCode::FAILURE; }
  /// check for interfaces
  if( id == ICaloClusterTool::interfaceID() ) 
    { *ppI = static_cast<ICaloClusterTool*> (this); }
  else 
    { return CaloTool::queryInterface( id , ppI ) ; }
  ///
  addRef();
  ///
  return StatusCode::SUCCESS ;
  ///
};

// ============================================================================
/** The main processing method (functor interface) 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode SubClusterSelectorBase::process ( CaloCluster* cluster ) const 
{ return (*this) ( cluster ) ; }

  
// ============================================================================
/** The main processing method with hypothesis 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @param hypo    processing hypothesis 
 *  @return status code 
 */  
// ============================================================================
StatusCode SubClusterSelectorBase::process    
( CaloCluster* cluster                   , 
  const CaloHypotheses::Hypothesis& /* hypo */ ) const 
{
  ///
  Warning("The hypotheses selector is not implemented yet!");
  ///
  return process( cluster );
};

// ============================================================================
/** untag/unselect the all digits in the clusters
 * 
 *  Error codes 
 *    -  225  - cluster points to NULL
 *
 *  @param cluster pointer to cluster 
 *  @return status code 
 */
// ============================================================================
StatusCode 
SubClusterSelectorBase::untag( CaloCluster* cluster ) const
{
  /// check the arguments
  if( 0 == cluster ) { return StatusCode( 225 ) ; }
  /// loop over all digits 
  for( CaloCluster::Digits::iterator iDigit = cluster->digits().begin() ;
       cluster->digits().end() != iDigit ; ++iDigit ) 
    {
      CaloDigitStatus& status = iDigit->second ;
      status.removeStatus ( DigitStatus::UseForEnergy     ) ;
      status.removeStatus ( DigitStatus::UseForPosition   ) ;
      status.removeStatus ( DigitStatus::UseForCovariance ) ;
    }
  ///
  return StatusCode::SUCCESS ;  
};

// ============================================================================
// The End 
// ============================================================================
