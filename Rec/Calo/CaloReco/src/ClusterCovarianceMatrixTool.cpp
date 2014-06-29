// ============================================================================
// Include files
#include "GaudiKernel/ToolFactory.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloCluster.h"
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
  , m_useDB(true){
  // interface!
  declareInterface<ICaloClusterTool> (this);
  // try to get properties from the parent 
  DoubleProperty resolution      ( "Resolution"      , m_a       ) ;
  DoubleProperty gainError       ( "GainError"       , m_gainErr ) ;
  DoubleProperty noiseIncoherent ( "NoiseIncoherent" , m_noiseIn ) ;
  DoubleProperty noiseCoherent   ( "NoiseCoherent"   , m_noiseCo ) ;
  StringProperty detData         ( "Detector"        , m_detData ) ;

  // Inherit properties from parent algorithm
  if( 0 != parent ){
      const IProperty* prop = 
        dynamic_cast<const IProperty*> ( parent );
      if( 0 != prop ){
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
  declareProperty( "ConditionName"    , m_conditionName ="Conditions/Reco/Calo/Covariance");
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

  // get detector
  m_det = getDet<DeCalorimeter>( m_detData) ;


  std::string flag = " from options";  
  // get parameters from DB when available  
  if( m_useDB ){
    double a =0.;
    double gE=0.;
    double iN=0.;
    double cN=0.;
    bool ok = true;
    // first check if parameters exists in reconstruction conditions
    m_dbAccessor = tool<CaloCorrectionBase>("CaloCorrectionBase","DBAccessor",this);
    StatusCode scc=m_dbAccessor->setConditionParams(m_conditionName,true);// force access via DB - if not exist will return empty params
    if( scc.isFailure() )return Error("Cannot access DB",StatusCode::FAILURE);
    std::vector<double> params=m_dbAccessor->getParams(CaloCorrection::ClusterCovariance).second; 
    std::string source;
    if( params.size() == 4 ){
      source="using 'Reco' Condition";
      a  = params[0];
      gE = params[1];
      iN = params[2];
      cN = params[3];
    }else{
    // if not - use parameters as is 'gain' condition (i.e. detector intrinsic parameters as used for the digitization - may not be adapted)
      source="using 'Gain' Condition";
      a  = m_det->stochasticTerm();
      gE = m_det->gainError();
      iN = m_det->incoherentNoise();
      cN = m_det->coherentNoise();
    }

    if( a <= 0. || gE <= 0. )ok = false; // check stochastic term & gainError (noises are always defined in gain for digitization)
    if( ok ){
      flag = " from DB " + source;
      m_a       = a  ;  
      m_gainErr = gE ;
      m_noiseIn = (iN >= 0. ) ? iN : 0.;
      m_noiseCo = (cN >= 0. ) ? cN : 0.;
    }else warning()<<"No parameters for covariance in DB - use options values"<<endmsg;
  }
  // else use inherited property
  m_estimator.setDetector( m_det     ) ;
  m_estimator.setA       ( m_a       ) ;
  m_estimator.setGainS   ( m_gainErr ) ;
  m_estimator.setNoiseIn ( m_noiseIn ) ;
  m_estimator.setNoiseCo ( m_noiseCo ) ;
  
  info()      << " Has initialized with parameters: "              << endmsg 
              << " \t 'Detector'         = '" << m_detData << "'"  << endmsg 
              << " \t ==  Parameters for covariance estimation ==" << endmsg
              << " \t  "<< flag <<  endmsg
  //              << " \t   - Stochastic term    =  " << m_a         << endmsg 
  //            << " \t   - Gain error         =  " << m_gainErr   << endmsg 
  //            << " \t   - Incoherent noise   =  " << m_noiseIn   << endmsg 
  //            << " \t   - Coherent   noise   =  " << m_noiseCo   << endmsg 
              << " \t Estimator is          " << m_estimator << endmsg ;
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

