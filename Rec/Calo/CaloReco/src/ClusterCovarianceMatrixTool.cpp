// ============================================================================
// Include files
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
// CaloDet 
#include "CaloDet/DeCalorimeter.h"
//Event
#include "Event/CaloCluster.h"
// CaloUtil
#include "CaloUtils/CovarianceEstimator.h"
// local
#include "ClusterCovarianceMatrixTool.h"

// ============================================================================
/** @file
 *
 *  Implementation file for class ClusterCovarianceMatrixTool
 *
 *  @date 02/11/2001 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

DECLARE_TOOL_FACTORY( ClusterCovarianceMatrixTool )

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
  : GaudiTool( type , name , parent )
  , m_estimator (      ) 
  , m_a         ( "", 0.09 )
  , m_gainErr   ( "", 0    )
  , m_noiseIn   ( "", 0    ) 
  , m_noiseCo   ( "", 0    )
  , m_detData   ( DeCalorimeterLocation::Ecal )
  , m_det(0)
  , m_useDB(true)
{
  // interface!
  declareInterface<ICaloClusterTool> (this);
  // try to get properties from the parent 
  DoubleProperty resolution      ( "Resolution"      , m_a       ) ;
  DoubleProperty gainError       ( "GainError"       , m_gainErr ) ;
  DoubleProperty noiseIncoherent ( "NoiseIncoherent" , m_noiseIn ) ;
  DoubleProperty noiseCoherent   ( "NoiseCoherent"   , m_noiseCo ) ;
  StringProperty detData         ( "Detector"        , m_detData ) ;
  ///
  if( 0 != parent )
    {
      const IProperty* prop = 
        dynamic_cast<const IProperty*> ( parent );
      if( 0 != prop )
        {
          StatusCode sc;
          sc=prop->getProperty( &resolution      );
          sc=prop->getProperty( &gainError       );
          sc=prop->getProperty( &noiseIncoherent );
          sc=prop->getProperty( &noiseCoherent   );
          sc=prop->getProperty( &detData         );
          if(sc.isFailure())warning()<<"Unable to get properties"<<endmsg;
        }
    }
  ///
  m_a       = resolution      .value() ;
  m_gainErr = gainError       .value() ;
  m_noiseIn = noiseIncoherent .value() ;
  m_noiseCo = noiseCoherent   .value() ;
  m_detData = detData         .value() ;
  ///
  declareProperty( "Resolution"       , m_a       );
  declareProperty( "GainError"        , m_gainErr );
  declareProperty( "NoiseIncoherent"  , m_noiseIn );
  declareProperty( "NoiseCoherent"    , m_noiseCo );
  declareProperty( "Detector"         , m_detData  );
  declareProperty( "UseDBParameters"  , m_useDB=true);
}
// ============================================================================
/// destructor, virtual and protected 
// ============================================================================
ClusterCovarianceMatrixTool::~ClusterCovarianceMatrixTool() {}

// ============================================================================
/** standard initialization method 
 *  @return status code 
 */
// ============================================================================


StatusCode ClusterCovarianceMatrixTool::initialize (){
  StatusCode sc = GaudiTool::initialize ();
  /// 
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!") ; }
  ///
  m_det = getDet<DeCalorimeter>( m_detData) ;

  std::string fa=std::string(" from options");
  std::string fg=std::string(" from options");
  std::string fc=std::string(" from options");
  std::string fi=std::string(" from options");  
  // get parameters from DB if available
  if( m_useDB ){
    double a = m_det->stochasticTerm();
    double gE = m_det->gainError();
    double In = m_det->incoherentNoise();
    double cN = m_det->coherentNoise();
    
    if( a  > 0){m_a = a        ;fa=std::string(" from DB");}
    if( gE > 0){m_gainErr = gE ;fg=std::string(" from DB");}   
    if( In > 0){m_noiseIn  = In;fi=std::string(" from DB");}   
    if( cN > 0){m_noiseCo  = cN;fc=std::string(" from DB");}
  }
  

  m_estimator.setDetector( m_det     ) ;
  m_estimator.setA       ( m_a       ) ;
  m_estimator.setGainS   ( m_gainErr ) ;
  m_estimator.setNoiseIn ( m_noiseIn ) ;
  m_estimator.setNoiseCo ( m_noiseCo ) ;
  
  info()      << " Has initialized with parameters: "              << endmsg 
              << " \t 'Detector'         = '" << m_detData << "'"  << endmsg 
              << " \t 'Resolution'       = '" << m_a       << "'"  << fa << endmsg 
              << " \t 'GainError'        = '" << m_gainErr << "'"  << fg << endmsg 
              << " \t 'NoiseIncoherent'  = '" << m_noiseIn << "'"  << fi << endmsg 
              << " \t 'NoiseCoherent'    = '" << m_noiseCo << "'"  << fc << endmsg 
              << " \t Estimator is          " << m_estimator       << endmsg ;
  return StatusCode::SUCCESS ;
}
// ============================================================================

// ============================================================================
/** standard finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode ClusterCovarianceMatrixTool::finalize   ()
{  
  return GaudiTool::finalize ();
}
// ============================================================================

// ============================================================================
/** The main processing method (functor interface) 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode 
ClusterCovarianceMatrixTool::operator() ( LHCb::CaloCluster* cluster ) const 
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

// ============================================================================
/** The main processing method 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode 
ClusterCovarianceMatrixTool::process ( LHCb::CaloCluster* cluster ) const 
{ return (*this)( cluster ); }
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
