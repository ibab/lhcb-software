// $Id: ClusterSpreadTool.cpp,v 1.3 2002-04-07 18:15:02 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/12/09 14:33:09  ibelyaev
//  update for newer version of Gaudi
//
// Revision 1.1  2001/11/23 11:44:51  ibelyaev
//  new tool for calculation of cluster spread
// 
// ============================================================================
// Include files
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
// CaloDet 
#include "CaloDet/DeCalorimeter.h"
// local
#include "ClusterSpreadTool.h"

// ============================================================================
/** @file ClusterSpeadTool.cpp
 *
 *  Implementation file for class : ClusterSpreadTool
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date 23/11/2001
 */
// ============================================================================

// ============================================================================
/** factory business
 */
// ============================================================================
static const ToolFactory<ClusterSpreadTool>         s_Factory ;
const       IToolFactory&ClusterSpreadToolFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param type tool type (useless)
 *  @param name tool name
 *  @param parent pointer to parent object (service, algorithm or tool)  
 */
// ============================================================================
ClusterSpreadTool::ClusterSpreadTool
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : CaloTool    ( type , name , parent )
  , m_estimator (      )
{
  /// declare available interafces 
  declareInterface<ICaloClusterTool>(this);
};

// ============================================================================
/** destructor, virtual and protected 
 */
// ============================================================================
ClusterSpreadTool::~ClusterSpreadTool()
{ setDet( (const DeCalorimeter*) 0 ); };

 
// ============================================================================
/** standard initialization method 
 *  @return status code 
 */
// ============================================================================
StatusCode ClusterSpreadTool::initialize ()
{
  /// initialize the base class 
  StatusCode sc = CaloTool::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class ",sc);}
  ///  
  if( 0 == det() ) 
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
  if( 0 == det() ) 
    { return Error("DeCalorimeter* points to NULL!");  }
  /// configure the estimator 
  m_estimator.setDetector( det() ) ;
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** standard finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode ClusterSpreadTool::finalize   ()
{  
  /// remove detector 
  setDet( (const DeCalorimeter*) 0 );
  /// finalize the  the base class
  return CaloTool::finalize ();
};

// ============================================================================
/** The main processing method 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode ClusterSpreadTool::process    
( CaloCluster* cluster ) const 
{ return (*this)( cluster ); };

// ============================================================================
/** The main processing method (functor interface) 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode ClusterSpreadTool::operator() 
  ( CaloCluster* cluster ) const
{
  /// check the argument 
  if( 0 == cluster                ) 
    { return Error( "CaloCluster*   points to NULL!") ; }
  if( 0 == m_estimator.detector() ) 
    { return Error( "DeCalorimeter* points to NULL!") ; }
  /// apply the estimator 
  return m_estimator( cluster );
};

// ============================================================================
// The End 
// ============================================================================
