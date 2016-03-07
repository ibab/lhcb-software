// $Id$
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
#include "GaudiKernel/ObjectContainerBase.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ToStream.h"
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
std::string LoKi::Print::Printer::toString ( const DataObject*      obj ) 
{
  if ( 0 == obj ) { return "NULL" ; }
  std::ostringstream s ;
  obj -> fillStream ( s ) ;
  return s.str() ;
}
// ============================================================================
// make printout of DataObject
// ============================================================================
std::string LoKi::Print::Printer::toString ( const ContainedObject* obj ) 
{
  if ( 0 == obj ) { return "NULL" ; }
  std::ostringstream s ;
  obj -> fillStream ( s ) ;
  return s.str() ;
}
// ============================================================================
// tesLocation for ContainedObject
// ============================================================================
std::string LoKi::Print::Printer::tesLocation ( const ContainedObject* obj ) 
{
  if ( 0 == obj    ) { return "" ; }
  const ObjectContainerBase* parent = obj->parent() ;
  if ( 0 == parent ) { return "" ; }
  return 
    tesLocation ( parent ) + "[" + Gaudi::Utils::toString ( parent->index ( obj ) ) + "]";  
}
// ============================================================================
// tesLocation for DataObject
// ============================================================================
std::string LoKi::Print::Printer::tesLocation ( const DataObject*      obj ) 
{
  if ( 0 == obj ) { return "" ; }
  const IRegistry* reg  = obj->registry() ;
  if ( 0 == reg ) { return "" ; }
  return reg->identifier() ;
}
// ========================================================================
MsgStream& LoKi::Print::Printer::print_string 
( MsgStream& s , const std::string& what ) 
{
  if ( s.isActive() ) { s.stream() << what ; }
  return s ;
}
// ============================================================================
// make printout of DataObject
// ============================================================================
std::string LoKi::Print::toString ( const DataObject*      obj ) 
{ return LoKi::Print::Printer::toString ( obj ) ; }
// ============================================================================
// make printout of DataObject
// ============================================================================
std::string LoKi::Print::toString ( const ContainedObject* obj ) 
{ return LoKi::Print::Printer::toString ( obj ) ; }
// ============================================================================
//  helper function 
// ============================================================================
MsgStream& 
LoKi::Print::print_string ( MsgStream& s , const std::string& what ) 
{
  if ( s.isActive() ) { s.stream() << what ; }
  return s ;
}
// ========================================================================


// ============================================================================
// The END
// ============================================================================


