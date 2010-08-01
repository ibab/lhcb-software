// $Id: ITrHybridFactory.cpp,v 1.2 2007-12-11 18:56:21 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL
// ============================================================================
#include <set>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ITrHybridFactory.h"
// ============================================================================
/** @file 
 *
 *  Implementation file for class LoKi::ITrHybridFactory
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
  const InterfaceID IID_ITrHybridFactory( "LoKi::ITrHybridFactory" , 2 , 0 ) ;
}
// ============================================================================
// Return the unique interface ID
// ============================================================================
const InterfaceID& 
LoKi::ITrHybridFactory::interfaceID() { return  IID_ITrHybridFactory ; }
// ============================================================================
// destructor : virtual and protected 
// ============================================================================
LoKi::ITrHybridFactory::~ITrHybridFactory () {}; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
