// $Id: PhysMCParticles.cpp,v 1.1.1.1 2006-03-14 19:12:21 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// LoKiPhysMC 
// ============================================================================
#include "LoKi/MCMatchObj.h"
#include "LoKi/MCMatch.h"
#include "LoKi/PhysMCParticles.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for fuctions form the file LoKi/PhysMCParticles.h
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-13 
 */
// ============================================================================

// ============================================================================
/** constructor 
 *  @param match MCMatch object (working horse)
 *  @param particle  pointer to MC particle object 
 */
// ============================================================================
LoKi::Particles::MCTruth::MCTruth 
( const LoKi::MCMatch&     match    , 
  const LHCb::MCParticle*  particle ) 
  : LoKi::Predicate<const LHCb::Particle*> () 
  , LoKi::Keeper<LHCb::MCParticle> ( particle ) 
  , m_match ( match ) 
{} ;
// ============================================================================
/** constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::Particles::MCTruth::MCTruth
( const LoKi::MCMatch&            match   , 
  const LoKi::MCTypes::MCRange&  range   ) 
  : LoKi::Predicate<const LHCb::Particle*> ()
  , LoKi::Keeper<LHCb::MCParticle>( range.begin() , range.end() )
  , m_match ( match ) 
{} ;
// ============================================================================
/** constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::Particles::MCTruth::MCTruth
( const LoKi::MCMatch&              match   , 
  const LoKi::MCTypes::MCContainer& range   ) 
  : LoKi::Predicate<const LHCb::Particle*> ()
  , LoKi::Keeper<LHCb::MCParticle>( range.begin() , range.end() )
  , m_match     ( match ) 
{} ;
// ============================================================================
/** constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::Particles::MCTruth::MCTruth
( const LoKi::MCMatch&                       match   , 
  const std::vector<const LHCb::MCParticle*> range   ) 
  : LoKi::Predicate<const LHCb::Particle*> ()
  , LoKi::Keeper<LHCb::MCParticle>( range.begin() , range.end() )
  , m_match     ( match ) 
{} ;
// ============================================================================
/** constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::Particles::MCTruth::MCTruth
( const LoKi::MCMatch&                       match   , 
  const LoKi::Keeper<LHCb::MCParticle>&      range   ) 
  : LoKi::Predicate<const LHCb::Particle*> ()
  , LoKi::Keeper<LHCb::MCParticle>( range )
  , m_match     ( match ) 
{} ;

// ============================================================================
/** constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::Particles::MCTruth::MCTruth
( const LoKi::MCMatch&                        match   , 
  const LoKi::UniqueKeeper<LHCb::MCParticle>& range   ) 
  : LoKi::Predicate<const LHCb::Particle*> ()
  , LoKi::Keeper<LHCb::MCParticle>( range )
  , m_match     ( match ) 
{} ;
// ============================================================================
/** constructor 
 *  @param match MCMatch object (working horse)
 *  @param particle  pointer to MC particle object 
 */
