// $Id: MCMatcher.cpp,v 1.1 2009-08-11 18:27:00 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/ToStream.h"
#include "GaudiKernel/IAlgContextSvc.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GetAlgs.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/ProtoParticle.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Trees.h"
#include "LoKi/IReporter.h"
// ============================================================================
// LoKiMC 
// ============================================================================
#include "LoKi/IMCDecay.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/MCMatchObj.h"
#include "LoKi/MCMatch.h"
#include "LoKi/MCMatcher.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::PhysMCParticles::MCMatcher
 *  @date 2009-08-11 
 *  @author Vanya BELYAEV ibelyaev@nikhef.nl
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /// validation of decay finder 
  inline StatusCode _validate 
  ( const LoKi::PhysMCParticles::MCMatcher::iTree& tree , 
    const LHCb::IParticlePropertySvc*              svc  ) 
  { return tree.validate ( svc ) ; }
  // ==========================================================================
  /// get gaudi algorithm from the context 
  GaudiAlgorithm* _getAlg ( LoKi::ILoKiSvc* loki ) 
  {
    SmartIF<IAlgContextSvc> cntx ( loki );
    return Gaudi::Utils::getGaudiAlg  ( cntx ) ;
  }
  // ==========================================================================
  /// invalid decay
  const Decays::Trees::Types_<const LHCb::MCParticle*>::Invalid s_INVALID ;
  // ==========================================================================
} //                                                 end of anonymous namespace 
// ============================================================================
// check the decay descriptor
// ============================================================================
void LoKi::PhysMCParticles::MCMatcher::checkFinder ()
{
  if ( !m_finder ) 
  {
    // try to locate IParticle property service 
    SmartIF<LHCb::IParticlePropertySvc> ppSvc ( lokiSvc().getObject() ) ;
    const iTree& tree = m_finder ;
    Assert ( !(!ppSvc) , "LHCb::IParticleProperty is not available for '"  + 
             tree.toString() + "'") ;
    StatusCode sc = _validate ( m_finder , ppSvc ) ;
    Assert ( sc.isSuccess() , "Can't validate the decay descritor '"  + 
             tree.toString() + "'" ) ;
  }
}
// ============================================================================
// check the decay descriptor
// ============================================================================
void LoKi::PhysMCParticles::MCMatcher::getFinder ( const std::string& decay )
{
  // get GaudiAlgorithm 
  if ( 0 == m_alg ) { m_alg = _getAlg ( lokiSvc () ) ; }
  Assert ( 0 != m_alg , "GaudiAlgorithm* points to NULL" ) ;
  // get the factory:
  Decays::IMCDecay* factory = 
    m_alg->tool<Decays::IMCDecay>( "LoKi::MCDecay" , m_alg ) ;
  // get the decay finder from the factory 
  m_finder = Finder ( factory->tree ( decay ) ) ;
  //
  checkFinder ( ) ;
}
// ============================================================================
// destructor 
// ============================================================================
LoKi::PhysMCParticles::MCMatcher::~MCMatcher() {}
// ============================================================================
/*  constructor from the decay and MC-truth matching tables
 *  @param decay the decay 
 *  @param locations the location of MCtruth relation tables 
 */
// ============================================================================
LoKi::PhysMCParticles::MCMatcher::MCMatcher 
( const LoKi::PhysMCParticles::MCMatcher::iTree&      decay     , 
  const LoKi::PhysMCParticles::MCMatcher::Locations&  locations ) 
  : LoKi::PhysMCParticles::MCTruth() 
  , m_finder    ( decay      ) 
  , m_locations ( locations  )
  , m_alg       ( 0          )
{
  checkFinder () ; 
}
// ============================================================================
/*  constructor from the decay and MC-truth matching tables
 *  @param decay the decay 
 */
// ============================================================================
LoKi::PhysMCParticles::MCMatcher::MCMatcher 
( const LoKi::PhysMCParticles::MCMatcher::iTree& decay     , 
  const std::string&                             location ) 
  : LoKi::PhysMCParticles::MCTruth() 
  , m_finder    ( decay         ) 
  , m_locations ( 1 , location  )
  , m_alg       ( 0             )
{
  checkFinder () ; 
}
// ============================================================================
/*  constructor from the decay and MC-truth matching tables
 *  @param decay the decay 
 */
