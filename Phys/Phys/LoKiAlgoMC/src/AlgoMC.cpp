// $Id: AlgoMC.cpp,v 1.5 2007-04-16 16:16:50 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2006/09/06 13:04:38  ibelyaev
//  v2r0 : agg LoKi::AlgoMC::gselect functions
//
// Revision 1.3  2006/08/29 15:17:16  ibelyaev
//  many minor fixes
//
// ============================================================================
// Include files 
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// LoKiMC 
// ============================================================================
#include "LoKi/MCFinder.h"
#include "LoKi/MCFinderObj.h"
#include "LoKi/MCMatch.h"
#include "LoKi/MCMatchObj.h"
// ============================================================================
// LoKiAlgoMC
// ============================================================================
#include "LoKi/AlgoMC.h"
// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::AlgoMC
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
 *  @date 2006-03-31 
 */
// ============================================================================
/** standard constructor 
 *  @param name algorithm instance name 
 *  @param pSvc pointer to Service Locator 
 */
// ============================================================================
LoKi::AlgoMC::AlgoMC
( const std::string& name , 
  ISvcLocator*       pSvc ) 
  : LoKi::Algo ( name , pSvc ) 
  ///
  , m_mcselected   ()
  , m_mcvselected  ()
  //
  , m_genselected  ()
  , m_genvselected ()
  //
  , m_mcfinders    () 
  , m_imcfinders   () 
  , m_mcmatchers   ()  
  //
  // Relation tables (TES addresses) 
  //
  // Particle      -> MC
  , m_P2MC         ()
  // Particle      -> MC
  , m_P2MCW        () 
  // ProtoParticle -> MC
  , m_PP2MC        () 
  // Track         -> MC
  , m_T2MC         ()
  // Track         -> MC
  , m_T2MCW        ()
  //
  , m_mc2collisionName ( "LoKi_MC2Collision/MC2Collision:PUBLIC" )
  , m_mc2collision     ( 0 )
  //
  , m_hepmc2mcName     ( "LoKi_HepMC2MC/HepMC2MC:PUBLIC"         )
  , m_hepmc2mc         ( 0 )
  //
  , m_pv2mcName        ( "LoKi_PV2MC/PV2MC:PUBLIC"               )
  , m_pv2mc            ( 0 )
  //
    , m_disableMCMatch   ( false ) 
{
  //
  m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Charged  ) ;
  m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Upstream ) ;
  m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Neutrals ) ;
  
  declareProperty   ( "P2MCs"  , m_P2MC   ) ;
  declareProperty   ( "WP2MCs" , m_P2MCW  ) ;  
  declareProperty   ( "PP2MCs" , m_PP2MC  ) ;
  declareProperty   ( "T2MCs"  , m_T2MC   ) ;
  declareProperty   ( "WT2MCs" , m_T2MCW  ) ;
  //
  declareProperty   ( "MC2CollisionTool" , m_mc2collisionName ) ;  
  declareProperty   ( "HepMC2MCTool"     , m_hepmc2mcName     ) ;  
  declareProperty   ( "PV2MCTool"        , m_pv2mcName        ) ;  
  //
  declareProperty   ( "DisableMCMatch"   , m_disableMCMatch   ) ;
} ;
// ============================================================================
/// virtual and protected destructor 
// ============================================================================
LoKi::AlgoMC::~AlgoMC(){} ;
// ============================================================================
/** 'Select' the MC particles to be used in local storage
 *  
 *  - The MC Particles are selected from the location
 *
 *  @code
 *
 *  MCRange kaons = mcselect( "Kaons" , 
 *                  "K+" == MCABSID  && MCP > 5 * GeV );
 *
 *  @endcode
 *
 *  - The example illustrate the 'selection'/'filtering of MC particles, 
 *    which are @c K+ or @c K- and have a
 *  momentum in excess of 5GeV/c
 *  - The selected MC particles
 *  are stored inside local LoKi storage under the tag @c "Kaons"
 *  and returned as a sequence of MC particles @c kaons .
 *
 *  @see LHCb::MCParticle 
 *  @see LoKi::Types::MCRange
 *  @see LoKi::Cuts::MCABSID 
 *  @see LoKi::Cuts::MCP
 *  @param name name/tag assigned to the selected particles
 *  @param cut  cut to be applied
 *  @return selected range of particles
 */
// ============================================================================
LoKi::Types::MCRange 
LoKi::AlgoMC::mcselect 
( const std::string&         tag      , 
  const LoKi::Types::MCCuts& cut      , 
  const std::string&         location ) 
{
  const LHCb::MCParticles* mcps = get<LHCb::MCParticles>( location );
  return mcselect ( tag , mcps , cut ) ;
};
// ============================================================================
/** 'Select' the MC vertices to be used in local storage
 *  
 *  - The MC vertices are selected from the TES location
 *
 *  @code
 *
 *  MCVRange primary = mcvselect( "MCPVs" , MCISPRIMARY );
 *
 *  @endcode
 *
 *  @see LHCb::MCVertex
 *  @see LoKi::Types::MCVRange
 *  @see LoKi::Cuts::MCISPRIMARY 
 *  @param name name/tag assigned to the selected particles
 *  @param cut  cut to be applied
 *  @param location TES location of MC-vertices
 *  @return selected range of particles
 */
