// $Id: Particles5.cpp,v 1.9 2009-05-09 19:24:13 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
#include <algorithm>
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Algs.h"
#include "LoKi/PhysAlgs.h"
#include "LoKi/Particles5.h"
#include "LoKi/Child.h"
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
/*  standard constructor 
 *  @param cut cut to be applied 
 */
// ============================================================================
LoKi::Particles::InTree::InTree 
( const LoKi::PhysTypes::Cuts& cut ) 
  : LoKi::PhysTypes::Cuts() 
  , m_cut ( cut ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Particles::InTree::InTree 
( const LoKi::Particles::InTree& right ) 
  : LoKi::AuxFunBase      ( right ) 
  , LoKi::PhysTypes::Cuts ( right ) 
  , m_cut                 ( right.m_cut ) 
{}
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::InTree::result_type 
LoKi::Particles::InTree::operator() 
  ( LoKi::Particles::InTree::argument p ) const 
{ return LoKi::PhysAlgs::found( p , m_cut ) ; }
// ============================================================================
//  OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::InTree::fillStream( std::ostream& s ) const 
{ return s << "INTREE(" << m_cut << ")" ; }
// ============================================================================


// ============================================================================
/*  standard constructor 
 *  @param cut cut to be applied 
 */
// ============================================================================
LoKi::Particles::NinTree::NinTree 
( const LoKi::PhysTypes::Cuts& cut ) 
  : LoKi::PhysTypes::Func () 
  , m_cut ( cut ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Particles::NinTree::NinTree 
( const LoKi::Particles::NinTree& right ) 
  : LoKi::AuxFunBase      ( right ) 
  , LoKi::PhysTypes::Func ( right ) 
  , m_cut                 ( right.m_cut ) 
{}
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::NinTree::result_type 
LoKi::Particles::NinTree::operator() 
  ( LoKi::Particles::NinTree::argument p ) const 
{ return LoKi::PhysAlgs::count_if( p , m_cut ) ; }
// ============================================================================
//  OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::NinTree::fillStream( std::ostream& s ) const 
{ return s << "NINTREE(" << m_cut << ")" ; }
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
{}
// ============================================================================
LoKi::Particles::SumTree::SumTree
( const LoKi::PhysTypes::Cuts& cut ,
  const LoKi::PhysTypes::Func& fun ,
  const double                 res ) 
  : LoKi::PhysTypes::Func() 
  , m_fun ( fun ) 
  , m_cut ( cut )
  , m_res ( res ) 
{}
// ============================================================================
LoKi::Particles::SumTree::SumTree
( const LoKi::Particles::SumTree& right ) 
  : LoKi::AuxFunBase      ( right ) 
  , LoKi::PhysTypes::Func ( right ) 
  , m_fun ( right.m_fun ) 
  , m_cut ( right.m_cut )
  , m_res ( right.m_res ) 
{}
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::SumTree::result_type 
LoKi::Particles::SumTree::operator() 
  ( LoKi::Particles::SumTree::argument p ) const 
{ 
  return  LoKi::PhysAlgs::accumulate
    ( p , m_fun , m_cut , m_res , std::plus<result_type>() ) ;
} 
// ============================================================================
//  OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::SumTree::fillStream( std::ostream& s ) const 
{
  s << "SUMTREE(" << m_fun << "," << m_cut ;
  if ( 0 != m_res ) { s << "," << m_res ; }
  return s << ")" ;
}
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
{}
// ============================================================================
LoKi::Particles::MultTree::MultTree
( const LoKi::PhysTypes::Cuts& cut ,
  const LoKi::PhysTypes::Func& fun ,
  const double                 res ) 
  : LoKi::PhysTypes::Func() 
  , m_fun ( fun ) 
  , m_cut ( cut )
  , m_res ( res ) 
{}
// ============================================================================
LoKi::Particles::MultTree::MultTree
( const LoKi::Particles::MultTree& right ) 
  : LoKi::AuxFunBase      ( right ) 
  , LoKi::PhysTypes::Func ( right ) 
  , m_fun ( right.m_fun ) 
  , m_cut ( right.m_cut )
  , m_res ( right.m_res ) 
{}
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MultTree::result_type 
LoKi::Particles::MultTree::operator() 
  ( LoKi::Particles::MultTree::argument p ) const 
{ return LoKi::PhysAlgs::accumulate
    ( p , m_fun , m_cut , m_res , std::multiplies<result_type>() ) ; };
// ============================================================================
//  OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::MultTree::fillStream( std::ostream& s ) const 
{ 
  s << "MULTTREE(" << m_fun << "," << m_cut ;
  if ( 1.0 != m_res ) { s << ","   << m_res ; }
  return s << ")" ;
}
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
{}
// ============================================================================
LoKi::Particles::MinTree::MinTree
( const LoKi::PhysTypes::Cuts& cut ,
  const LoKi::PhysTypes::Func& fun ,
  const double                 res ) 
  : LoKi::PhysTypes::Func() 
  , m_fun ( fun ) 
  , m_cut ( cut )
  , m_res ( res ) 
{}
// ============================================================================
LoKi::Particles::MinTree::MinTree
( const LoKi::Particles::MinTree& right ) 
  : LoKi::AuxFunBase      ( right ) 
  , LoKi::PhysTypes::Func ( right ) 
  , m_fun ( right.m_fun ) 
  , m_cut ( right.m_cut )
  , m_res ( right.m_res ) 
{}
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MinTree::result_type 
LoKi::Particles::MinTree::operator() 
  ( LoKi::Particles::MinTree::argument p ) const 
{ return LoKi::PhysAlgs::min_value ( p , m_fun , m_cut , m_res ) ; }
// ============================================================================
//  OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::MinTree::fillStream( std::ostream& s ) const 
{ 
  s << "MINTREE(" << m_fun << "," << m_cut ; 
  if ( LoKi::Constants::PositiveInfinity != m_res ) { s << "," << m_res ; }
  return s << ")" ;
}
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
{}
// ============================================================================
LoKi::Particles::MaxTree::MaxTree
( const LoKi::PhysTypes::Cuts& cut ,
  const LoKi::PhysTypes::Func& fun ,
  const double                 res ) 
  : LoKi::PhysTypes::Func() 
  , m_fun ( fun ) 
  , m_cut ( cut )
  , m_res ( res ) 
{}
// ============================================================================
LoKi::Particles::MaxTree::MaxTree
( const LoKi::Particles::MaxTree& right ) 
  : LoKi::AuxFunBase      ( right ) 
  , LoKi::PhysTypes::Func ( right ) 
  , m_fun ( right.m_fun ) 
  , m_cut ( right.m_cut )
  , m_res ( right.m_res ) 
{}
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MaxTree::result_type 
LoKi::Particles::MaxTree::operator() 
  ( LoKi::Particles::MaxTree::argument p ) const 
{ return LoKi::PhysAlgs::max_value ( p , m_fun , m_cut , m_res ) ; }
// ============================================================================
//  OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::MaxTree::fillStream( std::ostream& s ) const 
{ 
  s << "MAXTREE(" << m_fun << "," << m_cut ;
  if ( LoKi::Constants::NegativeInfinity != m_res ) { s << "," << m_res ; }
  return s << ")" ;
}
// ============================================================================

// ============================================================================
// the constructor from the predicate and the generation level
// ============================================================================
LoKi::Particles::InGeneration::InGeneration 
( const LoKi::PhysTypes::Cuts& cut   , 
  const unsigned int           level ) 
  : LoKi::PhysTypes::Cuts() 
  , m_cut   ( cut   ) 
  , m_level ( level ) 
{}
// ============================================================================
// the constructor from the generation level and the prediate 
// ============================================================================
LoKi::Particles::InGeneration::InGeneration 
( const unsigned int           level , 
  const LoKi::PhysTypes::Cuts& cut   ) 
  : LoKi::PhysTypes::Cuts() 
  , m_cut   ( cut   ) 
  , m_level ( level ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor") ;
// ============================================================================
LoKi::Particles::InGeneration*
LoKi::Particles::InGeneration::clone() const 
{ return new LoKi::Particles::InGeneration(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::InGeneration::result_type 
LoKi::Particles::InGeneration::operator() 
  ( LoKi::Particles::InGeneration::argument p ) const 
{
  if ( 0 == p ) { Warning ("LHCb::Particle* points to NULL" ) ; }
  if ( 0 == m_level ) { return m_cut.fun ( p ) ; }              // RETURN 
  // treat the first level explicitly (for efficiency reasons):
  if ( 1 == m_level )
  {
    const SmartRefVector<LHCb::Particle>& daugs = p->daughters() ;
    return daugs.end() != LoKi::Algs::find_if  
      ( daugs.begin() , daugs.end() , m_cut.func() ) ;           // RETURN 
  }
  // get all daughters at the given level:
  LHCb::Particle::ConstVector daughters ;
  if ( 0 == LoKi::Child::children ( p , m_level , daughters ) ) { return false ; }
  //
  return daughters.end() != LoKi::Algs::find_if  
    ( daughters.begin() , daughters.end() , m_cut.func() ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::InGeneration::fillStream( std::ostream& s ) const 
{ return s << "INGENERATION(" << m_cut << "," << m_level << ")" ; }

// ============================================================================
// the constructor from the predicate and the generation level
// ============================================================================
LoKi::Particles::NinGeneration::NinGeneration 
( const LoKi::PhysTypes::Cuts& cut   , 
  const unsigned int           level ) 
  : LoKi::PhysTypes::Func () 
  , m_cut   ( cut   ) 
  , m_level ( level ) 
{}
// ============================================================================
// the constructor from the generation level and the prediate 
// ============================================================================
LoKi::Particles::NinGeneration::NinGeneration 
( const unsigned int           level , 
  const LoKi::PhysTypes::Cuts& cut   ) 
  : LoKi::PhysTypes::Func () 
  , m_cut   ( cut   ) 
  , m_level ( level ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor") ;
// ============================================================================
LoKi::Particles::NinGeneration*
LoKi::Particles::NinGeneration::clone() const 
{ return new LoKi::Particles::NinGeneration(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::NinGeneration::result_type 
LoKi::Particles::NinGeneration::operator() 
  ( LoKi::Particles::InGeneration::argument p ) const 
{
  if ( 0 == p ) { Warning ("LHCb::Particle* points to NULL" ) ; }
  if ( 0 == m_level ) { return m_cut.fun ( p ) ? 1 : 0 ; }           // RETURN
  // treat the first level explicitly for efficiency reasons:
  if ( 1 == m_level )
  {
    const SmartRefVector<LHCb::Particle>& daugs = p->daughters() ;
    return LoKi::Algs::count_if  
      ( daugs.begin() , daugs.end() , m_cut.func() ) ;                // RETURN 
  }
  // get all daughters at the given level:
  LHCb::Particle::ConstVector daughters ;
  if ( 0 == LoKi::Child::children ( p , m_level , daughters ) ) { return 0 ; }
  //
  return LoKi::Algs::count_if  
    ( daughters.begin() , daughters.end() , m_cut.func() ) ;          // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::NinGeneration::fillStream( std::ostream& s ) const 
{ return s << "NINGENERATION(" << m_cut << "," << m_level << ")" ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
