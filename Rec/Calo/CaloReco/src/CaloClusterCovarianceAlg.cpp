// $Id: CaloClusterCovarianceAlg.cpp,v 1.4 2005-11-07 12:12:41 odescham Exp $ 
//  ===========================================================================
// CVS tag $Name: not supported by cvs2svn $
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/02/17 12:08:06  ibelyaev
//  update for new CaloKernel and CaloInterfaces
//
// Revision 1.2  2002/12/01 14:22:57  ibelyaev
//  Hcal stuff and updated S-coprrections
//
// Revision 1.1.1.1  2002/11/13 20:46:39  ibelyaev
// new package 
//
// ===========================================================================
#define CALOALGS_CALOCLUSTERCOVARIANCEALG_CPP 1 
/// ===========================================================================
// Include files
// from Gaudi
#include  "GaudiKernel/AlgFactory.h"
#include  "GaudiKernel/MsgStream.h" 
#include  "GaudiKernel/SmartDataPtr.h" 
#include  "GaudiKernel/SmartRef.h" 
#include  "GaudiKernel/IToolSvc.h" 
// CaloDet 
#include  "CaloDet/DeCalorimeter.h" 
// Event
#include  "Event/CaloCluster.h"
// CaloInterfaces
#include  "CaloInterfaces/ICaloClusterTool.h"
#include  "CaloInterfaces/ICaloSubClusterTag.h"
// CaloUtils 
#include  "CaloUtils/CovarianceEstimator.h"
// local
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
 */
// ===========================================================================

// ===========================================================================
/** @var CaloClusterCovarianceAlgFactory
 *  Declaration of the Algorithm Factory
 */
// ===========================================================================
static const  AlgFactory<CaloClusterCovarianceAlg>         s_Factory ;
const        IAlgFactory&CaloClusterCovarianceAlgFactory = s_Factory ; 
// ===========================================================================

// ============================================================================
/** Standard constructor
 *  @param   name  algorith name
 *  @param   svc   pointer to Service Locator
*/
// ============================================================================
CaloClusterCovarianceAlg::CaloClusterCovarianceAlg
( const std::string& name,
  ISvcLocator* pSvcLocator)
  : CaloAlgorithm        ( name , pSvcLocator            ) 
  , m_copy               ( false                         )
  , m_a                  ( 0.10                          )
  , m_gainErr            ( 0.01                          )
  , m_noiseIn            ( 1.30                          ) 
  , m_noiseCo            ( 0.30                          )
  , m_taggerType         ( "SubClusterSelector3x3"       )
  , m_taggerName         ( ""                            )
  , m_tagger             ( 0                             )
  , m_covType            ( "ClusterCovarianceMatrixTool" )
  , m_covName            ( "EcalCovarTool"               )
  , m_cov                ( 0                             ) 
  , m_spreadType         ( "ClusterSpreadTool"           )
  , m_spreadName         ( "EcalSpreadTool"              )
  , m_spread             ( 0                             )
{
  declareProperty( "Resolution"       , m_a            ) ;
  declareProperty( "GainError"        , m_gainErr      ) ;
  declareProperty( "NoiseIncoherent"  , m_noiseIn      ) ;
  declareProperty( "NoiseCoherent"    , m_noiseCo      ) ;
  //
  declareProperty( "CovarianceType"   , m_covType      ) ;
  declareProperty( "CovarianceName"   , m_covName      ) ;
  ///
  declareProperty( "SubClusterType"   , m_taggerType   ) ;
  declareProperty( "SubClusterName"   , m_taggerName   ) ;
  ///
  declareProperty( "SpreadType"       , m_spreadType   ) ;
  declareProperty( "SpreadName"       , m_spreadName   ) ;

  // set the appropriate default for input     data 
  setInputData   ( CaloClusterLocation::   Ecal        ) ;
  // set the appropriate default for detector  data 
  setDetData     ( DeCalorimeterLocation:: Ecal        ) ;
};
// ===========================================================================

// ============================================================================
/// Destructor
// ============================================================================
CaloClusterCovarianceAlg::~CaloClusterCovarianceAlg() {}; 
// ===========================================================================

