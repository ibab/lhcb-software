// $Id: CaloHypoReShuffleAlg.cpp,v 1.3 2005-11-07 12:12:42 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/02/17 12:08:08  ibelyaev
//  update for new CaloKernel and CaloInterfaces
//
// Revision 1.1  2003/03/13 18:47:50  ibelyaev
//  see ../doc/release.notes file
// 
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
// Event 
#include "Event/CaloHypo.h"
// local
#include "CaloHypoReShuffleAlg.h"

/** @file 
 *  
 * Implementation file for class : CaloHypoReShuffleAlg
 *
 *  @date 2003-03-13 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
/** @var  CaloHypoReShuffleAlgFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloHypoReShuffleAlg>         s_Factory ;
const        IAlgFactory&CaloHypoReShuffleAlgFactory = s_Factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @see CaloAlgorithm
 *  @see     Algorithm 
 *  @param name name of the algorithm
 *  @param svc  service locator
 */
// ============================================================================
CaloHypoReShuffleAlg::CaloHypoReShuffleAlg 
( const std::string& name,
  ISvcLocator* pSvcLocator)
  : CaloAlgorithm ( name , pSvcLocator ) 
{
  setInputData( CaloHypoLocation::MergedPi0s );
};
// ============================================================================


// ============================================================================/
// destructor 
// ============================================================================
CaloHypoReShuffleAlg::~CaloHypoReShuffleAlg() {}; 
// ============================================================================

// ============================================================================
/** execution of the algorithm 
 *  @return statsu code 
 */
// ============================================================================
StatusCode CaloHypoReShuffleAlg::execute() 
{

  // avoid long names 
  typedef CaloHypos           Hypos    ;
  typedef CaloHypo::Clusters  Clusters ;
  
  Hypos* hypos = get<Hypos>( inputData() );
  if( 0 == hypos ) { return StatusCode::FAILURE ; }
  
  for( Hypos::iterator ihypo = hypos->begin() ; 
       hypos->end() != ihypo ; ++ihypo ) 
  {
      CaloHypo* pi0 = *ihypo ;
      if( 0 == pi0                                           ) { continue ; }
      if( CaloHypotheses::Pi0Merged != pi0 -> hypothesis ()  ) { continue ; }
      if( 2 != pi0->hypos().size()                           ) { continue ; }
      
      CaloHypo* g1 = *(pi0->hypos().begin()     ) ;
      if( 0 == g1                                            ) { continue ; }
      CaloHypo* g2 = *(pi0->hypos().begin() + 1 ) ;
      if( 0 == g2                                            ) { continue ; }
      
      Clusters& cls1 = g1->clusters();
      Clusters& cls2 = g2->clusters();
      if      ( 3 == cls1.size() && 1 == cls2.size() ) 
      {
          cls2.push_back ( *( cls1.begin() + 2 ) ) ;
          cls1.erase     (    cls1.begin() + 2   ) ;
      }
      else if ( 1 == cls1.size() && 3 == cls2.size() ) 
      {
        cls1.push_back ( *( cls2.begin() + 2 ) ) ;
        cls2.erase     (    cls2.begin() + 2   ) ;
      }
      else { Warning("Resuffling can be skipped!"); }
      
  };
  
  return StatusCode::SUCCESS;
};
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
