// $Id: AParticles.cpp,v 1.13 2009-11-30 11:17:38 ibelyaev Exp $
// ============================================================================
// Include files 
// ===========================================================================
// STD & STL 
// ===========================================================================
#include <climits>
#include <algorithm>
#include <functional>
// ===========================================================================
// GaudiKernel
// ===========================================================================
#include "GaudiKernel/ToStream.h"
// ===========================================================================
// PartProp
// ===========================================================================
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
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
// Constructor from the function
// ============================================================================
LoKi::AParticles::MinChild::MinChild 
( const LoKi::PhysTypes::Func& fun  )
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_fun   ( fun ) 
  , m_index ( index ) 
{}
// ===========================================================================
// Constructor from the fuction and index 
// ===========================================================================
LoKi::AParticles::ChildFun::ChildFun
( const int                    index ,
  const LoKi::PhysTypes::Func& fun   )
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_cut   ( cut ) 
  , m_index ( index ) 
{}
// ===========================================================================
// Constructor from the fuction and index 
// ===========================================================================
LoKi::AParticles::ChildCut::ChildCut
( const int                    index ,
  const LoKi::PhysTypes::Cuts& cut   )
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
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
LoKi::AParticles::MaxDOCA::MaxDOCA ( const IDistanceCalculator*  doca  ) 
 : LoKi::AParticles::MaxDOCA::DOCA ( 1 , 1 , doca ) 
{}
// ============================================================================
// constructor from the tool:
// ============================================================================
LoKi::AParticles::MaxDOCA::MaxDOCA
( const LoKi::Interface<IDistanceCalculator>& doca  ) 
 : LoKi::AParticles::MaxDOCA::DOCA ( 1 , 1 , doca ) 
{}
// ============================================================================
// constructor from the tool:
// ============================================================================
LoKi::AParticles::MaxDOCA::MaxDOCA
( const std::string& doca  ) 
  : LoKi::AParticles::MaxDOCA::DOCA ( 1 , 1 , doca ) 
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
{ return s << "AMAXDOCA('" << toolName() << "')" ; }


// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCACut::MaxDOCACut 
( const double          threshold ,
  const IDistanceCalculator*  doca      ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_doca      ( 1 , 1 , doca ) 
  , m_threshold ( threshold )
{}
// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCACut::MaxDOCACut
( const double                           threshold ,
  const LoKi::Interface<IDistanceCalculator>& doca )
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_doca      ( 1 , 1 , doca ) 
  , m_threshold ( threshold )
{}
// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCACut::MaxDOCACut
( const double       threshold ,
  const std::string& doca      )
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_doca      ( 1 , 1 , doca ) 
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
           << toolName() << "')" ; }

// ============================================================================
// constructor from the tool:
// ============================================================================
LoKi::AParticles::MaxDOCAChi2::MaxDOCAChi2 
( const IDistanceCalculator*  doca  ) 
  : LoKi::AParticles::DOCAChi2 ( 1 , 1 , doca ) 
{}
// ============================================================================
// constructor from the tool:L
// ============================================================================
LoKi::AParticles::MaxDOCAChi2::MaxDOCAChi2
( const LoKi::Interface<IDistanceCalculator>& doca  ) 
  : LoKi::AParticles::DOCAChi2 ( 1 , 1 , doca ) 
{}
// ============================================================================
// constructor from the tool:L
// ============================================================================
LoKi::AParticles::MaxDOCAChi2::MaxDOCAChi2
( const std::string& doca  ) 
  : LoKi::AParticles::DOCAChi2 ( 1 , 1 , doca ) 
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
{ return s << "AMAXDOCACHI2('" << toolName () << "')" ; }


// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCAChi2Cut::MaxDOCAChi2Cut 
( const double          threshold ,
  const IDistanceCalculator*  doca      ) 
  : LoKi::AParticles::MaxDOCACut ( threshold , doca ) 
{}
// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCAChi2Cut::MaxDOCAChi2Cut
( const double                           threshold ,
  const LoKi::Interface<IDistanceCalculator>& doca )
  : LoKi::AParticles::MaxDOCACut ( threshold , doca ) 
{}
// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCAChi2Cut::MaxDOCAChi2Cut
( const double                           threshold ,
  const std::string& doca )
  : LoKi::AParticles::MaxDOCACut ( threshold , doca ) 
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
           << toolName ()  << "')" ; }
// ============================================================================
/*  constructor with daughter index (starts from 1).
 *  E.g. for 2-body decays it could be 1 or 2 
 *  @param index of daughter particles
 */
// ============================================================================
LoKi::AParticles::DecayAngle::DecayAngle
( const size_t child ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
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
// constructor from two masses 
// ============================================================================
LoKi::AParticles::WrongMass:: WrongMass  
( const double m1 , 
  const double m2 ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_masses () 
{
  m_masses.push_back ( m1 ) ;
  m_masses.push_back ( m2 ) ;  
}
// ============================================================================
// constructor from three masses 
// ============================================================================
LoKi::AParticles::WrongMass:: WrongMass 
( const double m1 , 
  const double m2 , 
  const double m3 ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_masses () 
{
  m_masses.push_back ( m1 ) ;
  m_masses.push_back ( m2 ) ;  
  m_masses.push_back ( m3 ) ;  
}
// ============================================================================
// constructor from four masses 
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass 
( const double m1 , 
  const double m2 , 
  const double m3 , 
  const double m4 ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_masses () 
{
  m_masses.push_back ( m1 ) ;
  m_masses.push_back ( m2 ) ;  
  m_masses.push_back ( m3 ) ;
  m_masses.push_back ( m4 ) ;  
}
// ============================================================================
// constructor from the vector of masses 
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass 
( const std::vector<double>& masses ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_masses ( masses ) 
{}
// ============================================================================
// constructor from two pids 
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass 
( const LHCb::ParticleID& pid1 , 
  const LHCb::ParticleID& pid2 ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_masses () 
{
  m_masses.push_back ( LoKi::Particles::massFromPID ( pid1 ) ) ;
  m_masses.push_back ( LoKi::Particles::massFromPID ( pid2 ) ) ;
}
// ============================================================================
// constructor from three pids 
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass
( const LHCb::ParticleID& pid1 , 
  const LHCb::ParticleID& pid2 ,
  const LHCb::ParticleID& pid3 ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_masses () 
{
  m_masses.push_back ( LoKi::Particles::massFromPID ( pid1 ) ) ;
  m_masses.push_back ( LoKi::Particles::massFromPID ( pid2 ) ) ;
  m_masses.push_back ( LoKi::Particles::massFromPID ( pid3 ) ) ;
}
// ============================================================================
// constructor from four pids 
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass  
( const LHCb::ParticleID& pid1 , 
  const LHCb::ParticleID& pid2 ,
  const LHCb::ParticleID& pid3 ,
  const LHCb::ParticleID& pid4 ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_masses () 
{
  m_masses.push_back ( LoKi::Particles::massFromPID ( pid1 ) ) ;
  m_masses.push_back ( LoKi::Particles::massFromPID ( pid2 ) ) ;
  m_masses.push_back ( LoKi::Particles::massFromPID ( pid3 ) ) ;
  m_masses.push_back ( LoKi::Particles::massFromPID ( pid4 ) ) ;
}
// ============================================================================
// constructor from the vector of pids  
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass
( const std::vector<LHCb::ParticleID>& pids ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_masses ( pids.size() ) 
{
  std::transform 
    ( pids.begin     () , 
      pids.end       () , 
      m_masses.begin () , 
      std::ptr_fun(&LoKi::Particles::massFromPID) ) ;
}
// ============================================================================
// constructor from two names  
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass
( const std::string& name1 , 
  const std::string& name2 ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_masses () 
{
  m_masses.push_back ( LoKi::Particles::massFromName ( name1 ) ) ;
  m_masses.push_back ( LoKi::Particles::massFromName ( name2 ) ) ;
}
// ============================================================================
// constructor from three names  
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_masses () 
{
  m_masses.push_back ( LoKi::Particles::massFromName ( name1 ) ) ;
  m_masses.push_back ( LoKi::Particles::massFromName ( name2 ) ) ;
  m_masses.push_back ( LoKi::Particles::massFromName ( name3 ) ) ;
}
// ============================================================================
// constructor from four  names  
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass 
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ,
  const std::string& name4 ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_masses () 
{
  m_masses.push_back ( LoKi::Particles::massFromName ( name1 ) ) ;
  m_masses.push_back ( LoKi::Particles::massFromName ( name2 ) ) ;
  m_masses.push_back ( LoKi::Particles::massFromName ( name3 ) ) ;
  m_masses.push_back ( LoKi::Particles::massFromName ( name4 ) ) ;
}
// ============================================================================
// constructor from the vector of names 
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass 
( const std::vector<std::string>& names ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_masses ( names.size() ) 
{
  std::transform 
    ( names.begin    () , 
      names.end      () , 
      m_masses.begin () , 
      std::ptr_fun(&LoKi::Particles::massFromName) ) ;
}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::AParticles::WrongMass::WrongMass 
( const LoKi::AParticles::WrongMass& right )
  : LoKi::AuxFunBase                                         ( right ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function ( right ) 
  , m_masses ( right.m_masses ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::WrongMass::result_type 
LoKi::AParticles::WrongMass::operator() 
  ( LoKi::AParticles::WrongMass::argument a ) const 
{
  if ( m_masses.size() != a.size() ) 
  {
    Error ( "Mismatch in contaienr size(s), return InvaidMass" ) ;
    return LoKi::Constants::InvalidMass ;
  }
  // evaluate the mass 
  LoKi::LorentzVector vct = 
    LoKi::Kinematics::wrongMass 
    ( a.begin() , a.end() , m_masses.begin() , LoKi::Objects::_VALID_ ) ;
  //
  return vct.M() ;
}
// ============================================================================
// OPTIONAL: specific printout 
// ============================================================================
std::ostream& 
LoKi::AParticles::WrongMass::fillStream( std::ostream& s ) const 
{
  s << "AWM(" ;
  if ( 4 < m_masses.size() )
  { s << Gaudi::Utils::toStream ( m_masses , s ) ; }
  else 
  {
    for ( std::vector<double>::const_iterator im = m_masses.begin() ;
          m_masses.end() != im ; ++im ) 
    {
      if ( m_masses.begin() != im ) { s << " , " ; }
      s <<  (*im) ;
    }
  }
  return s << ")" ;
}
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
( const size_t               i1 , 
  const size_t               i2 , 
  const IDistanceCalculator* dc ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function()
  , m_eval ( i1 , i2 , dc ) 
{}
// ============================================================================
// constructor from two indices and the tool 
// ============================================================================
LoKi::AParticles::DOCA::DOCA 
( const size_t                                i1 , 
  const size_t                                i2 , 
  const LoKi::Interface<IDistanceCalculator>& dc ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function()
  , m_eval ( i1 , i2 , dc ) 
{}
// ============================================================================
// constructor from two indices and the tool nickname
// ============================================================================
LoKi::AParticles::DOCA::DOCA 
( const size_t       i1 , 
  const size_t       i2 , 
  const std::string& nick ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function()
  , m_eval ( i1 , i2 , nick ) 
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
           <<        toolName    () << "')" ; }
// ============================================================================


// ============================================================================
// constructor from two indices and the tool 
// ============================================================================
LoKi::AParticles::DOCAChi2::DOCAChi2
( const size_t               i1 , 
  const size_t               i2 , 
  const IDistanceCalculator* dc ) 
  : LoKi::AParticles::DOCA ( i1 , i2 , dc ) 
{}
// ============================================================================
// constructor from two indices and the tool 
// ============================================================================
LoKi::AParticles::DOCAChi2::DOCAChi2 
( const size_t                                i1 , 
  const size_t                                i2 , 
  const LoKi::Interface<IDistanceCalculator>& dc ) 
  : LoKi::AParticles::DOCA ( i1 , i2 , dc ) 
{}
// ============================================================================
// constructor from two indices and the tool nickname
// ============================================================================
LoKi::AParticles::DOCAChi2::DOCAChi2 
( const size_t       i1 , 
  const size_t       i2 , 
  const std::string& nick ) 
  : LoKi::AParticles::DOCA ( i1 , i2 , nick ) 
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
           <<        toolName    () << "')" ; }
// ============================================================================


// ============================================================================
// constructor from the tool:
// ============================================================================
LoKi::AParticles::MinDOCA::MinDOCA ( const IDistanceCalculator*  doca  ) 
  : LoKi::AParticles::DOCA ( 1 , 1 , doca ) 
{}
// ============================================================================
// constructor from the tool:
// ============================================================================
LoKi::AParticles::MinDOCA::MinDOCA
( const LoKi::Interface<IDistanceCalculator>& doca  ) 
  : LoKi::AParticles::DOCA ( 1 , 1 , doca ) 
{}
// ============================================================================
// constructor from the tool:
// ============================================================================
LoKi::AParticles::MinDOCA::MinDOCA
( const std::string& doca  ) 
  : LoKi::AParticles::DOCA ( 1 , 1 , doca ) 
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
{ return s << "AMINDOCA('" << toolName() << "')" ; }


// ============================================================================
// the default constructor (creates the object in invalid state)
// ============================================================================
LoKi::AParticles::AllSameBestPV::AllSameBestPV()
  : LoKi::AuxDesktopBase()
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate ()
{}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::AParticles::AllSameBestPV::AllSameBestPV ( const AllSameBestPV& right)
  : AuxFunBase ( right ) 
  , LoKi::AuxDesktopBase( right )
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate ( right ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::AllSameBestPV::result_type
LoKi::AParticles::AllSameBestPV::operator()
  ( LoKi::AParticles::AllSameBestPV::argument v ) const
{
  // load the desktop if needed
  if ( !validDesktop() ) { loadDesktop() ; }
  // check it!
  Assert ( validDesktop () , "No valid IPhysDesktop is found" );

  // Get key for the related PV of each daughter.
  // Key comparison assumes that all related PVs come from the same container.
  bool havePVs = true;
  std::vector< LHCb::VertexBase::key_type > pvkeys;
  // Is there a way to derive this iterator type from the argument or base
  //   class type?
  LoKi::ATypes::Combination::const_iterator iter;
  for( iter = v.begin(); iter != v.end(); ++iter )
  {
    // Does the desktop issue a warning if no PV is found, or should a
    //   suppressable warning be included here?
    const LHCb::VertexBase *relPV = desktop()->relatedVertex( *iter );
    if( relPV )
      pvkeys.push_back(relPV->key());
    else
      havePVs = false;
  }

  // Using sort and unique are unnecessary overhead here, but might be more
  //   useful in a more general functor that would return the number of
  //   unique best pv, but would that be useful?
  std::sort(pvkeys.begin(), pvkeys.end());
  std::vector< LHCb::Vertex::key_type >::iterator
        enduniq = std::unique(pvkeys.begin(), pvkeys.end());

  // Return true if all have the same related PV
  return havePVs && (std::distance(pvkeys.begin(), enduniq) == 1);
}
// ============================================================================


// ============================================================================
// constructor 
// ============================================================================
LoKi::AParticles::ACutV::ACutV
( const LoKi::Types::CutVals& cut ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function()
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function()
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
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function()
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
  : LoKi::AParticles::MaxVal ( eval , cmpv , cuts , retv ) 
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
  : LoKi::AParticles::MaxVal ( eval , cmpv , cuts , LoKi::Constants::PositiveInfinity ) 
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
  : LoKi::AParticles::MaxVal 
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
// The END 
// ============================================================================