// ============================================================================
LoKi::PhysMCParticles::MCMatcher::MCMatcher 
( const LoKi::PhysMCParticles::MCMatcher::iTree& decay     , 
  const std::string&                             location1 ,
  const std::string&                             location2 ) 
  : LoKi::PhysMCParticles::MCTruth() 
  , m_finder    ( decay         ) 
  , m_locations ()
  , m_alg       ( 0             )
{
  m_locations.push_back ( location1 ) ;
  m_locations.push_back ( location2 ) ;  
  checkFinder () ; 
}
// ============================================================================
/*  constructor from the decay and MC-truth matching type for 
 *  protoparticles 
 */
// ============================================================================
LoKi::PhysMCParticles::MCMatcher::MCMatcher 
( const LoKi::PhysMCParticles::MCMatcher::iTree&       decay     , 
  const LoKi::PhysMCParticles::MCMatcher::ProtoPMatch  matchType ) 
  : LoKi::PhysMCParticles::MCTruth()
  , m_finder    ( decay ) 
  , m_locations (       )
  , m_alg       ( 0     )
{
  //
  switch ( matchType ) 
  {
  case Neutral     : 
    m_locations.push_back ( LHCb::ProtoParticleLocation::Neutrals ) ; break ;
  case ChargedLong : 
    m_locations.push_back ( LHCb::ProtoParticleLocation::Charged  ) ; break ;
  case Charged     : 
    m_locations.push_back ( LHCb::ProtoParticleLocation::Charged  ) ; 
    m_locations.push_back ( LHCb::ProtoParticleLocation::Upstream ) ; break ;
  default:
    m_locations.push_back ( LHCb::ProtoParticleLocation::Charged  ) ; 
    m_locations.push_back ( LHCb::ProtoParticleLocation::Upstream ) ; 
    m_locations.push_back ( LHCb::ProtoParticleLocation::Neutrals ) ;
  }
  //
  checkFinder () ;
}
// ============================================================================
/*  constructor from the decay and MC-truth matching tables
 *  @param decay the decay 
 *  @param locations the location of MCtruth relation tables 
 */
// ============================================================================
LoKi::PhysMCParticles::MCMatcher::MCMatcher 
( const std::string&                                  decay     , 
  const LoKi::PhysMCParticles::MCMatcher::Locations&  locations ) 
  : LoKi::PhysMCParticles::MCTruth() 
  , m_finder    ( s_INVALID  ) 
  , m_locations ( locations  )
  , m_alg       ( 0          )
{
  getFinder   ( decay ) ; 
}
// ============================================================================
/*  constructor from the decay and MC-truth matching tables
 *  @param decay the decay 
 */
// ============================================================================
LoKi::PhysMCParticles::MCMatcher::MCMatcher 
( const std::string&  decay     , 
  const std::string&  location  ) 
  : LoKi::PhysMCParticles::MCTruth() 
  , m_finder    ( s_INVALID  ) 
  , m_locations ( 1  , location  )
  , m_alg       ( 0          )
{
  getFinder   ( decay ) ; 
}
// ============================================================================
/*  constructor from the decay and MC-truth matching tables
 *  @param decay the decay 
 */
// ============================================================================
LoKi::PhysMCParticles::MCMatcher::MCMatcher 
( const std::string&  decay     , 
  const std::string&  location1 , 
  const std::string&  location2 )
  : LoKi::PhysMCParticles::MCTruth() 
  , m_finder    ( s_INVALID  ) 
  , m_locations ()
  , m_alg       ( 0          )
{
  m_locations.push_back ( location1 ) ;
  m_locations.push_back ( location2 ) ;  
  getFinder   ( decay ) ; 
}
// ============================================================================
/*  constructor from the decay and MC-truth matching type for 
 *  protoparticles 
 */
