// $Id: ClusterCovarianceMatrixTool.cpp,v 1.5 2002-04-07 18:15:01 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2001/12/09 14:33:09  ibelyaev
//  update for newer version of Gaudi
//
// Revision 1.3  2001/11/25 15:50:40  ibelyaev
//  update for newer CaloKernel package
//
// Revision 1.2  2001/11/08 10:58:34  ibelyaev
//  new tools are added for selection of subclusters within the cluster
//
// Revision 1.1  2001/11/04 15:30:40  ibelyaev
// add concrete tool
// 
// ============================================================================
// Include files
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/Property.h"
// CaloKernel
#include "CaloKernel/CaloPrint.h"     
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
  , m_estimator (      ) 
  , m_a         ( 0.09 )
  , m_gainErr   ( 0    )
  , m_noiseIn   ( 0    ) 
  , m_noiseCo   ( 0    )
{
  // inetraface!
  declareInterface<ICaloClusterTool> (this);
  /// try to get properties from the parent 
  DoubleProperty resolution      ( "Resolution"      , m_a       ) ;
  DoubleProperty gainError       ( "GainError"       , m_gainErr ) ;
  DoubleProperty noiseIncoherent ( "NoiseIncoherent" , m_noiseIn ) ;
  DoubleProperty noiseCoherent   ( "NoiseCoherent"   , m_noiseCo ) ;
  ///
  if( 0 != parent )
    {
      const IProperty* prop = 
        dynamic_cast<const IProperty*> (parent );
      if( 0 != prop )
        {
          prop->getProperty( &resolution      );
          prop->getProperty( &gainError       );
          prop->getProperty( &noiseIncoherent );
          prop->getProperty( &noiseCoherent   );
        }
    }
  ///
  m_a       = resolution      .value() ;
  m_gainErr = gainError       .value() ;
  m_noiseIn = noiseIncoherent .value() ;
  m_noiseCo = noiseCoherent   .value() ;
  ///
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
  ///
  if( 0 == det() ) 
    { return Error("DeCalorimeter* points to NULL!");  }
  ///
  {  /// configure the estimator 
    m_estimator.setDetector( det()     ) ;
    m_estimator.setA       ( m_a       ) ;
    m_estimator.setGainS   ( m_gainErr ) ;
    m_estimator.setNoiseIn ( m_noiseIn ) ;
    m_estimator.setNoiseCo ( m_noiseCo ) ;
  }
  ///
  CaloPrint Print;
  MsgStream log( msgSvc() , name() );
  log << MSG::DEBUG
      << " Has initialized with parameters: "                        << endreq 
      << " \t 'Detector'         = '" << detName()          << "'"  << endreq 
      << " \t 'Resolution'       = '" << Print( m_a       ) << "'"  << endreq 
      << " \t 'GainError'        = '" << Print( m_gainErr ) << "'"  << endreq 
      << " \t 'NoiseIncoherent'  = '" << Print( m_noiseIn ) << "'"  << endreq 
      << " \t 'NoiseCoherent'    = '" << Print( m_noiseCo ) << "'"  << endreq ;
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
  /// remove detector 
  setDet( (const DeCalorimeter*) 0 );
  /// finalize the  the base class
  return CaloTool::finalize ();
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
  /// check the argument 
  if( 0 == cluster                ) 
    { return Error( "CaloCluster*   points to NULL!") ; }
  if( 0 == m_estimator.detector() ) 
    { return Error( "DeCalorimeter* points to NULL!") ; }
  /// apply the estimator 
  return m_estimator( cluster );
};

// ============================================================================
/** The main processing method 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode 
ClusterCovarianceMatrixTool::process ( CaloCluster* cluster ) const 
{ return (*this)( cluster ); };

// ============================================================================
// The End 
// ============================================================================


