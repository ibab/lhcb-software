// $Id: CaloElectronAlg.cpp,v 1.4 2003-12-11 16:33:40 cattanem Exp $
// ============================================================================
// CVS atg $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2003/05/15 19:27:02  ibelyaev
//  fix error exists/stops
//
// Revision 1.2  2002/12/09 17:43:09  cattanem
// bug fixes
//
// Revision 1.1.1.1  2002/11/13 20:46:39  ibelyaev
// new package 
//
// ============================================================================
// Include files
// STD & STL 
#include <algorithm>
#include <functional>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/MsgStream.h"
// CaloDet 
#include "CaloDet/DeCalorimeter.h"
// Event/CaloEvent 
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
// CaloInterfaces
#include "CaloInterfaces/ICaloClusterSelector.h"
#include "CaloInterfaces/ICaloHypoTool.h"
// local
#include "CaloElectronAlg.h"

// ============================================================================
/** @file
 * 
 *  Implementation file for class : CaloElectronAlg
 *  The implementation is based on F.Machefert's codes.
 *  @see CaloElectronAlg
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 31/03/2002 
 */
// ============================================================================

// ============================================================================
/** @var CaloElectronAlgFactory
 *  mandatory instantiation of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloElectronAlg>         s_Factory ;
const        IAlgFactory&CaloElectronAlgFactory = s_Factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param name algorithm name 
 *  @param pSvc service locator 
 */
// ============================================================================
CaloElectronAlg::CaloElectronAlg
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
  // set the appropriate default value for input data 
  setInputData    ( CaloClusterLocation::   Ecal                 ) ;
  // set the appropriate default value for output data 
  setOutputData   ( CaloHypoLocation::      Electrons            ) ;
  // set the appropriate default value for detector data 
  setDetData      ( DeCalorimeterLocation:: Ecal                 ) ;  
};
// ============================================================================

// ============================================================================
/// destructor (protected and virtual)
// ============================================================================
CaloElectronAlg::~CaloElectronAlg() {}; 
// ============================================================================

// ============================================================================
/**  standard Algorithm initialization
 *   @return status code 
 */
