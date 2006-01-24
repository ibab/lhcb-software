// $Id: Range.cpp,v 1.1.1.1 2006-01-24 09:39:41 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Range.h"
#include "LoKi/Exception.h"
// ============================================================================


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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot at al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

// ============================================================================
/** @fn rangeException
 *  Helpful function to throw an out-of-range exception for class Range_
 *  @param index invalid index
 *  @param size  range size 
 */
// ============================================================================
void LoKi::Range_Local::rangeException ( const long   /* index */ , 
                                         const size_t /* size  */ )
{
  throw LoKi::Exception ("Out-of-Range for class LoKi::Range_");
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
