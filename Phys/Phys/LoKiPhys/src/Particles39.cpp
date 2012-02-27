// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
// ============================================================================
// LoKiCode
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Algs.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/PhysKinematics.h"
#include "LoKi/Particles39.h"
#include "LoKi/PhysSources.h"
// ============================================================================
// Boost.Lambda
// ============================================================================
#include "boost/lambda/bind.hpp"
// ============================================================================
/** @file
 *  Implementation file for functions form file LoKi/Particles39.h
 *  
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-17
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 *
 */
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::MinMaxDistance::MinMaxDistance
( const bool                                 minmax ,  
  const LHCb::Particle::Range&               parts  ,
  LoKi::Particles::MinMaxDistance::dist_func dist   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , LoKi::UniqueKeeper<LHCb::Particle> ( parts.begin () , parts.end () )
  , m_minimum  ( minmax ) 
  , m_distance ( dist   ) 
{
  if ( 0 == m_distance )
  { m_distance  = &LoKi::PhysKinematics::deltaR2 ; }
}
// ============================================================================
// MANDATORY: destructor 
// ============================================================================
LoKi::Particles::MinMaxDistance::~MinMaxDistance(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinMaxDistance*
LoKi::Particles::MinMaxDistance::clone() const 
{ return new LoKi::Particles::MinMaxDistance ( *this ) ; }
// ============================================================================
// MANDATORY: the only essential method
// ============================================================================
LoKi::Particles::MinMaxDistance::result_type 
LoKi::Particles::MinMaxDistance::operator() 
  ( LoKi::Particles::MinMaxDistance::argument p ) const 
{
  // 
  if ( 0 == p ) 
  {
    Error ("LHCb::Particle* points to NULL, return Infinity") ;
    //
    return minimum () ?
      LoKi::Constants::PositiveInfinity : 
      LoKi::Constants::NegativeInfinity ;
    //
  }
  //
  return distance ( p ) ;
}
// ============================================================================
// the actual computation 
// ============================================================================
double 
LoKi::Particles::MinMaxDistance::distance 
( const LHCb::Particle* p  ) const 
{
  //
  typedef std::pair<iterator,double> PAIR ;
  //
  LHCb::Particle::ConstVector vct ;
  using namespace boost::lambda ;
  //
  PAIR pa = 
    minimum() ? 
    LoKi::Algs::extremum 
    ( begin    () , 
      end      () ,
      std::bind1st ( std::ptr_fun ( m_distance ) , p ) , 
      std::less<double>   () ) :
    LoKi::Algs::extremum 
    ( begin    ()                   , 
      end      ()                   , 
      std::bind1st ( std::ptr_fun ( m_distance ) , p ) , 
      std::greater<double>() ) ;
  //
  if ( end() == pa.first ) 
  {
    return minimum () ?
      LoKi::Constants::PositiveInfinity : 
      LoKi::Constants::NegativeInfinity ;
  }
  //
  return pa.second ;
}
// ============================================================================


// ============================================================================
// Constructor 
// ============================================================================
LoKi::Particles::MinMaxDistanceWithSource::MinMaxDistanceWithSource
( const bool                                                minmax ,  
  const LoKi::BasicFunctors<const LHCb::Particle*>::Source& source , 
  LoKi::Particles::MinMaxDistance::dist_func                dist   )
  : LoKi::Particles::MinMaxDistance::MinMaxDistance  ( minmax                  , 
                                                       LHCb::Particle::Range() ,
                                                       dist                    ) 
  , m_source ( source ) 
{}
// ============================================================================
// MANDATORY: destructor 
// ============================================================================
LoKi::Particles::MinMaxDistanceWithSource::~MinMaxDistanceWithSource(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinMaxDistanceWithSource*
LoKi::Particles::MinMaxDistanceWithSource::clone() const 
{ return new LoKi::Particles::MinMaxDistanceWithSource(*this) ; }
// ============================================================================
// MANDATORY: the only essential method
// ============================================================================
LoKi::Particles::MinMaxDistanceWithSource::result_type 
LoKi::Particles::MinMaxDistanceWithSource::operator() 
  ( LoKi::Particles::MinMaxDistanceWithSource::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return Infinity") ;
    //
    return minimum () ?
      LoKi::Constants::PositiveInfinity : 
      LoKi::Constants::NegativeInfinity ;
    //
  }
  // check the event 
  if ( !sameEvent() ) 
  {
    typedef LoKi::UniqueKeeper<LHCb::Particle> KEEPER ;
    const KEEPER& keep1 = *this ;
    KEEPER&       keep2 = const_cast<KEEPER&> ( keep1 ) ;
    //
    // clear the list of particles 
    keep2.clear() ;
    // get the particles form the source 
    LHCb::Particle::ConstVector particles = source() () ;   // NB 
    // fill the functor with particles 
    keep2.addObjects ( particles.begin() , particles.end () ) ;
    if ( empty() ) { Warning ( "Empty list of particles is loaded!" ) ; }
    // update the event:
    setEvent () ;
  }
  // use the functor 
  return distance ( p ) ;  
}
// ============================================================================

// ============================================================================
// constructor from the source 
// ============================================================================
LoKi::Particles::MinDR2::MinDR2
( const LoKi::BasicFunctors<const LHCb::Particle*>::Source& source ) 
  : LoKi::Particles::MinMaxDistanceWithSource 
( true   , 
  source ,
  &LoKi::PhysKinematics::deltaR2 ) 
{}
// ============================================================================
// constructor from the location & filter  
// ============================================================================
LoKi::Particles::MinDR2::MinDR2
( const std::string&                                            location , 
  const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut      ) 
  : LoKi::Particles::MinMaxDistanceWithSource 
( true   , 
  LoKi::Particles::SourceTES ( location , cut ) ,
  &LoKi::PhysKinematics::deltaR2 ) 
{}
// ============================================================================
// constructor from the location & filter  
// ============================================================================
LoKi::Particles::MinDR2::MinDR2
( const std::vector<std::string>&                               locations , 
  const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut       ) 
  : LoKi::Particles::MinMaxDistanceWithSource 
( true                                           , 
  LoKi::Particles::SourceTES ( locations , cut ) ,
  &LoKi::PhysKinematics::deltaR2 ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::MinDR2::~MinDR2(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinDR2*
LoKi::Particles::MinDR2::clone() const 
{ return new LoKi::Particles::MinDR2 ( *this ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::MinDR2::fillStream ( std::ostream& s ) const 
{ return s << " MINDR2( " << source() << ") " ; }
// ============================================================================




// ============================================================================
// constructor from the source 
// ============================================================================
LoKi::Particles::MaxDR2::MaxDR2
( const LoKi::BasicFunctors<const LHCb::Particle*>::Source& source ) 
  : LoKi::Particles::MinMaxDistanceWithSource 
( false  , 
  source ,
  &LoKi::PhysKinematics::deltaR2 ) 
{}
// ============================================================================
// constructor from the location & filter  
// ============================================================================
LoKi::Particles::MaxDR2::MaxDR2
( const std::string&                                            location , 
  const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut      ) 
  : LoKi::Particles::MinMaxDistanceWithSource 
( false , 
  LoKi::Particles::SourceTES ( location , cut ) ,
  &LoKi::PhysKinematics::deltaR2 ) 
{}
// ============================================================================
// constructor from the location & filter  
// ============================================================================
LoKi::Particles::MaxDR2::MaxDR2
( const std::vector<std::string>&                               locations , 
  const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut       ) 
  : LoKi::Particles::MinMaxDistanceWithSource 
( false                                          , 
  LoKi::Particles::SourceTES ( locations , cut ) ,
  &LoKi::PhysKinematics::deltaR2 ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::MaxDR2::~MaxDR2(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MaxDR2*
LoKi::Particles::MaxDR2::clone() const 
{ return new LoKi::Particles::MaxDR2 ( *this ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::MaxDR2::fillStream ( std::ostream& s ) const 
{ return s << " MAXDR2( " << source() << ") " ; }
// ============================================================================




// ============================================================================
// constructor from the source 
// ============================================================================
LoKi::Particles::MinKL::MinKL
( const LoKi::BasicFunctors<const LHCb::Particle*>::Source& source ) 
  : LoKi::Particles::MinMaxDistanceWithSource 
( true   , 
  source ,
  &LoKi::PhysKinematics::kullback ) 
{}
// ============================================================================
// constructor from the location & filter  
// ============================================================================
LoKi::Particles::MinKL::MinKL
( const std::string&                                            location , 
  const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut      ) 
  : LoKi::Particles::MinMaxDistanceWithSource 
( true   , 
  LoKi::Particles::SourceTES ( location , cut ) ,
  &LoKi::PhysKinematics::kullback ) 
{}
// ============================================================================
// constructor from the location & filter  
// ============================================================================
LoKi::Particles::MinKL::MinKL
( const std::vector<std::string>&                               locations , 
  const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut       ) 
  : LoKi::Particles::MinMaxDistanceWithSource 
( true                                           , 
  LoKi::Particles::SourceTES ( locations , cut ) ,
  &LoKi::PhysKinematics::kullback ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::MinKL::~MinKL(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinKL*
LoKi::Particles::MinKL::clone() const 
{ return new LoKi::Particles::MinKL ( *this ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::MinKL::fillStream ( std::ostream& s ) const 
{ return s << " MINKL( " << source() << ") " ; }
// ============================================================================


// ============================================================================
// constructor from the source 
// ============================================================================
LoKi::Particles::MaxIDs::MaxIDs
( const LoKi::BasicFunctors<const LHCb::Particle*>::Source& source ) 
  : LoKi::Particles::MinMaxDistanceWithSource 
( false  , 
  source ,
  &LoKi::PhysKinematics::overlap ) 
{}
// ============================================================================
// constructor from the location & filter  
// ============================================================================
LoKi::Particles::MaxIDs::MaxIDs
( const std::string&                                            location , 
  const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut      ) 
  : LoKi::Particles::MinMaxDistanceWithSource 
( false , 
  LoKi::Particles::SourceTES ( location , cut ) ,
  &LoKi::PhysKinematics::overlap ) 
{}
// ============================================================================
// constructor from the location & filter  
// ============================================================================
LoKi::Particles::MaxIDs::MaxIDs
( const std::vector<std::string>&                               locations , 
  const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut       ) 
  : LoKi::Particles::MinMaxDistanceWithSource 
( false                                          , 
  LoKi::Particles::SourceTES ( locations , cut ) ,
  &LoKi::PhysKinematics::overlap ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::MaxIDs::~MaxIDs(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MaxIDs*
LoKi::Particles::MaxIDs::clone() const 
{ return new LoKi::Particles::MaxIDs ( *this ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::MaxIDs::fillStream ( std::ostream& s ) const 
{ return s << " MAXIDS( " << source() << ") " ; }
// ============================================================================


// ============================================================================
// constructor from the source 
// ============================================================================
LoKi::Particles::MinAngle::MinAngle
( const LoKi::BasicFunctors<const LHCb::Particle*>::Source& source ) 
  : LoKi::Particles::MinMaxDistanceWithSource 
( true   , 
  source ,
  &LoKi::PhysKinematics::deltaAlpha ) 
{}
// ============================================================================
// constructor from the location & filter  
// ============================================================================
LoKi::Particles::MinAngle::MinAngle
( const std::string&                                            location , 
  const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut      ) 
  : LoKi::Particles::MinMaxDistanceWithSource 
( true   , 
  LoKi::Particles::SourceTES ( location , cut ) ,
  &LoKi::PhysKinematics::deltaAlpha ) 
{}
// ============================================================================
// constructor from the location & filter  
// ============================================================================
LoKi::Particles::MinAngle::MinAngle
( const std::vector<std::string>&                               locations , 
  const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut       ) 
  : LoKi::Particles::MinMaxDistanceWithSource 
( true                                           , 
  LoKi::Particles::SourceTES ( locations , cut ) ,
  &LoKi::PhysKinematics::deltaAlpha ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::MinAngle::~MinAngle(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinAngle*
LoKi::Particles::MinAngle::clone() const 
{ return new LoKi::Particles::MinAngle ( *this ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::MinAngle::fillStream ( std::ostream& s ) const 
{ return s << " MINANGLE( " << source() << ") " ; }
// ============================================================================


// ============================================================================
// constructor from the source 
// ============================================================================
LoKi::Particles::MinM2::MinM2
( const LoKi::BasicFunctors<const LHCb::Particle*>::Source& source ) 
  : LoKi::Particles::MinMaxDistanceWithSource 
( true   , 
  source ,
  &LoKi::PhysKinematics::deltaM2 ) 
{}
// ============================================================================
// constructor from the location & filter  
// ============================================================================
LoKi::Particles::MinM2::MinM2
( const std::string&                                            location , 
  const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut      ) 
  : LoKi::Particles::MinMaxDistanceWithSource 
( true   , 
  LoKi::Particles::SourceTES ( location , cut ) ,
  &LoKi::PhysKinematics::deltaM2 ) 
{}
// ============================================================================
// constructor from the location & filter  
// ============================================================================
LoKi::Particles::MinM2::MinM2
( const std::vector<std::string>&                               locations ,
  const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut       ) 
  : LoKi::Particles::MinMaxDistanceWithSource 
( true                                           , 
  LoKi::Particles::SourceTES ( locations , cut ) ,
  &LoKi::PhysKinematics::deltaM2 ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::MinM2::~MinM2(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinM2*
LoKi::Particles::MinM2::clone() const 
{ return new LoKi::Particles::MinM2 ( *this ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::MinM2::fillStream ( std::ostream& s ) const 
{ return s << " MINM2( " << source() << ") " ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
