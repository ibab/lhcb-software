// $Id: ClusterCovarianceMatrixTool.cpp,v 1.1 2001-11-04 15:30:40 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
// CaloDet 
#include "CaloDet/DeCalorimeter.h"
// CaloUtil
#include "CaloUtils/CovarianceEstimator.h"
// local
#include "ClusterCovarianceMatrixTool.h"

// ============================================================================
/** @file ClusterCovarianceMatrixTool.cpp
 *
 *  Implementation file for class : ClusterCovarianceMatrixTool
 *
 *  @date 02/11/2001 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

// ============================================================================
/** factory business
 */
// ============================================================================
static const ToolFactory<ClusterCovarianceMatrixTool>         s_Factory ;
const       IToolFactory&ClusterCovarianceMatrixToolFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param type tool type (useless)
 *  @param name tool name
 *  @param parent pointer to parent object (service, algorithm or tool)  
 */
// ============================================================================
ClusterCovarianceMatrixTool::ClusterCovarianceMatrixTool
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : CaloTool( type , name , parent )
  , m_a       ( 0.09 )
  , m_gainErr ( 0    )
  , m_noiseIn ( 0    ) 
  , m_noiseCo ( 0    )
{
  declareProperty( "Resolution"       , m_a       );
  declareProperty( "GainError"        , m_gainErr );
  declareProperty( "NoiseIncoherent"  , m_noiseIn );
  declareProperty( "NoiseCoherent"    , m_noiseCo );
};

// ============================================================================
/** destructor, virtual and protected 
 */
// ============================================================================
ClusterCovarianceMatrixTool::~ClusterCovarianceMatrixTool()
{
  /// delete estimator 
  if( 0 != m_estimator ) { delete m_estimator ; m_estimator = 0 ; }
  /// remove detector 
  setDet( (const DeCalorimeter*) 0 );
  ///
};

// ============================================================================
/** standard initialization method 
 *  @return status code 
 */
// ============================================================================
StatusCode ClusterCovarianceMatrixTool::initialize ()
{
  /// initialize the base class
  StatusCode sc = CaloTool::initialize ();
  /// 
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!") ; }
  ///
  if( 0 != m_estimator ) { delete m_estimator ; m_estimator = 0 ; }
  ///
  if( 0 == det() ) 
    {
      IDataProviderSvc* detsvc = 0 ;
      StatusCode sc = 
        svcLoc()->service( "DetectorDataSvc" , detsvc );
      if( sc.isFailure() ) 
        { return Error("Could not locate 'DetectorDataSvc'",sc); }
      if( 0 == detsvc    ) 
        { return Error("IDataProviderSvc* points to NULL!",sc); }    
      detsvc->addRef();
      /// 
      SmartDataPtr<DeCalorimeter> calo( detsvc , detName() );
      if( !calo )
        { return Error("Could not locate detector='"+detName()+"'"); }
      /// set detector
      setDet( calo );
      ///  release the service 
      detsvc->release();
      if( 0 == det() ) 
        { return Error("DeCalorimeter* points to NULL!");  }
    }
  ///
  m_estimator = 
    new CovarianceEstimator( det()     ,  ///< detector 
                             m_a       ,  ///< resolution 
                             m_gainErr ,  ///< gain error 
                             m_noiseIn ,  ///< incoherent noise
                             m_noiseCo ); ///< coherent noise
  ///
  return StatusCode::SUCCESS ;
};

// ============================================================================
/** standard finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode ClusterCovarianceMatrixTool::finalize   ()
{  
  /// delete estimator 
  if( 0 != m_estimator ) { delete m_estimator ; m_estimator = 0 ; }
  /// remove detector 
  setDet( (const DeCalorimeter*) 0 );
  /// finalize the  the base class
  return CaloTool::initialize ();
};

// ============================================================================
/** The main processing method (functor interface) 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode 
ClusterCovarianceMatrixTool::operator() ( CaloCluster* cluster ) const 
{
  return process( cluster );
};

// ============================================================================
/** The main processing method with hypothesis 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @param hypo    processing hypothesis 
 *  @return status code 
 */  
// ============================================================================
StatusCode 
ClusterCovarianceMatrixTool::process 
( CaloCluster* cluster                          , 
  const CaloHypotheses::Hypothesis& /* hypo */  ) const 
{
  return process( cluster );
};

// ============================================================================
/** The main processing method 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode 
ClusterCovarianceMatrixTool::process ( CaloCluster* cluster ) const 
{
  if( 0 == cluster     ) { return Error( "CaloCluster*   points to NULL!") ; }
  ///
  if( 0 == m_estimator ) { return Error( "Estimator is not created!"     ) ; }
  if( 0 == det ()      ) { return Error( "DeCalorimeter* points to NULL!") ; }
  ///
  /// apply the estimator 
  return (*m_estimator)( cluster );
};



// ============================================================================
// The End 
// ============================================================================


