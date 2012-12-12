// $Id$
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ICoreFactory.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hybrid::ICoreFactory
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
 *  @see LoKi::Hybrid::ICoreFactory
 *  @date 2008-10-19 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
// unique interface identifier 
// ============================================================================
const InterfaceID& LoKi::Hybrid::ICoreFactory::interfaceID() 
{
  static const InterfaceID s_ID ( "LoKi::Hybryd::ICoreFactory" , 1 , 1 ) ;
  return s_ID ;
}
// ============================================================================
// virtual & protected destructor 
// ============================================================================
LoKi::Hybrid::ICoreFactory::~ICoreFactory(){}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
