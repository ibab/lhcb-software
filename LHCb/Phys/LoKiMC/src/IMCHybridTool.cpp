// $Id: IMCHybridTool.cpp,v 1.2 2008-02-19 15:04:01 ibelyaev Exp $
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
 *  @date 2004-06-29 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
namespace 
{
  const InterfaceID IID_IMCHybridTool ( "LoKi::IMCHybridTool" , 3 , 0 ) ;
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
