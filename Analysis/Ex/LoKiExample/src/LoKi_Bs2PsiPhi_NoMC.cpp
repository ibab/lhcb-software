// $Id: LoKi_Bs2PsiPhi_NoMC.cpp,v 1.3 2008-07-09 17:00:48 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Algo.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/VertexCuts.h"
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
LOKI_ALGORITHM(Bs2PsiPhi_NoMC)
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
    // make precise mass evaluation 
    const double mass = M ( mm ) / GeV ;
    plot ( mass , " mu mu  mass, chi2_vx < 49 " , 2.5 , 3.5 ) ;
    //
    mm->save("psi").ignore() ;                          // SAVE "psi"
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
    //
    phi->save("phi").ignore() ;                                 // SAVE "phi"    
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
