// $Id: MCFinder.cpp,v 1.5 2007-07-23 17:27:31 ibelyaev Exp $
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-11 
 */
// ============================================================================
//  Standard constructor
// ============================================================================
LoKi::MCFinder::MCFinder 
( const LoKi::MCFinderObj* obj ) 
  : LoKi::Interface<LoKi::MCFinderObj>( obj ) 
{}
// ============================================================================
//  destructor 
// ============================================================================
LoKi::MCFinder::~MCFinder(){} 
// ============================================================================
//  implicit conversion to the pointer 
// ============================================================================
LoKi::MCFinder::operator const LoKi::MCFinderObj* () const
{ return getObject() ; }
// ============================================================================


// ============================================================================
// The END
// ============================================================================

