// $Id: IHybridFactory.cpp,v 1.1 2007-07-26 13:25:09 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
// ============================================================================
// LoKiHybrid 
// ============================================================================
#include "LoKi/IHybridFactory.h"
// ============================================================================
/** @file 
 *
 *  Implementation file for class LoKi::IHybridFactory
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
  const InterfaceID IID_IHybridFactory( "LoKi::IHybridFactory" , 1 , 0 ) ;
}
// ============================================================================
// Return the unique interface ID
// ============================================================================
const InterfaceID& 
LoKi::IHybridFactory::interfaceID() { return  IID_IHybridFactory ; }
// ============================================================================
// destructor : virtual and protected 
// ============================================================================
LoKi::IHybridFactory::~IHybridFactory () {}; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
