// $Id: IRelationBases.cpp,v 1.2 2004-01-15 11:03:12 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2004/01/14 15:13:04  ibelyaev
//  few tricks to make POOL/ROOT happy
// 
// ============================================================================
// local
// ============================================================================
#include "Relations/IRelationBase.h"
#include "Relations/IRelationWeightedBase.h"

/** @file 
 *  Implementation file for class IRelationBase and class IRelationWeighedBase
 * 
 *  @author Vanya  BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2004-01-14 
 */

// ============================================================================
/** the unique interface ID (static)
 *  @return the unique interface identifier 
 */
// ============================================================================
const InterfaceID& IRelationBase::interfaceID() 
{ return IID_IRelationBase; } ;
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
IRelationWeightedBase::~IRelationWeightedBase(){}
// ============================================================================

// ============================================================================
/** the unique interface ID (static)
 *  @return the unique interface identifier 
 */
// ============================================================================
const InterfaceID& IRelationWeightedBase::interfaceID() 
{ return IID_IRelationWeightedBase; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

