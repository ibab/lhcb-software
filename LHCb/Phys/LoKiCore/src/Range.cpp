// $Id: Range.cpp,v 1.5 2007-07-23 17:07:43 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Range.h"
#include "LoKi/Exception.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================
/* Helpful function to throw an out-of-range exception for class Range_
 *  @param index invalid index
 *  @param size  range size 
 */
// ============================================================================
void LoKi::Range_Local::rangeException ( const long   /* index */ , 
                                         const size_t /* size  */ )
{
  throw LoKi::Exception ("Out-of-Range for class LoKi::Range_");
}
// ============================================================================
// The END 
// ============================================================================
