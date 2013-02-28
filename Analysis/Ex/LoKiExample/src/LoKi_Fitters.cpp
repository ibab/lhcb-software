// $Id: LoKi_Fitters.cpp,v 1.5 2008-10-10 12:37:55 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AlgoMC.h"
#include "LoKi/VertexCuts.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/MCParticleCuts.h"
#include "LoKi/PhysMCParticleCuts.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/EigenSystem.h"
#include "LHCbMath/MatrixUtils.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IVertexFit.h"
#include "Kernel/IParticleReFitter.h"
#include "Kernel/IDistanceCalculator.h"
// ============================================================================
/** @file 
 *  simple LoK-based algorithm to test the various fitters 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2008-02-19
 */
// ============================================================================
LOKI_MCALGORITHM(Fitters)
{
  // ==========================================================================
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  // ==========================================================================
  
  setFilterPassed ( false ) ;
  
  const StatusCode SUCCESS = StatusCode ( StatusCode::SUCCESS , true ) ;
    
  // get all primary vertices
  VRange primaries = vselect( "PV" , ISPRIMARY ) ;
  if ( primaries.empty() ) 
  { return Warning ( "No Primary vertices are found!" , SUCCESS ) ; } // RETURN 
  
  if ( 1 < primaries.size() ) 
  { return Warning ( "Skip event with several PV-vertices" , SUCCESS ) ; } // RETURN 
  
  // minimal impact parameter chi2 :
  Fun mips = MIPCHI2 ( primaries, geo() ) ;
  
  MCFinder finder  = mcFinder("My local MC-decay finder");
  
  MCRange mcbs  = finder->findDecays 
    ( "[ B_s0 -> (  J/psi(1S) ->  mu+  mu- ) (  phi(1020) ->  K+  K- ) ]cc" ) ;
  MCRange mcpsi = finder->findDecays 
    ( "[ B_s0 -> ( ^J/psi(1S) ->  mu+  mu- ) (  phi(1020) ->  K+  K- ) ]cc" ) ;
  MCRange mcphi = finder->findDecays
    ( "[ B_s0 -> (  J/psi(1S) ->  mu+  mu- ) ( ^phi(1020) ->  K+  K- ) ]cc" ) ;
  MCRange mcmu  = finder->findDecays
    ( "[ B_s0 -> (  J/psi(1S) -> ^mu+ ^mu- ) (  phi(1020) ->  K+  K- ) ]cc" ) ;
  MCRange mck   = finder->findDecays
    ( "[ B_s0 -> (  J/psi(1S) ->  mu+  mu- ) (  phi(1020) -> ^K+ ^K- ) ]cc" ) ;
  
  if ( mcbs  .empty() ) { return SUCCESS ; } 
  if ( mcpsi .empty() ) { return SUCCESS ; } 
  if ( mcphi .empty() ) { return SUCCESS ; }
  if ( mcmu  .empty() ) { return SUCCESS ; }
  if ( mck   .empty() ) { return SUCCESS ; }
  
  if ( 1 != mcbs .size() ) { return Warning ( "No unique  is found!" , SUCCESS ) ; } 
  
  MCMatch matcher = mcTruth ( "Bs->PsiPhi") ;
  
  Cut mcBs  =  MCTRUTH ( matcher , mcbs   ) ;
  Cut mcPsi =  MCTRUTH ( matcher , mcpsi  ) ;
  Cut mcPhi =  MCTRUTH ( matcher , mcphi  ) ;
  Cut mcMu  =  MCTRUTH ( matcher , mcmu   ) ;
  Cut mcK   =  MCTRUTH ( matcher , mck    ) ;
  
  // select all muons
  Range muons = select ( "muons"    , "mu+" == ABSID ) ;
  if ( 2 > muons.size() )                                 { return SUCCESS ; } // RETURN
  if ( select ( "mu+" , muons , 0 < Q && mcMu ).empty() ) { return SUCCESS ; } // RETURN
  if ( select ( "mu-" , muons , 0 > Q && mcMu ).empty() ) { return SUCCESS ; } // RETURN
  
  
  // select all kaons 
  Range kaons = select( "kaons"     , "K+"  == ABSID ) ;
  if ( 2 > kaons.size() )                                { return SUCCESS ; } // RETURN
  if ( select ( "K+"  , kaons , 0 < Q && mcK ).empty() ) { return SUCCESS ; } // RETURN  
  if ( select ( "K-"  , kaons , 0 > Q && mcK ).empty() ) { return SUCCESS ; } // RETURN
  
  
  const LHCb::MCVertex* mcv = mcbs.front()->endVertices().front() ;
  if ( 0 == mcv ) { return Warning("No MCVertex", SUCCESS ) ; }
  
  
  // const double mc_tau = MCCTAU ( mcbs.front() ) ;

  const LHCb::VertexBase* primary = primaries.front() ;
  
  Fun dira = acos ( DIRA ( primary ) ) ;

  bool good = false ;
  
  VFun vdchi2  = VVDCHI2 ( primary ) ;
  
  Tuple tuple = nTuple("imppar") ;
  
  // make a loop over all combinations
  for ( Loop Bs = loop ( "mu+ mu- K+ K-" , "B_s0" ) ; Bs ; ++Bs ) 
  {
    const double m1234 = Bs->mass(1,2,3,4) / Gaudi::Units::GeV ;
    if ( m1234 < 5.2 || m1234 > 5.4 ) { continue ; }
    
    plot ( m1234 , " 1 - m1234" ,  5.2 , 5.4 ) ;
    
    if ( !mcPsi ( Bs ( 1 ) ) ) { continue ; }
    if ( !mcPsi ( Bs ( 2 ) ) ) { continue ; }
    if ( !mcPhi ( Bs ( 3 ) ) ) { continue ; }
    if ( !mcPhi ( Bs ( 4 ) ) ) { continue ; }
    if ( !mcBs  ( Bs ( 1 ) ) ) { continue ; }
    if ( !mcBs  ( Bs ( 2 ) ) ) { continue ; }
    if ( !mcBs  ( Bs ( 3 ) ) ) { continue ; }
    if ( !mcBs  ( Bs ( 4 ) ) ) { continue ; }
    
    plot ( m1234 , " 2 - m1234" ,  5.2 , 5.4 ) ;
    
    const double chi2 = VCHI2 ( Bs ) ;
    if ( 0   > chi2 ) { continue ; }
    if ( 16  < chi2 ) { continue ; }
    
  }

  setFilterPassed ( good ) ;

  // ==========================================================================
  return StatusCode::SUCCESS ;                                        // RETURN 
}
// ============================================================================
// The END 
// ============================================================================
