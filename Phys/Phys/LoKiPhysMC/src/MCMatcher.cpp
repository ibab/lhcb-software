// $Id: MCMatcher.cpp,v 1.5 2009-12-03 13:31:30 ibelyaev Exp $
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
#include "LoKi/select.h"
// ============================================================================
// LoKiMC 
// ============================================================================
#include "LoKi/IMCDecay.h"
#include "LoKi/MCParticles.h"
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
  ( const LoKi::PhysMCParticles::MCTreeMatch::iTree& tree , 
    const LHCb::IParticlePropertySvc*                svc  ) 
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
  const Decays::Trees::Types_<const LHCb::MCParticle*>::Invalid s_INVALID     ;
  // ==========================================================================
  inline std::string rels0 ( const std::string& loc  ) 
  {
    if ( std::string::npos != loc.find ( "Relations" ) ) { return loc ; }
    if ( 0                 == loc.find ('/')           ) { return loc ; }
    return "Relations/" + loc ;
  }  
  // ==========================================================================
} //                                                 end of anonymous namespace
// ============================================================================
/*  constructor from the decay and MC-truth matching tables
 *  @param locations the location of MCtruth relation tables 
 */
// ============================================================================
LoKi::PhysMCParticles::MCMatcherBase::MCMatcherBase
( const LoKi::PhysMCParticles::MCMatcherBase::Locations&  locations ) 
  : LoKi::PhysMCParticles::MCTruth() 
  , m_locations ( locations  )
  , m_alg       ( 0          )
{}
// ============================================================================
//  constructor from the decay and MC-truth matching tables
// ============================================================================
LoKi::PhysMCParticles::MCMatcherBase::MCMatcherBase
( const std::string&  location ) 
  : LoKi::PhysMCParticles::MCTruth() 
  , m_locations ( 1 , location )
  , m_alg       ( 0            )
{}
// ============================================================================
//  constructor from the decay and MC-truth matching tables
// ============================================================================
LoKi::PhysMCParticles::MCMatcherBase::MCMatcherBase
( const std::string&  location1 ,
  const std::string&  location2 ) 
  : LoKi::PhysMCParticles::MCTruth() 
  , m_locations ()
  , m_alg       ( 0            )
{
  m_locations.push_back ( location1 ) ;
  m_locations.push_back ( location2 ) ;  
}
// ============================================================================
//  constructor from the decay and MC-truth matching tables
// ============================================================================
LoKi::PhysMCParticles::MCMatcherBase::MCMatcherBase
( const std::string&  location1 ,
  const std::string&  location2 , 
  const std::string&  location3 ) 
  : LoKi::PhysMCParticles::MCTruth() 
  , m_locations ()
  , m_alg       ( 0 )
{
  m_locations.push_back ( location1 ) ;
  m_locations.push_back ( location2 ) ;  
  m_locations.push_back ( location3 ) ;  
}
// ============================================================================
/*  constructor from the decay and MC-truth matching tables
 *  @param locations the location of MCtruth relation tables 
 */
