// $Id: IReporter.cpp,v 1.2 2006-02-18 18:06:04 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2006/01/24 09:39:41  ibelyaev
// New Import: the core part of restructurized LoKi project
//
// ============================================================================
// Include files
// ============================================================================
// local
// ============================================================================
#include "LoKi/IReporter.h"
// ============================================================================


// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::IReporter
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
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================


// ============================================================================
/** @var IID_ILoKiReporter 
 *  unique static identifier of ILoKiReporter interface 
 */
// ============================================================================
namespace 
{
  const InterfaceID IID_ILoKiReporter ( "LoKi::IReporter" , 2 , 1 ) ;
} ;
// ============================================================================

// ============================================================================
/** Retrieve interface ID
 *  mandatory method from IInterface 
 *  @see IInterface 
 *  @see InterfaceID 
 *  @return unique interface identifier 
 */
// ============================================================================
const InterfaceID& LoKi::IReporter::interfaceID() 
{ return IID_ILoKiReporter ; } ;
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
LoKi::IReporter::~IReporter() {};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

