// $Id: IHybridTool.cpp,v 1.3 2009-11-17 12:41:41 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
// ============================================================================
// LoKiHybrid 
// ============================================================================
#include "LoKi/IHybridTool.h"
// ============================================================================
/** @file 
 *
 *  Implementation file for class LoKi::IHybridTools
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @date 2004-06-29 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
namespace 
{
  const InterfaceID IID_IHybridTool ( "LoKi::IHybridTool" , 3 , 0 ) ;
}
// ============================================================================
// Return the unique interface ID
// ============================================================================
const InterfaceID& 
LoKi::IHybridTool::interfaceID() { return  IID_IHybridTool ; }
// ============================================================================
// destructor : virtual and protected 
// ============================================================================
LoKi::IHybridTool::~IHybridTool() {}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
