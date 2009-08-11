// $Id: MCTruth.cpp,v 1.1 2009-08-11 18:27:00 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKiPhysMC 
// ============================================================================
#include "LoKi/MCMatchObj.h"
#include "LoKi/MCMatch.h"
#include "LoKi/MCTruth.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-13 
 */
// ============================================================================
namespace
{
  // ==========================================================================
  const LoKi::MCMatchObj* const s_MCMATCH =  0 ;
  // ==========================================================================
}
// ============================================================================
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param particle  pointer to MC particle object 
 */
// ============================================================================
LoKi::PhysMCParticles::MCTruth::MCTruth 
( const LoKi::MCMatch&     match    , 
  const LHCb::MCParticle*  particle ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::Keeper<LHCb::MCParticle> ( particle ) 
  , m_match ( match ) 
{} 
// ============================================================================
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::PhysMCParticles::MCTruth::MCTruth
( const LoKi::MCMatch&            match   , 
  const LoKi::MCTypes::MCRange&  range   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::Keeper<LHCb::MCParticle>( range.begin() , range.end() )
  , m_match ( match ) 
{} 
// ============================================================================
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::PhysMCParticles::MCTruth::MCTruth
( const LoKi::MCMatch&              match   , 
  const LHCb::MCParticle::Vector& range   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::Keeper<LHCb::MCParticle>( range.begin() , range.end() )
  , m_match     ( match ) 
{} 
// ============================================================================
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::PhysMCParticles::MCTruth::MCTruth
( const LoKi::MCMatch&                       match   , 
  const LHCb::MCParticle::ConstVector&       range   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::Keeper<LHCb::MCParticle>( range.begin() , range.end() )
  , m_match     ( match ) 
{} 
// ============================================================================
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::PhysMCParticles::MCTruth::MCTruth
( const LoKi::MCMatch&                       match   , 
  const LoKi::Keeper<LHCb::MCParticle>&      range   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::Keeper<LHCb::MCParticle>( range )
  , m_match     ( match ) 
{} 
// ============================================================================
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::PhysMCParticles::MCTruth::MCTruth
( const LoKi::MCMatch&                        match   , 
  const LoKi::UniqueKeeper<LHCb::MCParticle>& range   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::Keeper<LHCb::MCParticle>( range )
  , m_match     ( match ) 
{} 
// ============================================================================
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param particle  pointer to MC particle object 
 */
// ============================================================================
LoKi::PhysMCParticles::MCTruth::MCTruth 
( const LHCb::MCParticle*  particle ,
  const LoKi::MCMatch&     match    ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::Keeper<LHCb::MCParticle>( particle )
  , m_match     ( match ) 
{} 
// ============================================================================
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::PhysMCParticles::MCTruth::MCTruth
( const LoKi::MCTypes::MCRange&  range    ,
  const LoKi::MCMatch&            match   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::Keeper<LHCb::MCParticle>( range.begin() , range.end() )
  , m_match     ( match ) 
{} 
// ============================================================================
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::PhysMCParticles::MCTruth::MCTruth
( const LHCb::MCParticle::Vector& range   ,
  const LoKi::MCMatch&              match   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::Keeper<LHCb::MCParticle>( range.begin() , range.end() )
  , m_match     ( match ) 
{} 
// ============================================================================
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::PhysMCParticles::MCTruth::MCTruth
( const LHCb::MCParticle::ConstVector&       range   ,
  const LoKi::MCMatch&                       match   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::Keeper<LHCb::MCParticle>( range.begin() , range.end() )
  , m_match     ( match                       ) 
{} 
// ============================================================================
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::PhysMCParticles::MCTruth::MCTruth
( const LoKi::Keeper<LHCb::MCParticle>&      range ,
  const LoKi::MCMatch&                       match ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::Keeper<LHCb::MCParticle>( range )
  , m_match     ( match ) 
{} 
// ============================================================================
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::PhysMCParticles::MCTruth::MCTruth
( const LoKi::UniqueKeeper<LHCb::MCParticle>& range ,
  const LoKi::MCMatch&                        match )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::Keeper<LHCb::MCParticle>( range )
  , m_match     ( match ) 
{} 
// ============================================================================
//  copy constructor
// ============================================================================
LoKi::PhysMCParticles::MCTruth::MCTruth 
( const LoKi::PhysMCParticles::MCTruth& right ) 
  : LoKi::AuxFunBase                      ( right )
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate ( right ) 
  , LoKi::Keeper<LHCb::MCParticle>        ( right )
  , m_match ( right.m_match ) 
{} 
// ============================================================================
//  MANDATORY: virual destructor 
// ============================================================================
LoKi::PhysMCParticles::MCTruth::~MCTruth(){}
// ============================================================================
//  MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::PhysMCParticles::MCTruth*
LoKi::PhysMCParticles::MCTruth::clone() const
{ return new MCTruth(*this) ; }
// ============================================================================
//  MANDATORY: the only one essential method ("function")
// ============================================================================
bool LoKi::PhysMCParticles::MCTruth::match 
( LoKi::PhysMCParticles::MCTruth::argument p ) const
{
  if ( 0 == p ) 
  {
    Error("Invalid Particle! return 'False'") ;
    return false ;                                         // RETURN 
  }
  if ( empty() ) 
  { 
    Warning("Empty container of MC, return 'False'");
    return false ;                                         // RETURN 
  }
  if ( !m_match.validPointer() ) 
  {
    Error("LoKi::MCMatch is invalid! return 'False'") ;
    return false ;                                         // RETURN 
  }
  if ( m_match->empty() ) 
  {
    Warning ( "Empty list of Relation Tables, return 'False'");
    return false ;                                         // RETURN 
  }
  //
  return end() != m_match->match( p , begin () , end   () ) ;
} 
// ============================================================================
//  OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::PhysMCParticles::MCTruth::fillStream 
( std::ostream& s ) const { return s << "MCTRUTH" ; }
// ============================================================================
// protected defauls contructor 
// ============================================================================
LoKi::PhysMCParticles::MCTruth::MCTruth () 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::Keeper<LHCb::MCParticle>()
  , m_match     ( s_MCMATCH  ) 
{} 
// ============================================================================
// set new mc match object
// ============================================================================
void LoKi::PhysMCParticles::MCTruth::setMatch ( const LoKi::MCMatch& m ) const
{
  LoKi::MCMatch& tmp = const_cast<LoKi::MCMatch&>( m_match ) ;
  tmp = m ; 
}
// ============================================================================
// clear the list of particles 
// ============================================================================
LoKi::PhysMCParticles::MCTruth::Keeper& 
LoKi::PhysMCParticles::MCTruth::storage () const
{
  const Keeper& keeper = *this ;
  return const_cast<Keeper&> ( keeper ) ;
}
// ============================================================================
// The END
// ============================================================================
