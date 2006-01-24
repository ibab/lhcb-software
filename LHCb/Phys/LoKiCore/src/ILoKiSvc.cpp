// $Id: ILoKiSvc.cpp,v 1.1.1.1 2006-01-24 09:39:41 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// include files 
// ============================================================================
#include "LoKi/ILoKiSvc.h"
// ============================================================================

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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot at al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

// ============================================================================
/** @var IID_ILoKiSvc 
 *  unique static identifier of ILoKiSvc interface 
 */
// ============================================================================
namespace 
{ const InterfaceID IID_ILoKiSvc ( "LoKi::ILoKiSvc" , 1 , 1 ) ; };
// ============================================================================

// ============================================================================
/** Retrieve interface ID
 *  mandatory method from IInterface 
 *  @see IInterface 
 *  @see InterfaceID 
 *  @return unique interface identifier 
 */
// ============================================================================
const InterfaceID& LoKi::ILoKiSvc::interfaceID() { return IID_ILoKiSvc ; }
// ============================================================================

// ============================================================================
// destructor
// ============================================================================
LoKi::ILoKiSvc::~ILoKiSvc() {}; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
