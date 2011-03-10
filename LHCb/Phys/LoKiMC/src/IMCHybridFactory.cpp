// $Id: IMCHybridFactory.cpp,v 1.2 2008-02-19 15:04:01 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IMCHybridFactory.h"
// ============================================================================
/** @file 
 *
 *  Implementation file for class LoKi::IMCHybridFactory
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
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
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
  const InterfaceID IID_IMCHybridFactory( "LoKi::IMCHybridFactory" , 4 , 0 ) ;
}
// ============================================================================
// Return the unique interface ID
// ============================================================================
const InterfaceID& 
LoKi::IMCHybridFactory::interfaceID() { return  IID_IMCHybridFactory ; }
// ============================================================================
// destructor : virtual and protected 
// ============================================================================
LoKi::IMCHybridFactory::~IMCHybridFactory () {}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
