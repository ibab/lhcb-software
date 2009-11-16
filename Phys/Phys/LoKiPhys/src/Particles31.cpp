// $Id: Particles31.cpp,v 1.1 2009-11-16 11:54:27 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Particles31.h"
// ============================================================================
/* @file
 *  
 *  Implementation file for class : Particles31
 *
 *  @date 2009-11-16 
 *  @author Vanya Belyaev
 */
// ============================================================================
// constructor from "array"-predicate 
// ============================================================================
LoKi::Particles::PCutA::PCutA
( const LoKi::ATypes::ACuts& cut ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut ( cut ) 
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::PCutA::~PCutA(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::PCutA*
LoKi::Particles::PCutA::clone() const 
{ return new LoKi::Particles::PCutA(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::PCutA::result_type
LoKi::Particles::PCutA::operator() 
  ( LoKi::Particles::PCutA::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return false" );
    return false ;
  }
  LHCb::Particle::ConstVector d = p->daughtersVector() ;
  return  m_cut ( LoKi::ATypes::Combination ( d.begin() , d.end() ) ) ;  
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::PCutA::fillStream ( std::ostream& s ) const 
{ return s << " PCUTA( " << m_cut << " )" ; }
// ============================================================================
// constructor from "array"-predicate 
// ============================================================================
LoKi::Particles::PFunA::PFunA
( const LoKi::ATypes::AFunc& fun ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( fun ) 
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::PFunA::~PFunA(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::PFunA*
LoKi::Particles::PFunA::clone() const 
{ return new LoKi::Particles::PFunA(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::PFunA::result_type
LoKi::Particles::PFunA::operator() 
  ( LoKi::Particles::PFunA::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return NegativeInfinity" );
    return LoKi::Constants::NegativeInfinity ;
  }
  LHCb::Particle::ConstVector d = p->daughtersVector() ;
  return  m_fun ( LoKi::ATypes::Combination ( d.begin() , d.end() ) ) ;  
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::PFunA::fillStream ( std::ostream& s ) const 
{ return s << " PFUNA( " << m_fun << " )" ; }
// ============================================================================
// constructor from "array"-predicate 
// ============================================================================
LoKi::Particles::PCutV::PCutV
( const LoKi::Types::CutVals& cut ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut ( cut ) 
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::PCutV::~PCutV(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::PCutV*
LoKi::Particles::PCutV::clone() const 
{ return new LoKi::Particles::PCutV(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::PCutV::result_type
LoKi::Particles::PCutV::operator() 
  ( LoKi::Particles::PCutV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return false" );
    return false ;
  }
  return  m_cut ( p->daughtersVector() ) ;  
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::PCutV::fillStream ( std::ostream& s ) const 
{ return s << " PCUTV( " << m_cut << " )" ; }
// ============================================================================
// constructor from "array"-predicate 
// ============================================================================
LoKi::Particles::PFunV::PFunV
( const LoKi::Types::FunVals& fun ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( fun ) 
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::PFunV::~PFunV(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::PFunV*
LoKi::Particles::PFunV::clone() const 
{ return new LoKi::Particles::PFunV(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::PFunV::result_type
LoKi::Particles::PFunV::operator() 
  ( LoKi::Particles::PFunV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return NegativeInfinity" );
    return LoKi::Constants::NegativeInfinity ;
  }
  return  m_fun ( p->daughtersVector() ) ;  
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::PFunV::fillStream ( std::ostream& s ) const 
{ return s << " PFUNV( " << m_fun << " )" ; }
// ============================================================================  


// ============================================================================
// The END 
// ============================================================================