// ============================================================================
LoKi::Particles::MCTruth::MCTruth 
( const LHCb::MCParticle*  particle ,
  const LoKi::MCMatch&     match    ) 
  : LoKi::Predicate<const LHCb::Particle*> () 
  , LoKi::Keeper<LHCb::MCParticle>( particle )
  , m_match     ( match ) 
{} ;
// ============================================================================
/** constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::Particles::MCTruth::MCTruth
( const LoKi::MCTypes::MCRange&  range    ,
  const LoKi::MCMatch&            match   ) 
  : LoKi::Predicate<const LHCb::Particle*> ()
  , LoKi::Keeper<LHCb::MCParticle>( range.begin() , range.end() )
  , m_match     ( match ) 
{} ;
// ============================================================================
/** constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::Particles::MCTruth::MCTruth
( const LoKi::MCTypes::MCContainer& range   ,
  const LoKi::MCMatch&              match   ) 
  : LoKi::Predicate<const LHCb::Particle*> () 
  , LoKi::Keeper<LHCb::MCParticle>( range.begin() , range.end() )
  , m_match     ( match ) 
{} ;
// ============================================================================
/** constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::Particles::MCTruth::MCTruth
( const std::vector<const LHCb::MCParticle*> range   ,
  const LoKi::MCMatch&                       match   ) 
  : LoKi::Predicate<const LHCb::Particle*> () 
  , LoKi::Keeper<LHCb::MCParticle>( range.begin() , range.end() )
  , m_match     ( match                       ) 
{} ;
// ============================================================================
/** constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::Particles::MCTruth::MCTruth
( const LoKi::Keeper<LHCb::MCParticle>&      range ,
  const LoKi::MCMatch&                       match ) 
  : LoKi::Predicate<const LHCb::Particle*> ()
  , LoKi::Keeper<LHCb::MCParticle>( range )
  , m_match     ( match ) 
{} ;
// ============================================================================
/** constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::Particles::MCTruth::MCTruth
( const LoKi::UniqueKeeper<LHCb::MCParticle>& range ,
  const LoKi::MCMatch&                        match )
  : LoKi::Predicate<const LHCb::Particle*> ()
  , LoKi::Keeper<LHCb::MCParticle>( range )
  , m_match     ( match ) 
{} ;
// ============================================================================
/// copy constructor
// ============================================================================
LoKi::Particles::MCTruth::MCTruth 
( const LoKi::Particles::MCTruth& right ) 
  : LoKi::Predicate<const LHCb::Particle*>( right ) 
  , LoKi::Keeper<LHCb::MCParticle>        ( right )
  , m_match ( right.m_match ) 
{} ;
// ============================================================================
/// MANDATORY: virual destructor 
// ============================================================================
LoKi::Particles::MCTruth::~MCTruth(){}
// ============================================================================
/// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MCTruth*
LoKi::Particles::MCTruth::clone() const
{ return new MCTruth(*this) ; }
// ============================================================================
/// MANDATORY: the only one essential method ("function")
// ============================================================================
LoKi::Particles::MCTruth::result_type 
LoKi::Particles::MCTruth::operator() 
  ( LoKi::Particles::MCTruth::argument p ) const
{
  if ( 0 == p ) 
  {
    Error("Invalid Particle! return 'false'") ;
    return false ;                                         // RETURN 
  }
  if ( empty() ) 
  { 
    Warning("Empty conatiner of MC, return 'false'");
    return false ;                                         // RETURN 
  }
  if ( !m_match.validPointer() ) 
  {
    Error("MCMatchObj* is invalid! return 'false'") ;
    return false ;                                         // RETURN 
  }
  //
  return end() != m_match->match( p , begin () , end   () ) ;
} ;
// ============================================================================
/// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Particles::MCTruth::fillStream 
( std::ostream& s ) const { return s << "MCTRUTH" ; }
// ============================================================================

// ============================================================================
/** constructor 
 *  @param match MCMatch object (working horse)
 *  @param particle  pointer to particle object 
 */
// ============================================================================
LoKi::MCParticles::RCTruth::RCTruth 
( const LoKi::MCMatch&  match    , 
  const LHCb::Particle* particle ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( particle )
  , m_match ( match )
{} ;
// ============================================================================
/** constructor a
 *  @param match MCMatch object (working horse)
 *  @param range "container" of particles 
 */
// ============================================================================
LoKi::MCParticles::RCTruth::RCTruth 
( const LoKi::MCMatch&      match , 
  const LoKi::Types::Range& range ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range.begin() , range.end() )
  , m_match ( match )
{} ;
// ============================================================================
/** constructor 
 *  @param match MCMatch object (working horse)
 *  @param range container of particles 
 */
// ============================================================================
LoKi::MCParticles::RCTruth::RCTruth
( const LoKi::MCMatch&          match , 
  const LHCb::Particle::Vector& range ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range.begin() , range.end() )
  , m_match ( match )
{} ;
// ============================================================================
/** constructor 
 *  @param match MCMatch object (working horse)
 *  @param range container of particles 
 */
// ============================================================================
LoKi::MCParticles::RCTruth::RCTruth
( const LoKi::MCMatch&               match , 
  const LHCb::Particle::ConstVector& range ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range.begin() , range.end() )
  , m_match ( match )
{} ;
// ============================================================================
/** constructor 
 *  @param match MCMatch object (working horse)
 *  @param range container of particles 
 */
// ============================================================================
LoKi::MCParticles::RCTruth::RCTruth
( const LoKi::MCMatch&                match , 
  const LoKi::Keeper<LHCb::Particle>& range ) ;
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range )
  , m_match ( match )
{} ;
// ============================================================================
/** constructor 
 *  @param match MCMatch object (working horse)
 *  @param range container of particles 
 */
