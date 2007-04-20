// $Id: LoKi_Phi_Makers.cpp,v 1.3 2007-04-20 11:20:34 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2007/04/16 16:16:53  pkoppenb
// removed polemic comment
//
// Revision 1.1  2007/04/04 12:16:40  ibelyaev
//  v5r2: update
// 
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
#include "GaudiAlg/GaudiTool.h" // Needed on windows
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
LOKI_ALGORITHM(LoKi_Phi_Makers)
{
  using namespace LoKi ;
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
  
} ;



// ============================================================================
/// The END 
// ============================================================================
