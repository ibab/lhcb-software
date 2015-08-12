// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
#include <functional>
#include <memory>
// ============================================================================
#include "GaudiKernel/SystemOfUnits.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "CaloUtils/CaloDataFunctor.h"
#include "Event/CellID.h"
#include "CaloInterfaces/ICaloClusterSelector.h"
#include "CaloInterfaces/ICaloHypoTool.h"
#include  "CaloUtils/CaloAlgUtils.h"
#include "CaloSinglePhotonAlg.h"
#include "CaloUtils/CaloMomentum.h"
#include "boost/lexical_cast.hpp"
// ============================================================================
/** @file
 * 
 *  Implementation file for class : CaloSinglePhotonAlg
 *  The implementation is based on F.Machefert's codes.
 *  @see CaloSinglePhotonAlg
 *
 *  @author Frederic Machefert machefer@in2p3.fr
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 31/03/2002 
 */
// ============================================================================
DECLARE_ALGORITHM_FACTORY( CaloSinglePhotonAlg )
// ============================================================================
/*  Standard constructor
 *  @param name algorithm name 
 *  @param pSvc service locator 
 */
// ============================================================================
CaloSinglePhotonAlg::CaloSinglePhotonAlg
( const std::string& name ,
  ISvcLocator*       pSvc )
  : GaudiAlgorithm ( name , pSvc )  
  //
  , m_eTcut ( 0. )
  //
  , m_selectorsTypeNames    () 
  , m_selectors             ()
  , m_correctionsTypeNames  () 
  , m_corrections           () 
  , m_hypotoolsTypeNames    ()
  , m_hypotools             () 
  , m_correctionsTypeNames2 () 
  , m_corrections2          () 
  , m_hypotoolsTypeNames2   ()
  , m_hypotools2            () 
  , m_inputData    ( LHCb::CaloClusterLocation::Ecal   ) 
  , m_outputData   ( LHCb::CaloHypoLocation::Photons   ) 
  , m_detData      ( DeCalorimeterLocation:: Ecal      )
  , m_det(0)
{
  m_selectorsTypeNames.push_back( "CaloSelectCluster/PhotonCluster"                   ) ;
  m_selectorsTypeNames.push_back( "CaloSelectNeutralClusterWithTracks/NeutralCluster" ) ;
  declareProperty( "SelectionTools"  , m_selectorsTypeNames    ,"List of tools for selection of clusters") ;
  declareProperty( "CorrectionTools" , m_correctionsTypeNames  ,"List of tools for primary corrections") ;  
  declareProperty ( "HypoTools" , m_hypotoolsTypeNames  , "List of generic Hypo-tools to apply to newly created hypos") ;

  m_hypotoolsTypeNames .push_back ( "CaloExtraDigits/SpdPrsExtraG" ) ;  
  m_correctionsTypeNames2.push_back ( "CaloECorrection/ECorrection" ) ;
  m_correctionsTypeNames2.push_back ( "CaloSCorrection/SCorrection" ) ;
  m_correctionsTypeNames2.push_back ( "CaloLCorrection/LCorrection" ) ;
  
  declareProperty ( "CorrectionTools2" , m_correctionsTypeNames2 , "List of tools for 'fine-corrections' " ) ;  
  declareProperty ( "HypoTools2" , m_hypotoolsTypeNames2 , "List of generic Hypo-tools to apply to corrected hypos") ;
  declareProperty ( "EtCut"   , m_eTcut , "Threshold on cluster & hypo ET" ) ;
  //
  declareProperty ( "InputData"         , m_inputData     ) ;  
  declareProperty ( "OutputData"        , m_outputData     ) ;  
  declareProperty ( "Detector"          , m_detData      ) ;  
  

 // Default context-dependent locations
  m_inputData = LHCb::CaloAlgUtils::CaloClusterLocation( "Ecal", context() );
  m_outputData= LHCb::CaloAlgUtils::CaloHypoLocation(  "Photons", context() );
  m_detData   = LHCb::CaloAlgUtils::DeCaloLocation("Ecal");

  setProperty ( "PropertiesPrint" , true ) ;
  
}
// ============================================================================
// destructor (protected and virtual)
// ============================================================================
CaloSinglePhotonAlg::~CaloSinglePhotonAlg() {}
// ============================================================================
/*   standard Algorithm initialization
 *   @return status code 
 */
