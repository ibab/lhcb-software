// $Id: CaloSinglePhotonAlg.cpp,v 1.1.1.1 2002-11-13 20:46:40 ibelyaev Exp $
// ============================================================================
// CVS atg $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/06/13 12:36:13  ibelyaev
//  new algorithms and new options
//
// Revision 1.1.1.1  2002/05/21 14:18:21  ibelyaev
// New package
// 
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
  , m_selectorsTypeNames   () 
  , m_selectors            ()
  , m_correctionsTypeNames () 
  , m_corrections          () 
  , m_hypotoolsTypeNames   ()
  , m_hypotools            () 
{
  declareProperty ( "SelectionTools"   , m_selectorsTypeNames   ) ;
  declareProperty ( "CorrectionTools"  , m_correctionsTypeNames ) ;
  declareProperty ( "HypoTools"        , m_hypotoolsTypeNames   ) ;
  // set the appropriate default values for input data
  setInputData    ( CaloClusterLocation::   Ecal                ) ;
  // set the appropriate default values for output data
  setOutputData   ( CaloHypoLocation::      Photons             ) ;
  // set the appropriate default values for input data
  setDetData      ( DeCalorimeterLocation:: Ecal                ) ;
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
  const DeCalorimeter* det = get( detSvc() , detData() , det ) ;
  if( 0 == det ) { return Error("Detrector information is not available!");}
  // locate selector tools
  for( Names::const_iterator i1 = m_selectorsTypeNames.begin() ;
       m_selectorsTypeNames.end() != i1 ; ++i1 )
    {
      ICaloClusterSelector* selector   = tool( *i1 , selector );
      m_selectors.push_back( selector );
    }  
  // locate correction tools
  for( Names::const_iterator i2 = m_correctionsTypeNames.begin() ;
       m_correctionsTypeNames.end() != i2 ; ++i2 )
    {
      ICaloHypoTool*  correction       = tool( *i2 , correction );
      m_corrections.push_back( correction );
    }  
  // locate other hypo  tools
  for( Names::const_iterator i3 = m_hypotoolsTypeNames.begin() ;
       m_hypotoolsTypeNames.end() != i3 ; ++i3 )
    {
      ICaloHypoTool*  hypotool         = tool( *i3 , hypotool );
      m_hypotools.push_back(  hypotool  );
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
  // release all tools 
  std::for_each( m_selectors   .begin () , 
                 m_selectors   .end   () , std::mem_fun(&IInterface::release) );
  std::for_each( m_corrections .begin () , 
                 m_corrections .end   () , std::mem_fun(&IInterface::release) );
  std::for_each( m_hypotools   .begin () , 
                 m_hypotools   .end   () , std::mem_fun(&IInterface::release) );
  // clear containers
  m_selectors            .clear () ;
  m_corrections          .clear () ;
  m_hypotools            .clear () ;
  m_selectorsTypeNames   .clear () ;
  m_correctionsTypeNames .clear () ;
  m_hypotoolsTypeNames   .clear () ;
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
          delete hypo ;                                // ATTENTION !
          return Error("Error from Correction Tool " , sc ); 
        }
      
      // loop over other hypo tools (e.g. add extra digits)
      for( HypoTools::const_iterator hypotool = m_hypotools.begin() ;
           sc.isSuccess() && m_hypotools.end() != hypotool ; ++hypotool )
        { sc = (**hypotool) ( hypo ); }

      if( sc.isFailure() ) 
        {
          delete hypo ;                                // ATTENTION !
          return Error("Error from Other Hypo Tool " , sc );
        }

      /// add the hypo into container of hypos 
      hypos->insert( hypo );
      
    } // end of the loop over all clusters
  
  
  MsgStream log( msgSvc() , name() );  
  log << MSG::DEBUG
      << " # of created Photon  Hypos is  " << hypos->size() << endreq ;
  
  return StatusCode::SUCCESS;
};
// ============================================================================


// ============================================================================
// The End 
// ============================================================================


