// $Id: CaloSinglePhotonAlg.cpp,v 1.4 2004-02-17 12:08:10 ibelyaev Exp $
// ============================================================================
// CVS atg $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// STD & STL 
#include <algorithm>
#include <functional>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/Stat.h"
// CaloDet 
#include "CaloDet/DeCalorimeter.h"
// Event/CaloEvent 
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
// CaloInterfaces
#include "CaloInterfaces/ICaloClusterSelector.h"
#include "CaloInterfaces/ICaloHypoTool.h"
// local
#include "CaloSinglePhotonAlg.h"

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

// ============================================================================
/** @var CaloSinglePhotonAlgFactory
 *  mandatory instantiation of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloSinglePhotonAlg>         s_Factory ;
const        IAlgFactory&CaloSinglePhotonAlgFactory = s_Factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param name algorithm name 
 *  @param pSvc service locator 
 */
// ============================================================================
CaloSinglePhotonAlg::CaloSinglePhotonAlg
( const std::string& name ,
  ISvcLocator*       pSvc )
  : CaloAlgorithm ( name , pSvc )  
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
{
  declareProperty ( "SelectionTools"   , m_selectorsTypeNames    ) ;
  declareProperty ( "CorrectionTools"  , m_correctionsTypeNames  ) ;
  declareProperty ( "HypoTools"        , m_hypotoolsTypeNames    ) ;
  declareProperty ( "CorrectionTools2" , m_correctionsTypeNames2 ) ;
  declareProperty ( "HypoTools2"       , m_hypotoolsTypeNames2   ) ;
  // set the appropriate default values for input data
  setInputData    ( CaloClusterLocation::   Ecal                 ) ;
  // set the appropriate default values for output data
  setOutputData   ( CaloHypoLocation::      Photons              ) ;
  // set the appropriate default values for input data
  setDetData      ( DeCalorimeterLocation:: Ecal                 ) ;
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
  Print( " == > Initialize " , StatusCode::SUCCESS , MSG::DEBUG );  
  // initialize  the base class 
  StatusCode sc = CaloAlgorithm::initialize();
  if( sc.isFailure() ) 
  { return Error("Could not initialize the base class CaloAlgorithm!",sc);}
  // check the geometry information 
  const DeCalorimeter* det = getDet<DeCalorimeter>( detData() ) ;
  if( 0 == det ) { return Error("Detector information is not available!");}
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
  Print( " == > Finalize  " , StatusCode::SUCCESS , MSG::DEBUG );  
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
  return CaloAlgorithm::finalize() ; 
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
  // avoid long names 
  typedef CaloClusters             Clusters ;
  typedef CaloHypos                Hypos    ;
  typedef Clusters::iterator       iterator ;
  
  // get input clusters
  Clusters* clusters = get<Clusters>( inputData() );
  
  // create and the output container of hypotheses and put in to ETS  
  Hypos*    hypos = new Hypos() ;
  put( hypos , outputData() );
  
  // loop over clusters 
  for( iterator cluster = clusters->begin() ; 
       clusters->end() != cluster ; ++cluster )
  {
    bool select = true ;
    // loop over all selectors 
    for( Selectors::const_iterator selector = m_selectors.begin() ;
         select && m_selectors.end() != selector ; ++selector )
    { 
      select = (**selector)( *cluster ); 
    }
    // cluster to be selected? 
    if( !select ) { continue ; }
    
    // create "Hypo"/"Photon" object
    CaloHypo* hypo = new CaloHypo() ;
    // set parameters of newly created hypo 
    hypo->setHypothesis( CaloHypotheses::Photon );      
    hypo->addToClusters( *cluster );
    
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
  
  debug() << " # of created Photon  Hypos is  " << hypos->size() << endreq ;
  
  return StatusCode::SUCCESS;
};
// ============================================================================


// ============================================================================
// The End 
// ============================================================================


