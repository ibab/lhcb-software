// $Id: LoKi_HelloWorld.cpp,v 1.6 2008-07-09 17:00:48 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Algo.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  The most trivial LoKi-based algorithm: "Hello,World!"
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-03-22
 */
// ============================================================================
LOKI_ALGORITHM(HelloWorld)
{
  info() << "Hello,World!" << endmsg ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
// The END 
// ============================================================================
