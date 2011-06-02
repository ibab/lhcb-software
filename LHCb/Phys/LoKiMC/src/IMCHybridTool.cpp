// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IMCHybridTool.h"
// ============================================================================
/** @file 
 *
 *  Implementation file for class LoKi::IMCHybridTool
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
 *  @date 2004-06-29 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  const InterfaceID IID_IMCHybridTool ( "LoKi::IMCHybridTool" , 5 , 0 ) ;
  // ==========================================================================
}
// ============================================================================
// Return the unique interface ID
// ============================================================================
const InterfaceID& 
LoKi::IMCHybridTool::interfaceID() { return  IID_IMCHybridTool ; }
// ============================================================================
// destructor : virtual and protected 
// ============================================================================
LoKi::IMCHybridTool::~IMCHybridTool() {} 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