// ============================================================================
StatusCode
CaloElectronAlg::initialize() 
{
  Print( " == > Initialize " , StatusCode::SUCCESS , MSG::DEBUG );  
  // initialize  the base class 
  StatusCode sc = CaloAlgorithm::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class CaloAlgorithm!",sc);}
  // check the geometry information 
  const DeCalorimeter* det = get( detSvc() , detData() , det ) ;
  if( 0 == det ) { return Error("Detector information is not available!");}
  { // locate selector tools
    for( Names::const_iterator item = m_selectorsTypeNames.begin() ;
         m_selectorsTypeNames.end() != item ; ++item )
      {
        ICaloClusterSelector* selector   = tool( *item , selector );
        m_selectors.push_back( selector );
      }
  }
  { // locate correction tools
    for( Names::const_iterator item = m_correctionsTypeNames.begin() ;
         m_correctionsTypeNames.end() != item ; ++item )
      {
        ICaloHypoTool*  correction       = tool( *item , correction );
        m_corrections.push_back( correction );
      }
  }
  { // locate other hypo  tools
    for( Names::const_iterator item = m_hypotoolsTypeNames.begin() ;
         m_hypotoolsTypeNames.end() != item ; ++item )
      {
        ICaloHypoTool*  hypotool         = tool( *item , hypotool );
        m_hypotools.push_back(  hypotool  );
      }
  }
  { // locate correction tools
    for( Names::const_iterator item = m_correctionsTypeNames2.begin() ;
         m_correctionsTypeNames2.end() != item ; ++item )
      {
        ICaloHypoTool*  correction       = tool( *item , correction );
        m_corrections2.push_back( correction );
      }
  }
  { // locate other hypo  tools
    for( Names::const_iterator item = m_hypotoolsTypeNames2.begin() ;
         m_hypotoolsTypeNames2.end() != item ; ++item )
      {
        ICaloHypoTool*  hypotool         = tool( *item , hypotool );
        m_hypotools2.push_back(  hypotool  );
      }
  }

  ///
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/*
 *  simple utiliti to count digiyts from certain calorimeter 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 31/03/2002 
 */
class DigitFromCalo 
  : public std::unary_function<const CaloDigit*,bool>
{ 
public:

  /** constructor
   *  @parameter calo  calorimeter name 
   */
  explicit DigitFromCalo( const std::string& calo )
    : m_calo( CaloCellCode::CaloNumFromName( calo ) ) {} ;
  
  /** constructor
   *  @parameter calo  calorimeter index 
   */
  explicit DigitFromCalo( const int  calo )
    : m_calo(                                calo   ) {} ;
  
  /** the only essential method
   *  @param digit pointer to CaloDigit object 
   *  @return true if digit belongs to the predefined calorimeter 
   */
  inline bool operator() ( const CaloDigit* digit ) const 
  {
    if( 0 == digit ) { return false ; }
    return (int) digit->cellID().calo() == m_calo ;
  };
  
private:
  
  /// default constructor is private 
  DigitFromCalo();
  
private:
  
  int m_calo ;
  
};
// ============================================================================

// ============================================================================
/**  standard Algorithm finalization
 *   @return status code 
 */
// ============================================================================
StatusCode 
CaloElectronAlg::finalize() 
{
  Print( " == > Finalize  " , StatusCode::SUCCESS , MSG::DEBUG );  
  // release all tools 
 // release all tools 
  std::for_each
    ( m_selectors    .begin () , 
      m_selectors    .end   () , std::mem_fun(&IInterface::release) );
  std::for_each
    ( m_corrections  .begin () , 
      m_corrections  .end   () , std::mem_fun(&IInterface::release) );
  std::for_each
    ( m_hypotools    .begin () , 
      m_hypotools    .end   () , std::mem_fun(&IInterface::release) );
  std::for_each
    ( m_corrections2 .begin () , 
      m_corrections2 .end   () , std::mem_fun(&IInterface::release) );
  std::for_each
    ( m_hypotools2   .begin () , 
      m_hypotools2   .end   () , std::mem_fun(&IInterface::release) );
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
CaloElectronAlg::execute() 
{
  Print( " == > Execute   " , StatusCode::SUCCESS , MSG::DEBUG );  

  
  // avoid long names 
  typedef CaloClusters             Clusters ;
  typedef CaloHypos                Hypos    ;
  typedef Clusters::iterator       iterator ;
  
  // get input clusters
  Clusters* clusters = 
    get( eventSvc() , inputData() , clusters );
  
  // create and the output container of hypotheses and put in to ETS  
  Hypos*    hypos = new Hypos() ;
  put( hypos , outputData() );
  
  DigitFromCalo prs( "Prs" );
  DigitFromCalo spd( "Spd" );

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
      hypo->setHypothesis( CaloHypotheses::Photon ); /// temporary!      
      hypo->addToClusters( *cluster );
      
      StatusCode sc( StatusCode::SUCCESS );
      // loop over all corrections and apply corrections  
      for( Corrections::const_iterator correction = m_corrections.begin() ;
           sc.isSuccess() && m_corrections.end() != correction ; ++correction )
        { sc = (**correction) ( hypo ); }
      
      if( sc.isFailure() ) 
        {
          delete hypo ; hypo = 0  ;                            // ATTENTION !
          Error("Error from Correction Tool, skip the cluster " , sc ); 
          continue ;                                           // CONTINUE 
        }
      

      // loop over other hypo tools (e.g. add extra digits)
      for( HypoTools::const_iterator hypotool = m_hypotools.begin() ;
           sc.isSuccess() && m_hypotools.end() != hypotool ; ++hypotool )
        { sc = (**hypotool) ( hypo ); }
      
      if( sc.isFailure() ) 
        {
          delete hypo ;                                // ATTENTION !
          hypo   =  0 ;
          Error("Error from Other Hypo Tool, skip the cluster " , sc );
          continue    ;                                // CONTINUE  !
        }
      
      // need to have at least 2 "extra" digits (1 Spd & 1 Prs)
      // count Spd digits 
      if( 1 > std::count_if( hypo->digits().begin () , 
                             hypo->digits().end   () , spd ) )
        {
          delete hypo ;
          hypo = 0    ;
          continue    ;
        }
      // count Prs digits 
      if( 1 > std::count_if( hypo->digits().begin () , 
                             hypo->digits().end   () , prs ) ) 
        {
          delete hypo ;
          hypo = 0    ;
          continue    ;
        }
      
      // loop over all corrections and apply corrections  
      for( Corrections::const_iterator cor2 = m_corrections2.begin() ;
           sc.isSuccess() && m_corrections2.end() != cor2 ; ++cor2 )
        { sc = (**cor2) ( hypo ); }
      
      if( sc.isFailure() ) 
        {
          delete hypo ;                                // ATTENTION !
          hypo = 0    ;
          Error("Error from Correction Tool 2 , skip the cluster " , sc );
          continue  ;                                  // CONTINUE 
        }
      
      // loop over other hypo tools (e.g. add extra digits)
      for( HypoTools::const_iterator hypotool2 = m_hypotools2.begin() ;
           sc.isSuccess() && m_hypotools2.end() != hypotool2 ; ++hypotool2 )
        { sc = (**hypotool2) ( hypo ); }
      
      if( sc.isFailure() ) 
        {
          delete hypo ;                                // ATTENTION !
          hypo = 0    ;
          Error("Error from Other Hypo Tool 2 , skip the cluster " , sc );
          continue    ;
        }

      // set "correct" hypothesis
      hypo->setHypothesis( CaloHypotheses::EmCharged ); /// final!

      /// set new Z 
      /// hypo->position()->setZ( hypo->position()->z() - 200.0 );      
      
      /// add the hypo into container of hypos 
      if( 0 != hypo ) { hypos->insert( hypo ); }
      
    } // end of the loop over all clusters
  
  
  MsgStream log( msgSvc() , name() );
  log << MSG::DEBUG
      << " # of created Electron Hypos is  " << hypos->size() << endreq ;
  
  return StatusCode::SUCCESS;
};
// ============================================================================


// ============================================================================
// The End 
// ============================================================================


