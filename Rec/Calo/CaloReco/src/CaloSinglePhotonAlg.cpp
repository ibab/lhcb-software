// $Id: CaloSinglePhotonAlg.cpp,v 1.12 2008-09-22 01:41:23 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
#include <functional>
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
// ============================================================================
// CaloDet 
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
// ============================================================================
// Event/CaloEvent 
// ============================================================================
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "CaloUtils/CaloDataFunctor.h"
#include "Event/CellID.h"
// ============================================================================
// CaloInterfaces
// ============================================================================
#include "CaloInterfaces/ICaloClusterSelector.h"
#include "CaloInterfaces/ICaloHypoTool.h"
// ============================================================================
// local
// ============================================================================
#include "CaloSinglePhotonAlg.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/lexical_cast.hpp"
// ============================================================================

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

DECLARE_ALGORITHM_FACTORY( CaloSinglePhotonAlg );

// ============================================================================
/** Standard constructor
 *  @param name algorithm name 
 *  @param pSvc service locator 
 */
// ============================================================================
CaloSinglePhotonAlg::CaloSinglePhotonAlg
( const std::string& name ,
  ISvcLocator*       pSvc )
  : GaudiAlgorithm ( name , pSvc )  
  //
  , m_eTcut ( -10 * Gaudi::Units::GeV )
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
{    
  declareProperty ( "SelectionTools"   , m_selectorsTypeNames    ) ;
  declareProperty ( "CorrectionTools"  , m_correctionsTypeNames  ) ;
  declareProperty ( "HypoTools"        , m_hypotoolsTypeNames    ) ;
  declareProperty ( "CorrectionTools2" , m_correctionsTypeNames2 ) ;
  declareProperty ( "HypoTools2"       , m_hypotoolsTypeNames2   ) ;
  //
  declareProperty ( "EtCut"            , m_eTcut                 ) ;
  //
  declareProperty ( "InputData"         , m_inputData     ) ;  
  declareProperty ( "OutputData"        , m_outputData     ) ;  
  declareProperty ( "Detector"          , m_detData      ) ;  

  if("HLT"==context()){
    m_inputData = LHCb::CaloClusterLocation::EcalHlt;
    m_outputData= LHCb::CaloHypoLocation::PhotonsHlt;
}
  
};
// ============================================================================

// ============================================================================
/// destructor (protected and virtual)
// ============================================================================
CaloSinglePhotonAlg::~CaloSinglePhotonAlg() {}; 
// ============================================================================


// ============================================================================
/**  standard Algorithm initialization
 *   @return status code 
 */
// ============================================================================
StatusCode
CaloSinglePhotonAlg::initialize() 
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() ) 
  { return Error("Could not initialize the base class GaudiAlgorithm!",sc);}
  // check the geometry information 
  const DeCalorimeter* det = getDet<DeCalorimeter>( m_detData) ;
  if( 0 == det ) { return Error("Detector information is not available!");}
  
  if ( 0 < m_eTcut ) 
  { Warning ( " Et Cut of " + 
              boost::lexical_cast<std::string>( m_eTcut / Gaudi::Units::GeV )  +
              " GeV is applied " ) ; }
  
  { // locate selector tools
    for( Names::const_iterator item = m_selectorsTypeNames.begin() ;
         m_selectorsTypeNames.end() != item ; ++item )
    {
      ICaloClusterSelector* selector   = tool<ICaloClusterSelector>( *item );
      m_selectors.push_back( selector );
    }
  }
  { // locate correction tools
    for( Names::const_iterator item = m_correctionsTypeNames.begin() ;
         m_correctionsTypeNames.end() != item ; ++item )
    {
      ICaloHypoTool*  correction       = tool<ICaloHypoTool>( *item );
      m_corrections.push_back( correction );
    }
  }
  { // locate other hypo  tools
    for( Names::const_iterator item = m_hypotoolsTypeNames.begin() ;
         m_hypotoolsTypeNames.end() != item ; ++item )
    {
      ICaloHypoTool*  hypotool         = tool<ICaloHypoTool>( *item );
      m_hypotools.push_back(  hypotool  );
    }
  }
  { // locate correction tools
    for( Names::const_iterator item = m_correctionsTypeNames2.begin() ;
         m_correctionsTypeNames2.end() != item ; ++item )
    {
      ICaloHypoTool*  correction       = tool<ICaloHypoTool>( *item );
      m_corrections2.push_back( correction );
    }
  }
  { // locate other hypo  tools
    for( Names::const_iterator item = m_hypotoolsTypeNames2.begin() ;
         m_hypotoolsTypeNames2.end() != item ; ++item )
    {
      ICaloHypoTool*  hypotool         = tool<ICaloHypoTool>( *item );
      m_hypotools2.push_back(  hypotool  );
    }
  }
  ///
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/**  standard Algorithm finalization
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
};
// ============================================================================

// ============================================================================
/**  standard Algorithm execution
 *   @return status code 
 */
