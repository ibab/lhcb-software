// $Id: AParticles.cpp,v 1.5 2007-12-02 17:10:46 ibelyaev Exp $
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
  StatusCode sc = m_fit -> fit ( a.begin() , a.end() , vertex ) ;
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
LoKi::AParticles::MaxDOCA::MaxDOCA ( IGeomDispCalculator*  doca  ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_doca ( doca ) 
{}
// ============================================================================
// constructor from the tool:L
// ============================================================================
LoKi::AParticles::MaxDOCA::MaxDOCA
( const LoKi::Interface<IGeomDispCalculator>& doca  ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_doca ( doca ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::AParticles::MaxDOCA::MaxDOCA
( const LoKi::AParticles::MaxDOCA& right ) 
  : LoKi::AuxFunBase                                         ( right ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function ( right ) 
  , m_doca ( right.m_doca ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::MaxDOCA::result_type
LoKi::AParticles::MaxDOCA::operator()
  ( LoKi::AParticles::MaxDOCA::argument a ) const 
{
  Assert ( m_doca.validPointer() , 
           "Invalid pointer to IGeomDispCalculator tool" ) ;
  if ( 2 > a.size() )
  {
    Error ( "Invalid number of daughters, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;
  }
  double result = -1 * std::numeric_limits<double>::max() ;
  typedef LoKi::ATypes::Combination A ;
  for ( A::const_iterator i = a.begin() ; a.end() != i ; ++i )
  {
    for ( A::const_iterator j = i + 1 ; a.end() != j ; ++j )
    {
      double doca    = 0.0 ;
      double docaErr = 0.0 ;
      StatusCode sc = m_doca->calcCloseAppr ( **i , **j , doca , docaErr ) ;
      if ( sc.isFailure() )
      {
        Warning ( "Error from IGeomDispCalculator, skip", sc ) ;
        continue ;                                                // CONTINUE 
      }
      // get the maximum
      result = std::max ( result , doca ) ;              ///< get the maximum
    } 
  }
  return result ;
}
// ============================================================================
std::ostream& 
LoKi::AParticles::MaxDOCA::fillStream ( std::ostream& s ) const 
{ return s << "AMAXDOCA['" << m_doca->name() << "']" ; }
// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCACut::MaxDOCACut 
( IGeomDispCalculator*  doca      ,
  const double          threshold )
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_doca      ( doca      ) 
  , m_threshold ( threshold )
{}
// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCACut::MaxDOCACut 
( const double          threshold ,
  IGeomDispCalculator*  doca      ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_doca      ( doca      ) 
  , m_threshold ( threshold )
{}
// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCACut::MaxDOCACut
( const LoKi::Interface<IGeomDispCalculator>& doca ,
  const double          threshold )
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_doca      ( doca      ) 
  , m_threshold ( threshold )
{}
// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCACut::MaxDOCACut
( const double                           threshold ,
  const LoKi::Interface<IGeomDispCalculator>& doca )
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_doca      ( doca      ) 
  , m_threshold ( threshold )
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::AParticles::MaxDOCACut::MaxDOCACut
( const LoKi::AParticles::MaxDOCACut& right ) 
  : LoKi::AuxFunBase                                          ( right ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate ( right ) 
  , m_doca ( right.m_doca ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::MaxDOCACut::result_type
LoKi::AParticles::MaxDOCACut::operator()
  ( LoKi::AParticles::MaxDOCACut::argument a ) const 
{
  Assert ( m_doca.validPointer() , 
           "Invalid pointer to IGeomDispCalculator tool" ) ;
  if ( 2 > a.size() )
  {
    Error ( "Invalid number of daughters, return false" ) ;
    return false ;
  }
  typedef LoKi::ATypes::Combination A ;
  for ( A::const_iterator i = a.begin() ; a.end() != i ; ++i )
  {
    for ( A::const_iterator j = i + 1 ; a.end() != j ; ++j )
    {
      double doca    = 0.0 ;
      double docaErr = 0.0 ;
      StatusCode sc = m_doca->calcCloseAppr ( **i , **j , doca , docaErr ) ;
      if ( sc.isFailure() )
      {
        Warning ( "Error from IGeomDispCalculator, skip", sc ) ;
        continue ;                                                // CONTINUE 
      }
      ///
      if ( doca > m_threshold ) { return false ; }                // RETURN      
    } 
  }
  return true ;
}
// ============================================================================
std::ostream& 
LoKi::AParticles::MaxDOCACut::fillStream ( std::ostream& s ) const 
{ return s << "ACUTDOCA['" << m_doca->name() << "," << m_threshold << "']" ; }
// ============================================================================
// constructor from the tool:
// ============================================================================
LoKi::AParticles::MaxDOCAChi2::MaxDOCAChi2 ( IGeomDispCalculator*  doca  ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_doca ( doca ) 
{}
// ============================================================================
// constructor from the tool:L
// ============================================================================
LoKi::AParticles::MaxDOCAChi2::MaxDOCAChi2
( const LoKi::Interface<IGeomDispCalculator>& doca  ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
  , m_doca ( doca ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::AParticles::MaxDOCAChi2::MaxDOCAChi2
( const LoKi::AParticles::MaxDOCAChi2& right ) 
  : LoKi::AuxFunBase                                         ( right ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function ( right ) 
  , m_doca ( right.m_doca ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::MaxDOCAChi2::result_type
LoKi::AParticles::MaxDOCAChi2::operator()
  ( LoKi::AParticles::MaxDOCAChi2::argument a ) const 
{
  Assert ( m_doca.validPointer() , 
           "Invalid pointer to IGeomDispCalculator tool" ) ;
  if ( 2 > a.size() )
  {
    Error ( "Invalid number of daughters, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;
  }
  double result = -1 * std::numeric_limits<double>::max() ;
  typedef LoKi::ATypes::Combination A ;
  for ( A::const_iterator i = a.begin() ; a.end() != i ; ++i )
  {
    for ( A::const_iterator j = i + 1 ; a.end() != j ; ++j )
    {
      double doca    = 0.0 ;
      double docaErr = 0.0 ;
      StatusCode sc = m_doca->calcCloseAppr ( **i , **j , doca , docaErr ) ;
      if ( sc.isFailure() )
      {
        Warning ( "Error from IGeomDispCalculator, skip", sc ) ;
        continue ;                                                // CONTINUE 
      }
      // get the maximum
      const double chi = doca/docaErr ;
      result = std::max ( result , chi*chi ) ;         ///< get the maximum
    } 
  }
  return result ;
}
// ============================================================================
std::ostream& 
LoKi::AParticles::MaxDOCAChi2::fillStream ( std::ostream& s ) const 
{ return s << "ADOCACHI2['" << m_doca->name() << "']" ; }


// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCAChi2Cut::MaxDOCAChi2Cut 
( IGeomDispCalculator*  doca      ,
  const double          threshold )
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_doca      ( doca      ) 
  , m_threshold ( threshold )
{}
// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCAChi2Cut::MaxDOCAChi2Cut 
( const double          threshold ,
  IGeomDispCalculator*  doca      ) 
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_doca      ( doca      ) 
  , m_threshold ( threshold )
{}
// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCAChi2Cut::MaxDOCAChi2Cut
( const LoKi::Interface<IGeomDispCalculator>& doca ,
  const double          threshold )
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_doca      ( doca      ) 
  , m_threshold ( threshold )
{}
// ============================================================================
// constructor from the tool and threshold 
// ============================================================================
LoKi::AParticles::MaxDOCAChi2Cut::MaxDOCAChi2Cut
( const double                           threshold ,
  const LoKi::Interface<IGeomDispCalculator>& doca )
  : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate () 
  , m_doca      ( doca      ) 
  , m_threshold ( threshold )
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::AParticles::MaxDOCAChi2Cut::MaxDOCAChi2Cut
( const LoKi::AParticles::MaxDOCAChi2Cut& right ) 
  : LoKi::AuxFunBase                                          ( right ) 
  , LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate ( right ) 
  , m_doca ( right.m_doca ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::AParticles::MaxDOCAChi2Cut::result_type
LoKi::AParticles::MaxDOCAChi2Cut::operator()
  ( LoKi::AParticles::MaxDOCAChi2Cut::argument a ) const 
{
  Assert ( m_doca.validPointer() , 
           "Invalid pointer to IGeomDispCalculator tool" ) ;
  if ( 2 > a.size() )
  {
    Error ( "Invalid number of daughters, return false" ) ;
    return false ;
  }
  typedef LoKi::ATypes::Combination A ;
  for ( A::const_iterator i = a.begin() ; a.end() != i ; ++i )
  {
    for ( A::const_iterator j = i + 1 ; a.end() != j ; ++j )
    {
      double doca    = 0.0 ;
      double docaErr = 0.0 ;
      StatusCode sc = m_doca->calcCloseAppr ( **i , **j , doca , docaErr ) ;
      if ( sc.isFailure() )
      {
        Warning ( "Error from IGeomDispCalculator, skip", sc ) ;
        continue ;                                                // CONTINUE 
      }
      ///
      double chi = doca / docaErr ;
      if ( chi*chi > m_threshold ) { return false ; }             // RETURN      
    } 
  }
  return true ;
}
// ============================================================================
std::ostream& 
LoKi::AParticles::MaxDOCAChi2Cut::fillStream ( std::ostream& s ) const 
{ return s << "ACUTDOCACHI2['" << m_doca->name() 
           << "," << m_threshold << "']" ; }
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
// The END 
// ============================================================================
