// $Id: Separators.h,v 1.2 2006-02-18 18:06:04 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2006/01/24 09:39:42  ibelyaev
// New Import: the core part of restructurized LoKi project
//
// ============================================================================
#ifndef LOKI_TUPLES_H 
#define LOKI_TUPLES_H 1
// ============================================================================
// Include files
// ============================================================================
// STD& STL 
// ============================================================================
#include <string>
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
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

namespace LoKi
{
  /** @var Separators 
   *
   *  string of possible separators for list items 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2003-02-08
   */
  const std::string Separators(" ,;");
  
}; // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TUPLES_H
// ============================================================================
