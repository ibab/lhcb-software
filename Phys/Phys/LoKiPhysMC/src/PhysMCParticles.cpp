// $Id: PhysMCParticles.cpp,v 1.8 2007-06-10 20:17:54 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKiPhysMC 
// ============================================================================
#include "LoKi/MCMatchObj.h"
#include "LoKi/MCMatch.h"
#include "LoKi/PhysMCParticles.h"
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
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param particle  pointer to MC particle object 
 */
// ============================================================================
LoKi::PhysMCParticles::MCTruth::MCTruth 
( const LoKi::MCMatch&     match    , 
  const LHCb::MCParticle*  particle ) 
  : LoKi::Predicate<const LHCb::Particle*> () 
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
  : LoKi::Predicate<const LHCb::Particle*> ()
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
  : LoKi::Predicate<const LHCb::Particle*> ()
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
  : LoKi::Predicate<const LHCb::Particle*> ()
  , LoKi::Keeper<LHCb::MCParticle>( range.begin() , range.end() )
  , m_match     ( match ) 
{} ;
// ============================================================================
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param range range of MC particles 
 */
// ============================================================================
LoKi::PhysMCParticles::MCTruth::MCTruth
( const LoKi::MCMatch&                       match   , 
  const LoKi::Keeper<LHCb::MCParticle>&      range   ) 
  : LoKi::Predicate<const LHCb::Particle*> ()
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
  : LoKi::Predicate<const LHCb::Particle*> ()
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
  : LoKi::Predicate<const LHCb::Particle*> () 
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
  : LoKi::Predicate<const LHCb::Particle*> ()
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
  : LoKi::Predicate<const LHCb::Particle*> () 
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
  : LoKi::Predicate<const LHCb::Particle*> () 
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
  : LoKi::Predicate<const LHCb::Particle*> ()
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
  : LoKi::Predicate<const LHCb::Particle*> ()
  , LoKi::Keeper<LHCb::MCParticle>( range )
  , m_match     ( match ) 
{} 
// ============================================================================
//  copy constructor
// ============================================================================
LoKi::PhysMCParticles::MCTruth::MCTruth 
( const LoKi::PhysMCParticles::MCTruth& right ) 
  : LoKi::AuxFunBase                      ( right )
  , LoKi::Predicate<const LHCb::Particle*>( right ) 
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
LoKi::PhysMCParticles::MCTruth::result_type 
LoKi::PhysMCParticles::MCTruth::operator() 
  ( LoKi::PhysMCParticles::MCTruth::argument p ) const
{
  if ( 0 == p ) 
  {
    Error("Invalid Particle! return 'False'") ;
    return false ;                                         // RETURN 
  }
  if ( empty() ) 
  { 
    Warning("Empty conatiner of MC, return 'False'");
    return false ;                                         // RETURN 
  }
  if ( !m_match.validPointer() ) 
  {
    Error("LoKi::MCMatch is invalid! return 'False'") ;
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
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param particle  pointer to particle object 
 */
// ============================================================================
LoKi::PhysMCParticles::RCTruth::RCTruth 
( const LoKi::MCMatch&  match    , 
  const LHCb::Particle* particle ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( particle )
  , m_match ( match )
{} 
// ============================================================================
/*  constructor a
 *  @param match MCMatch object (working horse)
 *  @param range "container" of particles 
 */
// ============================================================================
LoKi::PhysMCParticles::RCTruth::RCTruth 
( const LoKi::MCMatch&      match , 
  const LoKi::Types::Range& range ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range.begin() , range.end() )
  , m_match ( match )
{} 
// ============================================================================
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param range container of particles 
 */
// ============================================================================
LoKi::PhysMCParticles::RCTruth::RCTruth
( const LoKi::MCMatch&          match , 
  const LHCb::Particle::Vector& range ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range.begin() , range.end() )
  , m_match ( match )
{} 
// ============================================================================
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param range container of particles 
 */
// ============================================================================
LoKi::PhysMCParticles::RCTruth::RCTruth
( const LoKi::MCMatch&               match , 
  const LHCb::Particle::ConstVector& range ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range.begin() , range.end() )
  , m_match ( match )
{} 
// ============================================================================
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param range container of particles 
 */
// ============================================================================
LoKi::PhysMCParticles::RCTruth::RCTruth
( const LoKi::MCMatch&                match , 
  const LoKi::Keeper<LHCb::Particle>& range ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range )
  , m_match ( match )
{} 
// ============================================================================
/*  constructor 
 *  @param match MCMatch object (working horse)
 *  @param range container of particles 
 */
// ============================================================================
LoKi::PhysMCParticles::RCTruth::RCTruth
( const LoKi::MCMatch&                      match , 
  const LoKi::UniqueKeeper<LHCb::Particle>& range ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range )
  , m_match ( match )
{} 
// ============================================================================
/*  constructor 
 *  @param particle  pointer to particle object 
 *  @param match MCMatch object (working horse)
 */
// ============================================================================
LoKi::PhysMCParticles::RCTruth::RCTruth 
( const LHCb::Particle* particle ,
  const LoKi::MCMatch&  match    ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( particle )
  , m_match ( match )
{} 
// ============================================================================
/*  constructor 
 *  @param range "container" of particles 
 *  @param match MCMatch object (working horse)
 */
// ============================================================================
LoKi::PhysMCParticles::RCTruth::RCTruth 
( const LoKi::Types::Range& range ,
  const LoKi::MCMatch&      match ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range.begin() , range.end() )
  , m_match ( match )
{} 
// ============================================================================
/*  constructor 
 *  @param range container of particles 
 *  @param match MCMatch object (working horse)
 */
// ============================================================================
LoKi::PhysMCParticles::RCTruth::RCTruth
( const LHCb::Particle::Vector& range ,
  const LoKi::MCMatch&          match ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range.begin() , range.end() )
  , m_match ( match )
{} 
// ============================================================================
/*  constructor 
 *  @param range container of particles 
 *  @param match MCMatch object (working horse)
 */
// ============================================================================
LoKi::PhysMCParticles::RCTruth::RCTruth
( const LHCb::Particle::ConstVector& range ,
  const LoKi::MCMatch&               match ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range.begin() , range.end() )
  , m_match ( match )
{} 
// ============================================================================
/*  constructor 
 *  @param range container of particles 
 *  @param match MCMatch object (working horse)
 */
// ============================================================================
LoKi::PhysMCParticles::RCTruth::RCTruth
( const LoKi::Keeper<LHCb::Particle>& range ,
  const LoKi::MCMatch&                match ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range )
  , m_match ( match )
{} 
// ============================================================================
/*  constructor 
 *  @param range container of particles 
 *  @param match MCMatch object (working horse)
 */
// ============================================================================
LoKi::PhysMCParticles::RCTruth::RCTruth
( const LoKi::UniqueKeeper<LHCb::Particle>& range ,
  const LoKi::MCMatch&                      match ) 
  : LoKi::Predicate<const LHCb::MCParticle*> () 
  , LoKi::Keeper<LHCb::Particle> ( range )
  , m_match ( match )
{} 
// ============================================================================
//  copy constructor
// ============================================================================
LoKi::PhysMCParticles::RCTruth::RCTruth
( const LoKi::PhysMCParticles::RCTruth& right ) 
  : LoKi::AuxFunBase                         ( right )
  , LoKi::Predicate<const LHCb::MCParticle*> ( right ) 
  , LoKi::Keeper<LHCb::Particle> ( right )
  , m_match ( right.m_match )
{} 
// ============================================================================
//  MANDATORY : virtual destructor destructor 
// ============================================================================
LoKi::PhysMCParticles::RCTruth::~RCTruth(){}
// ============================================================================
//  MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::PhysMCParticles::RCTruth*
LoKi::PhysMCParticles::RCTruth::clone() const 
{ return new RCTruth(*this) ; }
// ============================================================================
//  MANDATORY: the only one essential method ("function")
// ============================================================================
LoKi::PhysMCParticles::RCTruth::result_type 
LoKi::PhysMCParticles::RCTruth::operator() 
  ( LoKi::PhysMCParticles::RCTruth::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("Invalid Particle! return 'False'") ;
    return false ;                                         // RETURN 
  }
  if ( empty() ) 
  { 
    Warning("Empty container of RC, return 'False'");
    return false ;                                         // RETURN 
  }
  if ( !m_match.validPointer() ) 
  {
    Error("MCMatchObj* is invalid! return 'False'") ;
    return false ;                                         // RETURN 
  }
  //
  return end() != m_match->match( begin () , end () , p ) ;
} 
// ============================================================================
//  OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::PhysMCParticles::RCTruth::fillStream 
( std::ostream& s ) const { return s << "RCTRUTH" ; }
// ============================================================================
//  constructor from HepMC::GenVertex
// ============================================================================
LoKi::PhysMCParticles::GenMCTruth::GenMCTruth 
( const HepMC::GenVertex*             particle   , 
  const LHCb::HepMC2MC*               table      , 
  const LoKi::MCMatch&                match      ) 
  : LoKi::Predicate<const LHCb::Particle*>() 
  , m_table ( table ) 
  , m_match ( match ) 
  , m_hepmc () 
  , m_mc    () 
{
  _add ( particle ) ;
  rebuild().ignore () ;
} 
// ============================================================================
//  constructor from HepMC::GenParticle
// ============================================================================
LoKi::PhysMCParticles::GenMCTruth::GenMCTruth 
( const HepMC::GenParticle*           particle   , 
  const LHCb::HepMC2MC*               table      , 
  const LoKi::MCMatch&                match      ) 
  : LoKi::Predicate<const LHCb::Particle*>() 
  , m_table ( table ) 
  , m_match ( match ) 
  , m_hepmc () 
  , m_mc    () 
{
  _add ( particle ) ;
  rebuild().ignore() ;
}   
// ============================================================================
//  constructor from container of HepMC::GenParticles
// ============================================================================
LoKi::PhysMCParticles::GenMCTruth::GenMCTruth 
( const LoKi::GenTypes::GenContainer& particles  , 
  const LHCb::HepMC2MC*               table      , 
  const LoKi::MCMatch&                match      ) 
  : LoKi::Predicate<const LHCb::Particle*>() 
  , m_table ( table ) 
  , m_match ( match ) 
  , m_hepmc () 
  , m_mc    () 
{
  _add ( particles.begin() , particles.end() ) ;
  rebuild().ignore()  ;
} 
// ============================================================================
//  constructor from range of HepMC::GenParticles
// ============================================================================
LoKi::PhysMCParticles::GenMCTruth::GenMCTruth 
( const LoKi::Types::GRange&          particles  , 
  const LHCb::HepMC2MC*               table      , 
  const LoKi::MCMatch&                match      ) 
  : LoKi::Predicate<const LHCb::Particle*>() 
  , m_table ( table ) 
  , m_match ( match ) 
  , m_hepmc () 
  , m_mc    () 
{
  _add ( particles.begin() , particles.end() ) ;
  rebuild().ignore() ;
} 
// ============================================================================
//  MANDATORY: copy constructor 
// ============================================================================
LoKi::PhysMCParticles::GenMCTruth::GenMCTruth
( const LoKi::PhysMCParticles::GenMCTruth& right ) 
  : LoKi::AuxFunBase                       ( right )
  , LoKi::Predicate<const LHCb::Particle*> ( right ) 
  , m_table ( right.m_table ) 
  , m_match ( right.m_match ) 
  , m_hepmc ( right.m_hepmc ) 
  , m_mc    ( right.m_mc    ) 
{}
// ============================================================================
//  MANDATORY: virtual destructor
// ============================================================================
LoKi::PhysMCParticles::GenMCTruth::~GenMCTruth() {}
// ============================================================================
//  MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::PhysMCParticles::GenMCTruth*
LoKi::PhysMCParticles::GenMCTruth::clone() const 
{ return new LoKi::PhysMCParticles::GenMCTruth(*this) ; }
// ============================================================================
//  add HepMC::GenParticle
// ============================================================================
void LoKi::PhysMCParticles::GenMCTruth::_add ( const HepMC::GenParticle* p ) 
{
  if ( 0 == p ) { return ; }
  m_hepmc.insert ( p ) ;
  _add ( p->end_vertex() ) ;
}
// ============================================================================
//  add HepMC::GenVertex
// ============================================================================
void LoKi::PhysMCParticles::GenMCTruth::_add ( const HepMC::GenVertex* p ) 
{
  if ( 0 == p ) { return ; }
  HepMC::GenVertex* _p  = const_cast<HepMC::GenVertex*> ( p ) ;
  _add ( _p->particles_begin ( HepMC::children ) , 
         _p->particles_end   ( HepMC::children ) ) ;       
}
// ============================================================================
//  add LoKi::GenTypes::GenContainer
// ============================================================================
void LoKi::PhysMCParticles::GenMCTruth::_add 
( const LoKi::GenTypes::GenContainer&    p ) 
{ _add ( p.begin() , p.end() ) ; }
// ============================================================================
//  add LoKi::Types::GRange 
// ============================================================================
void LoKi::PhysMCParticles::GenMCTruth::_add 
( const LoKi::Types::GRange&             p ) 
{ _add ( p.begin() , p.end() ) ; }
// ============================================================================
//  add HepMC::GenParticle
// ============================================================================
LoKi::PhysMCParticles::GenMCTruth& LoKi::PhysMCParticles::GenMCTruth::add 
( const HepMC::GenParticle*           p ) 
{ _add ( p ) ; rebuild().ignore() ; return *this ; }
// ============================================================================
//  add HepMC::GenVertex
// ============================================================================
LoKi::PhysMCParticles::GenMCTruth& LoKi::PhysMCParticles::GenMCTruth::add 
( const HepMC::GenVertex*             p ) 
{ _add ( p ) ; rebuild().ignore() ; return *this ; }
// ============================================================================
//  add LoKi::GenTypes::GenContainer
// ============================================================================
LoKi::PhysMCParticles::GenMCTruth& LoKi::PhysMCParticles::GenMCTruth::add 
( const LoKi::GenTypes::GenContainer& p ) 
{ _add ( p ) ; rebuild().ignore() ; return *this ; }
// ============================================================================
//  add LoKi::Types::GRange 
// ============================================================================
LoKi::PhysMCParticles::GenMCTruth& LoKi::PhysMCParticles::GenMCTruth::add 
( const LoKi::Types::GRange&          p ) 
{ _add ( p ) ; rebuild().ignore() ; return *this ; }
// ============================================================================
//  rebuild the internal storages 
// ============================================================================
StatusCode LoKi::PhysMCParticles::GenMCTruth::rebuild() 
{
  m_mc.clear() ;
  if( !m_table.validPointer() ) 
  { Exception ( "LHCb::HepMC2MC table is invalid" ) ; }
  if ( m_hepmc.empty() ) 
  { 
    Warning ( "Empty container of input HepMC::GenParticles" ) ; 
    return StatusCode::FAILURE ;
  }
  //
  typedef LoKi::UniqueKeeper<HepMC::GenParticle> GPs ;
  for ( GPs::const_iterator igp = m_hepmc.begin() ; 
        m_hepmc.end() != igp ; ++igp )
  {
    const LHCb::HepMC2MC::Range links = m_table->relations( *igp ) ;
    for ( LHCb::HepMC2MC::Range::iterator link = links.begin() ; 
          links.end() != link ; ++link ) { m_mc.insert ( link->to() ) ; }
  }
  if ( m_mc.empty() ) 
  { 
    Warning ( "Empty intermediate container of LHCb::MCPaticles" ) ; 
    return StatusCode::FAILURE ;
  }  
  //
  return StatusCode::SUCCESS ;
} 
// ============================================================================
//  MANDATORY: the only one essential method ("function")
// ============================================================================
LoKi::PhysMCParticles::GenMCTruth::result_type 
LoKi::PhysMCParticles::GenMCTruth::operator() 
  ( LoKi::PhysMCParticles::GenMCTruth::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* pointd to NULL, return False" ) ;
    return false ;                                         // RETURN 
  }
  if ( m_mc.empty() ) 
  { 
    Warning ( "Empty container of LHCb::MCPaticles, return False" ) ; 
    return false ;                                         // RETURN 
  }
  if ( !m_match.validPointer() ) 
  {
    Error ( "LoKi::MCMatch is invalid! return False" ) ;
    return false ;                                         // RETURN 
  }
  //
  return m_mc.end() != m_match->match ( p , m_mc.begin () ,  m_mc.end() ) ; 
} 
// ============================================================================
//  OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::PhysMCParticles::GenMCTruth::fillStream 
( std::ostream& s ) const { return s << "GMCTRUTH" ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================

