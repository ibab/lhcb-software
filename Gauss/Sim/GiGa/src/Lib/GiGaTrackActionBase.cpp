// $Id: GiGaTrackActionBase.cpp,v 1.9 2004-02-22 13:28:38 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2002/12/07 14:27:51  ibelyaev
//  see $GIGAROOT/cmt/requirements file
//
// Revision 1.7  2002/05/07 12:21:34  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
// GiGa
#include "GiGa/GiGaTrackActionBase.h"
#include "GiGa/GiGaUtil.h"

/** @file 
 *  implementation of class GiGaTrackActionBAse 
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
GiGaTrackActionBase::GiGaTrackActionBase
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : GiGaBase( type , name , parent )
{ 
  declareInterface<IGiGaTrackAction> (this); 
}
// ============================================================================

// ============================================================================
// destructor
// ============================================================================
GiGaTrackActionBase::~GiGaTrackActionBase() {}
// ============================================================================

// ============================================================================
/** initialize the track action  
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaTrackActionBase::initialize() 
{ 
  return GiGaBase::initialize() ; 
}
// ============================================================================

// ============================================================================
/** finalize the track action
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaTrackActionBase::finalize() 
{ 
return GiGaBase::finalize() ; 
}

// ============================================================================
// ============================================================================
void GiGaTrackActionBase::PreUserTrackingAction( const G4Track* /*track*/ ) {}

// ============================================================================
// ============================================================================
void GiGaTrackActionBase::PostUserTrackingAction( const G4Track* /*track*/ ) {}

// ============================================================================
// The END 
// ============================================================================










