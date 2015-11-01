// $Id: CheckMC_Bs2PsiPhi.cpp,v 1.3 2008-07-09 17:00:48 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AlgoMC.h"
#include "LoKi/LoKiAlgoMC.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/PhysMCParticleCuts.h"
#include "LoKi/PrintDecay.h"
#include "LoKi/Child.h"
// ============================================================================
/** @file
 *  Simple LoKi-based algoritm for Bs->phi psi selection 
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-22
 */
// ============================================================================
LOKI_MCALGORITHM ( CheckMC_Bs2PsiPhi )
{
  // avoid long names
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  using namespace LoKi::Child ;
  
  const StatusCode SUCCESS = StatusCode::SUCCESS ;
  
  MCFinder finder = mcFinder() ;
  
  // get true Bs 
  MCRange mcbs  = finder->findDecays 
    ( "[ B_s0 -> (  J/psi(1S) ->  mu+  mu- { , gamma } { , gamma } ) (  phi(1020) ->  K+  K- ) ]cc" ) ;
  MCRange mcpsi = finder->findDecays 
    ( "[ B_s0 -> ( ^J/psi(1S) ->  mu+  mu- { , gamma } { , gamma } ) (  phi(1020) ->  K+  K- ) ]cc" ) ;
  MCRange mcphi = finder->findDecays
    ( "[ B_s0 -> (  J/psi(1S) ->  mu+  mu- { , gamma } { , gamma } ) ( ^phi(1020) ->  K+  K- ) ]cc" ) ;
  MCRange mcmu  = finder->findDecays
    ( "[ B_s0 -> (  J/psi(1S) -> ^mu+ ^mu- { , gamma } { , gamma } ) (  phi(1020) ->  K+  K- ) ]cc" ) ;
  MCRange mck   = finder->findDecays
    ( "[ B_s0 -> (  J/psi(1S) ->  mu+  mu- { , gamma } { , gamma } ) (  phi(1020) -> ^K+ ^K- ) ]cc" ) ;
  
  if ( mcbs  .empty() ) { return Warning ( "No MC-Bs  is found!" , SUCCESS ) ; } 
  if ( mcpsi .empty() ) { return Warning ( "No MC-psi is found!" , SUCCESS ) ; } 
  if ( mcphi .empty() ) { return Warning ( "No MC-phi is found!" , SUCCESS ) ; }
  if ( mcmu  .empty() ) { return Warning ( "No MC-mu  is found!" , SUCCESS ) ; }
  if ( mck   .empty() ) { return Warning ( "No MC-K   is found!" , SUCCESS ) ; }

  MCMatch matcher = mcTruth ( "Bs->PsiPhi") ;
  
  Cut mcBs  =  MCTRUTH ( matcher , mcbs   ) ;
  Cut mcPsi =  MCTRUTH ( matcher , mcpsi  ) ;
  Cut mcPhi =  MCTRUTH ( matcher , mcphi  ) ;
  Cut mcMu  =  MCTRUTH ( matcher , mcmu   ) ;
  Cut mcK   =  MCTRUTH ( matcher , mck    ) ;
  
  Cut Psi  = "J/psi(1S)" == ID ;
  Cut Phi  = "phi(1020)" == ID ;
  Cut Kp   = "K+"        == ID ;
  Cut Km   = "K-"        == ID ;
  Cut Mup  = "mu+"       == ID ;
  Cut Mum  = "mu-"       == ID ;
  
  // select the reconstructed Bs form the Input locations 
  Range Bs = select 
    ( "Bs" , "B_s0" == ABSID 
      && 1 == NINTREE ( Psi )
      && 1 == NINTREE ( Phi )
      && 2 == NINTREE ( "K+"  == ABSID ) 
      && 2 == NINTREE ( "mu+" == ABSID ) ) ;
  
  if ( Bs.empty() ) 
  { Warning ("No recontructed Bs are found! ") ;}
  

  // loop over all reconstructed Bs 
  for ( Range::iterator iB = Bs.begin() ; Bs.end() != iB ; ++iB ) 
  {
    const LHCb::Particle* B = *iB ;
    if ( 0 == B ) { continue ; }                        // CONTINUE
    
    // the first  daughter 
    const LHCb::Particle* d1 = Child::child ( B , 1 ) ;
    // the second daughter 
    const LHCb::Particle* d2 = Child::child ( B , 2 ) ;
    
    // J/psi 
    const LHCb::Particle* psi = 0 ;
    const LHCb::Particle* phi = 0 ;
    
    if      ( Psi ( d1 ) && Phi ( d2 ) ) { psi = d1 ; phi = d2 ; }
    else if ( Psi ( d2 ) && Phi ( d1 ) ) { psi = d2 ; phi = d1 ; }
    
    if ( 0 == psi || 0 == psi ) 
    { Warning ( "Invalid daughters for Bs" ) ; continue ; }
    
    const LHCb::Particle* k1 = Child::child ( phi , 1 ) ;
    const LHCb::Particle* k2 = Child::child ( phi , 2 ) ;
    
    // Kaons 
    const LHCb::Particle* kp = 0 ;
    const LHCb::Particle* km = 0 ;
    if      ( Kp ( k1 ) && Km ( k2 ) ) { kp = k1 ; km = k2 ; }
    else if ( Kp ( k2 ) && Km ( k1 ) ) { kp = k2 ; km = k1 ; }
    
    if ( 0 == kp || 0 == km ) 
    { Warning ("Invalid daughters for phi") ; continue ; }
    
    
    const LHCb::Particle* mu1 = Child::child ( psi , 1 ) ;
    const LHCb::Particle* mu2 = Child::child ( psi , 2 ) ;
    
    // Kaons 
    const LHCb::Particle* mup = 0 ;
    const LHCb::Particle* mum = 0 ;
    if      ( Mup ( mu1 ) && Mum ( mu2 ) ) { mup = mu1 ; mum = mu2 ; }
    else if ( Mup ( mu2 ) && Mum ( mu1 ) ) { mup = mu2 ; mum = mu1 ; }
    
    if ( 0 == kp || 0 == km ) 
    { Warning ( "Invalid daughters for psi" ) ; continue ; }
    
    // now we all particles:
    // psi, phi, kp, km, mup, mum
    
    const bool trueKp  = mcK   ( kp  ) ;
    const bool trueKm  = mcK   ( km  ) ;
    
    const bool trueMup = mcMu  ( mup ) ;
    const bool trueMum = mcMu  ( mum ) ;
    
    const bool truePsi = mcPsi ( psi ) ;
    const bool truePhi = mcPhi ( phi ) ;
    
    const bool trueBs  = mcBs  ( B   ) ;
    
    
    info() 
      << " Bs candidate: "  << LoKi::PrintPhys::printDecay ( B )  << std::endl
      << " \t" << " K+  ? " << ( trueKp  ? "True" : "False" )     << std::endl
      << " \t" << " K-  ? " << ( trueKm  ? "True" : "False" )     << std::endl 
      << " \t" << " mu+ ? " << ( trueMup ? "True" : "False" )     << std::endl
      << " \t" << " mu- ? " << ( trueMum ? "True" : "False" )     << std::endl 
      << " \t" << " psi ? " << ( truePsi ? "True" : "False" )     << std::endl
      << " \t" << " phi ? " << ( truePhi ? "True" : "False" )     << std::endl 
      << " \t" << " Bs  ? " << ( trueBs  ? "True" : "False" )     << endmsg ;
    
  }
  
  return SUCCESS ;                                             // RETURN 
}
// ============================================================================
// The END 
// ============================================================================
