// $Id: Loop.cpp,v 1.2 2007-04-16 16:16:40 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/04/09 08:51:50  ibelyaev
//  update for LoKiAlgo
//
// ============================================================================
// Include files 
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Interface.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include  "LoKi/LoopObj.h"
#include  "LoKi/Loop.h"
#include  "LoKi/Algo.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::Loop
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
 *  @date 2006-03-14 
 */
// ============================================================================



// ============================================================================
LoKi::Loop::Loop 
( const LoKi::LoopObj* obj ) 
  : LoKi::Interface<LoKi::LoopObj>( obj )
{}
// ============================================================================
LoKi::Loop::Loop 
( const LoKi::Loop& obj ) 
  : LoKi::Interface<LoKi::LoopObj>( obj )
{};
// ============================================================================
LoKi::Loop::~Loop() {};
// ============================================================================
const LHCb::Particle* LoKi::Loop::particle() const 
{
  if ( !validPointer() ){ return 0 ;}
  return *getObject() ;
} ;
// ============================================================================
const LHCb::Vertex*  LoKi::Loop::vertex () const 
{
  if ( !validPointer() ){ return 0 ;}
  return *getObject() ;
} ;
// ============================================================================
const LHCb::Particle* LoKi::Loop::child ( const size_t index ) const 
{
  if ( !validPointer() ){ return 0 ;}
  return getObject()->child ( index ) ;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================

