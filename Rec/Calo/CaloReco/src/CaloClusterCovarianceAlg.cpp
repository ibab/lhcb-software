//  ===========================================================================
#define CALORECO_CALOCLUSTERCOVARIANCEALG_CPP 1 
/// ===========================================================================
// Include files
// from Gaudi
#include  "GaudiKernel/AlgFactory.h"
#include  "CaloDet/DeCalorimeter.h" 
#include  "Event/CaloCluster.h"
#include  "CaloInterfaces/ICaloClusterTool.h"
#include  "SubClusterSelectorTool.h"
#include  "CaloUtils/CovarianceEstimator.h"
#include  "CaloUtils/CaloAlgUtils.h"
#include  "CaloClusterCovarianceAlg.h"

// ===========================================================================
/** @file
 * 
 *  Implementation file for class CaloClusterCovarianceAlg
 * 
 *  @see CaloClusterCovarinanceAlg
 *  @see ICaloClusterTool
 *  @see ICaloSubClusterTag 
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 04/07/2001 
s */
// ===========================================================================

DECLARE_ALGORITHM_FACTORY( CaloClusterCovarianceAlg )

// ============================================================================
/** Standard constructor
 *  @param   name          algorithm name
 *  @param   pSVcLocator   pointer to Service Locator
*/
// ============================================================================
CaloClusterCovarianceAlg::CaloClusterCovarianceAlg
( const std::string& name,
  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm        ( name , pSvcLocator            ) 
  , m_copy               ( false                         )
  , m_a                  ( 0.10                          )
  , m_gainErr            ( 0.01                          )
  , m_noiseIn            ( 1.20                          ) 
  , m_noiseCo            ( 0.30                          )
  , m_covType            ( "ClusterCovarianceMatrixTool" )
  , m_covName            ( "EcalCovarTool"               )
  , m_cov                ( NULL                          ) 
  , m_spreadType         ( "ClusterSpreadTool"           )
  , m_spreadName         ( "EcalSpreadTool"              )
  , m_spread             ( NULL                          )
  , m_inputData ( )
  , m_condition ("")
  , m_taggerE            ( 1 , "useDB") 
  , m_taggerP            ( 1 , "useDB") 
  , m_detData            (  DeCalorimeterLocation::Ecal ){
  declareProperty( "Resolution"       , m_a            ) ;
  declareProperty( "GainError"        , m_gainErr      ) ;
  declareProperty( "NoiseIncoherent"  , m_noiseIn      ) ;
  declareProperty( "NoiseCoherent"    , m_noiseCo      ) ;
  declareProperty( "CovarianceType"   , m_covType      ) ;
  declareProperty( "CovarianceName"   , m_covName      ) ;
  declareProperty( "SpreadType"       , m_spreadType   ) ;
  declareProperty( "SpreadName"       , m_spreadName   ) ;
  declareProperty ( "InputData"       , m_inputData    = LHCb::CaloClusterLocation::Ecal) ;  
  declareProperty ( "OutputData"      , m_outputData   ) ;  

  // following properties are inherited by the selector tool :
  declareProperty( "TagCondition" , m_condition    ) ;
  declareProperty( "EnergyTags"   , m_taggerE      ) ;
  declareProperty( "PositionTags" , m_taggerP      ) ;
  declareProperty( "Detector"     , m_detData      ) ;

  // set default data as a function of detector
  m_detData= LHCb::CaloAlgUtils::DeCaloLocation( name ) ;
  m_inputData = LHCb::CaloAlgUtils::CaloClusterLocation( name , context() );
}
// ===========================================================================

// ============================================================================
/// Destructor
// ============================================================================
CaloClusterCovarianceAlg::~CaloClusterCovarianceAlg() {}
// ===========================================================================

// ===========================================================================
/** standard initialization method 
 *  @see GaudiAlgorithm
 *  @see     Algorithm 
 *  @see    IAlgorithm
 *  @return status code 
 */
// ===========================================================================
StatusCode CaloClusterCovarianceAlg::initialize(){
  // try to initialize base class   
  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() )
    { return Error("Could not initialize the base class GaudiAlgorithm"); }
  
  
  // locate the detector
  m_detector = getDet<DeCalorimeter> ( m_detData   );
  if( 0 == m_detector ) { return Error("DeCalorimeter error",StatusCode::FAILURE) ; }
  unsigned int narea = m_detector->numberOfAreas();

  // locate the tagger (inherit from relevant properties)
  m_tagger = tool<SubClusterSelectorTool>( "SubClusterSelectorTool" , "ClusterTag" , this );

  // locate the tool for covariance matrix calculations 
  m_cov    = m_covName.empty() ?
    tool<ICaloClusterTool>(    m_covType                   , this ) :
    tool<ICaloClusterTool>(    m_covType    , m_covName    , this ) ;
  
  // locate the tool for cluster spread(2nd moments) estimation 
  m_spread = m_spreadName.empty() ? 
    tool<ICaloClusterTool>(    m_spreadType                , this ) :
    tool<ICaloClusterTool>(    m_spreadType , m_spreadName , this ) ;
  
  // copy flag
  m_copy = 
    m_outputData.empty()        ?  false :
    m_outputData == m_inputData ?  false : true ;

  return StatusCode::SUCCESS;
}



// ===========================================================================
StatusCode CaloClusterCovarianceAlg::finalize(){
  // finalize the base class 
  return GaudiAlgorithm::finalize();
}

// ===========================================================================
StatusCode CaloClusterCovarianceAlg::execute(){
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;
  
  // useful typedefs
  typedef LHCb::CaloClusters        Clusters ;
  
  // locate input data
  Clusters* clusters =    get<Clusters> ( m_inputData );
  if( 0 == clusters ) { return StatusCode::FAILURE ; }
  //
  
  // define the output data 
  Clusters* output = 0;
  if( m_copy ){
    // make a copy of container 
    output = new Clusters();
    put( output , m_outputData );
    // make a copy 
    for( Clusters::const_iterator i = clusters->begin() ;
         clusters->end() != i ; ++i ){ 
      if( 0 != *i )output->insert( (*i)->clone() ) ; 
    }
  }
  else { output = clusters; } // update existing sequence
  
  // 
  for( Clusters::iterator cluster =  output->begin() ; output->end() != cluster ; ++cluster ){

    // skip nulls 
    if( 0 == *cluster  ) { continue ; }

    
    // == APPLY TAGGER
    StatusCode sc = m_tagger -> tag ( *cluster );
    if( sc.isFailure() ){
      Error("Error from tagger, skip cluster ", sc ).ignore() ; 
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << *cluster << endmsg ;
      continue ; 
    }

    // == APPLY COVARIANCE ESTIMATOR
    sc = cov() -> process( *cluster ) ;    
    if( sc.isFailure() ){ 
      Error("Error from cov,    skip cluster ", sc, 0 ).ignore() ; 
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << *cluster << endmsg ;
      continue ; 
    }

    // == APPLY SPREAD ESTIMATOR
    sc = spread() -> process( *cluster ) ;
    if( sc.isFailure() ){ 
      Error("Error from spread, skip cluster ", sc, 0 ).ignore() ; 
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << *cluster << endmsg ;
      continue ; 
    }
  }
  
  // == COUNTER
  counter ( "#Clusters from '" + m_inputData  + "'") += clusters->size() ;
  return StatusCode::SUCCESS ;
}
