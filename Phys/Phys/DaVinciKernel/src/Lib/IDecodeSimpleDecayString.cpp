// $Id: IDecodeSimpleDecayString.cpp,v 1.1 2008-03-31 13:00:04 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// DaVinciKernel
#include "Kernel/IDecodeSimpleDecayString.h"
// ============================================================================
/** @file
 *  Implementation file for class IDecodeSimpleDecayString
 *  @date 2008-03-31 
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /// the unique interface identifier 
  const InterfaceID IID_IDecodeSimpleDecayString 
  ( "IDecodeSimpleDecayString" , 2 , 0 ) ;
  // ==========================================================================
}
// ============================================================================
// Return the unique interface ID
// ============================================================================
const InterfaceID& IDecodeSimpleDecayString::interfaceID() 
{ return IID_IDecodeSimpleDecayString;}
// ============================================================================
// virtual and protected destructor
// ============================================================================
IDecodeSimpleDecayString::~IDecodeSimpleDecayString() {} 
// ============================================================================
// The END 
// ============================================================================
