// $Id: LoKiEmpty.cpp,v 1.2 2007-09-22 13:46:50 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
// ============================================================================
/** @file
 *
 *  The most trivial LoKi'based algorithm 
 *
 *  @author   Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 *  @date     2005-07-21
 */
// ============================================================================
LOKI_ALGORITHM( LoKiEmpty ) 
{
  // avoid long names:
  using namespace LoKi        ;
  using namespace LoKi::Types ;  
  using namespace LoKi::Cuts  ;  
  //
  always () << "Empty Algorithm is executed! " << endreq ;
  // 
  return StatusCode::SUCCESS ;                          // RETURN 
}; // end of the algorithm
// ============================================================================
// The END 
// ============================================================================
