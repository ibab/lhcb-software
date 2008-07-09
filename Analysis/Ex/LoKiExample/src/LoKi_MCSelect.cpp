// $Id: LoKi_MCSelect.cpp,v 1.3 2008-07-09 17:00:48 ibelyaev Exp $
// ============================================================================
// Include file 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AlgoMC.h"
#include "LoKi/VertexCuts.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/MCParticleCuts.h"
#include "LoKi/PhysMCParticleCuts.h"
#include "LoKi/ParticleContextCuts.h"
// ============================================================================
/** @file 
 *  Simple helper file which selects "good" MC-events 
 *  to speed-up drastically other examples 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @2008-02-27
 */
// ============================================================================
LOKI_MCALGORITHM(MCSelect)
{
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  // 
  const StatusCode OK ( StatusCode::SUCCESS , true ) ;
  //  
  setFilterPassed ( false ) ;
  
  // get all primary vertices
  VRange primaries = vselect( "PV" , ISPRIMARY ) ;
  if ( primaries.empty() ) 
  { return Warning ( "No Primary vertices are found"       , OK ) ; } // RETURN 
  if ( 1 < primaries.size() ) 
  { return Warning ( "Skip event with several PV-vertices" , OK ) ; } // RETURN 
  
  // get the reconstructed muons & kaons :
  
  Range muons = select ( "muons" , "mu+" == ABSID ) ;
  if ( 2 > muons.size() ) { return OK ; }              // RETURN 
  Range kaons = select ( "kaons" , "K+"  == ABSID ) ;
  if ( 2 > kaons.size() ) { return OK ; }              // RETURN
  
  
  MCFinder finder  = mcFinder("My local MC-decay finder");
  
  MCRange mcbs  = finder->findDecays 
    ( "[ B_s0 -> (  J/psi(1S) ->  mu+  mu- {,gamma} {,gamma} ) (  phi(1020) ->  K+  K- ) ]cc" ) ;
  if ( mcbs .empty() ) 
  { return Warning ( "No MCTruth decay is found" , OK ) ; }  // RETURN 
  if ( 1 != mcbs .size() ) 
  { return Warning ( "No unique MCTruth decay is found" , OK ) ; }  // RETURN 
  
  MCRange mcmu  = finder->findDecays
    ( "[ B_s0 -> (  J/psi(1S) -> ^mu+ ^mu- {,gamma} {,gamma} ) (  phi(1020) ->  K+  K- ) ]cc" ) ;
  MCRange mck   = finder->findDecays
    ( "[ B_s0 -> (  J/psi(1S) ->  mu+  mu- {,gamma} {,gamma} ) (  phi(1020) -> ^K+ ^K- ) ]cc" ) ;
  
  if ( mcmu.empty() || mck.empty() ) { return OK ; }

  MCMatch matcher = mcTruth ( "Bs->PsiPhi") ;
  
  Cut mcBs  =  MCTRUTH ( matcher , mcbs   ) ;
  Cut mcMu  =  MCTRUTH ( matcher , mcmu   ) ;
  Cut mcK   =  MCTRUTH ( matcher , mck    ) ;  
  
  if ( select ( "mu+" , muons , Q > 0 && mcMu ).empty() ) { return OK ; }
  if ( select ( "mu-" , muons , Q < 0 && mcMu ).empty() ) { return OK ; }  
  if ( select ( "K+"  , kaons , Q > 0 && mcK  ).empty() ) { return OK ; }
  if ( select ( "K-"  , kaons , Q < 0 && mcK  ).empty() ) { return OK ; }
  
  // make a loop over all combinations
  for ( Loop Bs = loop ( "mu+ mu- K+ K-" , "B_s0" ) ; Bs ; ++Bs ) 
  {
    // fast evaluation of the mass 
    const double m1234 = Bs->mass(1,2,3,4) / Gaudi::Units::GeV ;
    if ( m1234 < 4.0 || m1234 > 6.0 ) { continue ; }    // CONTINUE 
    
    const double chi2 = VCHI2( Bs ) ;
    if ( chi2 < 0 || 400 < chi2     ) { continue ; }    // CONTINUE
    
    Bs -> save ( "Bs" ) . ignore () ;                  // SAVE
    
  }
  
  if ( !selected("Bs").empty() ) { setFilterPassed ( true ) ; }  
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
// The END 
// ============================================================================

