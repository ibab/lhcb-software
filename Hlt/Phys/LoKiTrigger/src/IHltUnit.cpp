// $Id: IHltUnit.cpp,v 1.1 2008-11-10 17:31:53 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IHltUnit.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::IHltUnit
 *  @date 2008-11-10 
 *  @author Vanya BELYAEV Ivan.BELYAEV@nikhef.nl
 */
// ============================================================================
// the unique interface identifier 
// ============================================================================
const InterfaceID& LoKi::IHltUnit::interfaceID() 
{
  // the unique interafcx eidentifier 
  static const InterfaceID s_IID ( "LoKi::IHltUnit" , 1 , 0 ) ;
  // 
  return s_IID ;
}
// ============================================================================
// virtual & protected destructor 
// ============================================================================
LoKi::IHltUnit::~IHltUnit(){}    
// ============================================================================
// The END
// ============================================================================