// ============================================================================
LoKi::PhysMCParticles::MCMatcher::MCMatcher 
( const std::string&                                   decay     , 
  const LoKi::PhysMCParticles::MCMatcher::ProtoPMatch  matchType ) 
  : LoKi::PhysMCParticles::MCTruth()
  , m_finder    ( s_INVALID ) 
  , m_locations (       )
  , m_alg       ( 0     )
{
  //
  switch ( matchType ) 
  {
  case Neutral     : 
    m_locations.push_back ( LHCb::ProtoParticleLocation::Neutrals ) ; break ;
  case ChargedLong : 
    m_locations.push_back ( LHCb::ProtoParticleLocation::Charged  ) ; break ;
  case Charged     : 
    m_locations.push_back ( LHCb::ProtoParticleLocation::Charged  ) ; 
    m_locations.push_back ( LHCb::ProtoParticleLocation::Upstream ) ; break ;
  default:
    m_locations.push_back ( LHCb::ProtoParticleLocation::Charged  ) ; 
    m_locations.push_back ( LHCb::ProtoParticleLocation::Upstream ) ; 
    m_locations.push_back ( LHCb::ProtoParticleLocation::Neutrals ) ;
  }
  //
  getFinder   ( decay ) ;
}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::PhysMCParticles::MCMatcher*
LoKi::PhysMCParticles::MCMatcher::clone() const 
{ return new LoKi::PhysMCParticles::MCMatcher ( m_finder    , m_locations ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::PhysMCParticles::MCMatcher::fillStream ( std::ostream& s ) const 
{
  const iTree& tree = m_finder ;
  s << "MCMATCH('" << tree << "'," ;
  Gaudi::Utils::toStream ( m_locations , s ) ;
  return s << ")" ;
}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::PhysMCParticles::MCMatcher::result_type 
LoKi::PhysMCParticles::MCMatcher::operator()
  ( LoKi::PhysMCParticles::MCMatcher::argument p ) const 
{
  // check argument 
  if ( 0 == p ) 
  {
    Error ( "Invalid Particle! return 'False'" ) ;
    return false ;                                         // RETURN 
  }
  // 
  if ( !match().validPointer() ) 
  {
    // get GaudiAlgorithm 
    if ( 0 == m_alg ) { m_alg = _getAlg ( lokiSvc () ) ; }
    Assert ( 0 != m_alg , "GaudiAlgorithm* points to NULL" ) ;
    const iTree& tree = m_finder ;
    LoKi::IReporter* rep = m_alg->tool<LoKi::IReporter>
      ( "LoKi::Reporter/MCMATCH(" + tree.toString() + ")" , m_alg ) ;
    setMatch ( new MCMatchObj ( tree.toString () , rep ) ) ;
  }
  // the same event ? 
  if ( !sameEvent() ) 
  {
    
    // clear the list of MC-particles
    storage().clear() ;
    
    // clear the relation tables  
    match() -> clear () ;
    
    // get GaudiAlgorithm 
    if ( 0 == m_alg ) { m_alg = _getAlg ( lokiSvc () ) ; }
    Assert ( 0 != m_alg , "GaudiAlgorithm* points to NULL" ) ;
    
    // locate the data 
    for ( Locations::const_iterator item = m_locations.begin() ; 
          m_locations.end() != item ; ++item ) 
    {
      if      ( m_alg->exist<LoKi::Types::TableP2MC> ( *item ) ) 
      { match()->addMatchInfo ( m_alg->get<LoKi::Types::TableP2MC>  ( *item ) ) ; }
      else if ( m_alg->exist<LoKi::Types::TableP2MCW> ( *item ) ) 
      { match()->addMatchInfo ( m_alg->get<LoKi::Types::TableP2MCW> ( *item ) ) ; }
      else if ( m_alg->exist<LoKi::Types::TablePP2MC> ( *item ) ) 
      { match()->addMatchInfo ( m_alg->get<LoKi::Types::TablePP2MC> ( *item ) ) ; }
      else if ( m_alg->exist<LoKi::Types::TableT2MC>  ( *item ) ) 
      { match()->addMatchInfo ( m_alg->get<LoKi::Types::TableT2MC>  ( *item ) ) ; }
      else if ( m_alg->exist<LoKi::Types::TableT2MCW> ( *item ) ) 
      { match()->addMatchInfo ( m_alg->get<LoKi::Types::TableT2MCW> ( *item ) ) ; }
      else { Error ( "No valid relation table at '" + (*item) + "'" ) ; } 
    }
    //
    if ( match() -> empty() ) 
    { Error ("Empty list of relation tables, MC-truth matching is disabled") ; }
    // get MC-particles 
    const LHCb::MCParticle::Container* mcparticles = 
      m_alg->get<LHCb::MCParticle::Container> ( LHCb::MCParticleLocation::Default ) ;
    
    // use the decay finder to fill keeper with interesing decays
    m_finder ( mcparticles -> begin ()    , 
               mcparticles -> end   ()    , 
               std::back_inserter ( storage () ) ) ;
    
  }
  /// use the base class matching 
  return match ( p ) ;
}
// ============================================================================

  




// ============================================================================
// The END 
// ============================================================================