// ============================================================================
LoKi::Types::MCVRange 
LoKi::AlgoMC::mcvselect 
( const std::string&          tag      , 
  const LoKi::Types::MCVCuts& cut      ,
  const std::string&          location )
{
  const LHCb::MCVertices* mcps = get<LHCb::MCVertices>( location );
  return mcvselect ( tag , mcps , cut ) ;
};
// ============================================================================
/// get LoKi::MCFinder objects
// ============================================================================
LoKi::MCFinder 
LoKi::AlgoMC::mcFinder( const std::string& name ) const 
{
  { // check exisiting tool 
    MCfinders::iterator ifind = m_mcfinders.find( name ) ;
    if ( m_mcfinders.end() != ifind ) { return LoKi::MCFinder( ifind->second ) ; }
  }
  // create new finder
  const LoKi::IReporter* rep = reporter ("MCFinder " + name  ) ;
  // get IMCDecayFinder tool 
  IMCDecayFinder* finder = 0 ;
  { // check located tool 
    IMCfinders::iterator ifind = m_imcfinders.find( name ) ;
    if ( m_imcfinders.end() == ifind )
    {
      // locate new tool
      finder = tool<IMCDecayFinder>( "MCDecayFinder" , name , this ) ;
      // add the finder into the container 
      m_imcfinders[name] = finder ;
    }
    else { finder = ifind->second ; }
  }
  
  Assert ( 0 != finder , "IMCDecayFinder* points to NULL!" );
  
  LoKi::MCFinderObj* object = 
    new LoKi::MCFinderObj ( name , finder , this , rep ) ;
  // increment the reference counter 
  object->addRef() ;
  // add the finder into the container 
  m_mcfinders[name] = object ;
  
  return LoKi::MCFinder ( object ) ;
} ;
// ============================================================================
// get LoKi::MCMatch obejct
// ============================================================================
LoKi::MCMatch 
LoKi::AlgoMC::mcTruth  ( const std::string& name ) const 
{  
  //
  LoKi::MCMatchObj* object = 0 ;
  // check exisiting tool 
  MCmatchers::iterator ifind = m_mcmatchers.find( name ) ;
  if ( m_mcmatchers.end() != ifind ) { object = ifind->second ; }
  if ( 0 == object ) 
  {
    // get the reporter 
    const LoKi::IReporter* rep = reporter( "MCMatch " + name ) ;  
    // create the new matcher 
    object = new LoKi::MCMatchObj( name , rep ) ;
    // increment the reference counter 
    object->addRef() ;
    // add the matcher into container 
    m_mcmatchers[name] = object  ;
  } ;
  //
  object->clear() ;
  /// feed the matcher with the information
  if ( !m_disableMCMatch ) 
  {
    // Particle      -> MC
    _feedIt<LoKi::Types::TableP2MC>   ( object , m_P2MC   ) ;
    // Particle      -> MC with weight 
    _feedIt<LoKi::Types::TableP2MCW>  ( object , m_P2MCW  ) ;
    // ProtoParticle -> MC with weight 
    _feedIt<LoKi::Types::TablePP2MC>  ( object , m_PP2MC  ) ;
    // Track         -> MC 
    _feedIt<LoKi::Types::TableT2MC>   ( object , m_T2MC   ) ;
    // Track         -> MC with double  weight 
    _feedIt<LoKi::Types::TableT2MCW>  ( object , m_T2MCW  ) ;
  }
  //
  return LoKi::MCMatch( object ) ;
} ;
// ============================================================================
/// clear the internal LoKi storages 
// ============================================================================
StatusCode LoKi::AlgoMC::clear() 
{
  StatusCode sc = LoKi::Algo::clear() ;
  m_mcselected   .clear () ;
  m_mcvselected  .clear () ;
  m_genselected  .clear () ;
  m_genvselected .clear () ;
  // clear the matchers  
  for ( MCmatchers::iterator imc = m_mcmatchers.begin() ; 
        m_mcmatchers.end() != imc ; ++imc )
  {
    LoKi::MCMatchObj* obj = imc->second  ;
    if ( 0 != obj ) { obj->clear() ; }
  }
  return sc ;
} ;
// ============================================================================
/// initialize the algorithm 
// ============================================================================
StatusCode LoKi::AlgoMC::initialize () 
{ 
  StatusCode sc = LoKi::Algo::initialize () ; 
  if ( sc.isFailure() ) { return sc ; }
  //
  if      ( m_disableMCMatch   )
  { Warning ( "MCMatch is explicitely DISABLED" ) ; }
  else if ( m_P2MC  .empty() && 
            m_P2MCW .empty() && 
            m_PP2MC .empty() && 
            m_T2MC  .empty() && 
            m_T2MCW .empty()   )
  { Warning ( "MCMatch is implicitely DISABLED: no input data specified" ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// make the execution of the algorithm 
// ============================================================================
StatusCode LoKi::AlgoMC::execute    () { return LoKi::Algo::execute    () ; } ;
// ============================================================================
/// perform the real analysis 
// ============================================================================
StatusCode LoKi::AlgoMC::analyse    () { return LoKi::Algo::analyse    () ; } ;
// ============================================================================
/// finalize the algorithm 
// ============================================================================
StatusCode LoKi::AlgoMC::finalize   () 
{
  // clear all the storages 
  clear() ;  
  { // finders
    for ( MCfinders::iterator entry = m_mcfinders.begin() ; 
          m_mcfinders.end() != entry ; ++entry ) 
    {
      LoKi::MCFinderObj* obj = entry->second ;
      if ( 0 != obj ) { obj->release() ; }
    } ;
    m_mcfinders  .clear() ;
    m_imcfinders .clear() ;
  }  
  { // matchers  
    for ( MCmatchers::iterator entry = m_mcmatchers.begin() ; 
          m_mcmatchers.end() != entry ; ++entry ) 
    {
      LoKi::MCMatchObj* obj = entry->second ;
      if ( 0 != obj ) { obj->release() ; }
    } ;
    m_mcmatchers .clear() ;
  }
  // finalize the base class 
  return LoKi::Algo::finalize   () ; 
} ;
// ============================================================================
/// get the pointer to IMC2Collision tool 
// ============================================================================
const IMC2Collision* LoKi::AlgoMC::mc2collision    () const 
{
  if ( 0 == m_mc2collision ) 
  { m_mc2collision = tool<IMC2Collision>( m_mc2collisionName , this ) ; }
  return m_mc2collision ;
} ;
// ============================================================================
/// get the pointer to IHepMC2MC tool 
// ============================================================================
const IHepMC2MC* LoKi::AlgoMC::hepMC2MC  () const 
{
  if ( 0 == m_hepmc2mc ) { m_hepmc2mc = tool<IHepMC2MC>( m_hepmc2mcName, this ) ; }
  return m_hepmc2mc ;
} ;
// ============================================================================
/// get the pointer to IPV2MC tool 
// ============================================================================
const IPV2MC* LoKi::AlgoMC::pv2MC        () const 
{
  if ( 0 == m_pv2mc ) { m_pv2mc = tool<IPV2MC> ( m_pv2mcName , this ) ; }
  return m_pv2mc ;
} ;
// ============================================================================


// ============================================================================
LoKi::Types::GRange 
LoKi::AlgoMC::gselect 
( const std::string&                  tag    ,
  const HepMC::GenEvent*              event  , 
  const LoKi::Types::GCuts&           cut    ) 
{
  if ( 0 == event ) 
  {
    Error ( "HepMC::GenEvent* points to null, return empty range!" ) ;
    return LoKi::Types::GRange() ;
  }
  return gselect
    ( tag , event->particles_begin() , event->particles_end() , cut ) ; 
}
// ============================================================================
LoKi::Types::GRange 
LoKi::AlgoMC::gselect 
( const std::string&                  tag    ,
  const LHCb::HepMCEvent*             event  , 
  const LoKi::Types::GCuts&           cut    ) 
{
  if ( 0 == event ) 
  {
    Error ( "LHCb::HepMCEvent* points to null, return empty range!" ) ;
    return LoKi::Types::GRange() ;
  }
  return gselect ( tag , event->pGenEvt() , cut ) ;
}
// ============================================================================
LoKi::Types::GRange 
LoKi::AlgoMC::gselect 
( const std::string&                  tag    ,
  const LHCb::HepMCEvent::Container*  events , 
  const LoKi::Types::GCuts&           cut    ) 
{
  if ( 0 == events ) 
  {
    Error ( "LHCb::HepMCEvents* points to null, return empty range!" ) ;
    return LoKi::Types::GRange() ;
  }
  // explicit loop over the container of events
  for ( LHCb::HepMCEvent::Container::const_iterator ie = 
          events->begin() ; events->end() != ie ; ++ie ) 
  {
    gselect ( tag , *ie , cut ) ;  
  }
  return gselected ( tag ) ;
}
// ============================================================================
LoKi::Types::GRange 
LoKi::AlgoMC::gselect 
( const std::string&        tag      , 
  const LoKi::Types::GCuts& cuts     ,
  const std::string&        location ) 
{
  const LHCb::HepMCEvent::Container* events = 
    get<LHCb::HepMCEvent::Container> ( location ) ;
  return gselect ( tag , events , cuts ) ; 
}
// ============================================================================
LoKi::Types::GRange 
LoKi::AlgoMC::gselect 
( const std::string&                  tag    ,
  const HepMC::GenVertex*             vertex ,
  HepMC::IteratorRange                range  ,
  const LoKi::Types::GCuts&           cut    ) 
{
  if ( 0 == vertex ) 
  {
    Error ( "HepMC::GenVertex* points to null, return empty range!" ) ;
    return LoKi::Types::GRange() ;
  }
  //
  HepMC::GenVertex* v = const_cast<HepMC::GenVertex*> ( vertex ) ;
  //
  return gselect 
    ( tag , v->particles_begin ( range ) , v->particles_end   ( range ) , cut ) ;
} ;
// ============================================================================


// ============================================================================
/// The END 
// ============================================================================

