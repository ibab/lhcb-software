// $Id: AlgoMC.cpp,v 1.2 2006-04-09 16:43:04 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-31 
 */
// ============================================================================

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
  , m_mcfinders    () 
  , m_imcfinders   () 
  , m_mcmatchers   ()  
  //
  // Relation tables (TES addresses) 
  //
  // Particle      -> MC
  , m_P2MC         ()
  // Particle      -> MC
  , m_WP2MC        () 
  // ProtoParticle -> MC
  , m_WPP2MC       () 
  // Track         -> MC
  , m_T2MC         ()
  // Track         -> MC
  , m_WDT2MC       ()
  // Track         -> MC
  , m_WIT2MC       ()
{} ;
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
  // Particle      -> MC
  _feedIt<LoKi::Types::TableP2MC>   ( object , m_P2MC   ) ;
  // Particle      -> MC with weight 
  _feedIt<LoKi::Types::TableWP2MC>  ( object , m_WP2MC  ) ;
  // ProtoParticle -> MC with weight 
  _feedIt<LoKi::Types::TableWPP2MC> ( object , m_WPP2MC ) ;
  // Track         -> MC 
  _feedIt<LoKi::Types::TableT2MC>   ( object , m_T2MC   ) ;
  // Track         -> MC with double  weight 
  _feedIt<LoKi::Types::TableWDT2MC> ( object , m_WDT2MC ) ;
  // Track         -> MC with integer weight 
  _feedIt<LoKi::Types::TableWIT2MC> ( object , m_WIT2MC ) ;
  //
  return LoKi::MCMatch( object ) ;
} ;
// ============================================================================
/// clear the internal LoKi storages 
// ============================================================================
StatusCode LoKi::AlgoMC::clear() 
{
  StatusCode sc = LoKi::Algo::clear() ;
  m_mcselected  .clear () ;
  m_mcvselected .clear () ;
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
StatusCode LoKi::AlgoMC::initialize () { return LoKi::Algo::initialize () ; }
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
      if ( 0 == obj ) { obj->release() ; }
    } ;
    m_mcmatchers .clear() ;
  }
  // finalize the base class 
  return LoKi::Algo::finalize   () ; 
} ;
// ============================================================================

// ============================================================================
/// The END 
// ============================================================================

