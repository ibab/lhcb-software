// $Id: Particles6.cpp,v 1.3 2007-04-16 16:16:27 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $ Revision:$
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/10/27 13:39:33  ibelyaev
//  fix for SLC4 platform
//
// Revision 1.1  2006/02/22 20:53:47  ibelyaev
//  add a lot of new functions (without fillStream)
//
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
#include <algorithm>
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Child.h"
#include "LoKi/Particles6.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace  LoKi::Particles
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
 *  @date 2006-02-21
 */
// ============================================================================



// ============================================================================
/** constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const LoKi::PhysTypes::Func& fun   , 
  const size_t                 index ) 
  : LoKi::Function<const LHCb::Particle*> () 
  , m_fun   ( fun   ) 
  , m_index ( index ) 
{} ;
// ============================================================================
/** constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const size_t                 index ,
  const LoKi::PhysTypes::Func& fun   ) 
  : LoKi::Function<const LHCb::Particle*> () 
  , m_fun   ( fun   ) 
  , m_index ( index ) 
{} ;
// ============================================================================
/// copy 
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const LoKi::Particles::ChildFunction& right ) 
  : LoKi::AuxFunBase                      ( right ) 
  , LoKi::Function<const LHCb::Particle*> ( right ) 
  , m_fun   ( right.m_fun   ) 
  , m_index ( right.m_index ) 
{} ;
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ChildFunction::result_type 
LoKi::Particles::ChildFunction::operator()  
  ( LoKi::Particles::ChildFunction::argument p ) const
{ return m_fun( LoKi::Child::child( p , m_index ) ) ; }
// ============================================================================
/// OPTIONAL:  specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::ChildFunction::fillStream( std::ostream& s ) const 
{ return s << "CHILD[" << m_fun << "," << m_index << "]" ; }
// ============================================================================




// ============================================================================
/** constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate 
( const LoKi::PhysTypes::Cuts& cut   , 
  const size_t                 index ) 
  : LoKi::Predicate<const LHCb::Particle*> () 
  , m_cut   ( cut   ) 
  , m_index ( index ) 
{} ;
// ============================================================================
/** constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate 
( const size_t                 index ,
  const LoKi::PhysTypes::Cuts& cut   ) 
  : LoKi::Predicate<const LHCb::Particle*> () 
  , m_cut   ( cut   ) 
  , m_index ( index ) 
{} ;
// ============================================================================
/// copy 
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate
( const LoKi::Particles::ChildPredicate& right ) 
  : LoKi::AuxFunBase                       ( right ) 
  , LoKi::Predicate<const LHCb::Particle*> ( right ) 
  , m_cut   ( right.m_cut   ) 
  , m_index ( right.m_index ) 
{} ;
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ChildPredicate::result_type 
LoKi::Particles::ChildPredicate::operator()  
  ( LoKi::Particles::ChildPredicate::argument p ) const
{ return m_cut( LoKi::Child::child( p , m_index ) ) ; }
// ============================================================================
/// OPTIONAL:  specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::ChildPredicate::fillStream( std::ostream& s ) const 
{ return s << "CHILDCUT[" << m_cut << "," << m_index << "]" ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
