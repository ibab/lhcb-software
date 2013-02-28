// $Id: LoKi_Bs2PsiPhi.cpp,v 1.9 2008-08-04 09:55:23 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AlgoMC.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/VertexCuts.h"
#include "LoKi/PhysMCParticleCuts.h"
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
LOKI_MCALGORITHM(Bs2PsiPhi)
{
  // avoid long names
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;

  const StatusCode SUCCESS = StatusCode::SUCCESS ;

  // get all primary vertices
  VRange primaries = vselect( "PV" , ISPRIMARY ) ;
  if ( primaries.empty() )
  { return Warning ( "No Primary vertices are found!" , SUCCESS ) ; } // RETURN

  // minimal impact parameter chi2 :
  Fun mips = MIPCHI2 ( primaries, geo() ) ;

  MCFinder finder  = mcFinder("My local MC-decay finder");

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

  // select all muons
  Range muons = select ( "muons" , "mu+" == ABSID ) ;
  if ( muons.empty() ) { return Warning ( "No RC-muons" , SUCCESS) ; } // RETURN
  select ( "mu+" , muons , 0 < Q ) ;
  select ( "mu-" , muons , 0 > Q ) ;

  // select all kaons
  Range kaons = select( "kaons"  , "K+"  == ABSID ) ;
  if ( kaons.empty() ) { return Warning ( "No RC-kaons" , SUCCESS) ; } // RETURN
  select ( "K+"  , kaons , 0 < Q ) ;
  select ( "K-"  , kaons , 0 > Q ) ;

  const double GeV = Gaudi::Units::GeV ;

  // make a loop over all dimuons
  for ( Loop mm = loop ( "mu+ mu-" , "J/psi(1S)" ) ; mm  ; ++mm )
  {
    // fast evaluationof the mass (4-momenta adding)
    const double m12 = mm->mass()  ;
    if ( 3.5 * GeV < m12 || 2.5 * GeV > m12 ) { continue ; }   // CONTINUE
    plot ( m12 / GeV , " mu mu  mass ", 2.5 , 3.5 ) ;
    // chi2 of vertex fit
    const double vchi2 = VCHI2( mm )  ;
    if ( 0 > vchi2  ||  49 < vchi2          ) { continue ; }   // CONTINUE
    // refit ?
    StatusCode sc = mm->reFit() ;
    if ( sc.isFailure() )                     { continue ; }   // CONTINUE
    // make precise mass evaluation
    const double mass = M ( mm ) / GeV ;
    plot ( mass , " mu mu  mass, chi2_vx < 49 " , 1. , 1.1 ) ;
    // get children:
    const LHCb::Particle* mu1 = mm(1) ;
    const LHCb::Particle* mu2 = mm(1) ;
    if ( mcMu ( mu1 ) && mcMu ( mu2 ) && mcPsi ( mm ) )
    {
      plot ( mass , " mu mu  mass (MC) ", 2.5 , 3.5 ) ;
      mm->save("psi").ignore() ;                          // SAVE "psi"
    }
  } // end of loop over di,uons

  // get all selected psis
  Range psis = selected ( "psi" ) ;
  if ( psis.empty() ) { return Warning ( "No RC-psi" , SUCCESS ) ; } // RETURN

  // loop over all dikaons
  for ( Loop phi = loop( "K+ K-" , "phi(1020)" ) ; phi ; ++phi )
  {
    // fast evaluation of the mass (4-momenta addition)
    const double m12 = phi->mass() ;
    if ( 1.1 * GeV < m12 )                  { continue ; }        // CONTINUE
    plot ( m12 / GeV , "K+K- mass " , 1. , 1.1 ) ;
    // chi2 of vertex fit
    const double vchi2 = VCHI2( phi ) ;
    if ( 0 > vchi2 || 49 < vchi2  )        { continue ; }        // CONTINUE
    // precise mass evaluation:
    const double mass = M ( phi ) / GeV  ;
    plot ( mass , "K+K- mass, chi2_vx < 49 " , 1. , 1.1 ) ;
    // get children:
    const LHCb::Particle* k1 = phi(1) ;
    const LHCb::Particle* k2 = phi(2) ;
    if( mcK ( k1 ) && mcK ( k2 ) && mcPhi ( phi ) )
    {
      plot ( mass , "K+K- mass (MC)" , 1. , 1.1 ) ;
      phi->save("phi").ignore() ;                                 // SAVE "phi"
    }
  } // end of loop over dikaons

    // get all selected psis
  Range phis= selected ( "phi" ) ;
  if ( phis.empty() ) { return Warning ( "No RC-phi" , SUCCESS ) ; } // RETURN

  // loop over selected psi & phi
  for ( Loop b = loop("psi phi", "B_s0") ; b ; ++b )
  {
    // fast evaluation of mass (4-momenta addition)
    const double m12 = b->mass(1,2) ;
    if ( 4 * GeV > m12 || 6 * GeV < m12 ) { continue ; }        // CONTINUE
    plot ( m12 / GeV  , " psi phi mass " , 4 , 6 ) ;
    // chi2 of vertex fit
    const double vchi2 = VCHI2 ( b ) ;
    if ( 0 > vchi2     || 49 < vchi2    ) { continue ; }        // CONTINUE
    // presice mass evaluation
    const double mass = M ( b ) / GeV ;
    plot ( mass , " psi phi mass, chi2_vx < 49  " , 4 , 6 ) ;
    // get children:
    const LHCb::Particle* psi = b(1) ;
    const LHCb::Particle* phi = b(2) ;
    if ( 0 == psi || 0 == phi )           { continue ; }        // CONTINUE

    const bool psiMC = mcPsi ( psi ) ;
    const bool phiMC = mcPhi ( phi ) ;
    const bool bMC   = mcBs  ( b   ) ;

    if ( psiMC )
    { plot ( mass , " psi(MC) phi     mass        " , 4 , 6 ) ; }
    if ( phiMC )
    { plot ( mass , " psi     phi(MC) mass        " , 4 , 6 ) ; }
    if ( psiMC && phiMC )
    { plot ( mass , " psi(MC) phi(MC) mass        " , 4 , 6 ) ; }
    if ( bMC   )
    { plot ( mass , " psi     phi     mass (MC-1) " , 4 , 6 ) ; }
    if ( bMC && psiMC && phiMC )
    { plot ( mass , " psi(MC) phi(MC) mass (MC-1) " , 4 , 6 ) ; }

    // find proper primary vertex
    VRange::iterator ipv = LoKi::select_min
      ( primaries.begin() , primaries.end() , VIPCHI2 ( b , geo() ) ) ;
    if ( primaries.end() == ipv )        { continue ; }          // CONTINUE

    Fun dira = DIRA( *ipv ) ;
    plot ( dira( b ) , "Cosine Direction Angle" , 0.995 , 1.0 ) ;

    b->save("Bs").ignore() ;                                  // SAVE "Bs"

  } // end of loop over psis and phis

  Range Bs = selected ( "Bs" ) ;
  if ( !Bs.empty() ) { setFilterPassed( true ) ; }             // IMPORTANT

  return SUCCESS ;                                             // RETURN
}

// ============================================================================
// The END
// ============================================================================
