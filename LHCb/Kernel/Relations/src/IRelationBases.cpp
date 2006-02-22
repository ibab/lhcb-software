// $Id: IRelationBases.cpp,v 1.4 2006-02-22 19:57:53 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/02/07 09:22:24  ibelyaev
//  update for Win32
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

namespace 
{
  const InterfaceID IID_IRelationBase ( "IRelationBase", 1, 0 );
  //  the unique identifier of interface IRelationWeightedBase
  const InterfaceID IID_IRelationWeightedBase 
  ( "IRelationWeightedBase", 1, 0 );
};

// ============================================================================
/** the unique interface ID (static)
 *  @return the unique interface identifier 
 */
// ============================================================================
const InterfaceID& IRelationBase::interfaceID() 
{ return IID_IRelationBase; } ;
// ============================================================================
// destructor 
// ============================================================================
IRelationBase::~IRelationBase(){} ;
// ============================================================================


// ============================================================================
/** the unique interface ID (static)
 *  @return the unique interface identifier 
 */
// ============================================================================
const InterfaceID& IRelationWeightedBase::interfaceID() 
{ return IID_IRelationWeightedBase; }
// ============================================================================
// destructor 
// ============================================================================
IRelationWeightedBase::~IRelationWeightedBase(){} ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