// ============================================================================
StatusCode CaloSinglePhotonAlg::initialize() 
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() ) 
  { return Error("Could not initialize the base class GaudiAlgorithm!",sc);}

  // check the geometry information 
  m_det = getDet<DeCalorimeter>( m_detData ) ; 
  if( 0 == m_det )return Error("Detector information is not available!");
  
  
  // locate selector tools
  for( Names::const_iterator item = m_selectorsTypeNames.begin() ; m_selectorsTypeNames.end() != item ; ++item ){
    ICaloClusterSelector* selector   = tool<ICaloClusterSelector>( *item , this );
      m_selectors.push_back( selector );
  }
  if ( m_selectors.empty() )info() << "No Cluster Selection tools are specified!" << endmsg ; 
  
  // locate correction tools
  for( Names::const_iterator item = m_correctionsTypeNames.begin() ; m_correctionsTypeNames.end() != item ; ++item ) {
      ICaloHypoTool*  correction = tool<ICaloHypoTool>( *item , this );
      m_corrections.push_back( correction );
  }
  if ( m_corrections.empty() )info() << "No Hypo    Correction(1) tools are specified!" << endmsg ; 
  
  // locate other hypo  tools
  for( Names::const_iterator item = m_hypotoolsTypeNames.begin() ; m_hypotoolsTypeNames.end() != item ; ++item ){
      ICaloHypoTool*  hypotool = tool<ICaloHypoTool>( *item , this );
      m_hypotools.push_back(  hypotool  );
  }
  if ( m_hypotools.empty() )info() << "No Hypo    Processing(1) tools are specified!" << endmsg ; 
  
  // locate correction tools
  for( Names::const_iterator item = m_correctionsTypeNames2.begin() ; m_correctionsTypeNames2.end() != item ; ++item ){
      ICaloHypoTool*  correction = tool<ICaloHypoTool>( *item , this );
      m_corrections2.push_back( correction );
  }
  if ( m_corrections2.empty() )info() << "No Hypo    Correction(2) tools are specified!" << endmsg ; 
  
  // locate other hypo  tools
  for( Names::const_iterator item = m_hypotoolsTypeNames2.begin() ;m_hypotoolsTypeNames2.end() != item ; ++item ){
    ICaloHypoTool*  hypotool = tool<ICaloHypoTool>( *item , this );
    m_hypotools2.push_back(  hypotool  );
  }
  if ( m_hypotools2.empty() )info () << "No Hypo    Processing(2) tools are specified!" << endmsg ;

  ///
  return StatusCode::SUCCESS;
}
// ============================================================================
/*   standard Algorithm finalization
 *   @return status code 
 */
// ============================================================================
StatusCode 
CaloSinglePhotonAlg::finalize() 
{
  // clear containers
  m_selectors             .clear () ;
  m_corrections           .clear () ;
  m_hypotools             .clear () ;
  m_corrections2          .clear () ;
  m_hypotools2            .clear () ;
  m_selectorsTypeNames    .clear () ;
  m_correctionsTypeNames  .clear () ;
  m_hypotoolsTypeNames    .clear () ;
  m_correctionsTypeNames2 .clear () ;
  m_hypotoolsTypeNames2   .clear () ;
  // finalize the base class 
  return GaudiAlgorithm::finalize() ; 
}
// ============================================================================
/*   standard Algorithm execution
 *   @return status code 
 */
