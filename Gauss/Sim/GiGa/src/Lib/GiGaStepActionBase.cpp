// $Id: GiGaStepActionBase.cpp,v 1.11 2004-02-22 13:28:38 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.10  2002/12/07 14:27:51  ibelyaev
//  see $GIGAROOT/cmt/requirements file
//
// Revision 1.9  2002/05/07 12:21:34  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
// 
// ============================================================================
// GiGa
#include "GiGa/GiGaStepActionBase.h"
#include "GiGa/GiGaUtil.h"

/** @file 
 *  implementation of class GiGaStepActionBase
 *
 *  @author Vanya Belyaev 
 */

// ============================================================================
/** standard constructor 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaStepActionBase::GiGaStepActionBase
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : GiGaBase ( type , name , parent  ) 
{ 
  declareInterface<IGiGaStepAction> (this); 
}
// ============================================================================

// ============================================================================
// destructor 
// ============================================================================
GiGaStepActionBase::~GiGaStepActionBase() {}
// ============================================================================

// ============================================================================
/** initialize the step action  
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaStepActionBase::initialize() 
{ 
return GiGaBase::initialize() ; 
} 
// ============================================================================

// ============================================================================
/** finalize the step action
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaStepActionBase::finalize() 
{ 
  return GiGaBase::finalize(); 
}
// ============================================================================

// ============================================================================
// The End 
// ============================================================================

