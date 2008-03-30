// $Id: IReporter.cpp,v 1.5 2008-03-30 17:55:52 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// local
// ============================================================================
#include "LoKi/IReporter.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /** @var IID_ILoKiReporter 
   *  unique static identifier of ILoKiReporter interface 
   */
  const InterfaceID IID_ILoKiReporter ( "LoKi::IReporter" , 3 , 1 ) ;
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
const InterfaceID& LoKi::IReporter::interfaceID() { return IID_ILoKiReporter ; } 
// ============================================================================
// destructor 
// ============================================================================
LoKi::IReporter::~IReporter() {}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