// ============================================================================
LoKi::PhysMCParticles::MCMatcherBase::MCMatcherBase
( const LoKi::PhysMCParticles::ProtoPMatch& protoMatch ) 
  : LoKi::PhysMCParticles::MCTruth() 
  , m_locations ()
  , m_alg       ( 0 )
{
  switch ( protoMatch ) 
  {
  case Neutral     : 
    m_locations.push_back ( rels0 ( LHCb::ProtoParticleLocation::Neutrals ) ) ; break ;
  case ChargedLong : 
    m_locations.push_back ( rels0 ( LHCb::ProtoParticleLocation::Charged  ) ) ; break ;
  case Charged     : 
    m_locations.push_back ( rels0 ( LHCb::ProtoParticleLocation::Charged  ) ) ; 
    m_locations.push_back ( rels0 ( LHCb::ProtoParticleLocation::Upstream ) ) ; break ;
  default:
    m_locations.push_back ( rels0 ( LHCb::ProtoParticleLocation::Charged  ) ) ; 
    m_locations.push_back ( rels0 ( LHCb::ProtoParticleLocation::Upstream ) ) ; 
    m_locations.push_back ( rels0 ( LHCb::ProtoParticleLocation::Neutrals ) ) ;
  }
}
// ============================================================================
// destructor
// ============================================================================
LoKi::PhysMCParticles::MCMatcherBase::~MCMatcherBase()
{
  clearAll() ; 
  //
  LoKi::MCMatchObj* _m = match() ;
  if ( 0 != _m && 1 > _m->refCount () ) { _m->release() ; }
}
// ============================================================================
// clear all storages 
// ============================================================================
void LoKi::PhysMCParticles::MCMatcherBase::clearAll () const 
{ 
  // clear the list of MC-particles
  storage().clear  () ;  
  // clear the relation tables  
  if ( !(!match() ) ) { match() -> clear () ; }
}
// ============================================================================
// load the data 
// ============================================================================
StatusCode LoKi::PhysMCParticles::MCMatcherBase::load() const  // load the data 
{
  
  if ( !match() ) 
  {
    // get GaudiAlgorithm 
    if ( 0 == alg () ) { setAlg ( _getAlg ( lokiSvc () ) ) ; }
    Assert ( 0 != alg () , "GaudiAlgorithm* points to NULL" ) ;
    LoKi::IReporter* rep = alg() ->tool<LoKi::IReporter>
      ( "LoKi::Reporter/" + this -> printOut () , alg () ) ;
    LoKi::MCMatchObj* _m = new MCMatchObj ( this -> printOut () , rep )  ;
    setMatch ( _m ) ;
  }
  
  /// clear all storages 
  clearAll () ;
  
  // get GaudiAlgorithm 
  if ( 0 == alg () ) { setAlg ( _getAlg ( lokiSvc () ) ) ; }
  Assert ( 0 != alg () , "GaudiAlgorithm* points to NULL" ) ;
  
  // locate the data 
  for ( Locations::const_iterator item = m_locations.begin() ; 
        m_locations.end() != item ; ++item ) 
  {
    if      ( alg()->exist<LoKi::Types::TableP2MC> ( *item ) ) 
    { match() -> addMatchInfo ( alg()->get<LoKi::Types::TableP2MC>  ( *item ) ) ; continue ; }
    else if ( alg()->exist<LoKi::Types::TableP2MCW> ( *item ) ) 
    { match() -> addMatchInfo ( alg()->get<LoKi::Types::TableP2MCW> ( *item ) ) ; continue ; }
    else if ( alg()->exist<LoKi::Types::TablePP2MC> ( *item ) ) 
    { match() -> addMatchInfo ( alg()->get<LoKi::Types::TablePP2MC> ( *item ) ) ; continue ; }
    else if ( alg()->exist<LoKi::Types::TableT2MC>  ( *item ) ) 
    { match() -> addMatchInfo ( alg()->get<LoKi::Types::TableT2MC>  ( *item ) ) ; continue ; }
    else if ( alg()->exist<LoKi::Types::TableT2MCW> ( *item ) ) 
    { match() -> addMatchInfo ( alg()->get<LoKi::Types::TableT2MCW> ( *item ) ) ; continue ; }
    else 
    {
      const std::string loc = rels0 ( *item ) ;
      if ( *item == loc ) 
      { Error ( "No valid relation table at '" + (*item) + "'" )                ; continue ; }
      else if ( alg()->exist<LoKi::Types::TableP2MC>  ( loc ) ) 
      { match() -> addMatchInfo ( alg()->get<LoKi::Types::TableP2MC>  ( loc ) ) ; continue ; }
      else if ( alg()->exist<LoKi::Types::TableP2MCW> ( loc ) ) 
      { match() -> addMatchInfo ( alg()->get<LoKi::Types::TableP2MCW> ( loc ) ) ; continue ; }
      else if ( alg()->exist<LoKi::Types::TablePP2MC> ( loc ) ) 
      { match() -> addMatchInfo ( alg()->get<LoKi::Types::TablePP2MC> ( loc ) ) ; continue ; }
      else if ( alg()->exist<LoKi::Types::TableT2MC>  ( loc ) ) 
      { match() -> addMatchInfo ( alg()->get<LoKi::Types::TableT2MC>  ( loc ) ) ; continue ; }
      else if ( alg()->exist<LoKi::Types::TableT2MCW> ( loc ) ) 
      { match() -> addMatchInfo ( alg()->get<LoKi::Types::TableT2MCW> ( loc ) ) ; continue ; }
      //
      Error ( "No valid relation table at '" + (*item) + "'" ) ; 
    }
    
  }
  //
  if ( match() -> empty() ) 
  { Error ("Empty list of relation tables, MC-truth matching is disabled") ; }
  // get MC-particles 
  const LHCb::MCParticle::Container* mcparticles = 
    m_alg->get<LHCb::MCParticle::Container> ( LHCb::MCParticleLocation::Default ) ;
  
  /// load Monte Carlo particles 
  StatusCode sc = getMCParticles ( *mcparticles ) ;
  if ( sc.isSuccess() ) { setEvent() ; }
  
  if ( empty() ) 
  { return Warning( "load(): No MC-Particles are selected" , 
                    StatusCode::SUCCESS ) ; }
  
  return StatusCode::SUCCESS ; 
}
// ======================================================================


