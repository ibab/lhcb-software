// $Id: Particles5.cpp,v 1.4 2007-04-16 16:16:27 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2007/03/07 09:19:52  ibelyaev
//  fix a stupud bug in LoKi::PhysAlgs::accumulate ; retag
//
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
#include "LoKi/PhysAlgs.h"
#include "LoKi/Particles5.h"
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
 *  @date 2006-02-20 
 */
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @param cut cut to be applied 
 */
// ============================================================================
LoKi::Particles::InTree::InTree 
( const LoKi::PhysTypes::Cuts& cut ) 
  : LoKi::PhysTypes::Cuts() 
  , m_cut ( cut ) 
{};
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Particles::InTree::InTree 
( const LoKi::Particles::InTree& right ) 
  : LoKi::AuxFunBase      ( right ) 
  , LoKi::PhysTypes::Cuts ( right ) 
  , m_cut                 ( right.m_cut ) 
{};
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::InTree::result_type 
LoKi::Particles::InTree::operator() 
  ( LoKi::Particles::InTree::argument p ) const 
{ return LoKi::PhysAlgs::found( p , m_cut ) ; }
// ============================================================================
/// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::InTree::fillStream( std::ostream& s ) const 
{ return s << "INTREE[" << m_cut << "]" ; }
// ============================================================================


// ============================================================================
/** standard constructor 
 *  @param cut cut to be applied 
 */
// ============================================================================
LoKi::Particles::NinTree::NinTree 
( const LoKi::PhysTypes::Cuts& cut ) 
  : LoKi::PhysTypes::Func () 
  , m_cut ( cut ) 
{};
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Particles::NinTree::NinTree 
( const LoKi::Particles::NinTree& right ) 
  : LoKi::AuxFunBase      ( right ) 
  , LoKi::PhysTypes::Func ( right ) 
  , m_cut                 ( right.m_cut ) 
{};
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::NinTree::result_type 
LoKi::Particles::NinTree::operator() 
  ( LoKi::Particles::NinTree::argument p ) const 
{ return LoKi::PhysAlgs::count_if( p , m_cut ) ; }
// ============================================================================
/// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::NinTree::fillStream( std::ostream& s ) const 
{ return s << "NINTREE[" << m_cut << "]" ; }
// ============================================================================


// ============================================================================
LoKi::Particles::SumTree::SumTree
( const LoKi::PhysTypes::Func& fun ,
  const LoKi::PhysTypes::Cuts& cut ,
  const double                 res ) 
  : LoKi::PhysTypes::Func() 
  , m_fun ( fun ) 
  , m_cut ( cut )
  , m_res ( res ) 
{};
// ============================================================================
LoKi::Particles::SumTree::SumTree
( const LoKi::PhysTypes::Cuts& cut ,
  const LoKi::PhysTypes::Func& fun ,
  const double                 res ) 
  : LoKi::PhysTypes::Func() 
  , m_fun ( fun ) 
  , m_cut ( cut )
  , m_res ( res ) 
{};
// ============================================================================
LoKi::Particles::SumTree::SumTree
( const LoKi::Particles::SumTree& right ) 
  : LoKi::AuxFunBase      ( right ) 
  , LoKi::PhysTypes::Func ( right ) 
  , m_fun ( right.m_fun ) 
  , m_cut ( right.m_cut )
  , m_res ( right.m_res ) 
{};
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::SumTree::result_type 
LoKi::Particles::SumTree::operator() 
  ( LoKi::Particles::SumTree::argument p ) const 
{ 
  return  LoKi::PhysAlgs::accumulate
    ( p , m_fun , m_cut , m_res , std::plus<result_type>() ) ;
} ;
// ============================================================================
/// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::SumTree::fillStream( std::ostream& s ) const 
{ return s << "SUMTREE[" << m_fun << "," << m_cut << "]" ; }
// ============================================================================

