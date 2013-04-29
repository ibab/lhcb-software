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
#include "LoKi/IGenHybridFactory.h"
// ============================================================================
/** @file 
 *
 *  Implementation file for class LoKi::IGenHybridFactory
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
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace 
{
  const InterfaceID IID_IGenHybridFactory( "LoKi::IGenHybridFactory" , 4 , 0 ) ;
}
// ============================================================================
// Return the unique interface ID
// ============================================================================
const InterfaceID& 
LoKi::IGenHybridFactory::interfaceID() { return  IID_IGenHybridFactory ; }
// ============================================================================
// destructor : virtual and protected 
// ============================================================================
LoKi::IGenHybridFactory::~IGenHybridFactory () {}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
