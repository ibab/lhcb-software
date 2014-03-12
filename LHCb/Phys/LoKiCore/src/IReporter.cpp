// $Id$
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
  const InterfaceID IID_ILoKiReporter ( "LoKi::IReporter" , 4 , 1 ) ;
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
// define maximal number of prints 
// ============================================================================
void LoKi::IReporter::setMaxErrorPrint   ( const std::size_t v )// max prints 
{ s_maxErrorPrint    = v ; }
// ============================================================================
// define maximal number of warning prints 
// ============================================================================
void LoKi::IReporter::setMaxWarningPrint ( const std::size_t v )// max prints
{ s_maxWarningPrint  = v ; }
// ============================================================================
// maximal number of error   prints 
// ============================================================================
std::size_t LoKi::IReporter::maxErrorPrint   () 
{ return s_maxErrorPrint   ; } 
// ============================================================================
// maximal number of warning prints 
// ============================================================================
std::size_t LoKi::IReporter::maxWarningPrint () 
{ return s_maxWarningPrint ; }
// ============================================================================
/// define maximal number of prints 
std::size_t LoKi::IReporter::s_maxErrorPrint   = 2 ;
/// define maximal number of prints 
std::size_t LoKi::IReporter::s_maxWarningPrint = 2 ;
// ============================================================================
// The END 
// ============================================================================

