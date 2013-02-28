// $Id: LoKi_PhiMC.cpp,v 1.6 2008-07-09 17:00:48 ibelyaev Exp $
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
 *  The simple LoKi-based example: plot dikaon mass peak with MC-truth
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
 *  @date 2007-03-22
 */
// ============================================================================
LOKI_MCALGORITHM(PhiMC)
{
  // avoid long names
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;

  const StatusCode SUCCESS = StatusCode::SUCCESS ;
  
  // find MC-phi:
  MCFinder finder = mcFinder("my local MC-finder") ;
  MCRange  mcphi  = finder -> findDecays ( "phi(1020) -> K+ K-") ;
  
  if ( mcphi.empty() ) 
  { return Warning ( "No MC-phi is found!", SUCCESS ) ; }       // RETURN

  // prepare for MC-matching :
  MCMatch match = mcTruth() ;
  Cut mc = MCTRUTH( match , mcphi ) ;
  
  // select all RC-kaons:
  Range kaons  = select ( "kaons" , "K+" == ABSID ) ;
  Range kplus  = select ( "k+"    , kaons , 0 < Q ) ;
  Range kminus = select ( "k-"    , kaons , 0 > Q ) ;
  
   // loop over dikaons:
  for ( Loop dikaon = loop ( "k+ k-" , "phi(1020)" ) ; dikaon ; ++dikaon ) 
  {
    // fast evaluation of mass (4-momenta addition)
    const double m12 = dikaon->mass() ;
    if ( 1150 * Gaudi::Units::MeV < m12 ) { continue ; }           // CONTINUE
    // plot invarinat mass 
    plot ( M(dikaon) / Gaudi::Units::GeV  , "K+ K- mass" , 1 , 1.1 ) ; 
    const double vchi2 = VCHI2( dikaon ) ;
    if ( 0 > vchi2 || 49 < vchi2         ) { continue ; }          // CONTINUE 
    plot ( M(dikaon) / Gaudi::Units::GeV  , "K+ K- mass, chi2_vx<49" , 1 , 1.05 ) ;  
    // check for MC-truth:
    if ( !mc( dikaon)                    ) { continue ; }         // CONITINUE 
    plot ( M(dikaon) / Gaudi::Units::GeV  , "K+ K- mass, MC-truth"   , 1 , 1.05 ) ;    
  }
  
  setFilterPassed ( true ) ;


  return SUCCESS ;                                              // RETURN 
}

// ============================================================================
// The END 
// ============================================================================