// ===========================================================================
/** standard initialization method 
 *  @see CaloAlgorithm
 *  @see     Algorithm 
 *  @see    IAlgorithm
 *  @return status code 
 */
// ===========================================================================
StatusCode CaloClusterCovarianceAlg::initialize() 
{  
  MsgStream log(msgSvc(), name());
  
  // try to initialize base class   
  StatusCode sc = CaloAlgorithm::initialize();
  if( sc.isFailure() )
    { return Error("Could not initialize the base class CaloAlgorithm"); }
  
  // locate the tool for covariance matrix calculations 
  m_cov    = m_covName.empty() ?
    tool<ICaloClusterTool>(    m_covType                   , this ) :
    tool<ICaloClusterTool>(    m_covType    , m_covName    , this ) ;
  
  // locate the tool for subcluster selection/tagging 
  m_tagger = m_taggerName.empty() ?
    tool<ICaloSubClusterTag>(    m_taggerType                , this ) :
    tool<ICaloSubClusterTag>(    m_taggerType , m_taggerName , this ) ;
  
  // locate the tool for cluster spread(2nd moments) estimation 
  m_spread = m_spreadName.empty() ?
    tool<ICaloClusterTool>(    m_spreadType                , this ) :
    tool<ICaloClusterTool>(    m_spreadType , m_spreadName , this ) ;
  
  // copy flag
  m_copy = 
    outputData().empty()        ?  false :
    outputData() == inputData() ?  false : true ;

  return StatusCode::SUCCESS;
};
// ===========================================================================

// ===========================================================================
/** standard finalization method 
 *  @see CaloAlgorithm
 *  @see     Algorithm 
 *  @see    IAlgorithm
 *  @return status code 
 */
// ===========================================================================
StatusCode CaloClusterCovarianceAlg::finalize() 
{
  // finalize the base class 
  return CaloAlgorithm::finalize();
};
// ===========================================================================

// ===========================================================================
/** standard execution method 
 *  @see CaloAlgorithm
 *  @see     Algorithm 
 *  @see    IAlgorithm
 *  @return status code 
 */
// ===========================================================================
StatusCode CaloClusterCovarianceAlg::execute() 
{

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  // useful typedefs
  typedef CaloClusters        Clusters ;
  typedef const DeCalorimeter Detector ;
  
  // locate input data
  Clusters* clusters =    get<Clusters> ( inputData() );
  if( 0 == clusters ) { return StatusCode::FAILURE ; }
  
  //
  Detector* detector = getDet<Detector> ( detData()   );
  if( 0 == detector ) { return StatusCode::FAILURE ; }
  
  // define the output data 
  Clusters* output = 0;
  if( m_copy )     // make a copy of container 
  {
    output = new Clusters();
    StatusCode sc = put( output , outputData() );
    if( sc.isFailure() ) 
    { return Error("Could not register '"+outputData()+"'");}
    // make a copy 
    for( Clusters::const_iterator i = clusters->begin() ;
         clusters->end() != i ; ++i )
    { if( 0 != *i ) { output->insert( (*i)->clone() ) ; } }
  }
  else { output = clusters; } // update existing sequence
  // 
  for( Clusters::iterator cluster =  output->begin() ;
       output->end() != cluster ; ++cluster )
  {
    // skip nulls 
    if( 0 == *cluster  ) { continue ; }                // CONTINUE !
    StatusCode sc =   tagger () -> tag    ( *cluster ) ; 
    if( sc.isFailure() ) 
    {    
      Error("Error from tagger, skip cluster ", sc ) ; 
      log << MSG::DEBUG << *cluster << endreq ;
      continue ; 
    }
    if( sc.isSuccess() )     { sc = cov    () -> process( *cluster ) ; }
    else 
    { 
      Error("Error from cov,    skip cluster ", sc ) ; 
      log << MSG::DEBUG << *cluster << endreq ;
      continue ; 
    }
    if( sc.isSuccess() )     { sc = spread () -> process( *cluster ) ; }
    else 
    { 
      Error("Error from spread, skip cluster ", sc ) ; 
      log << MSG::DEBUG << *cluster << endreq ;
      continue ; 
    }
  }
  
  return StatusCode::SUCCESS ;
};
// ===========================================================================

// The End 
// ===========================================================================

