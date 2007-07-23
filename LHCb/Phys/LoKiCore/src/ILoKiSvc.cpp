// $Id: ILoKiSvc.cpp,v 1.4 2007-07-23 17:07:43 ibelyaev Exp $
// ============================================================================
// include files 
// ============================================================================
#include "LoKi/ILoKiSvc.h"
// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::ILoKiSvc
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================
namespace 
{ 
  // ==========================================================================
  /** @var IID_ILoKiSvc 
   *  unique static identifier of ILoKiSvc interface 
   */
  const InterfaceID IID_ILoKiSvc ( "LoKi::ILoKiSvc" , 2 , 1 ) ; 
  // ==========================================================================
}
// ============================================================================
/*  Retrieve interface ID
 *  mandatory method from IInterface 
 *  @see IInterface 
 *  @see InterfaceID 
 *  @return unique interface identifier 
 */
// ============================================================================
const InterfaceID& LoKi::ILoKiSvc::interfaceID() { return IID_ILoKiSvc ; }
// ============================================================================
// destructor
// ============================================================================
LoKi::ILoKiSvc::~ILoKiSvc() {}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
