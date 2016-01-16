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
#include "LoKi/PhysExtract.h"
#include "LoKi/PhysKinematics.h"
#include "LoKi/Particles41.h"
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
 *  @date 2012-03-31
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 *
 */
// ============================================================================
// constructor 
// ============================================================================
/*  constructor 
 *  @param minmax  min/max flag
 *  @param cut     the selection of daughters 
 *  @param fun     the function 
 */
// ============================================================================
LoKi::Particles::MinMaxPair::MinMaxPair 
( const bool minmax                                                , 
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut ,
  const LoKi::Particles::MinMaxPair::dist_func                 fun ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_minimum   ( minmax )
  , m_cut       ( cut    ) 
  , m_distance  ( fun    ) 
{
  Assert ( 0 != m_distance , "Invalid distance-function is specified!" );
} 
// ============================================================================
// MANDATORY: destructor 
// ============================================================================   
LoKi::Particles::MinMaxPair::~MinMaxPair (){}
// ============================================================================   
// MANDATORY: clone method ("virtual constructor")
// ============================================================================   
LoKi::Particles::MinMaxPair*
LoKi::Particles::MinMaxPair::clone() const 
{ return new LoKi::Particles::MinMaxPair (*this) ; }
// ============================================================================
// evaluate the distance between two particles 
// ============================================================================
double LoKi::Particles::MinMaxPair::distance 
( const LHCb::Particle* p1 , 
  const LHCb::Particle* p2 ) const 
{ return (*m_distance)( p1 , p2 ) ; }
// ============================================================================
// the actual computation
// ============================================================================
double LoKi::Particles::MinMaxPair::distance ( const LHCb::Particle* p ) const 
{
  /// get all proper daughters 
  LHCb::Particle::ConstVector good ;
  good.reserve ( 8 ) ;
  // 
  LoKi::Extract::particles ( p , std::back_inserter ( good ) ) ;
  //
  double result =  m_minimum ? 
    LoKi::Constants::PositiveInfinity : 
    LoKi::Constants::NegativeInfinity ;
  //
  if ( 2 > good.size() ) { return result ; }             // RETURN
  //
  // double loop to find the appropriate pairs 
  //
  for ( LHCb::Particle::ConstVector::const_iterator ip1 = good.begin() ;
        good.end() != ip1 ; ++ip1 ) 
  {
    for ( LHCb::Particle::ConstVector::const_iterator ip2 = ip1 + 1 ;
          good.end() != ip2 ; ++ip2 ) 
    {
      result = m_minimum ? 
        std::min ( result , distance ( *ip1 , *ip2 ) ) : 
        std::max ( result , distance ( *ip1 , *ip2 ) ) ;
    }
  }
  //
  return result ; 
}
// ============================================================================
// MANDATORY: the only essential method
// ============================================================================
LoKi::Particles::MinMaxPair::result_type 
LoKi::Particles::MinMaxPair::operator() 
  ( LoKi::Particles::MinMaxPair::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ("LHCb::Particle* points to NULL") ;
    return m_minimum ? 
      LoKi::Constants::PositiveInfinity : 
      LoKi::Constants::NegativeInfinity ;
  }
  //
  return distance ( p ) ;
}
// ============================================================================


// ============================================================================
// constructor from daughter selection 
// ============================================================================
LoKi::Particles::MinKullback::MinKullback 
( const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut ) 
  : LoKi::AuxFunBase  ( std::tie ( cut ) ) 
  , LoKi::Particles::MinMaxPair ( true , 
                                  cut  , 
                                  &LoKi::PhysKinematics::kullback ) 
{}
// ============================================================================
// MANDATORY: destructor 
// ============================================================================
LoKi::Particles::MinKullback::~MinKullback (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinKullback*
LoKi::Particles::MinKullback::clone() const 
{ return new LoKi::Particles::MinKullback ( *this ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::MinKullback::fillStream
( std::ostream& s ) const 
{ return s << " MINKULLBACK( " << cut() << ") " ; }
// ============================================================================



// ============================================================================
// constructor from daughter selection 
// ============================================================================
LoKi::Particles::MinAng::MinAng 
( const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut ) 
  : LoKi::AuxFunBase  ( std::tie ( cut ) ) 
  , LoKi::Particles::MinMaxPair ( true , 
                                  cut  , 
                                  &LoKi::PhysKinematics::deltaAlpha ) 
{}
// ============================================================================
// MANDATORY: destructor 
// ============================================================================
LoKi::Particles::MinAng::~MinAng (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinAng*
LoKi::Particles::MinAng::clone() const 
{ return new LoKi::Particles::MinAng ( *this ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::MinAng::fillStream
( std::ostream& s ) const 
{ return s << " MINANG( " << cut() << ") " ; }
// ============================================================================


// ============================================================================
// constructor from daughter selection 
// ============================================================================
LoKi::Particles::MinDeltaM2::MinDeltaM2 
( const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut ) 
  : LoKi::AuxFunBase  ( std::tie ( cut ) ) 
  , LoKi::Particles::MinMaxPair ( true , 
                                  cut  , 
                                  &LoKi::PhysKinematics::deltaM2 ) 
{}
// ============================================================================
// MANDATORY: destructor 
// ============================================================================
LoKi::Particles::MinDeltaM2::~MinDeltaM2 (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinDeltaM2*
LoKi::Particles::MinDeltaM2::clone() const 
{ return new LoKi::Particles::MinDeltaM2 ( *this ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::MinDeltaM2::fillStream
( std::ostream& s ) const 
{ return s << " MINDELTAM2( " << cut() << ") " ; }
// ============================================================================



// ============================================================================
// constructor from daughter selection 
// ============================================================================
LoKi::Particles::MaxOverlap::MaxOverlap
( const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut ) 
  : LoKi::AuxFunBase  ( std::tie ( cut ) ) 
  , LoKi::Particles::MinMaxPair ( false , 
                                  cut   , 
                                  &LoKi::PhysKinematics::overlap ) 
{}
// ============================================================================
// MANDATORY: destructor 
// ============================================================================
LoKi::Particles::MaxOverlap::~MaxOverlap (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MaxOverlap*
LoKi::Particles::MaxOverlap::clone() const 
{ return new LoKi::Particles::MaxOverlap ( *this ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::MaxOverlap::fillStream
( std::ostream& s ) const 
{ return s << " MAXOVERLAP( " << cut() << ") " ; }
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
