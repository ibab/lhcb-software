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
#include "LoKi/Functors.h"
#include "LoKi/Constants.h"
#include "LoKi/Algs.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/PhysKinematics.h"
#include "LoKi/Particles8.h"
#include "LoKi/Particles9.h"
#include "LoKi/Particles17.h"
#include "LoKi/Particles43.h"
#include "LoKi/PhysSources.h"
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
/*  constructor form value of \f$\left(\Delta R\right)^2\f$
 *  @param dr2 the DR2-value
 *  @param fun the functor for summation 
 *  @param parts the particles 
 *  @param init  the initial value 
 */
// ============================================================================
LoKi::Particles::SumInR2Cone::SumInR2Cone
( const double                                                dr2 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Function& fun , 
  const LHCb::Particle::Range& parts                              , 
  const double                 init                               ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::UniqueKeeper<LHCb::Particle> ( parts.begin() , parts.end() ) 
  , m_fun  ( fun  )
  , m_dr2  ( dr2  )
  , m_init ( init )
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::SumInR2Cone::~SumInR2Cone () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::SumInR2Cone*
LoKi::Particles::SumInR2Cone::clone() const 
{ return new LoKi::Particles::SumInR2Cone ( *this ) ; }
// ============================================================================
// MANDATORY: the only essential method
// ============================================================================
LoKi::Particles::SumInR2Cone::result_type 
LoKi::Particles::SumInR2Cone::operator()
  ( LoKi::Particles::SumInR2Cone::argument p  ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'NegativeInfinity'" ) ;
    return LoKi::Constants::NegativeInfinity ;       // RETURN 
  }
  //
  return sum ( p ) ;
}
// ============================================================================
// calcualet tehsum 
// ============================================================================
double LoKi::Particles::SumInR2Cone::sum ( const LHCb::Particle* p  ) const 
{
  // empty storage? 
  if ( empty() ) { return m_init ; }                 // RETURN 
  //
  // create selection criteria 
  //
  using namespace LoKi::Particles ;
  const DeltaR2   r2 ( p ) ;
  const HasProtos c1 ( p ) ;
  //
  double result = m_init ;
  return LoKi::Algs::accumulate 
    ( begin ()              , 
      end   ()              , 
      m_fun                 , 
      ( r2 < m_dr2 ) && !c1 , 
      result                , 
      std::plus<double> ()  ) ;
}
// ===========================================================================

// ===========================================================================
// constructor from the source 
// ===========================================================================
LoKi::Particles::SumInR2ConeWithSource::SumInR2ConeWithSource
( const double                                                 dr2    ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Function&  fun    , 
  const LoKi::BasicFunctors<const LHCb::Particle*>::Source&    source ,
  const double                                                 init   )
  : LoKi::AuxFunBase ( std::tie ( dr2 , fun , source , init ) ) 
  , LoKi::Particles::SumInR2Cone ( dr2 , fun ,  LHCb::Particle::Range() , init )
  , m_source ( source ) 
{}
// ===========================================================================
// constructor from the source 
// ===========================================================================
LoKi::Particles::SumInR2ConeWithSource::SumInR2ConeWithSource
( const double                                                 dr2      ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Function&  fun      , 
  const std::string&                                           location ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut      , 
  const double                                                 init     )
  : LoKi::AuxFunBase ( std::tie ( dr2 , fun , location , cut , init ) ) 
  , LoKi::Particles::SumInR2Cone ( dr2 , fun ,  LHCb::Particle::Range() , init )
  , m_source (   LoKi::Particles::SourceTES ( location , cut ) ) 
{}
// ===========================================================================
// constructor from the source 
// ===========================================================================
LoKi::Particles::SumInR2ConeWithSource::SumInR2ConeWithSource
( const double                                                 dr2      ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Function&  fun      , 
  const std::vector<std::string>&                              location ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut      , 
  const double                                                 init     )
  : LoKi::AuxFunBase ( std::tie ( dr2 , fun , location , cut , init ) ) 
  , LoKi::Particles::SumInR2Cone ( dr2 , fun ,  LHCb::Particle::Range() , init )
  , m_source (   LoKi::Particles::SourceTES ( location , cut ) ) 
{}
// ===========================================================================
// MANDATORY : virtual destructor 
// ===========================================================================
LoKi::Particles::SumInR2ConeWithSource::~SumInR2ConeWithSource () {}
// ===========================================================================
// MANDATORY : clone method ("virtual constructor")
// ===========================================================================
LoKi::Particles::SumInR2ConeWithSource*
LoKi::Particles::SumInR2ConeWithSource::clone () const 
{ return new LoKi::Particles::SumInR2ConeWithSource ( *this ) ; }
// ===========================================================================
// MANDATORY: the only essential method
// ===========================================================================
LoKi::Particles::SumInR2ConeWithSource::result_type 
LoKi::Particles::SumInR2ConeWithSource::operator() 
  ( LoKi::Particles::SumInR2ConeWithSource::argument p ) const
{
  //
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return NegativeInfinity") ;
    return LoKi::Constants::NegativeInfinity ;
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
  return sum ( p ) ;  
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::SumInR2ConeWithSource::fillStream 
( std::ostream& s ) const 
{
  s << " SUMCONE( " << dr2() << "," << source() ;
  if ( 0 != init() ) { s << "," << init() ; }
  return s << ") " ;
}
// ===========================================================================


 
// ============================================================================
// The END 
// ============================================================================
