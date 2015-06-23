// $Id$
// ============================================================================
// $URL$
// ============================================================================
// Include files 
// ===========================================================================
// STD & STL 
// ===========================================================================
#include <string>
#include <climits>
#include <algorithm>
#include <functional>
// ===========================================================================
// GaudiKernel
// ===========================================================================
#include "GaudiKernel/ToStream.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/Property.h"
// ===========================================================================
// PartProp
// ===========================================================================
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"
// ===========================================================================
// LHCbMath
// ===========================================================================
#include "LHCbMath/MatrixUtils.h"
#include "LHCbMath/MatrixTransforms.h"
// ===========================================================================
// LHCbKernel
// ===========================================================================
#include "Kernel/CountIterator.h"
// ===========================================================================
// DavinciTypes 
// ===========================================================================
#include "Kernel/HashIDs.h"
// ===========================================================================
// LoKi
// ============================================================================
#include "LoKi/Objects.h"
#include "LoKi/Algs.h"
#include "LoKi/Constants.h"
#include "LoKi/Primitives.h"
#include "LoKi/Kinematics.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/WrongMass.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/AParticles.h"
#include "LoKi/AChild.h"
#include "LoKi/AKinematics.h"
// ============================================================================
#include "LoKi/Algs.h"
// ============================================================================
// Contructor from the critearia:
// ============================================================================
LoKi::AParticles::Count::Count 
( const LoKi::PhysTypes::Cuts& cut ) 
  : LoKi::AuxFunBase  ( std::tie ( cut ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_cut (cut ) 
{}
// ============================================================================
// copy contructor 
// ============================================================================
LoKi::AParticles::Count::Count 
( const LoKi::AParticles::Count& right ) 
  : LoKi::AuxFunBase ( right ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function ( right ) 
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_tool ( tool ) 
{}
// ============================================================================
// Contructor from the tool 
// ============================================================================
LoKi::AParticles::Unique::Unique 
( const LoKi::Interface<ICheckOverlap>& tool ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_tool ( tool ) 
{}
// ============================================================================
// copy contructor 
// ============================================================================
LoKi::AParticles::Unique::Unique 
( const LoKi::AParticles::Unique& right ) 
  : AuxFunBase ( right ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate ( right ) 
  , m_tool ( right.m_tool ) 
{}
// ============================================================================
LoKi::AParticles::Unique::~Unique() 
{
  if ( m_tool && !gaudi() ) 
  {
    // Warning(""manual reset of ICheckOverlap");
    m_tool.reset() ;
  }
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::Unique::result_type  
LoKi::AParticles::Unique::operator() 
  ( LoKi::AParticles::Unique::argument v ) const 
{
  Assert ( m_tool.validPointer() , "Invalid pouinter to ICheckOverlap tool" ) ;
  // use the tool
  return  !m_tool->foundOverlap ( v.begin() , v.end() ) ;
}
// ============================================================================
// constructor from one index 
// ============================================================================
LoKi::AParticles::FourMomentum::FourMomentum
( const std::size_t index )
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_indices ( 1 , index ) 
{}
// ============================================================================
// constructor from two indices 
// ============================================================================
LoKi::AParticles::FourMomentum::FourMomentum
( const std::size_t index1 , 
  const std::size_t index2 )
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_indices () 
{
  m_indices.push_back ( index1 ) ;
  m_indices.push_back ( index2 ) ;  
  std::sort ( m_indices.begin() , m_indices.end() ) ;
}
// ============================================================================
// constructor from three indices 
// ============================================================================
LoKi::AParticles::FourMomentum::FourMomentum
( const std::size_t index1 , 
  const std::size_t index2 ,
  const std::size_t index3 )
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_indices () 
{
  m_indices.push_back ( index1 ) ;
  m_indices.push_back ( index2 ) ;  
  m_indices.push_back ( index3 ) ;  
  std::sort ( m_indices.begin() , m_indices.end() ) ;
}
// ============================================================================
// constructor from four indices 
// ============================================================================
LoKi::AParticles::FourMomentum::FourMomentum
( const std::size_t index1 , 
  const std::size_t index2 ,
  const std::size_t index3 ,
  const std::size_t index4 )
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_indices () 
{
  m_indices.push_back ( index1 ) ;
  m_indices.push_back ( index2 ) ;  
  m_indices.push_back ( index3 ) ;  
  m_indices.push_back ( index4 ) ;  
  std::sort ( m_indices.begin() , m_indices.end() ) ;
}
// ============================================================================
// contructor from list of indices 
// ============================================================================
LoKi::AParticles::FourMomentum::FourMomentum
( const LoKi::AParticles::FourMomentum::Indices& indices ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_indices ( indices ) 
{
  std::sort ( m_indices.begin() , m_indices.end() ) ;
}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::AParticles::FourMomentum::FourMomentum
( const LoKi::AParticles::FourMomentum& right ) 
  : LoKi::AuxFunBase                                         ( right ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function ( right ) 
  , m_indices ( right.m_indices ) 
{}
// ============================================================================
// base class printput 
// ============================================================================
std::ostream& LoKi::AParticles::FourMomentum::print_ 
( std::ostream& s , const std::string& nam ) const 
{
  if ( 4 < m_indices.size() )
  { return s << nam 
             << Gaudi::Utils::toString( m_indices ) << ")" ; } // REUTRN
  s << nam ;
  for ( Indices::const_iterator ii = m_indices.begin() ; 
        m_indices.end() != ii ; ++ii ) 
  {
    if ( m_indices.begin() != ii ) { s << "," ; }
    s << (*ii) ;
  }
  return s << ")" ;
}
// ============================================================================


// ============================================================================
// constructor from one index 
// ============================================================================
LoKi::AParticles::InvariantMass::InvariantMass
( const std::size_t index) 
  : LoKi::AuxFunBase (   std::tie ( index ) ) 
  , LoKi::AParticles::FourMomentum( index )
{}
// ============================================================================
// constructor from two indices 
// ============================================================================
LoKi::AParticles::InvariantMass::InvariantMass
( const std::size_t index1 , 
  const std::size_t index2 ) 
  : LoKi::AuxFunBase (   std::tie ( index1 , index2 ) ) 
  , LoKi::AParticles::FourMomentum( index1 , index2  ) 
{}
// ============================================================================
// constructor from three indices 
// ============================================================================
LoKi::AParticles::InvariantMass::InvariantMass
( const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ) 
  : LoKi::AuxFunBase (   std::tie ( index1 , index2 , index3 ) ) 
  , LoKi::AParticles::FourMomentum( index1 , index2 , index3 )
{}
// ============================================================================
// constructor from four indices 
// ============================================================================
LoKi::AParticles::InvariantMass::InvariantMass
( const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ,
  const std::size_t index4 ) 
  : LoKi::AuxFunBase (   std::tie ( index1 , index2 , index3 , index4 ) ) 
  , LoKi::AParticles::FourMomentum( index1 , index2 , index3 , index4 )
{}
// ============================================================================
// constructor from four-momentum :
// ============================================================================
LoKi::AParticles::InvariantMass::InvariantMass
( const LoKi::AParticles::FourMomentum& right ) 
  : LoKi::AuxFunBase (    std::tie ( right ) ) 
  , LoKi::AParticles::FourMomentum ( right )
{}
// ============================================================================
// constructor form the list of indices 
// ============================================================================
LoKi::AParticles::InvariantMass::InvariantMass
( const LoKi::AParticles::FourMomentum::Indices& indices ) 
  : LoKi::AuxFunBase (    std::tie ( indices ) )
  , LoKi::AParticles::FourMomentum ( indices )
{}
// ============================================================================


// ============================================================================
// constructor from one index 
// ============================================================================
LoKi::AParticles::Momentum::Momentum
( const std::size_t index) 
  : LoKi::AuxFunBase (   std::tie ( index ) ) 
  , LoKi::AParticles::FourMomentum( index )
{}
// ============================================================================
// constructor from two indices 
// ============================================================================
LoKi::AParticles::Momentum::Momentum
( const std::size_t index1 , 
  const std::size_t index2 ) 
  : LoKi::AuxFunBase (   std::tie ( index1 , index2 ) ) 
  , LoKi::AParticles::FourMomentum( index1 , index2  ) 
{}
// ============================================================================
// constructor from three indices 
// ============================================================================
LoKi::AParticles::Momentum::Momentum
( const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ) 
  : LoKi::AuxFunBase (   std::tie ( index1 , index2 , index3 ) ) 
  , LoKi::AParticles::FourMomentum( index1 , index2 , index3 )
{}
// ============================================================================
// constructor from four indices 
// ============================================================================
LoKi::AParticles::Momentum::Momentum
( const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ,
  const std::size_t index4 ) 
  : LoKi::AuxFunBase (   std::tie ( index1 , index2 , index3 , index4 ) ) 
  , LoKi::AParticles::FourMomentum( index1 , index2 , index3 , index4 )
{}
// ============================================================================
// constructor from four-momentum :
// ============================================================================
LoKi::AParticles::Momentum::Momentum
( const LoKi::AParticles::FourMomentum& right ) 
  : LoKi::AuxFunBase (    std::tie ( right ) ) 
  , LoKi::AParticles::FourMomentum ( right )
{}
// ============================================================================
// constructor form the list of indices 
// ============================================================================
LoKi::AParticles::Momentum::Momentum
( const LoKi::AParticles::FourMomentum::Indices& indices ) 
  : LoKi::AuxFunBase (    std::tie ( indices ) )
  , LoKi::AParticles::FourMomentum ( indices )
{}
// ============================================================================


// ============================================================================
// constructor from one index 
// ============================================================================
LoKi::AParticles::TransverseMomentum::TransverseMomentum
( const std::size_t index) 
  : LoKi::AuxFunBase (   std::tie ( index ) ) 
  , LoKi::AParticles::FourMomentum( index )
{}
// ============================================================================
// constructor from two indices 
// ============================================================================
LoKi::AParticles::TransverseMomentum::TransverseMomentum
( const std::size_t index1 , 
  const std::size_t index2 ) 
  : LoKi::AuxFunBase (   std::tie ( index1 , index2 ) ) 
  , LoKi::AParticles::FourMomentum( index1 , index2  ) 
{}
// ============================================================================
// constructor from three indices 
// ============================================================================
LoKi::AParticles::TransverseMomentum::TransverseMomentum
( const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ) 
  : LoKi::AuxFunBase (   std::tie ( index1 , index2 , index3 ) ) 
  , LoKi::AParticles::FourMomentum( index1 , index2 , index3 )
{}
// ============================================================================
// constructor from four indices 
// ============================================================================
LoKi::AParticles::TransverseMomentum::TransverseMomentum
( const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ,
  const std::size_t index4 ) 
  : LoKi::AuxFunBase (   std::tie ( index1 , index2 , index3 , index4 ) ) 
  , LoKi::AParticles::FourMomentum( index1 , index2 , index3 , index4 )
{}
// ============================================================================
// constructor from four-momentum :
// ============================================================================
LoKi::AParticles::TransverseMomentum::TransverseMomentum
( const LoKi::AParticles::FourMomentum& right ) 
  : LoKi::AuxFunBase (    std::tie ( right ) ) 
  , LoKi::AParticles::FourMomentum ( right )
{}
// ============================================================================
// constructor form the list of indices 
// ============================================================================
LoKi::AParticles::TransverseMomentum::TransverseMomentum
( const LoKi::AParticles::FourMomentum::Indices& indices ) 
  : LoKi::AuxFunBase (    std::tie ( indices ) )
  , LoKi::AParticles::FourMomentum ( indices )
{}
// ============================================================================

// ============================================================================
// constructor from one index 
// ============================================================================
LoKi::AParticles::Eta::Eta
( const std::size_t index) 
  : LoKi::AuxFunBase (   std::tie ( index ) ) 
  , LoKi::AParticles::FourMomentum( index )
{}
// ============================================================================
// constructor from two indices 
// ============================================================================
LoKi::AParticles::Eta::Eta
( const std::size_t index1 , 
  const std::size_t index2 ) 
  : LoKi::AuxFunBase (   std::tie ( index1 , index2 ) ) 
  , LoKi::AParticles::FourMomentum( index1 , index2  ) 
{}
// ============================================================================
// constructor from three indices 
// ============================================================================
LoKi::AParticles::Eta::Eta
( const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ) 
  : LoKi::AuxFunBase (   std::tie ( index1 , index2 , index3 ) ) 
  , LoKi::AParticles::FourMomentum( index1 , index2 , index3 )
{}
// ============================================================================
// constructor from four indices 
// ============================================================================
LoKi::AParticles::Eta::Eta
( const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ,
  const std::size_t index4 ) 
  : LoKi::AuxFunBase (   std::tie ( index1 , index2 , index3 , index4 ) ) 
  , LoKi::AParticles::FourMomentum( index1 , index2 , index3 , index4 )
{}
// ============================================================================
// constructor from four-momentum :
// ============================================================================
LoKi::AParticles::Eta::Eta
( const LoKi::AParticles::FourMomentum& right ) 
  : LoKi::AuxFunBase (    std::tie ( right ) ) 
  , LoKi::AParticles::FourMomentum ( right )
{}
// ============================================================================
// constructor form the list of indices 
// ============================================================================
LoKi::AParticles::Eta::Eta
( const LoKi::AParticles::FourMomentum::Indices& indices ) 
  : LoKi::AuxFunBase (    std::tie ( indices ) )
  , LoKi::AParticles::FourMomentum ( indices )
{}
// ============================================================================

// ============================================================================
// constructor from one index 
// ============================================================================
LoKi::AParticles::Phi::Phi
( const std::size_t index) 
  : LoKi::AuxFunBase (   std::tie ( index ) ) 
  , LoKi::AParticles::FourMomentum( index )
{}
// ============================================================================
// constructor from two indices 
// ============================================================================
LoKi::AParticles::Phi::Phi
( const std::size_t index1 , 
  const std::size_t index2 ) 
  : LoKi::AuxFunBase (   std::tie ( index1 , index2 ) ) 
  , LoKi::AParticles::FourMomentum( index1 , index2  ) 
{}
// ============================================================================
// constructor from three indices 
// ============================================================================
LoKi::AParticles::Phi::Phi
( const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ) 
  : LoKi::AuxFunBase (   std::tie ( index1 , index2 , index3 ) ) 
  , LoKi::AParticles::FourMomentum( index1 , index2 , index3 )
{}
// ============================================================================
// constructor from four indices 
// ============================================================================
LoKi::AParticles::Phi::Phi
( const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ,
  const std::size_t index4 ) 
  : LoKi::AuxFunBase (   std::tie ( index1 , index2 , index3 , index4 ) ) 
  , LoKi::AParticles::FourMomentum( index1 , index2 , index3 , index4 )
{}
// ============================================================================
// constructor from four-momentum :
// ============================================================================
LoKi::AParticles::Phi::Phi
( const LoKi::AParticles::FourMomentum& right ) 
  : LoKi::AuxFunBase ( std::tie ( right ) ) 
  , LoKi::AParticles::FourMomentum ( right )
{}
// ============================================================================
// constructor form the list of indices 
// ============================================================================
LoKi::AParticles::Phi::Phi
( const LoKi::AParticles::FourMomentum::Indices& indices ) 
  : LoKi::AuxFunBase (    std::tie ( indices ) )
  , LoKi::AParticles::FourMomentum ( indices )
{}
// ============================================================================



// ============================================================================
// constructor from one index 
// ============================================================================
LoKi::AParticles::Y::Y
( const std::size_t index) 
  : LoKi::AuxFunBase (   std::tie ( index ) ) 
  , LoKi::AParticles::FourMomentum( index )
{}
// ============================================================================
// constructor from two indices 
// ============================================================================
LoKi::AParticles::Y::Y
( const std::size_t index1 , 
  const std::size_t index2 ) 
  : LoKi::AuxFunBase (   std::tie ( index1 , index2 ) ) 
  , LoKi::AParticles::FourMomentum( index1 , index2  ) 
{}
// ============================================================================
// constructor from three indices 
// ============================================================================
LoKi::AParticles::Y::Y
( const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ) 
  : LoKi::AuxFunBase (   std::tie ( index1 , index2 , index3 ) ) 
  , LoKi::AParticles::FourMomentum( index1 , index2 , index3 )
{}
// ============================================================================
// constructor from four indices 
// ============================================================================
LoKi::AParticles::Y::Y
( const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ,
  const std::size_t index4 ) 
  : LoKi::AuxFunBase (   std::tie ( index1 , index2 , index3 , index4 ) ) 
  , LoKi::AParticles::FourMomentum( index1 , index2 , index3 , index4 )
{}
// ============================================================================
// constructor from four-momentum :
// ============================================================================
LoKi::AParticles::Y::Y
( const LoKi::AParticles::FourMomentum& right ) 
  : LoKi::AuxFunBase ( std::tie ( right  ) ) 
  , LoKi::AParticles::FourMomentum ( right )
{}
// ============================================================================
// constructor form the list of indices 
// ============================================================================
LoKi::AParticles::Y::Y
( const LoKi::AParticles::FourMomentum::Indices& indices ) 
  : LoKi::AuxFunBase (    std::tie ( indices ) )
  , LoKi::AParticles::FourMomentum ( indices )
{}
// ============================================================================


























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
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::AParticles::Phi*
LoKi::AParticles::Phi::clone() const 
{ return new LoKi::AParticles::Phi(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::AParticles::Y*
LoKi::AParticles::Y::clone() const 
{ return new LoKi::AParticles::Y(*this) ; }

// ============================================================================
// Constructor from the function
// ============================================================================
LoKi::AParticles::MinChild::MinChild 
( const LoKi::PhysTypes::Func& fun  )
  : LoKi::AuxFunBase ( std::tie  (fun ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun    ( fun ) 
  , m_cut    ( LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant (true) ) 
  , m_minval ( std::numeric_limits<double>::max() ) 
{}
// ============================================================================
// Constructor from the function and minval 
// ============================================================================
LoKi::AParticles::MinChild::MinChild 
( const LoKi::PhysTypes::Func& fun  ,
  const double                 minv ) 
  : LoKi::AuxFunBase ( std::tie  ( fun , minv) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun    ( fun ) 
  , m_cut    ( LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant (true) ) 
  , m_minval ( minv ) 
{}
// ============================================================================
// Contructor from the function and criteria 
// ============================================================================
LoKi::AParticles::MinChild::MinChild 
( const LoKi::PhysTypes::Func& fun  ,
  const LoKi::PhysTypes::Cuts& cut  ) 
  : LoKi::AuxFunBase ( std::tie  ( fun , cut ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun    ( fun ) 
  , m_cut    ( cut ) 
  , m_minval ( std::numeric_limits<double>::max() ) 
{}
// ============================================================================
// Contructor from the function and criteria 
// ============================================================================
LoKi::AParticles::MinChild::MinChild 
( const LoKi::PhysTypes::Cuts& cut  ,
  const LoKi::PhysTypes::Func& fun  ) 
  : LoKi::AuxFunBase ( std::tie  ( cut , fun ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun    ( fun ) 
  , m_cut    ( cut ) 
  , m_minval ( std::numeric_limits<double>::max() ) 
{}
// ============================================================================
// Contructor from the function, criteria and minval
// ============================================================================
LoKi::AParticles::MinChild::MinChild 
( const LoKi::PhysTypes::Func& fun  ,
  const LoKi::PhysTypes::Cuts& cut  ,
  const double                 minv ) 
  : LoKi::AuxFunBase ( std::tie  ( fun , cut , minv ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun    ( fun  ) 
  , m_cut    ( cut  ) 
  , m_minval ( minv ) 
{}
// ============================================================================
// Contructor from the function, criteria and minval
// ============================================================================
LoKi::AParticles::MinChild::MinChild 
( const LoKi::PhysTypes::Cuts& cut  ,
  const LoKi::PhysTypes::Func& fun  ,
  const double                 minv ) 
  : LoKi::AuxFunBase ( std::tie  ( cut , fun , minv ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun    ( fun  ) 
  , m_cut    ( cut  ) 
  , m_minval ( minv ) 
{}
// ============================================================================
// copy contructor 
// ============================================================================
LoKi::AParticles::MinChild::MinChild 
( const LoKi::AParticles::MinChild& right ) 
  : LoKi::AuxFunBase                                         ( right ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function ( right ) 
  , m_fun    ( right.m_fun    ) 
  , m_cut    ( right.m_cut    ) 
  , m_minval ( right.m_minval ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::MinChild::result_type  
LoKi::AParticles::MinChild::operator() 
  ( LoKi::AParticles::MinChild::argument v ) const 
{
  return LoKi::Algs::min_value 
    ( v.begin () , v.end() , m_fun , m_cut , m_minval ) ;
}
// ============================================================================
// OPTIONALLY: the nice printout 
// ============================================================================
std::ostream& LoKi::AParticles::MinChild::fillStream 
( std::ostream& s ) const 
{ 
  static const LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant s_S (true) ;
  //
  s << "AMINCHILD(" << m_fun ;
  if ( m_cut.id() != s_S.id()                         ) { s << "," << m_cut    ; }
  if ( std::numeric_limits<double>::max() != m_minval ) { s << "," << m_minval ; }
  return s << ")" ;
}
// ============================================================================
// Constructor from the function
// ============================================================================
LoKi::AParticles::MaxChild::MaxChild 
( const LoKi::PhysTypes::Func& fun  )
  : LoKi::AuxFunBase ( std::tie ( fun ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun    ( fun ) 
  , m_cut    ( LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true ) ) 
  , m_maxval ( -1 * std::numeric_limits<double>::max() ) 
{}
// ============================================================================
// Constructor from the function and minval 
// ============================================================================
LoKi::AParticles::MaxChild::MaxChild 
( const LoKi::PhysTypes::Func& fun  ,
  const double                 maxv ) 
  : LoKi::AuxFunBase ( std::tie ( fun , maxv ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun    ( fun ) 
  , m_cut    ( LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true ) ) 
  , m_maxval ( maxv ) 
{}
// ============================================================================
// Contructor from the function and criteria 
// ============================================================================
LoKi::AParticles::MaxChild::MaxChild 
( const LoKi::PhysTypes::Func& fun  ,
  const LoKi::PhysTypes::Cuts& cut  ) 
  : LoKi::AuxFunBase ( std::tie  ( fun , cut ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun    ( fun ) 
  , m_cut    ( cut ) 
  , m_maxval ( -1 * std::numeric_limits<double>::max() ) 
{}
// ============================================================================
// Contructor from the function and criteria 
// ============================================================================
LoKi::AParticles::MaxChild::MaxChild 
( const LoKi::PhysTypes::Cuts& cut  ,
  const LoKi::PhysTypes::Func& fun  ) 
  : LoKi::AuxFunBase ( std::tie  ( cut , fun ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun    ( fun ) 
  , m_cut    ( cut ) 
  , m_maxval ( -1 * std::numeric_limits<double>::max() ) 
{}
// ============================================================================
// Contructor from the function, criteria and minval
// ============================================================================
LoKi::AParticles::MaxChild::MaxChild 
( const LoKi::PhysTypes::Func& fun  ,
  const LoKi::PhysTypes::Cuts& cut  ,
  const double                 maxv ) 
  : LoKi::AuxFunBase ( std::tie  ( fun , cut , maxv ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun    ( fun  ) 
  , m_cut    ( cut  ) 
  , m_maxval ( maxv ) 
{}
// ============================================================================
// Contructor from the function, criteria and minval
// ============================================================================
LoKi::AParticles::MaxChild::MaxChild 
( const LoKi::PhysTypes::Cuts& cut  ,
  const LoKi::PhysTypes::Func& fun  ,
  const double                 maxv ) 
  : LoKi::AuxFunBase ( std::tie  ( cut , fun , maxv ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun    ( fun  ) 
  , m_cut    ( cut  ) 
  , m_maxval ( maxv ) 
{}
// ============================================================================
// copy contructor 
// ============================================================================
LoKi::AParticles::MaxChild::MaxChild 
( const LoKi::AParticles::MaxChild& right ) 
  : LoKi::AuxFunBase                                         ( right ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function ( right ) 
  , m_fun    ( right.m_fun    ) 
  , m_cut    ( right.m_cut    ) 
  , m_maxval ( right.m_maxval ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::MaxChild::result_type  
LoKi::AParticles::MaxChild::operator() 
  ( LoKi::AParticles::MaxChild::argument v ) const 
{
  return LoKi::Algs::max_value 
    ( v.begin () , v.end() , m_fun , m_cut , m_maxval ) ;
}
// ============================================================================
// OPTIONALLY: the nice printout 
// ============================================================================
std::ostream& LoKi::AParticles::MaxChild::fillStream 
( std::ostream& s ) const 
{ 
  static const LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant s_S (true) ;
  //
  s << "AMAXCHILD(" << m_fun ;
  if ( m_cut.id() != s_S.id()  ) 
  { s << "," << m_cut    ; }
  if ( -1 * std::numeric_limits<double>::max() != m_maxval ) 
  { s << "," << m_maxval ; }
  return s << ")" ;
}
// ============================================================================
// Constructor from the critearia:
// ============================================================================
LoKi::AParticles::HasChild::HasChild
( const LoKi::PhysTypes::Cuts& cut  ) 
  : LoKi::AuxFunBase ( std::tie  ( cut ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_cut ( cut ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::AParticles::HasChild::HasChild
( const LoKi::AParticles::HasChild& right )
  : LoKi::AuxFunBase                                          ( right ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate ( right ) 
  , m_cut ( right.m_cut ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::HasChild::result_type 
LoKi::AParticles::HasChild::operator() 
  ( LoKi::AParticles::HasChild::argument v ) const 
{ return LoKi::Algs::found ( v.begin() , v.end() , m_cut ) ; }
// ============================================================================
// OPTIONALLY: the nice printout 
// ============================================================================
std::ostream& 
LoKi::AParticles::HasChild::fillStream ( std::ostream& s ) const 
{ return s << "AHASCHILD(" << m_cut << ")" ; }
// ===========================================================================
// Constructor from the fuction and index 
// ===========================================================================
LoKi::AParticles::ChildFun::ChildFun
( const LoKi::PhysTypes::Func& fun   , 
  const int                    index ) 
  : LoKi::AuxFunBase ( std::tie  ( fun , index  ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun   ( fun ) 
  , m_index ( index ) 
{}
// ===========================================================================
// Constructor from the fuction and index 
// ===========================================================================
LoKi::AParticles::ChildFun::ChildFun
( const int                    index ,
  const LoKi::PhysTypes::Func& fun   )
  : LoKi::AuxFunBase ( std::tie  ( index , fun ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun   ( fun   ) 
  , m_index ( index ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::AParticles::ChildFun::ChildFun
( const LoKi::AParticles::ChildFun& right )
  : LoKi::AuxFunBase                                          ( right ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function  ( right ) 
  , m_fun   ( right.m_fun   ) 
  , m_index ( right.m_index ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::ChildFun::result_type
LoKi::AParticles::ChildFun::operator() 
  ( LoKi::AParticles::ChildFun::argument a ) const 
{
  const LHCb::Particle* c = LoKi::Child::child ( a , m_index ) ;
  if ( 0 == c ) { Warning ( "The child particle points to NULL!" ) ; }
  return m_fun ( c ) ;
}
// ============================================================================
// OPTIONALLY: the nice printout 
// ============================================================================
std::ostream& 
LoKi::AParticles::ChildFun::fillStream ( std::ostream& s ) const 
{ return s << "ACHILD(" << m_fun << "," << m_index << ")" ; }
// ===========================================================================
// Constructor from the fuction and index 
// ===========================================================================
LoKi::AParticles::ChildCut::ChildCut
( const LoKi::PhysTypes::Cuts& cut   , 
  const int                    index )
  : LoKi::AuxFunBase ( std::tie  ( cut , index  ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_cut   ( cut ) 
  , m_index ( index ) 
{}
// ===========================================================================
// Constructor from the fuction and index 
// ===========================================================================
LoKi::AParticles::ChildCut::ChildCut
( const int                    index ,
  const LoKi::PhysTypes::Cuts& cut   )
  : LoKi::AuxFunBase ( std::tie  ( index , cut ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_cut   ( cut   ) 
  , m_index ( index ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::AParticles::ChildCut::ChildCut
( const LoKi::AParticles::ChildCut& right )
  : LoKi::AuxFunBase                                          ( right ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate ( right ) 
  , m_cut   ( right.m_cut   ) 
  , m_index ( right.m_index ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::ChildCut::result_type
LoKi::AParticles::ChildCut::operator() 
  ( LoKi::AParticles::ChildCut::argument a ) const 
{
  const LHCb::Particle* c = LoKi::Child::child ( a , m_index ) ;
  if ( 0 == c ) { Warning ( "The child particle points to NULL!" ) ; }
  return m_cut ( c ) ;
}
// ============================================================================
// OPTIONALLY: the nice printout 
// ============================================================================
std::ostream& 
LoKi::AParticles::ChildCut::fillStream ( std::ostream& s ) const 
{ return s << "ACHILDCUT(" << m_cut << "," << m_index << ")" ; }
// ============================================================================
// constructor from the tool:
// ============================================================================
LoKi::AParticles::VertexChi2::VertexChi2 ( IVertexFit*  fitter  ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fit ( fitter ) 
{}
// ============================================================================
// constructor from the tool:L
// ============================================================================
LoKi::AParticles::VertexChi2::VertexChi2
( const LoKi::Interface<IVertexFit>& fitter  ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fit ( fitter ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::AParticles::VertexChi2::VertexChi2
( const LoKi::AParticles::VertexChi2& right ) 
  : LoKi::AuxFunBase                                         ( right ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function ( right ) 
  , m_fit ( right.m_fit ) 
{}
// ============================================================================
LoKi::AParticles::VertexChi2::~VertexChi2() 
{
  if ( m_fit && !gaudi() ) 
  {
    // Warning("Manual reset of IVertexFit") ;
    m_fit.reset() ;
  } 
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::VertexChi2::result_type
LoKi::AParticles::VertexChi2::operator()
  ( LoKi::AParticles::VertexChi2::argument a ) const 
{
  Assert ( m_fit.validPointer() , "Invalid pointer to IVetexFit tool" );
  if ( 2 > a.size() )
  {
    Error ( "Invalid number of daughters, return InvalidChi2" ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  // create the vertex
  LHCb::Vertex vertex ;
  StatusCode sc = m_fit -> fit ( vertex , a.begin() , a.end() ) ;
  if ( sc.isFailure() ) 
  {
    Error ( "Error from the VertexFitter, return InvalidChi2" , sc ) ;
    return LoKi::Constants::InvalidChi2 ;
  } 
  return vertex.chi2() ;
}
// ============================================================================
std::ostream& 
LoKi::AParticles::VertexChi2::fillStream ( std::ostream& s ) const 
{ return s << "AVCHI2['" << m_fit->name() << "']" ; }


// ============================================================================
// constructor from the tool:
// ============================================================================
LoKi::AParticles::MaxDOCA::MaxDOCA 
( const IDistanceCalculator*  doca  , 
  const bool                  allow ) 
  : LoKi::AParticles::DOCA ( 1 , 1 , doca , allow ) 
{}
// ============================================================================
// constructor from the tool:
// ============================================================================
LoKi::AParticles::MaxDOCA::MaxDOCA
( const LoKi::Interface<IDistanceCalculator>& doca  ,
  const bool                                  allow ) 
  : LoKi::AParticles::DOCA ( 1 , 1 , doca , allow ) 
{}
// ============================================================================
// constructor from the tool:
// ============================================================================
LoKi::AParticles::MaxDOCA::MaxDOCA
( const std::string& doca  ,
  const bool         allow ) 
  : LoKi::AuxFunBase ( std::tie  ( doca , allow ) ) 
  , LoKi::AParticles::DOCA ( 1 , 1 , doca , allow ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::MaxDOCA::result_type
LoKi::AParticles::MaxDOCA::operator()
  ( LoKi::AParticles::MaxDOCA::argument a ) const 
{
  //
  if ( !tool() ) { loadTool() ; }
  //
  if ( a.empty() ) 
  {
    Error("Empty constainer, return Invalid Distance!") ;
    return LoKi::Constants::InvalidDistance ;
  }
  return docamax ( a.begin() , a.end() ) ;
}
// ============================================================================
std::ostream& 
LoKi::AParticles::MaxDOCA::fillStream ( std::ostream& s ) const 
{ return s << "AMAXDOCA('" 
           << toolName() << "',"
           << ( allow() ? "True" : "False" ) 
           << "')" ; }


// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCACut::MaxDOCACut 
( const double                threshold ,
  const IDistanceCalculator*  doca      ,
  const bool                  allow     ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_doca      ( 1 , 1 , doca , allow ) 
  , m_threshold ( threshold )
{}
// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCACut::MaxDOCACut
( const double                                threshold ,
  const LoKi::Interface<IDistanceCalculator>& doca      ,
  const bool                                  allow     ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_doca      ( 1 , 1 , doca , allow ) 
  , m_threshold ( threshold )
{}
// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCACut::MaxDOCACut
( const double       threshold ,
  const std::string& doca      ,
  const bool         allow     ) 
  : LoKi::AuxFunBase ( std::tie  ( threshold , doca , allow ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_doca      ( 1 , 1 , doca , allow ) 
  , m_threshold ( threshold )
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::MaxDOCACut::result_type
LoKi::AParticles::MaxDOCACut::operator()
  ( LoKi::AParticles::MaxDOCACut::argument a ) const 
{
  // 
  if ( !tool() ) { loadTool() ; }
  //
  typedef LoKi::ATypes::Combination A ;
  for ( A::const_iterator i = a.begin() ; a.end() != i ; ++i )
  {
    for ( A::const_iterator j = i + 1 ; a.end() != j ; ++j )
    {
      const double dist = m_doca.doca ( *j , *i ) ;
      if ( dist > m_threshold ) { return false ; }                // RETURN      
    } 
  }
  return true ;
}
// ============================================================================
std::ostream& 
LoKi::AParticles::MaxDOCACut::fillStream ( std::ostream& s ) const 
{ return s << "ACUTDOCA(" 
           << m_threshold << ",'" 
           << toolName()  << "',"
           << ( allow() ? "True" : "False" ) 
           << ")" ; }

// ============================================================================
// constructor from the tool:
// ============================================================================
LoKi::AParticles::MaxDOCAChi2::MaxDOCAChi2 
( const IDistanceCalculator*  doca  , 
  const bool                  allow )
  : LoKi::AParticles::DOCAChi2 ( 1 , 1 , doca , allow ) 
{}
// ============================================================================
// constructor from the tool:L
// ============================================================================
LoKi::AParticles::MaxDOCAChi2::MaxDOCAChi2
( const LoKi::Interface<IDistanceCalculator>& doca  , 
  const bool                                  allow )
  : LoKi::AParticles::DOCAChi2 ( 1 , 1 , doca , allow ) 
{}
// ============================================================================
// constructor from the tool:L
// ============================================================================
LoKi::AParticles::MaxDOCAChi2::MaxDOCAChi2
( const std::string& doca  ,
  const bool         allow )
  : LoKi::AuxFunBase ( std::tie  ( doca , allow ) ) 
  , LoKi::AParticles::DOCAChi2 ( 1 , 1 , doca , allow ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::MaxDOCAChi2::result_type
LoKi::AParticles::MaxDOCAChi2::operator()
  ( LoKi::AParticles::MaxDOCAChi2::argument a ) const 
{
  if ( !tool() ) { loadTool() ; }
  
  //
  if ( a.empty() ) 
  {
    Error("Empty constainer, return Invalid Distance!") ;
    return LoKi::Constants::InvalidDistance ;
  }
  return docachi2max ( a.begin() , a.end() ) ;
}
// ============================================================================
std::ostream& 
LoKi::AParticles::MaxDOCAChi2::fillStream ( std::ostream& s ) const 
{ return s << "AMAXDOCACHI2('" 
           << toolName () << "',"
           << ( allow() ? "True" : "False" )
           << ")" ; }


// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCAChi2Cut::MaxDOCAChi2Cut 
( const double                threshold ,
  const IDistanceCalculator*  doca      , 
  const bool                  allow     )
  : LoKi::AParticles::MaxDOCACut ( threshold , doca , allow  ) 
{}
// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCAChi2Cut::MaxDOCAChi2Cut
( const double                                threshold ,
  const LoKi::Interface<IDistanceCalculator>& doca      ,
  const bool                                  allow     )
  : LoKi::AParticles::MaxDOCACut ( threshold , doca , allow ) 
{}
// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCAChi2Cut::MaxDOCAChi2Cut
( const double       threshold ,
  const std::string& doca      ,
  const bool         allow     )
  : LoKi::AuxFunBase ( std::tie  ( threshold , doca , allow ) ) 
  , LoKi::AParticles::MaxDOCACut ( threshold , doca , allow ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::MaxDOCAChi2Cut::result_type
LoKi::AParticles::MaxDOCAChi2Cut::operator()
  ( LoKi::AParticles::MaxDOCAChi2Cut::argument a ) const 
{
  
  if ( !tool() ) { loadTool() ; }
  
  typedef LoKi::ATypes::Combination A ;
  for ( A::const_iterator i = a.begin() ; a.end() != i ; ++i )
  {
    for ( A::const_iterator j = i + 1 ; a.end() != j ; ++j )
    {
      const double chi2 = m_doca.chi2 ( *j , *i ) ;
      if ( m_threshold < chi2  ) { return false ; }
    } 
  }
  return true ;
}
// ============================================================================
std::ostream& 
LoKi::AParticles::MaxDOCAChi2Cut::fillStream ( std::ostream& s ) const 
{ return s << "ACUTDOCACHI2(" << m_threshold << ",'" 
           << toolName ()  << "',"
           << ( allow() ? "True" : "False" )
           << ")" ; }
// ============================================================================
/*  constructor with daughter index (starts from 1).
 *  E.g. for 2-body decays it could be 1 or 2 
 *  @param index of daughter particles
 */
// ============================================================================
LoKi::AParticles::DecayAngle::DecayAngle
( const size_t child ) 
  : LoKi::AuxFunBase ( std::tie  ( child ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_child ( child ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::AParticles::DecayAngle::DecayAngle
( const LoKi::AParticles::DecayAngle& right ) 
  : LoKi::AuxFunBase                                         ( right )
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function ( right ) 
  , m_child                                   ( right.m_child ) 
{}
// ============================================================================
LoKi::AParticles::DecayAngle::result_type 
LoKi::AParticles::DecayAngle::operator() 
  ( LoKi::AParticles::DecayAngle::argument a ) const
{
  if ( 2 > a.size() ) 
  { 
    Error ( " Argumentis invalid, return 'InvalidAngle' " ) ;
    return LoKi::Constants::InvalidAngle ;                        // RETURN 
  }               
  // get the gaughter particle
  const LHCb::Particle* child = LoKi::Child::child ( a , m_child ) ;
  if ( 0 == child ) 
  { 
    Error ( " 'Child' is invalid, return 'InvalidAngle' " ) ;
    return LoKi::Constants::InvalidAngle;                         // RETURN 
  }               
  const LoKi::LorentzVector& mv = LoKi::Kinematics::momentum ( a ) ;
  const LoKi::LorentzVector& dv = child -> momentum() ;
  // 
  return LoKi::Kinematics::decayAngle( dv , mv ) ;
} 
// ============================================================================
// OPTIONAL:  the specific printout 
// ============================================================================
std::ostream& 
LoKi::AParticles::DecayAngle::fillStream ( std::ostream& s ) const 
{ 
  s << "LV0" ;
  switch ( m_child ) 
  {
  case 1  :
    s << "1"                   ; break ;
  case 2  :
    s << "2"                   ; break ;
  case 3  :
    s << "3"                   ; break ;
  case 4  :
    s << "4"                   ; break ;
  default :
    s << "(" << m_child << ")" ; break ;
  }
  return s ;
}
// ============================================================================
/*  constructor with daughter index (starts from 1).
 *  E.g. for 2-body decays it could be 1 or 2 
 *  @param child1 index of first daughter particle
 *  @param child2 index of first daughter particle
 */
// ============================================================================
LoKi::AParticles::DeltaAngle::DeltaAngle 
( const unsigned short child1 ,
  const unsigned short child2 ) 
  : LoKi::AuxFunBase ( std::tie ( child1 , child2 ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function()
  , m_child1 ( child1 ) 
  , m_child2 ( child2 ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::AParticles::DeltaAngle::DeltaAngle 
( const LoKi::AParticles::DeltaAngle& right ) 
  : LoKi::AuxFunBase ( right  ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function( right ) 
  , m_child1 ( right.m_child1 ) 
  , m_child2 ( right.m_child2 ) 
{}
// ============================================================================
// MANDATORY: virual destructor
// ============================================================================
LoKi::AParticles::DeltaAngle::~DeltaAngle(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::AParticles::DeltaAngle*
LoKi::AParticles::DeltaAngle::clone() const 
{ return new LoKi::AParticles::DeltaAngle ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::DeltaAngle::result_type 
LoKi::AParticles::DeltaAngle::operator() 
  ( LoKi::AParticles::DeltaAngle::argument a ) const
{
  // get the gaughter particles
  const LHCb::Particle* child1 = LoKi::Child::child ( a , m_child1 ) ;
  if ( 0 == child1 ) 
  { 
    Error ( " 'Child1' is invalid, return 'InvalidAngle' " ) ;
    return LoKi::Constants::InvalidAngle;                         // RETURN 
  }               
  //
  const LHCb::Particle* child2 = LoKi::Child::child ( a , m_child2 ) ;
  if ( 0 == child2 ) 
  { 
    Error ( " 'Child2' is invalid, return 'InvalidAngle' " ) ;
    return LoKi::Constants::InvalidAngle;                         // RETURN 
  }
  //
  if ( child1 == child2 ) { return 1 ; }  // RETURN
  //
  const LoKi::LorentzVector& p1 = child1 -> momentum() ;
  const LoKi::LorentzVector& p2 = child2 -> momentum() ;
  //
  const LoKi::Vector3D v1 ( p1.Vect() ) ;
  const LoKi::Vector3D v2 ( p2.Vect() ) ;
  //
  return v1.Dot ( v2 ) / std::sqrt ( v1.Mag2() * v2.Mag2() ) ;
}
// ============================================================================
// OPTIONAL:  the specific printout 
// ============================================================================
std::ostream& 
LoKi::AParticles::DeltaAngle::fillStream ( std::ostream& s ) const 
{ return s << "ALV(" << m_child1 << "," << m_child2 << ")" ; }
// ============================================================================


// ============================================================================
// constructor from two masses 
// ============================================================================
LoKi::AParticles::WrongMass:: WrongMass  
( const double m1 , 
  const double m2 ) 
  : LoKi::AuxFunBase ( std::tie  ( m1 , m2 ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_wm ( m1 , m2 ) 
{}
// ============================================================================
// constructor from three masses 
// ============================================================================
LoKi::AParticles::WrongMass:: WrongMass 
( const double m1 , 
  const double m2 , 
  const double m3 ) 
  : LoKi::AuxFunBase ( std::tie  ( m1 , m2 , m3 ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_wm ( m1 , m2 , m3 ) 
{}
// ============================================================================
// constructor from four masses 
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass 
( const double m1 , 
  const double m2 , 
  const double m3 , 
  const double m4 ) 
  : LoKi::AuxFunBase ( std::tie  ( m1 , m2 , m3 , m4 ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_wm ( m1 , m2 , m3 , m4 ) 
{}
// ============================================================================
// constructor from the vector of masses 
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass 
( const std::vector<double>& masses ) 
  : LoKi::AuxFunBase ( std::tie  ( masses ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_wm ( masses ) 
{}
// ============================================================================
// constructor from two pids 
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass 
( const LHCb::ParticleID& pid1 , 
  const LHCb::ParticleID& pid2 ) 
  : LoKi::AuxFunBase ( std::tie  ( pid1 , pid2 ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_wm ( pid1 , pid2 ) 
{}
// ============================================================================
// constructor from three pids 
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass
( const LHCb::ParticleID& pid1 , 
  const LHCb::ParticleID& pid2 ,
  const LHCb::ParticleID& pid3 ) 
  : LoKi::AuxFunBase ( std::tie  ( pid1 , pid2 , pid3 ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_wm ( pid1 , pid2 , pid3 ) 
{}
// ============================================================================
// constructor from four pids 
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass  
( const LHCb::ParticleID& pid1 , 
  const LHCb::ParticleID& pid2 ,
  const LHCb::ParticleID& pid3 ,
  const LHCb::ParticleID& pid4 ) 
  : LoKi::AuxFunBase ( std::tie  ( pid1 , pid2 , pid3 , pid4 ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_wm ( pid1 , pid2 , pid3 , pid4 ) 
{}
// ============================================================================
// constructor from the vector of pids  
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass
( const std::vector<LHCb::ParticleID>& pids ) 
  : LoKi::AuxFunBase ( std::tie  ( pids ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_wm ( pids ) 
{}
// ============================================================================
// constructor from two names  
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass
( const std::string& name1 , 
  const std::string& name2 ) 
  : LoKi::AuxFunBase ( std::tie  ( name1 , name2 ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_wm ( name1 , name2 ) 
{}
// ============================================================================
// constructor from three names  
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ) 
  : LoKi::AuxFunBase ( std::tie  ( name1 , name2 , name3 ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_wm ( name1 , name2 , name3 ) 
{}
// ============================================================================
// constructor from four  names  
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass 
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ,
  const std::string& name4 ) 
  : LoKi::AuxFunBase ( std::tie  ( name1 , name2 , name3 , name4 ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_wm ( name1 , name2 , name3 , name4 ) 
{}
// ============================================================================
// constructor from the vector of names 
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass 
( const std::vector<std::string>& names ) 
  : LoKi::AuxFunBase ( std::tie  ( names ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_wm ( names ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass 
( const LoKi::AParticles::WrongMass& right )
  : LoKi::AuxFunBase                                         ( right ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function ( right ) 
  , m_wm ( right.m_wm ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::WrongMass::result_type 
LoKi::AParticles::WrongMass::operator() 
  ( LoKi::AParticles::WrongMass::argument a ) const 
{
  if ( masses().size() != a.size() ) 
  {
    Error ( "Mismatch in container size(s), return InvaidMass" ) ;
    return LoKi::Constants::InvalidMass ;
  }
  // evaluate wrong mass
  return m_wm.wmass ( a ) ;
}
// ============================================================================
// OPTIONAL: specific printout 
// ============================================================================
std::ostream& 
LoKi::AParticles::WrongMass::fillStream( std::ostream& s ) const 
{
  if      ( !names().empty () ) 
  {
    if      ( 4 == names().size() ) 
    {
      return 
        s << "AWM(" 
          << Gaudi::Utils::toString ( names()[0] ) << ","
          << Gaudi::Utils::toString ( names()[1] ) << ","
          << Gaudi::Utils::toString ( names()[2] ) << ","
          << Gaudi::Utils::toString ( names()[3] ) << ")" ;
    }  
    else if ( 3 == names().size() ) 
    {
      return 
        s << "AWM(" 
          << Gaudi::Utils::toString ( names()[0] ) << ","
          << Gaudi::Utils::toString ( names()[1] ) << ","
          << Gaudi::Utils::toString ( names()[2] ) << ")" ;
    } 
    else if ( 2 == names().size() ) 
    {
      return 
        s << "AWM(" 
          << Gaudi::Utils::toString ( names()[0] ) << ","
          << Gaudi::Utils::toString ( names()[1] ) << ")" ;
    }
    //
    return s << "AWM(" << Gaudi::Utils::toString ( names ()  ) << ")" ; 
  }
  else if ( !pids().empty  () )
  {
    s << "WMASS([" ;
    typedef std::vector<LHCb::ParticleID> PIDs ;
    for ( PIDs::const_iterator ipid = pids().begin() ; 
          pids().end () != ipid ; ++ipid )
    {
      if ( pids().begin() != ipid ) { s << "," ; }
      s << "LHCb.ParticleID( " << ipid->pid() << ")" ;
    }
    return s << "])" ;                                              // RETURN 
  }
  return s << "AWM(" << Gaudi::Utils::toString ( masses() ) << ")" ; 
}
// ============================================================================

// ============================================================================
// constructor from one index 
// ============================================================================
LoKi::AParticles::DeltaMass::DeltaMass 
( const double      m0     , 
  const std::size_t index  ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index ) ) 
  , LoKi::AParticles::InvariantMass ( index ) 
  , m_m0 ( m0 ) 
{}
// ============================================================================
// constructor from two indices 
// ============================================================================
LoKi::AParticles::DeltaMass::DeltaMass 
( const double      m0     ,
  const std::size_t index1 , 
  const std::size_t index2 ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index1 , index2  ) ) 
  , LoKi::AParticles::InvariantMass ( index1 , index2 ) 
  , m_m0 ( m0 ) 
{}
// ============================================================================
// constructor from three indices
// ============================================================================
LoKi::AParticles::DeltaMass::DeltaMass 
( const double      m0     , 
  const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index1 , index2 , index3 ) ) 
  , LoKi::AParticles::InvariantMass ( index1 , index2 , index3 ) 
  , m_m0 ( m0 ) 
{}
// ============================================================================
// constructor from four indices 
// ============================================================================
LoKi::AParticles::DeltaMass::DeltaMass 
( const double      m0     , 
  const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ,
  const std::size_t index4 ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index1 , index2 , index3 , index4 ) ) 
  , LoKi::AParticles::InvariantMass ( index1 , index2 , index3 , index4 ) 
  , m_m0 ( m0 ) 
{}
// ============================================================================
// constructor form the list of indices 
// ============================================================================
LoKi::AParticles::DeltaMass::DeltaMass 
( const double   m0      , 
  const LoKi::AParticles::FourMomentum::Indices& indices ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , indices ) ) 
  , LoKi::AParticles::InvariantMass ( indices ) 
  , m_m0( m0 ) 
{}
// ============================================================================
// constructor from the four-momentum 
// ============================================================================
LoKi::AParticles::DeltaMass::DeltaMass 
( const double                          m0   , 
  const LoKi::AParticles::FourMomentum& four ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , four ) ) 
  , LoKi::AParticles::InvariantMass ( four ) 
  , m_m0( m0 ) 
{}
// ============================================================================



// ============================================================================
// constructor from one index 
// ============================================================================
LoKi::AParticles::DeltaMass::DeltaMass 
( const LHCb::ParticleID& m0     , 
  const std::size_t       index  ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index ) ) 
  , LoKi::AParticles::InvariantMass ( index ) 
  , m_m0 ( LoKi::Particles::massFromPID ( m0 ) ) 
{}
// ============================================================================
// constructor from two indices 
// ============================================================================
LoKi::AParticles::DeltaMass::DeltaMass 
( const LHCb::ParticleID& m0     , 
  const std::size_t index1 , 
  const std::size_t index2 ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index1 , index2 ) ) 
  , LoKi::AParticles::InvariantMass ( index1 , index2 ) 
  , m_m0 ( LoKi::Particles::massFromPID ( m0 ) ) 
{}
// ============================================================================
// constructor from three indices
// ============================================================================
LoKi::AParticles::DeltaMass::DeltaMass 
( const LHCb::ParticleID& m0     , 
  const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index1 , index2 , index3 ) ) 
  , LoKi::AParticles::InvariantMass ( index1 , index2 , index3 ) 
  , m_m0 ( LoKi::Particles::massFromPID ( m0 ) ) 
{}
// ============================================================================
// constructor from four indices 
// ============================================================================
LoKi::AParticles::DeltaMass::DeltaMass 
( const LHCb::ParticleID& m0     , 
  const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ,
  const std::size_t index4 ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index1 , index2 , index3 , index4 ) ) 
  , LoKi::AParticles::InvariantMass ( index1 , index2 , index3 , index4 ) 
  , m_m0 ( LoKi::Particles::massFromPID ( m0 ) ) 
{}
// ============================================================================
// constructor form the list of indices 
// ============================================================================
LoKi::AParticles::DeltaMass::DeltaMass 
( const LHCb::ParticleID&                        m0     , 
  const LoKi::AParticles::FourMomentum::Indices& indices ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , indices ) ) 
  , LoKi::AParticles::InvariantMass ( indices ) 
  , m_m0 ( LoKi::Particles::massFromPID ( m0 ) ) 
{}
// ============================================================================
// constructor from the four-momentum 
// ============================================================================
LoKi::AParticles::DeltaMass::DeltaMass 
( const LHCb::ParticleID&               m0     , 
  const LoKi::AParticles::FourMomentum& four ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , four ) ) 
  , LoKi::AParticles::InvariantMass ( four ) 
  , m_m0 ( LoKi::Particles::massFromPID ( m0 ) ) 
{}
// ============================================================================

// ============================================================================
// constructor from one index 
// ============================================================================
LoKi::AParticles::DeltaMass::DeltaMass 
( const std::string&      m0     , 
  const std::size_t       index  ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index ) ) 
  , LoKi::AParticles::InvariantMass ( index ) 
  , m_m0 ( LoKi::Particles::massFromName( m0 ) ) 
{}
// ============================================================================
// constructor from two indices 
// ============================================================================
LoKi::AParticles::DeltaMass::DeltaMass 
( const std::string& m0     , 
  const std::size_t index1 , 
  const std::size_t index2 ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index1 , index2 ) ) 
  , LoKi::AParticles::InvariantMass ( index1 , index2 ) 
  , m_m0 ( LoKi::Particles::massFromName ( m0 ) ) 
{}
// ============================================================================
// constructor from three indices
// ============================================================================
LoKi::AParticles::DeltaMass::DeltaMass 
( const std::string& m0     , 
  const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index1 , index2 , index3 ) ) 
  , LoKi::AParticles::InvariantMass ( index1 , index2 , index3 ) 
  , m_m0 ( LoKi::Particles::massFromName ( m0 ) ) 
{}
// ============================================================================
// constructor from four indices 
// ============================================================================
LoKi::AParticles::DeltaMass::DeltaMass 
( const std::string& m0     , 
  const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ,
  const std::size_t index4 ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index1 , index2 , index3 , index4 ) ) 
  , LoKi::AParticles::InvariantMass ( index1 , index2 , index3 , index4 ) 
  , m_m0 ( LoKi::Particles::massFromName ( m0 ) ) 
{}
// ============================================================================
// constructor form the list of indices 
// ============================================================================
LoKi::AParticles::DeltaMass::DeltaMass 
( const std::string&                        m0     , 
  const LoKi::AParticles::FourMomentum::Indices& indices ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , indices ) ) 
  , LoKi::AParticles::InvariantMass ( indices ) 
  , m_m0 ( LoKi::Particles::massFromName ( m0 ) ) 
{}
// ============================================================================
// constructor from the four-momentum 
// ============================================================================
LoKi::AParticles::DeltaMass::DeltaMass 
( const std::string&               m0     , 
  const LoKi::AParticles::FourMomentum& four ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , four ) ) 
  , LoKi::AParticles::InvariantMass ( four ) 
  , m_m0 ( LoKi::Particles::massFromName ( m0 ) ) 
{}
// ============================================================================




// ============================================================================
// constructor from one index 
// ============================================================================
LoKi::AParticles::AbsDeltaMass::AbsDeltaMass 
( const double      m0     , 
  const std::size_t index  ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index ) ) 
  , LoKi::AParticles::DeltaMass ( m0 , index ) 
{}
// ============================================================================
// constructor from two indices 
// ============================================================================
LoKi::AParticles::AbsDeltaMass::AbsDeltaMass 
( const double      m0     ,
  const std::size_t index1 , 
  const std::size_t index2 ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index1 , index2  ) ) 
  , LoKi::AParticles::DeltaMass ( m0 , index1 , index2 ) 
{}
// ============================================================================
// constructor from three indices
// ============================================================================
LoKi::AParticles::AbsDeltaMass::AbsDeltaMass 
( const double      m0     , 
  const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index1 , index2 , index3 ) ) 
  , LoKi::AParticles::DeltaMass ( m0 , index1 , index2 , index3 ) 
{}
// ============================================================================
// constructor from four indices 
// ============================================================================
LoKi::AParticles::AbsDeltaMass::AbsDeltaMass 
( const double      m0     , 
  const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ,
  const std::size_t index4 ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index1 , index2 , index3 , index4 ) ) 
  , LoKi::AParticles::DeltaMass ( m0 , index1 , index2 , index3 , index4 ) 
{}
// ============================================================================
// constructor form the list of indices 
// ============================================================================
LoKi::AParticles::AbsDeltaMass::AbsDeltaMass 
( const double   m0      , 
  const LoKi::AParticles::FourMomentum::Indices& indices ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , indices ) ) 
  , LoKi::AParticles::DeltaMass ( m0 , indices ) 
{}
// ============================================================================
// constructor from the four-momentum 
// ============================================================================
LoKi::AParticles::AbsDeltaMass::AbsDeltaMass 
( const double                          m0   , 
  const LoKi::AParticles::FourMomentum& four ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , four ) ) 
  , LoKi::AParticles::DeltaMass ( m0 , four ) 
{}
// ============================================================================





// ============================================================================
// constructor from one index 
// ============================================================================
LoKi::AParticles::AbsDeltaMass::AbsDeltaMass 
( const std::string& m0     , 
  const std::size_t index  ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index ) ) 
  , LoKi::AParticles::DeltaMass ( m0 , index ) 
{}
// ============================================================================
// constructor from two indices 
// ============================================================================
LoKi::AParticles::AbsDeltaMass::AbsDeltaMass 
( const std::string& m0     , 
  const std::size_t index1 , 
  const std::size_t index2 ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index1 , index2  ) ) 
  , LoKi::AParticles::DeltaMass ( m0 , index1 , index2 ) 
{}
// ============================================================================
// constructor from three indices
// ============================================================================
LoKi::AParticles::AbsDeltaMass::AbsDeltaMass 
( const std::string& m0     , 
  const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index1 , index2 , index3 ) ) 
  , LoKi::AParticles::DeltaMass ( m0 , index1 , index2 , index3 ) 
{}
// ============================================================================
// constructor from four indices 
// ============================================================================
LoKi::AParticles::AbsDeltaMass::AbsDeltaMass 
( const std::string& m0     , 
  const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ,
  const std::size_t index4 ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index1 , index2 , index3 , index4 ) ) 
  , LoKi::AParticles::DeltaMass ( m0 , index1 , index2 , index3 , index4 ) 
{}
// ============================================================================
// constructor form the list of indices 
// ============================================================================
LoKi::AParticles::AbsDeltaMass::AbsDeltaMass 
( const std::string& m0     , 
  const LoKi::AParticles::FourMomentum::Indices& indices ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , indices ) ) 
  , LoKi::AParticles::DeltaMass ( m0 , indices ) 
{}
// ============================================================================
// constructor from the four-momentum 
// ============================================================================
LoKi::AParticles::AbsDeltaMass::AbsDeltaMass 
( const std::string& m0     , 
  const LoKi::AParticles::FourMomentum& four ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , four ) ) 
  , LoKi::AParticles::DeltaMass ( m0 , four ) 
{}
// ============================================================================


// ============================================================================
// constructor from one index 
// ============================================================================
LoKi::AParticles::AbsDeltaMass::AbsDeltaMass 
( const LHCb::ParticleID& m0     , 
  const std::size_t index  ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index ) ) 
  , LoKi::AParticles::DeltaMass ( m0 , index ) 
{}
// ============================================================================
// constructor from two indices 
// ============================================================================
LoKi::AParticles::AbsDeltaMass::AbsDeltaMass 
( const LHCb::ParticleID& m0     , 
  const std::size_t index1 , 
  const std::size_t index2 ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index1 , index2  ) ) 
  , LoKi::AParticles::DeltaMass ( m0 , index1 , index2 ) 
{}
// ============================================================================
// constructor from three indices
// ============================================================================
LoKi::AParticles::AbsDeltaMass::AbsDeltaMass 
( const LHCb::ParticleID& m0     , 
  const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index1 , index2 , index3 ) ) 
  , LoKi::AParticles::DeltaMass ( m0 , index1 , index2 , index3 ) 
{}
// ============================================================================
// constructor from four indices 
// ============================================================================
LoKi::AParticles::AbsDeltaMass::AbsDeltaMass 
( const LHCb::ParticleID& m0     , 
  const std::size_t index1 , 
  const std::size_t index2 , 
  const std::size_t index3 ,
  const std::size_t index4 ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , index1 , index2 , index3 , index4 ) ) 
  , LoKi::AParticles::DeltaMass ( m0 , index1 , index2 , index3 , index4 ) 
{}
// ============================================================================
// constructor form the list of indices 
// ============================================================================
LoKi::AParticles::AbsDeltaMass::AbsDeltaMass 
( const LHCb::ParticleID& m0     , 
  const LoKi::AParticles::FourMomentum::Indices& indices ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , indices ) ) 
  , LoKi::AParticles::DeltaMass ( m0 , indices ) 
{}
// ============================================================================
// constructor from the four-momentum 
// ============================================================================
LoKi::AParticles::AbsDeltaMass::AbsDeltaMass 
( const LHCb::ParticleID& m0     , 
  const LoKi::AParticles::FourMomentum& four ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , four ) ) 
  , LoKi::AParticles::DeltaMass ( m0 , four ) 
{}
// ============================================================================




// ============================================================================
// the specific printout 
// ============================================================================
std::ostream& 
LoKi::AParticles::DeltaMass::fillStream( std::ostream& s ) const 
{
  s << "DAMASS(" << m0() << "," ;
  return print_ ( s , "" ) ;
}
// ============================================================================
// get the mass from particle property 
// ============================================================================
double LoKi::AParticles::DeltaMass::getMass 
( const LHCb::ParticleProperty& pp ) const
{ return pp.mass() ; }
// ============================================================================
// get the mass from particle name 
// ============================================================================
double LoKi::AParticles::DeltaMass::getMass 
( const std::string&           name , 
  LHCb::IParticlePropertySvc* svc  ) const
{
  if ( 0 != svc ) 
  {
    const LHCb::ParticleProperty* pp = svc->find ( name ) ;
    if ( 0 != pp ) { return pp -> mass() ; }
    Warning ( "ParticleProperty* points to null, try by name") ;
    return getMass ( name ) ;
  }
  const LHCb::ParticleProperty* pp = LoKi::Particles::ppFromName ( name ) ;
  Assert ( 0 != pp , "Invalid particle name!" ) ;
  //
  return pp->mass() ; 
}
// ============================================================================
// get the mass from particle name 
// ============================================================================
double LoKi::AParticles::DeltaMass::getMass 
( const LHCb::ParticleID& pid , 
  LHCb::IParticlePropertySvc*   svc  ) const
{
  if ( 0 != svc ) 
  {
    const LHCb::ParticleProperty* pp = svc->find ( pid ) ;
    if ( 0 != pp ) { return pp -> mass() ; }
    Warning ( "ParticleProperty* points to null, try by PID") ;
    return getMass ( pid ) ;
  }
  const LHCb::ParticleProperty* pp = LoKi::Particles::ppFromPID ( pid ) ;
  Assert ( 0 != pp , "Invalid particle ID!" ) ;
  //
  return pp->mass() ; 
}
// ============================================================================
// the specific printout 
// ============================================================================
std::ostream& 
LoKi::AParticles::AbsDeltaMass::fillStream( std::ostream& s ) const 
{
  s << "ADAMASS(" << m0() << "," ;
  return print_ ( s , "" ) ;  
}
// ============================================================================


// ============================================================================
// constructor from two indices and the tool 
// ============================================================================
LoKi::AParticles::DOCA::DOCA 
( const size_t               i1    , 
  const size_t               i2    , 
  const IDistanceCalculator* dc    , 
  const bool                 allow ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function()
  , m_eval ( i1 , i2 , dc , allow ) 
{}
// ============================================================================
// constructor from two indices and the tool 
// ============================================================================
LoKi::AParticles::DOCA::DOCA 
( const size_t                                i1    , 
  const size_t                                i2    , 
  const LoKi::Interface<IDistanceCalculator>& dc    ,
  const bool                                  allow ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function()
  , m_eval ( i1 , i2 , dc , allow ) 
{}
// ============================================================================
// constructor from two indices and the tool nickname
// ============================================================================
LoKi::AParticles::DOCA::DOCA 
( const size_t       i1    , 
  const size_t       i2    , 
  const std::string& nick  ,
  const bool         allow ) 
  : LoKi::AuxFunBase ( std::tie ( i1 , i2 , nick , allow ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function()
  , m_eval ( i1 , i2 , nick , allow ) 
{}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::AParticles::DOCA::result_type 
LoKi::AParticles::DOCA::operator() 
  ( LoKi::AParticles::DOCA::argument a ) const 
{
  const size_t s = a.size() ;
  
  if ( s < m_eval.firstIndex() || s < m_eval.secondIndex() ) 
  {
    Error("Invalid size, index out of range, return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // tool is valid? 
  if ( !tool() ) { loadTool() ; }
  
  // evaluate the result 
  return doca ( a[ m_eval.firstIndex()-1] , 
                a[ m_eval.secondIndex() -1] ) ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::AParticles::DOCA::fillStream ( std::ostream& s ) const 
{ return s << "ADOCA(" 
           << m_eval.firstIndex  () << "," 
           << m_eval.secondIndex () << ",'" 
           <<        toolName    () << "',"
           << ( allow() ? "True" : "False" )
           << "')" ; }
// ============================================================================


// ============================================================================
// constructor from two indices and the tool 
// ============================================================================
LoKi::AParticles::DOCAChi2::DOCAChi2
( const size_t               i1    , 
  const size_t               i2    , 
  const IDistanceCalculator* dc    ,
  const bool                 allow ) 
  : LoKi::AParticles::DOCA ( i1 , i2 , dc , allow ) 
{}
// ============================================================================
// constructor from two indices and the tool 
// ============================================================================
LoKi::AParticles::DOCAChi2::DOCAChi2 
( const size_t                                i1    , 
  const size_t                                i2    , 
  const LoKi::Interface<IDistanceCalculator>& dc    , 
  const bool                                  allow ) 
  : LoKi::AParticles::DOCA ( i1 , i2 , dc , allow ) 
{}
// ============================================================================
// constructor from two indices and the tool nickname
// ============================================================================
LoKi::AParticles::DOCAChi2::DOCAChi2 
( const size_t       i1    , 
  const size_t       i2    , 
  const std::string& nick  ,
  const bool         allow ) 
  : LoKi::AuxFunBase ( std::tie ( i1 , i2 , nick , allow ) )
  , LoKi::AParticles::DOCA ( i1 , i2 , nick , allow ) 
{}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::AParticles::DOCAChi2::result_type 
LoKi::AParticles::DOCAChi2::operator() 
  ( LoKi::AParticles::DOCAChi2::argument a ) const 
{
  const size_t s = a.size() ;
  if ( s < firstIndex() || s < secondIndex()  ) 
  {
    Error("Invalid size, index out of range, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  // tool is valid? 
  if ( !tool() ) { loadTool() ; }
  
  // evaluate the result 
  return chi2 ( a [ m_eval.firstIndex  () - 1 ] , 
                a [ m_eval.secondIndex () - 1 ] ) ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::AParticles::DOCAChi2::fillStream ( std::ostream& s ) const 
{ return s << "ADOCACHI2(" 
           << m_eval.firstIndex  () << "," 
           << m_eval.secondIndex () << ",'" 
           <<        toolName    () << "',"
           << ( allow() ? "True" : "False" )
           << "')" ; }
// ============================================================================


// ============================================================================
// constructor from the tool:
// ============================================================================
LoKi::AParticles::MinDOCA::MinDOCA
( const IDistanceCalculator*  doca  , 
  const bool                  allow ) 
  : LoKi::AParticles::DOCA ( 1 , 1 , doca , allow ) 
{}
// ============================================================================
// constructor from the tool:
// ============================================================================
LoKi::AParticles::MinDOCA::MinDOCA
( const LoKi::Interface<IDistanceCalculator>& doca  ,
  const bool                                  allow ) 
  : LoKi::AParticles::DOCA ( 1 , 1 , doca , allow ) 
{}
// ============================================================================
// constructor from the tool:
// ============================================================================
LoKi::AParticles::MinDOCA::MinDOCA
( const std::string& doca  ,
  const bool         allow ) 
  : LoKi::AuxFunBase ( std::tie ( doca, allow ) ) 
  , LoKi::AParticles::DOCA ( 1 , 1 , doca , allow ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::MinDOCA::result_type
LoKi::AParticles::MinDOCA::operator()
  ( LoKi::AParticles::MinDOCA::argument a ) const 
{
  //
  if ( !tool() ) { loadTool() ; }
  
  //
  double result = std::numeric_limits<double>::max() ;
  typedef LoKi::ATypes::Combination A ;
  for ( A::const_iterator i = a.begin() ; a.end() != i ; ++i )
  {
    for ( A::const_iterator j = i + 1 ; a.end() != j ; ++j )
    {
      const double dist = doca ( *j, *i ) ;
      /// get the minimum
      result = std::min ( result , dist ) ;              // get the minimum
    } 
  }
  return result ;
}
// ============================================================================
std::ostream& 
LoKi::AParticles::MinDOCA::fillStream ( std::ostream& s ) const 
{ return s << "AMINDOCA('" 
           << toolName()  << "',"
           << ( allow() ? "True" : "False" )
           << "')" ; }


// ============================================================================
/*  constructor 
 */
// ============================================================================
LoKi::AParticles::AllSameBestPV::AllSameBestPV()
   : AllSameBestPV (-1., -1., -1. )
{

}
// ============================================================================
/*  constructor 
 *  @param maxdist maximal distance      for two vertices to be considered as identical 
 *  @param maxchi2 maximal distance-chi2 for two vertices to be considered as identical
 *  @param maxfrac maximal fraction of common tracks 
 *                         for two vertices to be considered as identical
 *  - criteria are appied in "OR" mode
 *  - negative value means criterion is not applied 
 */
// ============================================================================
LoKi::AParticles::AllSameBestPV::AllSameBestPV
( const double maxdist , 
  const double maxchi2 , 
  const double maxfrac ) 
  : LoKi::AuxFunBase ( std::tie ( maxdist , maxchi2 , maxfrac ) )
  , LoKi::AuxDesktopBase()
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate ()
  , m_maxdist ( 0 <= maxdist ? maxdist : -1. ) 
  , m_maxchi2 ( 0 <= maxchi2 ? maxchi2 : -1. ) 
  , m_maxfrac ( 0 <= maxfrac ? maxfrac : -1. ) 
{
  if ( gaudi() && desktop() ) { checkReFit() ; }
}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::AParticles::AllSameBestPV::AllSameBestPV ( const AllSameBestPV& right)
  : AuxFunBase ( right ) 
  , LoKi::AuxDesktopBase( right )
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate ( right )
  , m_maxdist ( right.m_maxdist ) 
  , m_maxchi2 ( right.m_maxchi2 ) 
  , m_maxfrac ( right.m_maxfrac ) 
{
  if ( gaudi() && desktop() ) { checkReFit() ; }
}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::AParticles::AllSameBestPV::~AllSameBestPV(){}
// ============================================================================
void LoKi::AParticles::AllSameBestPV::checkReFit() const 
{
  SmartIF<IProperty> iprop ( getDesktop() )    ;
  BooleanProperty refit ( "ReFitPVs" , false ) ;
  if ( !iprop ) 
  { Warning ( "Unable to check property 'ReFitPVs'!" ) ; }
  else 
  {
    StatusCode sc = iprop->getProperty( &refit ) ;
    if        ( sc.isFailure () && 
                ( 0 < m_maxdist ||
                  0 < m_maxchi2 || 
                  0 < m_maxfrac  ) )
    { Warning ( "Extra algorithms are activated - not optimal...." ) ; }
    else if   ( refit.value  () 
                && m_maxdist <= 0 
                && m_maxchi2 <= 0 
                && m_maxfrac <= 0 )
    { Error   ( "Property 'ReFitPVs' is activated without "
                "additional algorithms!" ) ; }
    else if   ( refit.value  () )
    { Warning ( "Property 'ReFitPVs' is activated!"       ) ; }
    else if   ( !refit.value  () && 
                ( 0 < m_maxdist ||
                  0 < m_maxchi2 || 
                  0 < m_maxfrac  ) )
    { Warning ( "Extra algorithms are activated - not optimal.... "
                + std::to_string(m_maxdist) + std::string(" ")
                + std::to_string(m_maxchi2) + std::string(" ")
                + std::to_string(m_maxfrac) ) ; }
  } 
}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::AParticles::AllSameBestPV* 
LoKi::AParticles::AllSameBestPV::clone() const 
{ return new LoKi::AParticles::AllSameBestPV(*this) ; }
// ============================================================================
namespace 
{
  // ==========================================================================
  inline 
  const LHCb::RecVertex* recVertex 
  ( const LHCb::VertexBase* vb ) 
  { return 0 == vb ? nullptr : static_cast<const LHCb::RecVertex*> ( vb ) ; }
  // ==========================================================================
  inline 
  unsigned long  get_tracks 
  ( const LHCb::RecVertex*    rv   , 
    LHCb::Track::ConstVector& trks )
  {
    trks.clear() ;
    if ( 0 == rv || rv->tracks().empty() ) { return trks.size() ; }
    typedef LHCb::RecVertex::TrackWithWeightVector TWV ;
    TWV tw ( rv->tracksWithWeights() ) ;
    trks.reserve ( tw.size() ) ;
    for ( TWV::const_iterator it = tw.begin() ; tw.end() != it ; ++it ) 
    { if ( 0 != it->first && 0 < it->second ) { trks.push_back ( it->first ) ; } }
    std::stable_sort ( trks.begin() , trks.end()  ) ;
    //
    return trks.size() ;
  }
  // ==========================================================================
  inline bool good_for_pv ( const LHCb::Particle* p ) 
  {
    if ( 0 == p     ) { return false ; }
    //
    if ( p->isBasicParticle () ) 
    {
      const LHCb::ParticleID& pid = p->particleID() ;
      /// gamma and merged pi0 
      return  ( 22 != pid.abspid () ) && ( 111 != pid.abspid() ) ; // RETURN 
    }
    //
    typedef SmartRefVector<LHCb::Particle>  DAUGS ;
    const DAUGS& children = p->daughters() ;
    for ( DAUGS::const_iterator id = children.begin() ; children.end() != id ; ++id ) 
    { if ( good_for_pv ( *id ) ) { return true ; } }
    //
    return false ;
  }
  // ==========================================================================
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::AllSameBestPV::result_type
LoKi::AParticles::AllSameBestPV::operator()
  ( LoKi::AParticles::AllSameBestPV::argument v ) const
{
  // load the desktop if needed (and check for ReFitPVs option!)
  if ( !validDesktop() ) { loadDesktop() ; checkReFit() ;  }
  // check it!
  Assert ( validDesktop () , "No valid IPhysDesktop is found" );
  //
  // no way to get mulptiple PVS for single candidate 
  // no way to get mulptiple PVs for events with single PV 
  if ( v.size() <= 1 || primaryVertices().size() <= 1 ) { return true ; } // RETURN
  //
  // copy the paricles that COULD HAVE THE PROPER PV-association 
  // e.g. there is no sense to check PV for photons or any of photonic cases 
  LHCb::Particle::ConstVector vct ; vct.reserve( v.size() ) ;
  std::copy_if ( v.begin () , v.end ()         , 
                 std::back_inserter( vct )     ,
                 std::ptr_fun ( &good_for_pv ) ) ;
  if ( vct.size() != v.size() ) 
  { Warning ("Not all elements are good for PV-association!") ; }
  //
  if ( vct.size() <= 1 ) { return true ; }                               // RETURN
  //
  const LHCb::RecVertex* pv1 = recVertex ( bestVertex ( vct.front() ) ) ;
  if ( 0 == pv1 ) 
  {
    Warning ( "Invalid PV for the first element! return false" ) ;
    return false ;
  }
  ///
  LHCb::Track::ConstVector tracks1 ;
  if ( 0 < m_maxfrac ) { get_tracks ( pv1 , tracks1 ) ; }
  ///
  /// loop over all other elements:
  for ( LHCb::Particle::ConstVector::const_iterator j = vct.begin() + 1 ; vct.end() != j ; ++j ) 
  {
    const LHCb::VertexBase* vb = bestVertex ( *j ) ;
    if ( 0 == vb ) 
    {
      Warning ( "Invalid PV for the next element! return false" ) ;
      return false ;
    }
    // Check equality as pointers  
    if ( vb == pv1 ) { continue ; }  // equal by pointers
    //
    // Try other equality criteria 
    //
    // 1) check by distance 
    //
    if ( 0 < m_maxdist  && 
         ( vb->position() - pv1->position() ).Mag2() < m_maxdist * m_maxdist ) 
    { continue ; }                                                         // CONTINUE 
    //
    // 2) check by chi2 
    //
    if ( 0 < m_maxchi2 ) 
    {
      const LoKi::Vector3D       delta  = vb ->position () - pv1->position () ;
      const Gaudi::SymMatrix3x3& c1     = pv1->covMatrix() ;
      const Gaudi::SymMatrix3x3& c2     = vb ->covMatrix() ;
      // simple check before inversion of 3x3 matrix 
      if ( delta.x() * delta.x() < 2 * m_maxchi2 * ( c1(0,0) + c2(0,0) ) &&
           delta.y() * delta.y() < 2 * m_maxchi2 * ( c1(1,1) + c2(1,1) ) && 
           delta.z() * delta.z() < 2 * m_maxchi2 * ( c1(2,2) + c2(2,2) ) ) 
      {
        const Gaudi::SymMatrix3x3 c  = c1 + c2 ;        
        Gaudi::SymMatrix3x3       ci ;
        const int ifail = Gaudi::Math::inverse ( c , ci ) ;
        if ( 0 != ifail ) 
        {
          Warning ( "Enable to calculate chi2(vertex distance)! ignore element" ) ;
          continue ;                                                      // CONTINUE 
        }
        const double chi2 =  Gaudi::Math::Similarity ( ci , delta ) ;
        if ( chi2 < m_maxchi2 ) { continue ; }                               // CONTINUIE
      }
    }
    //
    // 3) check by the fraction of common tracks 
    //
    const LHCb::RecVertex* pv2 = recVertex ( vb ) ;
    if ( 0 < m_maxfrac && !tracks1.empty() && 0 != pv2 && !pv2->tracks().empty() ) 
    {
      //
      LHCb::Track::ConstVector tracks2 ;  
      get_tracks ( pv2 , tracks2 ) ;
      //
      const unsigned long nCommon = std::set_intersection 
        (  tracks1 . begin () , tracks1 . end () ,
           tracks2 . begin () , tracks2 . end () , 
           count_iterator<const LHCb::Track*>() ).count() ;
      if ( nCommon >= m_maxfrac * tracks1.size() || 
           nCommon >= m_maxfrac * tracks2.size() ) { continue ; }      // CONTINUE 
    }
    //
    // finally two vertices are different... 
    //
    return false ;                                    // RETURN 
  } // go to the next element in combinations 
  //
  return false ;                                      // RETURN 
}
// ============================================================================
// OPTIONAL: specific printout 
// ============================================================================
std::ostream& LoKi::AParticles::AllSameBestPV::fillStream( std::ostream& s ) const
{
  return 
    s << "AALLSAMEBPV_("
      << m_maxdist    << ","
      << m_maxchi2    << ","
      << m_maxfrac    << ")" ;  
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::AParticles::ACutV::ACutV
( const LoKi::Types::CutVals& cut ) 
  : LoKi::AuxFunBase ( std::tie ( cut ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_cut ( cut ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::AParticles::ACutV::~ACutV(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::AParticles::ACutV*
LoKi::AParticles::ACutV::clone() const 
{ return new LoKi::AParticles::ACutV(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::ACutV::result_type 
LoKi::AParticles::ACutV::operator()
  ( LoKi::AParticles::ACutV::argument a ) const 
{
  LHCb::Particle::ConstVector v ( a.begin() , a.end() ) ;
  return m_cut ( v ) ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::AParticles::ACutV::fillStream ( std::ostream& s ) const 
{ return s << " ACUTV( " << m_cut << " )" ; }


// ============================================================================
// constructor 
// ============================================================================
LoKi::AParticles::AFunV::AFunV
( const LoKi::Types::FunVals& fun ) 
  : LoKi::AuxFunBase ( std::tie ( fun ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun ( fun ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::AParticles::AFunV::~AFunV(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::AParticles::AFunV*
LoKi::AParticles::AFunV::clone() const 
{ return new LoKi::AParticles::AFunV(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::AFunV::result_type 
LoKi::AParticles::AFunV::operator()
  ( LoKi::AParticles::AFunV::argument a ) const 
{
  LHCb::Particle::ConstVector v ( a.begin() , a.end() ) ;
  return m_fun ( v ) ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::AParticles::AFunV::fillStream ( std::ostream& s ) const 
{ return s << " AFUNV( " << m_fun << " )" ; }
// ============================================================================


// ============================================================================
/*  constructor with the function and "max-value"
 *  @param eval (INPUT) the function to be evaluated
 *  @param cmpv (INPUT) the function to be maximazed 
 */
// ============================================================================
LoKi::AParticles::MaxVal::MaxVal 
( const LoKi::Types::Func& eval , 
  const LoKi::Types::Func& cmpv ,
  const LoKi::Types::Cuts& cuts , 
  const double             retv ) 
  : LoKi::AuxFunBase ( std::tie ( eval , cmpv , cuts , retv ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function()
  , m_eval ( eval ) 
  , m_cmpv ( cmpv ) 
  , m_cut  ( cuts )
  , m_retv ( retv ) 
{}
// ============================================================================
/*  constructor with the function and "max-value"
 *  @param eval (INPUT) the function to be evaluated
 *  @param cmpv (INPUT) the function to be maximazed 
 */
// ============================================================================
LoKi::AParticles::MaxVal::MaxVal 
( const LoKi::Types::Func& eval , 
  const LoKi::Types::Func& cmpv ,
  const LoKi::Types::Cuts& cuts )
  : LoKi::AuxFunBase ( std::tie ( eval , cmpv , cuts ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function()
  , m_eval ( eval ) 
  , m_cmpv ( cmpv ) 
  , m_cut  ( cuts )
  , m_retv ( LoKi::Constants::NegativeInfinity ) 
{}
// ============================================================================
/* constructor with the function and "max-value"
 *  @param eval the function to be evaluated
 *  @param cmpv the function to be maximazed 
 */
// ============================================================================
LoKi::AParticles::MaxVal::MaxVal 
( const LoKi::Types::Func& eval , 
  const LoKi::Types::Func& cmpv )
  : LoKi::AuxFunBase ( std::tie ( eval , cmpv ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function()
  , m_eval ( eval ) 
  , m_cmpv ( cmpv ) 
  , m_cut  ( LoKi::Constant<const LHCb::Particle*,bool> ( true ) )
  , m_retv ( LoKi::Constants::NegativeInfinity ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::AParticles::MaxVal::~MaxVal(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor" ) 
// ============================================================================
LoKi::AParticles::MaxVal*
LoKi::AParticles::MaxVal::clone() const 
{ return new LoKi::AParticles::MaxVal ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::MaxVal::result_type
LoKi::AParticles::MaxVal::operator()
  ( LoKi::AParticles::MaxVal::argument a ) const 
{
  //
  LoKi::ATypes::Combination::iterator ifind = 
    LoKi::Algs::select_max ( a.begin       () , 
                             a.end         () ,
                             m_cmpv . func () , 
                             m_cut  . func () ) ;
  //
  if ( a.end() == ifind ) 
  {
    Error ( "No elements are selected!" ) ;
    return m_retv ;
  }
  //
  return m_eval.fun ( *ifind )  ;
}
// ===========================================================================
// OPTIONAL: nice printout 
// ===========================================================================
std::ostream&  LoKi::AParticles::MaxVal::fillStream ( std::ostream& s ) const 
{
  //
  s << " aval_max( " << m_eval   
    << " , "         << m_cmpv 
    << " , "         << m_cut     ;
  if ( LoKi::Constants::NegativeInfinity != m_retv ) { s << " , " << m_retv ; }
  //
  return s << " )" ;  
}
// ============================================================================
/*  constructor with the function and "min-value"
 *  @param eval (INPUT) the function to be evaluated
 *  @param cmpv (INPUT) the function to be minimized
 */
// ============================================================================
LoKi::AParticles::MinVal::MinVal 
( const LoKi::Types::Func& eval , 
  const LoKi::Types::Func& cmpv ,
  const LoKi::Types::Cuts& cuts , 
  const double             retv ) 
  : LoKi::AuxFunBase ( std::tie ( eval , cmpv , cuts , retv ) ) 
  , LoKi::AParticles::MaxVal ( eval , cmpv , cuts , retv ) 
{}
// ============================================================================
/*  constructor with the function and "min-value"
 *  @param eval (INPUT) the function to be evaluated
 *  @param cmpv (INPUT) the function to be minimized
 */
// ============================================================================
LoKi::AParticles::MinVal::MinVal 
( const LoKi::Types::Func& eval , 
  const LoKi::Types::Func& cmpv ,
  const LoKi::Types::Cuts& cuts )
  : LoKi::AuxFunBase ( std::tie ( eval , cmpv , cuts ) ) 
  , LoKi::AParticles::MaxVal ( eval , cmpv , cuts , LoKi::Constants::PositiveInfinity ) 
{}
// ============================================================================
/* constructor with the function and "max-value"
 *  @param eval the function to be evaluated
 *  @param cmpv the function to be maximazed 
 */
// ============================================================================
LoKi::AParticles::MinVal::MinVal 
( const LoKi::Types::Func& eval , 
  const LoKi::Types::Func& cmpv )
  : LoKi::AuxFunBase ( std::tie ( eval , cmpv ) ) 
  , LoKi::AParticles::MaxVal 
( eval , 
  cmpv ,
  LoKi::Constant<const LHCb::Particle*,bool> ( true ) ,
  LoKi::Constants::PositiveInfinity                   ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::AParticles::MinVal::~MinVal(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor" ) 
// ============================================================================
LoKi::AParticles::MinVal*
LoKi::AParticles::MinVal::clone() const 
{ return new LoKi::AParticles::MinVal ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::MinVal::result_type
LoKi::AParticles::MinVal::operator()
  ( LoKi::AParticles::MaxVal::argument a ) const 
{
  //
  LoKi::ATypes::Combination::iterator ifind = 
    LoKi::Algs::select_min ( a.begin       () , 
                             a.end         () ,
                             m_cmpv . func () , 
                             m_cut  . func () ) ;
  //
  if ( a.end() == ifind ) 
  {
    Error ( "No elements are selected!" ) ;
    return m_retv ;
  }
  //
  return m_eval.fun ( *ifind )  ;
}
// ===========================================================================
// OPTIONAL: nice printout 
// ===========================================================================
std::ostream&  LoKi::AParticles::MinVal::fillStream ( std::ostream& s ) const 
{
  //
  s << " aval_min( " << m_eval   
    << " , "         << m_cmpv 
    << " , "         << m_cut     ;
  if ( LoKi::Constants::PositiveInfinity != m_retv ) { s << " , " << m_retv ; }
  //
  return s << " )" ;  
}
// ============================================================================

// ============================================================================
// constructor from the functor 
// ============================================================================
LoKi::AParticles::Sum::Sum 
( const LoKi::Types::Func& fun , 
  const double             ini ) 
  : LoKi::AuxFunBase ( std::tie ( fun , ini ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun     ( fun  ) 
  , m_cut     (  LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true ) ) 
  , m_ini     ( ini  )
  , m_trivial ( true ) 
{}
// ============================================================================
// constructor from the functor & predicate 
// ============================================================================
LoKi::AParticles::Sum::Sum 
( const LoKi::Types::Func& fun , 
  const LoKi::Types::Cuts& cut , 
  const double             ini ) 
  : LoKi::AuxFunBase ( std::tie ( fun , cut , ini ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun     ( fun   ) 
  , m_cut     ( cut   ) 
  , m_ini     ( ini   )
  , m_trivial ( false ) 
{}
// ============================================================================
// constructor from the functor & predicate 
// ============================================================================
LoKi::AParticles::Sum::Sum 
( const LoKi::Types::Cuts& cut , 
  const LoKi::Types::Func& fun , 
  const double             ini ) 
  : LoKi::AuxFunBase ( std::tie ( cut , fun , ini ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun     ( fun   ) 
  , m_cut     ( cut   ) 
  , m_ini     ( ini   )
  , m_trivial ( false ) 
{}
// ============================================================================
// MANDATORY: virtual descructor 
// ============================================================================
LoKi::AParticles::Sum::~Sum(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::AParticles::Sum*
LoKi::AParticles::Sum::clone() const
{ return new LoKi::AParticles::Sum ( *this ) ;}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::Sum::result_type 
LoKi::AParticles::Sum::operator() 
  ( LoKi::AParticles::Sum::argument a ) const
{
  //
  double result = m_ini ;
  //
  return 
    m_trivial ? 
    LoKi::Algs::accumulate ( a. begin ()         , 
                             a. end   ()         , 
                             m_fun               , 
                             result              , 
                             std::plus<double>() ) :  
    LoKi::Algs::accumulate ( a. begin ()         , 
                             a. end   ()         , 
                             m_fun               , 
                             m_cut               , 
                             result              , 
                             std::plus<double>() ) ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::AParticles::Sum::fillStream ( std::ostream& s ) const 
{
  s << " ASUM(" << m_fun ;
  if ( !m_trivial  ) { s << "," << m_cut ; }
  if ( 0 != m_ini  ) { s << "," << m_ini ; }
  return s << ") ";
}
// ============================================================================

// ============================================================================
// calculate the overlap between two daughters 
// ============================================================================
LoKi::AParticles::Overlap::Overlap 
( const unsigned short        i1   , 
  const unsigned short        i2   ) 
  : LoKi::AuxFunBase ( std::tie ( i1 , i2 ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function() 
  , m_i1   ( i1   ) 
  , m_i2   ( i2   ) 
  , m_good ( 0    ) 
  , m_type ( LHCb::LHCbID::channelIDtype::Velo ) 
  , m_def  ( true ) 
{}
// ============================================================================
// calculate the overlap between two daughters 
// ============================================================================
LoKi::AParticles::Overlap::Overlap 
( const unsigned short           i1   , 
  const unsigned short           i2   , 
  LoKi::AParticles::Overlap::PMF good ) 
  : LoKi::AuxFunBase   () 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function() 
  , m_i1   ( i1    ) 
  , m_i2   ( i2    ) 
  , m_good ( good  ) 
  , m_type ( LHCb::LHCbID::channelIDtype::Velo ) 
  , m_def  ( false ) 
{
  Assert ( m_good , "Invalid pointer to member function!" ) ;
}
// ============================================================================
// calculate the overlap between two daughters 
// ============================================================================
namespace 
{
  LoKi::StrKeep _type_to_str_ ( LHCb::LHCbID::channelIDtype type )
  {
    std::ostringstream s;
    s << "LHCb::LHCbID::" << type ;
    return LoKi::StrKeep ( s.str() ) ;
  }
}
// ============================================================================
LoKi::AParticles::Overlap::Overlap 
( const unsigned short          i1   , 
  const unsigned short          i2   , 
  LHCb::LHCbID::channelIDtype   type ) 
  : LoKi::AuxFunBase ( std::make_tuple ( i1 ,i2 , _type_to_str_ ( type ) ) ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function() 
  , m_i1   ( i1    ) 
  , m_i2   ( i2    ) 
  , m_good ( 0     ) 
  , m_type ( type  ) 
  , m_def  ( false ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::AParticles::Overlap::~Overlap (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::AParticles::Overlap* 
LoKi::AParticles::Overlap::clone() const 
{ return new LoKi::AParticles::Overlap ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::Overlap::result_type 
LoKi::AParticles::Overlap::operator() 
  ( LoKi::AParticles::Overlap::argument a ) const 
{
  //
  if ( a.size() < m_i1 ) 
  {
    Error("Invalid index for the first daughter, return -2 ") ;
    return -2 ;
  }
  //
  const LHCb::Particle* d1 = a[m_i1 - 1 ] ;
  if ( 0 == d1 ) 
  {
    Error("Invalid first daughter, return -1 ") ;
    return -1 ;
  }
  //
  if ( a.size() < m_i2 ) 
  {
    Error("Invalid index for the second daughter, return -2 ") ;
    return -2 ;
  }
  //
  const LHCb::Particle* d2 = a[m_i2 - 1 ] ;
  if ( 0 == d2 ) 
  {
    Error("Invalid second daughter, return -1 ") ;
    return -1 ;
  }
  //
  //PMF good = m_good ;
  std::pair<double,double> result = 
    m_def  ? 
    LHCb::HashIDs::overlap ( d1 , d2          ) : 
    m_good ? 
    LHCb::HashIDs::overlap ( d1 , d2 , m_good ) : 
    LHCb::HashIDs::overlap ( d1 , d2 , m_type ) ;
  //
  return std::max ( result.first , result.second ) ;
} 
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::AParticles::Overlap::fillStream ( std::ostream& s ) const 
{
  s << " AOVERLAP("
    << m_i1 << ","
    << m_i1 ;
  if ( m_def  ) { return s << ") "              ; }
  if ( m_good ) { return s << ", <func-here>) " ; }
  return s << "," << m_type << ")" ;   
}

 

// ============================================================================
// The END 
// ============================================================================
