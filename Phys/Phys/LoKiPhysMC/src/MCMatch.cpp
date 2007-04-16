// $Id: MCMatch.cpp,v 1.3 2007-04-16 16:16:48 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/08/29 11:40:47  ibelyaev
//  many fixed to simplify MC-match
//
// Revision 1.1.1.1  2006/03/14 19:12:21  ibelyaev
// New package : RC <---> MC links for LoKi 
// 
// ============================================================================
// Include files
// ============================================================================
// LoKiPhysMC 
// ============================================================================
#include "LoKi/MCMatchObj.h"
#include "LoKi/MCMatch.h"
// ============================================================================

// ============================================================================
/** @file
 *
 * Implementation file for class LoKi::MCMatch
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
 *  @date 2006-03-11 
 */
// ============================================================================
/// Standard constructor from the object and reporter 
// ============================================================================
LoKi::MCMatch::MCMatch 
( const LoKi::MCMatchObj* object ) 
  : LoKi::Interface<LoKi::MCMatchObj>( object )
{} ;
// ============================================================================
/// destructor 
// ============================================================================
LoKi::MCMatch::~MCMatch() {}
// ============================================================================
/// implicit conversion to the pointer 
// ============================================================================
LoKi::MCMatch::operator const LoKi::MCMatchObj* () const { return getObject() ; }   
// ============================================================================

// ============================================================================
// The END 
// ============================================================================




