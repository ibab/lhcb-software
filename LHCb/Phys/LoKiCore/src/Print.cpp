// $Id: Print.cpp,v 1.6 2008-02-11 09:59:13 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <sstream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ContainedObject.h"
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Print.h"
// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace LoKi::Print 
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
 *  @date 2006-02-16 
 */
// ============================================================================
// make printout of DataObject
// ============================================================================
std::string LoKi::Print::toString ( const DataObject*      obj ) 
{
  if ( 0 == obj ) { return "NULL" ; }
  std::ostringstream s ;
  obj->fillStream ( s ) ;
  return s.str() ;
}
// ============================================================================
// make printout of DataObject
// ============================================================================
std::string LoKi::Print::toString ( const ContainedObject* obj ) 
{
  if ( 0 == obj ) { return "NULL" ; }
  std::ostringstream s ;
  obj -> fillStream ( s ) ;
  return s.str() ;
}
// ============================================================================


// ============================================================================
// The END
// ============================================================================