// ======================================================================
/*  constructor from MC-selector and TES-locations 
 *  @param selector Monte Carlo selector
 *  @param locations TES-locations of relation tables 
 */
// ======================================================================
LoKi::PhysMCParticles::MCSelMatch::MCSelMatch
( const LoKi::PhysMCParticles::MCSelMatch::MCCuts&       selector  , 
  const LoKi::PhysMCParticles::MCMatcherBase::Locations& locations ) 
  : LoKi::PhysMCParticles::MCMatcherBase ( locations ) 
  , m_cuts ( selector ) 
{}
// ======================================================================
/*  constructor from MC-selector and TES-locations 
 *  @param selector Monte Carlo selector
 */
// ======================================================================
LoKi::PhysMCParticles::MCSelMatch::MCSelMatch
( const LoKi::PhysMCParticles::MCSelMatch::MCCuts& selector , 
  const std::string& location ) 
  : LoKi::PhysMCParticles::MCMatcherBase ( location ) 
  , m_cuts ( selector ) 
{}
// ======================================================================
/*  constructor from MC-selector and TES-locations 
 *  @param selector Monte Carlo selector
 */
// ======================================================================
LoKi::PhysMCParticles::MCSelMatch::MCSelMatch
( const LoKi::PhysMCParticles::MCSelMatch::MCCuts&       selector , 
  const std::string& location1 , 
  const std::string& location2 ) 
  : LoKi::PhysMCParticles::MCMatcherBase ( location1  , 
                                           location2  ) 
  , m_cuts ( selector ) 
{}
// ======================================================================
/*  constructor from MC-selector and TES-locations 
 *  @param selector Monte Carlo selector
 */
// ======================================================================
LoKi::PhysMCParticles::MCSelMatch::MCSelMatch
( const LoKi::PhysMCParticles::MCSelMatch::MCCuts&       selector , 
  const std::string& location1 , 
  const std::string& location2 ,
  const std::string& location3 ) 
  : LoKi::PhysMCParticles::MCMatcherBase ( location1  , 
                                           location2  ,
                                           location3  ) 
  , m_cuts ( selector ) 
{}
// ======================================================================
/*  constructor from MC-selector and TES-locations 
 *  @param selector Monte Carlo selector
 */
