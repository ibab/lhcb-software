// $Id: Particles6.cpp,v 1.6 2009-07-09 13:39:13 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Particles6.h"
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
 *  @date 2006-02-21
 */
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const LoKi::PhysTypes::Func& fun   , 
  const size_t                 index ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun   ) 
  , m_index ( 1 , index ) 
{} 
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const LoKi::PhysTypes::Func& fun    , 
  const size_t                 index1 ,
  const size_t                 index2 )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun   ) 
  , m_index () 
{
  m_index.push_back ( index1 ) ;
  m_index.push_back ( index2 ) ;  
} 
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const LoKi::PhysTypes::Func& fun    , 
  const size_t                 index1 ,
  const size_t                 index2 ,
  const size_t                 index3 )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun   ) 
  , m_index () 
{
  m_index.push_back ( index1 ) ;
  m_index.push_back ( index2 ) ;  
  m_index.push_back ( index3 ) ;  
} 
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const LoKi::PhysTypes::Func& fun    , 
  const size_t                 index1 ,
  const size_t                 index2 ,
  const size_t                 index3 ,
  const size_t                 index4 )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun   ) 
  , m_index () 
{
  m_index.push_back ( index1 ) ;
  m_index.push_back ( index2 ) ;  
  m_index.push_back ( index3 ) ;
  m_index.push_back ( index4 ) ;  
} 
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const size_t                 index ,
  const LoKi::PhysTypes::Func& fun   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun   ) 
  , m_index ( 1 , index ) 
{} 
// ============================================================================
// copy 
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const LoKi::Particles::ChildFunction& right ) 
  : LoKi::AuxFunBase                      ( right ) 
    , LoKi::BasicFunctors<const LHCb::Particle*>::Function ( right ) 
  , m_fun   ( right.m_fun   ) 
  , m_index ( right.m_index ) 
{} 
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ChildFunction::result_type 
LoKi::Particles::ChildFunction::operator()  
  ( LoKi::Particles::ChildFunction::argument p ) const
{ return m_fun( LoKi::Child::child( p , m_index ) ) ; }
// ============================================================================
//  OPTIONAL:  specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::ChildFunction::fillStream( std::ostream& s ) const 
{ 
  s << "CHILD(" << m_fun << "," ;
  //
  switch ( m_index.size() ) 
  {
  case 1 : 
    s << m_index[0] ; break ;
  case 2 : 
    s << m_index[0] << "," << m_index[1] ; break ;
  case 3 : 
    s << m_index[0] << "," << m_index[1] << "," << m_index[2] ; break ;
  case 4 : 
    s << m_index[0] << "," << m_index[1] << "," << m_index[2] << "," << m_index[3]; break ;
  default:
    Gaudi::Utils::toStream (  m_index ,s ) ;
  }
  //
  return s << ")" ;
}
// ============================================================================




// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate 
( const LoKi::PhysTypes::Cuts& cut   , 
  const size_t                 index ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut   ) 
  , m_index ( 1 , index ) 
{} 
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate 
( const LoKi::PhysTypes::Cuts& cut    , 
  const size_t                 index1 ,
  const size_t                 index2 )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut   ) 
  , m_index () 
{
  m_index.push_back ( index1 ) ;
  m_index.push_back ( index2 ) ;
} 
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate 
( const LoKi::PhysTypes::Cuts& cut    , 
  const size_t                 index1 ,
  const size_t                 index2 ,
  const size_t                 index3 )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut   ) 
  , m_index () 
{
  m_index.push_back ( index1 ) ;
  m_index.push_back ( index2 ) ;
  m_index.push_back ( index3 ) ;
} 
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate 
( const LoKi::PhysTypes::Cuts& cut    , 
  const size_t                 index1 ,
  const size_t                 index2 ,
  const size_t                 index3 ,
  const size_t                 index4 )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut   ) 
  , m_index () 
{
  m_index.push_back ( index1 ) ;
  m_index.push_back ( index2 ) ;
  m_index.push_back ( index3 ) ;
  m_index.push_back ( index4 ) ;
} 
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate 
( const size_t                 index ,
  const LoKi::PhysTypes::Cuts& cut   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut   ) 
  , m_index ( 1 , index ) 
{} 
// ============================================================================
//  copy 
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate
( const LoKi::Particles::ChildPredicate& right ) 
  : LoKi::AuxFunBase                       ( right ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate ( right ) 
  , m_cut   ( right.m_cut   ) 
  , m_index ( right.m_index ) 
{} ;
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ChildPredicate::result_type 
LoKi::Particles::ChildPredicate::operator()  
  ( LoKi::Particles::ChildPredicate::argument p ) const
{ return m_cut( LoKi::Child::child( p , m_index ) ) ; }
// ============================================================================
//  OPTIONAL:  specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::ChildPredicate::fillStream( std::ostream& s ) const 
{
  s << "CHILDCUT(" << m_cut << "," ;
  //
  switch ( m_index.size() ) 
  {
  case 1 : 
    s << m_index[0] ; break ;
  case 2 : 
    s << m_index[0] << "," << m_index[1] ; break ;
  case 3 : 
    s << m_index[0] << "," << m_index[1] << "," << m_index[2] ; break ;
  case 4 : 
    s << m_index[0] << "," << m_index[1] << "," << m_index[2] << "," << m_index[3]; break ;
  default:
    Gaudi::Utils::toStream (  m_index ,s ) ;
  }
  //
  return s << ")" ;
}
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
