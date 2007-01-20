// $Id: PhysMCParticles.cpp,v 1.5 2007-01-20 14:47:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2006/10/27 13:41:27  ibelyaev
//  fix for SLC4 platform
//
// Revision 1.3  2006/09/06 12:37:21  ibelyaev
//  remove duplicated constructor
//
// Revision 1.2  2006/03/14 19:37:09  ibelyaev
//  minor changes
//
// Revision 1.1.1.1  2006/03/14 19:12:21  ibelyaev
// New package : RC <---> MC links for LoKi 
// 
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
  const LHCb::MCParticle::Vector& range   ) 
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
  const LHCb::MCParticle::ConstVector&       range   ) 
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
( const LHCb::MCParticle::Vector& range   ,
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
( const LHCb::MCParticle::ConstVector&       range   ,
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
  : LoKi::AuxFunBase                      ( right )
  , LoKi::Predicate<const LHCb::Particle*>( right ) 
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
  const LoKi::Keeper<LHCb::Particle>& range ) 
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
  const LoKi::MCMatch&  match    ) 
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
  : LoKi::AuxFunBase                         ( right )
  , LoKi::Predicate<const LHCb::MCParticle*> ( right ) 
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
} ;
// ============================================================================
/// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::MCParticles::RCTruth::fillStream 
( std::ostream& s ) const { return s << "RCTRUTH" ; }
// ============================================================================


// ============================================================================
/// constructor from HepMC::GenVertex
// ============================================================================
LoKi::Particles::GenMCTruth::GenMCTruth 
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
  rebuild() ;
} ;  
// ============================================================================
/// constructor from HepMC::GenParticle
// ============================================================================
LoKi::Particles::GenMCTruth::GenMCTruth 
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
  rebuild() ;
} ;  
// ============================================================================
/// constructor from container of HepMC::GenParticles
// ============================================================================
LoKi::Particles::GenMCTruth::GenMCTruth 
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
  rebuild() ;
} ;
// ============================================================================
/// constructor from range of HepMC::GenParticles
// ============================================================================
LoKi::Particles::GenMCTruth::GenMCTruth 
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
  rebuild() ;
} ;
// ============================================================================
/// MANDATORY: copy constructor 
// ============================================================================
LoKi::Particles::GenMCTruth::GenMCTruth
( const LoKi::Particles::GenMCTruth& right ) 
  : LoKi::AuxFunBase                       ( right )
  , LoKi::Predicate<const LHCb::Particle*> ( right ) 
  , m_table ( right.m_table ) 
  , m_match ( right.m_match ) 
  , m_hepmc ( right.m_hepmc ) 
  , m_mc    ( right.m_mc    ) 
{}
// ============================================================================
/// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::GenMCTruth::~GenMCTruth() {}
// ============================================================================
/// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::GenMCTruth*
LoKi::Particles::GenMCTruth::clone() const 
{ return new LoKi::Particles::GenMCTruth(*this) ; }
// ============================================================================
/// add HepMC::GenParticle
// ============================================================================
void LoKi::Particles::GenMCTruth::_add ( const HepMC::GenParticle* p ) 
{
  if ( 0 == p ) { return ; }
  m_hepmc.insert ( p ) ;
  _add ( p->end_vertex() ) ;
}
// ============================================================================
/// add HepMC::GenVertex
// ============================================================================
void LoKi::Particles::GenMCTruth::_add ( const HepMC::GenVertex* p ) 
{
  if ( 0 == p ) { return ; }
  HepMC::GenVertex* _p  = const_cast<HepMC::GenVertex*> ( p ) ;
  _add ( _p->particles_begin ( HepMC::children ) , 
         _p->particles_end   ( HepMC::children ) ) ;       
}
// ============================================================================
/// add LoKi::GenTypes::GenContainer
// ============================================================================
void LoKi::Particles::GenMCTruth::_add 
( const LoKi::GenTypes::GenContainer&    p ) 
{ _add ( p.begin() , p.end() ) ; }
// ============================================================================
/// add LoKi::Types::GRange 
// ============================================================================
void LoKi::Particles::GenMCTruth::_add 
( const LoKi::Types::GRange&             p ) 
{ _add ( p.begin() , p.end() ) ; }
// ============================================================================
/// add HepMC::GenParticle
// ============================================================================
LoKi::Particles::GenMCTruth& LoKi::Particles::GenMCTruth::add 
( const HepMC::GenParticle*           p ) 
{ _add ( p ) ; rebuild() ; return *this ; }
// ============================================================================
/// add HepMC::GenVertex
// ============================================================================
LoKi::Particles::GenMCTruth& LoKi::Particles::GenMCTruth::add 
( const HepMC::GenVertex*             p ) 
{ _add ( p ) ; rebuild() ; return *this ; }
// ============================================================================
/// add LoKi::GenTypes::GenContainer
// ============================================================================
LoKi::Particles::GenMCTruth& LoKi::Particles::GenMCTruth::add 
( const LoKi::GenTypes::GenContainer& p ) 
{ _add ( p ) ; rebuild() ; return *this ; }
// ============================================================================
/// add LoKi::Types::GRange 
// ============================================================================
LoKi::Particles::GenMCTruth& LoKi::Particles::GenMCTruth::add 
( const LoKi::Types::GRange&          p ) 
{ _add ( p ) ; rebuild() ; return *this ; }
// ============================================================================
/// rebuild the internal storages 
// ============================================================================
StatusCode LoKi::Particles::GenMCTruth::rebuild() 
{
  m_mc.clear() ;
  if( !m_table.validPointer() ) 
  { Exception ( "LHCb::HepMC2MC table is invalid" ) ; }
  if ( m_hepmc.empty() ) 
  { return Warning ( "Empty container of input HepMC::GenParticles" ) ; }
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
  { return Warning ( "Empty intermediate container of LHCb::MCPaticles" ) ; }  
  //
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
/// MANDATORY: the only one essential method ("function")
// ============================================================================
LoKi::Particles::GenMCTruth::result_type 
LoKi::Particles::GenMCTruth::operator() 
  ( LoKi::Particles::GenMCTruth::argument p ) const 
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
} ;
// ============================================================================
/// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Particles::GenMCTruth::fillStream 
( std::ostream& s ) const { return s << "GMCTRUTH" ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================