// ======================================================================
LoKi::PhysMCParticles::MCSelMatch::MCSelMatch
( const LoKi::PhysMCParticles::MCSelMatch::MCCuts&         selector , 
  const LoKi::PhysMCParticles::ProtoPMatch& protoMatch )
  : LoKi::PhysMCParticles::MCMatcherBase ( protoMatch )  
  , m_cuts ( selector ) 
{}
// ============================================================================
// destructor 
// ============================================================================
LoKi::PhysMCParticles::MCSelMatch::~MCSelMatch() {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::PhysMCParticles::MCSelMatch*
LoKi::PhysMCParticles::MCSelMatch::clone() const
{ return new LoKi::PhysMCParticles::MCSelMatch ( m_cuts , locations() ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::PhysMCParticles::MCSelMatch::result_type 
LoKi::PhysMCParticles::MCSelMatch::operator()
  ( LoKi::PhysMCParticles::MCSelMatch::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ("LHCb::Particle* point to NULL, return false") ;
    return false ;
  }
  // the same event ? 
  if ( !sameEvent() ) { load () ; }
  // 
  return match  ( p ) ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream&
LoKi::PhysMCParticles::MCSelMatch::fillStream( std::ostream& s ) const 
{
  s << "MCSELMATCH(" << m_cuts << "," ;
  switch ( locations().size() ) 
  {
  case 1 : 
    return s << "'"   << locations()[0] << "')" ;
  case 2 : 
    return s << "'"   << locations()[0] 
             << "','" << locations()[1] << "')" ;
  case 3 : 
    return s << "'"   << locations()[0] 
             << "','" << locations()[1] 
             << "','" << locations()[2] << "')" ;
  default:
    break ;
  }
  return Gaudi::Utils::toStream ( locations() , s ) << ")" ;
}
// ============================================================================
// get MC-particles 
// ============================================================================
StatusCode LoKi::PhysMCParticles::MCSelMatch::getMCParticles 
( const LHCb::MCParticle::Container& cnt ) const 
{
  // clear MC-particles ;
  storage().clear() ;
  
  if ( cnt.empty() ) 
  { return Warning ("Empty input container of MC-particles") ; }
  
  LoKi::select ( cnt.begin () , 
                 cnt.end   () , 
                 std::back_inserter ( storage() ) , 
                 m_cuts       ) ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================


// ============================================================================
LoKi::PhysMCParticles::MCTreeMatch::MCTreeMatch
( const LoKi::PhysMCParticles::MCTreeMatch::iTree&       decay     , 
  const LoKi::PhysMCParticles::MCMatcherBase::Locations& locations ) 
  : LoKi::PhysMCParticles::MCMatcherBase ( locations ) 
  , m_finder ( decay ) 
{
  checkFinder () ;
}
// ============================================================================
LoKi::PhysMCParticles::MCTreeMatch::MCTreeMatch
( const LoKi::PhysMCParticles::MCTreeMatch::iTree& decay , 
  const std::string& location ) 
  : LoKi::PhysMCParticles::MCMatcherBase ( location ) 
  , m_finder ( decay ) 
{
  checkFinder () ;
}
// ============================================================================
LoKi::PhysMCParticles::MCTreeMatch::MCTreeMatch
( const LoKi::PhysMCParticles::MCTreeMatch::iTree& decay , 
  const std::string& location1 , 
  const std::string& location2 )
  : LoKi::PhysMCParticles::MCMatcherBase ( location1 , location2 ) 
  , m_finder ( decay ) 
{
  checkFinder () ;
}
// ============================================================================
LoKi::PhysMCParticles::MCTreeMatch::MCTreeMatch
( const LoKi::PhysMCParticles::MCTreeMatch::iTree& decay , 
  const std::string& location1 , 
  const std::string& location2 ,
  const std::string& location3 )
  : LoKi::PhysMCParticles::MCMatcherBase ( location1 , 
                                           location2 ,
                                           location3 ) 
  , m_finder ( decay ) 
{
  checkFinder () ;
}
// ============================================================================
LoKi::PhysMCParticles::MCTreeMatch::MCTreeMatch
( const LoKi::PhysMCParticles::MCTreeMatch::iTree&         decay      , 
  const LoKi::PhysMCParticles::ProtoPMatch& protoMatch )
  : LoKi::PhysMCParticles::MCMatcherBase ( protoMatch )  
  , m_finder ( decay ) 
{
  checkFinder () ;
}
// ============================================================================
LoKi::PhysMCParticles::MCTreeMatch::MCTreeMatch
( const std::string&                                     decay     , 
  const LoKi::PhysMCParticles::MCMatcherBase::Locations& locations ) 
  : LoKi::PhysMCParticles::MCMatcherBase ( locations ) 
  , m_finder ( s_INVALID ) 
{
  getFinder ( decay ) ;
}
// ============================================================================
LoKi::PhysMCParticles::MCTreeMatch::MCTreeMatch
( const std::string& decay    , 
  const std::string& location ) 
  : LoKi::PhysMCParticles::MCMatcherBase ( location ) 
  , m_finder ( s_INVALID ) 
{
  getFinder ( decay ) ;
}
// ============================================================================
LoKi::PhysMCParticles::MCTreeMatch::MCTreeMatch
( const std::string& decay     , 
  const std::string& location1 , 
  const std::string& location2 )
  : LoKi::PhysMCParticles::MCMatcherBase ( location1 , location2 ) 
  , m_finder ( s_INVALID ) 
{
  getFinder ( decay ) ;
}
// ============================================================================
LoKi::PhysMCParticles::MCTreeMatch::MCTreeMatch
( const std::string& decay     , 
  const std::string& location1 , 
  const std::string& location2 ,
  const std::string& location3 )
  : LoKi::PhysMCParticles::MCMatcherBase ( location1 , 
                                           location2 ,
                                           location3 ) 
  , m_finder ( s_INVALID ) 
{
  getFinder ( decay ) ;
}
// ============================================================================
LoKi::PhysMCParticles::MCTreeMatch::MCTreeMatch
( const std::string&                                       decay      , 
  const LoKi::PhysMCParticles::ProtoPMatch& protoMatch )
  : LoKi::PhysMCParticles::MCMatcherBase ( protoMatch )  
  , m_finder ( s_INVALID ) 
{
  getFinder ( decay ) ;
}
// ============================================================================
// destructor 
// ============================================================================
LoKi::PhysMCParticles::MCTreeMatch::~MCTreeMatch() {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::PhysMCParticles::MCTreeMatch*
LoKi::PhysMCParticles::MCTreeMatch::clone() const 
{ return new LoKi::PhysMCParticles::MCTreeMatch ( m_finder , locations() ) ; }
// ============================================================================
void LoKi::PhysMCParticles::MCTreeMatch::checkFinder ()
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
void LoKi::PhysMCParticles::MCTreeMatch::getFinder ( const std::string& decay )
{
  // get GaudiAlgorithm 
  if ( 0 == alg () ) { setAlg ( _getAlg ( lokiSvc () ) ) ; }
  //
  Assert ( 0 != alg() , "Context: GaudiAlgorithm* points to NULL" ) ;
  // get the factory:
  Decays::IMCDecay* factory = 
    alg()->tool<Decays::IMCDecay>( "LoKi::MCDecay" , alg () ) ;
  // get the decay finder from the factory 
  m_finder = Finder ( factory->tree ( decay ) ) ;
  //
  checkFinder ( ) ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::PhysMCParticles::MCTreeMatch::result_type 
LoKi::PhysMCParticles::MCTreeMatch::operator()
  ( LoKi::PhysMCParticles::MCTreeMatch::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ("LHCb::Particle* point to NULL, return false") ;
    return false ;
  }
  // the same event ? 
  if ( !sameEvent() ) { load () ; }
  // 
  return match  ( p ) ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream&
LoKi::PhysMCParticles::MCTreeMatch::fillStream( std::ostream& s ) const 
{
  s << "MCMATCH('" << m_finder << "'," ;
  switch ( locations().size() ) 
  {
  case 1 : 
    return s << "'"   << locations()[0] << "')" ;
  case 2 : 
    return s << "'"   << locations()[0] 
             << "','" << locations()[1] << "')" ;
  case 3 : 
    return s << "'"   << locations()[0] 
             << "','" << locations()[1] 
             << "','" << locations()[2] << "')" ;
  default:
    break ;
  }
  return Gaudi::Utils::toStream ( locations() , s ) << ")" ;
}
// ============================================================================
// get MC-particles 
// ============================================================================
StatusCode LoKi::PhysMCParticles::MCTreeMatch::getMCParticles 
( const LHCb::MCParticle::Container& cnt ) const 
{
  // clear MC-particles ;
  storage().clear() ;
  
  if ( cnt.empty() ) 
  { return Warning ("Empty input container of MC-particles") ; }
  
  m_finder ( cnt.begin () , 
             cnt.end   () , 
             std::back_inserter ( storage() ) ) ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================






// ============================================================================
LoKi::PhysMCParticles::MCNodeMatch::MCNodeMatch
( const LoKi::PhysMCParticles::MCNodeMatch::iNode&       node      , 
  const LoKi::PhysMCParticles::MCMatcherBase::Locations& locations ) 
  : LoKi::PhysMCParticles::MCMatcherBase ( locations ) 
  , m_node ( node ) 
{
  checkNode () ;
}
// ============================================================================
LoKi::PhysMCParticles::MCNodeMatch::MCNodeMatch
( const LoKi::PhysMCParticles::MCNodeMatch::iNode&       node , 
  const std::string& location ) 
  : LoKi::PhysMCParticles::MCMatcherBase ( location ) 
  , m_node ( node ) 
{
  checkNode () ;
}
// ============================================================================
LoKi::PhysMCParticles::MCNodeMatch::MCNodeMatch
( const LoKi::PhysMCParticles::MCNodeMatch::iNode&       node      , 
  const std::string& location1 , 
  const std::string& location2 )
  : LoKi::PhysMCParticles::MCMatcherBase ( location1 , location2 ) 
  , m_node ( node ) 
{
  checkNode () ;
}
// ============================================================================
LoKi::PhysMCParticles::MCNodeMatch::MCNodeMatch
( const LoKi::PhysMCParticles::MCNodeMatch::iNode&       node      , 
  const std::string& location1 , 
  const std::string& location2 ,
  const std::string& location3 )
  : LoKi::PhysMCParticles::MCMatcherBase ( location1 , 
                                           location2 ,
                                           location3 ) 
  , m_node ( node ) 
{
  checkNode () ;
}
// ============================================================================
LoKi::PhysMCParticles::MCNodeMatch::MCNodeMatch
( const LoKi::PhysMCParticles::MCNodeMatch::iNode&       node      , 
  const LoKi::PhysMCParticles::ProtoPMatch& protoMatch )
  : LoKi::PhysMCParticles::MCMatcherBase ( protoMatch )  
  , m_node ( node ) 
{
  checkNode () ;
}
// ============================================================================
// destructor 
// ============================================================================
LoKi::PhysMCParticles::MCNodeMatch::~MCNodeMatch() {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::PhysMCParticles::MCNodeMatch*
LoKi::PhysMCParticles::MCNodeMatch::clone() const
{ return new LoKi::PhysMCParticles::MCNodeMatch ( m_node , locations() ) ; }
// ============================================================================
void LoKi::PhysMCParticles::MCNodeMatch::checkNode ()
{
  if ( !m_node ) 
  {
    // try to locate IParticle property service 
    SmartIF<LHCb::IParticlePropertySvc> ppSvc ( lokiSvc().getObject() ) ;
    Assert ( !(!ppSvc) , "LHCb::IParticleProperty is not available for '"  + 
             m_node.toString() + "'") ;
    StatusCode sc = m_node.validate ( ppSvc ) ;
    Assert ( sc.isSuccess() , "Can't validate the node descritor '"  + 
             m_node.toString() + "'" ) ;
  }
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::PhysMCParticles::MCNodeMatch::result_type 
LoKi::PhysMCParticles::MCNodeMatch::operator()
  ( LoKi::PhysMCParticles::MCNodeMatch::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ("LHCb::Particle* point to NULL, return false") ;
    return false ;
  }
  // the same event ? 
  if ( !sameEvent() ) { load () ; }
  // 
  return match  ( p ) ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream&
LoKi::PhysMCParticles::MCNodeMatch::fillStream( std::ostream& s ) const 
{
  s << "MCNODEMATCH('" << m_node << "'," ;
  switch ( locations().size() ) 
  {
  case 1 : 
    return s << "'"   << locations()[0] << "')" ;
  case 2 : 
    return s << "'"   << locations()[0] 
             << "','" << locations()[1] << "')" ;
  case 3 : 
    return s << "'"   << locations()[0] 
             << "','" << locations()[1] 
             << "','" << locations()[2] << "')" ;
  default:
    break ;
  }
  return Gaudi::Utils::toStream ( locations() , s ) << ")" ;
}
// ============================================================================
// get MC-particles 
// ============================================================================
StatusCode LoKi::PhysMCParticles::MCNodeMatch::getMCParticles 
( const LHCb::MCParticle::Container& cnt ) const 
{
  // clear MC-particles ;
  storage().clear() ;
  
  if ( cnt.empty() ) 
  { return Warning ("Empty input container of MC-particles") ; }
  
  LoKi::select ( cnt.begin () , 
                 cnt.end   () , 
                 std::back_inserter ( storage() )      , 
                 LoKi::MCParticles::DecNode ( m_node ) ) ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
