// $Id: LoKi_Phi_Makers.cpp,v 1.5 2008-07-09 17:00:48 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Algo.h"
#include "LoKi/ParticleCuts.h"
// ============================================================================
/** @file
 *  
 *  Simple exmaple to show various particle makers 
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
 *  @date 2007-03-26
 */
// ============================================================================
LOKI_ALGORITHM(Phi_Makers)
{
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  //
  const StatusCode SUCCESS = StatusCode::SUCCESS ;
  
  Range kaons  = select ( "kaons" , "K+" == ABSID ) ;
  Range kplus  = select ( "k+"    , kaons , 0 < Q ) ;
  Range kminus = select ( "k-"    , kaons , 0 > Q ) ;
  
  for ( Loop phi = loop ( "k+ k-" , "phi(1020)") ; phi ; ++phi ) 
  {
    const double mass = phi->mass(1,2) / Gaudi::Units::GeV ;
    plot ( mass , "k+ k- mass" , 1 , 1.1 ) ;
  }
  
  return  SUCCESS ;                                          // RETURN
  
}
// ============================================================================
// The END 
// ============================================================================
