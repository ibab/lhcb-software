// $Id$
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
#include "LoKi/Constants.h"
#include "LoKi/iTree.h"
#include "LoKi/DecayFinder.h"
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
  const unsigned int           index ) 
  : LoKi::AuxFunBase ( std::tie ( fun , index ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun   ) 
  , m_child ( index )  
{} 
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const LoKi::PhysTypes::Func& fun    , 
  const unsigned int           index1 ,
  const unsigned int           index2 )
  : LoKi::AuxFunBase ( std::tie ( fun , index1 , index2 ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun             ) 
  , m_child ( index1 , index2 ) 
{} 
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const LoKi::PhysTypes::Func& fun    , 
  const unsigned int           index1 ,
  const unsigned int           index2 ,
  const unsigned int           index3 )
  : LoKi::AuxFunBase ( std::tie ( fun , index1 , index2 , index3  ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun   ) 
  , m_child ( index1 , index2 , index3 ) 
{} 
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const LoKi::PhysTypes::Func& fun    , 
  const unsigned int           index1 ,
  const unsigned int           index2 ,
  const unsigned int           index3 ,
  const unsigned int           index4 )
  : LoKi::AuxFunBase ( std::tie ( fun , index1 , index2 , index3 , index4 ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun   ) 
  , m_child ( index1 , index2 , index3 , index4 ) 
{} 
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const LoKi::PhysTypes::Func&     fun     , 
  const std::vector<unsigned int>& indices )
  : LoKi::AuxFunBase ( std::tie ( fun , indices ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun   ) 
  , m_child ( indices ) 
{
  Assert ( m_child.valid() , "Child selector is invalid!" ) ;
} 
// ============================================================================
/* constructor from the function and child selector 
 *  @param fun      the function to be used 
 *  @param selector the child selector 
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const LoKi::PhysTypes::Func& fun       , 
  const LoKi::Child::Selector& selector  ) 
  : LoKi::AuxFunBase ( std::tie ( fun , selector ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun      ) 
  , m_child ( selector ) 
{} 
// ============================================================================
/* constructor from the function and child selector 
 *  @param fun      the function to be used 
 *  @param selector the child selector 
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const LoKi::PhysTypes::Func& fun       , 
  const std::string&           selector  ) 
  : LoKi::AuxFunBase ( std::tie ( fun , selector ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun      ) 
  , m_child ( selector ) 
{}
// ============================================================================
/*  constructor from the function and child selector 
 *  @param fun      the function to be used 
 *  @param selector the child selector 
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const LoKi::PhysTypes::Func& fun       , 
  const Decays::IDecay::iTree& selector  )
  : LoKi::AuxFunBase ( std::tie ( fun , selector ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun      ) 
  , m_child ( selector ) 
{} 
// ============================================================================
/*  constructor from the function and child selector 
 *  @param fun      the function to be used 
 *  @param selector the child selector 
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const LoKi::PhysTypes::Func& fun       , 
  const Decays::iNode&         selector  ) 
  : LoKi::AuxFunBase ( std::tie ( fun , selector ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun      ) 
  , m_child ( selector ) 
{} 
// ============================================================================
/*  constructor from the function and child selector 
 *  @param fun      the function to be used 
 *  @param selector the child selector 
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const LoKi::PhysTypes::Func& fun       , 
  const LoKi::PhysTypes::Cuts& selector  ) 
  : LoKi::AuxFunBase ( std::tie ( fun , selector ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun      ) 
  , m_child ( selector ) 
{}
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const unsigned int           index ,
  const LoKi::PhysTypes::Func& fun   ) 
  : LoKi::AuxFunBase ( std::tie ( index , fun  ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun   ) 
  , m_child ( index ) 
{} 
// ============================================================================
/*  constructor from the function and child selector 
 *  @param selector the child selector
 *  @param fun      the function to be used  
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const LoKi::Child::Selector& selector  ,
  const LoKi::PhysTypes::Func& fun       ) 
  : LoKi::AuxFunBase ( std::tie ( selector , fun  ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun      ) 
  , m_child ( selector ) 
{}
// ============================================================================
/*  constructor from the function and child selector 
 *  @param selector the child selector
 *  @param fun      the function to be used  
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const Decays::IDecay::iTree& selector  , 
  const LoKi::PhysTypes::Func& fun       ) 
  : LoKi::AuxFunBase ( std::tie ( selector , fun  ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun      ) 
  , m_child ( selector ) 
{}
// ============================================================================
/*  constructor from the function and child selector 
 *  @param selector the child selector
 *  @param fun      the function to be used  
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const Decays::iNode&         selector  , 
  const LoKi::PhysTypes::Func& fun       ) 
  : LoKi::AuxFunBase ( std::tie ( selector , fun  ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun      ) 
  , m_child ( selector ) 
{}
// ============================================================================
/*  constructor from the function and child selector 
 *  @param selector the child selector
 *  @param fun      the function to be used  
 */
// ============================================================================
LoKi::Particles::ChildFunction::ChildFunction 
( const std::string&           selector  , 
  const LoKi::PhysTypes::Func& fun       ) 
  : LoKi::AuxFunBase ( std::tie ( selector , fun  ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun   ( fun      ) 
  , m_child ( selector ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ChildFunction::result_type 
LoKi::Particles::ChildFunction::operator()  
  ( LoKi::Particles::ChildFunction::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ("LHCb::Particle* points to NULL, return NegativeInfinity") ;
    return LoKi::Constants::NegativeInfinity ;
  }
  const LHCb::Particle* c = m_child.child ( p ) ;
  if ( 0 == c ) 
  {
    Error (" child LHCb::Particle* points to NULL, return NegativeInfinity") ;
    return LoKi::Constants::NegativeInfinity ;
  }
  //
  return m_fun ( c ) ;
}
// ============================================================================
//  OPTIONAL:  specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::ChildFunction::fillStream( std::ostream& s ) const 
{ return s << " CHILD(" << m_fun << "," << m_child << " ) " ; }
// ============================================================================

// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate 
( const LoKi::PhysTypes::Cuts& cut   , 
  const unsigned int           index ) 
  : LoKi::AuxFunBase ( std::tie ( cut , index  ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut    ) 
  , m_child ( index  ) 
{} 
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate 
( const LoKi::PhysTypes::Cuts& cut    , 
  const unsigned int           index1 ,
  const unsigned int           index2 )
  : LoKi::AuxFunBase ( std::tie ( cut , index1 , index2 ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut   ) 
  , m_child ( index1 , index2 ) 
{} 
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate 
( const LoKi::PhysTypes::Cuts& cut    , 
  const unsigned int           index1 ,
  const unsigned int           index2 ,
  const unsigned int           index3 )
  : LoKi::AuxFunBase ( std::tie ( cut , index1 , index2 , index3 ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut   ) 
  , m_child ( index1 , index2 , index3 ) 
{} 
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate 
( const LoKi::PhysTypes::Cuts& cut    , 
  const unsigned int           index1 ,
  const unsigned int           index2 ,
  const unsigned int           index3 ,
  const unsigned int           index4 )
  : LoKi::AuxFunBase ( std::tie ( cut , index1 , index2 , index3 , index4 ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut   ) 
  , m_child ( index1 , index2 , index3 , index4 ) 
{}
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param cut     the function to be used 
 *  @param indices the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate
( const LoKi::PhysTypes::Cuts&     cut     , 
  const std::vector<unsigned int>& indices ) 
  : LoKi::AuxFunBase ( std::tie ( cut , indices ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut     ) 
  , m_child ( indices ) 
{}
// ============================================================================
/*  constructor from the function and child selector 
 *  @param cut      the function to be used 
 *  @param selector the child selector 
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate
( const LoKi::PhysTypes::Cuts& cut      , 
  const LoKi::Child::Selector& selector ) 
  : LoKi::AuxFunBase ( std::tie ( cut , selector) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut      ) 
  , m_child ( selector ) 
{}
// ============================================================================
/*  constructor from the function and child selector 
 *  @param cut      the function to be used 
 *  @param selector the child selector 
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate
( const LoKi::PhysTypes::Cuts& cut      , 
  const Decays::IDecay::iTree& selector ) 
  : LoKi::AuxFunBase ( std::tie ( cut , selector) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut      ) 
  , m_child ( selector ) 
{}
// ============================================================================
/*  constructor from the function and child selector 
 *  @param cut      the function to be used 
 *  @param selector the child selector 
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate
( const LoKi::PhysTypes::Cuts& cut      , 
  const Decays::iNode&         selector ) 
  : LoKi::AuxFunBase ( std::tie ( cut , selector) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut      ) 
  , m_child ( selector ) 
{}
// ============================================================================
/*  constructor from the function and child selector 
 *  @param cut      the function to be used 
 *  @param selector the child selector 
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate
( const LoKi::PhysTypes::Cuts& cut      , 
  const std::string&           selector ) 
  : LoKi::AuxFunBase ( std::tie ( cut , selector) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut      ) 
  , m_child ( selector ) 
{}
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate 
( const unsigned int           index ,
  const LoKi::PhysTypes::Cuts& cut   ) 
  : LoKi::AuxFunBase ( std::tie ( index , cut ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut   ) 
  , m_child ( index ) 
{}
// ============================================================================
/*  constructor from the function and child selector 
 *  @param cut      the function to be used 
 *  @param selector the child selector 
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate
( const LoKi::Child::Selector& selector , 
  const LoKi::PhysTypes::Cuts& cut      ) 
  : LoKi::AuxFunBase ( std::tie ( selector , cut ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut      ) 
  , m_child ( selector ) 
{}
// ============================================================================
/*  constructor from the function and child selector 
 *  @param cut      the function to be used 
 *  @param selector the child selector 
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate
( const Decays::IDecay::iTree& selector , 
  const LoKi::PhysTypes::Cuts& cut      ) 
  : LoKi::AuxFunBase ( std::tie ( selector , cut ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut      ) 
  , m_child ( selector ) 
{}
// ============================================================================
/*  constructor from the function and child selector 
 *  @param cut      the function to be used 
 *  @param selector the child selector 
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate
( const Decays::iNode&         selector , 
  const LoKi::PhysTypes::Cuts& cut      ) 
  : LoKi::AuxFunBase ( std::tie ( selector , cut ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut      ) 
  , m_child ( selector ) 
{}
// ============================================================================
/*  constructor from the function and child selector 
 *  @param cut      the function to be used 
 *  @param selector the child selector 
 */
// ============================================================================
LoKi::Particles::ChildPredicate::ChildPredicate
( const std::string&           selector ,
  const LoKi::PhysTypes::Cuts& cut      )
  : LoKi::AuxFunBase ( std::tie ( selector , cut ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut   ( cut      ) 
  , m_child ( selector ) 
{} 
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ChildPredicate::result_type 
LoKi::Particles::ChildPredicate::operator()  
  ( LoKi::Particles::ChildPredicate::argument p ) const
{
  //
  if ( 0 == p ) 
  {
    Error ("LHCb::Particle* points to NULL, return 'false'") ;
    return false ;
  }
  //
  const LHCb::Particle* c = m_child.child ( p ) ;
  if ( 0 == c ) 
  {
    Error ("Child LHCb::Particle* points to NULL, return 'false'") ;
    return false ;
  }
  //
  return m_cut ( c ) ;
}
// ============================================================================
//  OPTIONAL:  specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::ChildPredicate::fillStream( std::ostream& s ) const 
{ return  s << " CHILDCUT( " << m_cut << " , "  << m_child << " ) " ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