// ============================================================================
StatusCode 
CaloSinglePhotonAlg::execute() 
{
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

  const DeCalorimeter* det = getDet<DeCalorimeter>( m_detData ) ; 
  LHCb::CaloDataFunctor::EnergyTransverse<const LHCb::CaloCluster*,const DeCalorimeter*> eT ( det ) ;

  // loop over clusters 
  for( iterator cluster = clusters->begin() ; 
       clusters->end() != cluster ; ++cluster )
  {
    if ( m_eTcut > 0 && eT( *cluster ) < m_eTcut ) { continue ; }

    bool select = true ;
    // loop over all selectors 
    for( Selectors::const_iterator selector = m_selectors.begin() ;
         select && m_selectors.end() != selector ; ++selector ){ 
      select = (**selector)( *cluster ); 
    }
    // cluster to be selected? 
    if( !select ) { continue ; }
    
    // create "Hypo"/"Photon" object
    LHCb::CaloHypo* hypo = new LHCb::CaloHypo() ;
    // set parameters of newly created hypo 
    hypo->setHypothesis( LHCb::CaloHypo::Photon );      
    hypo->addToClusters( *cluster );
    hypo->setPosition( new LHCb::CaloPosition((*cluster)->position()) ); // NEW OD 13/06/06
    

    StatusCode sc( StatusCode::SUCCESS );
    
    // loop over all corrections and apply corrections  
    for( Corrections::const_iterator correction = m_corrections.begin() ;
         sc.isSuccess() && m_corrections.end() != correction ; ++correction )
    { sc = (**correction) ( hypo ); }
    
    if( sc.isFailure() ) 
    {
      delete hypo ; hypo = 0 ;                        // ATTENTION !
      Error("Error from Correction Tool, skip the cluster  " , sc ); 
      continue ;                                      // CONTINUE  !  
    }
    
    // loop over other hypo tools (e.g. add extra digits)
    for( HypoTools::const_iterator hypotool = m_hypotools.begin() ;
         sc.isSuccess() && m_hypotools.end() != hypotool ; ++hypotool )
    { sc = (**hypotool) ( hypo ); }
    
    if( sc.isFailure() ) 
    {
      delete hypo ; hypo = 0 ;                       // ATTENTION !
      Error("Error from Other Hypo Tool, skip the cluster  " , sc );
      continue  ;                                    // ATTENTION ! 
    }
    


    // loop over all corrections and apply corrections  
    for( Corrections::const_iterator cor2 = m_corrections2.begin() ;
         sc.isSuccess() && m_corrections2.end() != cor2 ; ++cor2 )
    { sc = (**cor2) ( hypo ); }
    
    if( sc.isFailure() ) 
    {
      delete hypo ; hypo = 0 ;                      // ATTENTION !
      Error("Error from Correction Tool 2 skip the cluster" , sc );  
      continue ;                                    // CONTINUE  ! 
    }

    // loop over other hypo tools (e.g. add extra digits)
    for( HypoTools::const_iterator hypotool2 = m_hypotools2.begin() ;
         sc.isSuccess() && m_hypotools2.end() != hypotool2 ; ++hypotool2 )
    { sc = (**hypotool2) ( hypo ); }
    
    if( sc.isFailure() ) 
    {
      delete hypo ; hypo = 0 ;                      // ATTENTION !
      Error("Error from Other Hypo Tool 2, skip the cluster" , sc ); 
      continue ;                                    // CONTINUE !
    }
    
    /// add the hypo into container of hypos 
    if( 0 != hypo ) { hypos->insert( hypo ); }
    
    
  } // end of the loop over all clusters
  
  debug() << " # of created Photon  Hypos is  " 
          << hypos->size()  << "/" << clusters->size()
          << endreq ;

  return StatusCode::SUCCESS;
};
// ============================================================================