// ============================================================================
LoKi::MCParticles::RCTruth::RCTruth
( const LoKi::MCMatch&                      match , 
  const LoKi::UniqueKeeper<LHCb::Particle>& range ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range )
  , m_match ( match )
{} ;
// ============================================================================
/** constructor 
 *  @param particle  pointer to particle object 
 *  @param match MCMatch object (working horse)
 */
// ============================================================================
LoKi::MCParticles::RCTruth::RCTruth 
( const LHCb::Particle* particle ,
  const LoKi::MCMatch&  match    ) ;
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( particle )
  , m_match ( match )
{} ;
// ============================================================================
/** constructor 
 *  @param range "container" of particles 
 *  @param match MCMatch object (working horse)
 */
// ============================================================================
LoKi::MCParticles::RCTruth::RCTruth 
( const LoKi::Types::Range& range ,
  const LoKi::MCMatch&      match ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range.begin() , range.end() )
  , m_match ( match )
{} ;
// ============================================================================
/** constructor 
 *  @param range container of particles 
 *  @param match MCMatch object (working horse)
 */
// ============================================================================
LoKi::MCParticles::RCTruth::RCTruth
( const LHCb::Particle::Vector& range ,
  const LoKi::MCMatch&          match ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range.begin() , range.end() )
  , m_match ( match )
{} ;
// ============================================================================
/** constructor 
 *  @param range container of particles 
 *  @param match MCMatch object (working horse)
 */
// ============================================================================
LoKi::MCParticles::RCTruth::RCTruth
( const LHCb::Particle::ConstVector& range ,
  const LoKi::MCMatch&               match ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range.begin() , range.end() )
  , m_match ( match )
{} ;
// ============================================================================
/** constructor 
 *  @param range container of particles 
 *  @param match MCMatch object (working horse)
 */
// ============================================================================
LoKi::MCParticles::RCTruth::RCTruth
( const LoKi::Keeper<LHCb::Particle>& range ,
  const LoKi::MCMatch&                match ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range )
  , m_match ( match )
{} ;
// ============================================================================
/** constructor 
 *  @param range container of particles 
 *  @param match MCMatch object (working horse)
 */
// ============================================================================
LoKi::MCParticles::RCTruth::RCTruth
( const LoKi::UniqueKeeper<LHCb::Particle>& range ,
  const LoKi::MCMatch&                      match ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range )
  , m_match ( match )
{} ;
// ============================================================================
/// copy constructor
// ============================================================================
LoKi::MCParticles::RCTruth::RCTruth
( const LoKi::MCParticles::RCTruth& right ) 
  : LoKi::Predicate<const LHCb::MCParticle*> ( right ) 
  , LoKi::Keeper<LHCb::Particle> ( right )
  , m_match ( right.m_match )
{} ;
// ============================================================================
/// MANDATORY : virtual destructor destructor 
// ============================================================================
LoKi::MCParticles::RCTruth::~RCTruth(){}
// ============================================================================
/// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::RCTruth*
LoKi::MCParticles::RCTruth::clone() const 
{ return new RCTruth(*this) ; }
// ============================================================================
/// MANDATORY: the only one essential method ("function")
// ============================================================================
LoKi::MCParticles::RCTruth::result_type 
LoKi::MCParticles::RCTruth::operator() 
  ( LoKi::MCParticles::RCTruth::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("Invalid Particle! return 'false'") ;
    return false ;                                         // RETURN 
  }
  if ( empty() ) 
  { 
    Warning("Empty container of RC, return 'false'");
    return false ;                                         // RETURN 
  }
  if ( !m_match.validPointer() ) 
  {
    Error("MCMatchObj* is invalid! return 'false'") ;
    return false ;                                         // RETURN 
  }
  //
  return end() != m_match->match( begin () , end () , p ) ;
} ;
// ============================================================================
/// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::MCParticles::RCTruth::fillStream 
( std::ostream& s ) const { return s << "RCTRUTH" ; }
// ============================================================================




// ============================================================================
// The END 
// ============================================================================