// ============================================================================
StatusCode 
CaloSinglePhotonAlg::execute(){
  using namespace  LHCb::CaloDataFunctor;
  // avoid long names 
  typedef LHCb::CaloClusters             Clusters ;
  typedef LHCb::CaloHypos                Hypos    ;
  typedef Clusters::iterator       iterator ;
  
  // get input clusters
  Clusters* clusters = get<Clusters>( m_inputData );
  
  // create and the output container of hypotheses and put in to ETS  
  Hypos*    hypos = new Hypos() ;
  put( hypos , m_outputData );
  
  LHCb::CaloDataFunctor::EnergyTransverse<const LHCb::CaloCluster*,const DeCalorimeter*> eT ( m_det ) ;
  
  // loop over clusters 
  for( iterator cluster = clusters->begin() ;clusters->end() != cluster ; ++cluster ){
    if ( eT( *cluster ) < m_eTcut )  continue ; 
    
    bool select = true ;
    // loop over all selectors 
    for( Selectors::const_iterator selector = m_selectors.begin() ; select && m_selectors.end() != selector ; ++selector ){ 
      select = (**selector)( *cluster ); 
    }
    // cluster to be selected? 
    if ( !select ) { continue ; }
    
    // create "Hypo"/"Photon" object
    std::auto_ptr<LHCb::CaloHypo> hypo ( new LHCb::CaloHypo() ) ;
    
    // set parameters of newly created hypo 
    hypo -> setHypothesis ( LHCb::CaloHypo::Photon );      
    hypo -> addToClusters ( *cluster );
    hypo -> setPosition   ( new LHCb::CaloPosition((*cluster)->position()) );    
    StatusCode sc( StatusCode::SUCCESS );
    
    // loop over all corrections and apply corrections  
    for ( Corrections::const_iterator correction = m_corrections.begin() ;
          sc.isSuccess() && m_corrections.end() != correction ; ++correction ){ 
      sc = (**correction) ( hypo.get() ); 
    }
    if( sc.isFailure() ){
      Error("Error from Correction Tool, skip the cluster  " , sc ).ignore(); 
      continue ;                                      // CONTINUE  !  
    }
    
    // loop over other hypo tools (e.g. add extra digits)
    for( HypoTools::const_iterator hypotool = m_hypotools.begin() ; sc.isSuccess() && m_hypotools.end() != hypotool ;++hypotool ){ 
      sc = (**hypotool) ( hypo.get() ); 
    }
    
    if ( sc.isFailure() ){
      Error("Error from Other Hypo Tool, skip the cluster  " , sc ).ignore();
      continue  ;                                    // ATTENTION ! 
    }
    
    // loop over all corrections and apply corrections  
    for( Corrections::const_iterator cor2 = m_corrections2.begin() ;sc.isSuccess() && m_corrections2.end() != cor2 ; ++cor2 ){
      sc = (**cor2) ( hypo.get() ); 
    }
    
    if ( sc.isFailure() ){
      Error ( "Error from Correction Tool 2 skip the cluster" , sc ).ignore();  
      continue ;                                    // CONTINUE  ! 
    }
    
    // loop over other hypo tools (e.g. add extra digits)
    for( HypoTools::const_iterator hypotool2 = m_hypotools2.begin() ;
         sc.isSuccess() && m_hypotools2.end() != hypotool2 ; ++hypotool2 ){ 
      sc = (**hypotool2) ( hypo.get() ); 
    }
    
    if ( sc.isFailure() ){
      Error ( "Error from Other Hypo Tool 2, skip the cluster" , sc ).ignore(); 
      continue ;                                    // CONTINUE !
    }

    // check momentum after all corrections :
    if( LHCb::CaloMomentum( hypo.get() ).pt() < m_eTcut )
      hypo.release();
    else
      hypos->insert ( hypo.release() ) ;  /// add the hypo into container of hypos 
    
  } // end of the loop over all clusters
  
  if(UNLIKELY(msgLevel(MSG::DEBUG)))debug() << " # of created Photon  Hypos is  " << hypos->size()  << "/" << clusters->size()<< endmsg ;
  counter ( m_inputData + "=>" + m_outputData  ) += hypos->size() ;  
  return StatusCode::SUCCESS;
}




