// $Id: MCFinder.cpp,v 1.2 2006-03-18 12:39:49 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/03/14 19:04:29  ibelyaev
//  rename LoKi.cpp -> LoKiMC.cpp
// 
// ============================================================================
// Include files 
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// LoKiMC 
// ============================================================================
#include "LoKi/MCFinderObj.h"
#include "LoKi/MCFinder.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::MCFinder
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-11 
 */
// ============================================================================


// ============================================================================
/// Standard constructor
// ============================================================================
LoKi::MCFinder::MCFinder 
( const LoKi::MCFinderObj* obj ) 
  : LoKi::Interface<LoKi::MCFinderObj>( obj ) 
{};
// ============================================================================
/// destructor 
// ============================================================================
LoKi::MCFinder::~MCFinder(){} ;
// ============================================================================
/// implicit conversion to the pointer 
// ============================================================================
LoKi::MCFinder::operator const LoKi::MCFinderObj* () const
{ return getObject() ; }
// ============================================================================


// ============================================================================
// The END
// ============================================================================

