// $Id: LoKi_HelloWorld.cpp,v 1.3 2007-04-16 16:16:53 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2007/04/04 12:16:39  ibelyaev
//  v5r2: update
//
//
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
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
LOKI_ALGORITHM(LoKi_HelloWorld)
{
  info() << "Hello,World!" << endreq ;
  
  return StatusCode::SUCCESS ;
};
// ============================================================================
// The END 
// ============================================================================