// ============================================================================
LoKi::Particles::MultTree::MultTree
( const LoKi::PhysTypes::Func& fun ,
  const LoKi::PhysTypes::Cuts& cut ,
  const double                 res ) 
  : LoKi::PhysTypes::Func() 
  , m_fun ( fun ) 
  , m_cut ( cut )
  , m_res ( res ) 
{};
// ============================================================================
LoKi::Particles::MultTree::MultTree
( const LoKi::PhysTypes::Cuts& cut ,
  const LoKi::PhysTypes::Func& fun ,
  const double                 res ) 
  : LoKi::PhysTypes::Func() 
  , m_fun ( fun ) 
  , m_cut ( cut )
  , m_res ( res ) 
{};
// ============================================================================
LoKi::Particles::MultTree::MultTree
( const LoKi::Particles::MultTree& right ) 
  : LoKi::AuxFunBase      ( right ) 
  , LoKi::PhysTypes::Func ( right ) 
  , m_fun ( right.m_fun ) 
  , m_cut ( right.m_cut )
  , m_res ( right.m_res ) 
{};
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MultTree::result_type 
LoKi::Particles::MultTree::operator() 
  ( LoKi::Particles::MultTree::argument p ) const 
{ return LoKi::PhysAlgs::accumulate
    ( p , m_fun , m_cut , m_res , std::multiplies<result_type>() ) ; };
// ============================================================================
/// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::MultTree::fillStream( std::ostream& s ) const 
{ return s << "MULTTREE[" << m_fun << "," << m_cut << "]" ; }
// ============================================================================


// ============================================================================
LoKi::Particles::MinTree::MinTree
( const LoKi::PhysTypes::Func& fun ,
  const LoKi::PhysTypes::Cuts& cut ,
  const double                 res ) 
  : LoKi::PhysTypes::Func() 
  , m_fun ( fun ) 
  , m_cut ( cut )
  , m_res ( res ) 
{};
// ============================================================================
LoKi::Particles::MinTree::MinTree
( const LoKi::PhysTypes::Cuts& cut ,
  const LoKi::PhysTypes::Func& fun ,
  const double                 res ) 
  : LoKi::PhysTypes::Func() 
  , m_fun ( fun ) 
  , m_cut ( cut )
  , m_res ( res ) 
{};
// ============================================================================
LoKi::Particles::MinTree::MinTree
( const LoKi::Particles::MinTree& right ) 
  : LoKi::AuxFunBase      ( right ) 
  , LoKi::PhysTypes::Func ( right ) 
  , m_fun ( right.m_fun ) 
  , m_cut ( right.m_cut )
  , m_res ( right.m_res ) 
{};
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MinTree::result_type 
LoKi::Particles::MinTree::operator() 
  ( LoKi::Particles::MinTree::argument p ) const 
{ return LoKi::PhysAlgs::min_value ( p , m_fun , m_cut , m_res ) ; };
// ============================================================================
/// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::MinTree::fillStream( std::ostream& s ) const 
{ return s << "MINTREE[" << m_fun << "," << m_cut << "]" ; }
// ============================================================================

// ============================================================================
LoKi::Particles::MaxTree::MaxTree
( const LoKi::PhysTypes::Func& fun ,
  const LoKi::PhysTypes::Cuts& cut ,
  const double                 res ) 
  : LoKi::PhysTypes::Func() 
  , m_fun ( fun ) 
  , m_cut ( cut )
  , m_res ( res ) 
{};
// ============================================================================
LoKi::Particles::MaxTree::MaxTree
( const LoKi::PhysTypes::Cuts& cut ,
  const LoKi::PhysTypes::Func& fun ,
  const double                 res ) 
  : LoKi::PhysTypes::Func() 
  , m_fun ( fun ) 
  , m_cut ( cut )
  , m_res ( res ) 
{};
// ============================================================================
LoKi::Particles::MaxTree::MaxTree
( const LoKi::Particles::MaxTree& right ) 
  : LoKi::AuxFunBase      ( right ) 
  , LoKi::PhysTypes::Func ( right ) 
  , m_fun ( right.m_fun ) 
  , m_cut ( right.m_cut )
  , m_res ( right.m_res ) 
{};
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MaxTree::result_type 
LoKi::Particles::MaxTree::operator() 
  ( LoKi::Particles::MaxTree::argument p ) const 
{ return LoKi::PhysAlgs::max_value ( p , m_fun , m_cut , m_res ) ; };
// ============================================================================
/// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::MaxTree::fillStream( std::ostream& s ) const 
{ return s << "MAXTREE[" << m_fun << "," << m_cut << "]" ; }
// ============================================================================









// ============================================================================
// The END 
// ============================================================================
