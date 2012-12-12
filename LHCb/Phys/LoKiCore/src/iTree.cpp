// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <sstream>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/iTree.h"
// ============================================================================
/** @file 
 *  Implementation file for class Decays::iTree
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
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-05-06 
 */
// ============================================================================
// conversion to the string 
// ============================================================================
std::string Decays::iTree::toString() const 
{
  std::ostringstream s ;
  fillStream ( s ) ;
  return s.str () ;
}
// ============================================================================
// destructor 
// ============================================================================
Decays::iTree::~iTree() {}
// ============================================================================
/*  The standard output stream operator for the sub-tree
 *  @param s the reference to the outptu stream 
 *  @param o the decay tree objects 
 *  @return the reference to the outptu stream
 */
// ============================================================================
std::ostream& operator<< ( std::ostream& s , const Decays::iTree& o )
{ return o.fillStream ( s ) ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
