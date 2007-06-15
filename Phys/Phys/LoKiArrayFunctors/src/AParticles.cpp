// $Id: AParticles.cpp,v 1.1.1.1 2007-06-15 08:04:18 ibelyaev Exp $
// ============================================================================
// Include files 
// ===========================================================================
// GaudiKernel
// ===========================================================================
#include "GaudiKernel/ToStream.h"
// ===========================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Algs.h"
#include "LoKi/Kinematics.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/AParticles.h"
#include "LoKi/AKinematics.h"
// ============================================================================
// Contructor from the critearia:
// ============================================================================
LoKi::AParticles::Count::Count 
( const LoKi::PhysTypes::Cuts& cut ) 
  : LoKi::Function<LoKi::ATypes::Combination> () 
  , m_cut (cut ) 
{}
// ============================================================================
// copy contructor 
// ============================================================================
LoKi::AParticles::Count::Count 
( const LoKi::AParticles::Count& right ) 
  : LoKi::AuxFunBase ( right ) 
  , LoKi::Function<LoKi::ATypes::Combination> ( right ) 
  , m_cut ( right.m_cut ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::Count::result_type  
LoKi::AParticles::Count::operator() 
  ( LoKi::AParticles::Count::argument v ) const
{ 
  return LoKi::Algs::count_if ( v.begin() , v.end() , m_cut ) ;
}      
// ============================================================================
// Contructor from the tool 
// ============================================================================
LoKi::AParticles::Unique::Unique 
( const ICheckOverlap*                   tool ) 
  : LoKi::Predicate<LoKi::ATypes::Combination> () 
  , m_tool ( tool ) 
{}
// ============================================================================
// Contructor from the tool 
// ============================================================================
LoKi::AParticles::Unique::Unique 
( const LoKi::Interface<ICheckOverlap>& tool ) 
  : LoKi::Predicate<LoKi::ATypes::Combination> () 
  , m_tool ( tool ) 
{}
// ============================================================================
// copy contructor 
// ============================================================================
LoKi::AParticles::Unique::Unique 
( const LoKi::AParticles::Unique& right ) 
  : AuxFunBase ( right ) 
  , LoKi::Predicate<LoKi::ATypes::Combination> ( right ) 
  , m_tool ( right.m_tool ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::Unique::result_type  
LoKi::AParticles::Unique::operator() 
  ( LoKi::AParticles::Unique::argument v ) const 
{
  Assert ( m_tool.validPointer() , "Invalid pouinter to ICheckOverlap tool" ) ;
  // use the tool
  return  !m_tool->foundOverlap ( v ) ;
}
// ============================================================================
// constructor from one index 
// ============================================================================
LoKi::AParticles::FourMomentum::FourMomentum
( const std::size_t index )
  : LoKi::Function<LoKi::ATypes::Combination> () 
    , m_indices ( 1 , index ) 
{}
// ============================================================================
// constructor from two indices 
// ============================================================================
LoKi::AParticles::FourMomentum::FourMomentum
( const std::size_t index1 , 
  const std::size_t index2 )
  : LoKi::Function<LoKi::ATypes::Combination> () 
  , m_indices () 
{
  m_indices.push_back ( index1 ) ;
  m_indices.push_back ( index2 ) ;  
}
// ============================================================================
// constructor from three indices 
// ============================================================================
LoKi::AParticles::FourMomentum::FourMomentum
( const std::size_t index1 , 
  const std::size_t index2 ,
  const std::size_t index3 )
  : LoKi::Function<LoKi::ATypes::Combination> () 
  , m_indices () 
{
  m_indices.push_back ( index1 ) ;
  m_indices.push_back ( index2 ) ;  
  m_indices.push_back ( index3 ) ;  
}
// ============================================================================
// constructor from four indices 
// ============================================================================
LoKi::AParticles::FourMomentum::FourMomentum
( const std::size_t index1 , 
  const std::size_t index2 ,
  const std::size_t index3 ,
  const std::size_t index4 )
  : LoKi::Function<LoKi::ATypes::Combination> () 
  , m_indices () 
{
  m_indices.push_back ( index1 ) ;
  m_indices.push_back ( index2 ) ;  
  m_indices.push_back ( index3 ) ;  
  m_indices.push_back ( index4 ) ;  
}
// ============================================================================
// contructor from list of indices 
// ============================================================================
LoKi::AParticles::FourMomentum::FourMomentum
( const LoKi::AParticles::FourMomentum::Indices& indices ) 
  : LoKi::Function<LoKi::ATypes::Combination> () 
  , m_indices ( indices ) 
{}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::AParticles::FourMomentum::FourMomentum
( const LoKi::AParticles::FourMomentum& right ) 
  : LoKi::AuxFunBase ( right ) 
  , LoKi::Function<LoKi::ATypes::Combination> ( right ) 
  , m_indices ( right.m_indices ) 
{}
// ============================================================================
// base class printput 
// ============================================================================
std::ostream& LoKi::AParticles::FourMomentum::print_ 
( std::ostream& s , const std::string& nam ) const 
{
  if ( 4 < m_indices.size() )
  { return s << nam << "(" 
             << Gaudi::Utils::toString( m_indices ) << ")" ; } // REUTRN
  s << nam << "(";
  for ( Indices::const_iterator ii = m_indices.begin() ; 
        m_indices.end() != ii ; ++ii ) 
  {
    if ( m_indices.begin() != ii ) { s << "," ; }
    s << (*ii) ;
  }
  return s << ")" ;
}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::AParticles::InvariantMass*
LoKi::AParticles::InvariantMass::clone() const 
{ return new LoKi::AParticles::InvariantMass(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::AParticles::Momentum*
LoKi::AParticles::Momentum::clone() const 
{ return new LoKi::AParticles::Momentum(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::AParticles::TransverseMomentum*
LoKi::AParticles::TransverseMomentum::clone() const 
{ return new LoKi::AParticles::TransverseMomentum(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::AParticles::Eta*
LoKi::AParticles::Eta::clone() const 
{ return new LoKi::AParticles::Eta(*this) ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
